// CIPCOutputVision.cpp: implementation of the CIPCOutputVision class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Vision.h"

#include "VisionDoc.h"
#include "ObjectView.h"
#include "VisionView.h"
#include "Server.h"
#include "CIPCOutputVision.h"

#include "MicoWindow.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCOutputVision::CIPCOutputVision(LPCTSTR shmName, CServer* pServer, CSecID id)
	: CIPCOutputClient(shmName,FALSE)
{
	m_pPictureLengthStatistic = NULL;
	if (   !(pServer->GetConfirmedOutputOptions() & SCO_NO_STREAM)
		&& theApp.m_bCorrectBitrate
		&& !pServer->IsLocal())
	{
		m_pPictureLengthStatistic = new CPictureLengthStatistic(this);
	}
	m_pServer = pServer;
	m_ID = id;
	m_bGotInfo = FALSE;
	m_dwTime = GetTickCount();
	m_dwBitrate = 2048*1024;

	//m_idActiveCamera
	StartThread();
}
//////////////////////////////////////////////////////////////////////
CIPCOutputVision::~CIPCOutputVision()
{
	DWORD dwTick = WK_GetTickCount();
	int c = 10;
	while (   GetSendQueueLength()>0
		   && c>0)
	{
		Sleep(10);
		c--;
	}
	WK_TRACE(_T("wait for CIPC send queue %d ms\n"),WK_GetTimeSpan(dwTick));
	StopThread();
	WK_DELETE(m_pPictureLengthStatistic);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputVision::OnReadChannelFound()
{
//	TRACE(_T("CIPCOutputVision::OnReadChannelFound()\n"));
	m_dwTime = GetTickCount();
	// RKE: CIPCInputVision ist kein Master
	DoRequestConnection();	// muß das aufgerufen werden ?
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputVision::OnConfirmConnection()
{
//	TRACE(_T("CIPCOutputVision::OnConfirmConnection()\n"));
	m_dwTime = GetTickCount();
	if (!m_pServer->IsLocal())
	{
		TRACE(_T("initial DoRequestBitrate\n"));
		DoRequestBitrate();
	}
	DoRequestVersionInfo(0);
	DoRequestInfoOutputs(SECID_NO_GROUPID);	// all groups
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputVision::OnConfirmVersionInfo(WORD wGroupID, DWORD dwVersion)
{
	TRACE(_T("server version is %d\n"),dwVersion);
}
//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputVision::Run(DWORD dwTimeOut)
{
	if (!m_bGotInfo)
	{
		DWORD curTime = GetTickCount();
		if ((curTime-m_dwTime)>15000)
		{
			// time out post app to stop connection
			WK_TRACE_ERROR(_T("time out for output connection <%s>\n"),m_pServer->GetName());
			m_dwTime = curTime;
			AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_DISCONNECT_LOW+m_pServer->GetID());
		}
	}
	return CIPC::Run(dwTimeOut);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputVision::OnConfirmBitrate(DWORD dwBitrate)
{
	TRACE(_T("CIPCOutputVision::OnConfirmBitrate(%d)\n"),dwBitrate);
	if (dwBitrate==0)
	{
		dwBitrate = 512 * 1024;
	}
	if (   !m_pServer->IsLocal() 
		&& m_dwBitrate!=dwBitrate)
	{
		m_dwBitrate = dwBitrate;
		if (theApp.m_bTraceBitrate)
		{
			CString s = _T("ConnectionBitrate|") + m_pServer->GetName();
			WK_STAT_LOG(_T("Bitrate"),dwBitrate,s);
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputVision::OnConfirmInfoOutputs(WORD wGroupID, int iNumGroups, int numRecords, const CIPCOutputRecord records[])
{
#if (0)
	for (int i=0;i<numRecords;i++)
	{
		CString sFlags;
		WORD wFlags = records[i].GetFlags();
		if (wFlags & OREC_IS_OKAY)
		{
			sFlags += _T("|OREC_IS_OKAY");
		}
		if (wFlags & OREC_IS_ENABLED)
		{
			sFlags += _T("|OREC_IS_ENABLED");
		}
		if (wFlags & OREC_IS_RELAY)
		{
			sFlags += _T("|OREC_IS_RELAY");
		}
		if (wFlags & OREC_RELAY_CLOSED)
		{
			sFlags += _T("|OREC_RELAY_CLOSED");
		}
		if (wFlags & OREC_IS_CAMERA)
		{
			sFlags += _T("|OREC_IS_CAMERA");
		}
		if (wFlags & OREC_CAMERA_COLOR)
		{
			sFlags += _T("|OREC_CAMERA_COLOR");
		}
		if (wFlags & OREC_CAMERA_IS_FIXED)
		{
			sFlags += _T("|OREC_CAMERA_IS_FIXED");
		}
		if (wFlags & OREC_CAMERA_DOES_JPEG)
		{
			sFlags += _T("|OREC_CAMERA_DOES_JPEG");
		}
		if (wFlags & OREC_CAMERA_VCS)
		{
			sFlags += _T("|OREC_CAMERA_VCS");
		}
		if (wFlags & OREC_CAMERA_PRESENCE)
		{
			sFlags += _T("|OREC_CAMERA_PRESENCE");
		}
		if (wFlags & OREC_CAMERA_REFERENCE)
		{
			sFlags += _T("|OREC_CAMERA_REFERENCE");
		}
		TRACE(_T("%d, %s, %04hx,%s\n"),i,records[i].GetName(),records[i].GetFlags(),sFlags);
	}
#endif
	CIPCOutputClient::OnConfirmInfoOutputs(wGroupID,iNumGroups,numRecords,records);

	m_bGotInfo = TRUE;

	SearchRelayPTZ();

// ML 30.9.99 Begin Insertation
	// Den Status aller Kameras und Relais an GEMOS senden
	if (m_pServer->IsControlConnected())
	{
		int c = GetNumberOfOutputs();
		for (int i=0;i<c && m_pServer->IsControlConnected();i++)
		{
			const CIPCOutputRecord& rec = GetOutputRecordFromIndex(i);
			if (rec.IsEnabled())
			{
				if (rec.IsRelay())
				{
					m_pServer->GetControl()->ConfirmRelay(rec.GetID(), Enabled);
					if (rec.RelayIsOpen())
						m_pServer->GetControl()->ConfirmRelay(rec.GetID(), Off);
					else		
						m_pServer->GetControl()->ConfirmRelay(rec.GetID(), On);
				}	
				if (rec.IsCamera())
				{
					m_pServer->GetControl()->ConfirmCamera(rec.GetID(), Enabled);
				}
			}		
			else
			{
				if (rec.IsRelay())
					m_pServer->GetControl()->ConfirmRelay(rec.GetID(), Disabled);
				if (rec.IsCamera())
					m_pServer->GetControl()->ConfirmCamera(rec.GetID(), Disabled);
			}
		}
	}
// ML 30.9.99 End Insertation

	if (m_pServer->IsDTS())
	{
		WK_DELETE(m_pPictureLengthStatistic);
	}

	CObjectView* pOV = m_pServer->GetDocument()->GetObjectView();
	if (WK_IS_WINDOW(pOV))
	{
		pOV->PostMessage(WM_USER,
						 MAKELONG(WPARAM_OUTPUT_CONNECTED,m_pServer->GetID())
						);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputVision::OnUpdateRecord(const CIPCOutputRecord & record)
{
	CObjectView* pOV = m_pServer->GetDocument()->GetObjectView();
	if (WK_IS_WINDOW(pOV))
	{
		pOV->PostMessage(WM_USER,
						 MAKELONG(WPARAM_ACTUALIZE,m_pServer->GetID()),
						 record.GetID().GetID());
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputVision::OnAddRecord(const CIPCOutputRecord & record)
{
	if (record.IsCamera())
	{
		if (    m_pServer 
			&&  m_pServer->IsControl()
			&& (m_pServer->GetAlarmID().GetSubID()==record.GetID().GetSubID())
			)
		{
			m_pServer->SetAlarmID(record.GetID());
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputVision::OnDeleteRecord(const CIPCOutputRecord & record)
{
	CObjectView* pOV = m_pServer->GetDocument()->GetObjectView();
	if (WK_IS_WINDOW(pOV))
	{
		pOV->PostMessage(WM_USER,
						 MAKELONG(WPARAM_DELETE,m_pServer->GetID()),
						 record.GetID().GetID());
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputVision::OnRequestDisconnect()
{
	//WK_TRACE(_T("request disconnect output <%s>\n"),m_pServer->GetName());
	DelayedDelete();
	CObjectView* pOV = m_pServer->GetDocument()->GetObjectView();
	if (WK_IS_WINDOW(pOV))
	{
		pOV->PostMessage(WM_USER,
						 MAKELONG(WPARAM_OUTPUT_DISCONNECTED,m_pServer->GetID())
						);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
// reactivate cam, switch immediately to cam
// for parameter changes
//////////////////////////////////////////////////////////////////////////////////////
// remove this cam from the active ones
void CIPCOutputVision::DeactivateCamera(CSecID id,BOOL bForceCoCoStop/*=FALSE*/)
{
	if (m_pServer->IsControlConnected())
	{
		m_pServer->GetControl()->ConfirmCamera(id.GetID(), Off);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
int	CIPCOutputVision::GetMaxMicoFPS()
{
	if (m_pServer->IsLocal())
	{
		return 25;
	}
	else
	{
		int iRet = m_dwBitrate / (64*1024);
		
		if (iRet>0)
		{
			if (iRet>12)
			{
				iRet = 12;
			}
			return iRet;
		}
		return 1;
	}
}
//////////////////////////////////////////////////////////////////////////////////////
/*
Mico Compessionstabelle
---------------------------
|Comp		    |HIGH|LOW |	
|COMPRESSION_26 |  4 |  2 |
|COMPRESSION_25 |  5 |  3 |
|COMPRESSION_24 |  6 |  3 |
|COMPRESSION_23 |  7 |  4 |
|COMPRESSION_22 |  8 |  4 |
|COMPRESSION_21 | 10 |  5 |
|COMPRESSION_20 | 12 |  6 |
|COMPRESSION_19 | 13 |  6 |
|COMPRESSION_18 | 14 |  7 |
|COMPRESSION_17 | 16 |  8 |
|COMPRESSION_16 | 17 |  9 |
|COMPRESSION_15 | 18 |  9 |
|COMPRESSION_14 | 20 | 10 |
|COMPRESSION_13 | 22 | 11 |
|COMPRESSION_12 | 24 | 12 |
|COMPRESSION_11 | 26 | 13 |
|COMPRESSION_10 | 28 | 14 |
|COMPRESSION_9  | 30 | 15 |
|COMPRESSION_8  | 31 | 15 |
|COMPRESSION_7  | 32 | 16 |
|COMPRESSION_6  | 35 | 17 |
|COMPRESSION_5  | 38 | 19 |
|COMPRESSION_4  | 38 | 19 |
|COMPRESSION_3  | 38 | 19 |
|COMPRESSION_2  | 38 | 19 |
|COMPRESSION_1  | 38 | 19 |
---------------------------
*/
////////////////////////////////////////////////////////////////////////////
Compression CIPCOutputVision::GetMicoCompression2(DWORD dwSize,Resolution res)
{
	DWORD dwMySize = dwSize;
	if (res==RESOLUTION_LOW)
	{
		dwMySize = 2 * dwSize; 
	}
	if (dwMySize>38*1024) return COMPRESSION_5;	
	if (dwMySize>35*1024) return COMPRESSION_6;	
	if (dwMySize>32*1024) return COMPRESSION_7;	
	if (dwMySize>31*1024) return COMPRESSION_8;	
	if (dwMySize>30*1024) return COMPRESSION_9;	

	if (dwMySize>28*1024) return COMPRESSION_10;	
	if (dwMySize>26*1024) return COMPRESSION_11;	
	if (dwMySize>24*1024) return COMPRESSION_12;	
	if (dwMySize>22*1024) return COMPRESSION_13;	
	if (dwMySize>20*1024) return COMPRESSION_14;	

	if (dwMySize>18*1024) return COMPRESSION_15;	
	if (dwMySize>17*1024) return COMPRESSION_16;	
	if (dwMySize>16*1024) return COMPRESSION_17;	
	if (dwMySize>14*1024) return COMPRESSION_18;	
	if (dwMySize>13*1024) return COMPRESSION_19;	

	if (dwMySize>12*1024) return COMPRESSION_20;	
	if (dwMySize>10*1024) return COMPRESSION_21;	
	if (dwMySize> 8*1024) return COMPRESSION_22;	
	if (dwMySize> 7*1024) return COMPRESSION_23;	
	if (dwMySize> 6*1024) return COMPRESSION_24;	
	if (dwMySize> 5*1024) return COMPRESSION_25;	
	if (dwMySize> 4*1024) return COMPRESSION_26;	


	return COMPRESSION_26;
}
////////////////////////////////////////////////////////////////////////////
Compression CIPCOutputVision::GetMicoCompression(int iQuality, Resolution res)
{
// iQuality = 0 high speed, low quality, high compression
// iQuality = 4 low speed, high quality, low compression
	DWORD dwBitrate = m_dwBitrate * 90 / 100;

	if (m_pServer->IsLocal())
	{
		return COMPRESSION_12;
	}

	if (iQuality<0)
	{
		DWORD dwSize = (dwBitrate/8) / (1-iQuality);
		return GetMicoCompression2(dwSize,res);
	}
	else
	{
		DWORD dwSize = (dwBitrate/8) * (1+iQuality);
		return GetMicoCompression2(dwSize,res);
	}

//	return COMPRESSION_26;
}
////////////////////////////////////////////////////////////////////////////
Compression CIPCOutputVision::GetCocoCompression(DWORD dwBitrate, int iQuality)
{
	// iQuality = 0 -> 25 fps high speed low quality
	// iQuality = 4 -> 1 fps
	if (dwBitrate<=64*1024)
	{
		switch (iQuality)
		{
		case 0:
			return COMPRESSION_22;
		case 1:
			return COMPRESSION_23;
		case 2:
			return COMPRESSION_24;
		case 3:
			return COMPRESSION_25;
		case 4:
			return COMPRESSION_26;
		}
		return COMPRESSION_22;
	}
	else if (dwBitrate<=128*1024)
	{
		switch (iQuality)
		{
		case 0:
			return COMPRESSION_17;
		case 1:
			return COMPRESSION_18;
		case 2:
			return COMPRESSION_19;
		case 3:
			return COMPRESSION_20;
		case 4:
			return COMPRESSION_21;
		}
		return COMPRESSION_17;
	}
	else if (dwBitrate<=256*1024)
	{
		switch (iQuality)
		{
		case 0:
			return COMPRESSION_12;
		case 1:
			return COMPRESSION_13;
		case 2:
			return COMPRESSION_14;
		case 3:
			return COMPRESSION_15;
		case 4:
			return COMPRESSION_16;
		}
		return COMPRESSION_12;
	}
	else if (dwBitrate<=512*1024)
	{
		switch (iQuality)
		{
		case 0:
			return COMPRESSION_7;
		case 1:
			return COMPRESSION_8;
		case 2:
			return COMPRESSION_9;
		case 3:
			return COMPRESSION_10;
		case 4:
			return COMPRESSION_11;
		}
		return COMPRESSION_7;
	}
	else if (dwBitrate<=1024*1024)
	{
		switch (iQuality)
		{
		case 0:
			return COMPRESSION_5;
		case 1:
			return COMPRESSION_6;
		}
		return COMPRESSION_5;
	}
	else if (dwBitrate<=1536*1024)
	{
		switch (iQuality)
		{
		case 0:
			return COMPRESSION_3;
		case 1:
			return COMPRESSION_4;
		}
		return COMPRESSION_3;
	}
	else if (dwBitrate<=2024*1024)
	{
		switch (iQuality)
		{
		case 0:
			return COMPRESSION_1;
		case 1:
			return COMPRESSION_2;
		}
		return COMPRESSION_1;
	}
	return COMPRESSION_NONE;
}
//////////////////////////////////////////////////////////////////////////////////////
// translate to spezial CIPC Request
void CIPCOutputVision::RequestPictures(CSecID id)
{
	CVisionView* pVV = m_pServer->GetDocument()->GetVisionView();

	if (!WK_IS_WINDOW(pVV))
	{
		return;
	}

	CDisplayWindow* pDW = pVV->GetDisplayWindow(m_pServer->GetID(),id);
	if (!WK_IS_WINDOW(pDW))
	{
		return;
	}
	CIPCOutputRecord* pRec = GetOutputRecordPtrFromSecID(id);
	if (!pRec)
	{
		return;
	}

	Resolution	res = pDW->GetResolution();
	int iQuality = pDW->GetQuality();
	Compression comp = COMPRESSION_1;

	if (pDW->IsMico())
	{
		comp = GetMicoCompression(iQuality,res);
	}
	else if (pDW->IsCoco())
	{
		DWORD dwBitrate = m_dwBitrate;
		if (dwBitrate>256*1024)
		{
			dwBitrate = 256*1024;
		}
		comp = GetCocoCompression(dwBitrate,iQuality);
	}

	if (pRec==NULL)
	{
		return;
	}
	
	if (pDW->IsPresence())
	{
		// Presence Cam
		m_idActiveCam = id;
		DoRequestEncodedVideo(id,res,comp,0,0,0);
		TRACE(_T("Request PT\n"));
	}
	else if (pDW->IsCoco())
	{
		StopJpegEncoding();

		// Coco Cam
		m_idActiveCam = id;
		DWORD dwTuning = 1000;
		if (m_pPictureLengthStatistic)
		{
			dwTuning = m_pPictureLengthStatistic->GetTuning();
		}
		DoRequestStartH263Encoding(id,
								   res,
								   comp,
								   dwTuning);
//		TRACE(_T("Request CoCo %08lx\n"),id.GetID());
	}
	else if (pDW->IsMico())
	{
		// stop coco cam if any
		StopH263Encoding();

		DWORD timeSlice = 1000;
		DWORD fps = 1;
		// Mico Cam
		// TODO Parameters of every Cam
		m_idActiveCam = id;
		if (iQuality<0)
		{
			timeSlice = 1000;
			if (m_pPictureLengthStatistic)
			{
				timeSlice += (m_pPictureLengthStatistic->GetDelay() - m_pPictureLengthStatistic->GetDelay()%1000);
			}
			fps = 1-iQuality;
		}
		else
		{
			fps = 1;
			timeSlice = (iQuality+1)*1000;
			if (m_pPictureLengthStatistic)
			{
				timeSlice += (m_pPictureLengthStatistic->GetDelay() - m_pPictureLengthStatistic->GetDelay()%1000);
			}
		}
//		TRACE(_T("DoRequestNewJpegEncoding %08lx\n"),id.GetID());
		DoRequestNewJpegEncoding(id,res,comp, fps,timeSlice); 
	}
	else
	{
		// BW ?
		return;
	}
	if (theApp.m_bTraceSwitch)
	{
		CString sCam;
		sCam.Format(_T("Switch %s|%lx"),m_pServer->GetName(),id.GetID());
		WK_STAT_LOG(_T("Switch"),1,sCam);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputVision::StopH263Encoding()
{
	CIPCOutputRecord* pRec = GetOutputRecordPtrFromSecID(m_idActiveCam);
	if (pRec!=NULL)
	{
		if (!pRec->CameraDoesJpeg() && pRec->CameraHasColor())
		{
			UnrequestPictures(m_idActiveCam,TRUE);
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputVision::StopJpegEncoding()
{
	if (!m_pServer->IsMultiCamera())
	{
		CVisionView* pVV = m_pServer->GetDocument()->GetVisionView();

		if (!WK_IS_WINDOW(pVV))
		{
			return;
		}
		CDisplayWindow* pDW = pVV->GetDisplayWindow(m_pServer->GetID(),m_idActiveCam);
		  
		if (   WK_IS_WINDOW(pDW)
			&& pDW->IsMico())
		{
			UnrequestPictures(m_idActiveCam,TRUE);
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
// translate to spezial CIPC Request
void CIPCOutputVision::UnrequestPictures(CSecID id,BOOL bForceCoCoStop/*=FALSE*/)
{
#ifdef _DEBUG
	//return;
#endif
	CIPCOutputRecord* pRec = GetOutputRecordPtrFromSecID(id);

	if (pRec==NULL)
	{
		return;
	}
	
	if (theApp.m_bTraceSwitch)
	{
		CString sCam;
		sCam.Format(_T("Switch %s|%lx"),m_pServer->GetName(),id.GetID());
		WK_STAT_LOG(_T("Switch"),0,sCam);
	}

	if (pRec->CameraIsVCS() && pRec->CameraHasColor())
	{
		// VCS Cam
	}
	else if (pRec->CameraIsPresence() && pRec->CameraHasColor())
	{
		// Presence Cam
		DoRequestEncodedVideo(id, 
								RESOLUTION_NONE,
								COMPRESSION_NONE,
								0,
								0,
								0);
	}
	else if (!pRec->CameraDoesJpeg() && pRec->CameraHasColor())
	{
		// Coco Cam

		if (bForceCoCoStop)
		{
			DoRequestStopH263Encoding(id);
		}
	}
	else if (pRec->CameraDoesJpeg() && pRec->CameraHasColor())
	{
		// Mico Cam
		DoRequestNewJpegEncoding(id,RESOLUTION_NONE,COMPRESSION_NONE, 0, 0);
	}
	else
	{
		// BW ?
		return;
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputVision::OnConfirmJpegEncoding(const CIPCPictureRecord &pict, DWORD dwUserData)
{
	CSecID id = m_pServer->IsDTS() ? CSecID(dwUserData) : pict.GetCamID();
	if (m_pPictureLengthStatistic)
	{
		*m_pPictureLengthStatistic += pict.GetDataLength();
	}
	if (theApp.m_bStatImage)
	{
		CString sCam;
		sCam.Format(_T("Image %s|%lx"),m_pServer->GetName(),id);
		WK_STAT_PEAK(_T("Image"),1,sCam);
	}
	m_pServer->PictureData(pict,id);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputVision::OnConfirmEncodedVideo(const CIPCPictureRecord &pict, DWORD dwUserData)
{
	CSecID id = m_pServer->IsDTS() ? CSecID(dwUserData) : pict.GetCamID();
	m_pServer->PictureData(pict,id);
	if (theApp.m_bStatImage)
	{
		CString sCam;
		sCam.Format(_T("Image %s|%lx"),m_pServer->GetName(),pict.GetCamID().GetID());
		WK_STAT_PEAK(_T("Image"),1,sCam);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputVision::OnIndicateH263Data(const CIPCPictureRecord &pict, DWORD dwUserData)
{
	CSecID id = m_pServer->IsDTS() ? CSecID(dwUserData) : pict.GetCamID();
	if (m_pPictureLengthStatistic)
	{
		*m_pPictureLengthStatistic += pict.GetDataLength();
	}
	if (theApp.m_bStatImage)
	{
		CString sCam;
		sCam.Format(_T("Image %s|%lx"),m_pServer->GetName(),pict.GetCamID().GetID());
		WK_STAT_PEAK(_T("Image"),1,sCam);
	}
	m_pServer->PictureData(pict,id);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputVision::OnConfirmSync(DWORD dwTickConfirm, DWORD dwTickSend, DWORD dwType,DWORD dwUserID)
{
	if (m_pPictureLengthStatistic)
	{
		if (m_pPictureLengthStatistic->OnConfirmSync(dwTickConfirm, dwTickSend))
		{
			if (m_idActiveCam!=SECID_NO_ID)
			{
				if (m_pPictureLengthStatistic->CalcTuning())
				{
					if (theApp.m_bTraceBitrate)
					{
						WK_STAT_LOG(_T("Bitrate"),m_pPictureLengthStatistic->GetTuning(),_T("EncoderTuning"));
					}
					RequestPictures(m_idActiveCam);
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputVision::OnRequestSelectCamera(CSecID camID)
{
	// 
	CVisionDoc* pDoc = m_pServer->GetDocument();
	if (pDoc==NULL)
	{ 
		return;
	}
	CObjectView* pOV = pDoc->GetObjectView();
	if (!WK_IS_WINDOW(pOV))
	{
		return;
	}
	CVisionView* pVV = pDoc->GetVisionView();
	if (!WK_IS_WINDOW(pVV))
	{
		return;
	}

	if (!m_pServer->IsAlarm())
	{
		WK_TRACE(_T("camera switch on non alarm connection\n"));
		m_pServer->SetAlarmSwitch(TRUE);
	}
	
	if (   m_pServer->GetAlarmSwitch()
		|| (!pVV->IsView1plus() && theApp.m_b1PlusAlarm)
		|| ((m_pServer->GetAlarmID() != camID) && theApp.m_b1PlusAlarm)
		)
	{
		if (camID != SECID_NO_ID)
		{
			WK_TRACE(_T("AlarmConnection %s switch to cam/input %lx\n"),m_pServer->GetName(),camID.GetID());
			m_pServer->SetAlarmID(camID);
		}
		pOV->PostMessage(WM_USER,
						 MAKELONG(WPARAM_SELECT,m_pServer->GetID()),
						 camID.GetID());
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputVision::OnRequestSetValue(
						CSecID id, // might be used as group ID only
						const CString &sKey,
						const CString &sValue,
						DWORD dwServerData
						)
{
	CVisionDoc* pDoc = m_pServer->GetDocument();
	if (pDoc==NULL)
	{ 
		return;
	}
	CObjectView* pOV = pDoc->GetObjectView();
	if (!WK_IS_WINDOW(pOV))
	{
		return;
	}

	if (id.IsNotificationMessageID())
	{
		if (sKey == NM_MESSAGE)
		{
			WK_TRACE(_T("new message is %s\n"),sValue);
			// todo transport ist to main thread
			TCHAR* pBuffer = new TCHAR[sValue.GetLength()+1];
			_tcscpy(pBuffer, sValue);
			pOV->PostMessage(WM_USER,
							 MAKELONG(WPARAM_MESSAGE,m_pServer->GetID()),
							 (LPARAM)pBuffer);
		}
		else if (sKey == NM_PARAMETER)
		{
			WK_TRACE(_T("new url is %s\n"),sValue);
			// todo transport ist to main thread
			TCHAR* pBuffer = new TCHAR[sValue.GetLength()+1];
			_tcscpy(pBuffer, sValue);
			pOV->PostMessage(WM_USER,
							 MAKELONG(WPARAM_URL,m_pServer->GetID()),
							 (LPARAM)pBuffer);
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputVision::RequestCSB(CSecID id)
{
	DoRequestGetValue(id,CSD_CONTRAST);
	DoRequestGetValue(id,CSD_BRIGHTNESS);
	DoRequestGetValue(id,CSD_SATURATION);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputVision::OnConfirmGetValue(CSecID id, const CString &sKey, const CString &sValue, DWORD dwServerData)
{
//	TRACE(_T("CIPCOutputVision::OnConfirmGetValue %08lx %s %s\n"),id.GetID(),sKey,sValue);
	// GF todo m_SmallWindows.Lock;
	CDisplayWindow* pDW = m_pServer->GetDocument()->GetVisionView()->GetDisplayWindow(
						 m_pServer->GetID(),id);

	if (pDW==NULL)
	{
		return;
	}

	if (sKey==CSD_CONTRAST)
	{
		int iContrast;
		// [120..390]
		iContrast = _ttoi(sValue);
		if (iContrast>390)
		{
			iContrast = 390;
		}
		if (iContrast<120)
		{
			iContrast = 120;
		}
		if (WK_IS_WINDOW(pDW))
		{
			pDW->PostMessage(WM_USER,WPARAM_CONTRAST,iContrast);
		}
	}
	else if (sKey==CSD_BRIGHTNESS)
	{
		int iBrightness;
		// [68..188]
		iBrightness = _ttoi(sValue);
		if (iBrightness>188)
		{
			iBrightness = 188;
		}
		if (iBrightness<68)
		{
			iBrightness = 68;
		}
		if (WK_IS_WINDOW(pDW))
		{
			pDW->PostMessage(WM_USER,WPARAM_BRIGHTNESS,iBrightness);
		}
	}
	else if (sKey==CSD_SATURATION)
	{
		int iSaturation;
		// [120..390]
		iSaturation = _ttoi(sValue);
		if (iSaturation>390)
		{
			iSaturation = 390;
		}
		if (iSaturation<120)
		{
			iSaturation = 120;
		}
		if (WK_IS_WINDOW(pDW))
		{
			pDW->PostMessage(WM_USER,WPARAM_SATURATION,iSaturation);
		}
	}
	else if (sKey==CSD_MODE)
	{
		if (WK_IS_WINDOW(pDW))
		{
			pDW->SetMode(sValue);
		}
	}
	if (0==sKey.Find(CSD_CAM_CONTROL_TYPE))
	{
		CameraControlType cct;
		DWORD dwID = 0;
		if (1 == _stscanf(sKey,CSD_CAM_CONTROL_TYPE_FMT,&dwID))
		{
			// GF todo m_SmallWindows.Lock;
			CDisplayWindow* pDW = m_pServer->GetDocument()->GetVisionView()->GetDisplayWindow(
								 m_pServer->GetID(),dwID);
			cct = NameToCameraControlType(sValue);
			if (cct != CCT_UNKNOWN)
			{
				CIPCOutputRecord*pOrec = GetOutputRecordPtrFromSecID(id);
/*				if (pOrec && (pOrec->GetFlags() & OREC_CAMERA_DIRECT_SHOW))
				{
					WORD wFlags = pOrec->GetFlags();
					wFlags &= ~OREC_CAMERA_IS_FIXED;
					pOrec->SetFlags(wFlags);
				}
*/
			}
			if (WK_IS_WINDOW(pDW))
			{
				pDW->PostMessage(WM_USER,WPARAM_CCT,(LPARAM)cct);
			}
		}
	}
	else if (0==sKey.Find(CSD_CAM_CONTROL_FKT))
	{
		DWORD dwID = 0;
		DWORD dwF  = 0;
		if (1 == _stscanf(sKey, CSD_CAM_CONTROL_FKT_FMT,&dwID))
		{
			// GF todo m_SmallWindows.Lock;
			CDisplayWindow* pDW = m_pServer->GetDocument()->GetVisionView()->GetDisplayWindow(
								 m_pServer->GetID(),dwID);
			_stscanf(sValue,_T("%08x"),&dwF);
			if (WK_IS_WINDOW(pDW))
			{
				pDW->PostMessage(WM_USER,WPARAM_CCF,dwF);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputVision::OnConfirmSetValue(CSecID id, const CString &sKey, const CString &sValue, DWORD dwServerData)
{
	OnConfirmGetValue(id,sKey,sValue,dwServerData);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputVision::SearchRelayPTZ()
{
	int i,c;
	c = GetNumberOfOutputs();

	for (i=0;i<c;i++)
	{
		const CIPCOutputRecord & rec = GetOutputRecordFromIndex(i);
		if (rec.IsRelay())
		{
			CString name = rec.GetName();
			name.MakeLower();
			if ( ((name.Find(_T("links"))!=-1) && (name.Find(_T("schwenk"))!=-1)) || 
				 ((name.Find(_T("left"))!=-1) && (name.Find(_T("pan"))!=-1))
			   )
			{
				m_RelayPTZ[(int)CCC_PAN_LEFT] = rec.GetID();
			}
			else if ( ((name.Find(_T("rechts"))!=-1) && (name.Find(_T("schwenk"))!=-1)) || 
					  ((name.Find(_T("right"))!=-1) && (name.Find(_T("pan"))!=-1))
					)
			{
				m_RelayPTZ[(int)CCC_PAN_RIGHT] = rec.GetID();
			}
			else if ( ((name.Find(_T("hoch"))!=-1) && (name.Find(_T("neig"))!=-1)) || 
				      ((name.Find(_T("up"))!=-1) && (name.Find(_T("tilt"))!=-1))
					)
			{
				m_RelayPTZ[(int)CCC_TILT_UP] = rec.GetID();
			}
			else if ( ((name.Find(_T("runter"))!=-1) && (name.Find(_T("neig"))!=-1)) || 
				      ((name.Find(_T("down"))!=-1) && (name.Find(_T("tilt"))!=-1))
					)
			{
				m_RelayPTZ[(int)CCC_TILT_DOWN] = rec.GetID();
			}
			else if ( ((name.Find(_T("nah"))!=-1) && (name.Find(_T("fokus"))!=-1)) || 
				      ((name.Find(_T("near"))!=-1) && (name.Find(_T("focus"))!=-1))
					)
			{
				m_RelayPTZ[(int)CCC_FOCUS_NEAR] = rec.GetID();
			}
			else if ( ((name.Find(_T("fern"))!=-1) && (name.Find(_T("fokus"))!=-1)) || 
				      ((name.Find(_T("far"))!=-1) && (name.Find(_T("focus"))!=-1))
					)
			{
				m_RelayPTZ[(int)CCC_FOCUS_FAR] = rec.GetID();
			}
			else if ( ((name.Find(_T("auf"))!=-1) && (name.Find(_T("blende"))!=-1)) || 
				      ((name.Find(_T("open"))!=-1) && (name.Find(_T("iris"))!=-1))
					)
			{
				m_RelayPTZ[(int)CCC_IRIS_OPEN] = rec.GetID();
			}
			else if ( ((name.Find(_T("zu"))!=-1) && (name.Find(_T("blende"))!=-1)) || 
				      ((name.Find(_T("close"))!=-1) && (name.Find(_T("iris"))!=-1))
					)
			{
				m_RelayPTZ[(int)CCC_IRIS_CLOSE] = rec.GetID();
			}
			else if ( (name.Find(_T("tele"))!=-1) || 
				      ((name.Find(_T("in"))!=-1) && (name.Find(_T("zoom"))!=-1))
					)
			{
				m_RelayPTZ[(int)CCC_ZOOM_IN] = rec.GetID();
			}
			else if ( (name.Find(_T("weit"))!=-1) || 
				      ((name.Find(_T("out"))!=-1) && (name.Find(_T("zoom"))!=-1))
					)
			{
				m_RelayPTZ[(int)CCC_ZOOM_OUT] = rec.GetID();
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputVision::IsRelayPTZ(CSecID id)
{
	int i;
	for (i=0;i<16;i++)
	{
		if ((m_RelayPTZ[i]!=SECID_NO_ID) && (m_RelayPTZ[i]==id))
		{
			return TRUE;
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputVision::IsPTZCamera(CSecID id)
{
	BOOL bReturn = FALSE;
	for (int i=0;i<GetNumberOfOutputs();i++)
	{
		const CIPCOutputRecord& rec = GetOutputRecordFromIndex(i);

		if (rec.GetID() == id)
		{
			if (rec.IsCamera())
			{
				if (rec.CameraIsFixed() == FALSE)
				{
					bReturn = TRUE;
				}
				break;
			}
		}
	}
	return bReturn;
}
//////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputVision::IsDirectShow(CSecID id)
{
	const CIPCOutputRecord& rec = GetOutputRecordFromSecID(id);
/*	if ((rec.GetID() == id) && (rec.GetFlags() & OREC_CAMERA_DIRECT_SHOW))
	{
		return TRUE;
	}
*/	return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputVision::RequestCameraControl(CameraControlCmd cmd, BOOL bClose)
{
	DoRequestSetRelay(m_RelayPTZ[(int)cmd],bClose);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputVision::OnIndicateError(DWORD dwCmd, CSecID id, 
								 CIPCError error, int iErrorCode,
								 const CString &sErrorMessage)
{
	if (!sErrorMessage.IsEmpty())
	{
		WK_TRACE_ERROR(_T("%s\n"),sErrorMessage);
	}

	if (error==CIPC_ERROR_UNABLE_TO_CONNECT)
	{
	}
	else if (error == CIPC_ERROR_ACCESS_DENIED)
	{
		CVisionView* pVV = m_pServer->GetDocument()->GetVisionView();

		if (WK_IS_WINDOW(pVV))
		{
			CDisplayWindow* pDW = pVV->GetDisplayWindow(m_pServer->GetID(),id);

			if (WK_IS_WINDOW(pDW))
			{
				pDW->OnIndicateError(dwCmd,error,iErrorCode);
			}
			else
			{
				WK_TRACE_ERROR(_T("confirm get mask no DisplayWindow %08lx\n"),id.GetID());
			}
		}
		else
		{
			WK_TRACE_ERROR(_T("confirm get mask no VisionView %08lx\n"),id.GetID());
		}

	}
	else
	{
		if (!sErrorMessage.IsEmpty())
		{
			CErrorMessage* pEM = new CErrorMessage(m_pServer->GetName(),sErrorMessage);
			theApp.m_ErrorMessages.SafeAddTail(pEM);
			CObjectView* pOV = m_pServer->GetDocument()->GetObjectView();
			if (WK_IS_WINDOW(pOV))
			{
				pOV->PostMessage(WM_USER,WPARAM_ERROR_MESSAGE);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputVision::ReferenceComplete()
{
	for (int i=0;i<GetNumberOfOutputs();i++)
	{
		const CIPCOutputRecord& rec = GetOutputRecordFromIndex(i);

		if (   rec.IsCamera() 
			&& rec.IsReference())
		{
			CString sPath;
			sPath.Format(_T("%s\\%08lx\\%08lx.jpg"),
				CNotificationMessage::GetWWWRoot(),
				m_pServer->GetHostID().GetID(),
				rec.GetID().GetID());
			if (!DoesFileExist(sPath))
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////////////
CSecID CIPCOutputVision::GetCamID(WORD wCamSubID)
{
	if (m_bGotInfo)
	{
		int i;
		WORD wCounter = 0;
		for (i=0;i<GetNumberOfOutputs();i++)
		{
			const CIPCOutputRecord& rec = GetOutputRecordFromIndex(i);
			
			if (rec.IsCamera())
			{
				if (   rec.IsEnabled()
					&& rec.IsOkay())
				{
					if (wCounter == wCamSubID)
					{
						return rec.GetID();
					}
				}
				wCounter++;
			}
		}
	}
	return SECID_NO_ID;
}

//////////////////////////////////////////////////////////////////////////////////////
//ML 24.9.99 Begin Insertation{
CSecID CIPCOutputVision::GetCamIDFromNr(WORD wCamNr)
{
	int nCameraCnt = 0;

	for (int i=0;i<GetNumberOfOutputs();i++)
	{
		const CIPCOutputRecord& rec = GetOutputRecordFromIndex(i);

		if (rec.IsCamera())
		{
			if (nCameraCnt == wCamNr)
			{
				return rec.GetID(); // Liefere die nte Kamera
			}
			nCameraCnt++;
		}
	}
	return SECID_NO_ID;
}

//////////////////////////////////////////////////////////////////////////////////////
CSecID CIPCOutputVision::GetRelayIDFromNr(WORD wRelayNr)
{
	int nRelayCnt = 0;

	for (int i=0;i<GetNumberOfOutputs();i++)
	{
		const CIPCOutputRecord& rec = GetOutputRecordFromIndex(i);

		if (rec.IsRelay())
		{
			if (nRelayCnt == wRelayNr)
			{
				return rec.GetID();	// Liefere das nte Relais
			}
			nRelayCnt++;
		}
	}
	return SECID_NO_ID;
}

//////////////////////////////////////////////////////////////////////////////////////
WORD CIPCOutputVision::GetNrFromCamID(CSecID camID)
{
	WORD wCamCnt = 0;

	for (int i=0;i<GetNumberOfOutputs();i++)
	{
		const CIPCOutputRecord& rec = GetOutputRecordFromIndex(i);

		if (rec.IsCamera())
		{
			if (rec.GetID() == camID)
			{
				return wCamCnt; // Liefere Nummer der Kamera
			}
			wCamCnt++;
		}
	}
	return (WORD)-1;
}

//////////////////////////////////////////////////////////////////////////////////////
WORD CIPCOutputVision::GetNrFromRelayID(CSecID relayID)
{
	WORD wRelayCnt = 0;

	for (int i=0;i<GetNumberOfOutputs();i++)
	{
		const CIPCOutputRecord& rec = GetOutputRecordFromIndex(i);

		if (rec.IsRelay())
		{
			if (rec.GetID() == relayID)
			{
				return wRelayCnt; // Liefere Nummer der Kamera
			}
			wRelayCnt++;
		}
	}
	return (WORD)-1;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputVision::OnConfirmSetRelay(CSecID relayID, BOOL bClosed)
{
	if (m_pServer->IsControlConnected())
	{
		m_pServer->GetControl()->ConfirmRelay(relayID.GetID(), (bClosed ? On : Off));
	}

	CIPCOutputClient::OnConfirmSetRelay(relayID, bClosed);
}
//ML 24.9.99 End Insertation}
//////////////////////////////////////////////////////////////////////////////////////
//GF 22.11.00 Um beim DTS-Empfänger nach Sequenzer die Bildanforderung wiederanzustoßen
void CIPCOutputVision::RestartJpegEncoding()
{
	RequestPictures(m_idActiveCam);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputVision::OnConfirmGetMask(CSecID camID, DWORD dwUserID, const CIPCActivityMask& mask)
{
	CVisionView* pVV = m_pServer->GetDocument()->GetVisionView();

	if (!WK_IS_WINDOW(pVV))
	{
		WK_TRACE_ERROR(_T("confirm get mask no VisionView %08lx\n"),camID.GetID());
		return;
	}
	CDisplayWindow* pDW = pVV->GetDisplayWindow(m_pServer->GetID(),camID);

	if (!WK_IS_WINDOW(pDW))
	{
		WK_TRACE_ERROR(_T("confirm get mask no DisplayWindow %08lx\n"),camID.GetID());
		return;
	}

	CMicoWindow* pMW = (CMicoWindow*)pDW;

	pMW->ConfirmGetMask(mask);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputVision::OnConfirmSetMask(CSecID camID, MaskType type, DWORD dwUserID)
{
	CVisionView* pVV = m_pServer->GetDocument()->GetVisionView();

	if (!WK_IS_WINDOW(pVV))
	{
		WK_TRACE_ERROR(_T("confirm get mask no VisionView %08lx\n"),camID.GetID());
		return;
	}
	CDisplayWindow* pDW = pVV->GetDisplayWindow(m_pServer->GetID(),camID);

	if (!WK_IS_WINDOW(pDW))
	{
		WK_TRACE_ERROR(_T("confirm get mask no DisplayWindow %08lx\n"),camID.GetID());
		return;
	}

	if (pDW->IsMico())
	{
		CMicoWindow* pMW = (CMicoWindow*)pDW;
		pMW->ConfirmSetMask(type);
	}
	else
	{
		WK_TRACE_ERROR(_T("confirm get mask no mico/jacob window %08lx\n"),camID.GetID());
	}
}

