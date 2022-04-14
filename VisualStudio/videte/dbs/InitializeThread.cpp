// InitializeThread.cpp: implementation of the CInitializeThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dbs.h"
#include "MainFrm.h"
#include "InitializeThread.h"

//////////////////////////////////////////////////////////////////////
// CInitializeThread Construction/Destruction
//////////////////////////////////////////////////////////////////////
CInitializeThread::CInitializeThread(BOOL bInit)
: CWK_Thread(_T("InitializeThread"))
{
	m_bInit = bInit;
}
//////////////////////////////////////////////////////////////////////
CInitializeThread::~CInitializeThread()
{

}
//////////////////////////////////////////////////////////////////////
BOOL CInitializeThread::Run(LPVOID lpContext)
{
	if (m_bInit)
	{
		theApp.Initialize();
		theApp.m_pMainWnd->PostMessage(WM_USER,WPARAM_INITIALIZE_THREAD);
	}
	else
	{
		theApp.Exit();
		theApp.m_pMainWnd->PostMessage(WM_USER,WPARAM_INITIALIZE_THREAD);
		theApp.m_pMainWnd->PostMessage(WM_COMMAND,ID_APP_EXIT);
	}


	return FALSE;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCheckDriveThread::CCheckDriveThread() : CWK_Thread(_T("CheckDriveThread"))
{
	m_bAutoDelete = TRUE;
	StartThread();
}
//////////////////////////////////////////////////////////////////////
CCheckDriveThread::~CCheckDriveThread()
{
	theApp.m_pCheckDrivesThread = NULL;
}
//////////////////////////////////////////////////////////////////////
BOOL CCheckDriveThread::Run(LPVOID lpContext)
{
	BOOL bResult = TRUE;
	if (theApp.m_bExiting || theApp.m_bFullResetting)
	{
		m_bCheck = FALSE;
		bResult = FALSE;
	}
	if (m_bCheck)
	{
		bResult = theApp.CheckDrives(TRUE);
		m_bCheck = FALSE;
	}

	return bResult;
}
//////////////////////////////////////////////////////////////////////
void CCheckDriveThread::AutoDelete()
{
	delete this;
}
//////////////////////////////////////////////////////////////////////
void CCheckDriveThread::DoCheck()
{
	m_bCheck = TRUE;
}

