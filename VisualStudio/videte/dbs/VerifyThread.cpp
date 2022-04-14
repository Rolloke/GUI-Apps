// VerifyThread.cpp: implementation of the CVerifyThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VerifyThread.h"
#include "dbs.h"
#include "mainfrm.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CVerifyThread::CVerifyThread() :CWK_Thread(_T("VerifyThread"))
{
	m_bCancel = FALSE;
	m_bIsRunning = FALSE;
	m_bVerify = TRUE;
	m_wArchive = 0;
	m_wSequence = 0;
}
//////////////////////////////////////////////////////////////////////
CVerifyThread::~CVerifyThread()
{
}
//////////////////////////////////////////////////////////////////////
BOOL CVerifyThread::Run(LPVOID lpContext)
{
	m_bCancel = FALSE;
	Verify();
	return m_bVerify;
}
//////////////////////////////////////////////////////////////////////
void CVerifyThread::Cancel()
{
	m_bCancel = TRUE;
}
//////////////////////////////////////////////////////////////////////
void CVerifyThread::Verify()
{
	if (m_bVerify)
	{
		if (   m_wArchive == 0
			&& m_wSequence == 0)
		{
			WK_TRACE(_T("starting verify\n"));
		}
		CSequence* pSequence = theApp.m_Archives.GetNextSequence(m_wArchive,m_wSequence);
		if (pSequence)
		{
			pSequence->Verify();
			m_wArchive = pSequence->GetArchivNr();
			m_wSequence = pSequence->GetNr();
		}
		else
		{
			theApp.GetSequenceMap().Flush();
			WK_TRACE(_T("verify finished\n"));
			m_bVerify = FALSE;
			m_wSequence = 0;
			m_wArchive = 0;
			theApp.OffsetCorrected();
		}
	}
}



