// TargetCrossControl.cpp : implementation file
//

#include "stdafx.h"
#include "TestJoystick.h"
#include "TargetCrossControl.h"


/////////////////////////////////////////////////////////////////////////////////////////
// CTargetCrossControl
IMPLEMENT_DYNAMIC(CTargetCrossControl, CStatic)
CTargetCrossControl::CTargetCrossControl() :
	m_szCross(10, 10),
	m_ptHorzRange(-100, 100),
	m_ptVertRange(-100, 100),
	m_ptPos(0, 0)
{

	m_nZeroRange  = 0;
	m_cBkGndColor = RGB(255,255,255);	// white
	m_cCrossColor = RGB(0,0,0);			// black
	m_cZeroRangeColor = RGB(255,0,0);	// red
}
/////////////////////////////////////////////////////////////////////////////////////////
CTargetCrossControl::~CTargetCrossControl()
{
}


/////////////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CTargetCrossControl, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////////////////
// CTargetCrossControl message handlers
void CTargetCrossControl::OnPaint()
{
	CPaintDC dc(this);
	CRect rc;
	CSize szOld;
	GetClientRect(&rc);

	CPoint pt = TransformPoint(rc, m_ptPos);
	CSize  szCross(m_szCross.cx>>1, m_szCross.cy>>1);
	CPen penCross(PS_SOLID, 1, m_cCrossColor);
	CRgn rgn;

	dc.SaveDC();
	rgn.CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
	dc.SelectClipRgn(&rgn, RGN_COPY);

	dc.SelectObject(&penCross);
	dc.MoveTo(pt.x - szCross.cx, pt.y);
	dc.LineTo(pt.x + szCross.cx, pt.y);
	dc.MoveTo(pt.x, pt.y - szCross.cy);
	dc.LineTo(pt.x, pt.y + szCross.cy);

	if (m_nZeroRange)
	{
		CPen penZero(PS_SOLID, 1, m_cZeroRangeColor);
		dc.SelectObject(&penZero);
		dc.SelectStockObject(HOLLOW_BRUSH);
		pt = TransformPoint(rc, CPoint(0,0));
		int nX = MulDiv(m_nZeroRange, rc.Width(), m_ptHorzRange.y - m_ptHorzRange.x);
		int nY = MulDiv(m_nZeroRange, rc.Height(),m_ptVertRange.y - m_ptVertRange.x);
		rc.left   = pt.x - nX;
		rc.right  = pt.x + nX;
		rc.top    = pt.y - nY;
		rc.bottom = pt.y + nY;
		dc.Rectangle(&rc);
		dc.RestoreDC(-1);
	}
	else
	{
		dc.RestoreDC(-1);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CTargetCrossControl::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);
	pDC->FillSolidRect(&rc, m_cBkGndColor);
	return TRUE;
//	return CStatic::OnEraseBkgnd(pDC);
}
/////////////////////////////////////////////////////////////////////////////////////////
void CTargetCrossControl::SetCrossColor(COLORREF col)
{
	m_cCrossColor = col;
	InvalidateRect(NULL);
}
/////////////////////////////////////////////////////////////////////////////////////////
void CTargetCrossControl::SetZeroRangeColor(COLORREF col)
{
	m_cZeroRangeColor = col;
	InvalidateRect(NULL);
}
/////////////////////////////////////////////////////////////////////////////////////////
void CTargetCrossControl::SetBkGndColor(COLORREF col)
{
	m_cBkGndColor = col;
	InvalidateRect(NULL);
}
/////////////////////////////////////////////////////////////////////////////////////////
void CTargetCrossControl::SetHorzRange(CPoint ptHorz)
{
	SetHorzRange(ptHorz.x, ptHorz.y);
}
void CTargetCrossControl::SetHorzRange(int nX1, int nX2)
{
	m_ptHorzRange.x = nX1;
	m_ptHorzRange.y = nX2;
	InvalidateRect(NULL);
}
/////////////////////////////////////////////////////////////////////////////////////////
void CTargetCrossControl::SetVertRange(CPoint ptVert)
{
	SetVertRange(ptVert.x, ptVert.y);
}
void CTargetCrossControl::SetVertRange(int nY1, int nY2)
{
	m_ptVertRange.x = nY1;
	m_ptVertRange.y = nY2;
	InvalidateRect(NULL);
}
/////////////////////////////////////////////////////////////////////////////////////////
void CTargetCrossControl::SetZeroRange(int nRange)
{
	m_nZeroRange = nRange;
	InvalidateRect(NULL);
}
/////////////////////////////////////////////////////////////////////////////////////////
void CTargetCrossControl::SetPos(CPoint ptPos)
{
	SetPos(ptPos.x, ptPos.y);
}
void CTargetCrossControl::SetPos(int nX, int nY)
{
	m_ptPos.x = nX;
	m_ptPos.y = nY;
	InvalidateRect(NULL);
}
/////////////////////////////////////////////////////////////////////////////////////////
CPoint CTargetCrossControl::TransformPoint(CRect& rc, CPoint pt)
{
	pt.x = (rc.Width()  >> 1) + MulDiv(pt.x, rc.Width(), m_ptHorzRange.y - m_ptHorzRange.x);
	pt.y = (rc.Height() >> 1) + MulDiv(pt.y, rc.Height(), m_ptVertRange.y - m_ptVertRange.x);
	return pt;
}