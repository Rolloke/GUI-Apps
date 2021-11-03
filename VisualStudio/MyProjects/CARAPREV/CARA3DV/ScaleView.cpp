// ScaleView.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "CARA3DV.h"
#include "ScaleView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScaleView
CScaleView::CScaleView()
{
   m_bValidSize = false;
}



BEGIN_MESSAGE_MAP(CScaleView, CStatic)
	//{{AFX_MSG_MAP(CScaleView)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CScaleView 

BOOL CScaleView::OnEraseBkgnd(CDC* pDC) 
{
   CBrush brush;
   brush.CreateSysColorBrush(COLOR_BTNFACE);
   CRect rcClient;
   GetClientRect(&rcClient);
   pDC->FillRect(&rcClient, &brush);
   return true;
}

void CScaleView::OnPaint() 
{
   CPaintDC dc(this); // device context for painting
   if (m_bValidSize)
   {
      CRect    rcClient;
      GetClientRect(&rcClient);
      m_ScalePainter.Draw(&dc, &rcClient);
   }
}

CSize CScaleView::GetMinSize()
{
   CDC *pDC = GetDC();
   CSize szMin = m_ScalePainter.GetMinSize(pDC);
   ReleaseDC(pDC);
   m_bValidSize = true;
   return szMin;
}
