/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: MainFrm.cpp $
// ARCHIVE:		$Archive: /Project/Clients/SDIConfig/MainFrm.cpp $
// CHECKIN:		$Date: 8.02.99 10:35 $
// VERSION:		$Revision: 21 $
// LAST CHANGE:	$Modtime: 8.02.99 10:35 $
// BY AUTHOR:	$Author: Uwe $
// $Nokeywords:$

#include "stdafx.h"
#include "SDIConfig.h"

#include "SDIConfigView.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame
IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_CONNECT, OnConnect)
	ON_UPDATE_COMMAND_UI(ID_CONNECT, OnUpdateConnect)
	ON_COMMAND(ID_DISCONNECT, OnDisconnect)
	ON_UPDATE_COMMAND_UI(ID_DISCONNECT, OnUpdateDisconnect)
	ON_COMMAND(ID_CONFIGURE, OnConfigure)
	ON_UPDATE_COMMAND_UI(ID_CONFIGURE, OnUpdateConfigure)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_EXTERN_PROGRAMM, OnExternProgramm)
	ON_UPDATE_COMMAND_UI(ID_EXTERN_PROGRAMM, OnUpdateExternProgramm)
	ON_COMMAND(ID_CONNECT_LOCAL, OnConnectLocal)
	ON_UPDATE_COMMAND_UI(ID_CONNECT_LOCAL, OnUpdateConnectLocal)
	//}}AFX_MSG_MAP
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
	// TODO: add member initialization code here
	m_bResizing = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
CMainFrame::~CMainFrame()
{
}
/////////////////////////////////////////////////////////////////////////////
CControlBar* CMainFrame::GetToolBar() const
{
	if (WK_IS_WINDOW(&m_wndCoolBar))
	{
		return (CControlBar*)&m_wndCoolBar;
	}
	if (WK_IS_WINDOW(&m_wndToolBar))
	{
		return (CControlBar*)&m_wndToolBar;
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	CRect rcWorkSpace(0,0,0,0);
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkSpace, 0);
	cs.cy = 100;
	cs.cx = 100;
	cs.y = rcWorkSpace.top;
	cs.x = rcWorkSpace.left;
	
	BOOL bReturn = CMDIFrameWnd::PreCreateWindow(cs);

	cs.lpszClass = WK_APP_NAME_SDICONFIG;
	cs.style &= ~FWS_ADDTOTITLE;

	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
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
//	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
//	EnableDocking(CBRS_ALIGN_ANY);
//	DockControlBar(&m_wndToolBar);

	m_CoolMenuManager.m_bAutoAccel = FALSE;
	m_CoolMenuManager.Install(this);
	m_CoolMenuManager.LoadToolbar(IDR_MAINFRAME);

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics
#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::Resize(CRect& rcNew)
{
	// Wir wollen gerne alles sehen koennen,
	// daher Hauptfenster an View bzw. Client-Bereich des MDIChild anpassen
	BOOL bReturn = TRUE;

	// Diese Routine darf niemals rekursiv aufgerufen werden
	if (m_bResizing) {
		return bReturn;
	}

	// Natuerlich nur, wenn ueberhaupt ein MDIChild vorhanden ist
	CMDIChildWnd* pMDIChild = MDIGetActive();
	if (!pMDIChild) {
		return bReturn;
	}
	
	// Natuerlich nur, wenn ueberhaupt ein View vorhanden ist
	CView *pView = pMDIChild->GetActiveView();
	if (!pView) {
		return bReturn;
	}

	// Sicherstellen, dass es auch ein SDIConfigView ist
	ASSERT(pView->IsKindOf( RUNTIME_CLASS(CSDIConfigView) ) );
	CSDIConfigView* pSDIView = (CSDIConfigView*)pView;

	// Diese Funktion nur bearbeiten, wenn die optimale Groesse ueberhaupt gegeben ist
	int iViewOptCx = pSDIView->GetOptimalSize().cx;
	int iViewOptCy = pSDIView->GetOptimalSize().cy;
	if ( 0==iViewOptCx || 0==iViewOptCy ) {
		return bReturn;
	}
	m_bResizing = TRUE;

	// Hole die Client-Masse des Hauptfenster
	// und berechne die Netto-Flaeche
	// Darin sind evtl. auch die Bars enthalten!
	CRect rcMainClient(0,0,0,0);
	GetClientRect(rcMainClient);
	// Wenn die Clienthoehe 0 ist, dann wurde das Fenster bis ins Menue hinein verkleinert
	// Dadurch wird die Fensterhoehe falsch berechnet:
	// Die StatusBarhoehe wird zu klein angenommen und die Menuehoehe fehlt auch
	// Deshalb FALSE zurueckgeben und danach nochmal berechnen
	if (0== rcMainClient.bottom) {
		bReturn = FALSE;
	}
	int ixNetto = rcMainClient.right;
	int iyNetto = rcMainClient.bottom;
	
	// Wenn die StatusBar sichtbar ist, muessen wir sie abziehen
	if ( GetStatusBar()->IsWindowVisible() ) {
		CRect rcStatusBarClient(0,0,0,0);
		GetStatusBar()->GetClientRect(rcStatusBarClient);
		iyNetto -= rcStatusBarClient.bottom;
	}

	// Wenn die ToolBar sichtbar ist, muessen wir sie abziehen
	if ( GetToolBar()->IsWindowVisible() ) {
		// aber nur, wenn sie an diesen Frame gedockt ist
		if ( this == GetToolBar()->GetDockingFrame() ) {
			CRect rcToolBarClient(0,0,0,0);
			// Wir nehmen die ganze ToolBar inklusive Rahmen
			GetToolBar()->GetWindowRect(rcToolBarClient);
			ScreenToClient(rcToolBarClient);
			// Wie ist die ToolBar angeordnet?
			// Vertikal?
			if ( (rcToolBarClient.bottom - rcToolBarClient.top) > 
										(rcToolBarClient.right - rcToolBarClient.left) )
			{
				ixNetto -= rcToolBarClient.right - rcToolBarClient.left;
			}
			// sonst horizontal
			else {
				iyNetto -= rcToolBarClient.bottom - rcToolBarClient.top;
			}
		}
	}

	// Ist die optimale Groesse des View groesser als der
	// Netto-Clientbereich des Hauptfensters?
	int xdiff = iViewOptCx  - ixNetto;
	int ydiff = iViewOptCy  - iyNetto;

	// Wir wollen das Fenster aber nicht unnoetig verkleinern,
	if ( (xdiff>0) || (ydiff>0) ) {
		// Hole die bisherige Fenstergroesse des Hauptfensters
		CRect rcMainWnd(0,0,0,0);
		GetWindowRect(rcMainWnd);

		if ( (xdiff>0) ) {
			// Berechne die neue x-Fenstergroesse
			rcMainWnd.right += xdiff;
		}

		if ( (ydiff>0) ) {
			// Berechne die neue y-Fenstergroesse,
			rcMainWnd.bottom += ydiff;
		}
		
		// Setze die neue Fenstergroesse
		rcNew = rcMainWnd;
		MoveWindow(rcMainWnd);

		// Lasse das Fenster neu zeichnen,
		// ToolBar und StatusBar werden dann automatisch angepasst
		UpdateWindow();
	}

	// Wir verlassen diese Funktion, also kann der naechste rein
	m_bResizing = FALSE;
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::RecalcLayout(BOOL bNotify) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CMDIFrameWnd::RecalcLayout(bNotify);

	// Diese Funktion wird immer aufgerufen, wenn
	//		die Fenstergroesse geandert wurde
	//		die ControlBars verschoben wurden
	// daher das Hauptfenster anpassen
	CRect rcNew(0,0,0,0);
	if ( !Resize(rcNew) ) {
		// Es kann sein, dass das Fenster evtl nicht korrekt berechnet wurde
		// dann nochmal das ganze
		Resize(rcNew);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (MSG_SDI_CONNECTION_READY==wParam) {
		// Natuerlich nur, wenn ueberhaupt ein MDIChild vorhanden ist
		CMDIChildWnd* pMDIChild = MDIGetActive();
		if (pMDIChild) {
			// Natuerlich nur, wenn ueberhaupt ein View vorhanden ist
			CView *pView = pMDIChild->GetActiveView();
			if (pView) {
				// Sicherstellen, dass es auch ein SDIConfigView ist
				ASSERT(pView->IsKindOf( RUNTIME_CLASS(CSDIConfigView) ) );
				CSDIConfigView* pSDIView = (CSDIConfigView*)pView;

				CPostFetchResult* pPostResult = (CPostFetchResult*)lParam;
				//	FetchCallType m_type;
				//	CIPCFetchResult m_result;
				//	DWORD m_dwMessage;
				//	DWORD m_dwUserData;

				// Wenn es das richtige Dokument ist
				if ( pSDIView->GetDocument() == (CSDIConfigDoc*)pPostResult->m_dwUserData ) {
					// Benachrichtige das Dokument
					pSDIView->GetDocument()->OnConnection(pPostResult->m_result);
				}

				// Auf jeden Fall muss das CPostFetchResult deleted werden!
				WK_DELETE(pPostResult);
				return TRUE;
			}
		}
	}
	
	if ( (MSG_SDI_FIRST<=wParam) && (MSG_SDI_LAST>=wParam) ) {
		WK_TRACE_WARNING("CMainFrame::OnCommand wParam:%08x lParam:%08x NOT HANDLED\n",
													wParam, lParam);
	}

	return CMDIFrameWnd::OnCommand(wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnConnect() 
{
	CMDIChildWnd* pMDIChild = MDIGetActive();
	if (pMDIChild) {
		// Natuerlich nur, wenn ueberhaupt ein View vorhanden ist
		CView *pView = pMDIChild->GetActiveView();
		if (pView) {
			// Sicherstellen, dass es auch ein SDIConfigView ist
			ASSERT(pView->IsKindOf( RUNTIME_CLASS(CSDIConfigView) ) );
			CSDIConfigView* pSDIView = (CSDIConfigView*)pView;
			// Benachrichtige das Dokument
			pSDIView->GetDocument()->StartConnection(FALSE);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateConnect(CCmdUI* pCmdUI) 
{
	BOOL bEnable = FALSE;
	CMDIChildWnd* pMDIChild = MDIGetActive();
	if (pMDIChild) {
		// Natuerlich nur, wenn ueberhaupt ein View vorhanden ist
		CView *pView = pMDIChild->GetActiveView();
		if (pView) {
			// Sicherstellen, dass es auch ein SDIConfigView ist
			ASSERT(pView->IsKindOf( RUNTIME_CLASS(CSDIConfigView) ) );
			CSDIConfigView* pSDIView = (CSDIConfigView*)pView;
			// Ist es erlaubt
			bEnable = ( !(pSDIView->GetDocument()->IsConnecting())
						&& !(pSDIView->GetDocument()->IsConnected())
						&& !(pSDIView->GetDocument()->IsExternBusy()) );
		}
	}
	pCmdUI->Enable(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnConnectLocal() 
{
	CMDIChildWnd* pMDIChild = MDIGetActive();
	if (pMDIChild) {
		// Natuerlich nur, wenn ueberhaupt ein View vorhanden ist
		CView *pView = pMDIChild->GetActiveView();
		if (pView) {
			// Sicherstellen, dass es auch ein SDIConfigView ist
			ASSERT(pView->IsKindOf( RUNTIME_CLASS(CSDIConfigView) ) );
			CSDIConfigView* pSDIView = (CSDIConfigView*)pView;
			// Benachrichtige das Dokument
			pSDIView->GetDocument()->StartConnection(TRUE);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateConnectLocal(CCmdUI* pCmdUI) 
{
	OnUpdateConnect(pCmdUI)	;
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnDisconnect() 
{
	CMDIChildWnd* pMDIChild = MDIGetActive();
	if (pMDIChild) {
		// Natuerlich nur, wenn ueberhaupt ein View vorhanden ist
		CView *pView = pMDIChild->GetActiveView();
		if (pView) {
			// Sicherstellen, dass es auch ein SDIConfigView ist
			ASSERT(pView->IsKindOf( RUNTIME_CLASS(CSDIConfigView) ) );
			CSDIConfigView* pSDIView = (CSDIConfigView*)pView;
			// Ist es erlaubt
			BOOL bAllowed = TRUE;
			if ( pSDIView->GetDocument()->IsTransfering()
					|| pSDIView->GetDocument()->IsConfiguring() ) {
				// Will der User das wirklich
				bAllowed = (IDYES == AfxMessageBox(IDP_CONNECTION_BUSY, MB_YESNO|MB_DEFBUTTON2));
			}
			if (bAllowed) {
				pSDIView->GetDocument()->EndConnection(TRUE);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateDisconnect(CCmdUI* pCmdUI) 
{
	// Immer erlaubt bei Verbindung, aber Abfrage, falls busy, siehe OnDisconnect()
	BOOL bEnable = FALSE;
	CMDIChildWnd* pMDIChild = MDIGetActive();
	if (pMDIChild) {
		// Natuerlich nur, wenn ueberhaupt ein View vorhanden ist
		CView *pView = pMDIChild->GetActiveView();
		if (pView) {
			// Sicherstellen, dass es auch ein SDIConfigView ist
			ASSERT(pView->IsKindOf( RUNTIME_CLASS(CSDIConfigView) ) );
			CSDIConfigView* pSDIView = (CSDIConfigView*)pView;
			// Ist es erlaubt
			bEnable = (pSDIView->GetDocument()->IsConnected()
							|| pSDIView->GetDocument()->IsConnecting());
		}
	}
	pCmdUI->Enable(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnExternProgramm() 
{
	CMDIChildWnd* pMDIChild = MDIGetActive();
	if (pMDIChild) {
		// Natuerlich nur, wenn ueberhaupt ein View vorhanden ist
		CView *pView = pMDIChild->GetActiveView();
		if (pView) {
			// Sicherstellen, dass es auch ein SDIConfigView ist
			ASSERT(pView->IsKindOf( RUNTIME_CLASS(CSDIConfigView) ) );
			CSDIConfigView* pSDIView = (CSDIConfigView*)pView;
			// Benachrichtige das Dokument
			pSDIView->GetDocument()->StartExternProgram();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateExternProgramm(CCmdUI* pCmdUI) 
{
	BOOL bEnable = FALSE;
	CMDIChildWnd* pMDIChild = MDIGetActive();
	if (pMDIChild) {
		// Natuerlich nur, wenn ueberhaupt ein View vorhanden ist
		CView *pView = pMDIChild->GetActiveView();
		if (pView) {
			// Sicherstellen, dass es auch ein SDIConfigView ist
			ASSERT(pView->IsKindOf( RUNTIME_CLASS(CSDIConfigView) ) );
			CSDIConfigView* pSDIView = (CSDIConfigView*)pView;
			// Ist es erlaubt
			bEnable = ( (pSDIView->GetDocument()->WasExternProgramFound())
						&& !(pSDIView->GetDocument()->IsConnecting())
						&& !(pSDIView->GetDocument()->IsConnected())
						&& !(pSDIView->GetDocument()->IsExternBusy()) );
		}
	}
	pCmdUI->Enable(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnConfigure() 
{
	CMDIChildWnd* pMDIChild = MDIGetActive();
	if (pMDIChild) {
		// Natuerlich nur, wenn ueberhaupt ein View vorhanden ist
		CView *pView = pMDIChild->GetActiveView();
		if (pView) {
			// Sicherstellen, dass es auch ein SDIConfigView ist
			ASSERT(pView->IsKindOf( RUNTIME_CLASS(CSDIConfigView) ) );
			CSDIConfigView* pSDIView = (CSDIConfigView*)pView;
			// Benachrichtige das Dokument
			pSDIView->Configure();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateConfigure(CCmdUI* pCmdUI) 
{
	BOOL bEnable = FALSE;
	CMDIChildWnd* pMDIChild = MDIGetActive();
	if (pMDIChild) {
		// Natuerlich nur, wenn ueberhaupt ein View vorhanden ist
		CView *pView = pMDIChild->GetActiveView();
		if (pView) {
			// Sicherstellen, dass es auch ein SDIConfigView ist
			ASSERT(pView->IsKindOf( RUNTIME_CLASS(CSDIConfigView) ) );
			CSDIConfigView* pSDIView = (CSDIConfigView*)pView;
			// Ist es erlaubt
			bEnable = (pSDIView->GetDocument()->IsConnected()
						&& !(pSDIView->GetDocument()->IsConfiguring())
						&& pSDIView->ConfigureEnabled()
						&& !(pSDIView->GetDocument()->IsExternBusy()) );
		}
	}
	pCmdUI->Enable(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnAppAbout() 
{
	COemGuiApi::AboutDialog(this);
}
