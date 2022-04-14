/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ViewSniCom.cpp $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/SDITest/ViewSniCom.cpp $
// CHECKIN:		$Date: 18.06.99 12:57 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 18.06.99 12:08 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#include "stdafx.h"
#include "ViewSniCom.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CViewSniCom, CSDITestView)
//////////////////////////////////////////////////////////////////////
CViewSniCom::CViewSniCom()
	: CSDITestView(CViewSniCom::IDD)
{
	//{{AFX_DATA_INIT(CViewSniCom)
	//}}AFX_DATA_INIT
	m_uTimerID = TIMER_SNI_COM;
}
//////////////////////////////////////////////////////////////////////
CViewSniCom::~CViewSniCom()
{

}
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
CDocSniCom* CViewSniCom::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDocSniCom)));
	return (CDocSniCom*)m_pDocument;
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
void CViewSniCom::ShowHide()
{
	// 
/*	BOOL bEnable = !(GetDocument()->m_bStarted)
					&& !(GetDocument()->m_bRepeat
					&& (0 != GetDocument()->m_iRepeatHow));
*/
	// Basisklasse aufrufen
	CSDITestView::ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CViewSniCom::InitializeData()
{
	CSDITestView::InitializeData();
}
/////////////////////////////////////////////////////////////////////////////
void CViewSniCom::DoDataExchange(CDataExchange* pDX)
{
	CSDITestView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewSniCom)
	//}}AFX_DATA_MAP

	if (m_ToolTip.GetSafeHwnd()) {
		m_ToolTip.Activate(TRUE);
	}
}
/////////////////////////////////////////////////////////////////////////////
// CViewSniCom diagnostics
#ifdef _DEBUG
void CViewSniCom::AssertValid() const
{
	CSDITestView::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CViewSniCom::Dump(CDumpContext& dc) const
{
	CSDITestView::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CViewSniCom, CSDITestView)
	//{{AFX_MSG_MAP(CViewSniCom)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// v message handlers
