/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: MainFrm.cpp $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/SDITest/MainFrm.cpp $
// CHECKIN:		$Date: 11.09.98 11:23 $
// VERSION:		$Revision: 10 $
// LAST CHANGE:	$Modtime: 2.09.98 13:31 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#include "stdafx.h"
#include "resource.h"

#include "!todo.h"

#include "SDITestView.h"
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
	m_bResizing = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
CMainFrame::~CMainFrame()
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	CRect rcWorkSpace(0,0,0,0);
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkSpace, 0);
	cs.cy = 740;
	cs.cx = 500;
	cs.y = rcWorkSpace.top;
	cs.x = rcWorkSpace.left;
	
	BOOL bReturn = CMDIFrameWnd::PreCreateWindow(cs);

	cs.lpszClass = WK_APP_NAME_SDITEST;
	cs.style &= ~FWS_ADDTOTITLE;

	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
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

	return 0;
}
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

	// Sicherstellen, dass es auch ein SDITestView ist
	ASSERT(pView->IsKindOf( RUNTIME_CLASS(CSDITestView) ) );
	CSDITestView* pSDIView = (CSDITestView*)pView;

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
	if ( m_wndStatusBar.IsWindowVisible() ) {
		CRect rcStatusBarClient(0,0,0,0);
		m_wndStatusBar.GetClientRect(rcStatusBarClient);
		iyNetto -= rcStatusBarClient.bottom;
	}

	// Wenn die ToolBar sichtbar ist, muessen wir sie abziehen
	if ( m_wndToolBar.IsWindowVisible() ) {
		// aber nur, wenn sie an diesen Frame gedockt ist
		if ( this == m_wndToolBar.GetDockingFrame() ) {
			CRect rcToolBarClient(0,0,0,0);
			// Wir nehmen die ganze ToolBar inklusive Rahmen
			m_wndToolBar.GetWindowRect(rcToolBarClient);
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
void CMainFrame::RecalcLayout(BOOL bNotify) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CMDIFrameWnd::RecalcLayout(bNotify);

	// Diese Funktion wird immer aufgerufen, wenn
	//		die Fenstergroesse geandert wurde
	//		die ControlBars verschoben wurden
	// daher das Hauptfenster anpassen
//	CRect rcNew(0,0,0,0);
//	if ( !Resize(rcNew) ) {
//		// Es kann sein, dass das Fenster evtl nicht korrekt berechnet wurde
//		// dann nochmal das ganze
//		Resize(rcNew);
//	}
}
/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics
#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnAppAbout() 
{
#if 0
	CSharedDlg dlg;
	dlg.AboutDlg(m_hWnd, IDR_MAINFRAME);
#else
	COemGuiApi::AboutDialog(this);
#endif
}
