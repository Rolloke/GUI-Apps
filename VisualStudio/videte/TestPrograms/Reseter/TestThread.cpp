// TestThread.cpp : implementation file
//

#include "stdafx.h"
#include "Reseter.h"
#include "TestThread.h"

#include "CIPCServerControlClientSide.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTestThread

IMPLEMENT_DYNCREATE(CTestThread, CWinThread)

CTestThread::CTestThread()
{
	m_bAutoDelete = FALSE;
	m_bThreadRun = TRUE;

	m_dwPauseTime=1000;
	m_bDoTest = FALSE;
}

CTestThread::~CTestThread()
{
	m_bThreadRun = FALSE;
}

BOOL CTestThread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int CTestThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CTestThread, CWinThread)
	//{{AFX_MSG_MAP(CTestThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestThread message handlers


int CTestThread::Run()
{
	while (m_bThreadRun) {
		if (m_bDoTest) {
			MessageBeep(0);

			// test action
			CConnectionRecord c;

			c.SetDestinationHost(LOCAL_LOOP_BACK);

			FetchServerReset(c);
		}
		Sleep(m_dwPauseTime);
	}

	return 0;
}

void CTestThread::StopTest()
{
	m_bDoTest=FALSE;
}

void CTestThread::StartTest(DWORD dwTime)
{
	m_dwPauseTime = dwTime;
	if (m_dwPauseTime<1000) {
		m_dwPauseTime=1000;
	}
	m_bDoTest=TRUE;	// CAVEAT has to be the last one, since it activates the test
}
