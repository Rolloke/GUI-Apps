/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ChildFrm.cpp $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/SDITest/ChildFrm.cpp $
// CHECKIN:		$Date: 11.03.98 10:34 $
// VERSION:		$Revision: 6 $
// LAST CHANGE:	$Modtime: 10.03.98 12:41 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#include "stdafx.h"
#include "SDITest.h"

#include "ChildFrm.h"
#include "SDITestView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildFrame
IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)
/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction
CChildFrame::CChildFrame()
{
	// TODO: add member initialization code here
	
}
/////////////////////////////////////////////////////////////////////////////
CChildFrame::~CChildFrame()
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style = WS_CHILD | WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE/* | WS_MAXIMIZE | WS_VISIBLE*/;

	// WS_MAXIMIZEBOX muss sein, sonst kann das Fenster nicht maximiert werden
	// Auch nicht durch Aufruf von MDIMaximize() !!!
	// KEIN WS_SYSMENU : entfernt die Menu-, Minimize-, Maximize- und Schliessen-Box
	// KEIN WS_VISIBLE : sonst flackert es fuerchterlich beim Aufbau
//	cs.style = WS_CHILD | WS_OVERLAPPED | WS_CAPTION |
//				FWS_ADDTOTITLE | WS_MAXIMIZEBOX;

	return CMDIChildWnd::PreCreateWindow(cs);
}
/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics
#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers
BOOL CChildFrame::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class

	// Den View zerstoeren,
	// damit wir einen definierten Zustand der Timer erhalten
	// CView::DestroyWindow wird nicht automatisch aufgerufen!!!

	CWnd* pView = GetWindow(GW_CHILD);
	if ( pView && IsChild(pView) ) {
		// Sicherstellen, dass es auch ein SDITestView ist
		ASSERT(pView->IsKindOf( RUNTIME_CLASS(CSDITestView) ) );
		pView->DestroyWindow();
	}

	return CMDIChildWnd::DestroyWindow();
}
