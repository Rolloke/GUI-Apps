/////////////////////////////////////////////////////////////////////////////
// PROJECT:		QUnit
// FILE:		$Workfile: CipcOutputQUnit.cpp $
// ARCHIVE:		$Archive: /Project/Units/QUnit/CipcOutputQUnit.cpp $
// CHECKIN:		$Date: 21.01.06 21:20 $
// VERSION:		$Revision: 19 $
// LAST CHANGE:	$Modtime: 21.01.06 21:20 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "QUnitDlg.h"
#include "CIPCOutputQUnit.h"
#include "CIPCExtraMemory.h"
#include "CIPCStringDefs.h"
#include "VideoJob.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

CIPCOutputQUnit::CIPCOutputQUnit(CQUnitDlg* pMainWnd, CUDP *pUDP,
							 int iType, LPCTSTR shmName,
							 const CString &sAppIniName)
	: CIPCOutput(shmName, FALSE)
{
	m_pUDP				= pUDP;							// Pointer auf CUDP-Objekt
	m_iType				= iType;						// OUTPUT_CAMERA, OUTPUT_RELAY
	m_wGroupID			= SECID_NO_GROUPID;				// Noch keine GroupID
	m_bOK				= TRUE;							// TRUE->Objekt ok
	m_pMainWnd			= pMainWnd;
	m_lpSmallTestbild	= NULL;							// Pointer auf kleines Störungsbild
	m_dwSmallTestbildLen= 0;							// Länge des kleinen Störungsbildes
	m_lpLargeTestbild	= NULL;							// Pointer auf großes Störungsbild
	m_dwLargeTestbildLen= 0;							// Länge des großen Störungsbildes
	m_sAppIniName		= sAppIniName;

	// Es gibt 2 Outputgruppen: a) Relays, b) Kameras
	if (iType == OUTPUT_TYPE_CAMERA)
	{
		for (int nCamera = 0; nCamera < m_pUDP->GetAvailableCameras(); nCamera++)
		{
			m_VideoStream[nCamera].dwUserData		= (DWORD)-1;
			m_VideoStream[nCamera].Resolution		= RESOLUTION_NONE;
			m_VideoStream[nCamera].bEncoderState	= FALSE;
		}
	}

	SetConnectRetryTime(25);
	StartThread();
}

//////////////////////////////////////////////////////////////////////////////////////
CIPCOutputQUnit::~CIPCOutputQUnit()
{
	// TRACE(_T("CIPCOutputQUnit::~CIPCOutputQUnit\n"));
	m_bOK	= FALSE;

	StopThread();
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputQUnit::IsValid()
{
	return m_bOK;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputQUnit::OnRequestGetValue(CSecID CId, const CString &sKey, DWORD dwUserData)
{
	CString sValue = _T("");

	if (CId.GetGroupID() != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCOutputQUnit::OnRequestGetValue Wrong GroupID %s, %04hx!=%04hx\n"),sKey,CId.GetGroupID(),m_wGroupID);
		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	int nCamera = CId.GetSubID();

	if (sKey == CSD_BRIGHTNESS)
	{
		if (m_pUDP)
		{
			int nValue = 0;
			if (m_pUDP->DoRequestGetBrightness(nCamera, nValue))
				sValue.Format(_T("%u"), nValue); 
		}
	}
	else if (sKey == CSD_CONTRAST)
	{
		if (m_pUDP)
		{
			int nValue = 0;
			if (m_pUDP->DoRequestGetContrast(nCamera, nValue))
				sValue.Format(_T("%u"), nValue); 
		}
	}
	else if (sKey == CSD_SATURATION)
	{
		if (m_pUDP)
		{
			int nValue = 0;
			if (m_pUDP->DoRequestGetSaturation(nCamera, nValue))
				sValue.Format(_T("%u"), nValue); 
		}
	}
	else if (sKey == CSD_SCAN_FOR_CAMERAS)
	{
		if (m_pUDP)
			sValue.Format(_T("%I64u"), m_pUDP->ScanForCameras()); 
//		if (m_pUDP)
//			sValue.Format(_T("%lu"), m_pUDP->ScanForCameras()); 
	}
	else if (sKey == CSD_MD_MASK_SENSITIVITY)
	{
		if (m_pUDP)
			sValue = m_pUDP->GetMDMaskSensitivity(nCamera);
	}
	else if (sKey == CSD_MD_ALARM_SENSITIVITY)
	{
		if (m_pUDP)
			sValue = m_pUDP->GetMDAlarmSensitivity(nCamera); 
	}

	DoConfirmGetValue(CId, sKey, sValue, dwUserData);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputQUnit::OnRequestSetValue(CSecID CId, const CString &sKey, const CString &sValue, DWORD dwUserData)
{
	if (CId.GetGroupID() != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCOutputQUnit::OnRequestSetValue\tWrong GroupID\n"));
		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	int nCamera = CId.GetSubID();

	if (sKey == CSD_BRIGHTNESS)
	{
		if (m_pUDP)
			m_pUDP->DoRequestSetBrightness(nCamera, _ttoi(sValue));
	}
	else if (sKey == CSD_CONTRAST)
	{
		if (m_pUDP)
			m_pUDP->DoRequestSetContrast(nCamera, _ttoi(sValue));
	}
	else if (sKey == CSD_SATURATION)
	{
		if (m_pUDP)
			m_pUDP->DoRequestSetSaturation(nCamera, _ttoi(sValue));
	}
	else if (sKey == CSD_MD)
	{
		if (m_pUDP)
		{
			if (sValue == CSD_ON)
				m_pUDP->ActivateMotionDetection(nCamera);
			if (sValue == CSD_OFF)
				m_pUDP->DeactivateMotionDetection(nCamera);
		}
	}
	else if (sKey == CSD_MD_MASK_SENSITIVITY)
	{
		if (m_pUDP)
			m_pUDP->SetMDMaskSensitivity(nCamera, sValue);
	}
	else if (sKey == CSD_MD_ALARM_SENSITIVITY)
	{
		if (m_pUDP)
			m_pUDP->SetMDAlarmSensitivity(nCamera, sValue);
	}
	else if (sKey == CSD_V_OUT)
	{
		if (m_pUDP)
			m_pUDP->SetCameraToAnalogOut(nCamera);
	}
	else if (sKey == CSD_LED1_ON)
	{
		if (m_pUDP)
			m_pUDP->SetLEDState(eLED1_On);	
	}
	else if (sKey == CSD_LED1_BLINK)
	{
		if (m_pUDP)
			m_pUDP->SetLEDState(eLED1_Blink);	
	}
	else if (sKey == CSD_LED2_ON)
	{
		if (m_pUDP)
			m_pUDP->SetLEDState(eLED2_On);	
	}
	else if (sKey == CSD_LED2_OFF)
	{
		if (m_pUDP)
			m_pUDP->SetLEDState(eLED2_Off);	
	}

	DoConfirmSetValue(CId, sKey, sValue, dwUserData);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputQUnit::OnRequestVersionInfo(WORD wGroupID)
{
	DoConfirmVersionInfo(wGroupID, 3);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputQUnit::OnRequestReset(WORD wGroupID)	
{
	WK_TRACE(_T("CIPCOutputQUnit::OnRequestReset\tGroupID=%i\n"), wGroupID);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCOutputQUnit::OnRequestReset\tWrong wGroupID\n"));

		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	// Alle Bildanforderungen disablen. Server muß die Streams neu anfordern!
	WK_TRACE(_T("CIPCOutputTashaUnit::OnRequestReset Dismiss VideoStream Info\n"));
	for (int nCamera = 0; nCamera < m_pUDP->GetAvailableCameras(); nCamera++)
	{
		m_VideoStream[nCamera].dwUserData		= (DWORD)-1;
		m_VideoStream[nCamera].Resolution		= RESOLUTION_NONE;
		m_VideoStream[nCamera].bEncoderState	= FALSE;
	}
	
	DoConfirmReset(m_wGroupID);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputQUnit::OnRequestSetRelay(CSecID relayID, BOOL bClosed)
{
 	if (relayID.GetGroupID() != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCOutputQUnit::OnRequestSetRelay\tWrong wGroupID\n"));

		DoIndicateError(0, relayID, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	if (m_iType != OUTPUT_TYPE_RELAY)
	{
		WK_TRACE_ERROR(_T("CIPCOutputQUnit::OnRequestSetRelay\tWrong Grouptype\n"));
		return;
	}

	if (m_pUDP)
	{
		DWORD	dwOldState	= 0L;
		DWORD	dwNewState	= 0L;
		
		// Bisherigen Relaisstatus holen.
		m_pUDP->DoRequestGetRelay(dwOldState);

		if (bClosed)
			dwNewState = (DWORD)SETBIT(dwOldState, relayID.GetSubID()); // Relais ein
		else
			dwNewState = (DWORD)CLRBIT(dwOldState, relayID.GetSubID()); // Relais aus
		
		// Relais setzen
		m_pUDP->DoRequestSetRelay((BYTE)(dwNewState & 0x0000000f));

		// Und Server mitteilen.
		DoConfirmSetRelay(relayID, bClosed);
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputQUnit::OnRequestCurrentState(WORD wGroupID)
{
	int		nCamera	= 0;
	DWORD	dwStateMask	= 0L;

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCOutputQUnit::OnRequestCurrentState\tWrong wGroupID\n"));
		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	if (m_iType == OUTPUT_TYPE_RELAY)
	{
		if (m_pUDP)
			m_pUDP->DoRequestGetRelay(dwStateMask);
	}	
	else if (m_iType == OUTPUT_TYPE_CAMERA)
	{
		// Bisherige Kameraauswahl holen
		if (m_pUDP)
			nCamera = m_pUDP->GetActiveCamera();

		dwStateMask = (DWORD)SETBIT(0L, nCamera);
	}	
	DoConfirmCurrentState(m_wGroupID, dwStateMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputQUnit::OnRequestSetGroupID(WORD wGroupID)
{
	m_wGroupID = wGroupID;
	DoConfirmSetGroupID(m_wGroupID);
	WK_TRACE(_T("%s Group ID %04lx\n"),GetShmName(),m_wGroupID);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputQUnit::OnRequestHardware(WORD wGroupID)
{
	int iErrorCode = 0;

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("CIPCOutputQUnit::OnRequestHardware\tWrong wGroupID\n"));

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
void CIPCOutputQUnit::OnRequestDisconnect()
{
}

//////////////////////////////////////////////////////////////////////////
void CIPCOutputQUnit::OnRequestStartVideo(CSecID camID,	Resolution res, Compression comp,
													CompressionType ct,	int iFPS, int iIFrameInterval, DWORD dwUserData)
{
	int nCamera   = camID.GetSubID();

	m_VideoStream[nCamera].bEncoderState	= TRUE;

	// Hat sich die geforderte Auflösung verändert?
	//if (m_VideoStream[nCamera].Resolution != res)
	{
		m_VideoStream[nCamera].Resolution	= res;
		m_VideoStream[nCamera].dwUserData	= dwUserData;
		
		switch (res)
		{
		case RESOLUTION_4CIF:
			m_pUDP->DoRequestSetImageSize(nCamera, CSize(704, 576));
			break;
		case RESOLUTION_2CIF:
			m_pUDP->DoRequestSetImageSize(nCamera, CSize(704, 288));
			break;
		case RESOLUTION_CIF:
			m_pUDP->DoRequestSetImageSize(nCamera, CSize(352, 288));
			break;
		case RESOLUTION_QCIF:
			m_pUDP->DoRequestSetImageSize(nCamera, CSize(176, 144));
			break;
		}
	}

	WK_TRACE(_T("CIPCOutputQUnit::OnRequestStartVideo Camera=%d fps=%d\n"), nCamera, iFPS);

	// Hat sich die geforderte Framerate verändert?
	//if (m_VideoStream[nCamera].iFPS != iFPS)
	{
		m_VideoStream[nCamera].iFPS			= iFPS;
		m_VideoStream[nCamera].dwUserData	= dwUserData;
		m_pUDP->DoRequestSetFramerate(nCamera, iFPS);
	}		
}

//////////////////////////////////////////////////////////////////////////
void CIPCOutputQUnit::OnRequestStopVideo(CSecID camID, DWORD dwUserData)
{
	int nCamera = camID.GetSubID(); 

	WK_TRACE(_T("CIPCOutputQUnit::OnRequestStopVideo Camera=%d\n"), nCamera);

	m_VideoStream[nCamera].bEncoderState = FALSE;
}

//////////////////////////////////////////////////////////////////////////
void CIPCOutputQUnit::IndicationDataReceived(const QIMAGE* pQImage)
{
	static  WORD wSendBlockNr	= 0;

	if ((pQImage==NULL) || (pQImage->lpBMI==NULL))
		return;

	int nCamera = pQImage->nCamera;

	// Ist der Stream aktiv?
	if (m_VideoStream[nCamera].bEncoderState)
	{
		DWORD	dwUserData	= m_VideoStream[nCamera].dwUserData;
		Resolution	res		= RESOLUTION_NONE;

		res	= m_VideoStream[nCamera].Resolution;

		CSecID camID(m_wGroupID, (WORD)nCamera);
		
		if (pQImage->ct == COMPRESSION_YUV_422)
		{	
			CIPCPictureRecord PictRec(this,
									(const unsigned char*)pQImage->lpBMI,
									pQImage->lpBMI->bmiHeader.biSizeImage + pQImage->lpBMI->bmiHeader.biSize, 
									camID,
									res,
									COMPRESSION_3, //COMPRESSION_NONE,
									pQImage->ct,
									pQImage->TimeStamp,
									TRUE,
									NULL,
									0,
									SPT_FULL_PICTURE,
									0);
			// ... und ab damit
			if (pQImage->pPMDoints->GetSize()>0)
				DoIndicateVideo(PictRec, pQImage->pPMDoints->GetPointAt(0).x,  pQImage->pPMDoints->GetPointAt(0).y, dwUserData, SECID_NO_ID);
			else
				DoIndicateVideo(PictRec, 0, 0, dwUserData, SECID_NO_ID);
		}
		else if (pQImage->ct == COMPRESSION_JPEG)
		{	
			CIPCPictureRecord PictRec(this,
									(const unsigned char*)pQImage->lpBMI->bmiColors,
									pQImage->lpBMI->bmiHeader.biSizeImage, 
									camID,
									RESOLUTION_CIF,
									COMPRESSION_3,
									pQImage->ct,
									pQImage->TimeStamp,
									TRUE,
									NULL,
									0,
									SPT_FULL_PICTURE,
									0);
			// ... und ab damit
			DoIndicateVideo(PictRec, 0, 0, dwUserData, SECID_NO_ID);
		}
		else // Kein DIB, kein Jpeg
		{
			WK_TRACE_ERROR(_T("CIPCOutputQUnit::IndicationDataReceived wrong Imageformat\n"));
		}

		// WK_STAT_PEAK(_T("Reset"), 1, _T("DoConfirmJpeg"));
		int iSendQueueLength = GetSendQueueLength();
		if (iSendQueueLength>25)
		{
			WK_STAT_LOG(_T("PictureSendQueue"),iSendQueueLength,GetShmName());
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputQUnit::OnIndicationVideoStateChanged(int nCamera, BOOL bVideoState)
{			
	CString sError, sMsg;
		   									 
	if (!bVideoState)		    
	{
		sMsg.LoadString(IDS_RTE_NO_VIDEO);
		sError.Format(sMsg, nCamera+1);
		CWK_RunTimeError RTerr(REL_ERROR, RTE_CAMERA_MISSING, sError, nCamera+1, TRUE);
		RTerr.Send();
	}
	else
	{
		sMsg.LoadString(IDS_RTE_VIDEO);
		sError.Format(sMsg, nCamera+1);

		CWK_RunTimeError RTerr(REL_ERROR, RTE_CAMERA_MISSING, sError, nCamera+1, FALSE);
		RTerr.Send();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputQUnit::OnRequestGetMask(CSecID camID, MaskType type, DWORD dwUserID)
{
	if (m_pUDP)
	{
		CIPCActivityMask mask(type);
		if (m_pUDP->GetMask(camID.GetSubID(), mask))
			DoConfirmGetMask(camID, dwUserID, mask);
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputQUnit::OnRequestSetMask(CSecID camID, DWORD dwUserID, const CIPCActivityMask& mask)
{
	if (m_pUDP)
	{
		if (m_pUDP->SetMask(camID.GetSubID(), mask))
			DoConfirmSetMask(camID, mask.GetType(), dwUserID);
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputQUnit::OnResetButtonPressed()
{
	DoRequestSetValue(SECID_NO_ID, CSD_RESET, _T(""), 0);
}
