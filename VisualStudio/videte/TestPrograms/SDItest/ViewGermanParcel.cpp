/* GlobalReplace: CSDITestGermanParcelView --> CViewGermanParcel */
/* GlobalReplace: CSDITestGermanParcelDoc --> CDocGermanParcel */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ViewGermanParcel.cpp $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/SDITest/ViewGermanParcel.cpp $
// CHECKIN:		$Date: 22.04.99 9:17 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 22.04.99 9:12 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#include "stdafx.h"
#include "ViewGermanParcel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewGermanParcel
IMPLEMENT_DYNCREATE(CViewGermanParcel, CSDITestView)
/////////////////////////////////////////////////////////////////////////////
CViewGermanParcel::CViewGermanParcel()
	: CSDITestView(CViewGermanParcel::IDD)
{
	//{{AFX_DATA_INIT(CViewGermanParcel)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
/////////////////////////////////////////////////////////////////////////////
CViewGermanParcel::~CViewGermanParcel()
{
}
/////////////////////////////////////////////////////////////////////////////
void CViewGermanParcel::DoDataExchange(CDataExchange* pDX)
{
	CSDITestView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewGermanParcel)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
// CViewGermanParcel diagnostics
#ifdef _DEBUG
void CViewGermanParcel::AssertValid() const
{
	CSDITestView::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CViewGermanParcel::Dump(CDumpContext& dc) const
{
	CSDITestView::Dump(dc);
}
/////////////////////////////////////////////////////////////////////////////
CDocGermanParcel* CViewGermanParcel::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDocGermanParcel)));
	return (CDocGermanParcel*)m_pDocument;
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CViewGermanParcel, CSDITestView)
	//{{AFX_MSG_MAP(CViewGermanParcel)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CViewGermanParcel message handlers
