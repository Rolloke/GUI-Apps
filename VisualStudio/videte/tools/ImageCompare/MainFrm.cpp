/////////////////////////////////////////////////////////////////////////////
// PROJECT:		ImageCompare
// FILE:		$Workfile: MainFrm.cpp $
// ARCHIVE:		$Archive: /Project/Tools/ImageCompare/MainFrm.cpp $
// CHECKIN:		$Date: 7.07.99 12:55 $
// VERSION:		$Revision: 18 $
// LAST CHANGE:	$Modtime: 7.07.99 12:54 $
// BY AUTHOR:	$Author: Uwe $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImageCompare.h"

#include "wk_trace.h"
#include "MainFrm.h"
#include "User.h"
#include "Permission.h"
#include <oemgui\oemlogindialog.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction
CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}

/////////////////////////////////////////////////////////////////////////////
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

// TODO: Delete these three lines if you don't want the toolbar to
//  be dockable
//	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
//	EnableDocking(CBRS_ALIGN_ANY);
//	DockControlBar(&m_wndToolBar);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.cx = 740;
	cs.cy = 400;
	cs.style |= WS_MAXIMIZE;
	cs.lpszName = theApp.m_pszAppName;
	cs.lpszClass = WK_APP_NAME_IMAGE_COMPARE;
	SetTitle(theApp.m_pszAppName);

	return CFrameWnd::PreCreateWindow(cs);

}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics
#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


