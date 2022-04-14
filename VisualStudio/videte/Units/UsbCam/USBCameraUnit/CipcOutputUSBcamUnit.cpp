/////////////////////////////////////////////////////////////////////////////
// PROJECT:		USBCameraUnit
// FILE:		$Workfile: CipcOutputUSBcamUnit.cpp $
// ARCHIVE:		$Archive: /Project/Units/USBCAM/USBCameraUnit/CipcOutputUSBcamUnit.cpp $
// CHECKIN:		$Date: 31.10.05 14:12 $
// VERSION:		$Revision: 12 $
// LAST CHANGE:	$Modtime: 12.09.05 15:18 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "USBCameraUnitDlg.h"
#include "CIPCOutputUSBcamUnit.h"
#include "CIPCExtraMemory.h"
#include "CIPCStringDefs.h"
#include "VideoJob.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG
	#define DBG_TRACE(S) WK_TRACE(_T("%s\n%s(%d):\n"), S, _T(__FILE__), __LINE__);
#else
	#define DBG_TRACE // 
#endif

// TODO! RKE: define new CameraControlType
#ifndef CCT_DIRECT_SHOW
	#define CCT_DIRECT_SHOW 20
#endif

CIPCOutputUSBcamUnit::CIPCOutputUSBcamUnit(CUSBCameraUnitDlg* pMainWnd, 
							 LPCTSTR shmName,
							 BOOL basMaster)
	: CIPCOutput(shmName, basMaster)
{
	m_wGroupID			= SECID_NO_GROUPID;				// Noch keine GroupID
	m_res				   = RESOLUTION_HIGH;				// Resolution
	m_comp				= COMPRESSION_NONE;				// Compression
//	m_wFormat			= ENCODER_RES_UNKNOWN;
	m_pMainWnd			= pMainWnd;
	m_lpSmallTestbild	= NULL;							// Pointer auf kleines Störungsbild
	m_dwSmallTestbildLen= 0;							// Länge des kleinen Störungsbildes
	m_lpLargeTestbild	= NULL;							// Pointer auf großes Störungsbild
	m_dwLargeTestbildLen= 0;							// Länge des großen Störungsbildes
	m_bReset		    = FALSE;

	// Kleines Testbild aus der Resource laden
	HRSRC hRc = FindResource(NULL, MAKEINTRESOURCE(BIN_SMALL_TESTBILD), _T("BINRES"));
	if (hRc)
	{
		HGLOBAL hJpegResource	= LoadResource(NULL, hRc);
		if (hJpegResource)
		{
			m_dwSmallTestbildLen	= SizeofResource(NULL, hRc);
			m_lpSmallTestbild		= (BYTE*)LockResource(hJpegResource);
		}
		else
			WK_TRACE_ERROR(_T("Kleines Testbild konnte nicht geladen werden\n"));
	}
	else
		WK_TRACE_ERROR(_T("Testbild konnte nicht geladen werden\n"));


	// Großes Testbild aus der Resource laden
	hRc = FindResource(NULL, MAKEINTRESOURCE(BIN_LARGE_TESTBILD), _T("BINRES"));
	if (hRc)
	{
		HGLOBAL hJpegResource	= LoadResource(NULL, hRc);
		if (hJpegResource)
		{
			m_dwLargeTestbildLen	= SizeofResource(NULL, hRc);
			m_lpLargeTestbild		= (BYTE*)LockResource(hJpegResource);
		}
		else
			WK_TRACE_ERROR(_T("Großes Testbild konnte nicht geladen werden\n"));
	}
	else
		WK_TRACE_ERROR(_T("Testbild konnte nicht geladen werden\n"));

	SetConnectRetryTime(25);
	StartThread();
}

//////////////////////////////////////////////////////////////////////////////////////
CIPCOutputUSBcamUnit::~CIPCOutputUSBcamUnit()
{
	// TRACE(_T("CIPCOutputUSBcamUnit::~CIPCOutputUSBcamUnit\n"));
	StopThread();
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::OnRequestGetValue(CSecID CId, const CString &sKey, DWORD dwUserData)
{
	CString sValue = _T("");
//	DBG_TRACE(_T("OnRequestGetValue"));
	if (CId.GetGroupID() != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCOutputUSBcamUnit::OnRequestGetValue\tWrong GroupID\n"));
		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	WORD wSource = CId.GetSubID(); // Kameranummer 0...15
	
	if (m_pMainWnd)
	{
		BOOL  bFound = FALSE;
		VideoProcAmpProperty CamProperty = (VideoProcAmpProperty)0;

		if (sKey == CSD_BRIGHTNESS)
		{
			CamProperty = VideoProcAmp_Brightness;
			bFound = TRUE;
		}
		else if (sKey == CSD_CONTRAST)
		{
			CamProperty = VideoProcAmp_Contrast;
			bFound = TRUE;
		}
		else if (sKey == CSD_SATURATION)
		{
			CamProperty = VideoProcAmp_Saturation;
			bFound = TRUE;
		}
		else if (sKey == CSD_HUE_PERCENT)
		{
			CamProperty = VideoProcAmp_Hue;
			bFound = TRUE;
		}
		else if (sKey == CSD_SHARPNESS_PERCENT)
		{
			CamProperty = VideoProcAmp_Sharpness;
			bFound = TRUE;
		}
		else if (sKey == CSD_GAMMA_PERCENT)
		{
			CamProperty = VideoProcAmp_Gamma;
			bFound = TRUE;
		}
		else if (sKey == CSD_VIDEOGAIN_PERCENT)
		{
			CamProperty = VideoProcAmp_Gain;
			bFound = TRUE;
		}
		else if (sKey == CSD_COLORENABLE)
		{
			CamProperty = VideoProcAmp_ColorEnable;
			bFound = TRUE;
		}
		else if (sKey == CSD_WHITEBALANCE_PERCENT)
		{
			CamProperty = VideoProcAmp_WhiteBalance;
			bFound = TRUE;
		}
		else if (sKey == CSD_BACKLIGHTCOMPENSATION_PERCENT)
		{
			CamProperty = VideoProcAmp_BacklightCompensation;
			bFound = TRUE;
		}

		if (bFound)
		{
			m_pMainWnd->PostMessage(WM_REQUEST_CAMERA_CONTROL,
				(WPARAM) new CameraControlStruct(CId, sKey, sValue, dwUserData, false, false, CamProperty),
				(LPARAM)this);
			return;
		}
		
		CameraControlProperty CamCtrlProp = (CameraControlProperty)0;

		if (sKey == CSD_PAN_PERCENT)
		{
			CamCtrlProp = CameraControl_Pan;
			bFound = TRUE;
		}
		else if (sKey == CSD_TILT_PERCENT)
		{
			CamCtrlProp = CameraControl_Tilt;
			bFound = TRUE;
		}
		else if (sKey == CSD_ROLL_PERCENT)
		{
			CamCtrlProp = CameraControl_Roll;
			bFound = TRUE;
		}
		else if (sKey == CSD_ZOOM_PERCENT)
		{
			CamCtrlProp = CameraControl_Zoom;
			bFound = TRUE;
		}
		else if (sKey == CSD_EXPOSURE_PERCENT)
		{
			CamCtrlProp = CameraControl_Exposure;
			bFound = TRUE;
		}
		else if (sKey == CSD_IRIS_PERCENT)
		{
			CamCtrlProp = CameraControl_Iris;
			bFound = TRUE;
		}
		else if (sKey == CSD_FOCUS_PERCENT)
		{
			CamCtrlProp = CameraControl_Focus;
			bFound = TRUE;
		}

		if (bFound)
		{
			m_pMainWnd->PostMessage(WM_REQUEST_CAMERA_CONTROL,
				(WPARAM) new CameraControlStruct(CId, sKey, sValue, dwUserData, false, false, CamCtrlProp),
				(LPARAM)this);

			return;
		}

		if (sKey == CSD_SCAN_FOR_CAMERAS)
		{
			DBG_TRACE(_T("ScanForCameras\n"));
			sValue.Format(_T("%lu"), m_pMainWnd->ScanForCameras(dwUserData));
			return;
		}
		else if (0==sKey.Find(CSD_CAM_CONTROL_TYPE))
		{
			HRESULT hr = m_pMainWnd->CameraControlPTZ(wSource, CCC_INVALID, NULL);
			if (hr == E_POINTER)
			{
				DoIndicateError(0, CId, CIPC_ERROR_CAMERA_NOT_PRESENT, 0);	
			}
			else if (SUCCEEDED(hr))
			{
				// TODO! RKE: define new CameraControlType
				DoConfirmGetValue(CId, sKey, NameOfEnum((CameraControlType)CCT_DIRECT_SHOW), dwUserData);
			}
			return;
		}
		else if (0==sKey.Find(CSD_CAM_CONTROL_FKT))
		{
			m_pMainWnd->PostMessage(WM_REQUEST_CAMERA_CONTROL,
				(WPARAM) new CameraControlStruct(CId, sKey, sValue, dwUserData, false, CCC_INVALID),
				(LPARAM)this);

			return;
		}
		else if (sKey == CSD_MD_MASK_SENSITIVITY)
		{
			if (m_pMainWnd->GetMD())
			{
				sValue = m_pMainWnd->GetMD()->GetMDMaskSensitivity(wSource);
				bFound = TRUE;
			}
		}
		else if (sKey == CSD_MD_ALARM_SENSITIVITY)
		{
			if (m_pMainWnd->GetMD())
			{
				sValue = m_pMainWnd->GetMD()->GetMDAlarmSensitivity(wSource);
				bFound = TRUE;
			}
		}
		if (bFound)
		{
			DoConfirmGetValue(CId, sKey, sValue, dwUserData);
			return;
		}

		WK_TRACE_ERROR(_T("No Main Window\n"));
		DoIndicateError(0, CId, CIPC_ERROR_UNHANDLED_CMD, 0);	
	}
	else
	{
		WK_TRACE_ERROR(_T("No Main Window\n"));
		DoIndicateError(0, CId, CIPC_ERROR_CAMERA_NOT_PRESENT, 0);	
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::OnRequestSetValue(CSecID CId, const CString &sKey, const CString &sValue, DWORD dwUserData)
{
//	DBG_TRACE(_T("OnRequestSetValue"));
	if (CId.GetGroupID() != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCOutputUSBcamUnit::OnRequestSetValue\tWrong GroupID\n"));
		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	WORD wSource = CId.GetSubID(); // Kameranummer 0...15
	if (m_pMainWnd)
	{
		BOOL  bFound = FALSE;
		DWORD dwValue;
		VideoProcAmpProperty CamProperty = (VideoProcAmpProperty)0;

		if (sKey == CSD_BRIGHTNESS)
		{
			CamProperty = VideoProcAmp_Brightness;
			bFound = TRUE;
		}
		else if (sKey == CSD_CONTRAST)
		{
			CamProperty = VideoProcAmp_Contrast;
			bFound = TRUE;
		}
		else if (sKey == CSD_SATURATION)
		{
			CamProperty = VideoProcAmp_Saturation;
			bFound = TRUE;
		}
		else if (sKey == CSD_HUE_PERCENT)
		{
			CamProperty = VideoProcAmp_Hue;
			bFound = TRUE;
		}
		else if (sKey == CSD_SHARPNESS_PERCENT)
		{
			CamProperty = VideoProcAmp_Sharpness;
			bFound = TRUE;
		}
		else if (sKey == CSD_GAMMA_PERCENT)
		{
			CamProperty = VideoProcAmp_Gamma;
			bFound = TRUE;
		}
		else if (sKey == CSD_VIDEOGAIN_PERCENT)
		{
			CamProperty = VideoProcAmp_Gain;
			bFound = TRUE;
		}
		else if (sKey == CSD_COLORENABLE)
		{
			CamProperty = VideoProcAmp_ColorEnable;
			bFound = TRUE;
		}
		else if (sKey == CSD_WHITEBALANCE_PERCENT)
		{
			CamProperty = VideoProcAmp_WhiteBalance;
			bFound = TRUE;
		}
		else if (sKey == CSD_BACKLIGHTCOMPENSATION_PERCENT)
		{
			CamProperty = VideoProcAmp_BacklightCompensation;
			bFound = TRUE;
		}

		if (bFound)
		{
			m_pMainWnd->PostMessage(WM_REQUEST_CAMERA_CONTROL,
				(WPARAM) new CameraControlStruct(CId, sKey, sValue, dwUserData, true, false, CamProperty),
				(LPARAM)this);

			return;
		}
		
		CameraControlProperty CamCtrlProp = (CameraControlProperty)0;

		if (sKey == CSD_PAN_PERCENT)
		{
			CamCtrlProp = CameraControl_Pan;
			bFound = TRUE;
		}
		else if (sKey == CSD_TILT_PERCENT)
		{
			CamCtrlProp = CameraControl_Tilt;
			bFound = TRUE;
		}
		else if (sKey == CSD_ROLL_PERCENT)
		{
			CamCtrlProp = CameraControl_Roll;
			bFound = TRUE;
		}
		else if (sKey == CSD_ZOOM_PERCENT)
		{
			CamCtrlProp = CameraControl_Zoom;
			bFound = TRUE;
		}
		else if (sKey == CSD_EXPOSURE_PERCENT)
		{
			CamCtrlProp = CameraControl_Exposure;
			bFound = TRUE;
		}
		else if (sKey == CSD_IRIS_PERCENT)
		{
			CamCtrlProp = CameraControl_Iris;
			bFound = TRUE;
		}
		else if (sKey == CSD_FOCUS_PERCENT)
		{
			CamCtrlProp = CameraControl_Focus;
			bFound = TRUE;
		}

		if (bFound)
		{
			m_pMainWnd->PostMessage(WM_REQUEST_CAMERA_CONTROL,
				(WPARAM) new CameraControlStruct(CId, sKey, sValue, dwUserData, true, false, CamCtrlProp),
				(LPARAM)this);

			return;
		}
		
		if (sKey == CSD_MD)
		{
			WK_TRACE(_T("SetMotionDetection\n"));
			m_pMainWnd->SetMotionDetection(wSource, (sValue == CSD_ON) ? true : false);
			bFound = TRUE;
		}
		else if (sKey == CSD_CAMERA_CONTROL)
		{
			dwValue = _ttoi(sValue);
			m_pMainWnd->PostMessage(WM_REQUEST_CAMERA_CONTROL,
				(WPARAM) new CameraControlStruct(CId, sKey, sValue, dwUserData, true, (CameraControlCmd)dwValue),
				(LPARAM)this);

			return;
		
		}
		else if (sKey == CSD_MD_MASK_SENSITIVITY)
		{
			if (m_pMainWnd->GetMD())
			{
				m_pMainWnd->GetMD()->SetMDMaskSensitivity(wSource, sValue);
				bFound = TRUE;
			}
		}
		else if (sKey == CSD_MD_ALARM_SENSITIVITY)
		{
			if (m_pMainWnd->GetMD())
			{
				m_pMainWnd->GetMD()->SetMDAlarmSensitivity(wSource, sValue);
				bFound = TRUE;
			}
		}
		else if (sKey == CSD_ACTIVE_CAMERA_MASK)
		{
			DWORD i, dwMask;
			dwValue = _ttoi(sValue);
			for (i=0, dwMask=1; i<MAX_CAMERAS; i++, dwMask<<=1)
			{
				m_pMainWnd->SetCameraActive((WORD)i, dwValue&dwMask ? true : false);
			}
			bFound = TRUE;
		}

		if (bFound)
		{
			DoConfirmSetValue(CId, sKey, sValue, dwUserData);
			return;
		}

		WK_TRACE_ERROR(_T("No Main Window\n"));
		DoIndicateError(0, CId, CIPC_ERROR_UNHANDLED_CMD, 0);	
	}
	else
	{
		WK_TRACE_ERROR(_T("No Main Window\n"));
		DoIndicateError(0, CId, CIPC_ERROR_CAMERA_NOT_PRESENT, 0);	
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::OnRequestVersionInfo(WORD wGroupID)
{
//	DBG_TRACE(_T("OnRequestVersionInfo"));
	DoConfirmVersionInfo(wGroupID, 1);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::OnRequestReset(WORD wGroupID)	
{
//	DBG_TRACE(_T("OnRequestReset"));

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCOutputUSBcamUnit::OnRequestReset\tWrong wGroupID\n"));

		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	m_bReset = true;
	m_pMainWnd->InitResetTimer();
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::OnRequestCurrentState(WORD wGroupID)
{
	WORD	wSourceNr	= 0;
	DWORD	dwStateMask	= 0L;

	// TODO! RKE: Return Active Cameras
	DBG_TRACE(_T("OnRequestCurrentState"));
	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCOutputUSBcamUnit::OnRequestCurrentState\tWrong wGroupID\n"));
		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	dwStateMask = m_pMainWnd->GetActiveCameraMask();
	
	DoConfirmCurrentState(m_wGroupID, dwStateMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::OnRequestSetGroupID(WORD wGroupID)
{
//	DBG_TRACE(_T("OnRequestSetGroupID"));
	m_wGroupID = wGroupID;
	DoConfirmSetGroupID(m_wGroupID);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::OnRequestHardware(WORD wGroupID)
{
	int iErrorCode = 0;
//	DBG_TRACE(_T("OnRequestHardware"));

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCOutputUSBcamUnit::OnRequestHardware\tWrong wGroupID\n"));

		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	DoConfirmHardware(m_wGroupID, iErrorCode,
									FALSE, // Can I BW-Compress
									FALSE, // Can I BW-DeCompress
									TRUE, // Can I Color-Compress
									FALSE, // Can I Color-DeCompress
									FALSE);// Can I Overlay: Das waere das IVideo Renderer Window
}

//////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::OnRequestDisconnect()
{
//	DBG_TRACE(_T("OnRequestDisconnect"));

 	if (m_pMainWnd)
	{
		m_pMainWnd->PostMessage(WM_COMMAND, MAKELONG(ID_RELEASE_ALL, BN_CLICKED), NULL);
	}
}

//////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::OnRequestSetDisplayWindow(WORD wGroupID, const CRect &Rect)
{
//	DBG_TRACE(_T("OnRequestSetDisplayWindow"));
	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCOutputUSBcamUnit::OnRequestSetDisplayWindow\tWrong wGroupID\n"));

		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	DoConfirmSetDisplayWindow(wGroupID);
}
		 
//////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::OnRequestSetOutputWindow(WORD wGroupID, const CRect& /*Rect*/, OverlayDisplayMode /*odm*/)
{
//	DBG_TRACE(_T("OnRequestSetOutputWindow"));
	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCOutputUSBcamUnit::OnRequestSetOutputWindow\tWrong wGroupID\n"));
		
		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	DoConfirmSetOutputWindow(wGroupID, 0);
}

//////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::OnRequestUnitVideo(WORD wGroupID,
											 BYTE byCam,
											 BYTE byNextCam,
											 Resolution res,
											 Compression comp,
											 CompressionType ct,
											 int nNumPictures,
											 DWORD dwUserID)
{
//	DBG_TRACE(_T("OnRequestUnitVideo"));
	CSecID camID(wGroupID, byCam);
	CSecID camIDNext;
	if (m_pMainWnd->IsShuttingDown())
	{
		return;
	}

	if (byNextCam != 255)
		camIDNext.Set(wGroupID, byNextCam);
	
	if (camID == SECID_NO_ID)
	{
		WK_TRACE_WARNING(_T("CIPCOutputUSBcamUnit::OnRequestEncodedVideo\tcamID=SECID_NOID\n"));
		return;
	}

	CVideoJob *pJob;

	if (nNumPictures != 0)
	{
		// Wurde diese Kamera als defekt erkannt und liegt der Ausfall weniger als
		// 30 Sekunden zurück, dann verwerfe diesen Job und schicke statdessen ein
		// Jpeg-Testbild an den Server.

		if (m_pMainWnd && !m_pMainWnd->IsCameraPresent(byCam))
		{
			IMAGE	Image;
			ZeroMemory(&Image, sizeof(IMAGE));

			Image.wSource			= camID.GetSubID();
			Image.pImageData		= NULL;
			if (res == RESOLUTION_LOW)
			{
				Image.pImageData	= m_lpSmallTestbild;
				Image.dwImageLen	= m_dwSmallTestbildLen;
				Image.dwProzessID	= dwUserID;
			}
			else
			{
				Image.pImageData	= m_lpLargeTestbild;
				Image.dwImageLen	= m_dwLargeTestbildLen;
				Image.dwProzessID	= dwUserID;
			}		
			Image.TimeStamp.bValid	= FALSE;
			Image.wCompressionType  = COMPRESSION_JPEG;
			IndicationDataReceived(&Image,FALSE);
			return;
		}

		WK_TRACE(_T("EncoderStart:%x->%x, %d, %d, %d, %x\n"), camID.GetID(), camIDNext.GetID(), res, comp, ct, dwUserID);
		pJob = new CVideoJob(camID, camIDNext, res, comp, ct, dwUserID, 0, 0);
	}
	else
	{
		// EncoderStop
		WK_TRACE(_T("EncoderStop:%x\n"), camID.GetID());
		pJob = new CVideoJob(camID, VJA_STOP_ENCODE);
	}
	
	if (pJob)
	{
		pJob->m_iOutstandingPics = nNumPictures;
		pJob->m_pCipc = this;
		WK_TRACE(_T("Pictures:%d\n"), nNumPictures);
	}
	
	if (m_pMainWnd)
	{
		m_pMainWnd->GetCsVideoJob()->Lock();
		m_pMainWnd->GetVideoJobQueue(byCam)->AddTail(pJob);
		m_pMainWnd->GetCsVideoJob()->Unlock();
		m_pMainWnd->PostMessage(WM_TIMER, EVENT_VIDEO_JOB_MIN+byCam);
	}
	else
	{
		WK_DELETE(pJob);
	}
}
//////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::OnRequestIdlePictures(int iNumPics)
{
//	DBG_TRACE(_T("OnRequestIdlePictures"));

	CVideoJob *pJob;

	pJob = new CVideoJob(iNumPics);

	if (!pJob)
	{
		WK_TRACE_ERROR(_T("pJob == NULL\n"));
		return;
	}

	pJob->m_pCipc = this;

	if (m_pMainWnd)
	{
		m_pMainWnd->GetCsVideoJob()->Lock();
		m_pMainWnd->GetVideoJobQueue(0)->AddTail(pJob);
		m_pMainWnd->GetCsVideoJob()->Unlock();
	}
	else
	{
		WK_DELETE(pJob);
	}
}

//////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::IndicationDataReceived(const IMAGE* pImage, BOOL bSignal)
{
//	DBG_TRACE(_T("IndicationDataReceived"));
	static  WORD wSendBlockNr	= 0;

	CSystemTime	TimeStamp;

	if ((pImage==NULL) || (pImage->pImageData==NULL) || (pImage->dwImageLen == 0L))
		return;

	// Uhrzeit gültig?
	if (pImage->TimeStamp.bValid)
	{
		TimeStamp.wYear		= pImage->TimeStamp.wYear;
		TimeStamp.wMonth		= pImage->TimeStamp.wMonth;
		TimeStamp.wDay			= pImage->TimeStamp.wDay;
	  	TimeStamp.wHour		= pImage->TimeStamp.wHour;
		TimeStamp.wMinute		= pImage->TimeStamp.wMinute;
		TimeStamp.wSecond		= pImage->TimeStamp.wSecond;
		TimeStamp.wMilliseconds = pImage->TimeStamp.wMSecond;
		TimeStamp.wDayOfWeek	= 0;
	}
	else
	{
		TimeStamp.GetLocalTime();
	}

	CSecID camID(m_wGroupID, pImage->wSource);
	
	if (pImage->wCompressionType == COMPRESSION_RGB_24)
	{	
		LPBITMAPINFO lpBMI = (LPBITMAPINFO) pImage->pHeaderData;
		if (lpBMI == NULL) lpBMI = (LPBITMAPINFO) pImage->pImageData;

		CIPCPictureRecord PictRec(this,
								 (const unsigned char*)pImage->pImageData,
								  lpBMI->bmiHeader.biSizeImage,
								  camID,
								  m_res,
								  COMPRESSION_NONE,
								  COMPRESSION_RGB_24,
								  TimeStamp,
								  TRUE,
								  NULL,
								  0,
								  SPT_FULL_PICTURE,
								  0,
								  pImage->pHeaderData, pImage->dwHeaderLen);
		CSecID idArchive; // TODO! RKE: Archive ID ermitteln
		DoConfirmJpegEncoding(PictRec, pImage->dwProzessID, idArchive, FALSE); // FALSE=Kein Sleep(0) im CIPC
	}
	else if (pImage->wCompressionType == COMPRESSION_YUV_422)
	{	
		LPBITMAPINFO lpBMI = (LPBITMAPINFO) pImage->pHeaderData;
		if (lpBMI == NULL) lpBMI = (LPBITMAPINFO) pImage->pImageData;

		CIPCPictureRecord PictRec(this,
								 (const unsigned char*)pImage->pImageData,
								  lpBMI->bmiHeader.biSizeImage,
								  camID,
								  m_res,
								  COMPRESSION_NONE,
								  COMPRESSION_YUV_422,
								  TimeStamp,
								  TRUE,
								  NULL,
								  0,
								  SPT_FULL_PICTURE,
								  0,
								  pImage->pHeaderData, pImage->dwHeaderLen);
		CSecID idArchive; // TODO! RKE: Archive ID ermitteln
		DoConfirmJpegEncoding(PictRec, pImage->dwProzessID, idArchive, FALSE ); // FALSE=Kein Sleep(0) im CIPC
	}
	else // Kein DIB
	{
		CIPCPictureRecord PictRec(this,
								 (const unsigned char*)pImage->pImageData,
								  pImage->dwImageLen,
								  camID,
								  m_res,
								  m_comp,
								  (CompressionType)pImage->wCompressionType,
								  TimeStamp,
								  bSignal,
								  NULL,
								  0,
								  SPT_FULL_PICTURE,
								  wSendBlockNr++);
   
		CSecID idArchive; // TODO! RKE: Archive ID ermitteln
		DoConfirmJpegEncoding(PictRec, pImage->dwProzessID, idArchive, FALSE); // FALSE=Kein Sleep(0) im CIPC
	}

	// WK_STAT_PEAK(_T("Reset"), 1, _T("DoConfirmJpeg"));
	int iSendQueueLength = GetSendQueueLength();
	if (iSendQueueLength>25)
	{
		WK_STAT_LOG(_T("PictureSendQueue"),iSendQueueLength,GetShmName());
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::IndicationVideoState(WORD wCamSubID, BOOL bVideoState)
{					  
//	DBG_TRACE(_T("IndicationVideoState"));
	CString sError, sMsg;
	   									 
	if (!bVideoState)		    
	{
		sMsg.LoadString(IDS_RTE_NO_VIDEO);
		sError.Format(sMsg, wCamSubID+1);
		CWK_RunTimeError RTerr(REL_ERROR, RTE_CAMERA_MISSING, sError, wCamSubID+1, TRUE);
		RTerr.Send();
	}
	else
	{
		sMsg.LoadString(IDS_RTE_VIDEO);
		sError.Format(sMsg, wCamSubID+1);

		CWK_RunTimeError RTerr(REL_ERROR, RTE_CAMERA_MISSING, sError, wCamSubID+1, FALSE);
		RTerr.Send();
	}

}

/////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::SelectCamera(CSecID camID, CSecID camIDNext, DWORD dwProzessID, CompressionType compType, DWORD dwPics)
{
//	DBG_TRACE(_T("SelectCamera"));
	WORD	wNewSource		= 0;
	WORD	wGroupID		= 0;
	WORD	wSubID			= 0;
	
	wGroupID = camID.GetGroupID();
	wSubID	 = camID.GetSubID();

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCOutputUSBcamUnit::SelectCamera Wrong wGroupID\n"));

		DoIndicateError(0, camID, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::OnRequestSync(DWORD dwTickSend,DWORD dwType, DWORD dwUser)
{
//	DBG_TRACE(_T("OnRequestSync"));

#ifdef TEST_WITH_SAVIC
	CVideoJob *pJob;
	pJob = new CVideoJob(dwTickSend,dwType,dwUser);	// VJA_SYNC

	if (pJob)
		pJob->m_pCipc = this;

	if (m_pMainWnd)
	{
		m_pMainWnd->GetCsVideoJob()->Lock();
		m_pMainWnd->GetVideoJobQueue(-1)->AddTail(pJob);
		m_pMainWnd->GetCsVideoJob()->Unlock();
	}
	else
	{
		WK_DELETE(pJob);
	}
#endif
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::OnRequestGetMask(CSecID camID, MaskType type, DWORD dwUserID)
{
//	DBG_TRACE(_T("OnRequestGetMask"));

	if (m_pMainWnd && m_pMainWnd->GetMD())
	{
		CIPCActivityMask mask(type);
		if (m_pMainWnd->GetMD()->GetMask(camID.GetSubID(), mask))
		{
			DoConfirmGetMask(camID, dwUserID, mask);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::OnRequestSetMask(CSecID camID, DWORD dwUserID, const CIPCActivityMask& mask)
{
//	DBG_TRACE(_T("OnRequestSetMask"));
	if (m_pMainWnd && m_pMainWnd->GetMD())
	{
		if (m_pMainWnd->GetMD()->SetMask(camID.GetSubID(), mask))
		{
			DoConfirmSetMask(camID, mask.GetType(), dwUserID);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::OnReceivedMotionDIB(WORD wSource, HANDLE hDIB)
{
//	DBG_TRACE(_T("OnReceivedMotionDIB"));
	CSystemTime	TimeStamp;

	if (!hDIB)
		return;

	TimeStamp.GetLocalTime();

	CSecID camID(m_wGroupID, wSource);
	
	LPBITMAPINFO lpBMI = (LPBITMAPINFO) GlobalLock(hDIB); 

	CIPCPictureRecord PictRec(this,
							 (const unsigned char*)lpBMI,
							  lpBMI->bmiHeader.biSizeImage + lpBMI->bmiHeader.biSize,
							  camID,
							  RESOLUTION_LOW,
							  COMPRESSION_NONE,
							  COMPRESSION_YUV_422,
							  TimeStamp,
							  TRUE,
							  NULL,
							  (WORD)0,
							  SPT_FULL_PICTURE,
							  0);
	DoIndicateLocalVideoData(camID, (WORD)lpBMI->bmiHeader.biWidth, (WORD)lpBMI->bmiHeader.biHeight, PictRec);

	GlobalUnlock(hDIB);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::OnResetButtonPressed()
{
//	DBG_TRACE(_T("OnResetButtonPressed"));
	DoRequestSetValue(SECID_NO_ID, CSD_RESET, _T(""), 0);
}

BOOL CIPCOutputUSBcamUnit::IsConnected()
{
	return (GetIPCState() == CIPC_STATE_CONNECTED) ? TRUE : FALSE;
}
//////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::ConfirmReset()
{
	m_bReset = FALSE;
	DoConfirmReset(m_wGroupID);
}
//////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::OnConfirmJpegEncoding(const CIPCPictureRecord &pict,
								   	   DWORD dwUserData,
									   CSecID idArchive)
{
	m_pMainWnd->PostMessage(WM_CONFIRM_PICTURE_RECORD, (WPARAM)new CIPCPictureRecord(pict), 0);
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::OnReadChannelFound()
{
	if (GetIsMaster())
	{
		DoRequestConnection();
	}
}


// Test to connect to tasha, jacob and savic unit
void CIPCOutputUSBcamUnit::OnConfirmSetGroupID(WORD wGroupID)
{

}
void CIPCOutputUSBcamUnit::OnConfirmReset(WORD wGroupID)
{
	TRACE(_T("OnConfirmReset\n"));
}
void CIPCOutputUSBcamUnit::OnConfirmHardware(WORD wGroupID,	
									int errorCode,	// 0==okay, !=0 some error
									BOOL bCanSWCompress,
									BOOL bCanSWDecompress,
									BOOL bCanColorCompress,
									BOOL bCanColorDecompress,
									BOOL bCanOverlay
									)
{
	TRACE(_T("OnConfirmHardware(%d, %d, %d, %d, %d, %d, %d)\n"),
									wGroupID,	
									errorCode,	// 0==okay, !=0 some error
									bCanSWCompress,
									bCanSWDecompress,
									bCanColorCompress,
									bCanColorDecompress,
									bCanOverlay);

}
void CIPCOutputUSBcamUnit::OnConfirmCurrentState(WORD wGroupID, DWORD stateMask)
{
	TRACE(_T("OnConfirmCurrentState(%d, %x)\n"), wGroupID, stateMask);
}

void CIPCOutputUSBcamUnit::OnIndicateVideo(const CIPCPictureRecord& pict,
								 DWORD dwMDX,
								 DWORD dwMDY,
								 DWORD dwUserData,
								 CSecID idArchive)
{
	OnConfirmJpegEncoding(pict, dwUserData, idArchive);
//	TRACE(_T("OnIndicateVideo(%d, %d, %d, %x)\n"), dwMDX, dwMDY, dwUserData, idArchive.GetID());
}
