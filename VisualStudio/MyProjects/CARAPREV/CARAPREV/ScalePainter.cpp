// ScalePainter.cpp: Implementierung der Klasse CScalePainter.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ScalePainter.h"
#include "Label.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define MAX_OUTPUT_TEXTLENGTH 256

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CScalePainter::CScalePainter()
{
   ZeroMemory(&m_Div, sizeof(ETSDIV_DIV));
   m_Div.l2          = 100;
   m_Div.stepwidth   =  10;
   m_Div.f2          = 1.0; 
   m_Div.divfnc      = ETSDIV_LINEAR;
   m_Ntx.fvalue      = 0.0;
   m_Ntx.nmode       = ETSDIV_NM_STD;
   m_Ntx.nround      =   2;
   m_Ntx.nmaxl       = MAX_OUTPUT_TEXTLENGTH;
   m_Ntx.dZero       = 0;
   m_bHorz           = true;
   m_TextColor       = 0;
   m_strUnit.Empty();
   ZeroMemory(&m_LogFont, sizeof(LOGFONT));
}

void CScalePainter::Draw(CDC *pDC, CRect *prcOut)
{
   CBrush   brush;
   TCHAR     text[MAX_OUTPUT_TEXTLENGTH];
   CSize    szUnit(0,0);
   pDC->SaveDC();
   DrawScale ds = {pDC, prcOut, {4,4}, CLabel::IsNegativeY(pDC)};

   ETSDIV_SCALE Scale;
   ZeroMemory(&Scale, sizeof(Scale));
   Scale.dwFlags = ETSDIV_FORMAT_TEXT|ETSDIV_CALC_ZEROVALUE;
   if (m_bHorz)
   {
      Scale.dwFlags |= ETSDIV_CALC_X_POS|ETSDIV_CALC_GRADIENT;
      Scale.pFkt     = DrawHorizontal;
      m_Div.l1 = prcOut->left;
      m_Div.l2 = prcOut->right;
   }
   else
   {
      Scale.dwFlags |= ETSDIV_CALC_Y_POS;
      Scale.pFkt     = DrawVertikal;
      Scale.dGradient = (double)(prcOut->top - prcOut->bottom) / (m_Div.f2 - m_Div.f1),
      Scale.dOrigin   = (double)prcOut->bottom - (m_Div.f1*Scale.dGradient);
      if (ds.bNegY) swap(prcOut->top, prcOut->bottom);
      m_Div.l1 = prcOut->top;
      m_Div.l2 = prcOut->bottom;
   }
   Scale.div     = m_Div;
   Scale.ntx     = m_Ntx;
   Scale.pszText = &text[0];
   Scale.pParam  = &ds;

   CFont Font;
   if (Font.CreateFontIndirect(&m_LogFont))
   {
      pDC->SelectObject(&Font);
   }
   if (CLabel::gm_bColorMatching) pDC->SetTextColor(m_TextColor);
   else                           pDC->CDC::SetTextColor(m_TextColor);

   CPen pen;
   pen.CreatePen(PS_SOLID, 1, m_TextColor);
   pDC->SelectObject(&pen);
   pDC->SetTextAlign(m_bHorz ? TA_CENTER|TA_TOP : TA_RIGHT|TA_TOP);
   pDC->SetBkMode(TRANSPARENT);

   if (!m_strUnit.IsEmpty())
   {
      szUnit = pDC->GetTextExtent(m_strUnit);
      if      (m_bHorz)  prcOut->right  -= szUnit.cx, szUnit.cy = 0;
      else if (ds.bNegY) prcOut->bottom -= szUnit.cy, szUnit.cx = 0;
      else               prcOut->top    += szUnit.cy, szUnit.cx = 0;
   }

   if (CEtsDiv::DrawScale(&Scale))
   {
      if (szUnit.cx)
      {
         pDC->SetTextAlign(TA_LEFT|TA_TOP);
         pDC->TextOut(prcOut->right, prcOut->top+4, m_strUnit);
      }
      else if (szUnit.cy)
      {
         pDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
         pDC->TextOut(prcOut->right, ((ds.bNegY) ? prcOut->bottom : prcOut->top), m_strUnit);
      }
   }

   pDC->RestoreDC(-1);
}

CSize CScalePainter::GetMinSize(CDC *pDC)
{
   CSize szMin(50, 14);
   CSize szTemp(0, 0);
   TCHAR text[MAX_OUTPUT_TEXTLENGTH];
   CFont Font;
   pDC->SaveDC();
   if (Font.CreateFontIndirect(&m_LogFont))
   {
      pDC->SelectObject(&Font);
   }
   m_Ntx.fvalue = m_Div.f1;
   CEtsDiv::NumberToText(&m_Ntx, text);
   szMin  = pDC->GetTextExtent(text, _tcsclen(text));
   m_Ntx.fvalue = m_Div.f2;
   CEtsDiv::NumberToText(&m_Ntx, text);
   szTemp = pDC->GetTextExtent(text, _tcsclen(text));
   szMin.cx = max(szMin.cx, szTemp.cx);
   szMin.cy = max(szMin.cy, szTemp.cy);
   m_Ntx.fvalue = (m_Div.f1 + m_Div.f2) * 0.5;
   if (m_Ntx.fvalue == 0.0)
      m_Ntx.fvalue = m_Div.f1 + (m_Div.f2-m_Div.f1) * 0.5513453;
   CEtsDiv::NumberToText(&m_Ntx, text);
   szTemp = pDC->GetTextExtent(text, _tcsclen(text));
   szMin.cx = max(szMin.cx, szTemp.cx);
   szMin.cy = max(szMin.cy, szTemp.cy);
   szMin.cx += 6;
   szMin.cy += 6;
   if (!m_strUnit.IsEmpty())
   {
      szTemp = pDC->GetTextExtent(m_strUnit);
      szMin.cx = max(szMin.cx, szTemp.cx);
      szMin.cy = max(szMin.cy, szTemp.cy);
   }
   pDC->RestoreDC(-1);
   return szMin;
}

void CScalePainter::SetUnit(TCHAR *psz)
{
   m_strUnit.Format(_T("[%s]"), psz);
}


bool CScalePainter::DrawHorizontal(ETSDIV_SCALE *pScale)
{
   DrawScale *pDs = (DrawScale*)pScale->pParam;
   int nLen  = _tcsclen(pScale->pszText),
       nPosX = CEtsDiv::Round(pScale->dPosX);
   CSize szText = pDs->pDC->GetTextExtent(pScale->pszText, nLen);
   szText.cx >>= 1;

   if (((nPosX - szText.cx) > pDs->prcOut->left) && ((nPosX + szText.cx) < pDs->prcOut->right))
   {
      pDs->pDC->MoveTo(   nPosX, pDs->prcOut->top);
      pDs->pDC->LineTo(   nPosX, pDs->prcOut->top+pDs->ptLine.y);
      pDs->pDC->TextOut(  nPosX, pDs->prcOut->top+pDs->ptLine.y, pScale->pszText, nLen);
      pDs->prcOut->left = nPosX + szText.cx;
   }
   return true;
}

bool CScalePainter::DrawVertikal(ETSDIV_SCALE *pScale)
{
   DrawScale *pDs = (DrawScale*)pScale->pParam;
   int nLen  = _tcsclen(pScale->pszText),
       nPosY = CEtsDiv::Round(pScale->dPosY);
   CSize szText = pDs->pDC->GetTextExtent(pScale->pszText, nLen);
   szText.cy >>= 1;

   if (((nPosY - szText.cy) > pDs->prcOut->top) && ((nPosY + szText.cy) < pDs->prcOut->bottom))
   {
      pDs->pDC->MoveTo( pDs->prcOut->left         , nPosY);
      pDs->pDC->LineTo( pDs->prcOut->left+pDs->ptLine.x, nPosY);
      if (pDs->bNegY) szText.cy = -szText.cy;
      pDs->pDC->TextOut(pDs->prcOut->right, nPosY - szText.cy, pScale->pszText, nLen);
   }
   return true;
}
