/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaUnit
// FILE:		$Workfile: CipcOutputTashaUnit.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha(Developer)/TashaUnit/CipcOutputTashaUnit.cpp $
// CHECKIN:		$Date: 5.01.04 9:54 $
// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 5.01.04 9:25 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TashaUnitDlg.h"
#include "CIPCOutputTashaUnit.h"
#include "CIPCExtraMemory.h"
#include "CIPCStringDefs.h"
#include "VideoJob.h"
#include "CCodec.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

CWK_Timer	theTimer;

CIPCOutputTashaUnit::CIPCOutputTashaUnit(CTashaUnitDlg* pMainWnd, CCodec *pCodec,
							 int iType, const char *shmName) : CIPCOutput(shmName, FALSE)
{
	m_pCodec				= pCodec;						// Pointer auf CTasha-Objekt
	m_iType					= iType;						// OUTPUT_CAMERA, OUTPUT_RELAY
	m_wGroupID				= SECID_NO_GROUPID;				// Noch keine GroupID
	m_res					= RESOLUTION_NONE;				// Resolution
	m_comp					= COMPRESSION_NONE;				// Compression
	m_bOK					= TRUE;							// TRUE->Objekt ok
	m_eImageRes				= eImageResUnvalid;
	m_pMainWnd				= pMainWnd;
	m_camID					= SECID_NO_ID;
	m_lpSmallTestbild		= NULL;							// Pointer auf kleines Störungsbild
	m_dwSmallTestbildLen	= 0;							// Länge des kleinen Störungsbildes
	m_lpLargeTestbild		= NULL;							// Pointer auf großes Störungsbild
	m_dwLargeTestbildLen	= 0;							// Länge des großen Störungsbildes

	m_lpBMI = (LPBITMAPINFO)new BYTE[MAX_HSIZE*MAX_VSIZE*sizeof(WORD)+sizeof(BITMAPINFO)+1024];

	StartThread();
}

//////////////////////////////////////////////////////////////////////////////////////
CIPCOutputTashaUnit::~CIPCOutputTashaUnit()
{
	// TRACE("CIPCOutputTashaUnit::~CIPCOutputTashaUnit\n");
	m_bOK	= FALSE;

	StopThread();

	WK_DELETE(m_lpBMI);
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputTashaUnit::IsValid()
{
	return m_bOK;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnRequestGetValue(CSecID CId, const CString &sKey, DWORD dwUserData)
{
	CString sValue = "";

	if (CId.GetGroupID() != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCOutputTashaUnit::OnRequestGetValue\tWrong GroupID\n");
		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	WORD wSource = CId.GetSubID(); // Kameranummer 0...7

	if (sKey == CSD_BRIGHTNESS)
	{
		if (m_pCodec)
			m_pCodec->DoRequestGetBrightness(dwUserData, wSource);
	}
	else if (sKey == CSD_CONTRAST)
	{
		if (m_pCodec)
			m_pCodec->DoRequestGetContrast(dwUserData, wSource);
	}
	else if (sKey == CSD_SATURATION)
	{
		if (m_pCodec)
			m_pCodec->DoRequestGetSaturation(dwUserData, wSource);
	}
	else if (sKey == CSD_SCAN_FOR_CAMERAS)
	{
		if (m_pCodec)
			m_pCodec->DoRequestScanForCameras(dwUserData); 
	}
/*	else if (sKey == CSD_MD_MASK_SENSITIVITY)
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
//	DoConfirmGetValue(CId, sKey, sValue, dwUserData);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnRequestSetValue(CSecID CId, const CString &sKey, const CString &sValue, DWORD dwUserData)
{
	if (CId.GetGroupID() != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCOutputTashaUnit::OnRequestSetValue\tWrong GroupID\n");
		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	WORD wSource = CId.GetSubID(); // Kameranummer 0...15

	if (sKey == CSD_BRIGHTNESS)
	{
		if (m_pCodec)
			m_pCodec->DoRequestSetBrightness(dwUserData, wSource, atoi(sValue));
	}
	else if (sKey == CSD_CONTRAST)
	{
		if (m_pCodec)
			m_pCodec->DoRequestSetContrast(dwUserData, wSource, atoi(sValue));
	}
	else if (sKey == CSD_SATURATION)
	{
		if (m_pCodec)
			m_pCodec->DoRequestSetSaturation(dwUserData, wSource, atoi(sValue));
	}
/*	else if (sKey == CSD_MD)
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
	else if (sKey == CSD_V_OUT)
	{
		if (m_pCodec)
			m_pCodec->SetCameraToAnalogOut(wSource, sValue);
	}
*/
	DoConfirmSetValue(CId, sKey, sValue, dwUserData);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnRequestVersionInfo(WORD wGroupID)
{
	DoConfirmVersionInfo(wGroupID, 3);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnRequestReset(WORD wGroupID)	
{
	WK_TRACE("CIPCOutputTashaUnit::OnRequestReset\tGroupID=%i\n", wGroupID);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCOutputTashaUnit::OnRequestReset\tWrong wGroupID\n");

		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

//	if (m_pMainWnd)
//		m_pMainWnd->RequestOutputReset();

	DoConfirmReset(m_wGroupID);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnRequestSetRelay(CSecID relayID, BOOL bOpenClose)
{
 	if (relayID.GetGroupID() != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCOutputTashaUnit::OnRequestSetRelay\tWrong wGroupID\n");

		DoIndicateError(0, relayID, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	if (m_iType != OUTPUT_TYPE_RELAY)
	{
		WK_TRACE_ERROR("CIPCOutputTashaUnit::OnRequestSetRelay\tWrong Grouptype\n");
		return;
	}

	if (m_pCodec)
		m_pCodec->DoRequestSetRelayState(relayID.GetSubID(), bOpenClose);
}

/////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmSetRelayState(WORD wRelayID, BOOL bOpenClose)
{
///	WK_TRACE("HOST: OnConfirmSetRelayState (Relay=%d State=%d)\n", wRelayID, bOpenClose);

	CSecID CId(m_wGroupID, wRelayID);

	// Und Server mitteilen.
	DoConfirmSetRelay(CId, bOpenClose);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnRequestCurrentState(WORD wGroupID)
{
	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCOutputTashaUnit::OnRequestCurrentState\tWrong wGroupID\n");
		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	if (m_iType == OUTPUT_TYPE_RELAY)
	{
		if (m_pCodec)
			m_pCodec->DoRequestGetRelayState();
	}	
	else
		WK_TRACE_ERROR(_T("OnRequestCurrentState Unknown iType (%d)\n"), m_iType);
}

/////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmGetRelayState(DWORD dwRelayStateMask)
{
	DoConfirmCurrentState(m_wGroupID, dwRelayStateMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnRequestSetGroupID(WORD wGroupID)
{
	m_wGroupID = wGroupID;
	DoConfirmSetGroupID(m_wGroupID);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnRequestHardware(WORD wGroupID)
{
	int iErrorCode = 0;

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCOutputTashaUnit::OnRequestHardware\tWrong wGroupID\n");

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
void CIPCOutputTashaUnit::OnRequestDisconnect()
{
 	if (m_pMainWnd)
		m_pMainWnd->ClearAllJobs();
}

//////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnRequestSetDisplayWindow(WORD wGroupID, const CRect &Rect)
{
	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCOutputTashaUnit::OnRequestSetDisplayWindow\tWrong wGroupID\n");

		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	DoConfirmSetDisplayWindow(wGroupID);
}
		 
//////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnRequestSetOutputWindow(WORD wGroupID, const CRect& /*Rect*/, OverlayDisplayMode /*odm*/)
{
	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCOutputTashaUnit::OnRequestSetOutputWindow\tWrong wGroupID\n");
		
		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	DoConfirmSetOutputWindow(wGroupID, 0);
}

//////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnRequestUnitVideo(WORD wGroupID,
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

	if (byNextCam != 255)
		camIDNext.Set(wGroupID, byNextCam);
	
	if (camID == SECID_NO_ID)
	{
		WK_TRACE_WARNING("CIPCOutputTashaUnit::OnRequestEncodedVideo\tcamID=SECID_NOID\n");
		return;
	}

	CVideoJob *pJob;

	WK_TRACE(">>>>>>>>>> OnRequestUnitVideo Source=%d\n", byCam);

	if (nNumPictures != 0)
	{
		nNumPictures = 25;	// ToDo: Fake the Request PictureCounter to speed up.
		// EncoderStart
		pJob = new CVideoJob(camID, camIDNext, res, comp, COMPRESSION_YUV_422, dwUserID, 0, 0);
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
		m_pMainWnd->GetJobQueueEncode(byCam).SafeAddTail(pJob);
	}
	else
	{
		WK_DELETE(pJob);
	}
}

//////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnRequestIdlePictures(int iNumPics)
{
	CVideoJob *pJob;

	pJob = new CVideoJob(iNumPics);

	if (!pJob)
	{
		WK_TRACE_ERROR("pJob == NULL\n");
		return;
	}

	pJob->m_pCipc = this;
	WK_TRACE(">>>>>>>>>> OnRequestIdlePictures NumPics=%d\n", iNumPics);

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
void CIPCOutputTashaUnit::IndicationDataReceived(const DATA_PACKET* pPacket, BOOL bSignal)
{
	static  WORD wSendBlockNr	= 0;
	LPCSTR	lpBuffer			= NULL;
	CSystemTime	TimeStamp;

	if (!pPacket)
		return;

	// Uhrzeit gültig?
	if (pPacket->TimeStamp.bValid)
	{
		TimeStamp.wYear			= pPacket->TimeStamp.wYear;
		TimeStamp.wMonth		= pPacket->TimeStamp.wMonth;
		TimeStamp.wDay			= pPacket->TimeStamp.wDay;
	  	TimeStamp.wHour			= pPacket->TimeStamp.wHour;
		TimeStamp.wMinute		= pPacket->TimeStamp.wMinute;
		TimeStamp.wSecond		= pPacket->TimeStamp.wSecond;
		TimeStamp.wMilliseconds = pPacket->TimeStamp.wMSecond;
		TimeStamp.wDayOfWeek	= 0;
	}
	else
	{
		TimeStamp.GetLocalTime();
	}

	lpBuffer	= (char*)&pPacket->pImageData;	// Pointer auf CodecDaten

	if ((pPacket->dwImageDataLen == 0L) || (lpBuffer == NULL))
		return;		  


	CSecID camID(m_wGroupID, pPacket->wSource);
	WK_TRACE("<<<<<<<<<< IndicationDataReceived (Source=%d\n", pPacket->wSource);

	CIPCPictureRecord PictRec(this,
							 (const unsigned char*)CreateDIB(pPacket),
							  pPacket->dwImageDataLen+sizeof(BITMAPINFOHEADER),
							  camID,
							  m_res,
							  m_comp,
							  COMPRESSION_YUV_422,
							  TimeStamp,
							  bSignal,
							  NULL,
							  0,
							  SPT_FULL_PICTURE,
							  wSendBlockNr++);
   
	DoConfirmJpegEncoding(PictRec, pPacket->dwProzessID, FALSE /*FALSE=Kein Sleep(0) im CIPC*/);
	
	// WK_STAT_PEAK("Reset", 1, "DoConfirmJpeg");
	int iSendQueueLength = GetSendQueueLength();
	if (iSendQueueLength>25)
	{
		WK_STAT_LOG("PictureSendQueue",iSendQueueLength,GetShmName());
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::IndicationVideoState(WORD wCamSubID, BOOL bVideoState)
{					  
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
EncoderState CIPCOutputTashaUnit::GetEncoderState()
{
	if (!m_pCodec)
		return eEncoderStateUnvalid;

	return m_pCodec->GetEncoderState();
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputTashaUnit::EncoderStart(CVideoJob *pCurrentJob)
{
	BOOL	bRet				= TRUE;

	if (!pCurrentJob)
	{
		WK_TRACE_ERROR("StartEncoder\tpJob==NULL\n");
		return FALSE;
	}

	if (!m_pCodec)
	{
		WK_TRACE_ERROR("StartEncoder\tm_pCodec==NULL\n");
		return FALSE;
	}

	// Alle notwendigen Daten aus dem aktuellen Videojob holen
	CSecID		camID				= pCurrentJob->m_camID;
	CSecID		camIDNext			= pCurrentJob->m_camIDNext;
	Resolution	res					= pCurrentJob->m_resolution;
	Compression	comp				= pCurrentJob->m_compression;

	if ((comp < COMPRESSION_1) || (comp > COMPRESSION_26))
	{
		WK_TRACE_ERROR("CIPCOutputTashaUnit::EncoderStart\tUnbekannte Compression (%u)\n", m_comp);
		return FALSE;
	}

	// res und comp werden hier für 'IndicationMpegDataReceived()' gespeichert
	m_res	= res;
	m_comp	= comp;

	// Auflösung festlegen
	switch (res)
	{
		case RESOLUTION_LOW:
				m_eImageRes = eImageResLow;
				break;
		case RESOLUTION_MID:
				m_eImageRes = eImageResHigh;
				break;
		case RESOLUTION_HIGH:
				m_eImageRes = eImageResHigh;
				break;
		default:
				WK_TRACE_ERROR("CIPCOutputTashaUnit::EncoderStart\tUnbekanntes Bildformat\n");	
				m_eImageRes = eImageResHigh;
				break;
	}

  // Encoder nur starten, wenn dieser noch nicht aktiv ist.
	if (m_pCodec->GetEncoderState() != eEncoderStateOn)
		bRet = m_pCodec->DoRequestStartCapture();

	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputTashaUnit::EncoderStop(CVideoJob* /*pJob*/)
{
	if (!m_pCodec)
		return FALSE;

	// Encoder nur stopen, wenn dieser aktiv.
	if (m_pCodec->GetEncoderState() == eEncoderStateOff)
	{
		WK_TRACE_WARNING("CIPCOutputTashaUnit::EncoderStop\tEncoder ist bereits off\n");
		return TRUE;
	}

	// Encoder Stoppen
	m_pCodec->DoRequestStopCapture();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnRequestSync(DWORD dwTickSend,DWORD dwType, DWORD dwUser)
{
	CVideoJob *pJob;
	pJob = new CVideoJob(dwTickSend,dwType,dwUser);	// VJA_SYNC

	if (pJob)
		pJob->m_pCipc = this;

	if (m_pMainWnd)
	{
		WK_TRACE(">>>>>>>>>> ReceiveSync TickSend=%lu Delta=%lu\n", pJob->m_dwSyncTick, WK_GetTickCount()-pJob->m_dwSyncTick);
		m_pMainWnd->GetJobQueueEncode(0).SafeAddTail(pJob);
	}
	else
	{
		WK_DELETE(pJob);
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnRequestGetMask(CSecID camID, DWORD dwUserID)
{
/* ToDo
	if (m_pCodec)
	{
		CIPCActivityMask mask(AM_PERMANENT);
		if (m_pCodec->GetMask(camID.GetSubID(), mask))
			DoConfirmGetMask(camID, dwUserID, mask);
	}
*/
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnRequestSetMask(CSecID camID, DWORD dwUserID, const CIPCActivityMask& mask)
{
/* ToDo
	if (m_pCodec)
	{
		if (m_pCodec->SetMask(camID.GetSubID(), mask))
			DoConfirmSetMask(camID, mask.GetType(), dwUserID);
	}
*/
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmGetBrightness(DWORD dwUserData, WORD wSource, int nValue)
{
	CString sValue;
	CSecID CId(m_wGroupID, wSource);

	sValue.Format("%d", nValue); 
	DoConfirmGetValue(CId, CSD_BRIGHTNESS, sValue, dwUserData);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmGetContrast(DWORD dwUserData, WORD wSource, int nValue)
{
	CString sValue;
	CSecID CId(m_wGroupID, wSource);

	sValue.Format("%d", nValue); 
	DoConfirmGetValue(CId, CSD_CONTRAST, sValue, dwUserData);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmGetSaturation(DWORD dwUserData, WORD wSource, int nValue)
{
	CString sValue;
	CSecID CId(m_wGroupID, wSource);

	sValue.Format("%d", nValue); 
	DoConfirmGetValue(CId, CSD_SATURATION, sValue, dwUserData);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmGetMDTreshold(DWORD dwUserData, WORD wSource, int nValue)
{
	CString sValue;
	CSecID CId(m_wGroupID, wSource);

	sValue.Format("%d", nValue); 
// TODO: Noch zu definieren	DoConfirmGetValue(CId, CSD_MD_TRESHOLD, sValue, dwUserData);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmGetMaskTreshold(DWORD dwUserData, WORD wSource, int nValue)
{
	CString sValue;
	CSecID CId(m_wGroupID, wSource);

	sValue.Format("%d", nValue); 
// TODO: Noch zu definieren	DoConfirmGetValue(CId, CSD_MASK_TRESHOLD, sValue, dwUserData);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmGetMaskIncrement(DWORD dwUserData, WORD wSource, int nValue)
{
	CString sValue;
	CSecID CId(m_wGroupID, wSource);

	sValue.Format("%d", nValue); 
// TODO: Noch zu definieren	DoConfirmGetValue(CId, CSD_MASK_INCREMENT, sValue, dwUserData);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmGetMaskDelay(DWORD dwUserData, WORD wSource, int nValue)
{
	CString sValue;
	CSecID CId(m_wGroupID, wSource);

	sValue.Format("%d", nValue); 
// TODO: Noch zu definieren	DoConfirmGetValue(CId, CSD_MASK_DELAY, sValue, dwUserData);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmSetBrightness(DWORD dwUserData, WORD wSource, int nValue)
{
	CString sValue;
	CSecID CId(m_wGroupID, wSource);

	sValue.Format("%d", nValue); 
	DoConfirmSetValue(CId, CSD_BRIGHTNESS, sValue, dwUserData);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmSetContrast(DWORD dwUserData, WORD wSource, int nValue)
{
	CString sValue;
	CSecID CId(m_wGroupID, wSource);

	sValue.Format("%d", nValue); 
	DoConfirmSetValue(CId, CSD_BRIGHTNESS, sValue, dwUserData);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmSetSaturation(DWORD dwUserData, WORD wSource, int nValue)
{
	CString sValue;
	CSecID CId(m_wGroupID, wSource);

	sValue.Format("%d", nValue); 
	DoConfirmSetValue(CId, CSD_BRIGHTNESS, sValue, dwUserData);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmSetMDTreshold(DWORD dwUserData, WORD wSource, int nValue)
{
	CString sValue;
	CSecID CId(m_wGroupID, wSource);

	sValue.Format("%d", nValue); 
// TODO: Noch zu definieren	DoConfirmSetValue(CId, CSD_MD_TRESHOLD, sValue, dwUserData);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmSetMaskTreshold(DWORD dwUserData, WORD wSource, int nValue)
{
	CString sValue;
	CSecID CId(m_wGroupID, wSource);

	sValue.Format("%d", nValue); 
// TODO: Noch zu definieren	DoConfirmSetValue(CId, CSD_MASK_TRESHOLD, sValue, dwUserData);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmSetMaskIncrement(DWORD dwUserData, WORD wSource, int nValue)
{
	CString sValue;
	CSecID CId(m_wGroupID, wSource);

	sValue.Format(_T("%d"), nValue); 
// TODO: Noch zu definieren	DoConfirmSetValue(CId, CSD_MASK_INCREMENT, sValue, dwUserData);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmSetMaskDelay(DWORD dwUserData, WORD wSource, int nValue)
{
	CString sValue;
	CSecID CId(m_wGroupID, wSource);

	sValue.Format(_T("%d"), nValue); 
// TODO: Noch zu definieren	DoConfirmSetValue(CId, CSD_MASK_DELAY, sValue, dwUserData);
}

//////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmClearPermanentMask(DWORD dwUserData, WORD wSource)
{
	CString sValue;
	CSecID CId(m_wGroupID, wSource);

// TODO: Noch zu definieren	DoConfirmSetValue(CId, CSD_CLEAR_PERM_MASK, _T(""), dwUserData);
}

//////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmInvertPermanentMask(DWORD dwUserData, WORD wSource)
{
	CString sValue;
	CSecID CId(m_wGroupID, wSource);

// TODO: Noch zu definieren	DoConfirmSetValue(CId, CSD_INVERT_PERM_MASK, _T(""), dwUserData);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmScanForCameras(DWORD dwUserData, DWORD dwCameraMask)
{
	CString sValue;

	sValue.Format("%lu", dwCameraMask); 
	
	CSecID CId(m_wGroupID, SECID_NO_SUBID);
	DoConfirmGetValue(CId, CSD_SCAN_FOR_CAMERAS, sValue, dwUserData);
}

/////////////////////////////////////////////////////////////////////////////
LPBITMAPINFO CIPCOutputTashaUnit::CreateDIB(const DATA_PACKET* pPacket)
{
	if (m_lpBMI)
	{
		m_lpBMI->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
		m_lpBMI->bmiHeader.biWidth			= pPacket->wSizeH;
		m_lpBMI->bmiHeader.biHeight			= pPacket->wSizeV;
		m_lpBMI->bmiHeader.biPlanes			= 1;
		m_lpBMI->bmiHeader.biBitCount		= pPacket->wBytesPerPixel*8; 
		m_lpBMI->bmiHeader.biCompression	= mmioFOURCC('Y','4','2','2');
		m_lpBMI->bmiHeader.biSizeImage		= pPacket->dwImageDataLen;

		memcpy(m_lpBMI->bmiColors, &pPacket->pImageData, pPacket->dwImageDataLen);
	}

	return m_lpBMI;
}
