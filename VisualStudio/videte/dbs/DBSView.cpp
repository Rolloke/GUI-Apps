// DBSView.cpp : implementation of the CDBSView class
//

#include "stdafx.h"
#include "DBS.h"

#include "DBSDoc.h"
#include "DBSView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDBSView

IMPLEMENT_DYNCREATE(CDBSView, CListView)

BEGIN_MESSAGE_MAP(CDBSView, CListView)
	//{{AFX_MSG_MAP(CDBSView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CListView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDBSView construction/destruction

CDBSView::CDBSView()
{
	// TODO: add construction code here

}

CDBSView::~CDBSView()
{
}

BOOL CDBSView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style &= ~LVS_ICON;
	cs.style |= LVS_LIST;

	return CListView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CDBSView drawing

void CDBSView::OnDraw(CDC* pDC)
{
	// nothing todo it's a list view
}

void CDBSView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	// TODO: You may populate your ListView with items by directly accessing
	//  its list control through a call to GetListCtrl().
}

/////////////////////////////////////////////////////////////////////////////
// CDBSView printing

BOOL CDBSView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CDBSView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CDBSView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CDBSView diagnostics

#ifdef _DEBUG
void CDBSView::AssertValid() const
{
	CListView::AssertValid();
}

void CDBSView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CDBSDoc* CDBSView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDBSDoc)));
	return (CDBSDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDBSView message handlers
void CDBSView::Clear()
{
	GetListCtrl().DeleteAllItems();
}
/////////////////////////////////////////////////////////////////////////////
void CDBSView::Add(const CString& s)
{
	GetListCtrl().InsertItem(GetListCtrl().GetItemCount(),s);
}
