// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "LogoutReminder.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_SHOW_ICON, OnShowIcon)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_UPDATE_COMMAND_UI(IDCANCEL, OnUpdateCancel)
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
   ON_MESSAGE(WM_APP+1, OnAppMessage1)
   ON_MESSAGE(WM_QUERYENDSESSION, OnQueryEndSession)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_nTimer           = 0;
	m_nDelayPopUpTimer = 0;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.uFlags = NIF_ICON|NIF_TIP|NIF_MESSAGE;
	nid.hWnd   = m_hWnd;
	nid.hIcon  = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	CTime t = CTime::GetCurrentTime();
	CString str = t.Format(IDS_TOOLTIP);
	strncpy(nid.szTip, LPCTSTR(str), 63);
	nid.uID    = 0;
	SetIcon(nid.hIcon, true);
	nid.uCallbackMessage = WM_APP + 1;

	if (!Shell_NotifyIcon(NIM_ADD, &nid))
	{
		if (nid.hIcon) ::DestroyIcon(nid.hIcon);
		return -1;
	}

	m_nDelayPopUpTimer = SetTimer(200, 5000, NULL);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
   
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

LRESULT CMainFrame::OnQueryEndSession(WPARAM wParam, LPARAM lParam) 
{
   CLogoutReminderApp* pApp = (CLogoutReminderApp*) AfxGetApp();
   if (!CFrameWnd::OnQueryEndSession())
		return FALSE;

   if (pApp->m_bShowDialog == SHUT_DOWN_EXIT)
   {
      return true;
   }

   if (lParam & ENDSESSION_LOGOFF)
   {
      pApp->m_bShowDialog = LOG_OFF;
   }
   else
   {
      pApp->m_bShowDialog = SHUT_DOWN;
   }

   PostMessage(WM_COMMAND, ID_APP_ABOUT, 0);

	return false;
}

void CMainFrame::OnShowIcon() 
{
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd   = m_hWnd;
	Shell_NotifyIcon(NIM_DELETE, &nid);
}

LRESULT CMainFrame::OnAppMessage1(WPARAM wParam, LPARAM lParam)
{
	if ((lParam == WM_CONTEXTMENU)||(lParam == WM_RBUTTONUP))
	{
		CMenu menu;
		((CLogoutReminderApp*)AfxGetApp())->m_bShowDialog = REMIND_DLG;
		if (menu.LoadMenu(IDR_MAINFRAME))
		{
			CMenu *pSub = menu.GetSubMenu(0);
			if (pSub)
			{
				CPoint ptCursor;
				::GetCursorPos(&ptCursor);
				pSub->TrackPopupMenu(0, ptCursor.x, ptCursor.y, this, NULL);
			}
		}
	}
	else
	{
	}
	return 0;
}

void CMainFrame::OnDestroy() 
{
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd   = m_hWnd;
	nid.hIcon  = GetIcon(true);
	Shell_NotifyIcon(NIM_DELETE, &nid);
	if (nid.hIcon) ::DestroyIcon(nid.hIcon);
	CFrameWnd::OnDestroy();
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == m_nTimer)
	{
		CLogoutReminderApp* pApp = (CLogoutReminderApp*) AfxGetApp();
		CTime tnow = CTime::GetCurrentTime();
		if (tnow >= pApp->m_LogoutTime)
		{
			pApp->m_LogoutTime = tnow + pApp->GetIncrementTimeSpan();
			Beep(880, 200);
			Beep(659, 300);
			PostMessage(WM_COMMAND, ID_APP_ABOUT, 0);
			StopTimer();
		}
        POSITION fTaskPos;
        for (fTaskPos = pApp->mTasks.GetHeadPosition(); fTaskPos != NULL; pApp->mTasks.GetNext(fTaskPos))
        {
            ScheduleTask&fTask = pApp->mTasks.GetAt(fTaskPos);
            if (!fTask.mIsActive && tnow >= fTask.mTime)
            {
	            STARTUPINFO startupinfo;
	            ZeroMemory(&startupinfo, sizeof(STARTUPINFO));              // StartupInfo Struktur füllen
	            startupinfo.cb          = sizeof(STARTUPINFO);
	            startupinfo.wShowWindow = SW_SHOW;
                fTask.mIsActive = CreateProcess(NULL, (char*)LPCTSTR(fTask.mTask), NULL, NULL, false, 0, NULL, NULL, &startupinfo, NULL);
            }
        }
	}

	if (nIDEvent == m_nDelayPopUpTimer)
	{
		Beep(659, 200);
		Beep(880, 300);
		PostMessage(WM_COMMAND, ID_APP_ABOUT, 0);
	}

	CFrameWnd::OnTimer(nIDEvent);
}

void CMainFrame::StartTimer()
{
	m_nTimer = SetTimer(102, 6000, NULL);
}

void CMainFrame::StopTimer()
{
	if (m_nTimer)
	{
		KillTimer(m_nTimer);
		m_nTimer = 0;
	}
	if (m_nDelayPopUpTimer)
	{
		KillTimer(m_nDelayPopUpTimer);
		m_nDelayPopUpTimer = 0;
	}
}

void CMainFrame::OnUpdateCancel(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(true);
}

void CMainFrame::OnCancel() {}

void CMainFrame::SetShellIconTip(CString &sTip)
{
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.uFlags = NIF_TIP;
	nid.hWnd   = m_hWnd;
	nid.hIcon  = NULL;
	nid.uID    = 0;
	strncpy(nid.szTip, LPCTSTR(sTip), 63);
	Shell_NotifyIcon(NIM_MODIFY, &nid);
}

void CMainFrame::OnKillFocus(CWnd* pNewWnd) 
{
	CFrameWnd::OnKillFocus(pNewWnd);
}
