// UnitTestView.cpp : implementation of the CUnitTestView class
//

#include "stdafx.h"
#include "UnitTest.h"

#include "UnitTestDoc.h"
#include "UnitTestView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUnitTestView

IMPLEMENT_DYNCREATE(CUnitTestView, CView)

BEGIN_MESSAGE_MAP(CUnitTestView, CView)
	//{{AFX_MSG_MAP(CUnitTestView)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUnitTestView construction/destruction

CUnitTestView::CUnitTestView()
{
}

CUnitTestView::~CUnitTestView()
{
}

BOOL CUnitTestView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_MAXIMIZE;

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CUnitTestView drawing

void CUnitTestView::OnDraw(CDC* pDC)
{
	CUnitTestDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CRect rect;
	CBrush brush(RGB(255,0,255));

	GetClientRect(rect);
	pDC->FillRect(rect,&brush);
}

////////////////////////////////////////////////////////////
// CUnitTestView printing

BOOL CUnitTestView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CUnitTestView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CUnitTestView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

/////////////////////////////////////////////////////////////////////////////
// CUnitTestView diagnostics

#ifdef _DEBUG
void CUnitTestView::AssertValid() const
{
	CView::AssertValid();
}

void CUnitTestView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

// non-debug version is inline
CUnitTestDoc* CUnitTestView::GetDocument() 
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CUnitTestDoc)));
	return (CUnitTestDoc*)m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CUnitTestView message handlers
