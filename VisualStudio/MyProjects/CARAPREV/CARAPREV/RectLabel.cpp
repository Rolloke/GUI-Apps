// RectLabel.cpp: Implementierung der Klasse CRectLabel.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RectLabel.h"
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

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CRectLabel, CLabel,1);

CPen   *CRectLabel::gm_pPen   = NULL;
CBrush *CRectLabel::gm_pBrush = NULL;
COLORREF CRectLabel::gm_BKColor = RGB(255,255,255);

CRectLabel::CRectLabel()
{
   #ifdef CHECK_LABEL_CONSTRUCTION
   REPORT("CRectLabel::Constructor(arg)");
   #endif
   Init(&CPoint(0,0));
}


CRectLabel::CRectLabel(CPoint *p)
{ 
   #ifdef CHECK_LABEL_CONSTRUCTION
   REPORT("CRectLabel::Constructor(arg)");
   #endif
   Init(p);
   m_nAct = 1;
}
void CRectLabel::Init(CPoint *p)
{
   m_State.scaleable = true;   //   Seitenverhältnis wird angehängt
   int nCount        = 2;
   m_nCount          = nCount;
   if (m_State.scaleable) nCount++;
   m_pP              = new CPoint[nCount];
   m_pP[m_nCount]    = CPoint(0, 0);
   m_pP[0]   = *p;
   m_pP[1]   = *p;

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
void CRectLabel::AssertValid() const
{
   CLabel::AssertValid();
   ASSERT(m_pPen != NULL);
   ASSERT(AfxIsMemoryBlock(m_pPen, sizeof(CPen)));
}
#endif

bool CRectLabel::Draw(CDC * pDC, int bAtr)
{
   if (!CLabel::Draw(pDC, bAtr)) return false;
   if (m_pP[0] == m_pP[1]) m_pP[1] += gm_PickPointSize;

   if (bAtr)
   {
      SelectPen(pDC, m_pPen);
      SelectBrush(pDC, m_pBrush);
   }
   CLabel::Rectangle(pDC, m_pP[0].x, m_pP[0].y, m_pP[1].x, m_pP[1].y, (bAtr!=0) ? m_State.drawbackgnd : true);
   return true;
}

bool CRectLabel::DrawShape(CDC * pDC, bool draw)
{
   if (!CLabel::DrawShape(pDC, draw)) return false;
   pDC->SaveDC();
   pDC->SelectObject(CLabel::gm_pShapePen);
   CLabel::Rectangle(pDC, m_pP[0].x, m_pP[0].y, m_pP[1].x, m_pP[1].y, false);
   pDC->RestoreDC(-1);
   return true;
}
void CRectLabel::SetRect(CRect &rc)
{
   if (m_pP)
   {
      SetChanged(TRUE);
      m_pP[0] = rc.TopLeft();
      m_pP[1] = rc.BottomRight();
   }
}

void CRectLabel::SetLogPen(LOGPEN* plp) 
{
   SetChanged(TRUE);
   m_pPen = CLabel::FindPen(plp);
   if (gm_bSaveGlobal)
   {
      ChangeGlobalObject(gm_pPen, m_pPen);
      gm_pPen = m_pPen;
   }
}

LOGPEN CRectLabel::GetLogPen() 
{
   LOGPEN  LogPen ={0, {0, 0}, 0};
   if (m_pPen) m_pPen->GetLogPen(&LogPen);
   return LogPen;
}

void CRectLabel::SetLogBrush(LOGBRUSH* plb) 
{
   SetChanged(TRUE);
   m_pBrush  = FindBrush(plb);
   if (gm_bSaveGlobal)
   {
      ChangeGlobalObject(gm_pBrush, m_pBrush);
      gm_pBrush  = m_pBrush;
      gm_BKColor = m_BkColor;
   }
   m_State.drawbackgnd = (m_pBrush == NULL) ? false : true;
}

LOGBRUSH CRectLabel::GetLogBrush() 
{
   LOGBRUSH  LogBrush ={BS_HOLLOW, 0, 0};
   if (m_pBrush) m_pBrush->GetLogBrush(&LogBrush);
   return LogBrush;
}

void CRectLabel::Serialize(CArchive &ar)
{
   #ifdef CHECK_LABEL_SERIALIZATION
   REPORT("CRectLabel::Serialize()");
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


CRect CRectLabel::GetRect(CDC * pDC, UINT nFlags)
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

void CRectLabel::ResetGlobalObjects()
{
   gm_pBrush = NULL;
   gm_pPen   = NULL;
}
