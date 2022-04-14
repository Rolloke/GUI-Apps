/////////////////////////////////////////////////////////////////////////////
// PROJECT:		ImageCompare
// FILE:		$Workfile: ImageCompareView.cpp $
// ARCHIVE:		$Archive: /Project/Tools/ImageCompare/ImageCompareView.cpp $
// CHECKIN:		$Date: 22.04.99 16:14 $
// VERSION:		$Revision: 7 $
// LAST CHANGE:	$Modtime: 22.04.99 16:14 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImageCompare.h"

#include "wk_trace.h"
#include "ImageCompareDoc.h"
#include "ImageCompareView.h"
#include "Imagn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImageCompareView

IMPLEMENT_DYNCREATE(CImageCompareView, CView)

BEGIN_MESSAGE_MAP(CImageCompareView, CView)
	//{{AFX_MSG_MAP(CImageCompareView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CImageCompareView::CImageCompareView()
{
}

/////////////////////////////////////////////////////////////////////////////
CImageCompareView::~CImageCompareView()
{
}

/////////////////////////////////////////////////////////////////////////////
void CImageCompareView::OnDraw(CDC* pDC)
{
	CImageCompareDoc* pDoc = GetDocument();
	if (pDoc && pDC)
	{
		CRect rcClient, rcImage1, rcImage2;
		GetClientRect(rcClient);
		rcImage1 = rcClient;
		rcImage2 = rcClient;
		rcImage1.right = rcImage1.left + rcClient.Width() / 2;
		rcImage2.left = rcImage1.right;

		IM_PaintDC(pDC->m_hDC, pDoc->GetBufferID1(), rcImage1, NULL, NULL, SCALE_OFF);
		IM_PaintDC(pDC->m_hDC, pDoc->GetBufferID2(), rcImage2, NULL, NULL, SCALE_OFF);
		pDC->TextOut(rcImage1.left, rcImage1.top, "Referenzbild");
		pDC->TextOut(rcImage2.left, rcImage2.top, "Istbild");
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CImageCompareView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	BOOL bRet = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

	if (!GetDocument()->InitImagin(m_hWnd))
	{
		WK_TRACE_ERROR("InitImagin failed\n");
		bRet = FALSE;
	}

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
// CImageCompareView diagnostics
#ifdef _DEBUG
/////////////////////////////////////////////////////////////////////////////
CImageCompareDoc* CImageCompareView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CImageCompareDoc)));
	return (CImageCompareDoc*)m_pDocument;
}
void CImageCompareView::AssertValid() const
{
	CView::AssertValid();
}

void CImageCompareView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG
