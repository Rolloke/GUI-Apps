// GuardTourThread.cpp: implementation of the CGuardTourThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sec3.h"
#include "GuardTourThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

volatile BOOL CGuardTourThread::m_sbFetching = FALSE;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGuardTourThread::CGuardTourThread(CGuardTourProcess* pProcess, CConnectionRecord* pCR)
: CWK_Thread(_T("GuardTourThread<")+pCR->GetSourceHost()+_T(">"))
{
	m_pProcess = pProcess;
	m_pFetchResult = NULL;
	m_pConnectionRecord = new CConnectionRecord(*pCR);
	m_bCancelled = FALSE;
	m_bFetching = FALSE;
}
//////////////////////////////////////////////////////////////////////
CGuardTourThread::~CGuardTourThread()
{
	WK_DELETE(m_pConnectionRecord);
	DeleteFetchResult();
}
//////////////////////////////////////////////////////////////////////
void CGuardTourThread::Lock()
{
	m_cs.Lock();
}
///////////////////////////////////////////////////////////////////
void CGuardTourThread::Unlock()
{
	m_cs.Unlock();
}
//////////////////////////////////////////////////////////////////////
BOOL CGuardTourThread::StopThread()
{
	Cancel();
	return CWK_Thread::StopThread();
}
//////////////////////////////////////////////////////////////////////
BOOL CGuardTourThread::Run(LPVOID lpContext)
{
	if (m_sbFetching)
	{
		if (m_bCancelled)
		{
			m_pFetchResult = new CIPCFetchResult("",
												CIPC_ERROR_CANCELLED,
												0,
												"",
												0,
												SECID_NO_ID
#ifdef _UNICODE
												, CODEPAGE_UNICODE
#endif
												);
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
	else
	{
		m_sbFetching = TRUE;
		m_bFetching = TRUE;
		DoFetch();
		if (m_pFetchResult->GetErrorCode() == CIPC_ERROR_CANCELLED)
		{
			Sleep(1000);
		}
		else if (m_pFetchResult->GetErrorCode() == CIPC_ERROR_TIMEOUT)
		{
			Sleep(500);
		}
		else
		{
			Sleep(100);
		}
		m_sbFetching = FALSE;
		m_bFetching = FALSE;

		return FALSE;
	}
}
//////////////////////////////////////////////////////////////////////
void CGuardTourThread::Cancel()
{
	if (   m_bFetching
		&& !m_Fetch.IsCancelled())
	{
		WK_TRACE(_T("cancelling guard tour fetch to %s\n"),m_pConnectionRecord->GetSourceHost());
		m_Fetch.Cancel();
	}
	m_bCancelled = TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL CGuardTourThread::DoFetch()
{
	CIPCFetchResult frAlarm;
	BOOL bSuccess = FALSE;

	frAlarm = m_Fetch.FetchAlarmConnection(*m_pConnectionRecord);
	
	if (frAlarm.GetError()!=CIPC_ERROR_OKAY)
	{
		WK_TRACE(_T("fetch guard tour failed to %s, %s %s\n"),
			m_pConnectionRecord->GetSourceHost(),
			frAlarm.GetErrorMessage(),
			NameOfEnum(frAlarm.GetError()));
		bSuccess = FALSE;
	}
	else
	{
		WK_TRACE(_T("fetch guard tour to %s version %s success\n"),
			m_pConnectionRecord->GetSourceHost(),frAlarm.GetErrorMessage());
		bSuccess = TRUE;
	}

	m_cs.Lock();
	m_pFetchResult = new CIPCFetchResult(frAlarm);
	m_cs.Unlock();

	return bSuccess;
}
//////////////////////////////////////////////////////////////////////
void CGuardTourThread::DeleteFetchResult()
{
	m_cs.Lock();
	WK_DELETE(m_pFetchResult);
	m_cs.Unlock();
}
//////////////////////////////////////////////////////////////////////
CIPCFetchResult* CGuardTourThread::GetFetchResult()
{
	return m_pFetchResult;
}
