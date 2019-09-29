// MainFrm.cpp : Implementierung der Klasse CMainFrame
//

#include "stdafx.h"
#include "AnalyseMapFile.h"

#include "MainFrm.h"
#include "LeftView.h"
#include "AnalyseMapFileView.h"
#include "AnalyseMapFileDoc.h"
#include "CallStackView.h"

#include "searchaddressinmodule.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////////////
// CMainFrame
IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI_RANGE(AFX_ID_VIEW_MINIMUM, AFX_ID_VIEW_MAXIMUM, OnUpdateViewStyles)
	ON_COMMAND_RANGE(AFX_ID_VIEW_MINIMUM, AFX_ID_VIEW_MAXIMUM, OnViewStyle)
	ON_COMMAND(ID_EDIT_FIND_ADDRESS, OnEditFindAddress)
	ON_COMMAND(ID_EDIT_FIND_FUNCTION, OnEditFindFunction)
	ON_COMMAND(ID_EDIT_FIND_NEXT_FNC, OnEditFindNextFnc)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FIND_NEXT_FNC, OnUpdateEditFindNextFnc)
	ON_MESSAGE(WM_OPEN_FILE, OnOpenFile)
	ON_COMMAND(ID_EDIT_SEARCH_IN_MODULES, OnEditSearchInModules)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SEARCH_IN_MODULES, OnUpdateEditSearchInModules)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
static UINT indicators[] =
{
	ID_SEPARATOR,           // Statusleistenanzeige
	IDS_SEARCH_FUNCTION,
	IDS_SEARCH_ADDRESS,
	IDS_FOUND_OFFSET,
};


/////////////////////////////////////////////////////////////////////////////
// CMainFrame Erstellung/Zerstörung
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

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Symbolleiste konnte nicht erstellt werden\n");
		return -1;      // Fehler bei Erstellung
	}
	int nIdicators = sizeof(indicators)/sizeof(UINT);
	if (m_wndStatusBar.Create(this) && m_wndStatusBar.SetIndicators(indicators, nIdicators))
	{
		UINT nID, nStyle;
		int i, cxWidth;
		for (i=1; i<nIdicators; i++)
		{
			m_wndStatusBar.GetPaneInfo(i, nID, nStyle, cxWidth);
			switch(nID)
			{
				case IDS_SEARCH_FUNCTION:
					cxWidth = 250;
					break;
				case IDS_SEARCH_ADDRESS:
				case IDS_FOUND_OFFSET:
					cxWidth = 80;
					break;
			}
			m_wndStatusBar.SetPaneInfo(i, nID, nStyle,	cxWidth);
		}
	}
	else
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
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	CRect rc;
	GetWindowRect(&rc);
	CSize szLeft(MulDiv(rc.Width(), 20, 100), rc.Height());
	CSize szRight(rc.Width()-szLeft.cx, MulDiv(rc.Height(), 20, 100));

	if (!m_wndSplitter.CreateStatic(this, 1, 2))
		return FALSE;

	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CLeftView), CSize(szLeft.cx, szLeft.cy), pContext))
	{
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}

	if (!m_wndSplitter2.CreateStatic(&m_wndSplitter, 2, 1,  WS_CHILD|WS_VISIBLE|WS_BORDER,
		 m_wndSplitter.IdFromRowCol(0,1)))
	{
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}

	if (!m_wndSplitter2.CreateView(0, 0, RUNTIME_CLASS(CAnalyseMapFileView), CSize(szRight.cx, szLeft.cy-szRight.cy), pContext) ||
		!m_wndSplitter2.CreateView(1, 0, RUNTIME_CLASS(CCallStackView), szRight, pContext))
	{
		m_wndSplitter2.DestroyWindow();
		return FALSE;
	}

	m_wndSplitter2.RecalcLayout();


	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.lpszClass = ANALYSEMAPFILE_WND;

	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnOpenFile(WPARAM, LPARAM)
{
	CCommandLineInfo cmdInfo;
	GetWindowText(cmdInfo.m_strFileName);
	cmdInfo.m_nShellCommand = CCommandLineInfo::FileOpen;

	theApp.ProcessShellCommand(cmdInfo);
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
// CMainFrame Diagnose
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
// CMainFrame Meldungshandler
CLeftView* CMainFrame::GetLeftPane()
{
	CWnd* pWnd = m_wndSplitter.GetPane(0, 0);
	CLeftView* pView = DYNAMIC_DOWNCAST(CLeftView, pWnd);
	return pView;
}
/////////////////////////////////////////////////////////////////////////////
CAnalyseMapFileView* CMainFrame::GetRightPane()
{
	CWnd* pWnd = m_wndSplitter2.GetPane(0, 0);

	CAnalyseMapFileView* pView = DYNAMIC_DOWNCAST(CAnalyseMapFileView, pWnd);
	return pView;
}
/////////////////////////////////////////////////////////////////////////////
CCallStackView* CMainFrame::GetCallStackPane()
{
	CWnd* pWnd = m_wndSplitter2.GetPane(1, 0);

	CCallStackView* pView = DYNAMIC_DOWNCAST(CCallStackView, pWnd);
	return pView;
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateViewStyles(CCmdUI* pCmdUI)
{
	if (!pCmdUI)
		return;

	CAnalyseMapFileView* pView = GetRightPane(); 

	// Wurde der rechte Bereich nicht erstellt oder ist dieser keine Ansicht,
	// dann deaktivieren Sie die Befehle in unserem Bereich

	if (pView == NULL)
		pCmdUI->Enable(FALSE);
	else
	{
		DWORD dwStyle = pView->GetStyle() & LVS_TYPEMASK;

		// Ist der Befehl ID_VIEW_LINEUP, dann aktivieren Sie den Befehl nur, wenn
		// wir im Modus LVS_ICON oder LVS_SMALLICON sind

		if (pCmdUI->m_nID == ID_VIEW_LINEUP)
		{
			if (dwStyle == LVS_ICON || dwStyle == LVS_SMALLICON)
				pCmdUI->Enable();
			else
				pCmdUI->Enable(FALSE);
		}
		else
		{
			// andernfalls verwenden Sie Punkte, um das Format der Ansicht widerzuspiegeln
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
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnViewStyle(UINT nCommandID)
{
	// TODO: Passen Sie diesen Code an, damit die Auswahl im Menü 'Ansicht' behandelt werden kann.
	CAnalyseMapFileView* pView = GetRightPane();

	// Wenn der rechte Bereich erstellt wurde und eine CAnalyseMapFileView ist,
	// werden die Menübefehle verarbeitet...
	if (pView != NULL)
	{
		DWORD dwStyle = -1;

		switch (nCommandID)
		{
		case ID_VIEW_LINEUP:
			{
				// Das Listen-Steuerelement soll sich am Raster ausrichten
				CListCtrl& refListCtrl = pView->GetListCtrl();
				refListCtrl.Arrange(LVA_SNAPTOGRID);
			}
			break;

		// Weitere Befehle ändern das Format im Listen-Steuerelement
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

		// Format ändern; das Fenster baut sich automatisch neu auf
		if (dwStyle != -1)
			pView->ModifyStyle(LVS_TYPEMASK, dwStyle);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::SetPaneText(UINT nID, CString& sText)
{
	int i, nIndicators = sizeof(indicators)/sizeof(UINT);
	for (i=1; i<nIndicators; i++)
	{
		if (nID == m_wndStatusBar.GetItemID(i))
		{
			m_wndStatusBar.SetPaneText(i, sText);
			CString sTip;
			sTip.LoadString(nID);
			m_wndStatusBar.GetStatusBarCtrl().SetTipText(i, sTip);
			break;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnEditFindAddress()
{
	GetRightPane()->OnEditFindAddress();
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnEditFindFunction()
{
	GetRightPane()->OnEditFindFunction();
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnEditFindNextFnc()
{
	GetRightPane()->OnEditFindNextFnc();
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateEditFindNextFnc(CCmdUI *pCmdUI)
{
	GetRightPane()->OnUpdateEditFindNextFnc(pCmdUI);
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnEditSearchInModules()
{
	CSearchAddressInModule dlg;
	if (GetRightPane()->GetDocument()->GetFunctionParam().IsDllFile())
	{
		dlg.m_sModuleName  = GetRightPane()->GetDocument()->GetFunctionParam().m_sMapFile;
		dlg.m_sBaseAddress = theApp.DWORD64ToString(GetRightPane()->GetDocument()->GetFunctionParam().m_pFS->dwBaseAddress);
		dlg.m_sModuleSize  = theApp.DWORD64ToString(GetRightPane()->GetDocument()->GetFunctionParam().m_pFS->dwModuleSize);
	}
	dlg.DoModal();
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateEditSearchInModules(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}
/////////////////////////////////////////////////////////////////////////////
