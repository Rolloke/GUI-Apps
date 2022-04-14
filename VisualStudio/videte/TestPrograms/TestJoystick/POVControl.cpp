// POVControl.cpp : implementation file
//

#include "stdafx.h"
#include "TestJoystick.h"
#include "POVControl.h"


#define  _USE_MATH_DEFINES
#include <math.h>


// CPOVControl

IMPLEMENT_DYNAMIC(CPOVControl, CStatic)

CPOVControl::CPOVControl()
{
	m_cBkGndColor = RGB(255,255,255);	// white
	m_cPointColor = RGB(0,0,0);			// black
	m_lAngle = -1;
}

CPOVControl::~CPOVControl()
{
}


BEGIN_MESSAGE_MAP(CPOVControl, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CPOVControl message handlers

void CPOVControl::SetBkGndColor(COLORREF col)
{
	m_cBkGndColor = col;
	InvalidateRect(NULL);
}
void CPOVControl::SetPointColor(COLORREF col)
{
	m_cPointColor= col;
	InvalidateRect(NULL);
}
void CPOVControl::SetAngle(long lAngle)
{
	m_lAngle = lAngle;
	InvalidateRect(NULL);
}

void CPOVControl::OnPaint()
{
	CPaintDC dc(this);
	CRect rc, rcBall(-3, -3, 3, 3);
	CPoint ptCenter;
	CSize szOld;
	GetClientRect(&rc);
	ptCenter = rc.CenterPoint();
	rc.DeflateRect(4,4);
	CPen pen(PS_SOLID, 1, m_cPointColor);
	CBrush brush(m_cPointColor);

	dc.SelectObject(&pen);
	dc.SelectStockObject(HOLLOW_BRUSH);
	
	dc.Ellipse(&rc);
	dc.SelectObject(&brush);

	if (m_lAngle != -1)
	{
		CPoint pt;
		double dAngle = 0.02* M_PI * m_lAngle / 360;
		pt.x = (long)(sin(dAngle) * (rc.Width() >> 1));
		pt.y = -(long)(cos(dAngle) * (rc.Height() >> 1));
		rcBall.OffsetRect(ptCenter+pt);
	}
	else
	{
		rcBall.OffsetRect(ptCenter);
	}
	dc.Ellipse(&rcBall);

	dc.RestoreDC(-1);
}
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CPOVControl::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);
	pDC->FillSolidRect(&rc, m_cBkGndColor);
	return TRUE;
//	return CStatic::OnEraseBkgnd(pDC);
}
