/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaUnit
// FILE:		$Workfile: CipcOutputTashaUnit.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaUnit/CipcOutputTashaUnit.cpp $
// CHECKIN:		$Date: 14.12.01 12:40 $
// VERSION:		$Revision: 8 $
// LAST CHANGE:	$Modtime: 14.12.01 12:31 $
// BY AUTHOR:	$Author: Martin $
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

CIPCOutputTashaUnit::CIPCOutputTashaUnit(CTashaUnitDlg* pMainWnd, CCodec *pCodec,
							 int iType, const char *shmName)
	: CIPCOutput(shmName, FALSE)
{
	m_pCodec				= pCodec;						// Pointer auf CTasha-Objekt
	m_iType					= iType;						// OUTPUT_CAMERA, OUTPUT_RELAY
	m_wGroupID				= SECID_NO_GROUPID;				// Noch keine GroupID
	m_res					= RESOLUTION_NONE;				// Resolution
	m_comp					= COMPRESSION_NONE;				// Compression
	m_bOK					= TRUE;							// TRUE->Objekt ok
  	m_dwBPF					= 0;
	m_eImageRes				= eImageResUnvalid;
	m_pMainWnd				= pMainWnd;
	m_camID					= SECID_NO_ID;
	m_lpSmallTestbild		= NULL;							// Pointer auf kleines Störungsbild
	m_dwSmallTestbildLen	= 0;							// Länge des kleinen Störungsbildes
	m_lpLargeTestbild		= NULL;							// Pointer auf großes Störungsbild
	m_dwLargeTestbildLen	= 0;							// Länge des großen Störungsbildes

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

	// Gibt Auskunft darüber, ob Tasha Decoding unterstützt wird.
	// CWK_Dongle dongle;
	m_bAllowHardDecode = FALSE;//dongle.AllowHardDecodeMiCo();

	// Es gibt 2 Outputgruppen: a) Relays, b) Kameras
	if (iType == OUTPUT_TYPE_CAMERA)
	{
		WK_TRACE("Compression1=%lu Bytes/Frame\n", m_dwDefaultBPF[COMPRESSION_5]);
		WK_TRACE("Compression2=%lu Bytes/Frame\n", m_dwDefaultBPF[COMPRESSION_7]);
		WK_TRACE("Compression3=%lu Bytes/Frame\n", m_dwDefaultBPF[COMPRESSION_12]);
		WK_TRACE("Compression4=%lu Bytes/Frame\n", m_dwDefaultBPF[COMPRESSION_17]);
		WK_TRACE("Compression5=%lu Bytes/Frame\n", m_dwDefaultBPF[COMPRESSION_22]);
		
		// Kleines Testbild aus der Resource laden
/*		HRSRC hRc = FindResource(NULL, MAKEINTRESOURCE(BIN_SMALL_TESTBILD), "BINRES");
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
*/	
	}

	StartThread();
}

//////////////////////////////////////////////////////////////////////////////////////
CIPCOutputTashaUnit::~CIPCOutputTashaUnit()
{
	// TRACE("CIPCOutputTashaUnit::~CIPCOutputTashaUnit\n");
	m_bOK	= FALSE;

	StopThread();
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

	WORD wSource = CId.GetSubID(); // Kameranummer 0...15

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
	else if (m_iType == OUTPUT_TYPE_CAMERA)
	{
		if (m_pCodec)
			m_pCodec->DoRequestGetInputSource();
	}	
}

/////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmGetRelayState(DWORD dwRelayStateMask)
{
	DoConfirmCurrentState(m_wGroupID, dwRelayStateMask);
}

/////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnConfirmGetInputSource(WORD wSource)
{
	DWORD dwStateMask = 0;
	dwStateMask = SETBIT(0L, wSource);
	DoConfirmCurrentState(m_wGroupID, dwStateMask);
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
void CIPCOutputTashaUnit::OnRequestUnitJpegEncoding(WORD wGroupID,
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
		WK_TRACE_WARNING("CIPCOutputTashaUnit::OnRequestEncodedVideo\tcamID=SECID_NOID\n");
		return;
	}

	// WK_STAT_PEAK("Reset", 1, "OnRequestJpeg");

	CVideoJob *pJob;

	if (nNumPictures != 0)
	{
		// Wurde diese Kamera als defekt erkannt und liegt der Ausfall weniger als
		// 30 Sekunden zurück, dann verwerfe diesen Job und schicke statdessen ein
		// Jpeg-Testbild an den Server.
/*		if (m_pMainWnd && !m_pMainWnd->VideoTimeOutCheck(camID.GetSubID()))
		{
			DataPacket	Packet;

			Packet.wSource				= camID.GetSubID();
			if (res == RESOLUTION_LOW)
			{
				memcpy(&Packet.dwData, m_lpSmallTestbild, m_dwSmallTestbildLen);
				Packet.dwUserID			= dwUserID;
				Packet.dwDataLen		= m_dwSmallTestbildLen;
			}
			else
			{
				memcpy(&Packet.dwData, m_lpLargeTestbild, m_dwLargeTestbildLen);
				Packet.dwUserID			= dwUserID;
				Packet.dwDataLen		= m_dwLargeTestbildLen;
			}		
			Packet.eType			= eTypeJpeg;
			Packet.eSubType			= eSubTypeUnvalid;
			Packet.TimeStamp.bValid	= FALSE;
			Packet.bValid			= TRUE;

			IndicationDataReceived(&Packet,FALSE);
			return;
		}
*/
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
void CIPCOutputTashaUnit::IndicationDataReceived(const DataPacket* pPacket, BOOL bSignal)
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

	lpBuffer	= (char*)&pPacket->dwData;	// Pointer auf CodecDaten

	if ((pPacket->dwDataLen == 0L) || (lpBuffer == NULL))
		return;		  


	CSecID camID(m_wGroupID, pPacket->wSource);
	
	CIPCPictureRecord PictRec(this,
							 (const unsigned char*)lpBuffer,
							  pPacket->dwDataLen,
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
EncoderState CIPCOutputTashaUnit::GetEncoderState()
{
	if (!m_pCodec)
		return eEncoderStateUnvalid;

	return m_pCodec->GetEncoderState();
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::SelectCamera(CSecID camID, CSecID camIDNext, DWORD dwProzessID, DWORD dwPics)
{
	// WK_TRACE("CIPCOutputTashaUnit::SelectCamera\tGroupID=%i\tNr=%i\n", wGroupID, iNr);
	WORD	wNewSource		= 0;
	WORD	wGroupID		= 0;
	WORD	wSubID			= 0;
	
	wGroupID = camID.GetGroupID();
	wSubID	 = camID.GetSubID();

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_ERROR("CIPCOutputTashaUnit::SelectCamera Wrong wGroupID\n");

		DoIndicateError(0, camID, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	if (m_iType == OUTPUT_TYPE_CAMERA)
	{
		// Auf gewünschte Kamera umschalten
		wNewSource		= wSubID;			// Kameranummer 0...15
		
		if (m_pCodec)
		{	
			SOURCE_SELECTION SrcSelCurr;
			SOURCE_SELECTION SrcSelNext;

			SrcSelCurr.wSource		= wNewSource;
			SrcSelCurr.eResolution	= m_eImageRes;
			SrcSelCurr.dwBPF		= m_dwBPF;
			SrcSelCurr.dwProzessID	= dwProzessID;
			SrcSelCurr.dwPics		= dwPics;
			SrcSelCurr.wBrightness	= -1;
			SrcSelCurr.wContrast	= -1;
			SrcSelCurr.wSaturation	= -1;

			// Ist die nächste Kamera bekannt?
			if (camIDNext != SECID_NO_ID)
				SrcSelNext.wSource	= camIDNext.GetSubID();	
			else
				SrcSelNext.wSource	= VIDEO_SOURCE_NO;

			SrcSelNext.wBrightness	= -1;
			SrcSelNext.wContrast	= -1;
			SrcSelNext.wSaturation	= -1;

			m_pCodec->DoRequestSetInputSource(SrcSelCurr, SrcSelNext);
			
			m_camID = camID; 
		}
	}
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
	DWORD		dwProzessID			= pCurrentJob->m_dwUserData;
	int			nOutstandingPics	= pCurrentJob->m_iOutstandingPics;

	if ((comp < COMPRESSION_1) || (comp > COMPRESSION_26))
	{
		WK_TRACE_ERROR("CIPCOutputTashaUnit::EncoderStart\tUnbekannte Compression (%u)\n", m_comp);
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
				m_eImageRes = eImageResLow;
				m_dwBPF /= 2;
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

	// Setze Kamera, Bildformat, Framerate und die Bytes pro Frame des Encoders.
	SelectCamera(camID, camIDNext, dwProzessID, (DWORD)nOutstandingPics);

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
//		WK_TRACE(">>>>>ReceiveSync TickSend=%lu Delta=%lu\n",
//				  pJob->m_dwSyncTick, WK_GetTickCount()-pJob->m_dwSyncTick);
		m_pMainWnd->GetJobQueueEncode().SafeAddTail(pJob);
	}
	else
	{
		WK_DELETE(pJob);
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputTashaUnit::OnRequestGetMask(CSecID camID, DWORD dwUserID)
{
/*
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
/*
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
void CIPCOutputTashaUnit::OnConfirmScanForCameras(DWORD dwUserData, DWORD dwCameraMask)
{
	CString sValue;

	sValue.Format("%lu", dwCameraMask); 
	
	CSecID CId(m_wGroupID, SECID_NO_SUBID);
	DoConfirmGetValue(CId, CSD_SCAN_FOR_CAMERAS, sValue, dwUserData);
}

