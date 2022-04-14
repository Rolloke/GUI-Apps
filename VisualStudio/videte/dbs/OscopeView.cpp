// OscopeView.cpp : implementation file
//

#include "stdafx.h"
#include "DBS.h"
#include "OscopeView.h"

#include "wkclasses\OScopeCtrl.h"
#include ".\oscopeview.h"

/////////////////////////////////////////////////////////////////////////////
// COscopeView
IMPLEMENT_DYNCREATE(COscopeView, CView)
/////////////////////////////////////////////////////////////////////////////
COscopeView::COscopeView()
{
	m_pScope1 = NULL;
	m_pScope2 = NULL;
}
/////////////////////////////////////////////////////////////////////////////
COscopeView::~COscopeView()
{
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(COscopeView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COscopeView drawing
void COscopeView::OnDraw(CDC* pDC)
{
//	CString sText = _T("COscopeView");
//	pDC->TextOut(100,100, sText);
}

/////////////////////////////////////////////////////////////////////////////
// COscopeView diagnostics
#ifdef _DEBUG
void COscopeView::AssertValid() const
{
	CView::AssertValid();
}

void COscopeView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// COscopeView message handlers
void COscopeView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}
/////////////////////////////////////////////////////////////////////////////
int COscopeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void COscopeView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	if (m_pScope1 && m_pScope2)
	{
		CWK_Profile wkp;
		m_pScope1->SetRange(0.0, (double)wkp.GetInt(SECTION_DBS, _T("RangeMaxFps"), 1000), 1);
		m_pScope1->Reset();
		m_pScope2->SetRange(0.0, (double)wkp.GetInt(SECTION_DBS, _T("RangeMaxMBps"), 10), 1);
		m_pScope2->Reset();
	}
}
/////////////////////////////////////////////////////////////////////////////
void COscopeView::InitScopeCtrls()
{
	WK_DELETE(m_pScope1);
	WK_DELETE(m_pScope2);
	CRect rc;
	GetClientRect(&rc);
	rc.bottom /= 2;
	m_pScope1 = new COScopeCtrl(1);
	m_pScope1->Create(WS_CHILD, rc, this, AFX_IDW_PANE_FIRST);
	m_pScope1->SetXUnits(_T("Interval")) ;
	m_pScope1->SetYUnits(_T("Frames / s")) ;
	m_pScope1->ShowWindow(SW_SHOW);
	rc.OffsetRect(0, rc.Height());
	m_pScope2 = new COScopeCtrl(1);
	m_pScope2->Create(WS_CHILD, rc, this, AFX_IDW_PANE_FIRST);
	m_pScope2->SetXUnits(_T("Interval")) ;
	m_pScope2->SetYUnits(_T("MB / s")) ;
	m_pScope2->ShowWindow(SW_SHOW);
}
/////////////////////////////////////////////////////////////////////////////
void COscopeView::OnTimer(UINT nIDEvent)
{
	CView::OnTimer(nIDEvent);
}
/////////////////////////////////////////////////////////////////////////////
void COscopeView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (bShow && !m_pScope1)
	{
		InitScopeCtrls();
	}
	CView::OnShowWindow(bShow, nStatus);
}
/////////////////////////////////////////////////////////////////////////////
BOOL COscopeView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style &= ~WS_VISIBLE;
	return CView::PreCreateWindow(cs);
}
/////////////////////////////////////////////////////////////////////////////
void COscopeView::AppendValue(int nType, double dVal, BOOL bUpdate)
{
	if (m_pScope1 && nType == 0)
	{
		m_pScope1->AppendPoint(dVal, 0, bUpdate);
	}
	if (m_pScope2 && nType == 1)
	{
		m_pScope2->AppendPoint(dVal, 0, bUpdate);
	}
}
/////////////////////////////////////////////////////////////////////////////
void COscopeView::OnDestroy()
{
	WK_DELETE(m_pScope1);
	WK_DELETE(m_pScope2);
	CView::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
