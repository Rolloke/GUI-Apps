/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ChildFrm.cpp $
// ARCHIVE:		$Archive: /Project/Clients/SDIConfig/ChildFrm.cpp $
// CHECKIN:		$Date: 16.01.98 9:57 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 5.12.97 13:21 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#include "stdafx.h"
#include "SDIConfig.h"

#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildFrame
IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
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
BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	// WS_MAXIMIZEBOX muss sein, sonst kann das Fenster nicht maximiert werden
	// Auch nicht durch Aufruf von MDIMaximize() !!!
	// KEIN WS_SYSMENU : entfernt die Menu-, Minimize-, Maximize- und Schliessen-Box
	// KEIN WS_VISIBLE : sonst flackert es fuerchterlich beim Aufbau
	cs.style = WS_CHILD | WS_OVERLAPPED | WS_CAPTION |
				FWS_ADDTOTITLE | WS_MAXIMIZEBOX;

	return CMDIChildWnd::PreCreateWindow(cs);
}
/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers
