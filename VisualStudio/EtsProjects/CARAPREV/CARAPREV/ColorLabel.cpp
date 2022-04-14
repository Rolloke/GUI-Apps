// ColorLabel.cpp: Implementierung der Klasse CColorLabel.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ColorLabel.h"
#include "ColorArray.h"
#include "PreviewFileHeader.h"
#include "TextLabel.h"

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
IMPLEMENT_SERIAL(CColorLabel, CRectLabel, 1);

CColorLabel::CColorLabel()
{
   m_pColorArray = NULL;
   m_bShowScale  = true;
   CTextLabel Text;
   m_ScalePainter.m_LogFont = Text.GetLogFont();
   m_ScalePainter.m_Div.stepwidth = 0;
}

CColorLabel::~CColorLabel()
{
   if (m_pColorArray) delete m_pColorArray;
}

#ifdef _DEBUG
void CColorLabel::AssertValid() const
{
   CRectLabel::AssertValid();
}
#endif

bool CColorLabel::Attach(CColorArray *pCA)
{
   if (m_pColorArray == NULL)
   {
      m_pColorArray = pCA;
      return true;
   }
   return false;
}

CColorArray * CColorLabel::Detach()
{
   CColorArray *pTemp = m_pColorArray;
   m_pColorArray = NULL;
   return pTemp;
}

bool CColorLabel::Draw(CDC * pDC, int bAtr)
{
   if (!CLabel::Draw(pDC, bAtr)) return false;
   CRect rc = CLabel::GetRect(pDC);

   bool  bNegY = IsNegativeY(pDC);
   if (bNegY) swap(rc.top, rc.bottom);
   CSize szView  = rc.Size();
   szView.cx = abs(szView.cx);
   szView.cy = abs(szView.cy);
   CSize szScale = m_ScalePainter.GetMinSize(pDC);
   CSize szColor = CSize(szView.cx-szScale.cx, szView.cy-szScale.cy);
   if (m_bShowScale && (szColor.cx > 0) && (szColor.cy > 0))
   {
      int   nStepWidth;
      CRect rcColor, rcScale;
      m_ScalePainter.m_bHorz = (szColor.cx > szColor.cy) ? true : false;
      if (bNegY)
      {
         szScale.cy = - szScale.cy; 
         szView.cy  = - szView.cy;
      }

      if (m_ScalePainter.m_bHorz)
      {
         rcColor.left   = rcScale.left    = rc.left;
         rcColor.right  = rcScale.right   = rcColor.left   + szView.cx;
         rcColor.top                      = rc.top;
         rcScale.bottom                   = rcColor.top    + szView.cy;
         rcScale.top    = rcColor.bottom  = rcScale.bottom - szScale.cy;
         nStepWidth     = MulDiv(szScale.cx, 3, 2);
      }
      else                    
      {
         rcColor.top    = rcScale.top     = rc.top;
         rcColor.bottom = rcScale.bottom  = rcColor.top   + szView.cy;
         rcColor.left                     = rc.left;
         rcScale.right                    = rcColor.left  + szView.cx;
         rcScale.left   = rcColor.right   = rcScale.right - szScale.cx;
         nStepWidth     = abs(MulDiv(szScale.cy, 3, 2));
      }
      if (m_ScalePainter.m_Div.stepwidth < nStepWidth) m_ScalePainter.m_Div.stepwidth = nStepWidth;
      if (bNegY)
      {
         swap(rcColor.top, rcColor.bottom);
      }
      m_ScalePainter.Draw(pDC, &rcScale);
      if (m_pColorArray)
      {
         m_pColorArray->Draw(pDC, &rcColor);
      }
   }
   else
   {
      if (bNegY) swap(rc.top, rc.bottom);
      m_pColorArray->Draw(pDC, &rc);
   }
   return true;
}

void CColorLabel::Serialize(CArchive &ar)
{
   #ifdef CHECK_LABEL_SERIALIZATION
   REPORT("CColorLabel::Serialize()");
   #endif
   CRectLabel::Serialize(ar);
   long index = 0;
   if (ar.IsStoring())
   {
      long nCount = 0;
      if (m_pColorArray)
      {
         nCount = m_pColorArray->GetNumColor();
      }
      CLabel::ArchiveWrite(ar, &nCount, sizeof(long));
      if (nCount>0)CLabel::ArchiveWrite(ar, m_pColorArray->GetColorBuffer(), sizeof(COLORREF)*nCount);
      CLabel::ArchiveWrite(ar, &m_ScalePainter.m_TextColor, sizeof(COLORREF));
      CLabel::ArchiveWrite(ar, &m_ScalePainter.m_LogFont, sizeof(LOGFONT));
      CLabel::ArchiveWrite(ar, &m_ScalePainter.m_Div.stepwidth, sizeof(m_ScalePainter.m_Div.stepwidth));
      CLabel::ArchiveWrite(ar, &m_ScalePainter.m_Ntx.nmode, sizeof(m_ScalePainter.m_Ntx.nmode));
      CLabel::ArchiveWrite(ar, &m_ScalePainter.m_Ntx.nround, sizeof(m_ScalePainter.m_Ntx.nround));
   }
   else
   {
      long nCount = 0;
      CLabel::ArchiveRead(ar, &nCount, sizeof(long));
      if (nCount > 0)
      {
         if (!m_pColorArray) m_pColorArray = new CColorArray;
         if (m_pColorArray)
         {
            m_pColorArray->SetArraySize(nCount);
            CLabel::ArchiveRead(ar, (void*)m_pColorArray->GetColorBuffer(), sizeof(COLORREF)*nCount);
         }
      }
      CLabel::ArchiveRead(ar, &m_ScalePainter.m_TextColor, sizeof(COLORREF));
      CLabel::ArchiveRead(ar, &m_ScalePainter.m_LogFont, sizeof(LOGFONT));
      CLabel::ArchiveRead(ar, &m_ScalePainter.m_Div.stepwidth, sizeof(m_ScalePainter.m_Div.stepwidth));
      CLabel::ArchiveRead(ar, &m_ScalePainter.m_Ntx.nmode, sizeof(m_ScalePainter.m_Ntx.nmode));
      CLabel::ArchiveRead(ar, &m_ScalePainter.m_Ntx.nround, sizeof(m_ScalePainter.m_Ntx.nround));
   }
}

void CColorLabel::SetMinMaxLevel(double dMin, double dMax)
{
   m_ScalePainter.m_Div.f1 = dMin;
   m_ScalePainter.m_Div.f2 = dMax;
}

void CColorLabel::SetUnit(TCHAR *pszText)
{
   m_ScalePainter.SetUnit(pszText);
}

bool CColorLabel::SetColors(COLORREF *pc, int n)
{
   if (m_pColorArray)
   {
      return m_pColorArray->SetColors(pc, n);
   }
   return false;
}

bool CColorLabel::GetColors(COLORREF *pc, int n)
{
   if (m_pColorArray)
   {
      return m_pColorArray->SetColors(pc, n);
   }
   return false;
}

bool CColorLabel::SetNumColors(int n)
{
   if (m_pColorArray)
   {
      return m_pColorArray->SetArraySize(n);
   }
   return false;
}

int CColorLabel::GetNumColors()
{
   if (m_pColorArray)
   {
      return m_pColorArray->GetNumColor();
   }
   return 0;
}

CSize CColorLabel::GetMinScaleSize(CDC *pDC)
{
   return m_ScalePainter.GetMinSize(pDC);
}
