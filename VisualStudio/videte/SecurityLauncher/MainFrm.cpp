// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "SecurityLauncher.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CSecurityLauncherApp theApp;

static TCHAR BASED_CODE szSection[] = _T("SecurityLauncher");
static TCHAR BASED_CODE szPosition[] = _T("Position");
static TCHAR szFormat[] = _T("%d,%d,%d,%d");
/////////////////////////////////////////////////////////////////////////////
// CMainFrame
IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_SKIP, OnSkip)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction
CMainFrame::CMainFrame()
{
	m_Pos = 1;
	m_Point.x = 0;
	m_Point.y = 0;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.Create(this))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (theApp.m_pDongle->GetOemCode() == CWK_Dongle::OEM_GARNY)
	{
		if (!m_wndToolBar.LoadToolBar(IDR_MAINFRAME_GARNY))
		{
			return -1;
		}
	}
	else
	{
		if (!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
		{
			return -1;
		}
	}

	m_wndToolBar.SetWindowText(theApp.GetName());
	SetWindowText(theApp.GetName());
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle()|CBRS_TOOLTIPS | CBRS_ALIGN_TOP);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	BOOL ret = CFrameWnd::PreCreateWindow(cs);

	if (theApp.m_pDongle->GetOemCode()==CWK_Dongle::OEM_GARNY)
	{
		cs.style = 0;
	}
	else
	{
		cs.style = WS_MINIMIZEBOX|WS_SYSMENU;
	}

	CWK_Profile wkp;
	int nLauncherOnTaskBar = wkp.GetInt("SecurityLauncher","LauncherOnTaskBar",1);
	if (nLauncherOnTaskBar){
		cs.dwExStyle = WS_EX_TOOLWINDOW|WS_EX_TOPMOST|WS_EX_APPWINDOW; 
	}
	else{
		cs.dwExStyle = WS_EX_TOOLWINDOW|WS_EX_TOPMOST;//CT: No Icon on Taskbar
	}

	return ret;
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
void CMainFrame::InitialShowToolBar()
{
	CRect framerect;
	CRect toolrect;
	CRect firect;
	CRect lirect;

	GetMenu()->DestroyMenu();
	SetMenu(NULL);	
	CRect rect(0,0,190,47);

	CalcWindowRect(rect);

	CString sPos;
	int x,y,cx,cy;
	CRect dRect;

	GetDesktopWindow()->GetWindowRect(dRect);
	CWK_Profile wkp;
	sPos = wkp.GetString(szSection,szPosition,"");

	if (sPos.IsEmpty())
	{
		x = dRect.Width() - rect.Width();
		y = dRect.top;
		cx = rect.Width();
		cy = rect.Height();
	}
	else
	{
		sscanf((const char*)sPos,szFormat,&x,&y,&cx,&cy);

		if ( (x<dRect.left) ||
			 (y<dRect.top) ||
			 (x+cx>dRect.right) ||
			 (y+cy>dRect.bottom) )
		{
			// ein Teil des Fensters liegt außerhalb des Bildschirms
			// korrigiere Fensterposition
			x = dRect.left;
			y = dRect.top;
			cx = rect.Width();
			cy = rect.Height();
		}
	}
	SetWindowPos(&wndTopMost,x,y,rect.Width(),rect.Height(),0);
	ShowWindow(SW_SHOW);
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::SetPosition(int n) 
{
	CRect rect(0,0,0,0);
	CRect dRect, aRect;

	GetWindowRect(aRect);
	GetDesktopWindow()->GetWindowRect(dRect);

	if (n==2)
	{
		rect.left = dRect.Width() - aRect.Width();
	}
	else if (n==3)
	{
		rect.left = dRect.right - aRect.Width();
		rect.top  = dRect.bottom - aRect.Height();
	}
	else if (n==4)
	{
		rect.top  = dRect.bottom - aRect.Height();
	}
	else if (n==5)
	{
		CenterWindow(GetDesktopWindow());
		return;
	}
	SetWindowPos(&wndTopMost,rect.left,rect.top,rect.Width(),rect.Height(),SWP_NOSIZE);
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnSkip() 
{
	CRect rect;
	CRect center;
	
	m_wndToolBar.GetItemRect(m_wndToolBar.CommandToIndex(ID_SKIP),&rect);

	if (rect.PtInRect(m_Point))
	{
		center = rect;
		center.InflateRect(-rect.Width()/3,-rect.Height()/3);

		if (center.PtInRect(m_Point))
		{
			m_Pos = 5;
		}
		else
		{
			if (m_Point.x < (rect.left + rect.right)/2)
			{
				// left
				if (m_Point.y < (rect.top + rect.bottom)/2)
				{
					// top
					m_Pos = 1;
				}
				else
				{
					// bottom
					m_Pos = 4;
				}
			}
			else
			{
				// right
				if (m_Point.y < (rect.top + rect.bottom)/2)
				{
					// top
					m_Pos = 2;
				}
				else
				{
					// bottom
					m_Pos = 3;
				}
			}
		}
	}
	SetPosition(m_Pos);
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnClose() 
{
	if (theApp.m_pDongle->GetOemCode()==CWK_Dongle::OEM_GARNY)
	{
		theApp.TerminateAll();
		CFrameWnd::OnClose();
	}
	else
	{
		if (theApp.GetMode()==LM_NORMAL)
		{
			if (theApp.Login())
			{
				if (theApp.System())
				{
					theApp.TerminateAll();
					CFrameWnd::OnClose();
				}
			}
		}
		else if (theApp.GetMode()==LM_BACKUP)
		{
			CApplication* pApp = theApp.m_Apps.GetApplication(WAI_CDR_WRITER);
			if (pApp && pApp->IsRunning())
			{
				// do nothing
				AfxMessageBox(IDP_DBBACKUP_STILL_RUNNING);
			}
			else
			{
				theApp.SysDown();
				CFrameWnd::OnClose();
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnTimer(UINT nIDEvent) 
{
	theApp.Actualize20s();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if (wParam==ID_UPDATE)
	{
		theApp.OnUpdate(lParam);
		return TRUE;
	}
	
	if (wParam==ID_ERROR_ARRIVED)
	{
		theApp.OnErrorArrived();
		return TRUE;
	}
	return CFrameWnd::OnCommand(wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnDestroy() 
{
	// Automatically quit when the main window is destroyed.
	if (AfxGetApp()->m_pMainWnd == this)
	{
		// closing the main application window
		::WinHelp(m_hWnd, NULL, HELP_QUIT, 0L);

		// will call PostQuitMessage in CWnd::OnNcDestroy
	}

	CRect rect;
	CString sPos;
	GetWindowRect(rect);
	sPos.Format(szFormat,rect.left,rect.top,rect.Width(),rect.Height());
	CWK_Profile wkp;
	wkp.WriteString(szSection,szPosition,sPos);

	CWnd::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
CLauncherToolBar::CLauncherToolBar()
{
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CLauncherToolBar, CToolBar)
	//{{AFX_MSG_MAP(CLauncherToolBar)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CLauncherToolBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	((CMainFrame*)GetParent())->m_Point = point;
	CToolBar::OnLButtonDown(nFlags, point);
}

#ifdef AfxOldTOOLINFO
int CLauncherToolBar::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));

	// check child windows first by calling CControlBar
	int nHit = CControlBar::OnToolHitTest(point, pTI);
	if (nHit != -1)
		return nHit;

	// now hit test against CToolBar buttons
	int nButtons = (int)GetToolBarCtrl().GetButtonCount();
	for (int i = 0; i < nButtons; i++)
	{
		CRect rect;
		TBBUTTON button;
		if (GetToolBarCtrl().GetItemRect(i,rect) &&
			rect.PtInRect(point) &&
			GetToolBarCtrl().GetButton(i, &button) &&
			!(button.fsStyle & TBSTYLE_SEP))
		{
			int nHit = GetItemID(i);
			if (pTI != NULL && pTI->cbSize >= sizeof(AfxOldTOOLINFO))
			{
				pTI->hwnd = m_hWnd;
				pTI->rect = rect;
				pTI->uId = nHit;
				pTI->lpszText = LPSTR_TEXTCALLBACK;
			}
			// found matching rect, return the ID of the button
			return nHit != 0 ? nHit : -1;
		}
	}
	return -1;
}
#endif