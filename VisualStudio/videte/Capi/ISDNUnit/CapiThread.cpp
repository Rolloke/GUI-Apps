
#include "stdafx.h"
#include "CapiThread.h"
#include "HiddenCapiWnd.h"
#include "ISDNConnection.h"
#include "ISDNunit.h"
#include "KnockKnockBox.h"

/////////////////////////////////////////////////////////////////////////////
static TCHAR BASED_CODE szCapiThread[] = _T("CapiThread");

/////////////////////////////////////////////////////////////////////////////
CCapiThread::CCapiThread()
{
	m_pConnection = NULL;
	m_bTerminate = FALSE;
	m_iNormalOnIdleCount=0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCapiThread::InitInstance( )
{
	InitVideteLocaleLanguage();
	WK_STAT_LOG(_T("Thread"),2,szCapiThread);
	WK_STAT_LOG(_T("Thread"),1,szCapiThread);

//	TRACE(_T("Thread %s ThreadProc this = %08lx, m_pWinThread = %08lx\n"),
//					pThread->m_sName, pThread, pThread->m_pWinThread);
	WK_TRACE(_T("Thread %s     Id = %08lx\n"),szCapiThread, GetCurrentThreadId());
	WK_TRACE(_T("Thread %s Handle = %08lx\n"),szCapiThread, m_hThread);

	// Erste und z.Z. einzige ISDN-Verbindung anlegen
	m_pConnection = new CISDNConnection(MyGetApp(), 
							MyGetApp()->m_pControl, ((WORD)1));

	m_pCapiWnd = new CHiddenCapiWnd(this);
	m_pCapiWnd->m_hWnd = NULL;
	if (m_pCapiWnd->CreateEx(0, AfxRegisterWndClass(0), _T("Capi Notify Window"),
								WS_OVERLAPPED, 0, 0, 0, 0, NULL, NULL))
	{
		// fine
	}
	else 
	{
		ASSERT(0);
	}

	BOOL bInitOK = m_pConnection->InitCapi(m_pCapiWnd->GetSafeHwnd());

	// CAVEAT special thread trick, InitInstance
	// waits for this call
	// 0xFFFFFFFF indicates error
	if (bInitOK)
	{
		MyGetApp()->SetConnection(m_pConnection);
	}
	else
	{
		MyGetApp()->SetConnection((CISDNConnection*)0xFFFFFFFF);
		WK_DELETE(m_pConnection);
		if (m_pCapiWnd)
		{
			m_pCapiWnd->DestroyWindow();
		}
	}

	// must return TRUE to avoid ExitInstance right now
	// terminate will be done from main thread
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
int CCapiThread::ExitInstance()
{
	MyGetApp()->SetConnection(NULL);
	WK_STAT_LOG(_T("Thread"), 0, szCapiThread);
	WK_TRACE(_T("Thread %s EXIT !\n"), szCapiThread);
	CWinThread::ExitInstance();

	// try to unleash possible CrossThreadXXX
	if (m_pConnection)
	{
		m_pConnection->m_bDoCloseCapi=FALSE;
		m_pConnection->m_bDoResetCapi=FALSE;
		Sleep(50);
		WK_DELETE(m_pConnection);
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
int CCapiThread::Run()
{
	ASSERT_VALID(this);

	// for tracking the idle time state
	BOOL bIdle = TRUE;
	LONG lIdleCount = 0;

#if _MSC_VER >= 1300
	_AFX_THREAD_STATE* pState = AfxGetThreadState();
	#define MSC_VER_MSG_CUR (pState->m_msgCur)
#else
	#define MSC_VER_MSG_CUR m_msgCur
#endif

	// acquire and dispatch messages until a WM_QUIT message is received.
	for (;;)
	{
		// phase1: check to see if we can do idle work
		while (bIdle &&
			!::PeekMessage(&MSC_VER_MSG_CUR, NULL, NULL, NULL, PM_NOREMOVE))
		{
			// call OnIdle while in bIdle state
			if (!OnIdle(lIdleCount++))
				bIdle = FALSE; // assume "no idle" state
		}
		// phase2: pump messages while available
		do
		{
			// pump message, but quit on WM_QUIT

			CWnd* pMainWnd = AfxGetMainWnd();
			if (   pMainWnd==NULL
				|| ::IsWindow(pMainWnd->GetSafeHwnd())==FALSE 
				)
			{
				WK_TRACE(_T("Lost mainWnd, terminating...\n"));
				return ExitInstance();
			}

			if (!PumpMessage())
			{
				return ExitInstance();
			}

			// reset "no idle" state after pumping "normal" message
			if (IsIdleMessage(&MSC_VER_MSG_CUR))
			{
				bIdle = TRUE;
				lIdleCount = 0;
			}

		}
		while (::PeekMessage(&MSC_VER_MSG_CUR, NULL, NULL, NULL, PM_NOREMOVE));
	}
}
/////////////////////////////////////////////////////////////////////////////
static DWORD dwLastFlush=0;
/////////////////////////////////////////////////////////////////////////////
BOOL CCapiThread::OnIdle( LONG lCount)
{
	if (m_bTerminate)
	{
		WK_TRACE(_T("CapiThread STOP\n"));

		::PostQuitMessage(123);
		CWinThread::OnIdle(lCount);
		Sleep(10);	// OOPS

		return FALSE;	// do not continue, but keep PumpMesage in action
	}
	else if (m_pConnection)
	{
		if (m_pConnection->m_bDoResetCapi)
		{
			// WK_TRACE(_T("CapiThread RESET...\n"));
			// perform capi reset
			m_pConnection->InitCapi(m_pCapiWnd->GetSafeHwnd());
			m_pConnection->m_bDoResetCapi=FALSE;	// sigh a public member
			// WK_TRACE(_T("CapiThread RESET done.\n"));
		}
		else if (m_pConnection->m_bDoCloseCapi)
		{
			// WK_TRACE(_T("CapiThread CLOSE...\n"));
			m_pConnection->CloseConnection();
			m_pConnection->m_bDoCloseCapi=FALSE;	// sigh a public member
			// WK_TRACE(_T("CapiThread CLOSE DONE.\n"));
		}
		else
		{
			int iNumActions = m_pConnection->PollConnection();
			// OOPS how often is that , or much much does the window check cost
			if (   m_pConnection
				&& m_pConnection->m_pKnockKnockBox
				&& IsWindow(m_pConnection->m_pKnockKnockBox->GetSafeHwnd())
				)
			{
				CWK_Profile wkp;
				int		iFirstChannels	= wkp.GetInt(_T("ISDNUnit"), szChannelsConnected, 0);
				if (   m_pConnection->m_pKnockKnockBox->IsWindowVisible()
					&& MyGetApp()->IsKnockBoxEnabled()
					&& (   (m_pConnection->GetConnectState() == CONNECT_OPEN)
						|| (   theApp.IsSecondInstance()
							&& (iFirstChannels >= 2)
							)
						)
					)
				{
					if (GetTickCount()-dwLastFlush>555)
					{
						CString sCurrentTitle;
						m_pConnection->m_pKnockKnockBox->GetWindowText(sCurrentTitle);
						if (sCurrentTitle.GetLength())
						{
							if (sCurrentTitle[0]!=_T(' '))
							{
								// prepend space
								sCurrentTitle = _T(" ")  + sCurrentTitle;
							}
							else
							{
								// remove leading space
								sCurrentTitle.TrimLeft();
							}
							m_pConnection->m_pKnockKnockBox->SetWindowText(sCurrentTitle);
						}
						dwLastFlush = GetTickCount();
					}	// end of intervall check

				}
				else
				{
					// no more visible or disabled in settings
					m_pConnection->DeleteKnockKnockBox();
				}
			}	// end of pre check


			CWinThread::OnIdle(lCount);

			// NOT YET different sleep depending on connectState
			ISDNProcessorUsage usage = MyGetApp()->GetProcessorUsage();
			int iFactor;
			switch (usage)
			{
				case IPU_HIGH: iFactor = 1; break;
				case IPU_MEDIUM: iFactor = 3; break;
				case IPU_LOW: iFactor = 5; break;
				default:
					iFactor=1;
			}
			if (iNumActions)
			{
				Sleep(3*iFactor);	// sleep less if there are ongoing actions
			}
			else 
			{
				if (m_pConnection->IsActive())
				{
					Sleep(3*iFactor);
				}
				else
				{
					Sleep(50*iFactor);
				}
			}
		}
		return TRUE;	// do continue
	}
	else
	{
		// continue, terminate will be done by main thread
		return TRUE;	// do continue
	}
}
