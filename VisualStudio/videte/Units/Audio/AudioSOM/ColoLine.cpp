// ColoLine.cpp : implementation file
//

#include "stdafx.h"
#include "AudioSOM.h"
#include "ColoLine.h"


// CColorLine

IMPLEMENT_DYNAMIC(CColorLine, CStatic)
CColorLine::CColorLine(CPen*pPen)
{
   m_pPen = pPen;
}

CColorLine::~CColorLine()
{
}


BEGIN_MESSAGE_MAP(CColorLine, CStatic)
   ON_WM_PAINT()
END_MESSAGE_MAP()



// CColorLine message handlers


void CColorLine::OnPaint()
{
    CRect rc;
    GetClientRect(&rc);
    CPaintDC dc(this); // device context for painting
    CPen * pOld = dc.SelectObject(m_pPen);
    rc.DeflateRect(1,1);
    dc.MoveTo(rc.TopLeft());
	dc.LineTo(rc.BottomRight());
    dc.SelectObject(pOld);
}
