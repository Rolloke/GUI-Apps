// PTConfigViewFoo.cpp : implementation of the CPTConfigViewFoo class
//

#include "stdafx.h"
#include "PTConfig.h"

#include "PTConfigDoc.h"
#include "PTConfigViewFoo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPTConfigViewFoo

IMPLEMENT_DYNCREATE(CPTConfigViewFoo, CView)

BEGIN_MESSAGE_MAP(CPTConfigViewFoo, CView)
	//{{AFX_MSG_MAP(CPTConfigViewFoo)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPTConfigViewFoo construction/destruction

CPTConfigViewFoo::CPTConfigViewFoo()
{
	// TODO: add construction code here

}

CPTConfigViewFoo::~CPTConfigViewFoo()
{
}

BOOL CPTConfigViewFoo::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CPTConfigViewFoo drawing

void CPTConfigViewFoo::OnDraw(CDC* pDC)
{
	CPTConfigDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CPTConfigViewFoo printing

BOOL CPTConfigViewFoo::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CPTConfigViewFoo::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CPTConfigViewFoo::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CPTConfigViewFoo diagnostics

#ifdef _DEBUG
void CPTConfigViewFoo::AssertValid() const
{
	CView::AssertValid();
}

void CPTConfigViewFoo::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPTConfigDoc* CPTConfigViewFoo::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPTConfigDoc)));
	return (CPTConfigDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPTConfigViewFoo message handlers
