// BackupThread.cpp: implementation of the CBackupThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BackupThread.h"
#include "BackupProcess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBackupThread::CBackupThread(CBackupProcess* pBackupProcess)
 : CWK_Thread(_T("BackupThread"),NULL)
{
	m_pBackupProcess = pBackupProcess;
	m_pFetchResult = NULL;
	m_iTimeOutCounter = 100;
}
//////////////////////////////////////////////////////////////////////
CBackupThread::~CBackupThread()
{
	WK_DELETE(m_pFetchResult);
}
//////////////////////////////////////////////////////////////////////
BOOL CBackupThread::Run(LPVOID lpContext)
{
	if (!DoFetch())
	{
		m_iTimeOutCounter--;
		if (m_iTimeOutCounter>0)
		{
			return TRUE;
		}
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
CIPCFetchResult* CBackupThread::GetFetchResult()
{
	CIPCFetchResult* pFetchResult;
	m_cs.Lock();
	pFetchResult = m_pFetchResult;
	m_cs.Unlock();
	return pFetchResult;
}
//////////////////////////////////////////////////////////////////////
BOOL CBackupThread::DoFetch()
{
	CPermissionArray pa;
	CWK_Profile wkp;
	pa.Load(wkp);
	CPermission* pPermission = pa.GetSuperVisor();

	CConnectionRecord c;
	c.SetDestinationHost(LOCAL_LOOP_BACK);
	c.SetPermission(pPermission->GetName());
	c.SetPassword(pPermission->GetPassword());
	c.SetSourceHost(m_pBackupProcess->GetName());

	BOOL bSuccess = FALSE;
	CIPCFetch fetch;
	CIPCFetchResult fr = fetch.FetchDatabase(c);
	CString shmDatabaseName;
	shmDatabaseName = fr.GetShmName();
	if ( (fr.GetError()!=CIPC_ERROR_OKAY) || (shmDatabaseName.GetLength()==0))
	{
		WK_TRACE(_T("fetch database failed %s retry\n"),fr.GetErrorMessage());
		Sleep(200);
		bSuccess = FALSE;
	}
	else
	{
		WK_TRACE(_T("fetch database success\n"));
		m_cs.Lock();
		m_pFetchResult = new CIPCFetchResult(fr);
		m_cs.Unlock();
		bSuccess = TRUE;
	}

	if (!bSuccess)
	{
		WK_TRACE(_T("fetch database failed %s\n"),fr.GetErrorMessage());
	}

	return bSuccess;
}
//////////////////////////////////////////////////////////////////////
void CBackupThread::DeleteFetchResult()
{
	WK_DELETE(m_pFetchResult);
}
