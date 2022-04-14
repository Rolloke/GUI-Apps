// TesterView.cpp : implementation of the CTesterView class
//

#include "stdafx.h"
#include "Tester.h"

#include "TesterDoc.h"
#include "TesterView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTesterView

IMPLEMENT_DYNCREATE(CTesterView, CView)

BEGIN_MESSAGE_MAP(CTesterView, CView)
	//{{AFX_MSG_MAP(CTesterView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTesterView construction/destruction

CTesterView::CTesterView()
{
	// TODO: add construction code here

}

CTesterView::~CTesterView()
{
}

BOOL CTesterView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CTesterView drawing

void CTesterView::OnDraw(CDC* pDC)
{
	CTesterDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CTesterView printing

BOOL CTesterView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CTesterView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CTesterView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CTesterView diagnostics

#ifdef _DEBUG
void CTesterView::AssertValid() const
{
	CView::AssertValid();
}

void CTesterView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTesterDoc* CTesterView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTesterDoc)));
	return (CTesterDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTesterView message handlers
