// PropertyPreview.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "PropertyPreview.h"
#include "Label.h"
#include "TextLabel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyPreview

CPropertyPreview::CPropertyPreview()
{
   m_pLabel     = NULL;
   m_pLogBrush  = NULL;
   m_pLogPen    = NULL;
   m_pLogFont   = NULL;
   m_pTextColor = NULL;
   m_pTextAlign = NULL;
   m_pBkColor   = NULL;
   m_pnZoom     = NULL;
   m_pbDrawBkGnd= NULL;
   m_pnExtraSpacing = NULL;
   m_BkColor   = RGB(255, 255,255);
}

CPropertyPreview::~CPropertyPreview()
{
}


BEGIN_MESSAGE_MAP(CPropertyPreview, CStatic)
	//{{AFX_MSG_MAP(CPropertyPreview)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CPropertyPreview 
void CPropertyPreview::OnPaint()
{
   CPaintDC dc(this);
   int    ViewExt, WndExt;
   CPen   pen;
   CBrush brush;
   CFont  font;
   CRect  clientrect;                                          // Clientrechteck bestimmen
   GetClientRect(&clientrect);
   CRgn   clientrgn;                                           // Clippregion erstellen
   clientrgn.CreateRectRgnIndirect(&clientrect);
   CSize clientsize = clientrect.Size();
   clientsize.cx -= 8;
   clientsize.cy -= 8;
   dc.SaveDC();

   if (m_pLogPen)
   {
      pen.CreatePenIndirect(m_pLogPen);
      dc.SelectObject(&pen);
   }
   else dc.SelectStockObject(BLACK_PEN);
   if (m_pLogBrush)
   {
      brush.CreateBrushIndirect(m_pLogBrush);
      dc.SelectObject(&brush);
   }
   else dc.SelectStockObject(HOLLOW_BRUSH);
   if (m_pLogFont)
   {
      font.CreateFontIndirect(m_pLogFont);
      dc.SelectObject(&font);
   }
   else dc.SelectStockObject(ANSI_VAR_FONT);
   COLORREF color = RGB(255, 255, 255);
   if (m_pBkColor) color = *m_pBkColor;
   if ((m_pbDrawBkGnd != NULL) && ((*m_pbDrawBkGnd) != 0))
   {
      dc.SetBkMode(OPAQUE);
      dc.SetBkColor(color);
   }
   else
   {
      dc.SetBkMode(TRANSPARENT);
   }
   if (m_pTextColor) dc.SetTextColor(*m_pTextColor);
   if (m_pTextAlign) dc.SetTextAlign(*m_pTextAlign);
   if (m_pnExtraSpacing) dc.SetTextCharacterExtra(*m_pnExtraSpacing);

   if (m_pLabel)
   {
      BSState states = m_pLabel->GetStates();
      m_pLabel->ResetPickStates();
      UINT nAlign = 0;
      CTextLabel *ptl = NULL;
      if (m_pLabel->IsKindOf(RUNTIME_CLASS(CTextLabel)))
      {
         ptl = (CTextLabel*)m_pLabel;
         if (ptl->IsMultiLineText()) ptl = NULL;
         else
         {
            nAlign = ptl->GetTextAlign();
            ptl->SetTextAlign(TA_LEFT|TA_TOP);
            dc.SetTextAlign(TA_LEFT|TA_TOP);
         }
      }

      dc.SetMapMode(MM_ANISOTROPIC);
      CRect  labelrect;
      CSize  labelsize;

      labelrect = m_pLabel->GetRect(&dc, (ptl!=NULL) ? 0 : INFLATE_RECT);
      labelsize = labelrect.Size();

      if (m_pnZoom)     // Zoomfaktor einstellen
      {
         ViewExt = clientsize.cy;
         WndExt  = MulDiv(ViewExt, *m_pnZoom, 100);
         dc.SetMapMode(MM_ANISOTROPIC);
      }
      else              // automatisch anpassen
      {
         double dx = (double)clientsize.cx / (double)labelsize.cx;
         double dy = (double)clientsize.cy / (double)labelsize.cy;
         if( dx < dy)
         {
            ViewExt = clientsize.cx;
            WndExt  = labelsize.cx;
         }
         else
         {
            ViewExt = clientsize.cy;
            WndExt  = labelsize.cy;
         }
      }
      int nxoff, nyoff;
      nxoff = (MulDiv(clientsize.cx,  WndExt, ViewExt) - labelsize.cx)/2;
      nyoff = (MulDiv(clientsize.cy, -WndExt, ViewExt) - labelsize.cy)/2;
      if (ptl)
      {
         nyoff+=labelsize.cy;
      }
      dc.SetViewportExt(ViewExt,  ViewExt);
      dc.SetViewportOrg( 4, 4);
      dc.SetWindowExt( WndExt, -WndExt);
      dc.SetWindowOrg( -nxoff+labelrect.left, -nyoff+labelrect.top);
      dc.SelectClipRgn(&clientrgn, RGN_COPY);

      m_pLabel->Draw(&dc, false);

      if (ptl)
      {
         CPoint ptAlign(labelrect.left, labelrect.top);
         if (m_pTextAlign)
         {
            if      ((*m_pTextAlign & TA_BASELINE) == TA_BASELINE) ptAlign.y = labelrect.top - labelsize.cy / 2;
            else if ((*m_pTextAlign & TA_BOTTOM)   == TA_BOTTOM)   ptAlign.y = labelrect.top - labelsize.cy;

            if      ((*m_pTextAlign & TA_CENTER)   == TA_CENTER)   ptAlign.x = labelrect.left + labelsize.cx / 2;
            else if ((*m_pTextAlign & TA_RIGHT)    == TA_RIGHT)    ptAlign.x = labelrect.right;
         }
         int nInfl = WndExt >> 4;
         if (nInfl <= 0) nInfl = 1;
         dc.PatBlt(ptAlign.x-nInfl, ptAlign.y-nInfl, nInfl<<1, nInfl<<1, BLACKNESS);
         ptl->SetTextAlign(nAlign);
      }

      m_pLabel->SetStates(states);
   }
   else
   {
      clientrect.DeflateRect(4,4,4,4);
      dc.Rectangle(&clientrect);
   }
   dc.RestoreDC(-1);
}


BOOL CPropertyPreview::OnEraseBkgnd(CDC* pDC)
{
   CRect  clientrect;
   GetClientRect(&clientrect);
   CBrush bkbrush;
   LOGBRUSH logbrush = {BS_SOLID, m_BkColor, 0};
   bkbrush.CreateBrushIndirect(&logbrush);
   pDC->SaveDC();
   pDC->SelectObject(bkbrush);
   pDC->PatBlt(clientrect.left, clientrect.top, clientrect.Width(), clientrect.Height(), PATCOPY);
   pDC->RestoreDC(-1);
   return true;
}


void CPropertyPreview::SetLogFont(LOGFONT *plf, COLORREF *pc, UINT *pa, int *pne)
{
   m_pLogFont   = plf;
   m_pTextColor = pc;
   m_pTextAlign = pa;
   m_pnExtraSpacing = pne;
}


/*
      if (btext)
      {
         TEXTMETRIC Metrics;
         dc.GetTextMetrics(&Metrics);
         ptl  = (CTextLabel*) m_pLabel;
         text = ptl->GetText();
         if (text) GetTextExtentPoint32(dc.m_hDC, text, strlen(text), &labelsize);
         else      GetTextExtentPoint32(dc.m_hDC, "Text",          4, &labelsize);
         labelrect.left   = 0;
         labelrect.top    = -labelsize.cy;
         labelrect.right  = labelsize.cx;
         labelrect.bottom = 0;
      }
      else
      {
      }
*/
/*
      if  (btext)
      {
         CPoint ptAlign(labelrect.left, labelrect.bottom);
         if (m_pTextAlign)
         {
            if      ((*m_pTextAlign & TA_BASELINE) == TA_BASELINE) ptAlign.y = labelrect.top / 2;
            else if ((*m_pTextAlign & TA_BOTTOM)   == TA_BOTTOM)   ptAlign.y = labelrect.top;

            if      ((*m_pTextAlign & TA_CENTER)   == TA_CENTER)   ptAlign.x = labelrect.right >> 1;
            else if ((*m_pTextAlign & TA_RIGHT)    == TA_RIGHT)    ptAlign.x = labelrect.right;
         }
         if (text)
         {
            if (m_pTextAlign)     dc.SetTextAlign(*m_pTextAlign&0xffffffdf);
            if (m_pnExtraSpacing) dc.SetTextCharacterExtra(*m_pnExtraSpacing);
            if ((m_pbDrawBkGnd != NULL) && ((*m_pbDrawBkGnd) != 0))
            {
               dc.SetBkMode(OPAQUE);
               dc.SetBkColor(*m_pBkColor);
            }
            else
            {
               dc.SetBkMode(TRANSPARENT);
            }

            dc.TextOut( ptAlign.x, ptAlign.y, text, strlen(text));

            if (m_pTextAlign)
            {
               int nInfl = WndExt >> 4;
               if (nInfl <= 0) nInfl = 1;
               dc.PatBlt(ptAlign.x-nInfl, ptAlign.y-nInfl, nInfl<<1, nInfl<<1, BLACKNESS);
            }
         }
      }
      else 
*/
