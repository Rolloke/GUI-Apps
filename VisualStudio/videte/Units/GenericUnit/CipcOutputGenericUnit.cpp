/////////////////////////////////////////////////////////////////////////////
// PROJECT:		GenericUnit
// FILE:		$Workfile: CipcOutputGenericUnit.cpp $
// ARCHIVE:		$Archive: /Project/Units/GenericUnit/CipcOutputGenericUnit.cpp $
// CHECKIN:		$Date: 19.10.00 8:46 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 19.10.00 8:26 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GenericUnitDlg.h"
#include "CIPCOutputGenericUnit.h"
#include "CIPCExtraMemory.h"
#include "CIPCStringDefs.h"
#include "VideoJob.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

CIPCOutputGenericUnit::CIPCOutputGenericUnit(CGenericUnitDlg* pMainWnd, CCodec *pCodec,
							 int iType, const char *shmName,
							 const CString &sAppIniName)
	: CIPCOutput(shmName, FALSE)
{
	m_pCodec			= pCodec;						// Pointer auf CGeneric-Objekt
	m_iType				= iType;						// OUTPUT_CAMERA, OUTPUT_RELAY
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
	m_dwLargeTestbildLen	= 0;						// Länge des großen Störungsbildes
	// Gibt Auskunft darüber, ob Generic Decoding unterstützt wird.
	// CWK_Dongle dongle;

	// Es gibt 2 Outputgruppen: a) Relays, b) Kameras
	if (iType == OUTPUT_TYPE_GENERIC_CAMERA)
	{
		WK_TRACE("Compression1=%lu Bytes/Frame\n", m_dwDefaultBPF[COMPRESSION_5]);
		WK_TRACE("Compression2=%lu Bytes/Frame\n", m_dwDefaultBPF[COMPRESSION_7]);
		WK_TRACE("Compression3=%lu Bytes/Frame\n", m_dwDefaultBPF[COMPRESSION_12]);
		WK_TRACE("Compression4=%lu Bytes/Frame\n", m_dwDefaultBPF[COMPRESSION_17]);
		WK_TRACE("Compression5=%lu Bytes/Frame\n", m_dwDefaultBPF[COMPRESSION_22]);
		
		// Kleines Testbild aus der Resource laden
		HRSRC hRc = FindResource(NULL, MAKEINTRESOURCE(BIN_SMALL_TESTBILD), "BINRES");
		if (hRc)
		{
			HGLOBAL hJpegResource	= LoadResource(NULL, hRc);
			if (hJpegResource)
			{
				m_dwSmallTestbildLen	= SizeofResource(NULL, hRc);
				m_lpSmallTestbild		= (char*)LockResource(hJpegResource);
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
				m_lpLargeTestbild		= (char*)LockResource(hJpegResource);
			}
			else
				WK_TRACE_ERROR("Großes Testbild konnte nicht geladen werden\n");
		}
		else
			WK_TRACE_ERROR("Testbild konnte nicht geladen werden\n");
	
	}

	StartThread();
}

//////////////////////////////////////////////////////////////////////////////////////
CIPCOutputGenericUnit::~CIPCOutputGenericUnit()
{
	// TRACE("CIPCOutputGenericUnit::~CIPCOutputGenericUnit\n");
	m_bOK	= FALSE;

	StopThread();
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputGenericUnit::IsValid()
{
	return m_bOK;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputGenericUnit::OnRequestGetValue(CSecID CId, const CString &sKey, DWORD dwUserData)
{
	CString sValue = "";

	if (CId.GetGroupID() != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCOutputGenericUnit::OnRequestGetValue\tWrong GroupID\n");
		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	WORD wSource = CId.GetSubID(); // Kameranummer 0...15

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
	
	DoConfirmGetValue(CId, sKey, sValue, dwUserData);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputGenericUnit::OnRequestSetValue(CSecID CId, const CString &sKey, const CString &sValue, DWORD dwUserData)
{
	if (CId.GetGroupID() != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCOutputGenericUnit::OnRequestSetValue\tWrong GroupID\n");
		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	WORD wSource = CId.GetSubID(); // Kameranummer 0...15

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

	DoConfirmSetValue(CId, sKey, sValue, dwUserData);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputGenericUnit::OnRequestVersionInfo(WORD wGroupID)
{
	DoConfirmVersionInfo(wGroupID, 3);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputGenericUnit::OnRequestReset(WORD wGroupID)	
{
	WK_TRACE("CIPCOutputGenericUnit::OnRequestReset\tGroupID=%i\n", wGroupID);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCOutputGenericUnit::OnRequestReset\tWrong wGroupID\n");

		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	if (m_pMainWnd)
		m_pMainWnd->RequestOutputReset();

	DoConfirmReset(m_wGroupID);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputGenericUnit::OnRequestSetRelay(CSecID relayID, BOOL bClosed)
{
 	if (relayID.GetGroupID() != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCOutputGenericUnit::OnRequestSetRelay\tWrong wGroupID\n");

		DoIndicateError(0, relayID, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	if (m_iType != OUTPUT_TYPE_GENERIC_RELAY)
	{
		WK_TRACE_ERROR("CIPCOutputGenericUnit::OnRequestSetRelay\tWrong Grouptype\n");
		return;
	}

	DWORD	dwOldState	= 0L;
	DWORD	dwNewState	= 0L;;
	int		iNr			= relayID.GetSubID();
	
	// Bisherigen Relaisstatus holen.
	dwOldState = GetAllRelaisState();

	if (bClosed)
		dwNewState = SETBIT(dwOldState, iNr); // Relais ein
	else
		dwNewState = CLRBIT(dwOldState, iNr); // Relais aus
	
	// Relais setzen
	SetAllRelaisState(dwNewState);

	// Und Server mitteilen.
	DoConfirmSetRelay(relayID, bClosed);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputGenericUnit::OnRequestCurrentState(WORD wGroupID)
{
	WORD	wSourceNr	= 0;
	DWORD	dwStateMask	= 0L;

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCOutputGenericUnit::OnRequestCurrentState\tWrong wGroupID\n");
		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	if (m_iType == OUTPUT_TYPE_GENERIC_RELAY)
	{
		dwStateMask = GetAllRelaisState();
	}	
	else if (m_iType == OUTPUT_TYPE_GENERIC_CAMERA)
	{
		// Bisherige Kameraauswahl holen
		if (m_pCodec)
			wSourceNr = m_pCodec->GetInputSource();

		dwStateMask = SETBIT(0L, wSourceNr);
	}	
	DoConfirmCurrentState(m_wGroupID, dwStateMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputGenericUnit::OnRequestSetGroupID(WORD wGroupID)
{
	m_wGroupID = wGroupID;
	DoConfirmSetGroupID(m_wGroupID);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputGenericUnit::OnRequestHardware(WORD wGroupID)
{
	int iErrorCode = 0;

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCOutputGenericUnit::OnRequestHardware\tWrong wGroupID\n");

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

//////////////////////////////////////////////////////////////////////////////////////
DWORD CIPCOutputGenericUnit::GetAllRelaisState()
{
	DWORD dwStateMask	= 0L;

	if (m_pCodec)
		dwStateMask = (DWORD)m_pCodec->GetRelais();

	return dwStateMask;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputGenericUnit::SetAllRelaisState(DWORD dwStateMask)
{
	if (m_pCodec)
		m_pCodec->SetRelais((BYTE)(dwStateMask & 0x0000000f));
}

//////////////////////////////////////////////////////////////////////////
void CIPCOutputGenericUnit::OnRequestDisconnect()
{
 	if (m_pMainWnd)
		m_pMainWnd->ClearAllJobs();
}

//////////////////////////////////////////////////////////////////////////
void CIPCOutputGenericUnit::OnRequestUnitJpegEncoding(WORD wGroupID,
												   BYTE byCam,
												   BYTE byNextCam,
												   Resolution res,
												   Compression comp,
												   int nNumPictures,
												   DWORD dwUserID)
{
	CSecID camID(wGroupID, byCam);
	CSecID camIDNext;

	if (byNextCam != 255)
		camIDNext.Set(wGroupID, byNextCam);
	
	if (camID == SECID_NO_ID)
	{
		WK_TRACE_WARNING("CIPCOutputGenericUnit::OnRequestEncodedVideo\tcamID=SECID_NOID\n");
		return;
	}

	// WK_STAT_PEAK("Reset", 1, "OnRequestJpeg");

	CVideoJob *pJob;

	if (nNumPictures != 0)
	{
		// EncoderStart
		pJob = new CVideoJob(camID, camIDNext, res, comp, COMPRESSION_COLORJPEG, dwUserID, 0, 0);
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
		m_pMainWnd->GetJobQueueEncode().SafeAddTail(pJob);
	}
	else
	{
		WK_DELETE(pJob);
	}
}

//////////////////////////////////////////////////////////////////////////
void CIPCOutputGenericUnit::OnRequestIdlePictures(int iNumPics)
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
		m_pMainWnd->GetJobQueueEncode().SafeAddTail(pJob);
	}
	else
	{
		WK_DELETE(pJob);
	}
}

//////////////////////////////////////////////////////////////////////////
void CIPCOutputGenericUnit::IndicationDataReceived(const JPEG* pJpeg, BOOL bSignal)
{
	static  WORD wSendBlockNr	= 0;
	LPCSTR	lpBuffer			= NULL;
	CSystemTime	TimeStamp;

	if (!pJpeg)
		return;

	lpBuffer	= (char*)pJpeg->Buffer.pLinAddr;	// Pointer auf Jpegdaten

	// Uhrzeit gültig?
	if (pJpeg->TimeStamp.bValid)
	{
		TimeStamp.wYear			= pJpeg->TimeStamp.wYear;
		TimeStamp.wMonth		= pJpeg->TimeStamp.wMonth,
		TimeStamp.wDay			= pJpeg->TimeStamp.wDay,
	  	TimeStamp.wHour			= pJpeg->TimeStamp.wHour,
		TimeStamp.wMinute		= pJpeg->TimeStamp.wMinute,
		TimeStamp.wSecond		= pJpeg->TimeStamp.wSecond;
		TimeStamp.wMilliseconds = pJpeg->TimeStamp.wMSecond;
		TimeStamp.wDayOfWeek	= 0;
	}
	else
	{
		TimeStamp.GetLocalTime();
	}

	if ((pJpeg->dwLen == 0L) || (lpBuffer == NULL))
		return;		  

	CSecID camID(m_wGroupID, pJpeg->wSource);
	
	CIPCPictureRecord PictRec(this,
							 (const unsigned char*)lpBuffer,
							  pJpeg->dwLen,
							  camID,
							  m_res,
							  m_comp,
							  COMPRESSION_COLORJPEG,
							  TimeStamp,
							  bSignal,
							  NULL,
							  0,
							  SPT_FULL_PICTURE,
							  wSendBlockNr++);
   
	DoConfirmJpegEncoding(PictRec, pJpeg->dwProzessID, FALSE /*FALSE=Kein Sleep(0) im CIPC*/);
	// WK_STAT_PEAK("Reset", 1, "DoConfirmJpeg");
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputGenericUnit::IndicationVideoState(WORD wCamSubID, BOOL bVideoState)
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
int CIPCOutputGenericUnit::GetEncoderState()
{
	if (!m_pCodec)
		return ENCODER_OFF;

	return m_pCodec->GetEncoderState();
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputGenericUnit::SelectCamera(CSecID camID, CSecID camIDNext, DWORD dwProzessID, DWORD dwPics)
{
	// WK_TRACE("CIPCOutputGenericUnit::SelectCamera\tGroupID=%i\tNr=%i\n", wGroupID, iNr);
	WORD	wNewSource		= 0;
	WORD	wGroupID		= 0;
	WORD	wSubID			= 0;
	
	wGroupID = camID.GetGroupID();
	wSubID	 = camID.GetSubID();

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCOutputGenericUnit::SelectCamera Wrong wGroupID\n");

		DoIndicateError(0, camID, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	if (m_iType == OUTPUT_TYPE_GENERIC_CAMERA)
	{
		// Auf gewünschte Kamera umschalten
		wNewSource		= wSubID;			// Kameranummer 0...15
		
		if (m_pCodec)
		{
			SOURCE_SELECT_STRUCT SrcSelCurr;
			SOURCE_SELECT_STRUCT SrcSelNext;

			SrcSelCurr.wSource		= wNewSource;
			SrcSelCurr.wFormat		= m_wFormat;
			SrcSelCurr.dwBPF		= m_dwBPF;
			SrcSelCurr.dwProzessID	= dwProzessID;
			SrcSelCurr.dwPics		= dwPics;

			// Ist die nächste Kamera bekannt?
			if (camIDNext != SECID_NO_ID)
				SrcSelNext.wSource	= camIDNext.GetSubID();	
			else
				SrcSelNext.wSource	= VIDEO_SOURCE_NO;

			m_pCodec->SetInputSource(SrcSelCurr, SrcSelNext);
			
			m_camID = camID; 
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputGenericUnit::EncoderStart(CVideoJob *pCurrentJob)
{
	BOOL	bRet				= TRUE;
	CSecID	camIDNext			= SECID_NO_ID;

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
	Resolution	res					= pCurrentJob->m_resolution;
	Compression	comp				= pCurrentJob->m_compression;
	DWORD		dwProzessID			= pCurrentJob->m_dwUserData;

	// Kamera, auf die als nächstes umgeschaltet werden soll.
	camIDNext = pCurrentJob->m_camIDNext;

	if ((comp < COMPRESSION_1) || (comp > COMPRESSION_26))
	{
		WK_TRACE_ERROR("CIPCOutputGenericUnit::EncoderStart\tUnbekannte Compression (%u)\n", m_comp);
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
				WK_TRACE_ERROR("CIPCOutputGenericUnit::EncoderStart\tUnbekanntes Bildformat\n");	
				m_wFormat = ENCODER_RES_HIGH;
				break;
	}
	// Encoder nur starten, wenn dieser noch nicht aktiv ist.
	if (m_pCodec->GetEncoderState() != ENCODER_ON)
		bRet = m_pCodec->EncoderStart();

	// Setze Kamera, Bildformat, Framerate und die Bytes pro Frame des Encoders.
	SelectCamera(camID, camIDNext, dwProzessID, (DWORD)pCurrentJob->m_iOutstandingPics);

	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputGenericUnit::EncoderStop(CVideoJob* /*pJob*/)
{
	if (!m_pCodec)
		return FALSE;

	// Encoder nur stopen, wenn dieser aktiv.
	if (m_pCodec->GetEncoderState() == ENCODER_OFF)
	{
		WK_TRACE_WARNING("CIPCOutputGenericUnit::EncoderStop\tEncoder ist bereits off\n");
		return TRUE;
	}

	// Encoder Stoppen
	m_pCodec->EncoderStop();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputGenericUnit::OnRequestSync(DWORD dwTickSend,DWORD dwType, DWORD dwUser)
{
	CVideoJob *pJob;
	pJob = new CVideoJob(dwTickSend,dwType,dwUser);	// VJA_SYNC

	if (pJob)
		pJob->m_pCipc = this;

	if (m_pMainWnd)
	{
//		WK_TRACE(">>>>>ReceiveSync TickSend=%lu Delta=%lu\n",
//				  pJob->m_dwSyncTick, WK_GetTickCount()-pJob->m_dwSyncTick);
		m_pMainWnd->GetJobQueueEncode().SafeAddTail(pJob);
	}
	else
	{
		WK_DELETE(pJob);
	}
}
