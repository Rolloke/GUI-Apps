// JpegView.cpp : implementation file
//

#include "stdafx.h"
#include "h263play.h"

#include "JpegDoc.h"
#include "JpegView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJpegView

IMPLEMENT_DYNCREATE(CJpegView, CView)

CJpegView::CJpegView()
{
	m_BufferID = 0;
	m_ViewID = 0;
}

CJpegView::~CJpegView()
{
}


BEGIN_MESSAGE_MAP(CJpegView, CView)
	//{{AFX_MSG_MAP(CJpegView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJpegView drawing

void CJpegView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CJpegView diagnostics

#ifdef _DEBUG
void CJpegView::AssertValid() const
{
	CView::AssertValid();
}

void CJpegView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
CJpegDoc* CJpegView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CJpegDoc)));
	return (CJpegDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CJpegView message handlers

void CJpegView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if (m_BufferID==0)
	{
		OnInitialUpdate();
	}
	CJpegDoc* pDoc = GetDocument();
	CSize s;
	s = pDoc->GetJpegSize();
	CRect vr(0,0,s.cx,s.cy);
	CRect mr;
	GetParent()->GetClientRect(mr);
	mr.right = mr.left + s.cx;
	mr.bottom = mr.top + s.cy;
	GetParent()->MoveWindow(mr,TRUE);
	MoveWindow(vr,TRUE);

	IM_LoadData(m_BufferID,	
				pDoc->GetBufferLength(),
				pDoc->GetBuffer());
}

void CJpegView::OnInitialUpdate() 
{
	m_BufferID = IM_GetUniqueBuf();
	m_ViewID = IM_GetUniqueView();
	IM_DefineView(m_hWnd, 
					m_ViewID,
					m_BufferID,
					0,	//long locx, 
					0,	//long locy, 
					0,	//long sizex, 
					0,	//long sizey,
					0);
	IM_SetViewOptions(m_ViewID, SCALE_OFF + GROW_FIT);
}
