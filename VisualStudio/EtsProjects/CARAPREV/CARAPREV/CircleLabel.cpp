// CircleLabel.cpp: Implementierung der Klasse CCircleLabel.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CircleLabel.h"

#ifdef ETSDEBUG_REPORT
 #define  ETSDEBUG_INCLUDE 
#endif
#include "CEtsDebug.h"

#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CCircleLabel, CLabel,0);

CPen    *CCircleLabel::gm_pPen    = NULL;
CBrush  *CCircleLabel::gm_pBrush  = NULL;
COLORREF CCircleLabel::gm_BKColor = RGB(255,255,255);
 
CCircleLabel::CCircleLabel()
{
   #ifdef CHECK_LABEL_CONSTRUCTION
   REPORT("CCircleLabel::Constructor");
   #endif
   Init(&CPoint(0,0), 15);
}

CCircleLabel::CCircleLabel(CPoint *p)
{ 
   #ifdef CHECK_LABEL_CONSTRUCTION
   REPORT("CCircleLabel::Constructor(arg)");
   #endif
   Init(p, 15);
   SetDoublePicked(true);
}

CCircleLabel::CCircleLabel(CRect *pr)
{
   #ifdef CHECK_LABEL_CONSTRUCTION
   REPORT("CCircleLabel::Constructor(arg)");
   #endif
   Init(&pr->TopLeft(), 2);
   m_pP[1] = pr->BottomRight();
}

void CCircleLabel::Init(CPoint *p, int nP)
{
   m_pP     = new CPoint[nP];
   m_nCount = nP;
   for (int i=0; i<m_nCount; i++) m_pP[i]  = *p;
   m_BkColor = gm_BKColor;

   if (!gm_pPen)
   {  
      LOGPEN lp = {PS_SOLID, {0, 0}, 0};
      gm_pPen = FindPen(&lp);
      ChangeGlobalObject(NULL, gm_pPen);
   }
   m_pPen = gm_pPen;

   m_pBrush = gm_pBrush;
   m_State.drawbackgnd = (m_pBrush == NULL) ? false : true;
}

#ifdef _DEBUG
void CCircleLabel::AssertValid() const
{
   CLabel::AssertValid();
   ASSERT(m_pPen != NULL);
   ASSERT(AfxIsMemoryBlock(m_pPen, sizeof(CPen)));
}
#endif

bool CCircleLabel::Draw(CDC * pDC, int bAtr)
{
   if (!CLabel::Draw(pDC, bAtr)) return false;

   if (m_nCount >= 12)
   {
      m_pP[12] = m_pP[0];
   }

   if (bAtr&0x01)
   {
      SelectPen(pDC, m_pPen);
      SelectBrush(pDC, m_pBrush);
   }

   if (m_nCount == 2)
   {
      if (m_pP[0] == m_pP[1]) m_pP[1] += gm_PickPointSize;
      pDC->Ellipse(m_pP[0].x, m_pP[0].y, m_pP[1].x, m_pP[1].y);
   }
   else
   {
      if ((abs(m_pP[13].x - m_pP[14].x) <=gm_PickPointSize.cx)||(abs(m_pP[13].y - m_pP[14].y) <= gm_PickPointSize.cy))
      {
         bool bdp = IsDoublePicked();
         SetDoublePicked(true);
         SetPoint(m_pP[13] + CPoint(gm_PickPointSize.cx*6, gm_PickPointSize.cy*6));
         SetDoublePicked(bdp);
      }
      if ((bAtr&NO_PATH)==0)
      {
         pDC->BeginPath();
      }

      pDC->PolyBezier(m_pP, m_nCount-2);

      if ((bAtr&NO_PATH)==0)
      {
         if (pDC->EndPath())
         {
            if (gm_bFillPath)
               pDC->StrokeAndFillPath();
            else
               pDC->StrokePath();
         }
      }
   }
   return true;
}

bool CCircleLabel::DrawShape(CDC * pDC, bool draw)
{
   int i;
   if (!CLabel::DrawShape(pDC, draw)) return FALSE;
   if (m_nCount >= 12) m_pP[12] = m_pP[0];
   pDC->SaveDC();
   if (m_nCount == 2)
   {
      pDC->SelectStockObject(HOLLOW_BRUSH);

      pDC->SelectObject(gm_pShapePen);
      pDC->Ellipse(m_pP[0].x, m_pP[0].y, m_pP[1].x, m_pP[1].y);
   }
   else
   {
      pDC->SelectObject(gm_pShapePen);
      pDC->PolyBezier(m_pP, m_nCount-2);                       // Zeichnen des eigentlichen Umrisses

      if (DoDrawPickPoints())
      {                                                        // von der Basisklasse gezeichnet
         CRect  r(m_pP[12],m_pP[12]);
         r.InflateRect(CLabel::gm_PickPointSize);
         pDC->PatBlt(r.left,r.top,r.right-r.left,r.bottom-r.top,PATINVERT);

         pDC->MoveTo(m_pP[11]);                                // Zeichnen der Verbindungslinien
         pDC->LineTo(m_pP[ 0]);                                // zu den Definitionspunkten der
         pDC->LineTo(m_pP[ 1]);                                // Bezierkurve
         for (i=0; i<3; i++)
         {
            pDC->MoveTo(m_pP[3*i+2]);
            pDC->LineTo(m_pP[3*i+3]);
            pDC->LineTo(m_pP[3*i+4]);
         }
      }
   }
   pDC->RestoreDC(-1);

   return TRUE;
}

void CCircleLabel::SetPoint(CPoint p)
{
   if (IsFixed()) return;
   if (m_nCount == 2)
   {
      if (IsDoublePicked()) CLabel::SetPoint(p, 1);
      else                  CLabel::SetPoint(p);
   }
   else
   {
      bool bSetScalePoints = true;
      if (IsDoublePicked())
      {
         m_pP[14] = p;
         if (m_pP[13] != m_pP[14]) SetChanged(TRUE);
         m_pP[ 8].x = m_pP[ 9].x = m_pP[10].x = m_pP[13].x;     // links
         m_pP[ 0].y = m_pP[ 1].y = m_pP[11].y = m_pP[13].y;     // oben
         m_pP[ 2].x = m_pP[ 3].x = m_pP[ 4].x = p.x;            // rechts
         m_pP[ 5].y = m_pP[ 6].y = m_pP[ 7].y = p.y;            // unten

         m_pP[ 0].x = m_pP[ 6].x =(m_pP[13].x + p.x) >> 1;      // 1/2 x
         m_pP[ 3].y = m_pP[ 9].y =(m_pP[13].y + p.y) >> 1;      // 1/2 y

         m_pP[ 7].x = m_pP[11].x = m_pP[13].x + MulDiv(m_pP[0].x - m_pP[13].x, 446, 1000);
         m_pP[ 1].x = m_pP[ 5].x = m_pP[ 0].x + MulDiv(p.x       - m_pP[ 0].x, 548, 1000);
         m_pP[ 2].y = m_pP[10].y = m_pP[13].y + MulDiv(m_pP[9].y - m_pP[13].y, 446, 1000);
         m_pP[ 4].y = m_pP[ 8].y = m_pP[ 9].y + MulDiv(p.y       - m_pP[ 9].y, 548, 1000);
         bSetScalePoints = false;
      }
      else if (m_nAct == 0)
      {
         CLabel::SetPoint(p,  0);
         CLabel::SetPoint(p, 12);
      }
      else if ((m_nAct == 13)||(m_nAct == 14))
      {
         CRect  rcDef(m_pP[13],m_pP[14]);
         CPoint ptCenter = rcDef.CenterPoint();
         CSize szOld(m_pP[14].x-m_pP[13].x, m_pP[14].y-m_pP[13].y);
         CSize szNew;

         if (m_nAct == 13)
         {
            rcDef.left = p.x;
            rcDef.top  = p.y;
            szNew.cx   = m_pP[14].x - p.x;
            szNew.cy   = m_pP[14].y - p.y;
         }
         else
         {
            rcDef.right  = p.x;
            rcDef.bottom = p.y;
            szNew.cx     = p.x - m_pP[13].x;
            szNew.cy     = p.y - m_pP[13].y;
         }
         if ((abs(szNew.cx) > 15)&&(abs(szNew.cy) > 15))
         {
            CLabel::MoveLabel(-ptCenter);
            CLabel::ScaleLabel(szNew, szOld);
            ptCenter = rcDef.CenterPoint();
            CLabel::MoveLabel(ptCenter);
            m_pP[13] = rcDef.TopLeft();
            m_pP[14] = rcDef.BottomRight();
         }
         bSetScalePoints = false;
      }
      else CLabel::SetPoint(p);
      if (bSetScalePoints)
      {
         m_nCount -= 2;             // Umgebendes Rechteck ohne die 
         CRect rc = GetRect(NULL);  // Skalierungspunke (13, 14)
         m_nCount += 2;             // bestimmen
         m_pP[13] = rc.TopLeft();
         m_pP[14] = rc.BottomRight();
      }
   }
}

void CCircleLabel::SetLogPen(LOGPEN* plp) 
{
   SetChanged(TRUE);
   m_pPen = CLabel::FindPen(plp);
   if (gm_bSaveGlobal)
   {
      ChangeGlobalObject(gm_pPen, m_pPen);
      gm_pPen = m_pPen;
   }
}

LOGPEN CCircleLabel::GetLogPen() 
{
   LOGPEN  LogPen ={0, {0, 0}, 0};
   if (m_pPen) m_pPen->GetLogPen(&LogPen);
   return LogPen;
}

void CCircleLabel::SetLogBrush(LOGBRUSH* plb) 
{
   SetChanged(TRUE);
   m_pBrush  = CLabel::FindBrush(plb);
   m_State.drawbackgnd = (m_pBrush == NULL) ? false : true;
   if (gm_bSaveGlobal)
   {
      ChangeGlobalObject(gm_pBrush, m_pBrush);
      gm_pBrush = m_pBrush;
      gm_BKColor = m_BkColor;
   }
}

LOGBRUSH CCircleLabel::GetLogBrush() 
{
   LOGBRUSH  LogBrush ={BS_HOLLOW, 0, 0};
   if (m_pBrush) m_pBrush->GetLogBrush(&LogBrush);
   return LogBrush;
}

bool CCircleLabel::GetRgn(int n_cm, CDC * pDC, CRgn *prgn, UINT nFlags)
{
   if (!prgn)           return false;
   if (((nFlags&INFLATE_RECT)==0) && pDC)
   {
      if (pDC->BeginPath())
      {
         if (m_nCount == 2) pDC->Ellipse(m_pP[0].x, m_pP[0].y, m_pP[1].x, m_pP[1].y);
         else               pDC->PolyBezier(m_pP, m_nCount-2);
         if (pDC->EndPath())
         {
            if (n_cm == RGN_COPY) prgn->DeleteObject();
            if (HRGN(*prgn) == NULL) 
            {
               if (prgn->CreateFromPath(pDC)) return true;
            }
            else
            {
               CRgn  rgn;
               if (rgn.CreateFromPath(pDC))
               {
                  if (prgn->CombineRgn(prgn, &rgn, n_cm&0x0f) != ERROR) return true;
               }
            }
         }
      }
   }
   return CLabel::GetRgn(n_cm, pDC, prgn, nFlags);
}

int CCircleLabel::IsOnLabel(CDC *pDC, CPoint *ppoint, CRect *prect)
{
   if (CLabel::IsOnLabel(pDC, ppoint, prect) != -1)
   {
      int nDist = -1;
      if (m_nCount == 2)                                       //  Ellipse
      {
         int na = abs(m_pP[0].x - m_pP[1].x) >> 1,
             nb = abs(m_pP[0].y - m_pP[1].y) >> 1,
             nx, ny;
         double dy;
         na *= na;
         nb *= nb;
         if (ppoint)
         {
            nx = abs(((m_pP[0].x + m_pP[1].x) >> 1) - ppoint->x);
            ny = abs(((m_pP[0].y + m_pP[1].y) >> 1) - ppoint->y);
            dy = (1.0 - (double)(nx * nx) / (double)na) * (double)(nb);
            dy = sqrt(dy);
            nDist = ny - (int)dy;
            if      (nDist <= -CLabel::MaxPickDistance()) return CLabel::IsInside();
            else if (nDist >=  abs(gm_PickPointSize.cy))  nDist = CLabel::IsOutside();
            else                  return abs(nDist);
         }
         if (prect)
         {
            CPoint ptM((m_pP[0].x + m_pP[1].x) >> 1, (m_pP[0].y + m_pP[1].y) >> 1);
            CRect rect = GetRect(NULL);
            POINT ptPoly[4] = 
            {
               {ptM.x, rect.top},
               {rect.right, ptM.y},
               {ptM.x, rect.bottom},
               {rect.left, ptM.y}
            };
            return HitPolygon((CPoint*)&ptPoly, -4, ppoint, prect);
         }
         return CLabel::IsOutside();
      }
      else
      {
         nDist = HitCurve(ppoint, prect);
         if (nDist != CLabel::IsOutside()) return nDist;
         else
         {
            CRgn rgn;
            if (GetRgn(RGN_COPY, pDC, &rgn))
            {
               if (ppoint)
               {
                  CPoint point(*ppoint);
                  ::LPtoDP(pDC->m_hDC, &point, 1);
                  if (rgn.PtInRegion(point))
                  {
                     return CLabel::IsInside();
                  }
               }
               if (prect)
               {
                  CRect rect(*prect);
                  ::LPtoDP(pDC->m_hDC, (LPPOINT)&rect, 2);
                  if (rgn.RectInRegion(rect )) return CLabel::IsInside();
               }
            }
         }
      }
   }
   return CLabel::IsOutside();
}

void CCircleLabel::Serialize(CArchive &ar)
{
   #ifdef CHECK_LABEL_SERIALIZATION
   REPORT("CCircleLabel::Serialize()");
   #endif
   CLabel::Serialize(ar);
   long index = 0;
   if (ar.IsStoring())
   {
      index = FindPen(m_pPen);
      ASSERT(index >= 0);
      ArchiveWrite(ar, &index, sizeof(long));
      index = FindBrush(m_pBrush);
      ArchiveWrite(ar, &index, sizeof(long));
      ArchiveWrite(ar, &m_BkColor, sizeof(COLORREF));
   }
   else
   {
      ArchiveRead(ar, &index, sizeof(long));
      m_pPen = FindPen(index);
      ArchiveRead(ar, &index, sizeof(long));
      m_pBrush = FindBrush(index);
      ArchiveRead(ar, &m_BkColor, sizeof(COLORREF));
   }
}

CRect CCircleLabel::GetRect(CDC * pDC, UINT nFlags)
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

void CCircleLabel::ResetGlobalObjects()
{
   gm_pBrush = NULL;
   gm_pPen   = NULL;
}
