// CallProcess.cpp: implementation of the CCallThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dvprocess.h"

#include "CallThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCallThread::CCallThread()
 : CWK_Thread(_T("CallThread"))
{
	m_iCurrentHost = 0;
	m_hSemaphore = CreateSemaphore( NULL, 0, 5000, _T("CallThreadSemaphore"));
	m_pInputClient = NULL;
	m_pOutputClient = NULL;
	m_pConnectionRecord = NULL;
	m_State = ACS_SLEEPING;
	m_dwFailedTime = 0;
	m_dwStatTime = 4*60*60*1000;
	m_bRuntimeErrorNoHostSent = FALSE;
}
//////////////////////////////////////////////////////////////////////
CCallThread::~CCallThread()
{
	EmptyAlarmIDQueue();
	WK_CLOSE_HANDLE(m_hSemaphore);
	WK_DELETE(m_pConnectionRecord);
}
//////////////////////////////////////////////////////////////////////
BOOL CCallThread::Run(LPVOID lpContext)
{
	HANDLE hHandles[2];

	hHandles[0]	= m_StopEvent;
	hHandles[1] = m_hSemaphore;

	DWORD r = WaitForMultipleObjects(2,hHandles,FALSE,100);
	
	if (r==WAIT_TIMEOUT)
	{
		HandleAlarmCall();
	}
	else if (r==WAIT_FAILED)
	{
		WK_TRACE(_T("%s wait failed %s\n"),m_sName,GetLastErrorString());
	}
	else if (r==WAIT_OBJECT_0)
	{
		// that's it thread is over
		return FALSE;
	}
	else if (r==WAIT_OBJECT_0+1)
	{
		// A real Alarm
		// Der Status wird gnadenlos gesetzt?
		// Was ist mit ACS_FAILED?
		// Dann wird sofort der naechste Anruf gestartet, der FailedTimeout ist fuer die Katz
		// dann haengt die ganze Zustandsmaschine wegen AlreadyConnected == TRUE
		// AlarmID darf nur bei ACS_SLEEPING rausgeholt und bearbeitet werden, siehe HandleAlarmCall
/*		m_idCurrent = m_AlarmIDs.GetAt(0);
		m_AlarmIDs.RemoveAt(0);
		m_State = ACS_INITIALIZED;
*/
		HandleAlarmCall();
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CCallThread::ResetAlarmHostList()
{
	m_saHosts.RemoveAll();
	int j=1;
	for (int i=0;i<theApp.GetAlarmHosts().GetSize();i++)
	{
		const CString &sAh = theApp.GetAlarmHosts().GetAt(i);
		if (!sAh.IsEmpty())
		{
			m_saHosts.Add(sAh);
			WK_TRACE(_T("Alarm Host %d is %s\n"), j++, sAh);
		}
	}
	m_bRuntimeErrorNoHostSent = FALSE;
	// the index may not be valid anymore, start new loop
	m_iCurrentHost = 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCallThread::StartThread()
{
	m_iCurrentHost = 0;
	m_dwFailedTime = 0;
	ResetAlarmHostList();
	m_State = ACS_SLEEPING;
	return CWK_Thread::StartThread();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCallThread::StopThread()
{
	m_saHosts.RemoveAll();
	EmptyAlarmIDQueue();
	return CWK_Thread::StopThread();
}
/////////////////////////////////////////////////////////////////////////////
void CCallThread::EmptyAlarmIDQueue()
{
	m_AlarmIDs.RemoveAll();
	while (WAIT_TIMEOUT != WaitForSingleObject(m_hSemaphore,0))
	{
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCallThread::AddAlarmOutputID(CSecID id)
{
	// CAVEAT: Called from other thread!
	if (m_saHosts.GetSize())
	{
		if (   IsRunning()
// AlarmIDs muessen immer hinzugefuegt werden, sonst gehen sie doch wohl verloren
// Die Semaphore funkt jetzt auch nicht mehr der Zustandsmaschine in die Quere
// siehe Run() und HandleAlarmCall()
// CriticalSection, da von zwei Threads auf m_AlarmIDs zugegriffen wird
//
//			&& m_State != ACS_TRYING
//			&& m_State != ACS_PREPARED
			)
		{
			// first search for cam
			// don't add it twice
			BOOL bFound = FALSE;
			m_csAlarmIDs.Lock();
			for (int i=0;i<m_AlarmIDs.GetSize() && !bFound;i++)
			{
				bFound = m_AlarmIDs[i]==id.GetID();
			}
			if (!bFound)
			{
				m_AlarmIDs.Add(id.GetID());
				if (m_hSemaphore) 
				{ 
					ReleaseSemaphore(m_hSemaphore, 1, NULL); 
				} 
			}
			m_csAlarmIDs.Unlock();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCallThread::HandleAlarmCall()
{
	if (m_saHosts.GetSize())
	{
		switch (m_State)
		{
		case ACS_SLEEPING:
			// Nur in diesem Zustand duerfen die IDs rausgeholt werden
			if (m_AlarmIDs.GetSize())
			{
				m_csAlarmIDs.Lock();
				m_idCurrent = m_AlarmIDs.GetAt(0);
				m_AlarmIDs.RemoveAt(0);
				m_State = ACS_INITIALIZED;
				m_csAlarmIDs.Unlock();
			}
			break;
		case ACS_INITIALIZED:
			{
			BOOL bAlreadyConnected = FALSE;
			if (m_idCurrent != SECID_NO_ID)
			{
				bAlreadyConnected = AlreadyConnected();
				if (bAlreadyConnected == FALSE)
				{
					PrepareAlarmCall();
				}
				else
				{
					m_State = ACS_SLEEPING;
				}
			}
//			WK_TRACE(_T("*** TKR ACS_INITIALIZED m_idCurrect: %08x  AlreadyConn. %d\n"), m_idCurrent, bAlreadyConnected);
			}
			break;
		case ACS_PREPARED:
//			WK_TRACE(_T("*** TKR ACS_PREPARED DoAlarmCall() m_idCurrect: %08x\n"), m_idCurrent);
			DoAlarmCall();
			break;
		case ACS_TRYING:
			m_iCurrentHost++;

//			WK_TRACE(_T("*** TKR ACS_TRYING Cur host: %i >= saHosts: %i ??\n"), m_iCurrentHost, m_saHosts.GetSize());
			if (m_iCurrentHost>=m_saHosts.GetSize())
			{
				m_iCurrentHost = 0;
				m_State = ACS_SLEEPING;
// Message already send at FAILED
/*
				CString sError;
				sError.LoadString(IDS_REJECT_ALARMCONNECTION);
				CWK_RunTimeError RTerr(WAI_SECURITY_SERVER, REL_WARNING, RTE_REJECT_ALARM_CONNECTION, sError, 1, 0);
				RTerr.Send();
*/
			}
			else
			{
//				WK_TRACE(_T("*** TKR ACS_TRYING else...\n"));
				m_State = ACS_INITIALIZED;
			}
			break;
		case ACS_SUCCESS:
//			WK_TRACE(_T("*** TKR ACS_SUCCESS \n"));
			m_iCurrentHost = 0;
			m_State = ACS_SLEEPING;
			// gf todo
			// reset m_iCurrentHost may cause 2 connections to different hosts, if current connection is != 0
			// and another alarm occures while not yet connected
			// see Run(m_hSemaphore) and AlreadyConnected()
			break;
		case ACS_FAILED:
			if (m_dwFailedTime == 0)
			{
//				WK_TRACE(_T("*** TKR ACS_FAILED \n"));
				m_dwFailedTime = GetTickCount();

				// Die Fehlermeldung kann bereits hier erfolgen, warum erst den 10 Sekunden Timeout abwarten
				if (m_iCurrentHost+1 >= m_saHosts.GetSize())
				{
					CString sError;
					sError.LoadString(IDS_REJECT_ALARMCONNECTION);
					CWK_RunTimeError RTerr(WAI_SECURITY_SERVER, REL_WARNING, RTE_REJECT_ALARM_CONNECTION, sError, 1, 0);
					RTerr.Send();
				}
			}
			else
			{
				if (GetTimeSpan(m_dwFailedTime)>10*1000)
				{
					m_State = ACS_TRYING;
					WK_TRACE(_T("*** TKR ACS_FAILED -> ACS_TRYING\n"));
				}
			}
			break;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
CString CCallThread::GetRealNumber(const CString& sNumber,BOOL bTCPPrefix)
{
	CString sRet(sNumber);
	LPCTSTR szRet = sRet;
	int i;
	for (i=0; i<sRet.GetLength(); i++)
	{
		if (isalpha(szRet[i]))
		{
			if (bTCPPrefix)
			{
				sRet = _T("tcp:") + sRet;
			}
			return sRet;
		}
	}

	if (-1!=sRet.Find('.'))
	{
		// assume tcp/ip
		int i1,i2,i3,i4;
		if (   (4 == _stscanf(sRet,_T("%d.%d.%d.%d"),&i1,&i2,&i3,&i4))
			|| (4 == _stscanf(sRet,_T("tcp:%d.%d.%d.%d"),&i1,&i2,&i3,&i4))
			)
		{
			if (   0<=i1 && i1<=255
				&& 0<=i2 && i2<=255
				&& 0<=i3 && i3<=255
				&& 0<=i4 && i4<=255)
			{
				if (bTCPPrefix)
				{
					sRet.Format(_T("tcp:%d.%d.%d.%d"),i1,i2,i3,i4);
				}
				else
				{
					sRet.Format(_T("%d.%d.%d.%d"),i1,i2,i3,i4);
				}
			}
		}
	}

	if (sRet == _T("0"))
	{
		sRet = LOCAL_LOOP_BACK;
	}
	return sRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCallThread::AlreadyConnected()
{
	BOOL bRet = FALSE;


	theApp.GetOutputClients().Lock();
	for (int i=0;i<theApp.GetOutputClients().GetSize() && !bRet;i++)
	{
		COutputClient* pOC = theApp.GetOutputClients().GetAtFast(i);


		// m_iCurrentHost is reset to 0 at ACS_SUCCESS
		// this will lead to a second connection, if m_iCurrentHost was not 0 before!
		// we have to check for all m_saHosts here, not only sHost(m_iCurrentHost) IMO
/*
		CString sHost = GetRealNumber(m_saHosts.GetAt(m_iCurrentHost),FALSE); 
		CString sClientNr = GetRealNumber(pOC->GetNumber(),FALSE); 
		TRACE(_T("AlreadyConnected %d <%s> <%s> con:%i alarm:%i\n"),i,sClientNr,sHost, pOC->IsConnected(), pOC->IsAlarm());
		if (   (sClientNr == sHost)
			&& pOC->IsAlarm()
			)
*/
		if (pOC->IsAlarm())



		{
			// we already have an alarm connection started
			bRet = TRUE;
			if (pOC->IsConnected())
			{
				// if already connected, we can change the camera
				TRACE(_T("alarm select camera %08lx\n"),m_idCurrent);
				pOC->GetCIPC()->DoRequestSelectCamera(m_idCurrent);
				m_idCurrent = SECID_NO_ID;
			}
			else
			{
				// not yet connected, so keep m_idCurrent
				// it may be called at Run() at timeout again,
				// so we can try again then
			}
		}
	}
	theApp.GetOutputClients().Unlock();

	TRACE(_T("AlreadyConnected = %d\n"),bRet);

	return bRet;

// New version has to checked for correct alarm handling on manual connected hosts
/*	BOOL bHostAlreadyConnected = FALSE;
	CString sHost, sClientNr; 

	theApp.GetOutputClients().Lock();
	for (int i=0;i<theApp.GetOutputClients().GetSize() && !bHostAlreadyConnected;i++)
	{
		COutputClient* pOC = theApp.GetOutputClients().GetAt(i);
		sClientNr = GetRealNumber(pOC->GetNumber(), FALSE);
		
		// m_iCurrentHost is reset to 0 at ACS_SUCCESS
		// this will lead to a second connection, if m_iCurrentHost was not 0 before!
		// we have to check for all m_saHosts here, not only sHost(m_iCurrentHost) IMO
		for (int j=0 ; j<m_saHosts.GetSize() ; j++)
		{
			sHost = GetRealNumber(m_saHosts.GetAt(j), FALSE); 
			if (sClientNr == sHost)
			{
				// we already have an connection
				bHostAlreadyConnected = TRUE;
				WK_TRACE(_T("HostAlreadyConnected %d <%s> <%s> con:%i alarm:%i\n"),j,sClientNr,sHost, pOC->IsConnected(), pOC->IsAlarm());
				break;
			}
		}
		if (bHostAlreadyConnected)
		{
			if (pOC->IsConnected())
			{
				// if already connected, we can change the camera
				WK_TRACE(_T("alarm select camera %08lx\n"),m_idCurrent);
				pOC->GetCIPC()->DoRequestSelectCamera(m_idCurrent);
				m_idCurrent = SECID_NO_ID;
			}
			else
			{
				// not yet connected, so keep m_idCurrent
				// it may be called at Run() at timeout again,
				// so we can try again then
			}
		}
	}
	theApp.GetOutputClients().Unlock();

	WK_TRACE(_T("HostAlreadyConnected = %d\n"), bHostAlreadyConnected);

	return bHostAlreadyConnected;
*/
}
/////////////////////////////////////////////////////////////////////////////
void CCallThread::PrepareAlarmCall()
{
	if (!IsInArray(m_iCurrentHost, m_saHosts.GetSize()))
	{
		return;
	}
	const CString &sCurrentHost = m_saHosts.GetAt(m_iCurrentHost);
	WK_TRACE(_T("PREPARE Alarm call to %s\n"), sCurrentHost);

	CUser dummyUser;
	dummyUser.SetName(_T("Alarm"));
	dummyUser.SetPermissionID(SECID_SPECIAL_RECEIVER_PERMISSION);

	CString sNumber;

	sNumber = GetRealNumber(sCurrentHost,TRUE);
	if (m_pConnectionRecord)
	{
		WK_TRACE_ERROR(_T("m_pConnectionRecord NOT NULL\n"));
		WK_DELETE(m_pConnectionRecord);
	}
	m_pConnectionRecord = new CConnectionRecord(sNumber,
												dummyUser,
												_T("SpecialReceiver"),
												_T(""));

	if (m_pInputClient)
	{
		WK_TRACE_ERROR(_T("m_pInputClient NOT NULL\n"));
	}
	m_pInputClient = theApp.GetInputClients().AddInputClient();
	if (m_pOutputClient)
	{
		WK_TRACE_ERROR(_T("m_pOutputClient NOT NULL\n"));
	}
	m_pOutputClient = theApp.GetOutputClients().AddOutputClient();

	m_pConnectionRecord->SetOptions(SCO_WANT_RELAYS|SCO_WANT_CAMERAS_BW|SCO_WANT_CAMERAS_COLOR);
	m_pConnectionRecord->SetCamID(m_idCurrent);
	m_pConnectionRecord->SetFieldValue(CSD_IS_DV,CSD_ON);
	m_pConnectionRecord->SetFieldValue(CSD_PIN,theApp.GetOperatorPIN());

	CWK_Profile prof(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));
	CHostArray	hosts;
	hosts.Load(prof);
	CString sHost = hosts.GetLocalHostName();
	m_pConnectionRecord->SetSourceHost(sHost);
	if (0 == sNumber.Find(_T("tcp:")))
	{
		CString sLocalIP;
		CWK_Profile wkp;
#ifdef _UNICODE
		sLocalIP = wkp.GetString(_T("SocketUnit"), _T("OwnNumber"), _T(""), TRUE);
#else
		sLocalIP = wkp.GetString(_T("SocketUnit"), _T("OwnNumber"), _T(""));
#endif
		if (!sLocalIP.IsEmpty())
		{
			CWK_Profile::Decode(sLocalIP);
			sLocalIP.TrimRight();
		}
		m_pConnectionRecord->SetFieldValue(CRF_IPADDRESS,sLocalIP);
	}
	else if (sNumber == LOCAL_LOOP_BACK)
	{
		m_pConnectionRecord->SetSourceHost(LOCAL_LOOP_BACK);
	}
	else
	{										   
		CString sLocalMSN;
		CWK_Profile wkp;
#ifdef _UNICODE
		sLocalMSN = wkp.GetString(_T("ISDNUnit"), _T("OwnNumber"), _T(""), TRUE);
#else
		sLocalMSN = wkp.GetString(_T("ISDNUnit"), _T("OwnNumber"), _T(""));
#endif 
		if (!sLocalMSN.IsEmpty())
		{
			CWK_Profile::Decode(sLocalMSN);
			sLocalMSN.TrimRight();
		}
		m_pConnectionRecord->SetFieldValue(CRF_MSN,sLocalMSN);
	}

	m_State = ACS_PREPARED;

	if (WK_IS_WINDOW(AfxGetMainWnd()))
	{
		AfxGetMainWnd()->PostMessage(WM_USER);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCallThread::DoAlarmCall()
{
	if (   m_pInputClient->GetCIPC()
		&& m_pInputClient->GetCIPC()->GetIPCState()==CIPC_STATE_WRITE_CREATED
		&& m_pOutputClient->GetCIPC()
		&& m_pOutputClient->GetCIPC()->GetIPCState()==CIPC_STATE_WRITE_CREATED)
	{
		WK_TRACE(_T("START Alarm call from %s to %s\n"),
			m_pConnectionRecord->GetSourceHost(),
			m_pConnectionRecord->GetDestinationHost());
		m_pConnectionRecord->SetInputShm(m_pInputClient->GetCIPC()->GetShmName());
		m_pConnectionRecord->SetOutputShm(m_pOutputClient->GetCIPC()->GetShmName());

		CIPCFetch fetch;

		CIPCFetchResult result = fetch.FetchAlarmConnection(*m_pConnectionRecord);
		if (result.IsOkay())
		{
			WK_TRACE(_T("Alarm call to %s SUCCESS\n"),m_pConnectionRecord->GetDestinationHost());
			m_idCurrent = SECID_NO_ID;
			m_State = ACS_SUCCESS;
#ifdef _UNICODE
			m_pInputClient->GetCIPC()->SetCodePage(result.GetCodePage());
			m_pOutputClient->GetCIPC()->SetCodePage(result.GetCodePage());
#endif
			if (m_pOutputClient)
			{
				m_pOutputClient->SetFetchResult(result);
			}
			else
			{
				WK_TRACE_WARNING(_T("Alarm call, NO OutputClient\n"));
			}
		}
		else
		{
			WK_TRACE(_T("Alarm call to %s FAILED\n"),m_pConnectionRecord->GetDestinationHost());
			m_State = ACS_FAILED;
		}
		// Always reset m_dwFailedTime to start the repeat timeout
		// Otherwise the status does not change to ACS_TRYING!
		// Then the host counter is not increased -> Same host is called, but AlreadyConnected
		// => No alarm connection is started - never!
		m_dwFailedTime = 0;
		WK_DELETE(m_pConnectionRecord);
		EmptyAlarmIDQueue();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCallThread::InputDisconnected(CInputClient* pInputClient)
{
	if (m_pInputClient == pInputClient)
	{
		m_pInputClient = NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCallThread::OutputDisconnected(COutputClient* pOutputClient)
{
	if (m_pOutputClient == pOutputClient)
	{
		m_pOutputClient = NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCallThread::DialingNumbersChanged()
{
	ResetAlarmHostList();
}
