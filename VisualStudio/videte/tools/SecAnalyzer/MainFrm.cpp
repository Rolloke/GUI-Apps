// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "SecAnalyzer.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_COMMAND_EX(CG_ID_VIEW_SECANALYZERBAR, OnBarCheck)
	ON_UPDATE_COMMAND_UI(CG_ID_VIEW_SECANALYZERBAR, OnUpdateControlBarMenu)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_CLEAR_TIMES, OnClearTimes)
	ON_COMMAND(ID_CLEAR_PATTERNS, OnClearPatterns)
	//}}AFX_MSG_MAP
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, CFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CFrameWnd::OnHelpFinder)
	// added by HEDY
	ON_COMMAND(IDC_LAST_RESET, OnLastReset)
	ON_COMMAND(IDC_START_DEFAULT, OnStartDefault)
	ON_COMMAND(IDC_RELATIVE_END, OnRelativeEnd)
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
	
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
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.Create(this) ||
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

	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	m_wndToolBar.ShowWindow(SW_HIDE);

	// TODO: Add a menu item that will toggle the visibility of the
	// dialog bar named "SecAnalyzerBar":
	//   1. In ResourceView, open the menu resource that is used by
	//      the CMainFrame class
	//   2. Select the View submenu
	//   3. Double-click on the blank item at the bottom of the submenu
	//   4. Assign the new item an ID: CG_ID_VIEW_SECANALYZERBAR
	//   5. Assign the item a Caption: SecAnalyzerBar

	// TODO: Change the value of CG_ID_VIEW_SECANALYZERBAR to an appropriate value:
	//   1. Open the file resource.h
	//   2. Find the definition for the symbol CG_ID_VIEW_SECANALYZERBAR
	//   3. Change the value of the symbol. Use a value in the range
	//      0xE804 to 0xE81A that is not already used by another symbol

	// CG: The following block was inserted by the 'Dialog Bar' component
	{
		// Initialize dialog bar m_wndSecAnalyzerBar
		if (!m_wndSecAnalyzerBar.Create(this, CG_IDD_SECANALYZERBAR,
			CBRS_BOTTOM | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_HIDE_INPLACE,
			CG_ID_VIEW_SECANALYZERBAR))
		{
			TRACE0("Failed to create dialog bar m_wndSecAnalyzerBar\n");
			return -1;		// fail to create
		}

		m_wndSecAnalyzerBar.EnableDocking(CBRS_ALIGN_BOTTOM | CBRS_ALIGN_TOP);
		EnableDocking(CBRS_ALIGN_ANY);
		DockControlBar(&m_wndSecAnalyzerBar);
	}

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFrameWnd::PreCreateWindow(cs);
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

void CMainFrame::OnClearTimes() 
{
	DoClearTimes();
}

void CMainFrame::DoClearTimes() 
{
	if (m_wndSecAnalyzerBar) {
		m_wndSecAnalyzerBar.SetDlgItemText(IDC_START_TIME,_T(""));
		m_wndSecAnalyzerBar.SetDlgItemText(IDC_START_DATE,_T(""));
		m_wndSecAnalyzerBar.SetDlgItemText(IDC_END_TIME,_T(""));
		m_wndSecAnalyzerBar.SetDlgItemText(IDC_END_DATE,_T(""));
	}
}

void CMainFrame::DoClearPatterns() 
{
	if (m_wndSecAnalyzerBar) {
		m_wndSecAnalyzerBar.SetDlgItemText(IDC_INCLUDE_PATTERN,_T(""));
		m_wndSecAnalyzerBar.SetDlgItemText(IDC_EXCLUDE_PATTERN,_T(""));
	}
}

void CMainFrame::DoClearMisc() 
{
	if (m_wndSecAnalyzerBar) {
		m_wndSecAnalyzerBar.SetDlgItemText(IDC_NUM_OLD_FILES,_T(""));
	}
}



static inline void EnableWindow(CWnd &wnd, DWORD nID,BOOL bState)
{
	CWnd *pWnd = wnd.GetDlgItem(nID);
	if (pWnd) { pWnd->EnableWindow(bState); }
}
static void ApplyRules()
{
	CWnd &bar = GetMain()->m_wndSecAnalyzerBar;
//	BOOL bUseDefault = bar.IsDlgButtonChecked(IDC_START_DEFAULT);
	BOOL bUseStart = bar.IsDlgButtonChecked(IDC_LAST_RESET);
	BOOL bUseLast = bar.IsDlgButtonChecked(IDC_RELATIVE_END);
	// use last reset
	// --> no start time/date
	if (bUseStart) {
		EnableWindow(bar,IDC_START_TIME,FALSE);
		EnableWindow(bar,IDC_START_DATE,FALSE);
		EnableWindow(bar,IDC_END_TIME,TRUE);
		EnableWindow(bar,IDC_END_DATE,TRUE);
	} else 	if (bUseLast) {
		// relative end, only start time
		EnableWindow(bar,IDC_START_TIME,TRUE);
		EnableWindow(bar,IDC_START_DATE,FALSE);
		EnableWindow(bar,IDC_END_TIME,FALSE);
		EnableWindow(bar,IDC_END_DATE,FALSE);
	} else {
		EnableWindow(bar,IDC_START_TIME,TRUE);
		EnableWindow(bar,IDC_START_DATE,TRUE);
		EnableWindow(bar,IDC_END_TIME,TRUE);
		EnableWindow(bar,IDC_END_DATE,TRUE);
	}
}

void CMainFrame::OnLastReset() 
{
	ApplyRules();
}

void CMainFrame::OnStartDefault() 
{
	ApplyRules();
}

void CMainFrame::OnRelativeEnd() 
{
	ApplyRules();
}


void CMainFrame::OnClearPatterns() 
{
	DoClearPatterns();
}

/////////////////////////////////////////////////////////////////////////////
long CMainFrame::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
		::PostMessage(hWnd, WM_SELFCHECK, WAI_ANALYZER, 0);

	return 0;
}