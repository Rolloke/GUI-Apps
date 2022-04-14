/////////////////////////////////////////////////////////////////////////////
// PROJECT:		CoCoUnit
// FILE:		$Workfile: CipcOutputCoCoUnit.cpp $
// ARCHIVE:		$Archive: /Project/Units/CoCoUnit/CipcOutputCoCoUnit.cpp $
// CHECKIN:		$Date: 29.11.02 8:59 $
// VERSION:		$Revision: 42 $
// LAST CHANGE:	$Modtime: 29.11.02 8:56 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "win16_32.h"
#include "CoCoUnitDlg.h"
#include "CIPCOutputCoCoUnit.h"
#include "CIPCExtraMemory.h"
#include "CIPCStringDefs.h"
#include "CCoCo.h"
#include "CMegra.h"
#include "VideoJob.h"
#include "wk_util.h"
#include "WK_RuntimeError.h"
#include "WK_Dongle.h"

#define	B0	0xFF000000
#define	B1	0x00FF0000
#define	B2	0x0000FF00
#define	B3	0x000000FF

#define SwapEndian(x)   \
	((((x)<<24)&B0)|(((x)<<8)&B1)|(((x)>>8)&B2)|(((x)>>24)&B3))

CIPCOutputCoCoUnit::CIPCOutputCoCoUnit(CCoCoUnitDlg* pMainWnd, CCoCo *pCoCo,
							 CMegra *pMegra, BYTE byCoCoID,
							 int iType, const char *shmName)
	: CIPCOutput(shmName, FALSE)
{
	CWK_Dongle dongle;
//	TRACE("CIPCOutputCoCoUnit::CIPCOutputCoCoUnit\n");
	m_pCoCo				= pCoCo;				// Pointer auf CoCo-Objekt
	m_pMegra			= pMegra;				// Pointer auf Megra-Objekt
	m_byCoCoID			= byCoCoID;				// CoCoID (COCO_COCOID0...COCO_COCOID3)
	m_iType				= iType;				// OUTPUT_CAMERA, OUTPUT_RELAY
	m_wGroupID			= SECID_NO_GROUPID;		// Noch keine GroupID
	m_res				= RESOLUTION_NONE;		// Resolution
	m_comp				= COMPRESSION_NONE;		// Compression
	m_dwBitrateScale	= 1000;					
	m_bOK				= TRUE;					// TRUE->Objekt ok
	m_dwHardwareState	= m_pCoCo->GetState();	// Hardwarekonfiguration
  	m_dwEncBitrate		= 64000L;
	m_dwDecBitrate		= 64000L;
	m_wMPI				= 0;
	m_wFormat			= COCO_ENCODER_NO_FORMAT;
	m_pMainWnd			= pMainWnd;
	m_camID				= SECID_NO_ID;
	m_bAllowHardDecode  = dongle.AllowHardDecodeCoCo();

	// HEDU use CTimeMessage
	m_tmInvalidEncoderResolutionInIndication.SetDelta(1000*10);	// alle 10 Sekunden soll reichen HEDU

	StartThread();
}

//////////////////////////////////////////////////////////////////////////////////////
CIPCOutputCoCoUnit::~CIPCOutputCoCoUnit()
{
	// TRACE("CIPCOutputCoCoUnit::~CIPCOutputCoCoUnit\n");
	m_bOK	= FALSE;
	StopThread();
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputCoCoUnit::IsValid()
{
	return m_bOK;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputCoCoUnit::OnRequestGetValue(CSecID CId, const CString &sKey, DWORD dwUserData)
{

	CString sValue = "";

	if (CId.GetGroupID() != m_wGroupID)
	{
		WK_TRACE_WARNING("CIPCOutputCoCoUnit::OnRequestGetValue\tWrong GroupID\n");
		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	if (sKey == CSD_BRIGHTNESS)
	{
		if (m_pCoCo)
			sValue.Format("%u", m_pCoCo->GetFeBrightness()); 
	}
	else if (sKey == CSD_CONTRAST)
	{
		if (m_pCoCo)
			sValue.Format("%u", m_pCoCo->GetFeContrast()); 
	}
	else if (sKey == CSD_SATURATION)
	{
		if (m_pCoCo)
			sValue.Format("%u", m_pCoCo->GetFeSaturation()); 
	}
	else if (sKey == CSD_MAX_OVERLAY_RECT)
	{
		if (m_pMegra && m_pMegra->IsValid())
		{
			RECT Rect;
			m_pMegra->GetAcqWindow(&Rect);

			if (m_pCoCo)
				sValue = RectToString(Rect);
		}
	}
	else if (sKey == CSD_COLORKEY)
	{
		if (m_pMegra && m_pMegra->IsValid())
		{
			COLORREF Col = RGB(255,0,255); // OOPS Keyfarbe fest eingestellt. m_pMegra->GetColorKey();
			sValue.Format("%08lx", (DWORD)Col);
//			WK_TRACE("ColorKey=%lu\n",(DWORD)Col);
		}	
	}

	DoConfirmGetValue(CId, sKey, sValue, dwUserData);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputCoCoUnit::OnRequestSetValue(CSecID CId, const CString &sKey, const CString &sValue, DWORD dwUserData)
{
	if (CId.GetGroupID() != m_wGroupID)
	{
		WK_TRACE_WARNING("CIPCOutputCoCoUnit::OnRequestSetValue\tWrong GroupID\n");
		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	if (sKey == CSD_BRIGHTNESS)
	{
		if (m_pCoCo)
			m_pCoCo->SetFeBrightness(atoi(sValue));
	}
	else if (sKey == CSD_CONTRAST)
	{
		if (m_pCoCo)
			m_pCoCo->SetFeContrast(atoi(sValue));
	}
	else if (sKey == CSD_SATURATION)
	{
		if (m_pCoCo)
			m_pCoCo->SetFeSaturation(atoi(sValue));
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputCoCoUnit::OnRequestVersionInfo(WORD wGroupID)
{
	// 2 Neues Job-handling mit EncodedVideo
	// 3 SyncEvent für CoCo und Mico haben unterschiedliche Namen
	DoConfirmVersionInfo(wGroupID, 3);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputCoCoUnit::OnRequestReset(WORD wGroupID)	
{
//	TRACE("CIPCOutputCoCoUnit::OnRequestReset\tGroupID=%i\n", wGroupID);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_WARNING("CIPCOutputCoCoUnit::OnRequestReset\tWrong wGroupID\n");

		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	if (m_pMainWnd)
		m_pMainWnd->ClearAllJobs();

	// Auf 1. Kamera umschalten.
	CSecID camID(m_wGroupID, COCO_SOURCE0);
	
	SelectCamera(camID);

	DoConfirmReset(m_wGroupID);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputCoCoUnit::OnRequestSetRelay(CSecID relayID, BOOL bClosed)
{
/*	WK_TRACE("CIPCOutputCoCoUnit::OnRequestSetRelay\tGroupID=%i\tNr=%i\t%i\n",
		   relayID.GetGroupID(), relayID.GetSubID(), (int)bClosed);
*/
 	if (relayID.GetGroupID() != m_wGroupID)
	{
		WK_TRACE_WARNING("CIPCOutputCoCoUnit::OnRequestSetRelay\tWrong wGroupID\n");

		DoIndicateError(0, relayID, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	if (m_iType != OUTPUT_TYPE_COCO_RELAY)
	{
		WK_TRACE_WARNING("CIPCOutputCoCoUnit::OnRequestSetRelay\tWrong OutputType\n");
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
void CIPCOutputCoCoUnit::OnRequestCurrentState(WORD wGroupID)
{
//	TRACE("CIPCOutputCoCoUnit::OnRequestCurrentState\tGroupID=%i\n", wGroupID);

	DWORD	dwRet		= 0L;
	WORD	wExtCard	= 0;
	WORD	wSource		= 0;
	WORD	wSourceNr	= 0;
	DWORD	dwStateMask	= 0L;

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_WARNING("CIPCOutputCoCoUnit::OnRequestCurrentState\tWrong wGroupID\n");
		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	if (m_iType == OUTPUT_TYPE_COCO_RELAY)
	{
		dwStateMask = GetAllRelaisState();
	}	
	else if (m_iType == OUTPUT_TYPE_COCO_CAMERA)
	{
		// Bisherige Kameraauswahl holen
		if (m_pCoCo)
			dwRet	= m_pCoCo->GetInputSource(m_byCoCoID);
		wExtCard	= HIWORD(dwRet);  // Erweiterungskarte 0...3
		wSource		= LOWORD(dwRet);  // Kameranummer 0...8
		wSourceNr	= (COCO_MAX_SOURCE + 1) * wExtCard + wSource;

		dwStateMask = SETBIT(0L, wSourceNr);
	}	
	else if (m_iType == OUTPUT_TYPE_MEGRA_CAMERA)
	{
		if (m_pMegra && m_pMegra->IsValid())
			wSourceNr	= m_pMegra->GetInputSource();
		dwStateMask		= SETBIT(0L, wSourceNr);
	}

	DoConfirmCurrentState(m_wGroupID, dwStateMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputCoCoUnit::OnRequestSetGroupID(WORD wGroupID)
{
	m_wGroupID = wGroupID;
	DoConfirmSetGroupID(m_wGroupID);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputCoCoUnit::OnRequestHardware(WORD wGroupID)
{
//	TRACE("CIPCOutputCoCoUnit::OnRequestHardware\tGroupID=%i\n", wGroupID);

	int iErrorCode = 0;

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_WARNING("CIPCOutputCoCoUnit::OnRequestHardware\tWrong wGroupID\n");

		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	if (m_pCoCo && m_pCoCo->CheckHardware(m_byCoCoID))
		iErrorCode = 0;
	else
	{
		WK_TRACE_ERROR("CIPCOutputCoCoUnit::OnRequestHardware\tCheckHardware failed\n");
		iErrorCode = 1;
	}

	DoConfirmHardware(m_wGroupID, iErrorCode,
									FALSE, /* No SW-Compress */
									FALSE, /* No SW-DeCompress */
									TRUE,	 /* Co-Compress */
									TRUE,	 /* Co-DeCompress */
									TRUE); /* Overlay */
}

//////////////////////////////////////////////////////////////////////////////////////
DWORD CIPCOutputCoCoUnit::GetAllRelaisState()
{
	DWORD dwBit			= m_byCoCoID * 4;	// m_byCoCoID = 0...3
	DWORD dwStateMask	= 0L;

	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pCoCo)
			dwStateMask |= (DWORD)m_pCoCo->GetRelais(m_byCoCoID, COCO_EXTCARD0);
	}
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pCoCo)
			dwStateMask |= (DWORD)m_pCoCo->GetRelais(m_byCoCoID, COCO_EXTCARD1)<<4;
	}
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pCoCo)
			dwStateMask |= (DWORD)m_pCoCo->GetRelais(m_byCoCoID, COCO_EXTCARD2)<<8;
	}	
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pCoCo)
			dwStateMask |= (DWORD)m_pCoCo->GetRelais(m_byCoCoID, COCO_EXTCARD3)<<12;
	}

	return dwStateMask;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputCoCoUnit::SetAllRelaisState(DWORD dwStateMask)
{
	DWORD dwBit = m_byCoCoID * 4;	// m_byCoCoID = 0...3

	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pCoCo)
			m_pCoCo->SetRelais(m_byCoCoID, COCO_EXTCARD0, (WORD)(dwStateMask>>0  & 0x0000000f));
	}
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pCoCo)
			m_pCoCo->SetRelais(m_byCoCoID, COCO_EXTCARD1, (WORD)(dwStateMask>>4  & 0x0000000f));
	}	
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pCoCo)
			m_pCoCo->SetRelais(m_byCoCoID, COCO_EXTCARD2, (WORD)(dwStateMask>>8  & 0x0000000f));
	}	
	if (TSTBIT(m_dwHardwareState, dwBit++))
	{
		if (m_pCoCo)
			m_pCoCo->SetRelais(m_byCoCoID, COCO_EXTCARD3, (WORD)(dwStateMask>>12 & 0x0000000f));
	}
}

void CIPCOutputCoCoUnit::OnRequestDisconnect()
{
	CVideoJob dummy;
	EncoderStop(&dummy);	// OOPS HEDU, job not used
}

//////////////////////////////////////////////////////////////////////////
void CIPCOutputCoCoUnit::OnRequestSetDisplayWindow(WORD wGroupID, const CRect &Rect)
{
//	WK_TRACE("CIPCOutputCoCoUnit::OnRequestSetDisplayWindow Left=%u, Right=%u, Top=%u, Bottom=%u\n", Rect.left,
//					Rect.right, Rect.top, Rect.bottom);

	/* Not Yet
	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_WARNING("CIPCOutputCoCoUnit::OnRequestSetDisplayWindow\tWrong wGroupID\n");

		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}
	*/

	if (m_pMegra && m_pMegra->IsValid())
	{
 		m_pMegra->SetDisplayWindow((const LPRECT)&Rect);
		DoConfirmSetDisplayWindow(wGroupID);
	}
}
		 
//////////////////////////////////////////////////////////////////////////
void CIPCOutputCoCoUnit::OnRequestSetOutputWindow(WORD wGroupID, const CRect &Rect, OverlayDisplayMode odm)
{
//	WK_TRACE("CIPCOutputCoCoUnit::OnRequestSetOutputWindow Left=%u, Right=%u, Top=%u, Bottom=%u\n", Rect.left,
//					Rect.right, Rect.top, Rect.bottom);

/*
	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_WARNING("CIPCOutputCoCoUnit::OnRequestSetOutputWindow\tWrong wGroupID\n");
		
		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}
*/
	switch (odm)
	{
		case ODM_LOCAL_VIDEO:
			if (m_pCoCo)
				m_pCoCo->ConnectVideoOutput(m_byCoCoID, COCO_PREP);
			break;
		case ODM_DECODE:
			if (m_pCoCo)
				m_pCoCo->ConnectVideoOutput(m_byCoCoID, COCO_DECODER);
			break;
		case ODM_ENCODE:
			if (m_pCoCo)
				m_pCoCo->ConnectVideoOutput(m_byCoCoID, COCO_ENCODER);
			break;

		default:
			WK_TRACE_ERROR("CIPCOutputCoCoUnit::OnRequestSetOutputWindow\tUnbekannter OverlayDisplayMode (%d)\n", odm); 
	}

	if (m_pMegra && m_pMegra->IsValid())
	{
		m_pMegra->Freeze(TRUE);
		Sleep(40);
		m_pMegra->SetOutputWindow((const LPRECT)&Rect);
		m_pMegra->Freeze(FALSE);
		DoConfirmSetOutputWindow(wGroupID, 0);
	}
}

//////////////////////////////////////////////////////////////////////////
void CIPCOutputCoCoUnit::OnRequestEncodedVideo(CSecID camID, Resolution  res, Compression comp, WORD wNumPictures,
							DWORD dwBitrateScale, DWORD dwUserData)
{
	CVideoJob *pJob;
	if (camID!=SECID_NO_ID)
	{
		if (wNumPictures != 0)
		{
//			WK_TRACE("OnRequestEncodedVideo\tEncodeStart\tOutstandigPict=%u\n", wNumPictures); 

			// EncoderStart
			pJob = new CVideoJob(camID, res, comp, COMPRESSION_H263, dwUserData, dwBitrateScale, 0);
			if (pJob)
			{
				pJob->m_pCipc = this;
				pJob->m_iOutstandingPics = (int)wNumPictures;

				if (m_pMainWnd)
					m_pMainWnd->GetJobQueueEncode().SafeAddTail(pJob);
				else
					WK_DELETE(pJob);
			}
		}
		else
		{
//			WK_TRACE("OnRequestEncodedVideo\tCancelCurrentEncodeJob\n"); 
			if (m_pMainWnd)
				m_pMainWnd->ClearCurrentEncodeJob("CancelCurrentEncodeJob");
		}
	}
	else
	{
		WK_TRACE_WARNING("OnRequestEncodedVideo\tOnRequestEncodedVideo with invalid cam ID");
	}
}

//////////////////////////////////////////////////////////////////////////
void CIPCOutputCoCoUnit::OnRequestH263Decoding(WORD wGroupID, 
											   const CRect &rect, 
											   const CIPCPictureRecord &Pict,
											   DWORD dwUserData)
{
	CVideoJob *pJob;
	pJob = new CVideoJob(rect, Pict, dwUserData);
	if (pJob)
	{
		pJob->m_pCipc = this;

		if (m_pMainWnd)
			m_pMainWnd->GetJobQueueDecode().SafeAddTail(pJob);
		else
			WK_DELETE(pJob);
	}
}

//////////////////////////////////////////////////////////////////////////
void CIPCOutputCoCoUnit::OnRequestStopH263Decoding(WORD wGroupID)
{
	CVideoJob *pJob;
	pJob = new CVideoJob(SECID_NO_ID, VJA_STOP_DECODE);
	if (pJob)
	{
		pJob->m_pCipc = this;

		if (m_pMainWnd)
			m_pMainWnd->GetJobQueueDecode().SafeAddTail(pJob);
		else
			WK_DELETE(pJob);
	}
}

//////////////////////////////////////////////////////////////////////////
void CIPCOutputCoCoUnit::OnRequestLocalVideo(CSecID camID, Resolution res, const CRect &outputRect, BOOL bActive)
{
	CVideoJob *pJob;

	pJob = new CVideoJob(camID, outputRect);
	if (pJob)
	{
		pJob->m_pCipc = this;
		if (m_pMainWnd)
			m_pMainWnd->GetJobQueueEncode().SafeAddTail(pJob);
		else
			WK_DELETE(pJob);
	}
}

//////////////////////////////////////////////////////////////////////////
void CIPCOutputCoCoUnit::OnRequestDumpOutputWindow(WORD wGroupID, WORD wUserID,	const CRect &outputRect, int iFormat)
{
	HGLOBAL hBitmap = NULL;
	DWORD	dwLen	= 0L;
	char*	pBitmap = NULL;

	if ((m_pMegra) && (m_pMegra->IsValid()))
		m_pMegra->GetBitmap(DIB8BIT, outputRect, hBitmap, dwLen);

	
	// OOPS die Datenmenge scheint bei DIB8BIT nur halb so groß zu sein!?
	dwLen /= 2L;

	
	if (!hBitmap || dwLen == 0L)
	{
		WK_TRACE_ERROR("CIPCOutputCoCoUnit::OnRequestDumpOutputWindow\thBimap=0 oder dwLen=0\n");
		return;
	}

	pBitmap = (char*)GlobalLock(hBitmap);

	if (!pBitmap)
	{
		WK_TRACE_ERROR("CIPCOutputCoCoUnit::OnRequestDumpOutputWindow\tlpBitmap=0\n");
		GlobalFree(hBitmap);
		return;
	}

	// Einbubbeln
	CIPCExtraMemory	bubble;
	bubble.Create(this, dwLen, pBitmap);
	
	GlobalUnlock(hBitmap);
	GlobalFree(hBitmap);

 	DoConfirmDumpOutputWindow(wGroupID, wUserID, iFormat, bubble);
}

//////////////////////////////////////////////////////////////////////////
void CIPCOutputCoCoUnit::IndicationDataReceived(WORD wExtCard, WORD wSource, LPCSTR lpData, DWORD dwLen, DWORD dwUserData, BOOL bIntra)
{
	static WORD wSendBlockNr = 0;

	if ((dwLen == 0L) || (lpData == NULL))
	{
		WK_TRACE_ERROR("CIPCOutputCoCoUnit::IndicationDataReceived\tlpData=NULL or dwLen=NULL\n");
		return;
	}

	CSecID camID(m_wGroupID, wExtCard * (COCO_MAX_SOURCE+1) + wSource);

	if (m_res==RESOLUTION_NONE)
	{
		// HEDU insertes CTimedMessage
		m_tmInvalidEncoderResolutionInIndication.Message(
			"IndicationDataReceived\tInvalid resolution NONE for mpeg data\n"
			);
	}
	
//	WK_TRACE("Schicke Datenblock an die CIPC\t(BlockNummer=%u)\n", wSendBlockNr);
	CSystemTime st;
	st.GetLocalTime();
	if (bIntra)
	{
		CIPCPictureRecord PictRec(this, (const unsigned char*)lpData, dwLen, camID, m_res, m_comp, COMPRESSION_H263 ,
						st, 0, NULL, m_dwEncBitrate, SPT_FULL_PICTURE, wSendBlockNr++);
		DoIndicateH263Data(PictRec, dwUserData);
	}
	else
	{
		CIPCPictureRecord PictRec(this, (const unsigned char*)lpData, dwLen, camID, m_res, m_comp, COMPRESSION_H263 ,
						st, 0, NULL, m_dwEncBitrate, SPT_DIFF_PICTURE, wSendBlockNr++);
		DoIndicateH263Data(PictRec, dwUserData);
	}
}  

//////////////////////////////////////////////////////////////////////////
void CIPCOutputCoCoUnit::IndicationEncoderStarted()
{
	// WK_TRACE("CIPCOutputCoCoUnit::IndicationEncoderStarted\n");
	// OOPS HEDU	DoConfirmStartH263Encoding(m_camID);
}

//////////////////////////////////////////////////////////////////////////
void CIPCOutputCoCoUnit::IndicationEncoderStoped()
{
	m_res		   = RESOLUTION_NONE;
	m_comp		   = COMPRESSION_NONE;
	m_dwEncBitrate = 0;
	// WK_TRACE("CIPCOutputCoCoUnit::IndicationEncoderStoped\n");

	DoConfirmStopH263Encoding(m_camID);
}

//////////////////////////////////////////////////////////////////////////
void CIPCOutputCoCoUnit::IndicationDecoderStarted()
{
	// WK_TRACE("CIPCOutputCoCoUnit::IndicationDecoderStarted\n");
	// OOPS ML DoConfirmH263Decoding(0);	// OOPS HEDU
	// OLD DoConfirmStartH263(m_camID);
}

//////////////////////////////////////////////////////////////////////////
void CIPCOutputCoCoUnit::IndicationDecoderStoped()
{
	m_dwDecBitrate = 0L;
	// WK_TRACE("CIPCOutputCoCoUnit::IndicationDecoderStoped\n");

	/* OOPS z.Z. kein DoConfirmStopH263
	RECT	rcDummy;
	DWORD	dwUserdata;
	rcDummy.left	= 0;
	rcDummy.top		= 0;
	rcDummy.right	= 0;
	rcDummy.bottom	= 0;
	dwUserdata		= 0L;
	
	DoConfirmStopH263(m_wGroupID, rcDummy, dwUserdata);
	*/
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputCoCoUnit::SelectCamera(CSecID camID)
{
	DWORD dwRet				= 0L;
	WORD  wOldExtCard		= 0L;
	WORD  wOldSource		= 0;
	WORD  wOldSourceNr		= 0;
	WORD  wNewExtCard		= 0;
	WORD  wNewSource		= 0;
	DWORD dwNewStateMask	= 0L;
	DWORD dwOldStateMask	= 0;
	WORD  wGroupID			= 0;
	WORD  wSubID			= 0;

	wGroupID = camID.GetGroupID();
	wSubID	 = camID.GetSubID();

//	WK_TRACE("CIPCOutputCoCoUnit::SelectCamera\tGroupID=%i\tNr=%i\n", wGroupID, iNr);

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_WARNING("CIPCOutputCoCoUnit::SelectCamera Wrong wGroupID\n");

		DoIndicateError(0, camID, CIPC_ERROR_INVALID_GROUP, 0);	
		return;
	}

	if (m_iType == OUTPUT_TYPE_COCO_CAMERA)
	{
		// Bisherige Kameraauswahl holen
		if (m_pCoCo)
			dwRet = m_pCoCo->GetInputSource(m_byCoCoID);
		wOldExtCard	= HIWORD(dwRet);  // Erweiterungskarte 0...3
		wOldSource	= LOWORD(dwRet);  // Kameranummer 0...8
		wOldSourceNr	= (COCO_MAX_SOURCE + 1) * wOldExtCard + wOldSource;

		// Auf gewünschte Kamera umschalten
		wNewExtCard	= wSubID / (COCO_MAX_SOURCE + 1);	// Erweiterungskarte 0...3
		wNewSource		= wSubID % (COCO_MAX_SOURCE + 1);	// Kameranummer 0...8
		dwNewStateMask = SETBIT(0L, wSubID);
		dwOldStateMask = SETBIT(0L, wOldSourceNr);
		
		if (m_pCoCo)
		{	
			m_pCoCo->SetInputSource(m_byCoCoID, wNewExtCard, wNewSource);
			m_camID = camID;
		}

		if (m_pMegra && m_pMegra->IsValid())
			m_pMegra->SetInputType(INPUT_CODEC);
		//OOPS ML DoConfirmSelect(wGroupID, dwNewStateMask, dwNewStateMask^dwOldStateMask);
	}
	else if (m_iType == OUTPUT_TYPE_MEGRA_CAMERA)
	{	  
		// Aktuell gewählte Kamera holen.
		if (m_pMegra && m_pMegra->IsValid())
			wOldSourceNr	= m_pMegra->GetInputSource();

		dwNewStateMask = SETBIT(0L, wSubID);
		dwOldStateMask = SETBIT(0L, wOldSourceNr);

		if (m_pMegra && m_pMegra->IsValid())
		{
			m_pMegra->SetInputType(INPUT_VIDEO);
			m_pMegra->SetInputSource(wSubID);
		}

		// CoCo auf Megrafrontend schalten
		if (m_pCoCo)
		{
			m_pCoCo->SetInputSource(m_byCoCoID, COCO_EXTCARD_MEGRA, COCO_SOURCE_CURRENT);
 			m_camID = camID;
		}
		// OOPS ML DoConfirmSelect(m_wGroupID,  dwNewStateMask,  dwNewStateMask^dwOldStateMask);
	}
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputCoCoUnit::EncoderStart(CVideoJob *pJob)
{
//	DWORD	dwBytesPerFrame	= 0; 
	DWORD	dwBitrateScale  = 0;
	
	if (!pJob)
	{
		WK_TRACE_ERROR("CIPCOutputCoCoUnit::StartEncoder\tpJob=NULL\n");
		return FALSE;
	}
	
	// Alle notwendigen Daten aus dem Videojob holen
	CSecID		camID	= pJob->m_camID;
	Resolution	res		= pJob->m_resolution;
	Compression	comp	= pJob->m_compression;
	dwBitrateScale		= pJob->m_dwBitrateScale; 


	if (res == RESOLUTION_NONE)
	{
		WK_TRACE_WARNING("CIPCOutputCoCoUnit::EncoderStart\tresolution=%u\n", (WORD)res);
	}

	if (comp == COMPRESSION_NONE)
	{
		WK_TRACE_WARNING("CIPCOutputCoCoUnit::EncoderStart\tcompression=%u\n", (WORD)res);
	}

	// WK_TRACE("CIPCOutputCoCoUnit::StartEncoder\n");

	// Auf die gewünschte Kamera umschalten
	SelectCamera(camID);

 	if ((comp != m_comp) || (res != m_res) || (dwBitrateScale != m_dwBitrateScale))
	{
		// res und comp werden hier für 'IndicationDataReceived()' gespeichert
		m_res			= res;
		m_comp			= comp;
		m_dwBitrateScale= dwBitrateScale;

		switch(m_comp)
		{
			case COMPRESSION_1:
				m_dwEncBitrate	= 2048000;
				m_wMPI			= 1;
				break;
			case COMPRESSION_2:
				m_dwEncBitrate	= 2048000;
				m_wMPI  		= 2;
				break;
			case COMPRESSION_3:
				m_dwEncBitrate	= 1536000;
				m_wMPI			= 1;
				break;
			case COMPRESSION_4:
				m_dwEncBitrate	= 1536000;
				m_wMPI			= 2;
				break;
			case COMPRESSION_5:
				m_dwEncBitrate	= 1024000;
				m_wMPI			= 1;
				break;
			case COMPRESSION_6:
				m_dwEncBitrate	= 1024000;
				m_wMPI			= 2;
				break;
			case COMPRESSION_7:
				m_dwEncBitrate	= 512000;
				m_wMPI			= 1;		// 25/1 = 25fps
				break;
			case COMPRESSION_8:
				m_dwEncBitrate	= 512000;
				m_wMPI			= 2;		// 25/2 = 12fps
				break;
			case COMPRESSION_9:
				m_dwEncBitrate	= 512000;
				m_wMPI			= 4;		// 25/4 = 6fps
				break;
			case COMPRESSION_10:
				m_dwEncBitrate	= 512000;
				m_wMPI			= 8;		// 25/8 = 3fps
				break;
			case COMPRESSION_11:
				m_dwEncBitrate	= 512000;
				m_wMPI			= 25;		// 25/25 = 1fps
				break;
			case COMPRESSION_12:
				m_dwEncBitrate	= 256000;
				m_wMPI			= 1;		// 25/1 = 25fps
				break;
			case COMPRESSION_13:
				m_dwEncBitrate	= 256000;
				m_wMPI			= 2;		// 25/2 = 12fps
				break;
			case COMPRESSION_14:
				m_dwEncBitrate	= 256000;
				m_wMPI			= 4;		// 25/4 = 6fps
				break;
			case COMPRESSION_15:
				m_dwEncBitrate	= 256000;
				m_wMPI			= 8;		// 25/8 = 3fps
				break;
			case COMPRESSION_16:
				m_dwEncBitrate	= 256000;
				m_wMPI			= 25;		// 25/25 = 1fps
				break;
			case COMPRESSION_17:
				m_dwEncBitrate	= 115200;
				m_wMPI			= 1;		// 25/1 = 25fps
				break;		   
			case COMPRESSION_18:
				m_dwEncBitrate	= 115200;
				m_wMPI			= 2;		// 25/2 = 12fps
				break;
			case COMPRESSION_19:
				m_dwEncBitrate	= 115200;
				m_wMPI			= 4;		// 25/4 = 6fps
				break;
			case COMPRESSION_20:
				m_dwEncBitrate	= 115200;
				m_wMPI			= 8;		// 25/8 = 3fps
				break;
			case COMPRESSION_21:
				m_dwEncBitrate	= 115200;
				m_wMPI			= 25;		// 25/25 = 1fps
				break;
			case COMPRESSION_22:
				m_dwEncBitrate	= 57600;
				m_wMPI			= 1;		// 25/1 = 25fps
				break;
			case COMPRESSION_23:
				m_dwEncBitrate	= 57600;
				m_wMPI			= 2;		// 25/2 = 12fps
				break;
			case COMPRESSION_24:
				m_dwEncBitrate	= 57600;
				m_wMPI			= 4;		// 25/4 = 6fps
				break;
			case COMPRESSION_25:
				m_dwEncBitrate	= 57600;
				m_wMPI			= 8;		// 25/8 = 3fps
				break;
			case COMPRESSION_26:
				m_dwEncBitrate	= 57600;
				m_wMPI			= 25;		// 25/25 = 1fps
				break;
			default:
				WK_TRACE_ERROR("CIPCOutputCoCoUnit::EncoderStart\tUnbekannter Kompressionsfaktor (Compression=%d)\n", m_comp);	

				m_dwEncBitrate	= 64000;
				m_wMPI			= 1;		// 25/1 = 25fps
				break;
		}		

		// Auflösung festlegen
		switch (res)
		{
			case RESOLUTION_MID:
			case RESOLUTION_HIGH:
					m_wFormat = COCO_ENCODER_CIF;
					break;
			case RESOLUTION_LOW:
					m_wFormat = COCO_ENCODER_QCIF;	
					break;
			default:
					WK_TRACE_ERROR("CIPCOutputCoCoUnit::EncoderStart\tUnbekanntes Bildformat\n");	
			   
					m_wFormat = COCO_ENCODER_QCIF;
					break;
		}
		
		// Korrekturfaktor zur Bitratenregelung
		if ((dwBitrateScale != 0) && (dwBitrateScale <= 2000))
		{
			m_dwEncBitrate  = m_dwEncBitrate * dwBitrateScale / 1000;
//			WK_TRACE("Bitratenkorrektur:Scale=%d\tBitrate=%d\n", dwBitrateScale, m_dwEncBitrate); 
		}

		// Setze Bildformat, Framerate und die Bitrate des Encoders.
		if (m_pCoCo)
			m_pCoCo->EncoderSetMode(m_byCoCoID, m_wFormat, m_wMPI, m_dwEncBitrate);
	}

	// Encoder nur starten, wenn dieser inaktiv ist.
	if (m_pCoCo && m_pCoCo->EncoderGetState(m_byCoCoID) == ENCODER_OFF)
	{
		// Encoder Starten
		if (m_pCoCo)
			m_pCoCo->EncoderStart(m_byCoCoID);

		// WK_TRACE("CIPCOutputCoCoUnit::CIPCOutputCoCoUnit::StartEncoder\tEncoderStart\n");
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputCoCoUnit::EncoderStop(CVideoJob *pJob)
{
	// WK_TRACE("CIPCOutputCoCoUnit::CIPCOutputCoCoUnit::StopEncoder\tEncoderStop\n");

	// Encoder Stoppen
	if (m_pCoCo)
		m_pCoCo->EncoderStop(m_byCoCoID);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputCoCoUnit::DecoderStart(CVideoJob *pJob, int nJobCount)
{
	if (!m_bAllowHardDecode)
	{
		CString sMsg;
		sMsg.LoadString(IDS_RTE_ALLOW_HARDDECODE_COCO);
	
		WK_TRACE_WARNING(sMsg);

		CWK_RunTimeError RTerr(WAI_COCOUNIT, REL_CANNOT_RUN, RTE_DONGLE, sMsg, 2, 0);
		RTerr.Send();

		return FALSE;
	}

	if (!pJob)
	{
		WK_TRACE_ERROR("CIPCOutputCoCoUnit::StartDecoder\tpJob=NULL\n");
		return FALSE;
	}
	
	// Alle notwendigen Daten dem VideoJob entnehmen
 	CIPCPictureRecord*	pPict		= pJob->m_pSource;
	DWORD				dwUserData	= pJob->m_dwUserData;

	if (!pPict)
	{
		WK_TRACE_ERROR("CIPCOutputCoCoUnit::StartDecoder\tpPict=NULL\n");
		return FALSE;
	}

	DWORD			dwDecBitrate    = pPict->GetBitrate();
	Compression		comp			= pPict->GetCompression();
	CRect			rect			= pJob->m_overlayRect;

	// Anhand der Kompression die Decoderbitrate ermitteln
	if (dwDecBitrate == 0)
	{
		switch(comp)
		{
			case COMPRESSION_1:
			case COMPRESSION_2:
				dwDecBitrate	= 2048000;
				break;
			case COMPRESSION_3:
			case COMPRESSION_4:
				dwDecBitrate	= 1536000;
				break;
			case COMPRESSION_5:
			case COMPRESSION_6:
				dwDecBitrate	= 1024000;
				break;
			case COMPRESSION_7:
			case COMPRESSION_8:
			case COMPRESSION_9:
			case COMPRESSION_10:
			case COMPRESSION_11:
				dwDecBitrate	= 512000;
				break;
			case COMPRESSION_12:
			case COMPRESSION_13:
			case COMPRESSION_14:
			case COMPRESSION_15:
			case COMPRESSION_16:
				dwDecBitrate	= 256000;
				break;
			case COMPRESSION_17:
			case COMPRESSION_18:
			case COMPRESSION_19:
			case COMPRESSION_20:
			case COMPRESSION_21:
				dwDecBitrate	= 128000;
				break;
			case COMPRESSION_22:
			case COMPRESSION_23:
			case COMPRESSION_24:
			case COMPRESSION_25:
			case COMPRESSION_26:
				dwDecBitrate	= 64000;
				break;
			default:
				WK_TRACE_ERROR("DecoderStart\tUnbekannter Kompressionsfaktor (Compression=%d)\n", comp);	// HEDU was m_comp, did not match switch(comp)
				// OOPS HEDU, changed 'error bitrate' from 64000 --> 512000
				dwDecBitrate	= 512000;
				break;
		}// switch		
	}// end if (dwDecBitrate == 0)

	// Bei einem Stau in der Decoderjobqueue, wird die Decoderbitrate erhöht
	if (nJobCount >= 4)
	{
		dwDecBitrate *= 2;
		// OOPS HEDU
		if (nJobCount >= 8)	// SuperStau, do it even fater
		{
			dwDecBitrate *= 2;		
		}

	}
	// OOPS HEDU any limit
	if (dwDecBitrate>2048000) {
			dwDecBitrate = 2048000;
	}


	if (m_pCoCo && m_pCoCo->DecoderGetState(m_byCoCoID) == DECODER_OFF)
	{
		// WK_TRACE("CIPCOutputCoCoUnit::StartDecoder\tDecoderStart\n");
		if (m_pCoCo)
			m_pCoCo->DecoderStart(m_byCoCoID);
	}
	else
	{
		if (m_pCoCo)
			m_pCoCo->ConnectVideoOutput(m_byCoCoID, COCO_DECODER);
	}

	// Decoderbitrate anpassen, falls diese sich ändert.
	if (dwDecBitrate  != m_dwDecBitrate)
	{
		if (m_pCoCo)
		{
			// Neue Decoderbitrate merken
			m_pCoCo->DecoderSetBitrate(m_byCoCoID, dwDecBitrate);
//			WK_STAT_LOG(1,"Timer",dwDecBitrate,"DBR");
			m_dwDecBitrate = dwDecBitrate;
		}
	}

	// Daten an den Decoder schicken
	SendDataToDecoder(pPict);

	DoConfirmH263Decoding(m_wGroupID, dwUserData);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputCoCoUnit::SendDataToDecoder(const CIPCPictureRecord *pPict)
{
	PARAMSTRUCT		Param;
	DWORD			dwDataLen	= 0L;
	const BYTE*		pData		= NULL;

	// Pointer Ok ?
	if (!pPict)
	{
		WK_TRACE_ERROR("CIPCOutputCoCoUnit::SendDataToDecoder\tpPict=NULL\n");
		return FALSE;
	}

	// Pointer auf die Bilddaten holen
	pData		= (const BYTE*)pPict->GetData();
	dwDataLen	= pPict->GetDataLength();

	// Länge sinvoll ?
	if (dwDataLen == 0L)
	{
		WK_TRACE_ERROR("CIPCOutputCoCoUnit::SendDataToDecoder\t Blocklänge 0\n");	
		return	FALSE;
	}

	// Pointer Ok ?
	if (!pData)
	{
		WK_TRACE_ERROR("CIPCOutputCoCoUnit::SendDataToDecoder\t pData = NULL \n");	
		return	FALSE;
	}

//	if (pPict->GetPictureType() == SPT_FULL_PICTURE)
//		MessageBeep(0);

	// Komprimierte Bilddaten verschicken
	Param.dwData	= COCO_DECODER_DATA;
	Param.lpData	= (LPCSTR)pData;
	Param.dwLen		= dwDataLen;
	Param.wCoCoID	= (WORD)m_byCoCoID;
	Param.wIntra	= 0;
	DWORD dwCnt		= 0L;

	do{
		if (m_pCoCo && m_pCoCo->WriteData(Param))
		{
			dwCnt += Param.dwLen;
			Param.lpData = (LPCSTR)&pData[dwCnt];
			Param.dwLen  = dwDataLen - dwCnt;
			if (m_pCoCo->DecoderGetState(m_byCoCoID) == DECODER_OFF)
				break;
			if (!m_pCoCo)
				break;
		}
		Sleep(5);	// switch to another thread
	}
	while (Param.dwLen > 0);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputCoCoUnit::DecoderStop(CVideoJob *pJob)
{
	if (!pJob)
	{
		WK_TRACE_ERROR("CIPCOutputCoCoUnit::DecoderStop\tpJob=NULL\n");
		return FALSE;
	}

 	// WK_TRACE("CIPCOutputCoCoUnit::StopDecoder\tDecoderStop\n");

	if (m_pCoCo)
		m_pCoCo->DecoderStop(m_byCoCoID);
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputCoCoUnit::LocalVideo(CVideoJob *pJob)
{
	if (!pJob)
	{
		WK_TRACE_ERROR("CIPCOutputCoCoUnit::LocalOverlay\tpJob=NULL\n");
		return FALSE;
	}

	// Alle notwendigen Daten aus dem Videojob holen
	CSecID	camID		= pJob->m_camID;
	CRect	outputRect	= pJob->m_overlayRect;
	
	WORD wGroupID = camID.GetGroupID();

	if (wGroupID != m_wGroupID)
	{
		WK_TRACE_WARNING("CIPCOutputCoCoUnit::LocalOverlay\tWrong wGroupID\t\n");

		DoIndicateError(0, camID, CIPC_ERROR_INVALID_GROUP, 0);	
		return FALSE;
	}

	// Neue Kamera?	-> einfrieren, Outputfenster umsetzen, Kamera umschalten, freigeben.
	if (camID != m_camID)
	{
		// Outputfenster setzen
		if (m_pMegra && m_pMegra->IsValid())
		{
			m_pMegra->Freeze(TRUE);
			Sleep(50);
			m_pMegra->SetOutputWindow((const LPRECT) &outputRect);
		}

		// Auf die gewünschte Kamera umschalten
		SelectCamera(camID);
		
		if (m_pMegra && m_pMegra->IsValid())
		{
			m_pMegra->Freeze(FALSE);
		}
	}
	else
	{
		if (m_pMegra && m_pMegra->IsValid())
		{
			m_pMegra->SetOutputWindow((const LPRECT) &outputRect);
		}
	}

	// Auf Livevideo schalten
	if (m_pCoCo)
		m_pCoCo->ConnectVideoOutput(m_byCoCoID,COCO_PREP);

	return TRUE;
}

// NEW HEDU
void CIPCOutputCoCoUnit::OnRequestSync(DWORD dwTickSend,DWORD dwType, DWORD dwUser)
{
	CVideoJob *pJob;
	pJob = new CVideoJob(dwTickSend,dwType,dwUser);	// VJA_SYNC
	if (pJob)
	{
		pJob->m_pCipc = this;

		if (m_pMainWnd)
			m_pMainWnd->GetJobQueueEncode().SafeAddTail(pJob);
	}
}
