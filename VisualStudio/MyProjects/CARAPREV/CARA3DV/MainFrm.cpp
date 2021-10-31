// MainFrm.cpp : Implementierung der Klasse CMainFrame
//

#include "stdafx.h"
#include "CARA3DV.h"
#include "MainFrm.h"
#include "CARA3DVView.h"
#include "CARA3DVDoc.h"
#include "ViewParams.h"
#include "StandardDlg.h"
#include "Plangraph.h"
#include "..\CaraMenu.h"
#include "..\resource.h"
#include "ETS3DGLRegKeys.h"

#ifdef ETSDEBUG_REPORT
 #define  ETSDEBUG_INCLUDE 
#endif
#include "CEtsDebug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define TOOLBARFLAGS     "ToolBarFlags"
#define MAINBARVISIBLE   0x00000001
#define MAINBARFLOATING  0x00000002
#define FUNCBARVISIBLE   0x00000010
#define FUNCBARFLOATING  0x00000020
#define FILMBARVISIBLE   0x00000100
#define FILMBARFLOATING  0x00000200
#define COLORBARVISIBLE  0x00001000
#define COLORBARFLOATING 0x00002000

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CCaraFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CCaraFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
   ON_WM_CREATE()
   ON_COMMAND(ID_VIEW_FUNCTIONS, OnViewFunctions)
   ON_UPDATE_COMMAND_UI(ID_VIEW_FUNCTIONS, OnUpdateViewFunctions)
   ON_COMMAND(ID_VIEW_FILMRUN, OnViewFilmrun)
   ON_UPDATE_COMMAND_UI(ID_VIEW_FILMRUN, OnUpdateViewFilmrun)
	ON_COMMAND(ID_VIEW_COLORBAR, OnViewInfobar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_COLORBAR, OnUpdateViewInfobar)
	ON_WM_DESTROY()
   ON_MESSAGE(WM_SETMESSAGESTRING, OnSetMessageString)
   ON_WM_SYSCOLORCHANGE()
   ON_WM_SYSCOMMAND()
   ON_MESSAGE(WM_ENTERSIZEMOVE, OnEnterSizeMove)
   ON_MESSAGE(WM_EXITSIZEMOVE , OnExitSizeMove)
   ON_MESSAGE(WM_APPLY_TO_VIEW, OnApplyToView)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
   ON_MESSAGE(WM_EXITMENULOOP, OnExitMenuLoop)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame Nachrichten-Handler
UINT CMainFrame::gm_nMainIndicators[NO_OF_MAIN_INDICATORS] =
{
   ID_SEPARATOR,           // Statusleistenanzeige
   ID_SEPARATOR            // 3D-Zusatzinfoanzeige
};

CMainFrame::CMainFrame()
{
   BEGIN("CMainFrame");
}

CMainFrame::~CMainFrame()
{
   REPORT("~CMainFrame)");
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   BEGIN("CMainFrame::OnCreate");
   if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
   {
      return -1;
   }

#ifdef ETS_OLE_SERVER
   CCARA3DVApp*pApp = (CCARA3DVApp*) AfxGetApp();
   if (pApp->m_CmdInfo.m_bRunEmbedded)                         // Eingebettet werden die Mainframe Toolbars
   {                                                           // und Menus nicht benötigt.
      SetMenu(NULL);
      return 0;
   }
#endif

   if (!m_wndToolBar.Create(this) || !m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
   {
      return -1;      // Fehler beim Erzeugen
   }
/*
   !!! MFC_Erweiterung !!!
   Zweck :  Speichern der Positionen un Zustände aller Toolbars in der Registry
            mit den Funktionen LoadBarState() und SaveBarState().
   Für jede weitere Toolbar wird eine zusätzliche ID vergeben:
   Die Main Toolbar hat folgendes
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
   if (!m_wndFuncToolBar.Create(this, WS_CHILD|CBRS_LEFT)||!m_wndFuncToolBar.LoadToolBar(IDR_FUNCTOOLBAR))
   {
      return -1;      // Fehler beim Erzeugen
   }
   int nIDUsertoolbar = 0x06;
   ::SetWindowLong(m_wndFuncToolBar.m_hWnd, GWL_ID, AFX_IDW_TOOLBAR+nIDUsertoolbar);

   if (!m_wndFilmToolBar.Create(this,WS_CHILD|CBRS_RIGHT)||!m_wndFilmToolBar.LoadToolBar(IDR_FILMTOOLBAR))
   {
      return -1;      // Fehler beim Erzeugen
   }
   nIDUsertoolbar++;
   ::SetWindowLong(m_wndFilmToolBar.m_hWnd, GWL_ID, AFX_IDW_TOOLBAR+nIDUsertoolbar);

   if (!m_wndColorBar.Create(this, WS_CHILD|CBRS_RIGHT))
   {
      return -1;      // Fehler beim Erzeugen
   }
   nIDUsertoolbar++;
   ::SetWindowLong(m_wndColorBar.m_hWnd, GWL_ID, AFX_IDW_TOOLBAR+nIDUsertoolbar);

   m_wndToolBar.GetToolBarCtrl(    ).ModifyStyle(0,TBSTYLE_FLAT); // ToolBar
   m_wndFilmToolBar.GetToolBarCtrl().ModifyStyle(0,TBSTYLE_FLAT); // FilmToolBar
   m_wndFuncToolBar.GetToolBarCtrl().ModifyStyle(0,TBSTYLE_FLAT); // FuncToolBar
   CRect rcBorders = m_wndToolBar.GetBorders();                   
   m_wndColorBar.SetBorders(rcBorders.left-1, rcBorders.top, rcBorders.right-1, rcBorders.bottom);
   m_wndColorBar.ModifyStyle(0,TBSTYLE_FLAT);                     // ColorBar

   CSize szButton = m_wndToolBar.GetButtonSize();
   szButton.cy   += 4;
   m_wndColorBar.SetButtonSize(szButton);     

   if (!m_wndStatusBar.Create(this) || !SetMainPaneInfo())
   {
      return -1;      // Fehler beim Erzeugen
   }
   m_wndStatusBar.m_dwStyle |= CBRS_FLYBY;

   m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle()|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC|CBRS_GRIPPER);
   m_wndFuncToolBar.SetBarStyle(m_wndFuncToolBar.GetBarStyle()|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC|CBRS_GRIPPER);
   m_wndFilmToolBar.SetBarStyle(m_wndFilmToolBar.GetBarStyle()|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC|CBRS_GRIPPER);
   m_wndColorBar.SetBarStyle(m_wndColorBar.GetBarStyle()|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC|CBRS_GRIPPER);
   EnableTBDocking(CBRS_ALIGN_ANY);

   EnableDocking(CBRS_ALIGN_ANY);

   DockControlBar(&m_wndToolBar);
   DockControlBar(&m_wndFuncToolBar);
   DockControlBar(&m_wndFilmToolBar);
   DockControlBar(&m_wndColorBar);

   if (!m_pMainMenu)
      m_pMainMenu = new CCaraMenu;

   m_pMainMenu->AddToolBar(&m_wndToolBar);
   m_pMainMenu->AddToolBar(&m_wndFuncToolBar);
   m_pMainMenu->AddToolBar(&m_wndFilmToolBar);

   m_nIDMenuResource = IDR_MAINFRAME;

   SetMenuBitmaps();

   m_wndFuncToolBar.SetRepeatTime(300);
   m_wndFuncToolBar.ModifyFlags(TOOLBAR_REPEAT_ALL, 0xffffffff);
   m_wndFilmToolBar.SetRepeatTime(300);
   m_wndFilmToolBar.ModifyFlags(TOOLBAR_REPEAT_SOME, 0xffffffff);
   m_wndFilmToolBar.SetButtonTimer(ID_FUNC_FILMSTEPPLUS, true);
   m_wndFilmToolBar.SetButtonTimer(ID_FUNC_FILMSTEPMINUS, true);

   CString string;
   string.LoadString(IDS_MAIN_TOOLBAR_NAME);
   m_wndToolBar.SetWindowText(string);
   string.LoadString(IDS_FUNC_TOOLBAR_NAME);
   m_wndFuncToolBar.SetWindowText(string);
   string.LoadString(IDS_FILM_TOOLBAR_NAME);
   m_wndFilmToolBar.SetWindowText(string);
   string.LoadString(IDS_COLORBAR_NAME);
   m_wndColorBar.SetWindowText(string);

   DWORD dwTBFlags = AfxGetApp()->GetProfileInt(REGKEY_DEFAULT, TOOLBARFLAGS, MAINBARVISIBLE|FUNCBARVISIBLE|FILMBARFLOATING|FILMBARVISIBLE|COLORBARVISIBLE);
   if (dwTBFlags)
   {
      if (dwTBFlags & MAINBARVISIBLE) m_wndToolBar.ModifyFlags(TOOLBAR_USERCMD_VISIBLE, 0);
      else                            m_wndToolBar.ModifyFlags(0, TOOLBAR_USERCMD_VISIBLE);
      if (dwTBFlags & FUNCBARVISIBLE) m_wndFuncToolBar.ModifyFlags(TOOLBAR_USERCMD_VISIBLE, 0);
      else                            m_wndFuncToolBar.ModifyFlags(0, TOOLBAR_USERCMD_VISIBLE);
      if (dwTBFlags & FILMBARVISIBLE) m_wndFilmToolBar.ModifyFlags(TOOLBAR_USERCMD_VISIBLE, 0);
      else                            m_wndFilmToolBar.ModifyFlags(0, TOOLBAR_USERCMD_VISIBLE);
      if (dwTBFlags & FILMBARFLOATING)m_wndFilmToolBar.ModifyFlags(TOOLBAR_USERCMD_FLOATING, 0);
      else                            m_wndFilmToolBar.ModifyFlags(0, TOOLBAR_USERCMD_FLOATING);

      m_wndColorBar.VisiblebyUser((dwTBFlags & COLORBARVISIBLE)? true: false);
//      m_wndColorBar.SetFloating((dwTBFlags&COLORBARFLOATING) ? true : false);
   }

   return 0;
}

bool CMainFrame::SetMainPaneInfo()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CMainFrame::SetMainPaneInfo");
#endif
   bool bResult = (m_wndStatusBar.SetIndicators(gm_nMainIndicators, NO_OF_MAIN_INDICATORS)!=0) ? true : false;
   if (bResult)
   {
      CString str;
      m_wndStatusBar.EnableToolTips();
      m_wndStatusBar.SetPaneInfo(0,0,SBPS_STRETCH|SBPS_NOBORDERS,0);
      if (str.LoadString(IDS_STATUS_PANE0)) m_wndStatusBar.GetStatusBarCtrl().SetTipText(0, str);
      m_wndStatusBar.SetPaneInfo(1,1,0, 200);
      if (str.LoadString(IDS_STATUS_PANE1)) m_wndStatusBar.GetStatusBarCtrl().SetTipText(1, str);
      m_nActualIndicatorSize = NO_OF_MAIN_INDICATORS;
   }
   else ASSERT(false);
   return bResult;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame Konstruktion/Destruktion

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
// CMainFrame Diagnose
void CMainFrame::OnViewFunctions() 
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CMainFrame::OnViewFunctions");
#endif
   if (m_wndFuncToolBar.IsWindowVisible())
   {
      ShowControlBar(&m_wndFuncToolBar, false, false);
   }
   else
   {
      ShowControlBar(&m_wndFuncToolBar, true, false);
   }
}
void CMainFrame::OnViewFilmrun() 
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CMainFrame::OnViewFilmrun");
#endif
   if (m_wndFilmToolBar.IsWindowVisible())
   {
      ShowControlBar(&m_wndFilmToolBar, false, false);
   }
   else
   {
      ShowControlBar(&m_wndFilmToolBar, true, false);
   }
}

void CMainFrame::OnViewInfobar() 
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CMainFrame::OnViewInfobar");
#endif
   if (m_wndColorBar.IsWindowVisible())
   {
      ShowControlBar(&m_wndColorBar, false, false);
      m_wndColorBar.VisiblebyUser(false);
   }
   else
   {
      ShowControlBar(&m_wndColorBar, true, false);
      m_wndColorBar.VisiblebyUser(true);
   }
}

void CMainFrame::OnUpdateViewFunctions(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(m_wndFuncToolBar.IsWindowVisible());
}
void CMainFrame::OnUpdateViewFilmrun(CCmdUI* pCmdUI)   {pCmdUI->SetCheck(m_wndFilmToolBar.IsWindowVisible());}
void CMainFrame::OnUpdateViewInfobar(CCmdUI* pCmdUI)   {pCmdUI->SetCheck(m_wndColorBar.IsWindowVisible());}

void CMainFrame::EnableTBDocking(DWORD dwStyle)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CMainFrame::EnableTBDocking");
#endif
   m_wndColorBar.EnableDocking(dwStyle);
   m_wndFilmToolBar.EnableDocking(dwStyle);
   m_wndFuncToolBar.EnableDocking(dwStyle);
   m_wndToolBar.EnableDocking(dwStyle);
}

LRESULT CMainFrame::OnEnterSizeMove(WPARAM wParam, LPARAM lParam)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CMainFrame::OnEnterSizeMove");
#endif
   CView *pView = GetActiveView();
   if (pView && pView->IsKindOf(RUNTIME_CLASS(CCARA3DVView)))
   {
      ((CCARA3DVView*)pView)->SendMsgToGLView(WM_ENTERSIZEMOVE, wParam, lParam);
   }
   return 0;
}

LRESULT CMainFrame::OnExitSizeMove(WPARAM wParam, LPARAM lParam)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CMainFrame::OnExitSizeMove");
#endif
   CView *pView = GetActiveView();
   if (pView && pView->IsKindOf(RUNTIME_CLASS(CCARA3DVView)))
   {
      ((CCARA3DVView*)pView)->SendMsgToGLView(WM_EXITSIZEMOVE, wParam, lParam);
   }
   return 0;
}

LRESULT CMainFrame::OnApplyToView(WPARAM wParam, LPARAM lParam)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CMainFrame::OnApplyToView");
#endif
   CView *pView = GetActiveView();
   LRESULT lResult = 0;
   if (pView && pView->IsKindOf(RUNTIME_CLASS(CCARA3DVView)))
   {
      lResult = ((CCARA3DVView*)pView)->OnApplyToView(wParam, lParam);
      if (lResult == IDD_VIEW_DIALOG)
      {
         CNonModalDlg *pDlg = (CNonModalDlg*)lParam;
         if (pDlg->m_dwChanges & VIEWPARAMS_MINMAXLEVEL)
         {
            CDocument *pDoc = GetActiveDocument();
            if (pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CCARA3DVDoc)))
            {
               m_wndColorBar.SetMinMaxLevel(((CCARA3DVDoc*)pDoc)->m_LevelColor.GetMinLevel(), ((CCARA3DVDoc*)pDoc)->m_LevelColor.GetMaxLevel());
            }
         }
      }
      if (lResult == IDD_STANDARD)
      {
         CNonModalDlg *pDlg = (CNonModalDlg*)lParam;
         if (pDlg->m_dwChanges & STDDLG_GRIDROUND)
         {
            CDocument *pDoc = GetActiveDocument();
            if (pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CCARA3DVDoc)))
            {
               CDocument *pDoc = GetActiveDocument();
               CCARA3DVDoc* p3DDoc = (CCARA3DVDoc*) pDoc;
               m_wndColorBar.SetNumRepresentation(p3DDoc->m_Grid.GetNumModeLevel(), p3DDoc->m_Grid.GetRoundingLevel());
            }
         }
      }
   }
   return lResult;
}

void CMainFrame::AttachColorArray(CColorArray *pCA)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CMainFrame::AttachColorArray");
#endif
   m_wndColorBar.AttachColorArray(pCA);
}

void CMainFrame::OnClose() 
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CMainFrame::OnClose");
#endif
   DWORD dwTBFlags = 0;
   if (m_wndToolBar.IsVisiblebyUserCmd())     dwTBFlags |= MAINBARVISIBLE;
   if (m_wndToolBar.IsFloatbyUserCmd())       dwTBFlags |= MAINBARFLOATING;
   if (m_wndFuncToolBar.IsVisiblebyUserCmd()) dwTBFlags |= FUNCBARVISIBLE;
   if (m_wndFuncToolBar.IsFloatbyUserCmd())   dwTBFlags |= FUNCBARFLOATING;
   if (m_wndFilmToolBar.IsVisiblebyUserCmd()) dwTBFlags |= FILMBARVISIBLE;
   if (m_wndFilmToolBar.IsFloatbyUserCmd())   dwTBFlags |= FILMBARFLOATING;
   if (m_wndColorBar.IsVisiblebyUserCmd())    dwTBFlags |= COLORBARVISIBLE;
   if (m_wndColorBar.GetFloating())           dwTBFlags |= COLORBARFLOATING;

   AfxGetApp()->WriteProfileInt(REGKEY_DEFAULT, TOOLBARFLAGS, dwTBFlags);

	CCaraFrameWnd::OnClose();
}

void CMainFrame::OnDestroy() 
{
   BEGIN("CMainFrame::OnDestroy");

   if (::IsWindow(m_wndColorBar.m_hWnd))
      m_wndColorBar.DetachColorArray();

   CCaraFrameWnd::OnDestroy();
}

LRESULT CMainFrame::OnSetMessageString(WPARAM wParam, LPARAM lParam)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CMainFrame::OnSetMessageString");
#endif
   if (wParam == AFX_IDS_IDLEMESSAGE)
   {
      lParam = (LPARAM)GetDocumentHeading();
      if (lParam) wParam = 0;
   }
   else
   {
      wParam &= ~HID_BASE_COMMAND;
   }
   return CFrameWnd::OnSetMessageString(wParam, lParam);
}

LPCTSTR CMainFrame::GetDocumentHeading()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CMainFrame::GetDocumentHeading");
#endif
   CDocument *pDoc = GetActiveDocument();
   if (pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CCARA3DVDoc)))
   {
      return (LPCTSTR) ((CCARA3DVDoc*)pDoc)->GetHeadLineText();
   }
   return NULL;
}

void CMainFrame::OnUpdateFrameContent()
{
   if (m_bFrameContentInvalid)
   {
      BEGIN("OnUpdateFrameContent");
      CDocument *pDoc = GetActiveDocument();
      CCaraToolbar::EnableUserCmds(false);
      if (pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CCARA3DVDoc)))
      {
         CCARA3DVDoc*p3DDoc = (CCARA3DVDoc*) pDoc;             // Die Farbskalenparameter einstellen
         m_wndColorBar.SetMinMaxLevel(p3DDoc->m_LevelColor.GetMinLevel(), p3DDoc->m_LevelColor.GetMaxLevel());
         m_wndColorBar.SetNumRepresentation(p3DDoc->m_Grid.GetNumModeLevel(), p3DDoc->m_Grid.GetRoundingLevel());
         m_wndColorBar.SetUnit((char*)LPCTSTR(p3DDoc->m_Grid.GetLevelUnit()));
         const char *pszText = p3DDoc->GetHeadLineText();      // Statuszeile updaten
         if (pszText) CFrameWnd::OnSetMessageString(0, (LPARAM)pszText);
         pDoc->UpdateAllViews(NULL, UPDATE_FRAME_CONTENT);     // Views updaten
         bool bShow;                                           // Filmtoolbar nur anzeigen, wenn mehrere Bilder
         bool bShowFilm = (p3DDoc->m_Grid.GetNoOfPictures() > 1) ? true : false; // vorhanden
         if (bShowFilm ^ (m_wndFilmToolBar.IsWindowVisible()!=0))// nur wenn der Zustand unterschiedlich ist,
         {
            ShowControlBar(&m_wndFilmToolBar, bShowFilm, true);// müssen die Funktionen aufgerufen werden
            bool bIsFloating       = m_wndFilmToolBar.IsFloating();
            bool bShouldBeFloating = m_wndFilmToolBar.IsFloatbyUserCmd();
            if (bShowFilm)
            {
               if (bShouldBeFloating)                          // Toolbar soll Floating sein
               {
                  if (!bIsFloating)                            // ist es aber noch nicht,
                  {
                     CRect rc;
                     GetWindowRect(&rc);                       // dann Funktion Float... ausführen
                     FloatControlBar(&m_wndFilmToolBar, rc.CenterPoint(), CBRS_ALIGN_LEFT);
                  }
               }
               else                                            // Toolbar soll gedockt sein
               {
                  if (bIsFloating)                             // ist es aber noch nicht,
                  {
                     DockControlBar(&m_wndFilmToolBar);        // dann Funktion Dock... ausführen
                  }
               }
            }
         }

         bool b3DObject = (p3DDoc->m_p3DObject != NULL) ? true : false; // ist das 3D-Objekt vorhanden ?
         bShow = b3DObject && m_wndFuncToolBar.IsVisiblebyUserCmd(); // soll die Funktions Toolbar angezeigt werden ?
         if (bShow ^ (m_wndFuncToolBar.IsWindowVisible()!=0))  // nur wenn der Zustand unterschiedlich ist,
            ShowControlBar(&m_wndFuncToolBar, bShow, true);    // muß die Funktion aufgerufen werden

         bShow = b3DObject && m_wndColorBar.IsVisiblebyUserCmd(); // soll die Farbskala angezeigt werden ?
         if (bShow ^ (m_wndColorBar.IsWindowVisible()!=0))     // nur wenn der Zustand unterschiedlich ist,
            ShowControlBar(&m_wndColorBar, bShow, true);       // muß die Funktion aufgerufen werden

         m_wndColorBar.OnUpdateCmdUI(this, true);
         m_bFrameContentInvalid = false;
      }
      CCaraToolbar::EnableUserCmds(true);
   }
}

void CMainFrame::ActivateFrame(int nCmdShow) 
{
   BEGIN("CMainFrame::ActivateFrame");
	CCaraFrameWnd::ActivateFrame(nCmdShow);
}

CLabel * CMainFrame::PasteSpecial(CPtrList *ptr)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CMainFrame::PasteSpecial");
#endif
   CPtrList Lptr;

   CString  strColorScale;
   strColorScale.LoadString(IDS_PASTECOLORSCALE);
   Lptr.AddTail((void*)LPCTSTR(strColorScale));

   CString  str3DPicture;
   str3DPicture.LoadString(IDS_PASTE3DPICTURE);
   Lptr.AddTail((void*)LPCTSTR(str3DPicture));

   CString  strPictureIndex;
   strPictureIndex.LoadString(IDS_PASTEPICTUREINDEX);
   Lptr.AddTail((void*)LPCTSTR(strPictureIndex));

   CLabel *pl = CCaraFrameWnd::PasteSpecial(&Lptr);
   if (pl) return pl;
   POSITION pos = Lptr.GetHeadPosition();
   if (pos != NULL)
   {
      int nCursel = (int) Lptr.GetAt(pos);
      CCARA3DVView * p3DView = Get3DVView();
      return p3DView->Paste3DLabel(nCursel);
   }
   return NULL;
}

CCARA3DVView * CMainFrame::Get3DVView()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CMainFrame::CMainFrame");
#endif
   CDocument *pDoc = GetActiveDocument();
   POSITION pos = pDoc->GetFirstViewPosition();
   while (pos)
   {
      CView *pView = pDoc->GetNextView(pos);
      if (pView && pView->IsKindOf(RUNTIME_CLASS(CCARA3DVView)))
      {
         return (CCARA3DVView*) pView;
      }
   }
   return NULL;
}

LRESULT CMainFrame::OnExitMenuLoop(WPARAM wParam, LPARAM lParam)
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CMainFrame::OnExitMenuLoop");
#endif
   CCARA3DVView *pView = Get3DVView();
   if (pView)
   {
      pView->UpdateMousePos();
   }
   return 0;
}

void CMainFrame::InvalidateFrameContent()
{
   m_bFrameContentInvalid = true;
}

/*
void CMainFrame::ShowChildWindows(int nCmdShow)
{
    EnumChildWindows(m_hWnd, ShowChildWindowsEnum, nCmdShow);
}

BOOL CALLBACK CMainFrame::ShowChildWindowsEnum(HWND hwnd, LPARAM lParam)
{
   ASSERT(::IsWindow(hwnd));
   ::ShowWindow(hwnd, (int) lParam);
   return true;
}
*/
