// CameraGroup.cpp: implementation of the CCameraGroup class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DVProcess.h"
#include "CameraGroup.h"

#include "RequestThread.h"
#include "IPCDatabaseProcess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCameraGroup::CCameraGroup(LPCTSTR shmName, int wGroup, int size)
	: COutputGroup(shmName,wGroup) , m_RequestThread(CString(_T("RQ")) + CString(shmName))
{
	m_iNrActiveCameras = 0;
	m_dwScanMask = 0;
	m_dwStartScanTick = 0;
	m_bScanning = FALSE; 
	m_bMustScan = FALSE; 
	m_iCurrentSwitchTimeIndex = 0;
	m_dwSwitchTimes.SetSize(10);
	for (int i=0;i<m_dwSwitchTimes.GetSize();i++)
	{
		m_dwSwitchTimes.SetAt(i,0);
	}

	m_hSyncSemaphore = NULL;
	m_dwRequestID = 0;
	m_wFirstRequest = (WORD)-1;
	m_lOutStandingSync = 0;
	m_tickSyncSend = 0;
	CString sEventName(shmName);
	sEventName.Replace(_T("UnitOutputCamera"),_T("SyncEvent"));

	Lock();
	SetSize(size);
	for (WORD s=0;s<size;s++)
	{
		SetAt(s,new CCamera(this,CSecID(GetID(),s)));
	}
	Unlock();

	WK_TRACE(_T("creating %s\n"),sEventName);
	m_hSyncSemaphore = CreateSemaphore(NULL, 0, 100, sEventName);	// no manual reset
	m_RequestThread.SetCameraGroup(this);
	m_dwLastPictureConfirmed = 0;
}
//////////////////////////////////////////////////////////////////////
CCameraGroup::~CCameraGroup()
{
	WK_TRACE(_T("Average Switch time %d ms\n"),GetAverageSwitchtime());
	m_RequestThread.StopThread();
	WK_CLOSE_HANDLE(m_hSyncSemaphore);
	SafeDeleteAll();
}
//////////////////////////////////////////////////////////////////////
BOOL CCameraGroup::HasCameras() const
{
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
int CCameraGroup::GetNrOfActiveCameras()
{
	return m_iNrActiveCameras;
}
//////////////////////////////////////////////////////////////////////
void CCameraGroup::OnConfirmReset(WORD wGroupID)
{
	COutputGroup::OnConfirmReset(wGroupID);
	ScanCameras();
}
//////////////////////////////////////////////////////////////////////
void CCameraGroup::OnRequestDisconnect()
{
	for (int i=0;i<GetSize();i++)
	{
		CCamera* pCamera = (CCamera*)GetAtFast(i);
	//pCamera->DeletePictures();
		pCamera->Disable();
	}
	
	COutputGroup::OnRequestDisconnect();
	m_lOutStandingSync = 0;
}
//////////////////////////////////////////////////////////////////////
void CCameraGroup::SaveReferenceImage()
{
	for (int i=0;i<GetSize();i++)
	{
		CCamera* pCamera = (CCamera*)GetAtFast(i);
		pCamera->SaveReferenceImage();
	}
}
//////////////////////////////////////////////////////////////////////
void CCameraGroup::ScanCameras()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	m_bScanning = TRUE;
	m_dwStartScanTick = GetTickCount();
	m_evScanResult.ResetEvent();
	RequestScanCameras();
}
//////////////////////////////////////////////////////////////////////
void CCameraGroup::RequestScanCameras()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	DWORD dwMask = (1<<GetSize())-1;
	if (theApp.GetSettings().TraceScan())
	{
		WK_TRACE(_T("scanning cameras on %s\n"),GetShmName());
	}
	DoRequestGetValue(CSecID(GetID(),0),CSD_SCAN_FOR_CAMERAS,dwMask);
}
//////////////////////////////////////////////////////////////////////
void CCameraGroup::StartRequests()
{
	if (theApp.GetSettings().TraceUnitRequests())
	{
		WK_TRACE(_T("starting requests to %s\n"),GetShmName());
	}
	if (IsJacob())
	{
		for (int i=0;i<GetSize();i++)
		{
			CCamera* pCamera = (CCamera*)GetAtFast(i);
			if (pCamera->IsMD())
			{
				DoRequestSetValue(pCamera->GetUnitID(),CSD_DIB_TRANSFER,CSD_OFF);
			}
		}
		m_RequestThread.StartThread();
	}
	else if (IsSavic() )
	{
		m_RequestThread.StartThread();
	}
	else if (IsTasha())
	{
		StartTashaRequests();
	}
	else if (IsQ())
	{
		StartQRequests();
	}
}
//////////////////////////////////////////////////////////////////////
void CCameraGroup::StopRequests()
{
	if (theApp.GetSettings().TraceUnitRequests())
	{
		WK_TRACE(_T("stopping requests to %s\n"),GetShmName());
	}

	if (IsJacob())
	{
		for (int i=0;i<GetSize();i++)
		{
			CCamera* pCamera = (CCamera*)GetAtFast(i);
			if (pCamera->IsMD())
			{
				DoRequestSetValue(pCamera->GetUnitID(),CSD_DIB_TRANSFER,CSD_OFF);
			}
		}
		m_RequestThread.StopThread();
	}
	else if (IsSavic() )
	{
		m_RequestThread.StopThread();
	}
	else if (IsTasha())
	{
		StopTashaRequests();
	}
	else if (IsQ())
	{
		StopQRequests();
	}
}

//////////////////////////////////////////////////////////////////////
void CCameraGroup::StartQRequests()
{
	for (int i=0;i<GetSize();i++)
	{
		CCamera* pCamera = (CCamera*)GetAtFast(i);
		StartRequest(pCamera,pCamera->IsInputActive()); 
	}
}
//////////////////////////////////////////////////////////////////////
void CCameraGroup::StartTashaRequests()
{
	for (int i=0;i<GetSize();i++)
	{
		CCamera* pCamera = (CCamera*)GetAtFast(i);
		StartRequest(pCamera,pCamera->IsInputActive()); 
	}
}
//////////////////////////////////////////////////////////////////////
DWORD CCameraGroup::GetIFrameDefault(int iFPS)
{
	DWORD i = min(iFPS,5);
	switch (iFPS)
	{
	case 30:
		i = 6;
		break;
	case 25:
		i = 5;
		break;
	case 12:
		i = 4;
		break;
	case 10:
		i = 5;
		break;
	case 8:
		i = 4;
		break;
	case 6:
		i = 3;
		break;
	case 5:
		i = 5;
		break;
	case 4:
		i = 4;
		break;
	case 3:
		i = 3;
		break;
	case 2:
		i = 2;
		break;
	default:
		i = 1;
		break;
	}
	return i;
}
//////////////////////////////////////////////////////////////////////
void CCameraGroup::StartRequest(const CCamera* pCamera, BOOL bIsActive)
{
	if (   pCamera 
		&& pCamera->GetSubID()<GetSize())
	{	
		if (pCamera->IsActive())
		{
			if (IsTasha())
			{
				StartTashaRequest(pCamera,bIsActive);
			}
			else if (IsQ())
			{
				StartQRequest(pCamera,bIsActive);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CCameraGroup::StartTashaRequest(const CCamera* pCamera, BOOL bIsActive)
{
	Resolution res = RESOLUTION_CIF;

	switch (theApp.GetSettings().GetResolution())
	{
	case RESOLUTION_4CIF:
		res = RESOLUTION_4CIF;
		break;
	case RESOLUTION_2CIF:
		res = RESOLUTION_2CIF;
		break;
	case RESOLUTION_CIF:
		res = RESOLUTION_CIF;
		break;
	case RESOLUTION_QCIF:
		res = RESOLUTION_CIF;
		break;
	default:
		break;
	}
	Compression comp = COMPRESSION_1;
	switch (pCamera->GetCompression())
	{
	case COMPRESSION_1:
		comp = COMPRESSION_12; // 2 MBit
		break;
	case COMPRESSION_2:
		comp = COMPRESSION_16; // 1 MBit
		break;
	case COMPRESSION_3:
		comp = COMPRESSION_20; // 512 kBit
		break;
	}
	if (theApp.GetSettings().TraceUnitRequests())
	{
		WK_TRACE(_T("Card:%d request  %s\n"),
			LOBYTE(GetID()),pCamera->GetName());
	}
	int iFPS = pCamera->GetFPS();
	int iIFrameIntervall = GetIFrameDefault(iFPS);
	if (pCamera->GetNrOfTashaClients()>0)
	{
		iFPS = min(iFPS,12);
		iIFrameIntervall = GetIFrameDefault(iFPS);
	}
	DoRequestStartVideo(pCamera->GetUnitID(),
		res,
		comp,
		COMPRESSION_MPEG4,
		iFPS,
		iIFrameIntervall,
		0);
}
//////////////////////////////////////////////////////////////////////
void CCameraGroup::StartQRequest(const CCamera* pCamera, BOOL bIsActive)
{
	Resolution res = RESOLUTION_CIF;

	switch (theApp.GetSettings().GetResolution())
	{
	case RESOLUTION_4CIF:
		res = RESOLUTION_4CIF;
		break;
	case RESOLUTION_2CIF:
		res = RESOLUTION_2CIF;
		break;
	case RESOLUTION_CIF:
		res = RESOLUTION_CIF;
		break;
	case RESOLUTION_QCIF:
		res = RESOLUTION_CIF;
		break;
	default:
		break;
	}
	if (theApp.GetSettings().TraceUnitRequests())
	{
		WK_TRACE(_T("Card:%d request  %s\n"),
			LOBYTE(GetID()),pCamera->GetName());
	}
	int iFPS = 5;
	int iNrOfActiveCams = GetNrOfActiveCameras();

	if (m_idClientActiveCam == SECID_NO_ID)
	{
		if (iNrOfActiveCams>3)
		{
			iFPS = 100 / iNrOfActiveCams;
		}
		else
		{
			iFPS = 25;
		}
	}
	else
	{
		if (pCamera->GetClientID() == m_idClientActiveCam)
		{
			iFPS = 25;
		}
		else
		{
			if (iNrOfActiveCams>2)
			{
				iFPS = 75 / (iNrOfActiveCams-1);
			}
			else
			{
				iFPS = 25;
			}
		}
	}
	iFPS = min(25,iFPS);
	iFPS = min((int)pCamera->GetFPS(),iFPS);

	TRACE(_T("request Q %08lx %d, %d\n"),pCamera->GetUnitID(),iFPS,pCamera->GetFPS());

	DoRequestStartVideo(pCamera->GetUnitID(),
						res,
						COMPRESSION_NONE,
						COMPRESSION_YUV_422,
						iFPS,
						0,
						0);

}
//////////////////////////////////////////////////////////////////////
void CCameraGroup::StopTashaRequests()
{
	for (int i=0;i<GetSize();i++)
	{
		StopRequest((CCamera*)GetAtFast(i));
	}
}
//////////////////////////////////////////////////////////////////////
void CCameraGroup::StopQRequests()
{
	for (int i=0;i<GetSize();i++)
	{
		StopRequest((CCamera*)GetAtFast(i));
	}
}

//////////////////////////////////////////////////////////////////////
void CCameraGroup::StopRequest(const CCamera* pCamera)
{
	if (pCamera && pCamera->GetSubID()<GetSize())
	{	
		DoRequestStopVideo(pCamera->GetUnitID(),0);
	}
}
//////////////////////////////////////////////////////////////////////
void CCameraGroup::RescanCameras()
{
	// ML (04.02.2005)
	// Der Kamerascan wird aus 'RequestSync' herraus eingeleitet. Da die Tasha
	// aber keine Syncs benötigt, erfolgt der Kamerascan für die Tasha an dieser Stelle.
	if (   IsTasha()
		|| IsQ())
	{
		ScanCameras();
	}
	else // Alle anderen Boards.
	{
		if (!m_bScanning)
		{
			m_bMustScan = TRUE;
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CCameraGroup::Request()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	RequestSync();
}
//////////////////////////////////////////////////////////////////////
void CCameraGroup::RequestSync()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	if (m_bMustScan)
	{
		m_bMustScan = FALSE;
		m_bScanning = TRUE;
		ScanCameras();
	}
	else if (!m_bScanning)
	{
		// ATTENTION Request Thread !!!
		if (m_lOutStandingSync == 0)
		{
			CRequest r;
			int iNumPicts = 0;
			r.CalcRequests(this,m_wFirstRequest);

			if (r.GetNrRequests()>0)
			{
				// Do the Requests
				iNumPicts = DoRequestPictures(r);
				// Sleep a while
				Sleep(iNumPicts*40);
				// Do the Sync
				DoSync();
				// Wait for the sync
				DoWaitSync(iNumPicts);
			}
			else
			{
				// no requests
			}
		}
		else
		{
			// outstanding syncs
			WK_TRACE_ERROR(_T("m_lOutStandingSync=%d\n"),m_lOutStandingSync);
		}
	}
}
//////////////////////////////////////////////////////////////////////
int CCameraGroup::DoRequestPictures(CRequest& r)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	WORD wSubRequest = 0;
	int iNumPicts = 0;

	if (theApp.GetSettings().TraceUnitRequests())
	{
		WK_TRACE(_T("Card:%d Request %s\n"),LOBYTE(GetID()),r.ToString());
	}

//#define TRACE_REQUESTS
#ifdef TRACE_REQUESTS
	CString s,o;
#endif

	if (r.GetNrRequests()>0)
	{
		int c = r.IsSingleMD() ? 3 : 1;
		if (m_wFirstRequest != (WORD)-1)
		{
#ifdef TRACE_REQUESTS
			o.Format(_T("<%d,%d>,"),m_wFirstRequest,c);
			s += o;
#endif
			RequestImage(c, m_wFirstRequest,r.GetRequest(0), MAKELONG(++wSubRequest,r.GetID()));
			iNumPicts += c;
		}
		if (r.GetNrRequests()>1)
		{
			for (int i=0;i<r.GetNrRequests()-1;i++)
			{
#ifdef TRACE_REQUESTS
				o.Format(_T("<%d,%d>,"),m_wFirstRequest,c);
				s += o;
#endif
				RequestImage(c, r.GetRequest(i), r.GetRequest(i+1), MAKELONG(++wSubRequest,r.GetID()));
				iNumPicts += c;
			}
			m_wFirstRequest = r.GetRequest(r.GetNrRequests()-1);
		}
		else
		{
#ifdef TRACE_REQUESTS
			o.Format(_T("<%d,%d>"),r.GetRequest(0),c);
			s += o;
#endif
			RequestImage(c, r.GetRequest(0), r.GetRequest(0), MAKELONG(++wSubRequest,r.GetID()));
			iNumPicts += c;
			m_wFirstRequest = (WORD)-1;
		}
	}
#ifdef TRACE_REQUESTS
	TRACE(_T("R = {%s}\n"),s);
#endif

	return iNumPicts;
}
//////////////////////////////////////////////////////////////////////
void CCameraGroup::DoSync()
{
	CCallStackEntry cse(_T(__FUNCTION__));
	m_tickSyncSend = WK_GetTickCount();
	m_lOutStandingSync++;
	if (theApp.GetSettings().TraceSync())
	{
		WK_TRACE(_T("%s DoRequestSync %d\n"),GetShmName(),m_lOutStandingSync);
	}
	DoRequestSync(m_tickSyncSend,1234,m_lOutStandingSync);
}
//////////////////////////////////////////////////////////////////////
void CCameraGroup::DoWaitSync(int iNumPicts)
{
	CCallStackEntry cse(_T(__FUNCTION__));
	CWK_StopWatch w;
	// gesamt 1 Sekunde
	int c = 10;
	while (    (m_lOutStandingSync > 0)
			&& (GetIPCState()==CIPC_STATE_CONNECTED)
			&& (c>0)
			&& m_RequestThread.IsRunning())
	{
		DWORD dwRet = WaitForSingleObject(m_hSyncSemaphore,250);
		if (dwRet == WAIT_OBJECT_0) 
		{
			m_lOutStandingSync--;
		}
		c--;
	}
	w.StopWatch();
	if (theApp.GetSettings().TraceSync() && (m_lOutStandingSync == 0))
	{
		WK_TRACE(_T("%s ConfirmSync %d %d \n"),GetShmName(),w.GetElapsedMS(),m_lOutStandingSync);
	}


	if (    m_RequestThread.IsRunning() 
		&& (GetIPCState() == CIPC_STATE_CONNECTED)
		&& (iNumPicts>0))
	{
		DWORD dwMSperCam = w.GetElapsedMS() / iNumPicts;
		m_dwSwitchTimes.SetAt(m_iCurrentSwitchTimeIndex++,dwMSperCam);
		if (m_iCurrentSwitchTimeIndex>=m_dwSwitchTimes.GetSize())
		{
			m_iCurrentSwitchTimeIndex = 0;
		}
		if (dwMSperCam>200)
		{
//			WK_TRACE(_T("%s wait for image %d ms\n"),GetShmName(),dwMSperCam);
		}
		if (theApp.GetSettings().StatSwitch())
		{
			WK_STAT_LOG(_T("SwitchTime"),dwMSperCam,GetShmName());
		}
		if ((0 == c) && (m_lOutStandingSync>0))
		{
			WK_TRACE_ERROR(_T("%s time out waiting for sync %d ms\n"),GetShmName(),w.GetElapsedMS());
			m_lOutStandingSync = 0;
		}
	}
	// await for CIPC images
	Sleep(10);
}
//////////////////////////////////////////////////////////////////////
void CCameraGroup::RequestImage(int iCount, WORD id, WORD next, DWORD dwID)
{
/*
	if (theApp.TraceUnitRequests())
	{
		WK_TRACE(_T("Card:%d request Frames:%d Current:%d Next:%d\n"),LOBYTE(GetID()),iCount,id,next);
	}
*/
	CompressionType ct = COMPRESSION_UNKNOWN;
	Resolution res = RESOLUTION_2CIF;
	Compression comp = COMPRESSION_1;

	// Resolution
	res = theApp.GetSettings().GetResolution();
	
	if (IsSavic())
	{
		if (theApp.GetSettings().GetRequestDIBs())
		{
			ct = COMPRESSION_YUV_422;
		}
		else
		{
			ct = COMPRESSION_JPEG;
		}
		// Die Bildkompression bei Savic wird über die
		// CCamera Klasse und die JPEG Statistik erledigt.
	}
	else if (IsJacob())
	{
		// Jacob macht immer JPEG
		ct = COMPRESSION_JPEG;

		if (IsInArray(id, GetSize()))
		{
			// Compression
			switch (((CCamera*)GetAtFast(id))->GetCompression())
			{
			case COMPRESSION_1:
				comp = COMPRESSION_1; // 50/25 kB
				break;
			case COMPRESSION_2:
				comp = COMPRESSION_5; // 38/19 kB
				break;
			case COMPRESSION_3:
				comp = COMPRESSION_12; // 24/12kB
				break;
			}
		}
	}
	DoRequestUnitVideo(GetID(),(BYTE)id,(BYTE)next,
					   res,
					   comp,
					   ct,
					   iCount,
					   dwID,
					   FALSE);
}
//////////////////////////////////////////////////////////////////////
void CCameraGroup::ScanResult(DWORD dwMask)
{
	if (m_dwScanMask != dwMask)
	{
		m_iNrActiveCameras = 0;
		if (theApp.GetSettings().TraceScan())
		{
			WK_TRACE(_T("%s new camera mask %08lx scan in %d ms\n"),GetShmName(),dwMask,GetTimeSpan(m_dwStartScanTick));
		}

		DWORD dwOneBit = 1;
		for (WORD i=0; i<GetSize(); i++, dwOneBit <<= 1) 
		{
			CCamera* pOutput = (CCamera*)GetAtFast(i);
			// set the new state
			if ((dwMask & dwOneBit)!=0)
			{
				m_iNrActiveCameras++;
				if (!pOutput->IsActive())
				{
					WK_TRACE(_T("camera %s changed to ON\n"),pOutput->GetName());
					pOutput->SetActive(TRUE);
					if (pOutput->GetMode() == COutput::OM_MD)
					{
						DoRequestSetValue(pOutput->GetUnitID(), CSD_MD, CSD_ON);
					}
					else
					{
						DoRequestSetValue(pOutput->GetUnitID(), CSD_MD, CSD_OFF);
					}
				}
			}
			else
			{
				if (pOutput->IsActive())
				{
					WK_TRACE(_T("camera %s changed to OFF\n"),pOutput->GetName());
				}
				pOutput->SetActive(FALSE);
				DoRequestSetValue(pOutput->GetUnitID(), CSD_MD, CSD_OFF);
			}
		} // for mask
		m_dwScanMask = dwMask;
		StartRequests();
		theApp.SetReset(FALSE,TRUE,TRUE);
	}
	else
	{
		// nothing changed from scan to scan
		if (theApp.GetSettings().TraceScan())
		{
			WK_TRACE(_T("%s Same Camera Mask %08lx\n"),GetShmName(),dwMask);
		}
	}
	m_bScanning = FALSE;
	m_evScanResult.SetEvent();
}
//////////////////////////////////////////////////////////////////////
HANDLE CCameraGroup::ScanEvent()
{
	return m_evScanResult;
}
//////////////////////////////////////////////////////////////////////
void CCameraGroup::OnConfirmGetValue(CSecID id, // might be used as group ID only
									const CString &sKey,
									const CString &sValue,
									DWORD dwServerData)
{
	//ML dwServerData wird für die Scanmaske benutzt!
	// if ((sKey == CSD_SCAN_FOR_CAMERAS) && (dwServerData == 0))
	if (sKey == CSD_SCAN_FOR_CAMERAS)
	{
		DWORD dwMask = 0;

		if (1 == _stscanf(sValue,_T("%lu"),&dwMask))
		{
			ScanResult(dwMask);
			if (theApp.GetSettings().TraceScan())
			{
				WK_TRACE(_T("camera scan result %08lx %s\n"),dwMask,GetShmName());
			}
		}
		else
		{
//			m_dwScanMask = 0;
		}
	}
	else
	{
		// call base class
		COutputGroup::OnConfirmGetValue(id,sKey,sValue,dwServerData);
	}
}
//////////////////////////////////////////////////////////////////////
void CCameraGroup::ClearActiveCam(WORD wSubID)
{
	DWORD dwClear = 1<<wSubID;
	if (m_dwScanMask & dwClear)
	{
		m_dwScanMask &= ~dwClear;
		m_iNrActiveCameras--;
	}
}
//////////////////////////////////////////////////////////////////////
void CCameraGroup::SetActiveCam(WORD wSubID)
{
	DWORD dwClear = 1<<wSubID;
	if ((m_dwScanMask & dwClear) == 0)
	{
		m_dwScanMask |= dwClear;
		m_iNrActiveCameras++;
	}
}
//////////////////////////////////////////////////////////////////////
void CCameraGroup::SetClientActiveCam(CSecID id)
{
	if (m_idClientActiveCam != id)
	{
		m_idClientActiveCam = id;
		if (IsQ())
		{
			StartQRequests();
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CCameraGroup::OnConfirmJpegEncoding(const CIPCPictureRecord &pict,
										 DWORD dwUserData, 
										 CSecID idArchive)
{
	if (!theApp.IsUpAndRunning())
	{
		return;
	}
	WORD wSubID = pict.GetCamID().GetSubID();
	
	if (theApp.GetSettings().TraceUnitConfirms())
	{
		WK_TRACE(_T("confirm %d %d %d,%d, %s %d Bytes\n"),wSubID,pict.GetBlockNr(),pict.GetTimeStamp().GetSecond(),
			pict.GetTimeStamp().GetMilliseconds(),NameOfEnum(pict.GetCompressionType()),
			pict.GetDataLength());
	}
	
	if (wSubID<GetSize())
	{
		CCamera* pCamera = (CCamera*)GetAtFast(wSubID);
		pCamera->NewPicture(pict,0,0);
		if (pict.GetJobTime()==0)
		{
			//				WK_TRACE(_T("cam %d has no signal\n"),wSubID);
		}
	}
	m_dwLastPictureConfirmed = GetTickCount();
}
//////////////////////////////////////////////////////////////////////
void CCameraGroup::OnIndicateLocalVideoData(CSecID camID, 
										  WORD wXSize, 
										  WORD wYSize, 
										  const CIPCPictureRecord &pict)
{
	if (!theApp.IsUpAndRunning())
	{
		return;
	}
	WORD wSubID = pict.GetCamID().GetSubID();
	
	if (theApp.GetSettings().TraceUnitConfirms())
	{
		WK_TRACE(_T("confirm %d %d %d %d,%d\n"),
			wSubID,
			(int)pict.GetPictureType(),
			pict.GetBlockNr(),
			pict.GetTimeStamp().GetSecond(),
			pict.GetTimeStamp().GetMilliseconds());
	}
	
	if (wSubID<GetSize())
	{
		CCamera* pCamera = (CCamera*)GetAtFast(wSubID);
		pCamera->NewDIBPicture(wXSize,wYSize,pict);
		if (pict.GetJobTime()==0)
		{
			//				WK_TRACE(_T("cam %d has no signal\n"),wSubID);
		}
	}
	m_dwLastPictureConfirmed = GetTickCount();
}
//////////////////////////////////////////////////////////////////////
void CCameraGroup::OnIndicateVideo(const CIPCPictureRecord& pict,
									DWORD dwMDX,
									DWORD dwMDY,
									DWORD dwUserData,
									CSecID idArchive)
{
	if (!theApp.IsUpAndRunning())
	{
		return;
	}
	WORD wSubID = pict.GetCamID().GetSubID();

	if (theApp.GetSettings().TraceUnitConfirms())
	{
		WK_TRACE(_T("confirm %d %d %08lx %02d:%02d:%02d,%03d %s-Frame\n"),
			wSubID,
			pict.GetBlockNr(),
			dwUserData,
			pict.GetTimeStamp().GetHour(),
			pict.GetTimeStamp().GetMinute(),
			pict.GetTimeStamp().GetSecond(),
			pict.GetTimeStamp().GetMilliseconds(),
			(pict.GetPictureType()==SPT_DIFF_PICTURE)?_T("P"):_T("I"));
	}

	if (wSubID<GetSize())
	{
		CCamera* pCamera = (CCamera*)GetAtFast(wSubID);
		if (dwUserData == 0)
		{
			// storage and IP
			pCamera->NewPicture(pict,dwMDX,dwMDY);
		}
		else if (dwUserData == 1)
		{
			/*
			WK_TRACE(_T("confirm %d %d %08lx %02d:%02d:%02d,%03d %s-Frame %d Bytes\n"),
				wSubID,
				pict.GetBlockNr(),
				dwUserData,
				pict.GetTimeStamp().GetHour(),
				pict.GetTimeStamp().GetMinute(),
				pict.GetTimeStamp().GetSecond(),
				pict.GetTimeStamp().GetMilliseconds(),
				(pict.GetPictureType()==SPT_DIFF_PICTURE)?_T("P"):_T("I"),
				pict.GetDataLength());*/
			pCamera->NewTashaClientPicture(pict,dwMDX,dwMDY);
		}
	}
	m_dwLastPictureConfirmed = GetTickCount();
}
//////////////////////////////////////////////////////////////////////
BOOL CCameraGroup::IsImageTimeout()
{
	BOOL bRet = FALSE;

	if (   m_dwLastPictureConfirmed != 0
		&& GetNrOfActiveCameras()>0)
	{
		// 30 Sekunden lang kein Bild ???
		bRet = GetTimeSpan(m_dwLastPictureConfirmed) > (30 * 1000);
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CCameraGroup::IsRequestThreadRunning()
{
	if (IsJacob() || IsSavic())
	{
		if (m_RequestThread.IsRunning())
		{
			return !m_RequestThread.IsTimedOut();
		}
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
void CCameraGroup::OnConfirmGetMask(CSecID camID, DWORD dwUserID, const CIPCActivityMask& mask)
{
	if (!theApp.IsUpAndRunning())
	{
		return;
	}
	COutputClient* pClient = theApp.GetOutputClients().GetOutputClient(dwUserID);
	if (pClient && pClient->IsConnected() && IsInArray(camID.GetSubID(), GetSize()))
	{
		CIPCOutputDVClient* pCIPCOutputDVClient = pClient->GetCIPC();
		CCamera* pOutput = (CCamera*)GetAtFast(camID.GetSubID());
		pCIPCOutputDVClient->DoConfirmGetMask(pOutput->GetClientID(),0,mask);
	}
	else
	{
		WK_TRACE_ERROR(_T("OnConfirmGetMask %08lx Client no longer connected\n"),camID.GetID());
	}
}
//////////////////////////////////////////////////////////////////////
void CCameraGroup::OnConfirmSetMask(CSecID camID, MaskType type, DWORD dwUserID)
{
	if (!theApp.IsUpAndRunning())
	{
		return;
	}
	COutputClient* pClient = theApp.GetOutputClients().GetOutputClient(dwUserID);
	if (pClient && pClient->IsConnected()&& IsInArray(camID.GetSubID(), GetSize()))
	{
		CIPCOutputDVClient* pCIPCOutputDVClient = pClient->GetCIPC();
		CCamera* pOutput = (CCamera*)GetAtFast(camID.GetSubID());
		pCIPCOutputDVClient->DoConfirmSetMask(pOutput->GetClientID(),type,0);
	}
}
//////////////////////////////////////////////////////////////////////
DWORD CCameraGroup::GetAverageSwitchtime()
{
	DWORD dwSum = 0;
	DWORD dwCount = 0;
	for (int i=0;i<m_dwSwitchTimes.GetSize();i++)
	{
		DWORD dwValue = m_dwSwitchTimes[i];
		if (dwValue>0)
		{
			dwSum += dwValue;
			dwCount++;
		}
	}
	if (dwCount>0)
	{
		return dwSum/ dwCount;
	}
	else
	{
		return 60;
	}
}
//////////////////////////////////////////////////////////////////////
void CCameraGroup::OnIndicateError(DWORD dwCmd, CSecID id, 
									CIPCError error, int iErrorCode,
									const CString &sErrorMessage)
{
	WK_TRACE(_T("CCameraGroup::OnIndicateError %s, %s, CamID=0x%x %s\n"),GetShmName(),NameOfCmd(dwCmd), id, sErrorMessage);

	if ((error == CIPC_ERROR_CAMERA_NOT_PRESENT) || ((error == CIPC_ERROR_VIDEO_STREAM_ABORTED)))
	{
		if (IsInArray(id.GetSubID(), GetSize()))
		{
			CCamera* pCamera = (CCamera*)GetAtFast(id.GetSubID());
			if (iErrorCode == 0)	// Camerasignal or videostream aborted
			{
				pCamera->Disable();
				StopRequest(pCamera);
				// camera signal loss
				// pause processes
			}
			else if (iErrorCode == 1)	// Camerasignal or videostream resumed
			{
				// ML 13.05.2004 Start request of that camera and perform a camera scan
				// camera signal present
				// restart processes
				pCamera->Enable();
				StartRequest(pCamera,pCamera->IsInputActive());
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CCameraGroup::OnResolutionChanged(Resolution resolution)
{
	if (IsTasha())
	{
		StartTashaRequests();
	}
	else if (IsQ())
	{
		StartQRequests();
	}
}
