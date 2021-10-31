// MainFrm.cpp : Implementierung der Klasse CMainFrame
//

#include "stdafx.h"
#include "CARA2DV.h"
#include "CARA2DVDoc.h"
#include "CARA2DVView.h"
#include "MainFrm.h"
#include "..\CaraMenu.h"
#include "..\Resource.h"

#ifdef ETSDEBUG_REPORT
 #define  ETSDEBUG_INCLUDE 
#endif
#include "CEtsDebug.h"
#include ".\mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CCaraFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CCaraFrameWnd)
    //{{AFX_MSG_MAP(CMainFrame)
    ON_WM_CREATE()
    ON_COMMAND(ID_VIEW_PLOTTOOLBAR, OnViewPlottoolbar)
    ON_UPDATE_COMMAND_UI(ID_VIEW_PLOTTOOLBAR, OnUpdateViewPlottoolbar)
    ON_WM_DESTROY()
    ON_WM_SYSCOLORCHANGE()
    //}}AFX_MSG_MAP
    ON_MESSAGE(WM_SETMESSAGESTRING, OnSetMessageString)
    ON_MESSAGE(WM_NEWCURVE, OnNewCurve)
    ON_MESSAGE(WM_ENDTHREAD, OnEndThread)
    ON_MESSAGE(WM_CLOSE, OnClose)
    ON_MESSAGE(WM_USER_OPT_UPDATE, OnUserOptUpdate)
    // Globale Hilfebefehle
    ON_COMMAND(ID_HELP_SHORTCUTS, OnHelpShortcuts)
    ON_UPDATE_COMMAND_UI(ID_HELP_SHORTCUTS, OnUpdateHelpShortcuts)
END_MESSAGE_MAP()

//    ON_WM_CLOSE()
/////////////////////////////////////////////////////////////////////////////
// CMainFrame Konstruktion/Destruktion
UINT CMainFrame::gm_nMainIndicators[NO_OF_MAIN_INDICATORS] =
{
   ID_SEPARATOR,           // Statusleistenanzeige
   ID_SEPARATOR,           // 2D-Zusatzinfoanzeige 1
   ID_SEPARATOR,           // 2D-Zusatzinfoanzeige 2
   ID_SEPARATOR            // 2D-Zusatzinfoanzeige 3
};

// long CMainFrame::gm_lOldStatusbarFc = 0;

CMainFrame::CMainFrame()
{
   m_pThread     = NULL;
   m_pLabel      = NULL;
   m_nPrecision  = 0;
   m_bSigned     = true;
}

CMainFrame::~CMainFrame()
{
   OnEndThread(0,0);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
   cs.style = WS_OVERLAPPED |WS_CAPTION|FWS_ADDTOTITLE|WS_THICKFRAME|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_MAXIMIZE;
   return CCaraFrameWnd::PreCreateWindow(cs);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   BEGIN("CMainFrame::OnCreate");
   CString strToolbartext;
   if (CCaraFrameWnd::OnCreate(lpCreateStruct) == -1)
   return -1;

   ModifyStyle(FWS_PREFIXTITLE, 0);
   CCARA2DVApp*pApp = (CCARA2DVApp*)AfxGetApp();
   if (pApp->m_CmdInfo.m_bRunEmbedded)                         // Eingebettet
   {                                                           // Keine Toolbars, Statusbars bzw Menüs
      SetMenu(NULL);                                           // im MainFrame
      return 0;
   }
    
   if (!m_wndToolBar.Create(this) || !m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
   {
      REPORT("Failed to create toolbar\n");
      return -1;      // Fehler beim Erzeugen
   }
   strToolbartext.LoadString(IDS_TOOLBAR);
   m_wndToolBar.SetWindowText(LPCTSTR(strToolbartext));
/*
   !!! MFC_Erweiterung !!!
   Zweck :  Speichern der Positionen und Zustände aller Toolbars in der Registry
            mit den Funktionen LoadBarState() und SaveBarState().
   Für jede weitere Toolbar wird eine zusätzliche ID vergeben:
   Die Main Toolbars haben folgendes (AFXRES.H)
      #define AFX_IDW_TOOLBAR                 0xE800  // main Toolbar for window
      #define AFX_IDW_STATUS_BAR              0xE801  // Status bar window
      #define AFX_IDW_PREVIEW_BAR             0xE802  // PrintPreview Dialog Bar
      #define AFX_IDW_RESIZE_BAR              0xE803  // OLE in-place resize bar
      #define AFX_IDW_REBAR                   0xE804  // COMCTL32 "rebar" Bar
      #define AFX_IDW_DIALOGBAR               0xE805  // CDialogBar
   dahinter ist nichts - hoffe ich - und die User Toolbars fangen dann
   bei 0xE806 an.
   Diese Main-ID wird in Create erfragt und fügt den Style "CBRS_HIDE_INPLACE" hinzu.
   Die übrigen Styles in Create sind WS_CHILD|WS_VISIBLE|CBRS_TOP.
   (Rolf Ehlers)
*/
   int nIDUsertoolbar = 0x06;
   if (!m_wndPlotToolBar.Create(this, WS_CHILD|WS_VISIBLE|CBRS_RIGHT) || !m_wndPlotToolBar.LoadToolBar(IDR_PLOTTOOLBAR))
   {
      REPORT("Failed to create toolbar\n");
      return -1;      // Fehler beim Erzeugen
   }
   ::SetWindowLong(m_wndPlotToolBar.m_hWnd, GWL_ID, AFX_IDW_TOOLBAR+nIDUsertoolbar);

   nIDUsertoolbar++;
   strToolbartext.LoadString(IDS_PLOTTOOLBAR);
   m_wndPlotToolBar.SetWindowText(LPCTSTR(strToolbartext));

   m_wndPlotToolBar.GetToolBarCtrl().ModifyStyle(0,TBSTYLE_FLAT);
   m_wndToolBar.GetToolBarCtrl().ModifyStyle(0,TBSTYLE_FLAT);

//   if (!m_wndStatusBar.Create(this, WS_VISIBLE|WS_CHILD|CBRS_BOTTOM|CBRS_FLYBY|SBT_TOOLTIPS)) //||
   if (!m_wndStatusBar.Create(this) )//||
//       !SetMainPaneInfo())
   {
      REPORT("Failed to create status bar\n");
      return -1;      // Fehler beim Erzeugen
   }
   m_wndStatusBar.m_dwStyle |= CBRS_FLYBY;
   EnableDocking(CBRS_ALIGN_ANY);

   m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle()|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC|CBRS_GRIPPER);
   m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
   DockControlBar(&m_wndToolBar);

   m_wndPlotToolBar.SetBarStyle(m_wndPlotToolBar.GetBarStyle()|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC|CBRS_GRIPPER);
   m_wndPlotToolBar.EnableDocking(CBRS_ALIGN_ANY);
   DockControlBar(&m_wndPlotToolBar);

   if (!m_pMainMenu)
      m_pMainMenu = new CCaraMenu;

   m_nIDMenuResource = IDR_MAINFRAME;
   m_wndPlotToolBar.SetRepeatTime(300);
   m_wndPlotToolBar.ModifyFlags(TOOLBAR_REPEAT_SOME, 0xffffffff);
   m_wndPlotToolBar.SetButtonTimer(ID_FUNC_EDITSTEPPLUS, true);
   m_wndPlotToolBar.SetButtonTimer(ID_FUNC_EDITSTEPMINUS, true);

   if (m_pMainMenu)
   {
      m_pMainMenu->AddToolBar(&m_wndToolBar);
      m_pMainMenu->AddToolBar(&m_wndPlotToolBar);
      SetMenuBitmaps();
   }

   return 0;
}

bool CMainFrame::SetMainPaneInfo()
{
   bool bResult = (m_wndStatusBar.SetIndicators(gm_nMainIndicators, NO_OF_MAIN_INDICATORS)!=0) ? true : false;
   if (bResult)
   {
      CString str;
      m_wndStatusBar.EnableToolTips();
      m_wndStatusBar.SetPaneInfo(0,0,SBPS_STRETCH|SBPS_NOBORDERS,0);
      if (str.LoadString(IDS_STATUS_PANE0)) m_wndStatusBar.GetStatusBarCtrl().SetTipText(0, str);
      m_wndStatusBar.SetPaneInfo(1,1,0,140);
      if (str.LoadString(IDS_STATUS_PANE1)) m_wndStatusBar.GetStatusBarCtrl().SetTipText(1, str);
      m_wndStatusBar.SetPaneInfo(2,2,0, 90);
      if (str.LoadString(IDS_STATUS_PANE2)) m_wndStatusBar.GetStatusBarCtrl().SetTipText(2, str);
      m_wndStatusBar.SetPaneInfo(3,3,0, 90);
      if (str.LoadString(IDS_STATUS_PANE3)) m_wndStatusBar.GetStatusBarCtrl().SetTipText(3, str);
      m_nActualIndicatorSize = NO_OF_MAIN_INDICATORS;
   }
   else ASSERT(false);
   return bResult;
}
 
void CMainFrame::OnDestroy() 
{
   BEGIN("OnDestroy");
   CCARA2DVDoc *pDoc = (CCARA2DVDoc *)GetActiveDocument();
   if (pDoc)
   {
//      HMENU hMenu = pDoc->GetPlotMenu();
//      if (hMenu) ::DestroyMenu(hMenu);
      CCaraMenu *pMenu = (CCaraMenu*) pDoc->GetPlotMenu();
      if (pMenu) pMenu->DestroyMenu();
   }
   CCaraFrameWnd::OnDestroy();
}

/////////////////////////////////////////////////////////////////////////////
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

/////////////////////////////////////////////////////////////////////////////
// CMainFrame Nachrichten-Handler
void CMainFrame::OnSysColorChange() 
{
   CCaraFrameWnd::OnSysColorChange();
}

LRESULT CMainFrame::OnNewCurve(WPARAM wParam, LPARAM lParam)
{
   if (wParam)
   {
      CPlotLabel  *pActual = NULL;
      CCurveLabel *pcl     = (CCurveLabel*) wParam;
#ifdef ETS_OLE_SERVER
      COleServerDoc *pDoc  = (COleServerDoc*) GetActiveDocument();
#else
      CDocument     *pDoc  = GetActiveDocument();
#endif
      CView       *pView   = GetActiveView();
      if (pView && pView->IsKindOf(RUNTIME_CLASS(CCARA2DVView)))
      {
         pActual = ((CCARA2DVView*)pView)->GetPickedPlot();
      }
      ASSERT_VALID(pDoc);
      if (pDoc->IsKindOf(RUNTIME_CLASS(CCARA2DVDoc)))
      {
         CCARA2DVDoc*pCaraDoc = (CCARA2DVDoc*) pDoc;
         bool bTargetFunction = (pcl->GetTargetFuncNo() != 0) ? true : false;
         CPlotLabel *ppl=pCaraDoc->InsertCurve(pcl, pActual);
         if (ppl)
         {
            CCARA2DVApp* pApp = (CCARA2DVApp*)AfxGetApp();
            if (lParam == 1)
            {
               RANGE r = ppl->DeterminRange();
               ppl->SetDefaultRange(&r);
            }
            pDoc->UpdateAllViews(NULL, UPDATE_INSERT_LABEL_INTO, ppl);
            if (!bTargetFunction)
               pApp->SetUndoCmd(ID_EDIT_PASTE, pcl, NULL, -1, ppl);
         }
         else delete pcl;
      }
   }
   return 0;
}

LRESULT CMainFrame::OnUserOptUpdate(WPARAM wParam, LPARAM lParam)
{
   CDocument     *pDoc  = GetActiveDocument();
   if (pDoc->IsKindOf(RUNTIME_CLASS(CCARA2DVDoc)))
   {
      CCARA2DVDoc*pCaraDoc = (CCARA2DVDoc*) pDoc;
      return pCaraDoc->OnUserOptUpdate(wParam, lParam);
   }
   return 0;
}

LRESULT CMainFrame::OnEndThread(WPARAM wParam, LPARAM lParam)
{
   if (m_pThread)
   {
      m_pThread = NULL;
   }

   if (wParam)
   {
      CString *pStr = (CString*)wParam;
      AfxMessageBox(*pStr, MB_OK|MB_ICONEXCLAMATION);
      delete pStr;
   }

   CDocument *pDoc = GetActiveDocument();
   if (pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CCARA2DVDoc))) ((CCARA2DVDoc*)pDoc)->ReleaseThreadFile();

   if (m_pLabel)
   {
      delete m_pLabel;
      m_pLabel  = NULL;
   }
   return 0;
}

// Control Bars
void CMainFrame::OnViewPlottoolbar()
{
   if (m_wndPlotToolBar.IsWindowVisible())
   {
      ShowControlBar(&m_wndPlotToolBar, false, false);
      m_wndPlotToolBar.ModifyFlags(0, TOOLBAR_USERCMD_VISIBLE);
   }
   else
   {
      ShowControlBar(&m_wndPlotToolBar, true, false);
      m_wndPlotToolBar.ModifyFlags(0, TOOLBAR_USERCMD_VISIBLE);
   }
}

void CMainFrame::OnUpdateViewPlottoolbar(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(m_wndPlotToolBar.IsWindowVisible());
}

LRESULT CMainFrame::OnClose(WPARAM wParam, LPARAM lParam)
{
   BEGIN("CMainFrame::OnClose");
   CCARA2DVApp* pApp = (CCARA2DVApp*) AfxGetApp();
   CCARA2DVDoc* pdoc = (CCARA2DVDoc*) GetActiveDocument();

   if (wParam  == TARGET_FUNCTION_FIXED)     // Abbruch durch Cancel
   {
      pdoc->SetModifiedFlag(false);
   }

   if (pApp->m_bEditMode && pdoc->IsModified() && pApp->m_CallingWindowwParam == TARGET_FUNCTION_CHANGEABLE)
   {
      if (AfxMessageBox(IDS_SAVE_CHANGES, MB_YESNO|MB_ICONQUESTION)==IDYES)
      {
         pdoc->SaveDocument(pApp->m_strEditPathName);
      }
      else
      {
         pdoc->SetModifiedFlag(false);
         pApp->m_CallingWindowwParam &= ~0x00010000; // HIWORD(wParam) 1 löschen
      }
   }
/*
   if (pdoc->IsModified()) REPORT("Modified");
   if (pdoc->m_bEmbedded)  REPORT("Embedded");
*/
   CCaraFrameWnd::OnClose();
   return 0;
}

void CMainFrame::ActivateFrame(int nCmdShow) 
{
   BEGIN("CMainFrame::ActivateFrame");
   CCaraFrameWnd::ActivateFrame(nCmdShow);
   CCARA2DVDoc *pDoc = (CCARA2DVDoc *)GetActiveDocument();
   if (pDoc) pDoc->StartReadWaveThread();
}

LRESULT CMainFrame::OnSetMessageString(WPARAM wParam, LPARAM lParam)
{
   if ((ID_VIEW_PLOT <= wParam) && (wParam <= ID_VIEW_LAST_PLOT))
   {
      CCARA2DVDoc *pDoc = (CCARA2DVDoc *)GetActiveDocument();
      if (pDoc)
      {
         ProcessLabel pl = {NULL, wParam-ID_VIEW_PLOT, NULL, NULL};
         pDoc->GetLabelContainer()->ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), &pl, CLabelContainer::GetLabelFromIndex);
         if (pl.pl)
         {
            lParam = (LPARAM)((CPlotLabel*)pl.pl)->GetHeading();
            if (lParam) wParam = 0;
         }
      }
      if (wParam) wParam = ID_VIEW_PLOT;
   }
   TRACE("Msg: (%x, %d), %x\n", wParam, wParam, lParam);
   return CCaraFrameWnd::OnSetMessageString(wParam, lParam);
}


BOOL CMainFrame::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult) 
{
    // TODO: Speziellen Code hier einfügen und/oder Basisklasse aufrufen
    
    return CCaraFrameWnd::OnChildNotify(message, wParam, lParam, pLResult);
}

BOOL CMainFrame::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
    // TODO: Speziellen Code hier einfügen und/oder Basisklasse aufrufen
    NMHDR *pNotify = (NMHDR*) lParam;
   if (pNotify->code == TTN_NEEDTEXT)
   {
      wParam = wParam;
   }
    return CCaraFrameWnd::OnNotify(wParam, lParam, pResult);
}

void CMainFrame::OnHelpShortcuts()
{ 
   ShowShortCuts(m_hAccelTable);
}

void CMainFrame::OnUpdateHelpShortcuts(CCmdUI *pCmdUI)
{
   pCmdUI->Enable();
}
