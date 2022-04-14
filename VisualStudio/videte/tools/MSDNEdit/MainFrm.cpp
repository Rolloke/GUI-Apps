// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "MSDNEdit.h"

#include "MainFrm.h"
#include "LeftView.h"
#include "MSDNEditView.h"

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
	ON_COMMAND(ID_EDIT_CLEAR, OnEditDelete)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditDelete)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT, OnUpdateFileExport)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EXPAND_TREE, OnUpdateViewExpandTree)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI_RANGE(AFX_ID_VIEW_MINIMUM, AFX_ID_VIEW_MAXIMUM, OnUpdateViewStyles)
	ON_COMMAND_RANGE(AFX_ID_VIEW_MINIMUM, AFX_ID_VIEW_MAXIMUM, OnViewStyle)
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
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
   
   PostMessage(WM_COMMAND, ID_FILE_RELOAD, (LPARAM)m_hWnd);
	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	// create splitter window
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
		return FALSE;

	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CLeftView), CSize(200, 100), pContext) ||
		!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CMSDNEditView), CSize(100, 100), pContext))
	{
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}

	return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

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

CMSDNEditView* CMainFrame::GetRightPane()
{
	CWnd* pWnd = m_wndSplitter.GetPane(0, 1);
	CMSDNEditView* pView = DYNAMIC_DOWNCAST(CMSDNEditView, pWnd);
	return pView;
}

void CMainFrame::OnUpdateViewStyles(CCmdUI* pCmdUI)
{
	// TODO: customize or extend this code to handle choices on the
	// View menu.

	CMSDNEditView* pView = GetRightPane(); 

	// if the right-hand pane hasn't been created or isn't a view,
	// disable commands in our range

	if (pView == NULL)
		pCmdUI->Enable(FALSE);
	else
	{
		DWORD dwStyle = pView->GetStyle() & LVS_TYPEMASK;

		// if the command is ID_VIEW_LINEUP, only enable command
		// when we're in LVS_ICON or LVS_SMALLICON mode

		if (pCmdUI->m_nID == ID_VIEW_LINEUP)
		{
			if (dwStyle == LVS_ICON || dwStyle == LVS_SMALLICON)
				pCmdUI->Enable();
			else
				pCmdUI->Enable(FALSE);
		}
		else
		{
			// otherwise, use dots to reflect the style of the view
			pCmdUI->Enable();
			BOOL bChecked = FALSE;

			switch (pCmdUI->m_nID)
			{
			case ID_VIEW_DETAILS:
				bChecked = (dwStyle == LVS_REPORT);
				break;

			case ID_VIEW_SMALLICON:
				bChecked = (dwStyle == LVS_SMALLICON);
				break;

			case ID_VIEW_LARGEICON:
				bChecked = (dwStyle == LVS_ICON);
				break;

			case ID_VIEW_LIST:
				bChecked = (dwStyle == LVS_LIST);
				break;

			default:
				bChecked = FALSE;
				break;
			}

			pCmdUI->SetRadio(bChecked ? 1 : 0);
		}
	}
}


void CMainFrame::OnViewStyle(UINT nCommandID)
{
	// TODO: customize or extend this code to handle choices on the
	// View menu.
	CMSDNEditView* pView = GetRightPane();

	// if the right-hand pane has been created and is a CMSDNEditView,
	// process the menu commands...
	if (pView != NULL)
	{
		DWORD dwStyle = -1;

		switch (nCommandID)
		{
		case ID_VIEW_LINEUP:
			{
				// ask the list control to snap to grid
				CListCtrl& refListCtrl = pView->GetListCtrl();
				refListCtrl.Arrange(LVA_SNAPTOGRID);
			}
			break;

		// other commands change the style on the list control
		case ID_VIEW_DETAILS:
			dwStyle = LVS_REPORT;
			break;

		case ID_VIEW_SMALLICON:
			dwStyle = LVS_SMALLICON;
			break;

		case ID_VIEW_LARGEICON:
			dwStyle = LVS_ICON;
			break;

		case ID_VIEW_LIST:
			dwStyle = LVS_LIST;
			break;
		}

		// change the style; window will repaint automatically
		if (dwStyle != -1)
			pView->ModifyStyle(LVS_TYPEMASK, dwStyle);
	}
}

void CMainFrame::OnEditDelete() 
{
   CView *pView = GetActiveView();
   if (pView)
   {
      if (pView->IsKindOf(RUNTIME_CLASS(CLeftView)))
      {
         ((CLeftView*)pView)->OnEditDelete();
      }
      else if (pView->IsKindOf(RUNTIME_CLASS(CMSDNEditView)))
      {
         ((CMSDNEditView*)pView)->OnEditDelete();
      }
   }
}

void CMainFrame::OnEditCopy() 
{
   CView *pView = GetActiveView();
   if (pView)
   {
      if (pView->IsKindOf(RUNTIME_CLASS(CLeftView)))
      {
         ((CLeftView*)pView)->OnEditCopy();
      }
   }
}

void CMainFrame::OnEditPaste() 
{
   CView *pView = GetActiveView();
   if (pView)
   {
      if (pView->IsKindOf(RUNTIME_CLASS(CLeftView)))
      {
         ((CLeftView*)pView)->OnEditPaste();
      }
   }
}

void CMainFrame::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
   CView *pView = GetActiveView();
   if (pView)
   {
      if (pView->IsKindOf(RUNTIME_CLASS(CLeftView)))
      {
         ((CLeftView*)pView)->OnUpdateEditPaste(pCmdUI);
      }
      else
      {
         pCmdUI->Enable(false);
      }
   }
}

/*********************************************************************************************
 Klasse  : CMainFrame
 Funktion: OnUpdateEditDelete
 Zweck   : Prüft, ob etwas im aktiven View selektiert ist.

 Parameter:  
  pCmdUI: (E): Command Update Interface  (CCmdUI*)

 Rückgabewert:  (void)
 Author: Rolf Kary-Ehlers
 erstellt am: 07. Juni 2002
*********************************************************************************************/
void CMainFrame::OnUpdateEditDelete(CCmdUI* pCmdUI)
{
   CView *pView = GetActiveView();
   if (pView->IsKindOf(RUNTIME_CLASS(CLeftView)))
   {
      ((CLeftView*)pView)->OnUpdateEditDelete(pCmdUI);
   }
   else if (pView->IsKindOf(RUNTIME_CLASS(CMSDNEditView)))
   {
      ((CMSDNEditView*)pView)->OnUpdateEditDelete(pCmdUI);
   }
   else
   {
      pCmdUI->Enable(false);
   }
}

void CMainFrame::OnUpdateFileExport(CCmdUI* pCmdUI) 
{
	OnUpdateEditDelete(pCmdUI); // s.o.
}

void CMainFrame::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	OnUpdateEditDelete(pCmdUI); // s.o.
}

void CMainFrame::OnUpdateViewExpandTree(CCmdUI* pCmdUI) 
{
	OnUpdateEditDelete(pCmdUI); // s.o.
}

void CMainFrame::OnEditCut() 
{
   CView *pView = GetActiveView();
   if (pView)
   {
      if (pView->IsKindOf(RUNTIME_CLASS(CLeftView)))
      {
         ((CLeftView*)pView)->OnEditCopy(true);
      }
   }
}



void CMainFrame::OnTimer(UINT nIDEvent) 
{
   if (nIDEvent == MSDN_WATCH_TIMER)
   {
      CMSDNEditApp *pApp = (CMSDNEditApp*) AfxGetApp();
      if (!pApp->TestMSDNIntProcess())
      {
         KillTimer(nIDEvent);
         EnableWindow(true);
         SendMessage(WM_COMMAND, ID_FILE_RELOAD, (LPARAM)m_hWnd);
      }
   }	
	CFrameWnd::OnTimer(nIDEvent);
}
