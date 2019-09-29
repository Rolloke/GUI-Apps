// MainFrm.cpp : Implementierung der Klasse CMainFrame
//

#include "stdafx.h"
#include "AnyFileViewer.h"

#include "MainFrm.h"
#include "FormatView.h"
#include "AnyFileViewerView.h"
#include "AnyFileViewerTreeView.h"
#include "MessageView.h"
#include "AnyFileViewerDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame
const UINT    wm_Find = RegisterWindowMessage( FINDMSGSTRING );

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    ON_WM_CREATE()
    // Globale Hilfebefehle
    ON_COMMAND(ID_HELP_FINDER, CFrameWnd::OnHelpFinder)
    ON_COMMAND(ID_HELP, CFrameWnd::OnHelp)
    ON_COMMAND(ID_CONTEXT_HELP, CFrameWnd::OnContextHelp)
    ON_COMMAND(ID_DEFAULT_HELP, CFrameWnd::OnHelpFinder)
    ON_COMMAND(ID_VIEW_DLG_TOOL_BAR, OnViewDlgToolBar)
    ON_COMMAND(ID_VIEW_FORMAT_TOOL_BAR, OnViewFormatToolBar)
    ON_COMMAND(ID_VIEW_SEARCH_TOOL_BAR, OnViewSearchToolBar)
    ON_COMMAND(IDC_BTN_STEP_BY, CMoveFindDlgBar::OnBnClickedBtnStepBy)
    ON_COMMAND(IDC_BTN_RE_STEP_BY, CMoveFindDlgBar::OnBnClickedBtnReStepBy)
    ON_COMMAND(ID_EDIT_FIND, OnEditFind)
    ON_COMMAND(ID_FORMAT_DIFFERENT_ENDIANESS, OnFormatDifferentEndianess)
    ON_COMMAND(ID_FORMAT_SAME_ENDIANESS, OnFormatSameEndianess)
    ON_COMMAND(ID_FORMAT_CONVERT_DLG, OnFormatConvertDlg)
    ON_REGISTERED_MESSAGE( wm_Find, OnFind )
    ON_UPDATE_COMMAND_UI(ID_FORMAT_DIFFERENT_ENDIANESS, OnUpdateFormatDifferentEndianess)
    ON_UPDATE_COMMAND_UI(ID_FORMAT_SAME_ENDIANESS, OnUpdateFormatSameEndianess)
    ON_UPDATE_COMMAND_UI(ID_FORMAT_CONVERT_DLG, OnUpdateFormatConvertDlg)
    ON_UPDATE_COMMAND_UI(ID_VIEW_DLG_TOOL_BAR, OnUpdateViewDlgToolBar)
    ON_UPDATE_COMMAND_UI(ID_VIEW_FORMAT_TOOL_BAR, OnUpdateViewFormatToolBar)
    ON_UPDATE_COMMAND_UI(ID_VIEW_SEARCH_TOOL_BAR, OnUpdateViewSearchToolBar)
    ON_WM_DESTROY()
    ON_COMMAND(ID_FORMAT_LOAD, &CMainFrame::OnFormatLoad)
    ON_UPDATE_COMMAND_UI(ID_FORMAT_LOAD, &CMainFrame::OnUpdateFormatLoad)
    ON_COMMAND(ID_TREEVIEW_FILE_CONTENT, &CMainFrame::OnTreeviewFileContent)
    ON_COMMAND(ID_TREEVIEW_FILE_STRUCTURE, &CMainFrame::OnTreeviewFileStructure)
    ON_UPDATE_COMMAND_UI(ID_TREEVIEW_FILE_STRUCTURE, &CMainFrame::OnUpdateTreeviewFileStructure)
    ON_UPDATE_COMMAND_UI(ID_TREEVIEW_FILE_CONTENT, &CMainFrame::OnUpdateTreeviewFileContent)
    ON_WM_KEYDOWN()
END_MESSAGE_MAP()

static UINT indicators[] =
{
   ID_SEPARATOR,            // Statusleistenanzeige
   IDS_POSITION,            // 1
   IDS_POSITION_INT,        // 2
   IDS_ROW,                 // 3
   IDS_ROW_INT,             // 4
   IDS_COLUMN,              // 5
   IDS_COLUMN_INT,          // 6
};

// CMainFrame Erstellung/Zerstörung

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    CString s;
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;
    
    s.LoadString(IDS_MAIN_TOOL_BAR);
    if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
        | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
        !m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
    {
        TRACE0("Symbolleiste konnte nicht erstellt werden\n");
        return -1;      // Fehler bei Erstellung
    }
    m_wndToolBar.SetWindowText(s);

    s.LoadString(IDD_MAINFRAME);
    if (!m_wndDlgBar.Create(this, IDD_MAINFRAME, CBRS_GRIPPER|CBRS_ALIGN_TOP|CBRS_TOOLTIPS|CBRS_FLYBY, IDD_MAINFRAME))
    {
        TRACE0("DialogBar konnte nicht erstellt werden\n");
        return -1;      // Fehler bei Erstellung
    }
    m_wndDlgBar.SetWindowText(s);

    s.LoadString(IDD_SEARCH_DLG);
    if (!m_wndMoveFindDlgBar.Create(this, IDD_SEARCH_DLG, CBRS_GRIPPER|CBRS_ALIGN_TOP|CBRS_TOOLTIPS|CBRS_FLYBY, IDD_SEARCH_DLG))
    {
        TRACE0("DialogBar konnte nicht erstellt werden\n");
        return -1;      // Fehler bei Erstellung
    }
    m_wndMoveFindDlgBar.SetWindowText(s);

    s.LoadString(IDD_FORMAT_VIEW);
    if (!m_wndFmtDlgBar.Create(this, IDD_FORMAT_VIEW, CBRS_GRIPPER|CBRS_ALIGN_TOP|CBRS_TOOLTIPS|CBRS_FLYBY, IDD_FORMAT_VIEW))
    {
        TRACE0("DialogBar konnte nicht erstellt werden\n");
        return -1;      // Fehler bei Erstellung
    }
    m_wndFmtDlgBar.SetWindowText(s);

    if (!m_wndStatusBar.Create(this) ||
        !m_wndStatusBar.SetIndicators(indicators,
            sizeof(indicators)/sizeof(UINT)))
    {
        TRACE0("Statusleiste konnte nicht erstellt werden\n");
        return -1;      // Fehler bei Erstellung
    }
    m_wndStatusBar.SetPaneStyle(PANE_POSITION-1, SBPS_NOBORDERS);
    m_wndStatusBar.SetPaneStyle(PANE_ROW-1     , SBPS_NOBORDERS);
    m_wndStatusBar.SetPaneStyle(PANE_COLUMN-1  , SBPS_NOBORDERS);
    SetPaneInt(PANE_POSITION, 0);
    SetPaneInt(PANE_ROW, 0);
    SetPaneInt(PANE_COLUMN, 0);

    m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
    EnableDocking(CBRS_ALIGN_ANY);
    DockControlBar(&m_wndToolBar);
    m_wndDlgBar.EnableDocking(CBRS_ALIGN_ANY);
    DockControlBar(&m_wndDlgBar);
    m_wndFmtDlgBar.EnableDocking(CBRS_ALIGN_ANY);
    DockControlBar(&m_wndFmtDlgBar);
    m_wndMoveFindDlgBar.EnableDocking(CBRS_ALIGN_ANY);
    DockControlBar(&m_wndMoveFindDlgBar);

    int i, n=theApp.GetNoOfDataTypes();
    for (i=0; i<n; i++)
    {
        m_wndFmtDlgBar.SendDlgItemMessage(IDC_COMBO_DATA_TYPE_INSERT, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR)theApp.GetDisplayType(i).Type());
        if (i >= CDisplayType::Ascii)
        {
            m_wndFmtDlgBar.SendDlgItemMessage(IDC_COMBO_DATA_TYPE_INSERT, CB_SETITEMDATA, (WPARAM)i, (LPARAM) 1);
        }
        else
        {
            m_wndDlgBar.SendDlgItemMessage(IDC_COMBO_DATA_TYPE, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR)theApp.GetDisplayType(i).Type());
            m_wndDlgBar.SendDlgItemMessage(IDC_COMBO_DATA_TYPE, CB_SETITEMDATA, i, theApp.GetDisplayType(i).getType());
            m_wndFmtDlgBar.SendDlgItemMessage(IDC_COMBO_DATA_TYPE_INSERT, CB_SETITEMDATA, (WPARAM)i, (LPARAM)theApp.GetDisplayType(i).getType());
        }
    }

    m_wndFmtDlgBar.SendDlgItemMessage(IDC_COMBO_DATA_TYPE_INSERT, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR)CDisplayType::getNameOfType(CDisplayType::Structure));
    m_wndFmtDlgBar.SendDlgItemMessage(IDC_COMBO_DATA_TYPE_INSERT, CB_SETITEMDATA, (WPARAM)i, (LPARAM) CDisplayType::Structure);

    i = static_cast<int>(m_wndDlgBar.SendDlgItemMessage(IDC_COMBO_DATA_TYPE, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR)CDisplayType::getNameOfType(CDisplayType::AsciiLine)));
    m_wndDlgBar.SendDlgItemMessage(IDC_COMBO_DATA_TYPE, CB_SETITEMDATA, i, CDisplayType::AsciiLine);
    i = static_cast<int>(m_wndDlgBar.SendDlgItemMessage(IDC_COMBO_DATA_TYPE, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR)CDisplayType::getNameOfType(CDisplayType::UnicodeLine)));
    m_wndDlgBar.SendDlgItemMessage(IDC_COMBO_DATA_TYPE, CB_SETITEMDATA, i, CDisplayType::UnicodeLine);
    i = static_cast<int>(m_wndDlgBar.SendDlgItemMessage(IDC_COMBO_DATA_TYPE, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR)CDisplayType::getNameOfType(CDisplayType::Structure)));
    m_wndDlgBar.SendDlgItemMessage(IDC_COMBO_DATA_TYPE, CB_SETITEMDATA, i, CDisplayType::Structure);

    UpdateFormatFiles();

    CAnyFileViewerView* pView = GetMainPane();
    m_wndDlgBar.SendDlgItemMessage(IDC_COMBO_DATA_TYPE, CB_SETCURSEL, pView->GetDataType(), 0);
    m_wndFmtDlgBar.SendDlgItemMessage(IDC_COMBO_DATA_TYPE_INSERT, CB_SETCURSEL, 0, 0);
    m_wndFmtDlgBar.SendDlgItemMessage(IDC_COMBO_INSERT_POSITION, CB_SETCURSEL, CAnyFileViewerTreeView::ip::after, 0);
    m_wndDlgBar.SetDlgItemInt(IDC_EDT_COLUMNS, pView->GetColumns(), 0);
    m_wndDlgBar.SetDlgItemInt(IDC_EDT_OFFSET, pView->GetOffset(), 0);

    m_wndMoveFindDlgBar.SetDlgItemInt(IDC_EDT_STEP_BY, 1);
    m_wndMoveFindDlgBar.SetDlgItemInt(IDC_EDT_GOTO_POSITION, 0);

    LoadBarState(BAR_STATE);
    CRect rc;
    GetClientRect(&rc);
    int fWidth = rc.Width();
    int fHeight = rc.Height();
    int fcxCur0   = MulDiv(theApp.mcxViewPane0, fWidth, 1000);
    int fcxCur1   = MulDiv(theApp.mcxViewPane1, fWidth, 1000);
    int fcyCur1_0 = MulDiv(theApp.mcxViewPane1_0, fHeight, 1000);
    m_wndSplitter.SetColumnInfo(  0, fcxCur0, 5);
    m_wndSplitter.SetColumnInfo(  1, fcxCur1, 5);
    m_wndSplitterLists.SetRowInfo(0, fcyCur1_0, 5);

    return 0;
}

void CMainFrame::OnDestroy()
{
    int fcxCur0, fcxMin, fcxCur1, fcyCur1_0, fWidth, fHeight;
    CRect rc;
    GetClientRect(&rc);
    fWidth  = rc.Width();
    fHeight = rc.Height();
    m_wndSplitter.GetColumnInfo(0, fcxCur0, fcxMin);
    m_wndSplitter.GetColumnInfo(1, fcxCur1, fcxMin);
    m_wndSplitterLists.GetRowInfo(0, fcyCur1_0, fcxMin);
    theApp.mcxViewPane0 = MulDiv(fcxCur0, 1000, fWidth);
    theApp.mcxViewPane1 = MulDiv(fcxCur1, 1000, fWidth);
    theApp.mcxViewPane1_0 = MulDiv(fcyCur1_0, 1000, fHeight);
    CFrameWnd::OnDestroy();
}

void CMainFrame::SetPaneInt(int nPane, int n)
{
   CString str;
   str.Format(_T("%d"), n);
   m_wndStatusBar.SetPaneText(nPane, str);
}

void CMainFrame::SetPaneInt(int nPane, size_t n)
{
    CString str;
    str.Format(_T("%ld"), n);
    m_wndStatusBar.SetPaneText(nPane, str);
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /* lpcs */,
    CCreateContext* pContext)
{
    // Unterteiltes Fenster erstellen
    if (!m_wndSplitter.CreateStatic(this, 1, 3))
        return FALSE;

    if (   !m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CFormatView), CSize(200, 100), pContext)
        || !m_wndSplitter.CreateView(0, 2, RUNTIME_CLASS(CAnyFileViewerTreeView), CSize(200, 100), pContext)
        )
    {
        m_wndSplitter.DestroyWindow();
        return FALSE;
    }

    if (!m_wndSplitterLists.CreateStatic(&m_wndSplitter, 2, 1, WS_CHILD | WS_VISIBLE, m_wndSplitter.IdFromRowCol(0, 1)))
        return FALSE;

    if (   !m_wndSplitterLists.CreateView(0, 0, RUNTIME_CLASS(CAnyFileViewerView), CSize(200, 80), pContext)
        || !m_wndSplitterLists.CreateView(1, 0, RUNTIME_CLASS(CMessageView), CSize(200, 20), pContext)
        )
    {
        m_wndSplitter.DestroyWindow();
        return FALSE;
    }

/*
    if (   !m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CFormatView), CSize(200, 100), pContext)
        || !m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CAnyFileViewerView), CSize(200, 100), pContext)
        || !m_wndSplitter.CreateView(0, 2, RUNTIME_CLASS(CAnyFileViewerTreeView), CSize(200, 100), pContext)
        )
    {
        m_wndSplitter.DestroyWindow();
        return FALSE;
    }
*/
    return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if( !CFrameWnd::PreCreateWindow(cs) )
        return FALSE;
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


// CMainFrame Meldungshandler

CAnyFileViewerView* CMainFrame::GetMainPane()
{
    CWnd* pWnd = m_wndSplitterLists.GetPane(0, 0);
    CAnyFileViewerView* pView = DYNAMIC_DOWNCAST(CAnyFileViewerView, pWnd);
    return pView;
}

CFormatView* CMainFrame::GetFormatPane()
{
    CWnd* pWnd = m_wndSplitter.GetPane(0, 0);
    CFormatView* pView = DYNAMIC_DOWNCAST(CFormatView, pWnd);
    return pView;
}

CAnyFileViewerTreeView* CMainFrame::GetTreeViewPane()
{
    CWnd* pWnd = m_wndSplitter.GetPane(0, 2);
    CAnyFileViewerTreeView* pView = DYNAMIC_DOWNCAST(CAnyFileViewerTreeView, pWnd);
    return pView;
}

void CMainFrame::SetClipboardData(CString str)
{
   int nLen = str.GetLength();
   if (nLen && OpenClipboard())
   {
      HANDLE   hMem = NULL;
      _TCHAR*  pstr = NULL;
      try
      {
         UINT uFormat = CF_TEXT;
         nLen += 1;
#ifdef _UNICODE
         uFormat = CF_UNICODETEXT;
         nLen *= 2;
#endif
         if (!::EmptyClipboard())      throw GetLastError();
         hMem = ::GlobalAlloc(GMEM_DDESHARE|GMEM_MOVEABLE, nLen);
         if (hMem == 0)                throw GetLastError();
         pstr = (_TCHAR*)::GlobalLock(hMem);
         if (pstr == NULL)             throw GetLastError();
         memcpy(pstr, LPCTSTR(str), nLen);
         DWORD dw = ::GlobalUnlock(hMem);
         if (dw == 0)
         {
            dw = GetLastError();
            if (dw != 0) throw dw;
         }
         if (::SetClipboardData(uFormat, hMem))
         {
            hMem = NULL;
         }
         else                          throw GetLastError();
      }
      catch (DWORD dwError)
      {
         TRACE("Error SetClipboardData: %d\n", dwError);
         dwError = 0;
      }
      if (hMem)
      {
         ::GlobalFree(hMem);
      }
      ::CloseClipboard();
   }
}

CString CMainFrame::GetClipboardData()
{
    CString str;
    UINT uFormat = CF_TEXT;
    #ifdef _UNICODE
    uFormat = CF_UNICODETEXT;
    #endif
    if (   ::IsClipboardFormatAvailable(uFormat)
        && OpenClipboard())
    {
        HGLOBAL hMem;
        _TCHAR*  pSource, *pDest;
        try
        {
            hMem = ::GetClipboardData(uFormat);
            if (!hMem)                    throw GetLastError();
            pSource = (_TCHAR*)::GlobalLock(hMem);
            if (pSource == NULL)          throw GetLastError();
            int nLen = (int)_tcslen(pSource);
            pDest = str.GetBufferSetLength(nLen+1);
            if (pDest == NULL)            throw GetLastError();
            _tcscpy_s(pDest, nLen, pSource);
            str.ReleaseBuffer();
            DWORD dw = ::GlobalUnlock(hMem);
            if (dw == 0)
            {
                dw = GetLastError();
                if (dw != 0) throw dw;
            }
        }
        catch (DWORD dwError)
        {
            TRACE("Error GetClipboardData: %d\n", dwError);
            dwError = 0;
        }
        ::CloseClipboard();
    }
    return str;
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    CWnd*pWnd = GetFocus();
    if (   pWnd
        && pWnd != this)
    {
        if (pWnd->IsKindOf(RUNTIME_CLASS(CView)))
        {
            pWnd->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
        }
        else if (!pWnd->IsKindOf(RUNTIME_CLASS(CDialogBar)))
        {
            _TCHAR szName[8];
            GetClassName(pWnd->m_hWnd, szName, 8);
            if (_tcscmp(szName, _T("Edit")) == 0)
            {
                CEdit *pEdit = (CEdit*)pWnd;
                if (nCode == CN_UPDATE_COMMAND_UI)
                {
                    switch (((CCmdUI*)pExtra)->m_nID)
                    {
                    case ID_EDIT_COPY:  case ID_EDIT_PASTE:
                    case ID_EDIT_CLEAR: case ID_EDIT_CLEAR_ALL:
                    case ID_EDIT_CUT:   case ID_EDIT_UNDO:
                        ((CCmdUI*)pExtra)->Enable(TRUE);
                        return TRUE;
                    }
                }
                else
                {
                    BOOL bResult = TRUE;
                    switch (nID)
                    {
                    case ID_EDIT_COPY:      pEdit->Copy();  break;
                    case ID_EDIT_PASTE:     pEdit->Paste(); break;
                    case ID_EDIT_CLEAR:     pEdit->SendMessage(WM_KEYDOWN, VK_DELETE, 0); break;
                    case ID_EDIT_CLEAR_ALL: pEdit->Clear(); break;
                    case ID_EDIT_CUT:       pEdit->Cut();   break;
                    case ID_EDIT_UNDO:      pEdit->Undo();  break;
                    default: bResult = FALSE; break;
                    }
                    if (bResult) return TRUE;
                }
            }
        }
    }
    return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CMainFrame::DestroyWindow()
{
    SaveBarState(BAR_STATE);
    return CFrameWnd::DestroyWindow();
}

void CMainFrame::OnViewDlgToolBar()
{
    OnBarCheck(IDD_MAINFRAME);
}

void CMainFrame::OnUpdateViewDlgToolBar(CCmdUI *pCmdUI)
{
    CControlBar* pBar = GetControlBar(IDD_MAINFRAME);
    if (pBar != NULL)
    {
        pCmdUI->SetCheck(pBar->IsVisible());
        pCmdUI->Enable();
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}

void CMainFrame::OnViewFormatToolBar()
{
    OnBarCheck(IDD_FORMAT_VIEW);
}

void CMainFrame::OnUpdateViewFormatToolBar(CCmdUI *pCmdUI)
{
    CControlBar* pBar = GetControlBar(IDD_FORMAT_VIEW);
    if (pBar != NULL)
    {
        pCmdUI->SetCheck(pBar->IsVisible());
        pCmdUI->Enable();
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}

void CMainFrame::OnViewSearchToolBar()
{
    OnBarCheck(IDD_SEARCH_DLG);
}

void CMainFrame::OnUpdateViewSearchToolBar(CCmdUI *pCmdUI)
{
    CControlBar* pBar = GetControlBar(IDD_SEARCH_DLG);
    if (pBar != NULL)
    {
        pCmdUI->SetCheck(pBar->IsVisible());
        pCmdUI->Enable();
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}

void CMainFrame::UpdateFormatFiles(void)
{
    WIN32_FIND_DATA FD;
    CString s = theApp.mFormatFilesDir + _T("*.fmt");
    HANDLE hFind = FindFirstFile(s, &FD);
    
    m_wndDlgBar.SendDlgItemMessage(IDC_COMBO_FORMATFILES, CB_RESETCONTENT, 0, 0);
    while (hFind != INVALID_HANDLE_VALUE)
    {
        m_wndDlgBar.SendDlgItemMessage(IDC_COMBO_FORMATFILES, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR)FD.cFileName);
        if (!FindNextFile(hFind, &FD))
        {
            FindClose(hFind);
            hFind = INVALID_HANDLE_VALUE;
        }
    }
    s.LoadString(IDS_FORMAT_ALL);
    m_wndDlgBar.SendDlgItemMessage(IDC_COMBO_FORMATFILES, CB_INSERTSTRING, 0, (LPARAM) (LPCTSTR)s);
    m_wndDlgBar.SendDlgItemMessage(IDC_COMBO_FORMATFILES, CB_SETCURSEL, 0, 0);
}

void CMainFrame::OnUpdateFormatDifferentEndianess(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();
    pCmdUI->SetRadio(theApp.isDifferentEndian() == TRUE);
}

void CMainFrame::OnUpdateFormatSameEndianess(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();
    pCmdUI->SetRadio(theApp.isDifferentEndian() == FALSE);
}

void CMainFrame::OnUpdateFormatConvertDlg(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();
}

void CMainFrame::OnEditFind()
{
    GetMainPane()->OnEditFind();
}

void CMainFrame::OnFormatDifferentEndianess()
{
    theApp.setDifferentEndianess(TRUE);
    CAnyFileViewerView*pView = GetMainPane();
    CHint hint(pView->GetDataPosition());
    pView->GetDocument()->UpdateAllViews(NULL, UPDATE_DATA_POS, &hint);
    if (m_FormatConvertDlg.m_hWnd)
    {
      m_FormatConvertDlg.UpdateAll();
    }
}

void CMainFrame::OnFormatSameEndianess()
{
    theApp.setDifferentEndianess(FALSE);
    CAnyFileViewerView*pView = GetMainPane();
    CHint hint(pView->GetDataPosition());
    pView->GetDocument()->UpdateAllViews(NULL, UPDATE_DATA_POS, &hint);
    if (m_FormatConvertDlg.m_hWnd)
    {
      m_FormatConvertDlg.UpdateAll();
    }
}

LRESULT CMainFrame::OnFind(WPARAM /*wParam*/, LPARAM lParam)
{
    LPFINDREPLACE lpfr = (LPFINDREPLACE)lParam;
    GetMainPane()->OnFind(lpfr);
    return 0;
}

void CMainFrame::OnFormatConvertDlg()
{
    if (m_FormatConvertDlg.m_hWnd)
    {
        m_FormatConvertDlg.ShowWindow(SW_SHOW);
    }
    else
    {
        m_FormatConvertDlg.Create(CFormatConvertDlg::IDD);
    }
}

void CMainFrame::OnFormatLoad()
{
    CString fStructure;
    fStructure.LoadString(IDS_STRUCTURE_FILE_DLG);
    CString fFormat;
    fFormat.LoadString(IDS_FORMAT_FILE_DLG);
    BOOL fStructureActive = GetActiveView()->IsKindOf(RUNTIME_CLASS(CAnyFileViewerTreeView));
    CFileDialog dlg(TRUE, fStructureActive ? _T("xml") : _T("fmt"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, fStructureActive ? fStructure+fFormat : fFormat+fStructure, this);
    dlg.m_ofn.lpstrInitialDir = theApp.mFormatFilesDir;

    if (dlg.DoModal() == IDOK)
    {
        CString fPathName = dlg.GetPathName();
        fPathName.MakeLower();
        if (fPathName.Find(_T("xml"), 0) != -1)
        {
            GetTreeViewPane()->LoadFormatFile(dlg.GetPathName());
        }
        else
        {
            GetFormatPane()->LoadFormatFile(dlg.GetPathName());
        }
    }
}


void CMainFrame::OnUpdateFormatLoad(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();
}


void CMainFrame::OnTreeviewFileContent()
{
    GetTreeViewPane()->OnTreeviewFileContent();
}


void CMainFrame::OnTreeviewFileStructure()
{
    GetTreeViewPane()->OnTreeviewFileStructure();
}


void CMainFrame::OnUpdateTreeviewFileStructure(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();
    pCmdUI->SetRadio(GetTreeViewPane()->getViewType() == CAnyFileViewerTreeView::showFileStructure);
}


void CMainFrame::OnUpdateTreeviewFileContent(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();
    pCmdUI->SetRadio(GetTreeViewPane()->getViewType() == CAnyFileViewerTreeView::showFileContent);
}


void CMainFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // TODO: Fügen Sie hier Ihren Meldungsbehandlungscode ein, und/oder benutzen Sie den Standard.

    CFrameWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}
