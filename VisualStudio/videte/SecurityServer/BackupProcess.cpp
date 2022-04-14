// BackupProcess.cpp: implementation of the CBackupProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sec3.h"

#include "BackupProcess.h"
#include "BackupThread.h"
#include "CIPCDatabaseServerBackup.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPtrList			CBackupProcess::gm_AutoBackupProcesses;
CCriticalSection	CBackupProcess::gm_csAutoBackupProcesses;
CDWordArray			CBackupProcess::gm_dwaBackupErrorUserData;

CBackupProcess::CBackupProcess(CProcessScheduler* pScheduler,CActivation *pActivation)
:CProcess(pScheduler)
{
	m_pActivation = pActivation;
	pActivation->SetActiveProcess(this);
	InitTimer();
	m_type = SPT_BACKUP;

	CAutoCritSec acs(&gm_csAutoBackupProcesses);
	if (gm_AutoBackupProcesses.GetCount()==0)
	{
		m_eBackupState = BS_INITIALIZED;
	}
	else
	{
		m_eBackupState = BS_SUSPEND;
		WK_TRACE(_T("Adding Suspended Backup Process %x\n"), pActivation->GetArchiveID().GetID());
	}
	gm_AutoBackupProcesses.AddTail(this);
	acs.Unlock();

	m_pBackupThread = NULL;
	m_pCIPCDatabaseServerBackup = NULL;
	m_dwBackupID = 0;

	m_sName.LoadString(IDS_AUTO_BACKUP);
	CSystemTime st;
	st.GetLocalTime();
	m_stBackupStart = st;
	m_sName += st.GetDateTime();
}
//////////////////////////////////////////////////////////////////////
CBackupProcess::~CBackupProcess()
{
	if (   (m_eBackupState != BS_FINISHED)
		&& (m_eBackupState != BS_FAILED)
		)
	{
		WK_TRACE(_T("deleting non finished backup process\n"));
	}
	WK_DELETE(m_pCIPCDatabaseServerBackup);
	WK_DELETE(m_pBackupThread);

	CAutoCritSec acs(&gm_csAutoBackupProcesses);
	POSITION pos = gm_AutoBackupProcesses.Find(this);
	if (pos)
	{
		gm_AutoBackupProcesses.RemoveAt(pos);
	}

	if (gm_AutoBackupProcesses.GetCount())
	{
		pos = gm_AutoBackupProcesses.GetHeadPosition();
		while (pos)
		{
			CBackupProcess*pNext = (CBackupProcess*)gm_AutoBackupProcesses.GetNext(pos);
			if (pNext->m_eBackupState == BS_SUSPEND)
			{
				//resume the next backup process only if no error occurred
				BOOL bFound = FALSE;
				DWORD dwLetter = pNext->GetMacro().GetBackupDestination().GetAt(0);
				for (int i=0; i<gm_dwaBackupErrorUserData.GetCount(); i++)
				{
					if(gm_dwaBackupErrorUserData.GetAt(i) == dwLetter)
					{
						bFound = TRUE;
						break;
					}
				}
				
				if(bFound)
				{
					WK_TRACE(_T("Backup Process failed, not resume suspended Backup Process%x\n"), pNext->m_pActivation->GetArchiveID().GetID());
					pNext->m_eBackupState = BS_FAILED;
				}
				else
				{
					WK_TRACE(_T("Resuming suspended Backup Process %x\n"), pNext->m_pActivation->GetArchiveID().GetID());
					pNext->m_eBackupState = BS_INITIALIZED;
				}
				break;
			}
		}
	}
	else
	{
		gm_dwaBackupErrorUserData.RemoveAll();
	}
}
//////////////////////////////////////////////////////////////////////
void CBackupProcess::Run()
{
	if(m_pCIPCDatabaseServerBackup)
	{
		if(m_pCIPCDatabaseServerBackup->GetIPCState() != CIPC_STATE_CONNECTED)
		{
			m_eBackupState = BS_FAILED;
		}
	}
	switch (m_eBackupState)
	{
	case BS_INITIALIZED:
		StartBackupThread();
		break;
	case BS_FETCHING:
		CheckForConnection();
		break;
	case BS_CONNECTED:
		RequestBackup();
		break;
	case BS_RUNNING:
		// just wait for confirm
		break;
	case BS_CONFIRMED:
		BackupConfirmed();
		break;
	case BS_DELETING:
		WaitForDeletes();
		break;
	case BS_FINISHED:
		WK_DELETE(m_pCIPCDatabaseServerBackup);
		SetState(SPS_TERMINATED,WK_GetTickCount());
		break;
	case BS_SUSPEND:
		break;
	case BS_FAILED:
		WK_DELETE(m_pCIPCDatabaseServerBackup);
		SetState(SPS_TERMINATED,WK_GetTickCount());
		break;
	default:
		WK_TRACE_ERROR(_T("invalid backup process state\n"));
		break;
	}
}
//////////////////////////////////////////////////////////////////////
void CBackupProcess::StartBackupThread()
{
	WK_TRACE(_T("connecting to databaseserver\n"));
	m_pBackupThread = new CBackupThread(this);

	
	m_pBackupThread->StartThread();
	m_eBackupState = BS_FETCHING;
}
//////////////////////////////////////////////////////////////////////
BOOL CBackupProcess::IsConnected()
{
	if (m_pCIPCDatabaseServerBackup)
	{
		if (m_pCIPCDatabaseServerBackup->GetIPCState()==CIPC_STATE_CONNECTED)
		{
			return TRUE;
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
void CBackupProcess::CheckForConnection()
{
	if (m_pBackupThread)
	{
		CIPCFetchResult* pFetchResult;
		
		pFetchResult = m_pBackupThread->GetFetchResult();
		
		if (pFetchResult)
		{
			CString shmDatabaseName;
			shmDatabaseName = pFetchResult->GetShmName();
			if (   (pFetchResult->GetError()!=CIPC_ERROR_OKAY) 
				|| (shmDatabaseName.GetLength()==0))
			{
				WK_TRACE(_T("fetch failed\n"));
				m_eBackupState = BS_FAILED;
			}
			else
			{
				WK_TRACE(_T("backup connecting to databaseserver\n"));
				m_pCIPCDatabaseServerBackup = new CIPCDatabaseServerBackup(
							this,pFetchResult->GetShmName());
#ifdef _UNICODE
				m_pCIPCDatabaseServerBackup->SetCodePage(pFetchResult->GetCodePage());
#endif	
			}
			m_pBackupThread->DeleteFetchResult();
		}
		else
		{
			WK_TRACE(_T("backup waiting for connection fetch result\n"));
		}
	}

	if (IsConnected())
	{
		WK_TRACE(_T("connected to databaseserver\n"));
		m_eBackupState = BS_CONNECTED;
	}
	else
	{
		WK_TRACE(_T("backup waiting for connection\n"));
	}
}
//////////////////////////////////////////////////////////////////////
void CBackupProcess::RequestBackup()
{
	WK_TRACE(_T("requesting backup\n"));
	if (IsConnected())
	{
		CString sName;
		CString sDestination;
		DWORD dwArchivID;
		dwArchivID = MAKELONG(0,GetMacro().GetBackupArchiveID().GetSubID());
		m_dwBackupID = GetTickCount();
		sDestination = GetMacro().GetBackupDestination();
		if (sDestination.GetAt(sDestination.GetLength()-1)!=_T('\\'))
		{
			sDestination += _T('\\');
		}

		if(GetMacro().GetBackupTimeSpan() == BU_TS_24HOURS)
		{
			CSystemTime stStart, stEnd;
			DWORD dw24Hours = 24;
			CSystemTime st24Hours(dw24Hours);
			stEnd = m_stBackupStart; 
			stStart =  stEnd - st24Hours;
			WK_TRACE(_T("Requesting Database backup by time  %08lx\n"), m_dwBackupID);
			m_pCIPCDatabaseServerBackup->DoRequestBackupByTime(LOWORD(m_dwBackupID),
															   sDestination,
															   GetName(),
															   1,
															   &dwArchivID,
															   stStart,
															   stEnd,
															   BBT_EXECUTE_AUTO);
		}
		else
		{
			m_pCIPCDatabaseServerBackup->DoRequestBackup(m_dwBackupID,
														 sDestination,
														 GetName(),
														 1,
														 &dwArchivID);
		}

		m_eBackupState = BS_RUNNING;
	}
	else
	{
		// error
		m_eBackupState = BS_FAILED;
	}
}
//////////////////////////////////////////////////////////////////////
void CBackupProcess::OnBackupSuccess()
{
	Lock(_T(__FUNCTION__));
	m_eBackupState = BS_CONFIRMED;
	Unlock();
}
//////////////////////////////////////////////////////////////////////
void CBackupProcess::BackupConfirmed()
{
	if (GetMacro().GetBackupDeleteAtSuccess())
	{
		// just delete all backup sequences
		m_pCIPCDatabaseServerBackup->DeleteConfirmedSequences();
		Lock(_T(__FUNCTION__));
		m_eBackupState = BS_DELETING;
		Unlock();
	}
	else
	{
		Lock(_T(__FUNCTION__));
		m_eBackupState = BS_FINISHED;
		Unlock();
	}
}
//////////////////////////////////////////////////////////////////////
void CBackupProcess::WaitForDeletes()
{
	if (m_pCIPCDatabaseServerBackup->GetNrOfConfirmedSequencesToDelete()==0)
	{
		Lock(_T(__FUNCTION__));
		m_eBackupState = BS_FINISHED;
		Unlock();
	}
	else
	{
		WK_TRACE(_T("#### not all sequences were deleted: missing %d\n"), m_pCIPCDatabaseServerBackup->GetNrOfConfirmedSequencesToDelete());
	}
}
//////////////////////////////////////////////////////////////////////
void CBackupProcess::OnBackupError()
{
	WK_TRACE(_T("automatic backup error stopping process\n"));
	Lock(_T(__FUNCTION__));
	m_eBackupState = BS_FAILED;
	Unlock();
	if(!GetMacro().GetBackupDestination().IsEmpty())
	{
		CAutoCritSec acs(&gm_csAutoBackupProcesses);
		DWORD dwLetter = GetMacro().GetBackupDestination().GetAt(0);
		gm_dwaBackupErrorUserData.Add(dwLetter);
	}
}
