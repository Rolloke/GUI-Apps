/////////////////////////////////////////////////////////////////////////////
// PROJECT:		USBCamUnit
// FILE:		$Workfile: CipcOutputUSBcamUnit.cpp $
// ARCHIVE:		$Archive: /Project/Units/USBCAM/USBCamUnit/CipcOutputUSBcamUnit.cpp $
// CHECKIN:		$Date: 17.11.03 14:01 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 28.04.03 12:41 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "USBcamUnitDlg.h"
#include "CIPCOutputUSBcamUnit.h"
#include "CIPCExtraMemory.h"
#include "CIPCStringDefs.h"
#include "VideoJob.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

CIPCOutputUSBcamUnit::CIPCOutputUSBcamUnit(CUSBCamUnitDlg* pMainWnd, 
							 LPCTSTR shmName,
							 const CString &sAppIniName)
	: CIPCOutput(shmName, FALSE)
{
	m_wGroupID			= SECID_NO_GROUPID;				// Noch keine GroupID
	m_res				= RESOLUTION_NONE;				// Resolution
	m_comp				= COMPRESSION_NONE;				// Compression
	m_bOK				= TRUE;							// TRUE->Objekt ok
  	m_dwBPF				= 0;
	m_wFormat			= ENCODER_RES_UNKNOWN;
	m_pMainWnd			= pMainWnd;
	m_camID				= SECID_NO_ID;
	m_lpSmallTestbild	= NULL;							// Pointer auf kleines Störungsbild
	m_dwSmallTestbildLen= 0;							// Länge des kleinen Störungsbildes
	m_lpLargeTestbild	= NULL;							// Pointer auf großes Störungsbild
	m_dwLargeTestbildLen	= 0;							// Länge des großen Störungsbildes
	m_sAppIniName		= sAppIniName;

	m_dwDefaultBPF[COMPRESSION_1]  = 50000;
	m_dwDefaultBPF[COMPRESSION_2]  = 47000;
	m_dwDefaultBPF[COMPRESSION_3]  = 44000;
	m_dwDefaultBPF[COMPRESSION_4]  = 41000;
	m_dwDefaultBPF[COMPRESSION_5]  = 38000;
	m_dwDefaultBPF[COMPRESSION_6]  = 35000;
	m_dwDefaultBPF[COMPRESSION_7]  = 32000;
	m_dwDefaultBPF[COMPRESSION_8]  = 31000;
	m_dwDefaultBPF[COMPRESSION_9]  = 30000;
	m_dwDefaultBPF[COMPRESSION_10] = 28000;
	m_dwDefaultBPF[COMPRESSION_11] = 26000;
	m_dwDefaultBPF[COMPRESSION_12] = 24000;
	m_dwDefaultBPF[COMPRESSION_13] = 22000;
	m_dwDefaultBPF[COMPRESSION_14] = 20000;
	m_dwDefaultBPF[COMPRESSION_15] = 18000;
	m_dwDefaultBPF[COMPRESSION_16] = 16000;
	m_dwDefaultBPF[COMPRESSION_17] = 15000;
	m_dwDefaultBPF[COMPRESSION_18] = 14000;
	m_dwDefaultBPF[COMPRESSION_19] = 13000;
	m_dwDefaultBPF[COMPRESSION_20] = 12000;
	m_dwDefaultBPF[COMPRESSION_21] = 10000;
	m_dwDefaultBPF[COMPRESSION_22] = 8000;
	m_dwDefaultBPF[COMPRESSION_23] = 7000;
	m_dwDefaultBPF[COMPRESSION_24] = 6000;
	m_dwDefaultBPF[COMPRESSION_25] = 5000;
	m_dwDefaultBPF[COMPRESSION_26] = 4000;

#if (0) //Daten aus der Registry lesen
	// Zuordnung der Bildgröße ui den Compressions
	m_dwDefaultBPF[COMPRESSION_1]  = ReadConfigurationInt("Compressions", "Compression1",   m_dwDefaultBPF[COMPRESSION_1], m_sAppIniName);
	m_dwDefaultBPF[COMPRESSION_2]  = ReadConfigurationInt("Compressions", "Compression2",   m_dwDefaultBPF[COMPRESSION_2], m_sAppIniName);
	m_dwDefaultBPF[COMPRESSION_3]  = ReadConfigurationInt("Compressions", "Compression3",   m_dwDefaultBPF[COMPRESSION_3], m_sAppIniName);
	m_dwDefaultBPF[COMPRESSION_4]  = ReadConfigurationInt("Compressions", "Compression4",   m_dwDefaultBPF[COMPRESSION_4], m_sAppIniName);
	m_dwDefaultBPF[COMPRESSION_5]  = ReadConfigurationInt("Compressions", "Compression5",   m_dwDefaultBPF[COMPRESSION_5], m_sAppIniName);
	m_dwDefaultBPF[COMPRESSION_6]  = ReadConfigurationInt("Compressions", "Compression6",   m_dwDefaultBPF[COMPRESSION_6], m_sAppIniName);
	m_dwDefaultBPF[COMPRESSION_7]  = ReadConfigurationInt("Compressions", "Compression7",   m_dwDefaultBPF[COMPRESSION_7], m_sAppIniName);
	m_dwDefaultBPF[COMPRESSION_8]  = ReadConfigurationInt("Compressions", "Compression8",   m_dwDefaultBPF[COMPRESSION_8], m_sAppIniName);
	m_dwDefaultBPF[COMPRESSION_9]  = ReadConfigurationInt("Compressions", "Compression9",   m_dwDefaultBPF[COMPRESSION_9], m_sAppIniName);
	m_dwDefaultBPF[COMPRESSION_10] = ReadConfigurationInt("Compressions", "Compression10",  m_dwDefaultBPF[COMPRESSION_10], m_sAppIniName);
	m_dwDefaultBPF[COMPRESSION_11] = ReadConfigurationInt("Compressions", "Compression11",  m_dwDefaultBPF[COMPRESSION_11], m_sAppIniName);
	m_dwDefaultBPF[COMPRESSION_12] = ReadConfigurationInt("Compressions", "Compression12",  m_dwDefaultBPF[COMPRESSION_12], m_sAppIniName);
	m_dwDefaultBPF[COMPRESSION_13] = ReadConfigurationInt("Compressions", "Compression13",  m_dwDefaultBPF[COMPRESSION_13], m_sAppIniName);
	m_dwDefaultBPF[COMPRESSION_14] = ReadConfigurationInt("Compressions", "Compression14",  m_dwDefaultBPF[COMPRESSION_14], m_sAppIniName);
	m_dwDefaultBPF[COMPRESSION_15] = ReadConfigurationInt("Compressions", "Compression15",  m_dwDefaultBPF[COMPRESSION_15], m_sAppIniName);
	m_dwDefaultBPF[COMPRESSION_16] = ReadConfigurationInt("Compressions", "Compression16",  m_dwDefaultBPF[COMPRESSION_16], m_sAppIniName);
	m_dwDefaultBPF[COMPRESSION_17] = ReadConfigurationInt("Compressions", "Compression17",  m_dwDefaultBPF[COMPRESSION_17], m_sAppIniName);
	m_dwDefaultBPF[COMPRESSION_18] = ReadConfigurationInt("Compressions", "Compression18",  m_dwDefaultBPF[COMPRESSION_18], m_sAppIniName);
	m_dwDefaultBPF[COMPRESSION_19] = ReadConfigurationInt("Compressions", "Compression19",  m_dwDefaultBPF[COMPRESSION_19], m_sAppIniName);
	m_dwDefaultBPF[COMPRESSION_20] = ReadConfigurationInt("Compressions", "Compression20",  m_dwDefaultBPF[COMPRESSION_20], m_sAppIniName);
	m_dwDefaultBPF[COMPRESSION_21] = ReadConfigurationInt("Compressions", "Compression21",  m_dwDefaultBPF[COMPRESSION_21], m_sAppIniName);
	m_dwDefaultBPF[COMPRESSION_22] = ReadConfigurationInt("Compressions", "Compression22",  m_dwDefaultBPF[COMPRESSION_22], m_sAppIniName);
	m_dwDefaultBPF[COMPRESSION_23] = ReadConfigurationInt("Compressions", "Compression23",  m_dwDefaultBPF[COMPRESSION_23], m_sAppIniName);
	m_dwDefaultBPF[COMPRESSION_24] = ReadConfigurationInt("Compressions", "Compression24",  m_dwDefaultBPF[COMPRESSION_24], m_sAppIniName);
	m_dwDefaultBPF[COMPRESSION_25] = ReadConfigurationInt("Compressions", "Compression25",  m_dwDefaultBPF[COMPRESSION_25], m_sAppIniName);
	m_dwDefaultBPF[COMPRESSION_26] = ReadConfigurationInt("Compressions", "Compression26",  m_dwDefaultBPF[COMPRESSION_26], m_sAppIniName);
#endif
	// Gibt Auskunft darüber, ob USBCam Decoding unterstützt wird.
	// CWK_Dongle dongle;
	m_bAllowHardDecode = FALSE;//dongle.AllowHardDecodeMiCo();

	WK_TRACE("Compression1=%lu Bytes/Frame\n", m_dwDefaultBPF[COMPRESSION_5]);
	WK_TRACE("Compression2=%lu Bytes/Frame\n", m_dwDefaultBPF[COMPRESSION_7]);
	WK_TRACE("Compression3=%lu Bytes/Frame\n", m_dwDefaultBPF[COMPRESSION_12]);
	WK_TRACE("Compression4=%lu Bytes/Frame\n", m_dwDefaultBPF[COMPRESSION_17]);
	WK_TRACE("Compression5=%lu Bytes/Frame\n", m_dwDefaultBPF[COMPRESSION_22]);
/*	
	// Kleines Testbild aus der Resource laden
	HRSRC hRc = FindResource(NULL, MAKEINTRESOURCE(BIN_SMALL_TESTBILD), "BINRES");
	if (hRc)
	{
		HGLOBAL hJpegResource	= LoadResource(NULL, hRc);
		if (hJpegResource)
		{
			m_dwSmallTestbildLen	= SizeofResource(NULL, hRc);
			m_lpSmallTestbild		= (BYTE*)LockResource(hJpegResource);
		}
		else
			WK_TRACE_ERROR("Kleines Testbild konnte nicht geladen werden\n");
	}
	else
		WK_TRACE_ERROR("Testbild konnte nicht geladen werden\n");


	// Großes Testbild aus der Resource laden
	hRc = FindResource(NULL, MAKEINTRESOURCE(BIN_LARGE_TESTBILD), "BINRES");
	if (hRc)
	{
		HGLOBAL hJpegResource	= LoadResource(NULL, hRc);
		if (hJpegResource)
		{
			m_dwLargeTestbildLen	= SizeofResource(NULL, hRc);
			m_lpLargeTestbild		= (BYTE*)LockResource(hJpegResource);
		}
		else
			WK_TRACE_ERROR("Großes Testbild konnte nicht geladen werden\n");
	}
	else
		WK_TRACE_ERROR("Testbild konnte nicht geladen werden\n");

*/
	
	SetConnectRetryTime(25);
	StartThread();
}

//////////////////////////////////////////////////////////////////////////////////////
CIPCOutputUSBcamUnit::~CIPCOutputUSBcamUnit()
{
	// TRACE("CIPCOutputUSBcamUnit::~CIPCOutputUSBcamUnit\n");
	m_bOK	= FALSE;

	StopThread();
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputUSBcamUnit::IsValid()
{
	return m_bOK;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::OnRequestGetValue(CSecID CId, const CString &sKey, DWORD dwUserData)
{
	CString sValue = "";

	if (CId.GetGroupID() != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCOutputUSBcamUnit::OnRequestGetValue\tWrong GroupID\n");
		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	WORD wSource = CId.GetSubID(); // Kameranummer 0...15
/*
	if (sKey == CSD_BRIGHTNESS)
	{
		if (m_pCodec)
			sValue.Format("%u", m_pCodec->GetBrightness(wSource)); 
	}
	else if (sKey == CSD_CONTRAST)
	{
		if (m_pCodec)
			sValue.Format("%u", m_pCodec->GetContrast(wSource)); 
	}
	else if (sKey == CSD_SATURATION)
	{
		if (m_pCodec)
			sValue.Format("%u", m_pCodec->GetSaturation(wSource)); 
	}
	else if (sKey == CSD_SCAN_FOR_CAMERAS)
	{
		if (m_pCodec)
			sValue.Format("%lu", m_pCodec->ScanForCameras(dwUserData)); 
	}
	else if (sKey == CSD_MD_MASK_SENSITIVITY)
	{
		if (m_pCodec)
			sValue = m_pCodec->GetMDMaskSensitivity(wSource);
	}
	else if (sKey == CSD_MD_ALARM_SENSITIVITY)
	{
		if (m_pCodec)
			sValue = m_pCodec->GetMDAlarmSensitivity(wSource); 
	}
*/
	DoConfirmGetValue(CId, sKey, sValue, dwUserData);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::OnRequestSetValue(CSecID CId, const CString &sKey, const CString &sValue, DWORD dwUserData)
{
	if (CId.GetGroupID() != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCOutputUSBcamUnit::OnRequestSetValue\tWrong GroupID\n");
		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	WORD wSource = CId.GetSubID(); // Kameranummer 0...15
/*
	if (sKey == CSD_BRIGHTNESS)
	{
		if (m_pCodec)
			m_pCodec->SetBrightness(wSource, atoi(sValue));
	}
	else if (sKey == CSD_CONTRAST)
	{
		if (m_pCodec)
			m_pCodec->SetContrast(wSource, atoi(sValue));
	}
	else if (sKey == CSD_SATURATION)
	{
		if (m_pCodec)
			m_pCodec->SetSaturation(wSource, atoi(sValue));
	}
	else if (sKey == CSD_MD)
	{
		if (m_pCodec)
		{
			if (sValue == CSD_ON)
				m_pCodec->ActivateMotionDetection(wSource);
			if (sValue == CSD_OFF)
				m_pCodec->DeactivateMotionDetection(wSource);
		}
	}
	else if (sKey == CSD_MD_MASK_SENSITIVITY)
	{
		if (m_pCodec)
			m_pCodec->SetMDMaskSensitivity(wSource, sValue);
	}
	else if (sKey == CSD_MD_ALARM_SENSITIVITY)
	{
		if (m_pCodec)
			m_pCodec->SetMDAlarmSensitivity(wSource, sValue);
	}
	else if (sKey == CSD_DIB_TRANSFER)
	{
		if (sValue == CSD_ON)
			m_pCodec->EnableDIBIndication(wSource);
		if (sValue == CSD_OFF)
			m_pCodec->DisableDIBIndication(wSource);
	}
	else if (sKey == CSD_V_OUT)
	{
		if (m_pCodec)
			m_pCodec->SetCameraToAnalogOut(wSource, sValue);
	}
	else if (sKey == CSD_LED1_ON)
	{
		if (m_pCodec)
			m_pCodec->SetLEDState(eLED1_On);	
	}
	else if (sKey == CSD_LED1_BLINK)
	{
		if (m_pCodec)
			m_pCodec->SetLEDState(eLED1_Blink);	
	}
	else if (sKey == CSD_LED2_ON)
	{
		if (m_pCodec)
			m_pCodec->SetLEDState(eLED2_On);	
	}
	else if (sKey == CSD_LED2_OFF)
	{
		if (m_pCodec)
			m_pCodec->SetLEDState(eLED2_Off);	
	}
*/
	DoConfirmSetValue(CId, sKey, sValue, dwUserData);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::OnRequestVersionInfo(WORD wGroupID)
{
	DoConfirmVersionInfo(wGroupID, 3);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::OnRequestReset(WORD wGroupID)	
{
	WK_TRACE("CIPCOutputUSBcamUnit::OnRequestReset\tGroupID=%i\n", wGroupID);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCOutputUSBcamUnit::OnRequestReset\tWrong wGroupID\n");

		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	if (m_pMainWnd)
		m_pMainWnd->RequestOutputReset();

	DoConfirmReset(m_wGroupID);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::OnRequestCurrentState(WORD wGroupID)
{
	WORD	wSourceNr	= 0;
	DWORD	dwStateMask	= 0L;

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCOutputUSBcamUnit::OnRequestCurrentState\tWrong wGroupID\n");
		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	// Bisherige Kameraauswahl holen
//	if (m_pCodec)
//		wSourceNr = m_pCodec->GetInputSource();

	dwStateMask = SETBIT(0L, wSourceNr);
	
	DoConfirmCurrentState(m_wGroupID, dwStateMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::OnRequestSetGroupID(WORD wGroupID)
{
	m_wGroupID = wGroupID;
	DoConfirmSetGroupID(m_wGroupID);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::OnRequestHardware(WORD wGroupID)
{
	int iErrorCode = 0;

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCOutputUSBcamUnit::OnRequestHardware\tWrong wGroupID\n");

		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	DoConfirmHardware(m_wGroupID, iErrorCode,
									FALSE, /* No SW-Compress */
									FALSE, /* No SW-DeCompress */
									TRUE,  /* Co-Compress */
									FALSE, /* No Co-DeCompress */
									FALSE);/* No Overlay */
}

//////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::OnRequestDisconnect()
{
 	if (m_pMainWnd)
		m_pMainWnd->ClearAllJobs();
}

//////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::OnRequestSetDisplayWindow(WORD wGroupID, const CRect &Rect)
{
	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCOutputUSBcamUnit::OnRequestSetDisplayWindow\tWrong wGroupID\n");

		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	DoConfirmSetDisplayWindow(wGroupID);
}
		 
//////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::OnRequestSetOutputWindow(WORD wGroupID, const CRect& /*Rect*/, OverlayDisplayMode /*odm*/)
{
	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCOutputUSBcamUnit::OnRequestSetOutputWindow\tWrong wGroupID\n");
		
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
	CSecID camID(wGroupID, byCam);
	CSecID camIDNext;

//ct  = COMPRESSION_DIB;
//res = RESOLUTION_LOW;
/*
	static CWK_Timer time;

	static LARGE_INTEGER tiCurr64;
	static LARGE_INTEGER tiLast64 = time.GetMicroTicks();;
	static LARGE_INTEGER tiDiff64; 
	
	tiCurr64 = time.GetMicroTicks();
	tiDiff64.QuadPart = tiCurr64.QuadPart - tiLast64.QuadPart;
	tiLast64.QuadPart = tiCurr64.QuadPart;
	WK_STAT_LOG("Reset", tiDiff64.LowPart, "Jobtime");
*/	
	if (byNextCam != 255)
		camIDNext.Set(wGroupID, byNextCam);
	
	if (camID == SECID_NO_ID)
	{
		WK_TRACE_WARNING("CIPCOutputUSBcamUnit::OnRequestEncodedVideo\tcamID=SECID_NOID\n");
		return;
	}

	// WK_STAT_PEAK("Reset", 1, "OnRequestJpeg");

	CVideoJob *pJob;

	if (nNumPictures != 0)
	{
		// Wurde diese Kamera als defekt erkannt und liegt der Ausfall weniger als
		// 30 Sekunden zurück, dann verwerfe diesen Job und schicke statdessen ein
		// Jpeg-Testbild an den Server.
/*
		if (m_pMainWnd && !m_pMainWnd->VideoTimeOutCheck(camID.GetSubID()))
		{
			IMAGE	Image;

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
			Image.wCompressionType  = ct;
			IndicationDataReceived(&Image,FALSE);
			return;
		}

		// EncoderStart
		pJob = new CVideoJob(camID, camIDNext, res, comp, ct, dwUserID, 0, 0);
*/
	}
	else
	{
		// EncoderStop
		pJob = new CVideoJob(camID, VJA_STOP_ENCODE);
	}
	
	if (pJob)
	{
		pJob->m_iOutstandingPics = nNumPictures;
		pJob->m_pCipc = this;
	}
	
	if (m_pMainWnd)
	{
//		m_pMainWnd->GetJobQueueEncode().SafeAddTail(pJob);
	}
	else
	{
		WK_DELETE(pJob);
	}
}
//////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::OnRequestIdlePictures(int iNumPics)
{
	CVideoJob *pJob;

	pJob = new CVideoJob(iNumPics);

	if (!pJob)
	{
		WK_TRACE_ERROR("pJob == NULL\n");
		return;
	}

	pJob->m_pCipc = this;

	if (m_pMainWnd)
	{
//		m_pMainWnd->GetJobQueueEncode().SafeAddTail(pJob);
	}
	else
	{
		WK_DELETE(pJob);
	}
}

//////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::IndicationDataReceived(const IMAGE* pImage, BOOL bSignal)
{
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
		LPBITMAPINFO lpBMI = (LPBITMAPINFO) pImage->pImageData;

		CIPCPictureRecord PictRec(this,
								 (const unsigned char*)lpBMI,
								  lpBMI->bmiHeader.biSizeImage + lpBMI->bmiHeader.biSize,
								  camID,
								  m_res,
								  COMPRESSION_NONE,
								  COMPRESSION_RGB_24,
								  TimeStamp,
								  TRUE,
								  NULL,
								  0,
								  SPT_FULL_PICTURE,
								  0);
		DoConfirmJpegEncoding(PictRec, pImage->dwProzessID, FALSE /*FALSE=Kein Sleep(0) im CIPC*/);
	}
	else if (pImage->wCompressionType == COMPRESSION_YUV_422)
	{	
		LPBITMAPINFO lpBMI = (LPBITMAPINFO) pImage->pImageData;

		CIPCPictureRecord PictRec(this,
								 (const unsigned char*)lpBMI,
								  lpBMI->bmiHeader.biSizeImage + lpBMI->bmiHeader.biSize,
								  camID,
								  m_res,
								  COMPRESSION_NONE,
								  COMPRESSION_YUV_422,
								  TimeStamp,
								  TRUE,
								  NULL,
								  0,
								  SPT_FULL_PICTURE,
								  0);
		DoConfirmJpegEncoding(PictRec, pImage->dwProzessID, FALSE /*FALSE=Kein Sleep(0) im CIPC*/);
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
   
		DoConfirmJpegEncoding(PictRec, pImage->dwProzessID, FALSE /*FALSE=Kein Sleep(0) im CIPC*/);
	}

	// WK_STAT_PEAK("Reset", 1, "DoConfirmJpeg");
	int iSendQueueLength = GetSendQueueLength();
	if (iSendQueueLength>25)
	{
		WK_STAT_LOG("PictureSendQueue",iSendQueueLength,GetShmName());
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::IndicationVideoState(WORD wCamSubID, BOOL bVideoState)
{					  
	CString sError, sMsg;
/*		   									 
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
*/
}

/////////////////////////////////////////////////////////////////////////
int CIPCOutputUSBcamUnit::GetEncoderState()
{
/*
	if (!m_pCodec)
		return ENCODER_OFF;

	return m_pCodec->GetEncoderState();
*/
	return ENCODER_OFF;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::SelectCamera(CSecID camID, CSecID camIDNext, DWORD dwProzessID, CompressionType compType, DWORD dwPics)
{
	// WK_TRACE("CIPCOutputUSBcamUnit::SelectCamera\tGroupID=%i\tNr=%i\n", wGroupID, iNr);
	WORD	wNewSource		= 0;
	WORD	wGroupID		= 0;
	WORD	wSubID			= 0;
	
	wGroupID = camID.GetGroupID();
	wSubID	 = camID.GetSubID();

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCOutputUSBcamUnit::SelectCamera Wrong wGroupID\n");

		DoIndicateError(0, camID, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

/*
	// Auf gewünschte Kamera umschalten
	wNewSource		= wSubID;			// Kameranummer 0...15
	
	if (m_pCodec)
	{
		SOURCE_SELECT_STRUCT SrcSelCurr;

		SrcSelCurr.wSource			= wNewSource;	// Nächste Kamera
		SrcSelCurr.wFormat			= m_wFormat;	// Bildformat (High,Low...)
		SrcSelCurr.dwBPF			= m_dwBPF;		// Bytes per Frame
		SrcSelCurr.dwProzessID		= dwProzessID;	// ProzessID
		SrcSelCurr.dwPics			= dwPics;		// Anzahl der Bilder
		SrcSelCurr.wCompressionType	= compType;		// Art der Kompresssion (Jpeg, DIB,...)
		
		m_pCodec->SetInputSource(SrcSelCurr);
		
		m_camID = camID; 
	}
*/
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputUSBcamUnit::EncoderStart(CVideoJob *pCurrentJob)
{
	BOOL	bRet				= TRUE;
	CSecID	camIDNext			= SECID_NO_ID;

	if (!pCurrentJob)
	{
		WK_TRACE_ERROR("StartEncoder\tpJob==NULL\n");
		return FALSE;
	}
/*
	if (!m_pCodec)
	{
		WK_TRACE_ERROR("StartEncoder\tm_pCodec==NULL\n");
		return FALSE;
	}
*/
	// Alle notwendigen Daten aus dem aktuellen Videojob holen
	CSecID			camID				= pCurrentJob->m_camID;
	Resolution		res				= pCurrentJob->m_resolution;
	Compression		comp				= pCurrentJob->m_compression;
	CompressionType compType		= pCurrentJob->m_compressionType;
	DWORD			dwProzessID			= pCurrentJob->m_dwUserData;

	// Kamera, auf die als nächstes umgeschaltet werden soll.
	camIDNext = pCurrentJob->m_camIDNext;

	if ((comp < COMPRESSION_1) || (comp > COMPRESSION_26))
	{
		WK_TRACE_ERROR("CIPCOutputUSBcamUnit::EncoderStart\tUnbekannte Compression (%u)\n", m_comp);
		return FALSE;
	}

	// res und comp werden hier für 'IndicationMpegDataReceived()' gespeichert
	m_res	= res;
	m_comp	= comp;

	// Bildgröße anhand der Compression wählen
	m_dwBPF = m_dwDefaultBPF[m_comp];
 		
	// Auflösung festlegen
	switch (res)
	{
		case RESOLUTION_LOW:
				m_wFormat = ENCODER_RES_LOW;
				m_dwBPF /= 2;
				break;
		case RESOLUTION_MID:
				m_wFormat = ENCODER_RES_HIGH;
				break;
		case RESOLUTION_HIGH:
				m_wFormat = ENCODER_RES_HIGH;
				break;
		default:
				WK_TRACE_ERROR("CIPCOutputUSBcamUnit::EncoderStart\tUnbekanntes Bildformat\n");	
				m_wFormat = ENCODER_RES_HIGH;
				break;
	}
/*
	// Encoder nur starten, wenn dieser noch nicht aktiv ist.
	if (m_pCodec->GetEncoderState() != ENCODER_ON)
		bRet = m_pCodec->EncoderStart();
*/
	// Setze Kamera, Bildformat, Framerate und die Bytes pro Frame des Encoders.
	SelectCamera(camID, camIDNext, dwProzessID, compType, (DWORD)pCurrentJob->m_iOutstandingPics);

	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputUSBcamUnit::EncoderStop(CVideoJob* /*pJob*/)
{
/*
	if (!m_pCodec)
		return FALSE;

	// Encoder nur stopen, wenn dieser aktiv.
	if (m_pCodec->GetEncoderState() == ENCODER_OFF)
	{
		WK_TRACE_WARNING("CIPCOutputUSBcamUnit::EncoderStop\tEncoder ist bereits off\n");
		return TRUE;
	}

	// Encoder Stoppen
	m_pCodec->EncoderStop();
*/
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::OnRequestSync(DWORD dwTickSend,DWORD dwType, DWORD dwUser)
{
	CVideoJob *pJob;
	pJob = new CVideoJob(dwTickSend,dwType,dwUser);	// VJA_SYNC

	if (pJob)
		pJob->m_pCipc = this;

	if (m_pMainWnd)
	{
//		WK_TRACE(">>>>>ReceiveSync TickSend=%lu Delta=%lu\n",
//				  pJob->m_dwSyncTick, WK_GetTickCount()-pJob->m_dwSyncTick);
//		m_pMainWnd->GetJobQueueEncode().SafeAddTail(pJob);
	}
	else
	{
		WK_DELETE(pJob);
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::OnRequestGetMask(CSecID camID, MaskType type, DWORD dwUserID)
{
/*
	if (m_pCodec)
	{
		CIPCActivityMask mask(type);
		if (m_pCodec->GetMask(camID.GetSubID(), mask))
			DoConfirmGetMask(camID, dwUserID, mask);
	}
*/
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::OnRequestSetMask(CSecID camID, DWORD dwUserID, const CIPCActivityMask& mask)
{
/*
	if (m_pCodec)
	{
		if (m_pCodec->SetMask(camID.GetSubID(), mask))
			DoConfirmSetMask(camID, mask.GetType(), dwUserID);
	}
*/
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::OnReceivedMotionDIB(WORD wSource, HANDLE hDIB)
{
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
							  0,
							  SPT_FULL_PICTURE,
							  0);
	DoIndicateLocalVideoData(camID,lpBMI->bmiHeader.biWidth, lpBMI->bmiHeader.biHeight, PictRec);

	GlobalUnlock(hDIB);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputUSBcamUnit::OnResetButtonPressed()
{
	DoRequestSetValue(SECID_NO_ID, CSD_RESET, "", 0);
}
