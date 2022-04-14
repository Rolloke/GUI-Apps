// CallProcess.cpp: implementation of the CCallProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sec3.h"

#include "CallProcess.h"
#include "ProcessScheduler.h"

// for DoAlarm
#include "CIPCServerControlClientSide.h"
#include "CIPCServerControlServerSide.h"
#include "CIPCInputServerClient.h"
#include "CIPCOutputServerClient.h"
#include "input.h"
#include "wk_msg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
volatile BOOL CCallProcess::ms_bFetching=FALSE;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCallProcess::CCallProcess(CProcessScheduler* pScheduler,CActivation *pActivation, CompressionType ct)
 : CProcess(pScheduler)
{
	m_pInputClient = NULL;
	m_pOutputClient = NULL;
	m_alarmCallState = ACS_INITIALIZED;
	m_pThread = NULL;
	m_bThreadRun = FALSE;
	m_bGotAlarmConnection = FALSE;
	m_tickLastAlarmCallTry = 0;
	m_tickLastAlarmCallFailure = 0;
	m_iCurrentCalledHostIndex = 0;
	m_pAlarmCallRecord = NULL;

	m_pActivation = pActivation;
	pActivation->SetActiveProcess(this);

	InitTimer();

	m_type = SPT_ALARM_CALL;
	m_CompressionType = ct;
	m_iCurrentCalledHostIndex = 0;
}
//////////////////////////////////////////////////////////////////////
CCallProcess::~CCallProcess()
{
	StopAlarmCallThread();
	WK_DELETE(m_pAlarmCallRecord);

	if (m_alarmCallState != ACS_SUCCESS)
	{
		TRACE(_T("CleanUpCIPC %s\n"),NameOfEnum(m_alarmCallState));
		CleanUpCIPC();
	}
}
LPCTSTR CCallProcess::NameOfEnum(AlarmCallState state)
{
	switch (state) 
	{
	NAME_OF_ENUM(ACS_INITIALIZED);
	NAME_OF_ENUM(ACS_TRYING);
	NAME_OF_ENUM(ACS_SUCCESS);
	NAME_OF_ENUM(ACS_FAILED);
	default:
		return _T("");
	}
}
//////////////////////////////////////////////////////////////////////
void CCallProcess::Run()
{
	BOOL bAlreadyConnected = FALSE;
	BOOL bCameraSelected = FALSE;

	//TRACE(_T("CCallProcess::Run %s %s\n"),::NameOfEnum(GetState()),NameOfEnum(m_alarmCallState));

	switch (m_alarmCallState) 
	{
		case ACS_INITIALIZED:
			// init a seperate thread for FetchAlarmConnection
			// so the process scheduler is not blocked by the call
			if (!ms_bFetching)
			{
				PrepareAlarmCall(bAlreadyConnected,bCameraSelected);
				if (!bAlreadyConnected) 
				{
					m_alarmCallState=ACS_TRYING;
					m_tickLastAlarmCallTry=WK_GetTickCount();
					m_bThreadRun=TRUE;
					m_pThread = AfxBeginThread(AlarmCallThreadFunction, this);
					m_pThread->m_bAutoDelete = FALSE;
				} 
				else if (bCameraSelected)
				{
					// like success
					m_alarmCallState = ACS_SUCCESS;	// OOPS used ?
					StopAlarmCallThread();
				}
			}
			// the thread will try to make the call and set a new state
			break;
		case ACS_TRYING:
			// keep on waiting, the thread will tell about the result
			break;
		case ACS_SUCCESS:
			// fine! and wait for terminate keep alive or detector
			StopAlarmCallThread();
			break;
		case ACS_FAILED:
			// intervall processes increment the host after each pause
			// non interval try one host after another
			if (GetMacro().IsIntervall()) 
			{
				// warte zur nächsten pause
			}
			else
			{
				Restart();
				NextHost();
				m_alarmCallState = ACS_INITIALIZED;
			}
			break;
		default:
			WK_TRACE(_T("Invalid AlarmCallState %d\n"),m_alarmCallState);
	}	// end of state switch
}
/////////////////////////////////////////////////////////////////////////////
void CCallProcess::OnPause()
{
	if (m_alarmCallState != ACS_SUCCESS)
	{
		NextHost();
		m_alarmCallState = ACS_INITIALIZED;
		m_tickLastAlarmCallFailure = 0;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCallProcess::OnStateChanging(SecProcessState newState,BOOL bShutDown)
{
//	TRACE(_T("CCallProcess::OnStateChanging %s\n"),::NameOfEnum(newState));
	if (   !bShutDown
		&& newState == SPS_TERMINATED)
	{
		// request unselect camera
		CSecID idCam = m_pActivation->GetOutputID();
		if (idCam.IsOutputID())
		{
			CIPCOutputClientsArray& clients = theApp.GetOutputClients();
			for (int i=0;i<clients.GetSize();i++)
			{
				if (clients[i] == m_pOutputClient)
				{
					if (!m_pProcessScheduler->FindCallProcess(m_pOutputClient,this))
					{
						m_pOutputClient->SelectCamera(SECID_NO_ID);
					}
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCallProcess::NextHost()
{
	// start at first host or move on to next host
	CleanUpCIPC();
	m_iCurrentCalledHostIndex = (m_iCurrentCalledHostIndex+1)%GetMacro().GetIDs().GetSize();
}
/////////////////////////////////////////////////////////////////////////////
void CCallProcess::CleanUpCIPC()
{
	// clean up
	TRACE(_T("CleanUpCIPC\n"));
	if (m_pInputClient) 
	{
		theApp.GetServerControl()->RemoveMe(m_pInputClient);
		m_pInputClient=NULL;
	}
	if (m_pOutputClient) 
	{
		theApp.GetServerControl()->RemoveMe(m_pOutputClient);
		m_pOutputClient=NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////
// a special prepare is needed to create the clients outside
// of the call thread, else the receiver will detect a thread end
// which will cause a CIPC disconnect (the sender's thread terminated)
void CCallProcess::PrepareAlarmCall(BOOL& bAlreadyConnected, BOOL& bCameraSelected)
{
	bAlreadyConnected = FALSE;
	bCameraSelected = FALSE;

	const CWordArray& hosts = GetMacro().GetIDs();

	if (m_iCurrentCalledHostIndex > hosts.GetSize()) {
		WK_TRACE_ERROR(_T("internal error host index is %d, not within 0..%d\n"),
			m_iCurrentCalledHostIndex,
			hosts.GetSize()-1
			);
	}
	CSecID hostID(SECID_GROUP_HOST,(WORD)hosts[m_iCurrentCalledHostIndex]);
	m_idCurrentHost = hostID;
	CWK_Profile wkp;
	CHostArray ha;
	BOOL bLocalUnits = FALSE;
	ha.Load(wkp);
	const CHost *pHost = ha.GetHost(hostID);

	if (pHost==NULL) 
	{
		WK_TRACE_ERROR(_T("Invalid host id %x for alarm call\n"),hostID.GetID());
		return;	// <<< EXIT >>> OOPS
	}

	const CPermission *pPermission = GetMacro().GetPermission();
	// PRE != NULL see CProcessMacro::IsValid()

	CUser dummyUser;
	dummyUser.SetName(_T("Alarm"));
	dummyUser.SetPermissionID(pPermission->GetID());

	if (pHost->IsISDN())
	{
		HWND hWnd = FindWindow(WK_APP_NAME_LAUNCHER, NULL);
		if (hWnd)
		{
			::PostMessage(hWnd, LAUNCHER_APPLICATION, WAI_ISDN_UNIT, MAKELONG(WM_ISDN_B_CHANNELS, 0));
		}
	}


	WK_DELETE(m_pAlarmCallRecord);	// OOPS any Lock ?
	
	DWORD dwCallTime = GetMacro().GetRecordSlice();

	if (dwCallTime>2000)
	{
		dwCallTime -= 2000;
	}

	m_pAlarmCallRecord = new CConnectionRecord(
								pHost->GetNumber(),	// remote host
								dummyUser,		// user
								pPermission->GetName(),	// permission
								pPermission->GetPassword(),	// password not used
								dwCallTime);
	if (_tcscmp(pHost->GetNumber(),_T("0"))==0) 
	{
		m_pAlarmCallRecord->SetDestinationHost(LOCAL_LOOP_BACK);
	}

	// What kind of options ?
	DWORD dwOptions =	 SCO_WANT_RELAYS
					   | SCO_WANT_CAMERAS_BW 
					   | SCO_WANT_CAMERAS_COLOR
					   | SCO_NO_STREAM
					   | SCO_JPEG_AS_H263
					   | SCO_MULTI_CAMERA;

	m_pAlarmCallRecord->SetOptions(dwOptions);

	for (int ai=0;ai<theApp.GetOutputClients().GetSize() && bAlreadyConnected==FALSE;ai++) 
	{
		CIPCOutputServerClient* pCIPC = theApp.GetOutputClients()[ai];
		CString sRemoteHost = pCIPC->GetRemoteHost();
		if (0 == sRemoteHost.CompareNoCase(pHost->GetName()))
		{
			bAlreadyConnected = TRUE;
			m_pOutputClient = pCIPC;
			if (m_pActivation)
			{
				CSecID idCam = m_pActivation->GetOutputID();
				CSecID idNot = m_pActivation->GetNotificationID();
				if (idCam.IsOutputID())
				{
					bCameraSelected = theApp.GetOutputClients()[ai]->SelectCamera(idCam);
				}
				// inform Client about Notification URL
				if (idNot.IsNotificationMessageID())
				{
					CNotificationMessageArray nma;
					CWK_Profile wkp;
					CNotificationMessage* pNM;
					nma.Load(wkp);
					pNM = nma.GetNotificationMessage(idNot);
					if (pNM)
					{
						CString sURL = pNM->GetURL();
						CString sMessage = pNM->GetMessage();
						if (!sURL.IsEmpty())
						{
							if (pCIPC->GetIPCState() == CIPC_STATE_CONNECTED)
							{
								theApp.GetOutputClients()[ai]->DoRequestSetValue(idNot,NM_PARAMETER,sURL);
								WK_TRACE(_T("Switch Notification URL for existing alarm connection %s\n"),sRemoteHost);
								bCameraSelected = TRUE;
							}
							else
							{
								TRACE(_T("client not yet connected for notification message %s\n"),pCIPC->GetRemoteHost());
							}
						}
						else if (!sMessage.IsEmpty())
						{
							CInput* pInput = GetAlarmInput();
							if (pInput)
							{
								CIPCFields& fields = pInput->GetFields();
								fields.Lock();
								int c = fields.GetSize();
								if (c>0)
								{
									sMessage += _T(": ");
									for (int i=0;i<c;i++)
									{
										sMessage += fields.GetAtFast(i)->ToString();
										if (i<c-1)
										{
											sMessage += _T(", ");
										}
									}
								}
								fields.Unlock();
							}
							theApp.GetOutputClients()[ai]->DoRequestSetValue(idNot,NM_MESSAGE,sMessage);
							WK_TRACE(_T("Switch Notification Message for existing alarm connection %s\n"), sRemoteHost);
						}
					}
				}
			}
		}
	}

	if (bAlreadyConnected==FALSE) 
	{
		// now create the clients first, the created
		// shared memory names are passed to the FetchAlarm.....
		BOOL bInput = TRUE;
		BOOL bOutput = TRUE;
		if (pHost->GetTyp()==_T("SMS"))
		{
			// only non SMS hosts will use
			// in and output
			bLocalUnits = TRUE;
			bInput = bOutput = FALSE;
		}
		if (   (pHost->GetTyp()==_T("E-Mail"))
			|| (pHost->GetTyp()==_T("FAX"))
			)
		{
			// only non SMS hosts will use
			// in and output
			bLocalUnits = TRUE;
			bInput = FALSE;
			if (m_pActivation->GetOutputID() == SECID_NO_ID)
			{
				// keine Kamera, kein Output
				bOutput = FALSE;
			}
		}
#ifdef _UNICODE
		if (bLocalUnits)
		{
			CString sCodePage;	// force Unicode for local connections
			sCodePage.Format(_T("%d"), CODEPAGE_UNICODE);
			m_pAlarmCallRecord->SetFieldValue(CRF_CODEPAGE, sCodePage);
		}
#endif

		m_pAlarmCallRecord->SetSourceHost(ha.GetLocalHost()->GetName());
		if (bInput)
		{
			m_pInputClient = theApp.GetServerControl()->AddInputClient(*m_pAlarmCallRecord);
			m_pInputClient->SetConnectionTime(GetMacro().GetConnectionTime());

		}
		if (bOutput)
		{
			m_pOutputClient = theApp.GetServerControl()->AddOutputClient(*m_pAlarmCallRecord,TRUE,pHost->GetName());
			m_pOutputClient->SetConnectionTime(GetMacro().GetConnectionTime());
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCallProcess::DoAlarmCall()
{
	if (m_pAlarmCallRecord==NULL) {
		TRACE(_T("No prepared alarm call record!?\n"));
		return FALSE;
	}

	BOOL bOkay=FALSE;

	CString shmInput,shmOutput;
	if (m_pInputClient)
	{
		shmInput = m_pInputClient->GetShmName();
	}
	if (m_pOutputClient)
	{
		shmOutput = m_pOutputClient->GetShmName();
	}

	CString sMsg;
	sMsg.Format(_T("AlarmCall|%s"),(LPCTSTR)m_pAlarmCallRecord->GetDestinationHost());
	m_pAlarmCallRecord->SetInputShm(shmInput);
	m_pAlarmCallRecord->SetOutputShm(shmOutput);
	m_pAlarmCallRecord->SetCamID(GetActivation()->GetOutputID());

	CSecID idNotification = GetActivation()->GetNotificationID();
	if (idNotification != SECID_NO_ID)
	{
		CNotificationMessageArray nma;
		CWK_Profile wkp;
		CNotificationMessage* pNM;
		nma.Load(wkp);
		pNM = nma.GetNotificationMessage(idNotification);
		if (pNM)
		{
			CString sURL = pNM->GetURL();
			CString sMessage = pNM->GetMessage();
			CString sCodePage;
			sCodePage.Format(_T("%d"), CWK_String::GetCodePage());
			if (sURL.IsEmpty())
			{
				sURL.Format(_T("%08lx.htm"),idNotification.GetID());
			}
			CInput* pInput = GetAlarmInput();
			if (pInput)
			{
				CIPCFields& fields = pInput->GetFields();
				fields.Lock();
				int c = fields.GetSize();
				if (c>0)
				{
					sMessage += _T(": ");
					for (int i=0;i<c;i++)
					{
						sMessage += fields.GetAtFast(i)->ToString();
						if (i<c-1)
						{
							sMessage += _T(", ");
						}
					}
				}
				fields.Unlock();
			}
			m_pAlarmCallRecord->SetFieldValue(NM_CODEPAGE,sCodePage);
			m_pAlarmCallRecord->SetFieldValue(NM_PARAMETER,sURL);
			m_pAlarmCallRecord->SetFieldValue(NM_MESSAGE,sMessage);
		}
	}
	WK_STAT_LOG(_T("Call"),1,sMsg);
	// NOT YET NewFetchAlarm ???
	// NOT YET reason and the like
	// NOT YET hostname in stat log
	if (LOCAL_LOOP_BACK == m_pAlarmCallRecord->GetDestinationHost())
	{
		// we have to start the vision application
		if (!WK_IS_RUNNING(WK_APP_NAME_IDIP_CLIENT))
		{
			CIPCServerControlClientSide::StartVision();
		}
	}
	// warte 10 Sekunden auf freies Fetch
	int c = 100;
//	DWORD dwTick = GetTickCount();
	while (   ms_bFetching 
		   && c>0
		   && theApp.IsUpAndRunning()
		   && m_bThreadRun)
	{
		Sleep(100);
		c--;
	}
	if (m_bThreadRun)
	{
		if (!ms_bFetching)
		{
			ms_bFetching = TRUE;
			CIPCFetchResult fr = m_Fetch.FetchAlarmConnection(*m_pAlarmCallRecord);
			ms_bFetching = FALSE;
			
			bOkay = fr.IsOkay();
			if (bOkay)
			{
				if (m_pOutputClient)
				{
					m_pOutputClient->SetFetchResult(fr);
				}
				else
				{
					WK_TRACE_WARNING(_T("Alarm call, NO OutputClient\n"));
				}
				if (m_pInputClient)
				{
					m_pInputClient->SetCodePage(fr.GetCodePage());
				}
				else
				{
					WK_TRACE_WARNING(_T("Alarm call, NO InputClient\n"));
				}
			}
			else
			{
				WK_TRACE(_T("Alarm call failed %s %s\n"),::NameOfEnum(fr.GetError()),fr.GetErrorMessage());
			}
		}
		else
		{
			WK_TRACE(_T("cannot fetch now already one Alarm call active\n"));
		}
	}
	else
	{
		WK_TRACE(_T("call process already terminated no call anymore\n"));
	}
	WK_STAT_LOG(_T("Call"),0,sMsg);

	if (bOkay) {
		sMsg.Format(_T("AlarmCallOkay|%s"),m_pAlarmCallRecord->GetDestinationHost());
	} else {
		sMsg.Format(_T("AlarmCallFailed|%s"),m_pAlarmCallRecord->GetDestinationHost());
	}
	WK_STAT_PEAK(_T("Call"),1,sMsg);

	return bOkay;
}
/////////////////////////////////////////////////////////////////////////////
// static function of CProcess
UINT CCallProcess::AlarmCallThreadFunction(LPVOID pParam)
{
	CCallProcess *pThis = (CCallProcess *)pParam;
	XTIB::SetThreadName(_T("AlarmCallThread"));
	WK_TRACE(_T("AlarmCallThreadFunction Start %08lx\n"),GetCurrentThreadId());

	BOOL bOkay = pThis->DoAlarmCall();
	pThis->m_alarmCallState = bOkay ? ACS_SUCCESS : ACS_FAILED;
	Sleep(0);

	WK_TRACE(_T("AlarmCallThreadFunction End %08lx\n"),GetCurrentThreadId());
	return bOkay;
}
/////////////////////////////////////////////////////////////////////////////
void CCallProcess::StopAlarmCallThread()
{
	m_bThreadRun=FALSE;
	if (m_pThread) 
	{
		WK_TRACE(_T("StopAlarmCallThread %08lx\n"),m_pThread->m_nThreadID);
		m_Fetch.Cancel();
		// SIGH timeout should more than the max FetchXXX timeout
		DWORD dwTick = WK_GetTickCount();
		DWORD dwThreadEnd = WaitForSingleObject(m_pThread->m_hThread,30000);
		if (dwThreadEnd==WAIT_TIMEOUT) 
		{
			WK_TRACE(_T("StopAlarmCallThread time out %d ms\n"),WK_GetTimeSpan(dwTick));
		} 
		else if (dwThreadEnd==WAIT_OBJECT_0) 
		{
			WK_TRACE(_T("StopAlarmCallThread in %d ms\n"),WK_GetTimeSpan(dwTick));
		}
	}
	WK_DELETE(m_pThread);
}
