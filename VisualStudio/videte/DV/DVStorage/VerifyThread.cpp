// VerifyThread.cpp: implementation of the CVerifyThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VerifyThread.h"
#include "DVStorage.h"
#include "mainfrm.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CVerifyThread::CVerifyThread()
:CWK_Thread(_T("VerifyThread"))
{
	m_bCancel = FALSE;
	m_bIsRunning = FALSE;
}
//////////////////////////////////////////////////////////////////////
CVerifyThread::~CVerifyThread()
{
}
//////////////////////////////////////////////////////////////////////
BOOL CVerifyThread::Run(LPVOID lpContext)
{
	m_bCancel = FALSE;
	BOOL bOK = TRUE;
	for (int i=0;i<theApp.m_Archives.GetSize() && !m_bCancel;i++)
	{
		CCollection* pArchive = theApp.m_Archives.GetAtFast(i);
		if (pArchive)
		{
			bOK &= pArchive->Verify(m_bCancel);
		}
	}

	if (bOK)
	{
		WK_TRACE(_T("DATABASE VERIFY OK\n"));
	}

	CWnd* pWnd = AfxGetMainWnd();
	if (WK_IS_WINDOW(pWnd))
	{
		pWnd->PostMessage(WM_USER,WPARAM_VERIFY_FINISHED);
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
void CVerifyThread::Cancel()
{
	m_bCancel = TRUE;
}

