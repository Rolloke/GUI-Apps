// CallThread.cpp: implementation of the CCallThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CallThread.h"
#include "MultipleCallProcess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCallThread::CCallThread(CMultipleCallProcess* pProcess)
 : CWK_Thread(_T("CallThread"),NULL)
{
	m_pProcess = pProcess;
	m_pInputResult = NULL;
	m_pOutputResult = NULL;
	m_sHost = pProcess->GetCurrentHost()->GetName();
}
//////////////////////////////////////////////////////////////////////
CCallThread::~CCallThread()
{
	WK_DELETE(m_pInputResult);
	WK_DELETE(m_pOutputResult);
}
//////////////////////////////////////////////////////////////////////
void CCallThread::Lock()
{
	m_cs.Lock();
}
///////////////////////////////////////////////////////////////////
void CCallThread::Unlock()
{
	m_cs.Unlock();
}
//////////////////////////////////////////////////////////////////////
BOOL CCallThread::Run(LPVOID lpContext)
{
	Sleep(200);

	DoFetch();

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
CIPCFetchResult* CCallThread::GetInputResult()
{
	CIPCFetchResult* pInputResult;
	m_cs.Lock();
	pInputResult = m_pInputResult;
	m_cs.Unlock();
	return pInputResult;
}
//////////////////////////////////////////////////////////////////////
CIPCFetchResult* CCallThread::GetOutputResult()
{
	CIPCFetchResult* pOutputResult;
	m_cs.Lock();
	pOutputResult = m_pOutputResult;
	m_cs.Unlock();
	return pOutputResult;
}
//////////////////////////////////////////////////////////////////////
BOOL CCallThread::DoFetch()
{
	CHost* pHost = m_pProcess->GetCurrentHost();
	const CPermission* pPermission = m_pProcess->GetMacro().GetPermission();

	CConnectionRecord c;
	c.SetDestinationHost(pHost->GetNumber());
	c.SetPermission(pPermission->GetName());
	c.SetPassword(pPermission->GetPassword());
	c.SetSourceHost(m_pProcess->GetName());

	if (pHost->IsPINRequired())
	{
		CString sPIN = pHost->GetPIN();
		if (!sPIN.IsEmpty())
		{
			c.SetFieldValue(CRF_PIN, sPIN);
		}
	}

// gf HACK CheckCallProcess uses only 1 B-Channel
	if (   (m_pProcess->GetType() == SPT_CHECK_CALL)
//		&& (pHost->IsISDN())  // not only ISDN-Type, may be routing also
		)
	{
		TRACE(_T("CheckCall ISDN, use only 1 B-Channel\n"));
		CString sBChannels;
		sBChannels.Format(_T("%u"), 1);
		if (!sBChannels.IsEmpty())
		{
			c.SetFieldValue(CRF_B_CHANNELS, sBChannels);
		}
	}

	BOOL bSuccess = FALSE;
	CIPCFetch fetch;
	
	CIPCFetchResult frInput = fetch.FetchInput(c);
	
	if (   (frInput.GetError()!=CIPC_ERROR_OKAY) 
		|| (frInput.GetShmName().GetLength()==0)
		)
	{
		WK_TRACE(_T("fetch input failed to %s, %s\n"),m_sHost,frInput.GetErrorMessage());
		bSuccess = FALSE;
	}
	else
	{
		WK_TRACE(_T("fetch input to %s success\n"),m_sHost);
		bSuccess = TRUE;
	}

	m_cs.Lock();
	m_pInputResult = new CIPCFetchResult(frInput);
	m_cs.Unlock();

	if (bSuccess)
	{
		// to free ServerControl on the other side
		// sleep a bit
		Sleep(200);
		CIPCFetchResult frOutput = fetch.FetchOutput(c);

		if (   (frOutput.GetError()!=CIPC_ERROR_OKAY) 
			|| (frOutput.GetShmName().GetLength()==0)
			)
		{
			WK_TRACE(_T("fetch output failed to %s, %s\n"),m_sHost,frInput.GetErrorMessage());
			bSuccess = FALSE;
		}
		else
		{
			WK_TRACE(_T("fetch output to %s success\n"),m_sHost);
			bSuccess = TRUE;
		}

		m_cs.Lock();
		m_pOutputResult = new CIPCFetchResult(frOutput);
		m_cs.Unlock();
	}
	else 
	{
		// same result for output
		m_cs.Lock();
		m_pOutputResult = new CIPCFetchResult(frInput);
		m_cs.Unlock();
	}


	return bSuccess;
}
//////////////////////////////////////////////////////////////////////
void CCallThread::DeleteInputResult()
{
	m_cs.Lock();
	WK_DELETE(m_pInputResult);
	m_cs.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CCallThread::DeleteOutputResult()
{
	m_cs.Lock();
	WK_DELETE(m_pOutputResult);
	m_cs.Unlock();
}
