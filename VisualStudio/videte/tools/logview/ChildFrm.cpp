/////////////////////////////////////////////////////////////////////////////
// PROJECT:		LogView
// FILE:		$Workfile: ChildFrm.cpp $
// ARCHIVE:		$Archive: /Project/Tools/LogView/ChildFrm.cpp $
// CHECKIN:		$Date: 29.04.05 14:09 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 29.04.05 14:08 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LogView.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include ".\childfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

/////////////////////////////////////////////////////////////////////////////
CChildFrame::CChildFrame()
{
	
}

/////////////////////////////////////////////////////////////////////////////
CChildFrame::~CChildFrame()
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
/////////////////////////////////////////////////////////////////////////////
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers


void CChildFrame::OnClose()
{
	if (!((CMainFrame*)AfxGetMainWnd())->IsSyncWindowThreadRunning())
	{
		CMDIChildWnd::OnClose();
	}
	((CMainFrame*)AfxGetMainWnd())->StopSyncWindowThread();
}
