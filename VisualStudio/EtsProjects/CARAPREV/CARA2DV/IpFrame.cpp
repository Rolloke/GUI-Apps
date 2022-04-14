// IpFrame.cpp : Implementierung der Klasse CInPlaceFrame
//

#include "stdafx.h"

#ifdef ETS_OLE_SERVER

#include "CARA2DV.h"
#include "CARA2DVView.h"
#include "MainFrm.h"

#include "IpFrame.h"
#include "..\CaraMenu.h"

#define  ETSDEBUG_INCLUDE 
#include "CEtsDebug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInPlaceFrame

IMPLEMENT_DYNCREATE(CInPlaceFrame, COleIPFrameWnd)

BEGIN_MESSAGE_MAP(CInPlaceFrame, COleIPFrameWnd)
	//{{AFX_MSG_MAP(CInPlaceFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_STATUS_BAR, OnViewStatusBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_STATUS_BAR, OnUpdateViewStatusBar)
	ON_WM_SHOWWINDOW()
	ON_WM_COPYDATA()
	ON_WM_KILLFOCUS()
	ON_WM_TIMER()
	ON_COMMAND(ID_VIEW_STATUS_BAR_FP, OnViewStatusBarFp)
	ON_UPDATE_COMMAND_UI(ID_VIEW_STATUS_BAR_FP, OnUpdateViewStatusBarFp)
	//}}AFX_MSG_MAP
	// Globale Hilfebefehle
	ON_COMMAND(ID_HELP_FINDER, COleIPFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, COleIPFrameWnd::OnHelp)
	ON_COMMAND(ID_DEFAULT_HELP, COleIPFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_CONTEXT_HELP, COleIPFrameWnd::OnContextHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceFrame Konstruktion/Destruktion

CInPlaceFrame::CInPlaceFrame()
{
   BEGIN("CInPlaceFrame");
   m_hwndStatusBar = NULL;
}

CInPlaceFrame::~CInPlaceFrame()
{
}

int CInPlaceFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   BEGIN("CInPlaceFrame::OnCreate");
	if (COleIPFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// CResizeBar implementiert direkte Größenänderungen.
	if (!m_wndResizeBar.Create(this))
	{
		TRACE0("Leiste für Größenänderung konnte nicht erstellt werden\n");
		return -1;      // Fehler bei Erstellung
	}

   CCaraMenu::SysColorChange();
	// Allgemein ist es von Vorteil, ein Drop-Ziel zu registrieren, das
	//  keinerlei Auswirkung auf Ihr Rahmenfenster hat. Dies verhindert bei Drops ("Abwürfen")
	// ein "Durchfallen" zu einem Container, der Drag and Drop unterstützt.
	m_dropTarget.Register(this);
	return 0;
}

// OnCreateControlBars wird automatisch aufgerufen, um Steuerleisten im Fenster der
//  Container-Anwendung zu erstellen. pWndFrame ist das hierarchisch höchste Rahmenfenster des
//  Containers und ist immer ungleich NULL. pWndDoc ist das Rahmenfenster auf Dokumentebene
//  und ist gleich NULL, wenn der Container eine SDI-Anwendung ist. Eine Server
//  Anwendung kann MFC-Steuerelementleisten in jedes Fenster platzieren.
BOOL CInPlaceFrame::OnCreateControlBars(CFrameWnd* pWndFrame, CFrameWnd* pWndDoc)
{
   BEGIN("CInPlaceFrame::OnCreateControlBars");
	// Entfernen Sie dies, wenn Sie pWndDoc verwenden
	UNREFERENCED_PARAMETER(pWndDoc);

	// Dieses Fenster als Owner festlegen, damit Nachrichten an die richtige Anwendung gesendet werden
	m_wndToolBar.SetOwner(this);

	// Symbolleiste im Client-Fenster erzeugen
	if (!m_wndToolBar.CreateEx(pWndFrame, TBSTYLE_FLAT,WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_SRVR_INPLACE))
	{
		REPORT("Symbolleiste konnte nicht erstellt werden\n");
		return FALSE;
	}

   m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle()|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC|CBRS_GRIPPER);

   m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	pWndFrame->EnableDocking(CBRS_ALIGN_ANY);
	pWndFrame->DockControlBar(&m_wndToolBar);
 
   CRect rect;
   rect.SetRectEmpty();
   if (!m_wndStatusBar.Create(pWndFrame) ||
       !SetMainPaneInfo())
   {
      REPORT("Failed to create status bar\n");
      return -1;      // Fehler beim Erzeugen
   }

   ::EnumChildWindows(pWndFrame->m_hWnd, SearchStatusWnd, (LPARAM) &m_hwndStatusBar);

   CString string;
   string.LoadString(IDS_TOOLBAR);
   m_wndToolBar.SetWindowText(string);
	return TRUE;
}

BOOL CALLBACK CInPlaceFrame::SearchStatusWnd(HWND hwnd, LPARAM lParam)
{
   char szName[64];
   if (::GetClassName(hwnd, szName, 64) && (strcmp(szName, "msctls_statusbar32")==0))
   {
      *((HWND*)lParam) = hwnd;
      return 0;
   }
   return 1;
}
/////////////////////////////////////////////////////////////////////////////
// CInPlaceFrame Diagnose

#ifdef _DEBUG
void CInPlaceFrame::AssertValid() const
{
	COleIPFrameWnd::AssertValid();
}

void CInPlaceFrame::Dump(CDumpContext& dc) const
{
	COleIPFrameWnd::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CInPlaceFrame Befehle

void CInPlaceFrame::RecalcLayout(BOOL bNotify) 
{
   BEGIN("CInPlaceFrame::RecalcLayout");
   CView *pView = GetActiveView();
   CCARA2DVView *pC2dv = NULL;
   if (pView && pView->IsKindOf(RUNTIME_CLASS(CCARA2DVView)))
   {
      pC2dv =  (CCARA2DVView*)pView;
      pC2dv->m_bIPframeRecalcLayOut = true;
   }
	
	COleIPFrameWnd::RecalcLayout(bNotify);

   if (pC2dv) pC2dv->m_bIPframeRecalcLayOut = false;
}

void CInPlaceFrame::ActivateFrame(int nCmdShow) 
{
	if (nCmdShow == SW_HIDE)
   {
      CCARA2DVDoc*pDoc = (CCARA2DVDoc*)GetActiveDocument();
      if (pDoc->m_bOleChanged)
      {
         pDoc->GetLabelContainer()->ResetStates();
         CCARA2DVView*pView = (CCARA2DVView*) GetActiveView();
         pView->ResetPickStatesQuiet();
         pDoc->UpdateAllItems(NULL);
         pDoc->m_bOleChanged = false;
      }
   }
	
	COleIPFrameWnd::ActivateFrame(nCmdShow);
}

bool CInPlaceFrame::SetMainPaneInfo()
{
   bool bResult = (m_wndStatusBar.SetIndicators(CMainFrame::gm_nMainIndicators, NO_OF_MAIN_INDICATORS)!=0) ? true : false;
   if (bResult)
   {
      m_wndStatusBar.SetPaneInfo(0,0,SBPS_STRETCH|SBPS_NOBORDERS,0);
      m_wndStatusBar.SetPaneInfo(1,1,0,140);
      m_wndStatusBar.SetPaneInfo(2,2,0, 90);
      m_wndStatusBar.SetPaneInfo(3,3,0, 90);
      m_wndStatusBar.ModifyStyle(SBARS_SIZEGRIP, 0, SWP_DRAWFRAME);
   }
   else ASSERT(false);
   return bResult;
}

void CInPlaceFrame::SetStatusPaneText(UINT nID, char *text)
{
   m_wndStatusBar.SetPaneText(nID, text);
}


void CInPlaceFrame::OnViewStatusBar() 
{
   if (m_wndStatusBar.IsWindowVisible())
   {
      ShowControlBar(&m_wndStatusBar, false, false);
   }
   else
   {
      ShowControlBar(&m_wndStatusBar, true, false);
      if (m_hwndStatusBar)
      {
         if (::IsWindowVisible(m_hwndStatusBar))
            m_wndStatusBar.ModifyStyle(SBARS_SIZEGRIP, 0, SWP_FRAMECHANGED|SWP_DRAWFRAME);
         else
            m_wndStatusBar.ModifyStyle(0, SBARS_SIZEGRIP, SWP_FRAMECHANGED|SWP_DRAWFRAME );
      }
   }
}

void CInPlaceFrame::OnUpdateViewStatusBar(CCmdUI* pCmdUI) 
{
   pCmdUI->SetCheck(m_wndStatusBar.IsWindowVisible());
}

void CInPlaceFrame::OnViewStatusBarFp() 
{
   if (m_hwndStatusBar)
   {
      if (m_pMainFrame)
      {
         long lID = ::GetWindowLong(m_hwndStatusBar, GWL_ID);
         m_pMainFrame->SendMessage(WM_COMMAND, lID, 0);
         m_pMainFrame->RecalcLayout(false);

         if (::IsWindowVisible(m_hwndStatusBar))
            m_wndStatusBar.ModifyStyle(SBARS_SIZEGRIP, 0, SWP_FRAMECHANGED|SWP_DRAWFRAME);
         else
            m_wndStatusBar.ModifyStyle(0, SBARS_SIZEGRIP, SWP_FRAMECHANGED|SWP_DRAWFRAME);
      }
   }
}

void CInPlaceFrame::OnUpdateViewStatusBarFp(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(m_hwndStatusBar != NULL);
   pCmdUI->SetCheck(::IsWindowVisible(m_hwndStatusBar) ? 1 : 0);
}

void CInPlaceFrame::OnFinalRelease() 
{
	// TODO: Speziellen Code hier einfügen und/oder Basisklasse aufrufen
	
	COleIPFrameWnd::OnFinalRelease();
}

#endif // ETS_OLE_SERVER
