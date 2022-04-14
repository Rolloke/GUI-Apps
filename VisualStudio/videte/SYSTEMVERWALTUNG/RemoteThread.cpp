// RemoteThread.cpp: implementation of the CRemoteThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "systemverwaltung.h"
#include "RemoteThread.h"

#include "RemoteDialog.h"
#include "CIPCInputSV.h"
#include "CIPCDatabaseSV.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRemoteThread::CRemoteThread()
	: CWK_Thread(_T("RemoteThread"),NULL)
{
	m_pCIPCInputSV = NULL;
	m_pCIPCDatabaseSV = NULL;
	m_bCancelled = FALSE;
	m_bSend = FALSE;
}
//////////////////////////////////////////////////////////////////////
CRemoteThread::~CRemoteThread()
{
	WK_DELETE(m_pCIPCInputSV);
	WK_DELETE(m_pCIPCDatabaseSV);
}
//////////////////////////////////////////////////////////////////////
void CRemoteThread::Cancel()
{
	CString sHost;
	if (m_pRemoteDialog)
	{
		sHost = m_pRemoteDialog->m_sHost;
	}
	WK_TRACE(_T("Connection cancelled to <%s>\n"), (LPCTSTR)sHost);
	m_pRemoteDialog = NULL;
	m_bCancelled = TRUE;
	m_Fetch.Cancel();
}
//////////////////////////////////////////////////////////////////////
BOOL CRemoteThread::Run(LPVOID lpContext)
{
	CWK_Profile wkp; // ML 1.9.99 Sollte nicht lieber GetProfile() benutzt werden?
	// RKE: nein, den die Gegenstationen einer evtl. vebundenen Gegenstation sind wohl nicht diselben
	CHostArray ha;
	ha.Load(wkp);
	CHost* pHost = ha.GetHost(m_pRemoteDialog->m_HostID);
	if (pHost==NULL)
	{
		if (m_pRemoteDialog)
		{
			m_pRemoteDialog->m_sState.LoadString(IDS_DISCONNECTED_ERROR);
		}
		return FALSE;
	}

	WK_TRACE(_T("Connecting to <%s>\n"),pHost->GetName());
	if (m_pRemoteDialog)
	{
		m_pRemoteDialog->m_sState.LoadString(IDS_CONNECTING);
	}

	CConnectionRecord c(pHost->GetNumber(), 
						*theApp.m_pUser, 
						theApp.m_pPermission->GetName(),
						theApp.m_pPermission->GetPassword(),
						30*1000);

	CIPCFetchResult frInput = m_Fetch.FetchInput(c);
	CIPCError frError = frInput.GetError();
	if (frError != CIPC_ERROR_OKAY)
	{
		// fetch canceled or something went wrong
		if (frError != CIPC_ERROR_CANCELLED)
		{
			int iFetchError = frInput.GetErrorCode();
			CString sFetchError = frInput.GetErrorMessage();
			WK_TRACE_ERROR(_T("Unable to connect to <%s>, Error:%s, Code:%i; %s\n"),
											pHost->GetName(),
											NameOfEnum(frError),
											iFetchError,
											sFetchError);
			if (m_pRemoteDialog)
			{
				CString sError;
				sError.LoadString(IDS_DISCONNECTED_ERROR);
				if (sFetchError.IsEmpty() == FALSE)
				{
					sError += _T("\n") + sFetchError;
				}
				m_pRemoteDialog->m_sState = sError;
			}
		}
		return FALSE;
	}

	WK_TRACE(_T("Fetch returned to <%s>\n"),pHost->GetName());
	if (m_bCancelled)
	{
		return FALSE;
	}

	m_pCIPCInputSV = new CIPCInputSV(m_pRemoteDialog->m_sCommand, m_pRemoteDialog->m_bReceiving, frInput.GetShmName()
#ifdef _UNICODE
									 , frInput.GetCodePage()
#endif
									 );
	
	if (m_pRemoteDialog && !m_pRemoteDialog->m_bReceiving)
	{
		m_pCIPCInputSV->m_sRegistry = m_sRegistryFile;
		m_pCIPCInputSV->m_sCoCoISAini = m_sCoCoISAini;
		m_pCIPCInputSV->m_nRemoteCodePage = m_nRemoteCodePage;
	}

	if (m_bCancelled)
	{
		return FALSE;
	}

	if (m_pRemoteDialog->m_bReceiving)
	{
		CIPCFetchResult frDatabase = m_Fetch.FetchDatabase(c);
		if (frDatabase.GetError()!=CIPC_ERROR_OKAY)
		{
			// fetch canceled or something went wrong
			if (frDatabase.GetError()!=CIPC_ERROR_CANCELLED)
			{
				WK_TRACE_ERROR(_T("Unable to connect to <%s>\n"),pHost->GetName());
				if (m_pRemoteDialog)
				{
					m_pRemoteDialog->m_sState.LoadString(IDS_DISCONNECTED_ERROR);
				}
			}
			return FALSE;
		}
		if (m_bCancelled)
		{
			return FALSE;
		}
		m_pCIPCDatabaseSV = new CIPCDatabaseSV(this, frDatabase.GetShmName()
#ifdef _UNICODE
			, frDatabase.GetCodePage()
#endif
			);
	}

	int i = 1000;
	if (m_pRemoteDialog->m_bReceiving)
	{
		while ( (m_pCIPCInputSV->GetIPCState()!=CIPC_STATE_CONNECTED) &&
				(m_pCIPCDatabaseSV->GetIPCState()!=CIPC_STATE_CONNECTED) &&
				!m_bCancelled)
		{
			Sleep(20);
			i--;
			if (i==0)
			{
				WK_TRACE_ERROR(_T("Timeout for getting connected <%s>\n"),pHost->GetName());
				if (m_pRemoteDialog)
				{
					m_pRemoteDialog->m_sState.LoadString(IDS_DISCONNECTED_ERROR);
				}
				return FALSE;
			}
		}
	}
	else
	{
		while ( (m_pCIPCInputSV->GetIPCState()!=CIPC_STATE_CONNECTED) &&
				!m_bCancelled)
		{
			Sleep(20);
			i--;
			if (i==0)
			{
				WK_TRACE_ERROR(_T("Timeout for getting connected <%s>\n"),pHost->GetName());
				if (m_pRemoteDialog)
				{
					m_pRemoteDialog->m_sState.LoadString(IDS_DISCONNECTED_ERROR);
				}
				return FALSE;
			}
		}
	}

	if (m_bCancelled)
	{
		return FALSE;
	}

	WK_TRACE(_T("Connected to <%s>\n"),pHost->GetName());
	if (m_pRemoteDialog)
	{
		m_pRemoteDialog->m_sState.LoadString(IDS_CONNECTED);
	}

	if (m_bCancelled)
	{
		return FALSE;
	}

	if (m_pRemoteDialog->m_bReceiving)
	{
		DWORD dwWaitResult = STATUS_WAIT_0;
		for (i=0 ; i<400 ; i++)
		{
			dwWaitResult = WaitForSingleObject(m_pCIPCInputSV->m_eventRegistry,100);
			if (dwWaitResult==WAIT_OBJECT_0)
			{
				WK_TRACE(_T("Got registry <%s>\n"),pHost->GetName());
				break;
			}
			else
			{
				if (m_bCancelled)
				{
					return FALSE;
				}
				else
				{
					// weitermachen
				}
			}
		}
		if (dwWaitResult!=WAIT_OBJECT_0)
		{
			WK_TRACE_ERROR(_T("Timeout for getting registry <%s>\n"),pHost->GetName());
			if (m_pRemoteDialog && m_pRemoteDialog->m_hWnd)
			{
				TRACE(_T("m_pRemoteDialog %08x, this %08x\n"), m_pRemoteDialog, this);
				m_pRemoteDialog->m_sState.LoadString(IDS_DISCONNECTED_ERROR);
			}
			return FALSE;
		}

		for (i=0 ; i<200 ; i++)
		{
			dwWaitResult = WaitForSingleObject(m_pCIPCDatabaseSV->m_eventGotDrives,100);
			if (dwWaitResult==WAIT_OBJECT_0)
			{
				WK_TRACE(_T("Got drives <%s>\n"),pHost->GetName());
				break;
			}
			else
			{
				if (m_bCancelled)
				{
					return FALSE;
				}
				else
				{
					// weitermachen
				}
			}
		}
		if (dwWaitResult!=WAIT_OBJECT_0)
		{
			WK_TRACE_ERROR(_T("Timeout for getting drives <%s>\n"),pHost->GetName());
			if (m_pRemoteDialog)
			{
				m_pRemoteDialog->m_sState.LoadString(IDS_DISCONNECTED_ERROR);
			}
			return FALSE;
		}

		WK_TRACE(_T("Got registry from to <%s>\n"),pHost->GetName());
		if (m_pRemoteDialog)
		{
			m_pRemoteDialog->m_sState.LoadString(IDS_GOT_REGISTRY);
		}

		m_sRegistryFile = m_pCIPCInputSV->m_sRegistry;
		m_sCoCoISAini = m_pCIPCInputSV->m_sCoCoISAini;
		m_nRemoteCodePage = m_pCIPCInputSV->m_nRemoteCodePage;
	}
	else
	{
		// sending registry
		DWORD dwWaitResult = STATUS_WAIT_0;
		for (int i=0 ; i<200 ; i++)
		{
			dwWaitResult = WaitForSingleObject(m_pCIPCInputSV->m_eventRegistry,20*1000);
			if (dwWaitResult==WAIT_OBJECT_0)
			{
				WK_TRACE(_T("Sent registry <%s>\n"),pHost->GetName());
				break;
			}
			else
			{
				if (m_bCancelled)
				{
					return FALSE;
				}
				else
				{
					// weitermachen
				}
			}
		}
		if (dwWaitResult!=WAIT_OBJECT_0)
		{
			WK_TRACE_ERROR(_T("Timeout for sending registry <%s>\n"),pHost->GetName());
			if (m_pRemoteDialog)
			{
				m_pRemoteDialog->m_sState.LoadString(IDS_DISCONNECTED_ERROR);
			}
			return FALSE;
		}
	}

	WK_TRACE(_T("Disconnecting from <%s>\n"),pHost->GetName());

	WK_DELETE(m_pCIPCInputSV);
	WK_DELETE(m_pCIPCDatabaseSV);

	WK_TRACE(_T("Disconnected from <%s>\n"),pHost->GetName());
	if (m_pRemoteDialog)
	{
		if (m_pRemoteDialog->m_bReceiving)
		{
			m_pRemoteDialog->m_sState.LoadString(IDS_DISCONNECTED_OK);
		}
		else
		{
			m_bSend = TRUE;
			m_pRemoteDialog->m_sState.LoadString(IDS_DISCONNECTED_OK2);
		}
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CRemoteThread::HasSettings() const
{
	return !m_sRegistryFile.IsEmpty();
}
//////////////////////////////////////////////////////////////////////
BOOL CRemoteThread::HasDrives() const
{
	return m_Drives.GetSize()>0;
}
