// GuardTourProcess.cpp: implementation of the CGuardTourProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GuardTourProcess.h"
#include "GuardTourThread.h"
#include "IPCServerToVision.h"
#include "ProcessScheduler.h"
#include "ProcessSchedulerVideo.h"
#include "sec3.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGuardTourProcess::CGuardTourProcess(CProcessScheduler* pScheduler,CActivation *pActivation)
:CProcess(pScheduler)
{
	m_pActivation = pActivation;
	pActivation->SetActiveProcess(this);
	TRACE(_T("new CGuardTourProcess %s %d\n"),GetMacro().GetName(),GetMacro().IsKeepAlive());
	InitTimer();
	m_type = SPT_GUARD_TOUR;
	m_State = GTS_INITIALIZED;
	m_pCIPCServerToVision = NULL;
	m_pGuardTourThread = NULL;
	m_iCurrentHost = 0;
	m_dwCurrentTry = 0;
	m_dwCameraSwitchTime = 0;
	m_pConnectionRecord = NULL;
	m_wHosts.Append(GetMacro().GetIDs());
	m_sCameras.Append(GetMacro().GetCameras());
	m_iCurrentCamera = 0;
	m_dwStartWaitingForConnect = 0;
}
//////////////////////////////////////////////////////////////////////
CGuardTourProcess::~CGuardTourProcess()
{
	TRACE(_T("CGuardTourProcess::~CGuardTourProcess %s %s\n"),GetMacro().GetName(),NameOfEnum(m_State));
	if (   m_pActivation
		&& m_pActivation->GetActiveProcess() !=NULL)
	{
		TRACE("GT Process BL not null %08lx, %08lx\n",m_pActivation->GetActiveProcess(),this);
	}
	if (m_pCIPCServerToVision)
	{
		m_pCIPCServerToVision->RemoveProcess(this);
	}
	if (m_pGuardTourThread)
	{
		m_pGuardTourThread->StopThread();
		WK_DELETE(m_pGuardTourThread);
	}
	WK_DELETE(m_pConnectionRecord);
}
//////////////////////////////////////////////////////////////////////
LPCTSTR CGuardTourProcess::NameOfEnum(GuardTourState gts)
{
	switch (gts)
	{
		NAME_OF_ENUM(GTS_INITIALIZED);
		NAME_OF_ENUM(GTS_WAITING_FOR_CLIENT);
		NAME_OF_ENUM(GTS_CLIENT_STARTED);
		NAME_OF_ENUM(GTS_START_FETCH_THREAD);
		NAME_OF_ENUM(GTS_FETCHING);
		NAME_OF_ENUM(GTS_WAITING_FOR_CONNECT);
		NAME_OF_ENUM(GTS_CONNECTED);
		NAME_OF_ENUM(GTS_FINISHED);
	}
	return _T("unknown GuardTourState");
}
//////////////////////////////////////////////////////////////////////
void CGuardTourProcess::DoDisconnect()
{
	if (((CProcessSchedulerVideo*)GetScheduler())->RemoveGuardTourProcess(this))
	{
		// CIPCServerToVision wurde gefunden und evtl gelöscht
		m_pCIPCServerToVision = NULL;
	}
}
//////////////////////////////////////////////////////////////////////
void CGuardTourProcess::CleanUpGuardTour()
{
	if (m_pGuardTourThread)
	{
		if (m_pGuardTourThread->IsRunning())
		{
			m_pGuardTourThread->Cancel();
			m_pGuardTourThread->StopThread();
		}
		WK_DELETE(m_pGuardTourThread);
	}
	WK_DELETE(m_pConnectionRecord);
	m_dwCurrentTry = 0;
	m_iCurrentCamera = 0;
	m_wCameras.RemoveAll();
}
//////////////////////////////////////////////////////////////////////
void CGuardTourProcess::Run()
{
	switch (m_State)
	{
	case GTS_INITIALIZED:
		OnInitialized();
		break;
	case GTS_WAITING_FOR_CLIENT:
		OnWaitingForClient();
		break;
	case GTS_CLIENT_STARTED:
		OnClientStarted();
		break;
	case GTS_START_FETCH_THREAD:
		OnStartFetchThread();
		break;
	case GTS_FETCHING:
		OnFetching();
		break;
	case GTS_WAITING_FOR_CONNECT:
		OnWaitingForConnect();
		break;
	case GTS_CONNECTED:
		OnConnected();
		break;
	case GTS_FINISHED:
		OnFinished();
		break;
	}
	TRACE(_T("Guard Tour %s %s %08lx\n"),GetMacro().GetName(),NameOfEnum(m_State),m_pCIPCServerToVision);
}
//////////////////////////////////////////////////////////////////////
void CGuardTourProcess::OnPause()
{
	TRACE(_T("CGuardTourProcess::OnPause\n"));
}
//////////////////////////////////////////////////////////////////////
BOOL CGuardTourProcess::Terminate(BOOL bShutDown/*=FALSE*/)
{
	if (bShutDown)
	{
		m_State = GTS_FINISHED;
		OnFinished();
	}
	else if (!IsMarkedForTermination())
	{
		TRACE(_T("Guard Tour %s %s marked for termination\n"),
			GetMacro().GetName(),NameOfEnum(m_State));
		m_bMarkedForTermination = TRUE;
	}
	if (m_State == GTS_FINISHED)
	{
		CProcess::Terminate(bShutDown);
	}

	return m_State == GTS_FINISHED;
}
//////////////////////////////////////////////////////////////////////
void CGuardTourProcess::OnInitialized()
{
	if (IsMarkedForTermination())
	{
		m_State = GTS_FINISHED;
	}
	else
	{
		if (WK_IS_RUNNING(WK_APP_NAME_IDIP_CLIENT))
		{
			// improve speed
			m_State = GTS_CLIENT_STARTED;
			OnClientStarted();
		}
		else
		{
			if (CIPCServerControlClientSide::StartVision())
			{
				m_State = GTS_WAITING_FOR_CLIENT;
			}
			else
			{
				SetState(SPS_TERMINATED,WK_GetTickCount());
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CGuardTourProcess::OnWaitingForClient()
{
	if (IsMarkedForTermination())
	{
		m_State = GTS_FINISHED;
		OnFinished();
	}
	else if (WK_IS_RUNNING(WK_APP_NAME_IDIP_CLIENT))
	{
		m_State = GTS_CLIENT_STARTED;
	}
}
//////////////////////////////////////////////////////////////////////
void CGuardTourProcess::OnClientStarted()
{
	if (  !IsMarkedForTermination()
		&& (m_iCurrentHost<m_wHosts.GetSize())
		)
	{
		CleanUpGuardTour();
		if (m_pConnectionRecord)
		{
			WK_TRACE_ERROR(_T("m_pConnectionRecord not null\n"));
			WK_DELETE(m_pConnectionRecord);
		}
		m_pConnectionRecord = new CConnectionRecord();
		m_pConnectionRecord->SetDestinationHost(LOCAL_LOOP_BACK);

		m_pConnectionRecord->SetFieldValue(CRF_FULLSCREEN,GetMacro().GetGuardTourFullScreen() ? CSD_ON : CSD_OFF);
		m_pConnectionRecord->SetFieldValue(CRF_SAVE,GetMacro().GetGuardTourSave() ? CSD_ON : CSD_OFF);
		m_pConnectionRecord->SetFieldValue(CRF_EXCLUSIVE,GetMacro().GetGuardTourExclusiveCam() ? CSD_ON : CSD_OFF);
		m_pConnectionRecord->SetFieldValue(CRF_DISCONNECT,GetMacro().GetGuardTourDisconnect() ? CSD_ON : CSD_OFF);

		const CPermission*pPermission = GetMacro().GetPermission();
		if (pPermission)
		{
			m_pConnectionRecord->SetPermission(pPermission->GetName());
		}
		m_pConnectionRecord->SetTimeout(20*1000);


		CSecID idHost(SECID_GROUP_HOST,m_wHosts[m_iCurrentHost]);
		CWK_Profile wkp;
		CHostArray ha;
		ha.Load(wkp);
		CHost* pHost = ha.GetHost(idHost);
		if (pHost)
		{
			if (pHost->IsPINRequired())
			{
				CString sPIN = pHost->GetPIN();
				if (!sPIN.IsEmpty())
				{
					m_pConnectionRecord->SetFieldValue(_T("PIN"),sPIN);
				}
			}
			m_pConnectionRecord->SetSourceHost(pHost->GetName());

			CStringArray saCameras;

			SplitString(m_sCameras.GetAt(m_iCurrentHost),saCameras,_T(';'));

			if (SeparatSAToWA(saCameras,m_wCameras,_T('-')))
			{
				if (m_wCameras.GetSize()>0)
				{
					WORD wCamNr = (WORD)(m_wCameras[0]-1);
					CSecID id(SECID_GROUP_OUTPUT,wCamNr);
					m_pConnectionRecord->SetCamID(id);
				}
			}
			WK_TRACE(_T("Guard tour to %s first cam %d\n"),m_pConnectionRecord->GetSourceHost(),
				m_pConnectionRecord->GetCamID().GetSubID());

			if (m_pCIPCServerToVision)
			{
				WK_TRACE_ERROR(_T("deleting non NULL CIPCServerToVision\n"));
				WK_DELETE(m_pCIPCServerToVision);
			}

			BOOL bCreated = FALSE;
			m_pCIPCServerToVision = ((CProcessSchedulerVideo*)GetScheduler())->GetCIPCServerToVision(pHost->GetID(),bCreated);

			if (m_pCIPCServerToVision)
			{
				m_pConnectionRecord->SetInputShm(m_pCIPCServerToVision->GetShmName());
				m_pCIPCServerToVision->AddProcess(this);
				if (bCreated)
				{
					WK_TRACE(_T("Guard tour to %s not connected start fetch\n"),m_pConnectionRecord->GetSourceHost());
					// improve connection speed
					m_State = GTS_START_FETCH_THREAD;
					OnStartFetchThread();
				}
				else
				{
					m_State = GTS_WAITING_FOR_CONNECT;
					// to start with camera 0
					m_iCurrentCamera = -1;
					WK_TRACE(_T("Guard tour to %s already fetching/connected\n"),m_pConnectionRecord->GetSourceHost());
					m_dwStartWaitingForConnect = WK_GetTickCount();
					OnWaitingForConnect();
				}
			}
			else
			{
				WK_TRACE_ERROR(_T("cannot create/get ServerToVision\n"));
				m_State = GTS_FINISHED;
			}
		}
		else
		{
			WK_TRACE_ERROR(_T("guard tour host not found\n"));
		}
		m_iCurrentHost++;
	}
	else
	{
		m_State = GTS_FINISHED;
	}
}
/////////////////////////////////////////////////////////////////////////////////
void CGuardTourProcess::OnStartFetchThread()
{
	if (   !IsMarkedForTermination()
		&& m_pConnectionRecord
		&& m_pCIPCServerToVision)
	{
		if (m_pGuardTourThread)
		{
			WK_TRACE_ERROR(_T("guard tour fetch thread still running\n"));
			WK_DELETE(m_pGuardTourThread);
		}

		m_pCIPCServerToVision->SetFetching(TRUE);
		m_pGuardTourThread = new CGuardTourThread(this,m_pConnectionRecord);
		m_pGuardTourThread->StartThread();

		m_State = GTS_FETCHING;
	}
	else
	{
		m_State = GTS_FINISHED;
	}
}
//////////////////////////////////////////////////////////////////////
void CGuardTourProcess::OnFetching()
{
	if (m_pGuardTourThread)
	{
		if (m_pGuardTourThread->IsRunning())
		{
			if (IsMarkedForTermination())
			{
//				m_pGuardTourThread->Cancel();
			}
		}
		else
		{
			// thread beendet
			m_pGuardTourThread->Lock();
			CIPCFetchResult frAlarm(*m_pGuardTourThread->GetFetchResult());
			m_pGuardTourThread->DeleteFetchResult();
			m_pGuardTourThread->Unlock();
			
			WK_DELETE(m_pGuardTourThread);
			if (m_pCIPCServerToVision)
			{
				m_pCIPCServerToVision->SetFetching(FALSE);
			}
			
			if (frAlarm.IsOkay())
			{
				WK_TRACE(_T("Guard tour to %s fetch OK\n"),m_pConnectionRecord->GetSourceHost());
				m_State = GTS_WAITING_FOR_CONNECT;
				m_dwStartWaitingForConnect = WK_GetTickCount();
				m_dwCameraSwitchTime = WK_GetTickCount();
			}
			else
			{
				WK_TRACE(_T("Guard tour to %s fetch FAILED\n"),m_pConnectionRecord->GetSourceHost());
				if (m_dwCurrentTry<GetMacro().GetNrOfTries())
				{
					m_dwCurrentTry++;
					m_State = GTS_START_FETCH_THREAD;
				}
				else
				{
					DoDisconnect();
					m_State = GTS_CLIENT_STARTED;
				}
			}
		}
	}
	else
	{
		TRACE(_T("guard tour waiting for fetch %s\n"),m_pConnectionRecord->GetSourceHost());
	}
}
//////////////////////////////////////////////////////////////////////
void CGuardTourProcess::OnWaitingForConnect()
{
	if (   m_pCIPCServerToVision
		&& !m_pCIPCServerToVision->IsFetching()
		&& m_pCIPCServerToVision->GetIPCState() == CIPC_STATE_CONNECTED)
	{
		WK_TRACE(_T("Guard tour to %s connected\n"),m_pConnectionRecord->GetSourceHost());
		m_State = GTS_CONNECTED;
		m_dwCameraSwitchTime = WK_GetTickCount();
		m_iCurrentCamera++;

		// special case Dwell Time is 0 switch left 1-n cameras
		// camera 0 is in connection record
		// TODO! UF: all n cameras in connection record
		if (GetMacro().GetDwellTime() == 0)
		{
			for (;m_iCurrentCamera<m_wCameras.GetSize();m_iCurrentCamera++)
			{
				WORD wCamera = m_wCameras.GetAt(m_iCurrentCamera);
				CString sCam;
				sCam.Format(_T("%d"),wCamera-1);
				WK_TRACE(_T("Guard Tour switch camera <%s>[%s]\n"),m_pConnectionRecord->GetSourceHost(),sCam);
				m_pCIPCServerToVision->DoRequestSetValue(CSecID(),_T("cam"),sCam,1);
			}
			m_dwCameraSwitchTime = WK_GetTickCount();
		}
	}
	else if (IsMarkedForTermination())
	{
		m_State = GTS_FINISHED;
	}
	else
	{
		WK_TRACE(_T("Guard tour to %s time out waiting for connect\n"),m_pConnectionRecord->GetSourceHost());
		if (WK_GetTimeSpan(m_dwStartWaitingForConnect)>10*1000)
		{
			DoDisconnect();
			m_State = GTS_CLIENT_STARTED;
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CGuardTourProcess::OnConnected()
{
	if (m_pCIPCServerToVision)
	{
		if (IsMarkedForTermination())
		{
			if (GetMacro().IsKeepAlive())
			{
				m_State = GTS_FINISHED;
			}
			else if (WK_GetTimeSpan(m_dwCameraSwitchTime)>GetMacro().GetDwellTime()*1000)
			{
				m_State = GTS_FINISHED;
			}
		}
		else if (m_pCIPCServerToVision->GetIsMarkedForDelete())
		{
			TRACE(_T("Control disconnected continue with next host\n"));
			m_State = GTS_CLIENT_STARTED;
			// continue with next host
		}
		else
		{
			// start switching cams
			if (   m_iCurrentCamera == 0
				|| WK_GetTimeSpan(m_dwCameraSwitchTime)>GetMacro().GetDwellTime()*1000)
			{
				if (   m_iCurrentCamera<m_wCameras.GetSize()
					&& m_pCIPCServerToVision)
				{
					WORD wCamera = m_wCameras.GetAt(m_iCurrentCamera);
					CString sCam;
					sCam.Format(_T("%d"),wCamera-1);
					WK_TRACE(_T("Guard Tour switch camera <%s>[%s]\n"),m_pConnectionRecord->GetSourceHost(),sCam);
					m_pCIPCServerToVision->DoRequestSetValue(CSecID(),_T("cam"),sCam,1);
					m_iCurrentCamera++;
					m_dwCameraSwitchTime = WK_GetTickCount();
				}
				else 
				{
					if (   GetMacro().IsKeepAlive()
						|| (m_iCurrentHost<m_wHosts.GetSize()-1)
						)
					{
						WK_TRACE(_T("Guard tour to %s all cameras switched, disconnecting\n"),
							m_pConnectionRecord->GetSourceHost());
						// disconnect
						DoDisconnect();
						m_State = GTS_CLIENT_STARTED;
					}
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CGuardTourProcess::OnFinished()
{
	if (!GetMacro().IsKeepAlive())
	{
		if (   m_pCIPCServerToVision
			&& m_pCIPCServerToVision->GetIPCState()==CIPC_STATE_CONNECTED)
		{
			for (int i=0;i<m_wCameras.GetSize();i++)
			{
				WORD wCamera = m_wCameras.GetAt(i);
				CString sCam;
				sCam.Format(_T("%d"),wCamera-1);
				TRACE(_T("SWITCH %s-%s\n"),m_pConnectionRecord->GetSourceHost(),sCam);
				m_pCIPCServerToVision->DoRequestSetValue(CSecID(),_T("cam"),sCam,0);
			}
		}
	}
	CleanUpGuardTour();
	DoDisconnect();
	SetState(SPS_TERMINATED,WK_GetTickCount());
}
//////////////////////////////////////////////////////////////////////
void CGuardTourProcess::ControlDisconnected(CIPCServerToVision* pCIPC)
{
	WK_TRACE(_T("Guard tour to %s control disconnected\n"),m_pConnectionRecord->GetSourceHost());
	if (m_pCIPCServerToVision == pCIPC)
	{
		m_pCIPCServerToVision = NULL;
		m_State = GTS_CLIENT_STARTED;
	}
}
