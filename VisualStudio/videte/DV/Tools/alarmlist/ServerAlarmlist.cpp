// Server.cpp: implementation of the CServerAlarmlist class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alarmlist.h"
#include "ServerAlarmlist.h"
#include "IPCDatabaseClientAlarmlist.h"
#include "AlarmListDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
CServerAlarmlist::CServerAlarmlist(CAlarmlistDlg* pDlg)
{
	m_pConnectionRecord		= NULL;
	m_sHost					= LOCAL_LOOP_BACK;
	m_sEnteredPIN			= "0000";
	m_bConnectThreadRunning = FALSE;
	m_pConnectionRecord		= NULL;
	m_pDatabaseResult		= NULL;
	m_pConnectThread		= NULL;
	m_pDatabase				= NULL;
	m_dwReconnectTick		= 0;
	m_pAlarmlistDlg			= pDlg;
}

//////////////////////////////////////////////////////////////////////
CServerAlarmlist::~CServerAlarmlist()
{
	WK_DELETE(m_pConnectionRecord);
	WK_DELETE(m_pDatabaseResult);
	WK_DELETE(m_pDatabase);
}

//////////////////////////////////////////////////////////////////////
BOOL CServerAlarmlist::Connect(const CString &sHost, const CString &sPIN)
{
	m_sHost			= sHost;
	m_sEnteredPIN	= sPIN;
	
 	Reconnect();
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CServerAlarmlist::Reconnect()
{
	if (m_pConnectThread==NULL
		&& !m_bConnectThreadRunning)
	{
		WK_DELETE(m_pConnectionRecord);
		// use same ConnectionRecord for all Input, Output and Database
		m_pConnectionRecord = new CConnectionRecord;
		m_pConnectionRecord->SetDestinationHost(m_sHost);
		m_pConnectionRecord->SetPermission("SuperVisor");
		// GF todo hier Operator PIN
		m_pConnectionRecord->SetPassword(m_sEnteredPIN);
		m_pConnectionRecord->SetFieldValue(CSD_PIN,m_sEnteredPIN);

 		m_bConnectThreadRunning = TRUE;
		m_pConnectThread = AfxBeginThread(ConnectThreadProc,(LPVOID)this);
		m_pConnectThread->m_bAutoDelete = FALSE;

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//////////////////////////////////////////////////////////////////////
UINT CServerAlarmlist::ConnectThreadProc(LPVOID pParam)
{
	TRACE("ConnectThreadProc Start\n");

	CServerAlarmlist* pServer = (CServerAlarmlist*)pParam;

	pServer->m_bConnectThreadRunning = TRUE;

	if (!pServer->IsDatabaseConnected())
	{
		pServer->Lock();
		CConnectionRecord c(*pServer->m_pConnectionRecord);
		pServer->Unlock();

		WK_TRACE("FetchDatabaseConnection\n");
		CIPCFetch fetch;
		CIPCFetchResult frDatabase = fetch.FetchDatabase(c);

		pServer->Lock();
		WK_DELETE(pServer->m_pDatabaseResult);
		pServer->m_pDatabaseResult = new CIPCFetchResult(frDatabase);
		pServer->Unlock();
	}
	
	pServer->Lock();
	pServer->m_bConnectThreadRunning = FALSE;
	pServer->Unlock();

	TRACE("ConnectThreadProc Ende\n");

	return 0xC0;
}

//////////////////////////////////////////////////////////////////////
void CServerAlarmlist::ConnectDatabase()
{
	Lock();

	if (m_pDatabaseResult)
	{
		CString shmDatabaseName;
		shmDatabaseName = m_pDatabaseResult->GetShmName();
		if ( (m_pDatabaseResult->GetError()!=CIPC_ERROR_OKAY) 
			|| (shmDatabaseName.GetLength()==0))
		{
			if (m_pAlarmlistDlg)
				m_pAlarmlistDlg->NotifyConnect(FALSE,m_pDatabaseResult->GetError());
		}
		else
		{
			WK_TRACE("connect database OK\n");
			WK_DELETE(m_pDatabase);
			m_pDatabase = new CIPCDatabaseClientAlarmlist(m_pDatabaseResult->GetShmName(),this);
		}
		WK_DELETE(m_pDatabaseResult);
	}
	Unlock();
}

//////////////////////////////////////////////////////////////////////
void CServerAlarmlist::DisconnectDatabase()
{
	WK_DELETE(m_pDatabase);
	WK_DELETE(m_pConnectionRecord);
}

//////////////////////////////////////////////////////////////////////
BOOL CServerAlarmlist::IsDatabaseDisconnected()
{
	BOOL bRet = FALSE;

	Lock();
	bRet = (m_pDatabase!=NULL) && (m_pDatabase->GetIsMarkedForDelete());
	Unlock();

	return bRet;
}

//////////////////////////////////////////////////////////////////////
BOOL CServerAlarmlist::IsDatabaseConnected()
{
	BOOL bRet = FALSE;


	Lock();
	if (m_pDatabase)
	{
		CIPCState state = m_pDatabase->GetIPCState(); 
	}

	bRet = (m_pDatabase!=NULL) && (m_pDatabase->GetIPCState()==CIPC_STATE_CONNECTED);
	Unlock();

	return bRet;
}

//////////////////////////////////////////////////////////////////////
void CServerAlarmlist::Lock()
{
	m_cs.Lock();
}

//////////////////////////////////////////////////////////////////////
void CServerAlarmlist::Unlock()
{
	m_cs.Unlock();
}

//////////////////////////////////////////////////////////////////////
void CServerAlarmlist::OnIdle()
{
	BOOL bDoReconnect = FALSE;

	Lock();
	if (   m_pConnectThread 
		&& !m_bConnectThreadRunning)
	{
		TRACE("deleting connection thread\n");
		WK_DELETE(m_pConnectThread);
	}
	Unlock();

	if (!IsDatabaseConnected())
	{
		Lock();
		if (m_pDatabaseResult!=NULL)
		{
			ConnectDatabase();
			Sleep(2000); // For Win2000!??
		}
		else
		{
			bDoReconnect = (m_pDatabase == NULL);
		}
		Unlock();
	}

	if (IsDatabaseDisconnected())
	{
		DisconnectDatabase();
	}

	if (bDoReconnect)
	{
		if (GetTimeSpan(m_dwReconnectTick) > 10*1000)
		{
			m_dwReconnectTick = GetTickCount();
			Reconnect();
		}
	}
}
