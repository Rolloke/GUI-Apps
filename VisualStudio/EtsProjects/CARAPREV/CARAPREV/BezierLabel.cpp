// BezierLabel.cpp: Implementierung der Klasse CBezierLabel.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BezierLabel.h"
#include "PreviewFileHeader.h"

#ifdef ETSDEBUG_REPORT
 #define  ETSDEBUG_INCLUDE 
#endif
#include "CEtsDebug.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//#define PT_INIT 0x80
  
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CBezierLabel, CLabel,0);

CPen *CBezierLabel::gm_pPen = NULL;

CBezierLabel::CBezierLabel()
{
   #ifdef CHECK_LABEL_CONSTRUCTION
   REPORT("CBezierLabel::Constructor");
   #endif
   Init(&CPoint(0,0));
}

CBezierLabel::CBezierLabel(CPoint *p)
{ 
   #ifdef CHECK_LABEL_CONSTRUCTION
   REPORT("CBezierLabel::Constructor(arg)");
   #endif
   Init(p);
   SetDoublePicked(true);
}

void CBezierLabel::Init(CPoint *p)
{
   m_pP     = new CPoint[4];
   m_nCount = 4;
   for (int i=0; i<m_nCount; i++) m_pP[i]  = *p;
   if (!gm_pPen)
   {  
      LOGPEN lp = {PS_SOLID, {0, 0}, 0};
      gm_pPen = FindPen(&lp);
      ChangeGlobalObject(NULL, gm_pPen);
   }
   m_pPen = gm_pPen;
}

#ifdef _DEBUG
void CBezierLabel::AssertValid()  const
{
   CLabel::AssertValid();
   ASSERT(m_pPen!=NULL);
   ASSERT(AfxIsMemoryBlock(m_pPen, sizeof(CPen)));
}
#endif

bool CBezierLabel::Draw(CDC *pDC, int bAtr)
{
   if (!CLabel::Draw(pDC, bAtr)) return false;

   if (m_pP[0] == m_pP[3]) 
   {
      bool bdp = IsDoublePicked();
      SetDoublePicked(true);
      SetPoint(m_pP[0] + CPoint(gm_PickPointSize.cx*3, gm_PickPointSize.cy*3));
      SetDoublePicked(bdp);
   }

   if (bAtr) SelectPen(pDC, m_pPen);

   pDC->PolyBezier(m_pP, m_nCount);
   return true;
}

bool CBezierLabel::DrawShape(CDC* pDC,bool draw)
{
   if (!CLabel::DrawShape(pDC,draw)) return FALSE;
   pDC->SaveDC();
   pDC->SelectObject(CLabel::gm_pShapePen);
   if (DoDrawPickPoints())
   {
      pDC->MoveTo(m_pP[0]);                                    // Zeichnen der Verbindungslinien
      pDC->LineTo(m_pP[1]);                                    // zu den Definitionspunkten der 
      pDC->MoveTo(m_pP[3]);                                    // Bezierkurve
      pDC->LineTo(m_pP[2]);
   }
   pDC->PolyBezier(m_pP, m_nCount);
   pDC->RestoreDC(-1);
   return true;
}

void CBezierLabel::SetPoint(CPoint p)
{
   if (IsFixed()) return;
   if (IsDoublePicked())
   {
      SetChanged(TRUE);
      m_pP[3]   = p;
      m_pP[1].y = m_pP[0].y;
      m_pP[2].x = m_pP[3].x;
      m_pP[1].x = m_pP[3].x + MulDiv(m_pP[0].x - m_pP[3].x, 446, 1000);
      m_pP[2].y = m_pP[3].y + MulDiv(m_pP[0].y - m_pP[3].y, 548, 1000);
   }
   else  CLabel::SetPoint(p);
}

void CBezierLabel::SetLogPen(LOGPEN* plp)
{
   SetChanged(TRUE);
   m_pPen = FindPen(plp);
   if (gm_bSaveGlobal)
   {
      ChangeGlobalObject(gm_pPen, m_pPen);
      gm_pPen = m_pPen;
   }
}

LOGPEN CBezierLabel::GetLogPen() 
{
   LOGPEN  LogPen = {0, {0, 0}, 0};
   if (m_pPen) m_pPen->GetLogPen(&LogPen);
   return LogPen;
}

int CBezierLabel::IsOnLabel(CDC *pDC, CPoint *ppoint, CRect *prect)
{
   if (CLabel::IsOnLabel(pDC, ppoint, prect)>=1)
   {
      return HitCurve(ppoint, prect);
   }
   return -1;
}

void CBezierLabel::Serialize(CArchive &ar)
{
   #ifdef CHECK_LABEL_SERIALIZATION
   REPORT("CBezierLabel::Serialize()");
   #endif
   CLabel::Serialize(ar);
   long index = 0;
   if (ar.IsStoring())
   {
      index = FindPen(m_pPen);
      ASSERT(index >= 0);
      ArchiveWrite(ar, &index, sizeof(long));
   }
   else
   {
      ArchiveRead(ar, &index, sizeof(long));
      m_pPen = FindPen(index);
   }
}


CRect CBezierLabel::GetRect(CDC *pDC, UINT nFlags)
{
   CRect rect = CLabel::GetRect(pDC, nFlags&INFLATE_RECT);
   if (nFlags&INFLATE_RECT)
   {
      LOGPEN pen = {0,{0,0},0};
      if (m_pPen)
      {
         m_pPen->GetLogPen(&pen);
         rect.InflateRect(pen.lopnWidth.x, pen.lopnWidth.x);
      }
   }
   if ((nFlags&DEV_COORD) && pDC)
   {
      ::LPtoDP(pDC->m_hDC, (LPPOINT)&rect, 2);
      rect.NormalizeRect();
   }
   return rect;
}

void CBezierLabel::ResetGlobalObjects()
{
   gm_pPen = NULL;
}
