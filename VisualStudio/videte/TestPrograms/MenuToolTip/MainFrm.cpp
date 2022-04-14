// MainFrm.cpp : Implementierung der Klasse CMainFrame
//

#include "stdafx.h"
#include "MenuToolTip.h"

#include "MainFrm.h"
#include <afxpriv.h>
#include ".\mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_MESSAGE(WM_SETMESSAGESTRING, OnSetMessageString)
	ON_MESSAGE(WM_ENTERMENULOOP, OnEnterMenuLoop)
	ON_MESSAGE(WM_EXITMENULOOP, OnExitMenuLoop)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_MENUSELECT()
	ON_WM_MENUCHAR()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // Statusleistenanzeige
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame Erstellung/Zerstörung

CMainFrame::CMainFrame()
{
	m_nTootipTimer = 0;
	m_bInMenuLoop = FALSE;
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Symbolleiste konnte nicht erstellt werden\n");
		return -1;      // Fehler bei Erstellung
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Statusleiste konnte nicht erstellt werden\n");
		return -1;      // Fehler bei Erstellung
	}
	// TODO: Löschen Sie diese drei Zeilen, wenn Sie nicht möchten, dass die Systemleiste andockbar ist
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Ändern Sie hier die Fensterklasse oder die Darstellung, indem Sie
	//  CREATESTRUCT cs modifizieren.

	return TRUE;
}


// CMainFrame Diagnose

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

void CMainFrame::KillToolTipTimer()
{
	if (m_nTootipTimer)
	{
		KillTimer(m_nTootipTimer);
	}
	m_nTootipTimer = 0;
}


// CMainFrame Meldungshandler
LRESULT CMainFrame::OnSetMessageString(WPARAM wParam, LPARAM lParam)
{
	KillToolTipTimer();
	m_nTootipTimer = (UINT)SetTimer((UINT_PTR) 1, 500, NULL);
	return CFrameWnd::OnSetMessageString(wParam, lParam);
}

LRESULT CMainFrame::OnEnterMenuLoop(WPARAM wParam, LPARAM )
{
	m_bInMenuLoop = TRUE;
	m_nFlags |= WF_TOOLTIPS;
//	m_nFlags |= WF_TRACKINGTOOLTIPS;
	CWnd::OnEnterMenuLoop((BOOL)wParam);
	return 0;
}

LRESULT CMainFrame::OnExitMenuLoop(WPARAM wParam, LPARAM )
{
	AFX_MODULE_THREAD_STATE* pModuleThreadState = AfxGetModuleThreadState();
	CToolTipCtrl* pToolTip = pModuleThreadState->m_pToolTip;
	pToolTip->SendMessage(TTM_DELTOOL, 0, (LPARAM)&pModuleThreadState->m_lastInfo);
	pToolTip->SendMessage(TTM_ACTIVATE, FALSE);

	m_bInMenuLoop = FALSE;
	m_nFlags &= ~(WF_TOOLTIPS|WF_TRACKINGTOOLTIPS);
	KillToolTipTimer();
	CWnd::OnExitMenuLoop((BOOL)wParam);
	return 0;
}

void CMainFrame::OnDestroy()
{
	KillToolTipTimer();
	CFrameWnd::OnDestroy();
}

void CMainFrame::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == m_nTootipTimer)
	{
		KillToolTipTimer();

		MSG msg = { m_hWnd, WM_MOUSEMOVE, 0, 0, 0, {0,0}};
		GetCursorPos(&msg.pt);
		msg.pt.x = m_rcMenu.right;
		msg.pt.y = m_rcMenu.bottom;
		CPoint pt = msg.pt;
		ScreenToClient(&pt);
		msg.lParam = MAKELONG(pt.x, pt.y);
		FilterToolTipMessage(&msg);
		if ((m_nFlags & WF_TRACKINGTOOLTIPS) == 0)
		{
			AFX_MODULE_THREAD_STATE* pModuleThreadState = AfxGetModuleThreadState();
			CToolTipCtrl* pToolTip = pModuleThreadState->m_pToolTip;
			if (pToolTip)
			{
//				SendMessage(TTM_TRACKPOSITION, 0, MAKELONG(msg.pt.x, msg.pt.y));
				pToolTip->SendMessage(TTM_TRACKACTIVATE, TRUE, (LPARAM)&pModuleThreadState->m_lastInfo);
			}
		}
	}
	CFrameWnd::OnTimer(nIDEvent);
}

INT_PTR  CMainFrame::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	if (m_bInMenuLoop 
		&& pTI != NULL)
//		&& pTI->cbSize >= sizeof(AFX_OLDTOOLINFO))
	{
		AFX_MODULE_THREAD_STATE* pModuleThreadState = AfxGetModuleThreadState();
		CToolTipCtrl* pToolTip = pModuleThreadState->m_pToolTip;
		if (pModuleThreadState->m_lastInfo.uId != m_nIDTracking)
		{
			pToolTip->SendMessage(TTM_DELTOOL, 0, (LPARAM)&pModuleThreadState->m_lastInfo);
			pToolTip->SendMessage(TTM_ACTIVATE, FALSE);
		}
		pTI->hwnd = m_hWnd;
		GetWindowRect(&pTI->rect);
//		pTI->rect = m_rcMenu;
		pTI->uId = m_nIDTracking;
		CString sTooltip;
		sTooltip.LoadString(m_nIDTracking);
		pTI->lpszText = _tcsdup(sTooltip);
		pTI->hinst = 0;//AfxGetInstanceHandle();
		pTI->uFlags |= TTF_ALWAYSTIP|TTF_ABSOLUTE|TTF_TRACK;
		return m_nIDTracking;
	}
	return -1;
}

void CMainFrame::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu)
{
	CFrameWnd::OnMenuSelect(nItemID, nFlags, hSysMenu);
	TRACE(_T("%x\n"), hSysMenu);
	if (hSysMenu)
	{
		int i, n = GetMenuItemCount(hSysMenu);
		for (i=0; i<n; i++)
		{
			if (GetMenuItemID(hSysMenu, i)==nItemID)
			{
				break;
			}
		}
		GetMenuItemRect(m_hWnd, hSysMenu, i, &m_rcMenu);
	}
}

LRESULT CMainFrame::OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu)
{
	return CFrameWnd::OnMenuChar(nChar, nFlags, pMenu);
}
