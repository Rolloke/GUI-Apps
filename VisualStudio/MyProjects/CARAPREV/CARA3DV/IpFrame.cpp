// IpFrame.cpp : Implementierung der Klasse CInPlaceFrame
//

#include "stdafx.h"

#ifdef ETS_OLE_SERVER

#include "CARA3DV.h"
#include "CARA3DVView.h"

#include "IpFrame.h"

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
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
	// Globale Hilfebefehle
	ON_COMMAND(ID_HELP_FINDER, COleIPFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, COleIPFrameWnd::OnHelp)
	ON_COMMAND(ID_DEFAULT_HELP, COleIPFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_CONTEXT_HELP, COleIPFrameWnd::OnContextHelp)
   ON_MESSAGE(WM_ENTERSIZEMOVE, OnEnterSizeMove)
   ON_MESSAGE(WM_EXITSIZEMOVE , OnExitSizeMove)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceFrame Konstruktion/Destruktion

CInPlaceFrame::CInPlaceFrame()
{
   BEGIN("CInPlaceFrame()");
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
		TRACE0("Symbolleiste konnte nicht erstellt werden\n");
		return FALSE;
	}


   m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	pWndFrame->EnableDocking(CBRS_ALIGN_ANY);
	pWndFrame->DockControlBar(&m_wndToolBar);

   CString string;
   string.LoadString(IDS_MAIN_TOOLBAR_NAME);
   m_wndToolBar.SetWindowText(string);

	return TRUE;
}

void CInPlaceFrame::OnDestroy() 
{
   BEGIN("CInPlaceFrame::OnDestroy");
	CInPlaceFrame::OnDestroy();
}

BOOL CInPlaceFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// ZU ERLEDIGEN: Ändern Sie hier die Fensterklasse oder das Erscheinungsbild, indem Sie
	//  CREATESTRUCT cs modifizieren.

	return COleIPFrameWnd::PreCreateWindow(cs);
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
   CCARA3DVView *pC3dv = NULL;
   if (pView && pView->IsKindOf(RUNTIME_CLASS(CCARA3DVView)))
   {
      pC3dv =  (CCARA3DVView*)pView;
      pC3dv->m_bIPframeRecalcLayOut = true;
   }
	
	COleIPFrameWnd::RecalcLayout(bNotify);

   if (pC3dv) pC3dv->m_bIPframeRecalcLayOut = false;
}

LRESULT CInPlaceFrame::OnEnterSizeMove(WPARAM wParam, LPARAM lParam)
{
   BEGIN("CInPlaceFrame::OnEnterSizeMove");
   CView *pView = GetActiveView();
   if (pView && pView->IsKindOf(RUNTIME_CLASS(CCARA3DVView)))
   {
      ((CCARA3DVView*)pView)->SendMsgToGLView(WM_ENTERSIZEMOVE, wParam, lParam);
   }
   return 0;
}

LRESULT CInPlaceFrame::OnExitSizeMove(WPARAM wParam, LPARAM lParam)
{
   BEGIN("CInPlaceFrame::OnExitSizeMove");
   CView *pView = GetActiveView();
   if (pView && pView->IsKindOf(RUNTIME_CLASS(CCARA3DVView)))
   {
      ((CCARA3DVView*)pView)->SendMsgToGLView(WM_EXITSIZEMOVE, wParam, lParam);
   }
   return 0;
}

#endif // ETS_OLE_SERVER
