// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "DVProcess.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_PANE_STORAGE,
	ID_PANE_IC,
	ID_PANE_OC,
	ID_PANE_AC,
	ID_PANE_IG,
	ID_PANE_OG,
	ID_PANE_AG,
	ID_PANE_LED,
	ID_PANE_AUDIO,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
	ON_MESSAGE(WM_USER, OnUser)
	ON_MESSAGE(WM_TRAYCLICKED, OnTrayClicked)	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_hwndStarter = NULL;
	m_uTimerID = 0;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	NOTIFYICONDATA tnd;

	CString sTip;
	
	GetWindowText(sTip);

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
	BOOL bReturn =  CFrameWnd::PreCreateWindow(cs);

	if (bReturn)
	{
		cs.style = WS_OVERLAPPED | WS_CAPTION 
			| WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
		cs.lpszClass = WK_APP_NAME_SERVER;
		cs.lpszName = AfxGetApp()->m_pszAppName;
	}

	return bReturn;
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
LRESULT CMainFrame::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
	{
		m_hwndStarter = hWnd;
		::PostMessage(hWnd, WM_SELFCHECK, WAI_SECURITY_SERVER, 0);
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnUser(WPARAM wParam, LPARAM lParam)
{
	WORD wCause = LOWORD(wParam);
	switch (wCause)
	{
	case WPARAM_ALARM_ON:
		{
			WORD wMD = HIWORD(wParam);
			BYTE x,y;

			x = LOBYTE(wMD);
			y = HIBYTE(wMD);
			theApp.OnSoftwareAlarm(CSecID((DWORD)lParam),TRUE,(WORD)((x*1000/256)),(WORD)((y*1000/256)));
		}
		break;
	case WPARAM_ALARM_OFF:
		{
			WORD wMD = HIWORD(wParam);
			BYTE x,y;

			x = LOBYTE(wMD);
			y = HIBYTE(wMD);
			theApp.OnSoftwareAlarm(CSecID((DWORD)lParam),FALSE,0,0);
		}
		break;
	case WPARAM_INPUT_CONNECTION_RECORD:
		theApp.OnInputConnectionRecord();
		break;
	case WPARAM_OUTPUT_CONNECTION_RECORD:
		theApp.OnOutputConnectionRecord();
		break;
	case WPARAM_AUDIO_CONNECTION_RECORD:
		theApp.OnAudioConnectionRecord();
		break;
	default:
		theApp.OnIdle(0);
		break;
	}
	return 0L;
}
/////////////////////////////////////////////////////////////////////////////
HWND CMainFrame::GetStarterWindowHandle()
{
	return m_hwndStarter;
}


void CMainFrame::OnDestroy() 
{
	CFrameWnd::OnDestroy();
	
	NOTIFYICONDATA tnd;

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= m_hWnd;
	tnd.uID			= 1;
	tnd.uFlags		= 0;

	Shell_NotifyIcon(NIM_DELETE, &tnd);
	KillTimer(m_uTimerID);
	
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

			SetForegroundWindow();		// Siehe ID: Q135788 
			pM->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
			PostMessage(WM_NULL, 0, 0); // Siehe ID: Q135788
			break;
		}
	}

	return 0;
}

void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam) 
{
	CFrameWnd::OnSysCommand(nID, lParam);
#ifndef _DEBUG
	if (nID == SC_MINIMIZE)
	{
		ShowWindow(SW_HIDE);
	}
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
