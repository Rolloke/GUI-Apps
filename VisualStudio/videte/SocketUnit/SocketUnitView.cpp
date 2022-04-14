// SocketUnitView.cpp : implementation of the CSocketUnitView class
//

#include "stdafx.h"


#include "SocketUnit.h"
#include "SocketUnitDoc.h"
#include "SocketUnitView.h"
#include "ServerControlSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSocketUnitView

IMPLEMENT_DYNCREATE(CSocketUnitView, CView)

BEGIN_MESSAGE_MAP(CSocketUnitView, CView)
	//{{AFX_MSG_MAP(CSocketUnitView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSocketUnitView construction/destruction

CSocketUnitView::CSocketUnitView()
{
	// TODO: add construction code here

}
/////////////////////////////////////////////////////////////////////////////////////
CSocketUnitView::~CSocketUnitView()
{
}
/////////////////////////////////////////////////////////////////////////////////////
BOOL CSocketUnitView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSocketUnitView drawing

void CSocketUnitView::OnDraw(CDC* pDC)
{
	CSocketUnitDoc* pDoc = GetDocument();
	CConnectionThreads& threads = pDoc->GetConnectionThreads();
	CString s;
	CFont font;

	font.CreatePointFont(100,_T("Arial"));

	CFont* pFont = pDC->SelectObject(&font);
	int x,y;

	x = 0;
	y = 0;

	for (int i=0;i<threads.GetSize();i++)
	{
		s = threads[i]->Format();
		CSize size = pDC->GetOutputTextExtent(s);
		pDC->TextOut(x,y,s);
		y += size.cy+1;
	}

	if (pDoc->m_pCipcControl)
	{
		s = CString(pDoc->m_pCipcControl->GetShmName())+ _T(" ")+NameOfEnum(pDoc->m_pCipcControl->GetIPCState());
		CSize size = pDC->GetOutputTextExtent(s);
		pDC->TextOut(x,y,s);
		y += size.cy+1;
	}

	pDC->SelectObject(pFont);
}

/////////////////////////////////////////////////////////////////////////////
// CSocketUnitView diagnostics

#ifdef _DEBUG
void CSocketUnitView::AssertValid() const
{
	CView::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////////////
void CSocketUnitView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
/////////////////////////////////////////////////////////////////////////////////////
CSocketUnitDoc* CSocketUnitView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSocketUnitDoc)));
	return (CSocketUnitDoc*)m_pDocument;
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CSocketUnitView message handlers

void CSocketUnitView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
}
