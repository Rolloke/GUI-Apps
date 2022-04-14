// MainFrm.cpp : Implementierung der Klasse CMainFrame
//

#include "stdafx.h"
#include "TestMotionDetection.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()


// CMainFrame Erstellung/Zerstörung

CMainFrame::CMainFrame()
{
	// TODO: Hier Code für die Memberinitialisierung einfügen
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Ansichtsfenster konnte nicht erstellt werden\n");
		return -1;
	}
	SetMenu(NULL);

#ifndef _DEBUG
	CMonitorInfo mi;
	int nMonitors = mi.GetNrOfMonitors();
	if (!IsBetween(theApp.m_nMonitor, 0, nMonitors-1))
	{
		theApp.m_nMonitor = 0;
	}
	CRect rc;
	mi.GetMonitorRect(theApp.m_nMonitor, rc);
	ModifyStyle(WS_CAPTION, 0);
	SetWindowPos(&CWnd::wndTopMost, rc.left, rc.top, rc.Width(), rc.Height(), SWP_FRAMECHANGED|SWP_ASYNCWINDOWPOS);
#endif

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.style = CS_HREDRAW| CS_VREDRAW| FWS_ADDTOTITLE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;

	cs.lpszClass = AfxRegisterWndClass(0);//cs.style);
//	cs.lpszClass = AfxRegisterWndClass(cs.style, theApp.LoadStandardCursor(IDC_ARROW), (HBRUSH)GetStockObject(WHITE_BRUSH), theApp.LoadIcon(IDR_MAINFRAME));
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


// CMainFrame Meldungshandler

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// Fokus an das Ansichtfenster weitergeben
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// Ansichtfenster erhält ersten Eindruck vom Befehl
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// andernfalls die Standardbehandlung durchführen
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
