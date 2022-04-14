/* GlobalReplace: pApp\-\> --> theApp. */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: mainfrm.cpp $
// ARCHIVE:		$Archive: /Project/SecurityServer/mainfrm.cpp $
// CHECKIN:		$Date: 25.04.06 17:16 $
// VERSION:		$Revision: 64 $
// LAST CHANGE:	$Modtime: 25.04.06 16:48 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include <CommCtrl.h>
#include <wk_msg.h>
#include "sec3.h"

#include "mainfrm.h"

#include "ProcessListDoc.h"
#include "ProcessListView.h"
#include "ProcessScheduler.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	ON_WM_CLOSE()
	ON_WM_TIMECHANGE()
	ON_WM_TIMER()
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
	ON_MESSAGE(WM_TRAYCLICKED, OnTrayClicked)	
	ON_MESSAGE(WM_LANGUAGE_CHANGED, OnLanguageChanged)	
	//}}AFX_MSG_MAP
	// Global help commands
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// arrays of IDs used to initialize control bars


static UINT BASED_CODE indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	IDS_PANE_UNITS,	// 1
	IDS_PANE_NO_UNIT,
	IDS_PANE_CLIENTS,	// 3
	IDS_PANE_NO_CLIENT,
	IDS_PANE_DATABASE,	// 5
	IDS_PANE_NO_DB
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_uTimerID = 0;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1) {
		return -1;
	}
	
	EnableDocking(CBRS_ALIGN_ANY);

	// Statusbar ///////////////////////////////////////////////////////////////////////
	if (!m_wndStatusBar.Create(this, WS_CHILD | CBRS_BOTTOM) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetPaneStyle(1,SBPS_NOBORDERS);
	m_wndStatusBar.SetPaneStyle(3,SBPS_NOBORDERS);
	m_wndStatusBar.SetPaneStyle(5,SBPS_NOBORDERS);

	SetWindowText(AfxGetApp()->m_pszAppName);

	// LoadBarState(getstr(IDS_REG_CTRLBARS));

	GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND|MF_GRAYED);

	// Icon ins Systemtray
	NOTIFYICONDATA tnd;
	
	CString sTip = COemGuiApi::GetApplicationName(WAI_SECURITY_SERVER);
	
	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= m_hWnd;
	tnd.uID			= 1;
	
	tnd.uFlags		= NIF_MESSAGE|NIF_ICON|NIF_TIP;
	tnd.uCallbackMessage = WM_TRAYCLICKED;
	tnd.hIcon		= theApp.LoadIcon(IDR_MAINFRAME);
	
	lstrcpyn(tnd.szTip, sTip, sTip.GetLength()+1);
	Shell_NotifyIcon(NIM_ADD, &tnd);

	return 0;
}
///////////////////////////////////////////////////////////////////////
void CMainFrame::SetTimer(UINT nElapse)
{
	if (m_uTimerID != 0)
	{
		KillTimer(m_uTimerID);
		m_uTimerID = 0;
	}
	m_uTimerID = CWnd::SetTimer(1,nElapse,NULL);
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	BOOL ret =  CFrameWnd::PreCreateWindow(cs);

	cs.style = WS_OVERLAPPED | WS_CAPTION 
		| WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;// | WS_MAXIMIZE;
	cs.lpszClass = WK_APP_NAME_SERVER;

	return ret;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnClose() 
{
	// SaveBarState(getstr(IDS_REG_CTRLBARS));
	
	CFrameWnd::OnClose();
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnDestroy() 
{
	CFrameWnd::OnDestroy();
	
	// Nur erforderlich, wenn Icon in der Systemtaskleiste
	NOTIFYICONDATA tnd;

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= m_hWnd;
	tnd.uID			= 1;
	tnd.uFlags		= 0;

	Shell_NotifyIcon(NIM_DELETE, &tnd);
	KillTimer(m_uTimerID);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam) 
{
	CFrameWnd::OnSysCommand(nID, lParam);

	if (nID == SC_MINIMIZE)
		ShowWindow(SW_HIDE);
}

/////////////////////////////////////////////////////////////////////////////
long CMainFrame::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
	{
		::PostMessage(hWnd, WM_SELFCHECK, WAI_SECURITY_SERVER, 0);
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnLanguageChanged(WPARAM wParam, LPARAM lParam)
{
	return theApp.SetLanguageParameters((UINT)wParam, (DWORD)lParam);;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnTrayClicked(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
		case WM_LBUTTONDBLCLK:
			ShowWindow(SW_RESTORE); //SW_SHOWNORMAL);
			break;
		case WM_RBUTTONDOWN:
		{
			CMenu menu;
			CMenu* pM;
			CPoint pt;

			GetCursorPos(&pt);
			menu.LoadMenu(IDR_MAINFRAME);
			pM = menu.GetSubMenu(0);
			CString sAbout;
			menu.GetMenuString(ID_APP_ABOUT, sAbout, MF_BYCOMMAND);
			pM->InsertMenu(ID_APP_EXIT, MF_STRING | MF_BYCOMMAND, ID_APP_ABOUT, (LPCTSTR)sAbout);
			pM->InsertMenu(ID_APP_EXIT, MF_SEPARATOR | MF_BYCOMMAND, 0, (LPCTSTR)NULL);

			SetForegroundWindow();		// Siehe ID: Q135788 
			pM->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
			PostMessage(WM_NULL, 0, 0); // Siehe ID: Q135788
			break;
		}
	}

	return 0;
}

void CMainFrame::OnTimeChange() 
{
	CFrameWnd::OnTimeChange();

#ifndef _DEBUG
	WK_TRACE(_T("time changed by windows resetting\n"));
	PostMessage(WM_COMMAND,ID_EINSTELLUNGEN_SUPERVISOR);
#endif
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{
	if (m_uTimerID == nIDEvent)
	{
		theApp.OnTimer();
	}
	
	CFrameWnd::OnTimer(nIDEvent);
}

