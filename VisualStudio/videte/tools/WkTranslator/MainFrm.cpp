// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "WkTranslator.h"
#include "WkTranslatorDoc.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame
IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(ID_EDIT_FIND, OnUpdateEditFind)
	ON_UPDATE_COMMAND_UI(ID_EDIT, OnUpdateEdit)
	ON_UPDATE_COMMAND_UI(ID_NEXT_TODO, OnUpdateNextTodo)
	ON_UPDATE_COMMAND_UI(ID_CHECK_MAX_LENGTH, OnUpdateCheckMaxLength)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS_GLOSSARY, OnUpdateFileSaveAsGlossary)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER, OnUserMsg)
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////
CMainFrame::~CMainFrame()
{
}


/////////////////////////////////////////////////////////////////////////////
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

/*
	CMenu *pM = GetMenu();
	if (pM)
	{
		MENUITEMINFO mii;

		ZeroMemory(&mii, sizeof(MENUITEMINFO));
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = 0x00000080;

		pM->GetMenuItemInfo(0, &mii, TRUE);
		mii.hbmpItem = (HBITMAP)-1;

		::SetMenuItemInfo(pM->m_hMenu, 0, TRUE, &mii);
	}
*/

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
//	cs.style &= ~FWS_ADDTOTITLE;

	return CFrameWnd::PreCreateWindow(cs);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::DocHasData()
{
	BOOL bHasData = FALSE;
	CDocument* pDoc = GetActiveDocument();
	if (pDoc->IsKindOf(RUNTIME_CLASS(CWkTranslatorDoc)))
	{
		bHasData = ((CWkTranslatorDoc*)pDoc)->HasData();
	}
	return bHasData;
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
/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::OnUpdateEditFind(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(DocHasData());
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateEdit(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(DocHasData());
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateNextTodo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(DocHasData());
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateCheckMaxLength(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(DocHasData());
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateFileSaveAsGlossary(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(DocHasData());
}

void CMainFrame::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT /* lpDrawItemStruct */)
{
	nIDCtl = 0;
	
//	TRACE(_T("MenuID:%d\n"), lpDrawItemStruct->itemData);
}

LRESULT CMainFrame::OnUserMsg(WPARAM wParam, LPARAM lParam)
{
	CDocument *pDoc = GetActiveDocument();
	if (pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CWkTranslatorDoc)))
	{
		return ((CWkTranslatorDoc*)pDoc)->OnUserMsg(wParam, lParam);
	}
	return 0;
}