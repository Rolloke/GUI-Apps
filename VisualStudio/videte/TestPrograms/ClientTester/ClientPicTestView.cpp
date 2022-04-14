// ClientPicTestView.cpp : implementation of the CClientPicTestView class
//

#include "stdafx.h"
#include "ClientPicTest.h"

#include "ClientPicTestDoc.h"
#include "ClientPicTestView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClientPicTestView

IMPLEMENT_DYNCREATE(CClientPicTestView, CView)

BEGIN_MESSAGE_MAP(CClientPicTestView, CView)
	//{{AFX_MSG_MAP(CClientPicTestView)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClientPicTestView construction/destruction

CClientPicTestView::CClientPicTestView()
{
}

CClientPicTestView::~CClientPicTestView()
{
}

BOOL CClientPicTestView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_MAXIMIZE;

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CClientPicTestView drawing

void CClientPicTestView::OnDraw(CDC* pDC)
{
	CClientPicTestDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CRect rect;
	CBrush brush(RGB(255,0,255));

	GetClientRect(rect);
	pDC->FillRect(rect,&brush);
}

////////////////////////////////////////////////////////////
// CClientPicTestView printing

BOOL CClientPicTestView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CClientPicTestView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CClientPicTestView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

/////////////////////////////////////////////////////////////////////////////
// CClientPicTestView diagnostics

#ifdef _DEBUG
void CClientPicTestView::AssertValid() const
{
	CView::AssertValid();
}

void CClientPicTestView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

// non-debug version is inline
CClientPicTestDoc* CClientPicTestView::GetDocument() 
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CClientPicTestDoc)));
	return (CClientPicTestDoc*)m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CClientPicTestView message handlers
