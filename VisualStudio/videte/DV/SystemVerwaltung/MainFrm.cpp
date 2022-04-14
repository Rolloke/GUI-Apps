// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "SystemVerwaltung.h"

#include "MainFrm.h"

#include "SVDoc.h"

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
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	// Global help commands
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
END_MESSAGE_MAP()

/*	ON_COMMAND(ID_HELP_FINDER, CFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CFrameWnd::OnHelpFinder)
*/
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
	
	if (COemGuiApi::GetComCtl32Version()==COMCTL32_471)
	{
		if (!m_wndCoolBar.Create(this,
			WS_CHILD|WS_VISIBLE|WS_BORDER|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|
				RBS_TOOLTIPS|RBS_BANDBORDERS|RBS_VARHEIGHT)) {
			TRACE0("Failed to create cool bar\n");
			return -1;      // fail to create
		}
	}
	else
	{
		if (!m_wndToolBar.Create(this) ||
			!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
		{
			TRACE0("Failed to create tool bar\n");
			return -1;      // fail to create
		}
		m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
			CBRS_ALIGN_TOP | CBRS_TOOLTIPS);
		m_wndToolBar.SetHeight(40);
		TBBUTTON tb;
		CString s;
		int i,c;
		c = m_wndToolBar.GetToolBarCtrl().GetButtonCount();
		for (i=0;i<c;i++)
		{
			if (m_wndToolBar.GetToolBarCtrl().GetButton(i,&tb))
			{
				if (tb.idCommand != ID_SEPARATOR)
				{
					s.LoadString(tb.idCommand);
					s = s.Left(s.Find('\n'));
					m_wndToolBar.SetButtonText(i,s);
				}
			}
		}
		m_wndToolBar.GetToolBarCtrl().SetButtonSize(CSize(72,42));
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
/*	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);*/

// TODO gf CoolMenuManager funzt nicht mit MFC 7.1
#if _MFC_VER < 0x0070
	m_CoolMenuManager.m_bAutoAccel = FALSE;
	m_CoolMenuManager.Install(this);
	m_CoolMenuManager.LoadToolbar(IDR_MAINFRAME);
#endif

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	BOOL ret;
	
	ret =  CFrameWnd::PreCreateWindow(cs);

	cs.lpszClass = WK_APP_NAME_SUPERVISOR;
	cs.style &= ~FWS_ADDTOTITLE;

	return ret;
}
/////////////////////////////////////////////////////////////////////////////
CWnd* CMainFrame::GetToolBar()
{
	if (WK_IS_WINDOW(&m_wndCoolBar))
	{
		return &m_wndCoolBar;
	}
	if (WK_IS_WINDOW(&m_wndToolBar))
	{
		return &m_wndToolBar;
	}
	return NULL;
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
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
CSVDoc* CMainFrame::GetDocument()
{
	return (CSVDoc*)GetActiveDocument();
}
/////////////////////////////////////////////////////////////////////////////
long CMainFrame::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
		::PostMessage(hWnd, WM_SELFCHECK, WAI_SUPERVISOR, 0);

	return 0;
}