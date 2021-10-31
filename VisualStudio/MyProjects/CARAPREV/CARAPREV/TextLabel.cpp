// TextLabel.cpp: Implementierung der Klasse CTextLabel.
//
//////////////////////////////////////////////////////////////////////
// CRuntimeClass
#include "stdafx.h"
#include "TextLabel.h"
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

CBrush  *CTextLabel::gm_pBrush = NULL;
CFont   *CTextLabel::gm_pFont  = NULL;
CPen    *CTextLabel::gm_pPen   = NULL;
COLORREF CTextLabel::gm_Color  = 0;
UINT     CTextLabel::gm_nAlign = 0;
COLORREF CTextLabel::gm_BKColor = RGB(255,255,255);
int      CTextLabel::gm_nExtraSpacing = 0;
BSState  CTextLabel::gm_TextStates = {0};

IMPLEMENT_SERIAL(CTextLabel, CLabel,0);

#if _MFC_VER < 0x0710
bool CEtsString::Attach(LPTSTR str)
{
   if (str)
   {
      if (m_pchData != NULL)
      {
         FreeData(GetData());
         Init();
      }
      m_pchData = str;
      return true;
   }
   return false;
}

LPTSTR CEtsString::Detach()
{
   LPTSTR str = m_pchData;
   Init();
   return str;
}
#endif

CTextLabel::CTextLabel()
{
   #ifdef CHECK_LABEL_CONSTRUCTION
   REPORT("CTextLabel::Constructor");
   #endif
   Init(&CPoint(0,0), NULL);
}

CTextLabel::CTextLabel(CPoint *p, const TCHAR *text)
{ 
   #ifdef CHECK_LABEL_CONSTRUCTION
   REPORT("CTextLabel::Constructor(arg)");
   #endif
   Init(p, text);
}
 
CTextLabel::CTextLabel(CRect *r, const TCHAR *text)
{ 
   #ifdef CHECK_LABEL_CONSTRUCTION
   REPORT("CTextLabel::Constructor(arg)");
   #endif
   m_State.multitextlines = true;
   m_State.cliplabel      = false;
   m_State.calctextrect   = true;
   m_State.scaleable      = true;
   CPoint p(r->left, r->top);
   Init(&p, text);
   m_pP[1].x = r->right;
   m_pP[1].y = r->bottom;
   m_rcClip.SetRect(m_pP[0], m_pP[1]);
   m_rcClip.NormalizeRect();
}

CTextLabel::CTextLabel(CTextLabel* ptl)
{
   m_State         = ptl->m_State;
   Init(&ptl->m_pP[0], ptl->GetText());
   m_State         = ptl->m_State;
   m_pP[1]         = ptl->m_pP[1];
   m_rcClip        = ptl->m_rcClip;
   m_Color         = ptl->m_Color;
   m_pFont         = ptl->m_pFont;
   m_pPen          = ptl->m_pPen;
   m_pBrush        = ptl->m_pBrush;
   m_nAlign        = ptl->m_nAlign;
   m_nExtraSpacing = ptl->m_nExtraSpacing;
}
void CTextLabel::Init(CPoint *p, const TCHAR *text)
{
   m_nCount       = (m_State.multitextlines != 0) ? 2 : 1;
   m_pP           = new CPoint[m_nCount+1];
   m_pP[0]        = *p;
   m_nTextCursor  = 0;
   SetText(text);

   m_Color   = gm_Color;
   m_nAlign  = gm_nAlign;
   m_BkColor = gm_BKColor;
   m_nExtraSpacing = gm_nExtraSpacing;
   if (!gm_pPen)
   {  
      LOGPEN lp = {PS_NULL, {0, 0}, 0};
      gm_pPen = FindPen(&lp);
      ChangeGlobalObject(NULL, gm_pPen);
   }
   m_pPen = gm_pPen;

   m_pBrush = gm_pBrush;
   m_State.drawbackgnd = (m_pBrush == NULL) ? false : true;
//   m_State.drawbackgnd = gm_TextStates.drawbackgnd;
   m_State.drawframe   = gm_TextStates.drawframe;

   if (!gm_pFont)
   {  
      LOGFONT lf = { -34, 15,                   // Height, Width,
                       0,  0,                   // Escapement, Orientation,
                     400,                       // Weight,
                       0, 0, 0,                 // Italic, Underline, Strikeout, 
                       0,                       // Charset,
                       0,                       // OutPrecision
                       0,                       // ClipPrecision
                       0,                       // Quality
                       0,                       // Pitch and Family
                       _T("Arial")};            // FaceName
      gm_pFont = CLabel::FindFont(&lf);
      ChangeGlobalObject(NULL, gm_pFont);
   }
   m_pFont = gm_pFont;
   LOGFONT lf = GetLogFont();
   m_pP[m_nCount].x = lf.lfWidth  << 8;
   m_pP[m_nCount].y = lf.lfHeight << 8;
}

#ifdef _DEBUG
void CTextLabel::AssertValid() const
{
   CLabel::AssertValid();
}
#endif

CTextLabel::~CTextLabel()
{
   #ifdef CHECK_LABEL_DESTRUCTION
   REPORT("CTextLabel::Destructor");
   #endif
}

bool CTextLabel::Draw(CDC * pDC, int bAtr)
{
//   REPORT("CTextLabel::Draw()");
   bool bPicked = IsPicked();
   if (bAtr == 2) SetPicked(false);
   if (!CLabel::Draw(pDC, bAtr)) return false;
   SetPicked(bPicked);

   if (m_State.scaleable && m_State.delayedscale && (m_pFont>(CFont*)STOCK_LAST))
   {
      LOGFONT lf  = GetLogFont();
      CSize Numerator(  m_pP[m_nCount].x, m_pP[m_nCount].y);    // neue Größe
      CSize Denominator(lf.lfWidth << 8 , lf.lfHeight << 8);    // alte Größe
      lf.lfWidth  = MulDiv(lf.lfWidth , Numerator.cx, Denominator.cx);
      lf.lfHeight = MulDiv(lf.lfHeight, Numerator.cy, Denominator.cy);
      if (lf.lfHeight > 0)
      {
         m_pP[0].y  += abs(m_rcClip.bottom-m_rcClip.top);
         lf.lfHeight = -lf.lfHeight;
      }
      if (lf.lfWidth < 0)
      {
         m_pP[0].x  -= abs(m_rcClip.right-m_rcClip.left);
         lf.lfWidth = -lf.lfWidth;
      }
      SetLogFont(&lf);
      m_State.delayedscale = false;
   }

   int offsety = 0;
   UINT nOldAlign = 0;
   if (!m_strText.IsEmpty())
   {
      if (bAtr)
      {
         pDC->SetTextCharacterExtra(m_nExtraSpacing);          // Extraspacing setzen
         if (m_State.multitextlines)                           // wenn Multiline-Text
            pDC->SetTextAlign(TA_LEFT|TA_TOP);                 // Textausrichtung left, top
         else                                                  // sonst
            pDC->SetTextAlign(m_nAlign&0xffffffdf);            // Textausrichtung setzen
         if (CLabel::gm_bColorMatching) pDC->SetTextColor(m_Color);
         else                           pDC->CDC::SetTextColor(m_Color);
         pDC->SelectStockObject(HOLLOW_BRUSH);
         SelectPen(pDC, m_pPen);
         if (((int)m_pFont >= OEM_FIXED_FONT) && ((int)m_pFont <= STOCK_LAST))
            pDC->SelectStockObject((int)m_pFont);
         else
            pDC->SelectObject(m_pFont);
      }
      else if (m_State.multitextlines)                         // wenn Multiline-Text
      {
         nOldAlign = m_nAlign;
         m_nAlign  = pDC->GetTextAlign();
         pDC->SetTextAlign(TA_LEFT|TA_TOP);                    // Textausrichtung left, top
      }

      if (m_State.multitextlines)
      {
         DrawMultiLineText(pDC);
      }
      else if (m_State.cliplabel)
      {
         if ((m_nAlign & TA_VCENTER)   == TA_VCENTER) offsety = (m_rcClip.top - m_rcClip.bottom)/ 2;
         pDC->ExtTextOut( m_pP[0].x, m_pP[0].y+offsety, ETO_CLIPPED, &m_rcClip, m_strText, NULL);
         if (m_State.drawframe) CLabel::Rectangle(pDC, &m_rcClip, m_State.drawbackgnd);
      }
      else
      {
         CRect rect = GetRect(pDC);
         if ((m_nAlign & TA_VCENTER) == TA_VCENTER) offsety = (rect.top - rect.bottom)/ 2;
         pDC->TextOut( m_pP[0].x, m_pP[0].y+offsety, m_strText);
         if (m_State.drawframe) CLabel::Rectangle(pDC, &rect, m_State.drawbackgnd);
      }
      if (!bAtr && m_State.multitextlines)
      {
         pDC->SetTextAlign(m_nAlign);
         m_nAlign = nOldAlign;
      }
   }

   if (IsPicked()) DrawShape(pDC, true);
   return true;
}

bool CTextLabel::DrawShape(CDC * pDC, bool draw)
{
   if (IsShapeDrawn() == draw) return false;                   // wenn Zustand vorher gleich, Funktion mit false beenden
   if (!IsVisible() && draw)   return false;                   // ist es unsichtbar, darf es nicht gezeichnet aber gelöscht werden

   CRect rect = GetRect(pDC);

   if (!CLabel::DrawShape(pDC, draw)) return false;

   if (IsDoublePicked())
   {
      CPoint caretpos;
      caretpos.x = rect.right;
      caretpos.y = (rect.top+rect.bottom) >> 1;
      pDC->LPtoDP(&caretpos);
      CWnd::SetCaretPos(caretpos);
   }
   else
   {
      pDC->SaveDC();
      pDC->SelectObject(CLabel::gm_pShapePen);
      pDC->SelectStockObject(HOLLOW_BRUSH);
      CLabel::Rectangle(pDC, &rect, false);
      pDC->RestoreDC(-1);
   }
   return true;
}

void CTextLabel::SetChar(UINT nChar)
{
   switch(nChar)
   {
      case  8:                                                 // Backspace
         if (m_nTextCursor>0) m_strText.SetAt(--m_nTextCursor, 0);
      case  9: case 13:                               return;  // Tabulator, Enter
      default:
         m_strText.Insert(m_nTextCursor++, (TCHAR)nChar);
      SetChanged(true);                                        // Text wurde geändert
   }
}

void CTextLabel::MoveLabel(CPoint p)
{
   if (!IsFixed())
   {
      m_rcClip.OffsetRect(p);
      CLabel::MoveLabel(p);
   }
}

void CTextLabel::ScaleLabel(CSize Numerator, CSize Denominator)
{
   if (!IsFixed() && m_State.scaleable)
   {
      CLabel::ScaleLabel(Numerator, Denominator);
      m_rcClip.left   = MulDiv(m_rcClip.left  , Numerator.cx, Denominator.cx);
      m_rcClip.top    = MulDiv(m_rcClip.top   , Numerator.cy, Denominator.cy);
      m_rcClip.right  = MulDiv(m_rcClip.right , Numerator.cx, Denominator.cx);
      m_rcClip.bottom = MulDiv(m_rcClip.bottom, Numerator.cy, Denominator.cy);

      m_pP[m_nCount].x = MulDiv(m_pP[m_nCount].x, Numerator.cx, Denominator.cx);
      m_pP[m_nCount].y = MulDiv(m_pP[m_nCount].y, Numerator.cy, Denominator.cy);
      if (!m_State.multitextlines) m_State.delayedscale = true;
   }
}

CRect CTextLabel::GetRect(CDC *pDC, UINT nFlags)
{
   CSize    textsize;
   CRect    textrect;
   CPoint   porg   = m_pP[0];
   bool     bYInv  = false;
   bool     bEmpty = false;
   bYInv = IsNegativeY(pDC);

   if ((!m_State.multitextlines && !m_State.textchanged && m_State.cliplabel) || // Singletextline
       ( m_State.multitextlines && !m_State.calctextrect))                       // Multitextlines
   {
      textrect = m_rcClip;
   }
   else
   {
      if (!pDC) return CRect(0,0,0,0);
      bool     bAttribDC_NULL = (pDC->m_hAttribDC==NULL);
      if (bAttribDC_NULL) pDC->m_hAttribDC = pDC->m_hDC;

      pDC->SaveDC();
      if (((int)m_pFont >= OEM_FIXED_FONT) && ((int)m_pFont <= STOCK_LAST))
         pDC->SelectStockObject((int)m_pFont);
      else
         pDC->SelectObject(m_pFont);

      pDC->SetTextCharacterExtra(m_nExtraSpacing);

      if (m_strText.IsEmpty()) m_strText = _T(" "), bEmpty = true;

      if (m_State.multitextlines)
      {
         DrawMultiLineText(pDC, true);
         textrect = m_rcClip;
         m_State.calctextrect = false;
/*
         textrect = m_rcClip;
         if (bYInv) swap(textrect.top, textrect.bottom);
         pDC->DrawText(m_strText, &textrect, MultiTextLineFormat()|DT_CALCRECT);
         if (bYInv) swap(textrect.top, textrect.bottom);
         m_State.cliplabel = true;
         m_pP[0].x = textrect.left;
         m_pP[1].x = textrect.right;
         m_pP[0].y = textrect.bottom;
         m_pP[1].y = textrect.top;
         m_State.calctextrect = false;
*/
//         m_rcClip = textrect;
/*
         m_rcClip.left  = m_pP[0].x = textrect.left;
         m_rcClip.right = m_pP[1].x = textrect.right;
         if (bYInv)
         {
            m_rcClip.top    = m_pP[1].y = textrect.bottom;
            m_rcClip.bottom = m_pP[0].y = textrect.top;
         }
         else
         {
            m_rcClip.top    = m_pP[0].y = textrect.top;
            m_rcClip.bottom = m_pP[1].y = textrect.bottom;
         }
*/
      }
      else
      {
         TEXTMETRIC Metrics;
         pDC->GetTextMetrics(&Metrics);
         textsize   = pDC->GetTextExtent(m_strText);

         if (bEmpty) m_strText.Empty();

         if (Metrics.tmItalic)    textsize.cx+= Metrics.tmAveCharWidth/2;
         if (nFlags&INFLATE_RECT) textsize.cx+= Metrics.tmAveCharWidth;

         if      ((m_nAlign & TA_BASELINE) == TA_BASELINE)
         {
            if (bYInv)
            {
               textrect.bottom = porg.y + Metrics.tmAscent;
               textrect.top    = porg.y - Metrics.tmDescent;
            }
            else
            {
               textrect.top    = porg.y + Metrics.tmAscent;
               textrect.bottom = porg.y - Metrics.tmDescent;
            }
         }
         else if ((m_nAlign & TA_VCENTER)   == TA_VCENTER)
         {
            if (bYInv)
            {
               textrect.bottom = porg.y + MulDiv(textsize.cy, 10, 20);
               textrect.top    = textrect.bottom - textsize.cy;
            }
            else
            {
               textrect.top    = porg.y - MulDiv(textsize.cy, 10, 20);
               textrect.bottom = textrect.top + textsize.cy;
            }
         }
         else if ((m_nAlign & TA_BOTTOM)   == TA_BOTTOM)
         {
            if (bYInv)
            {
               textrect.top    = porg.y;
               textrect.bottom = textrect.top + textsize.cy;
            }
            else
            {
               textrect.bottom = porg.y;
               textrect.top    = textrect.bottom - textsize.cy;
            }
         }
         else
         {
            if (bYInv)
            {
               textrect.bottom = porg.y;
               textrect.top    = textrect.bottom - textsize.cy;
            }
            else
            {
               textrect.top    = porg.y;
               textrect.bottom = textrect.top    + textsize.cy;
            }
         }

         if      ((m_nAlign & TA_CENTER)   == TA_CENTER)
         {
            textrect.left   = porg.x - MulDiv(textsize.cx, 10, 20);
            textrect.right  = textrect.left   + textsize.cx;
         }
         else if ((m_nAlign & TA_RIGHT)    == TA_RIGHT)
         {
            textrect.right  = porg.x;
            textrect.left   = textrect.right  - textsize.cx;
         }
         else
         {
            textrect.left   = porg.x;
            textrect.right  = textrect.left   + textsize.cx;
         }
      }
      if (bAttribDC_NULL) pDC->m_hAttribDC = NULL;
      m_State.textchanged = false;
      SetClipRect(&textrect);
      pDC->RestoreDC(-1);
   }

   if (nFlags & INFLATE_RECT) textrect.InflateRect(gm_PickPointSize);

   if ((nFlags&DEV_COORD) && pDC)
   {
      ::LPtoDP(pDC->m_hDC, (LPPOINT)&textrect, 2);
      textrect.NormalizeRect();
   }

   return textrect;
}

void CTextLabel::SetText(const TCHAR *text)
{
   if (text) m_strText = text;
   else      m_strText.Empty();
   m_State.textchanged  = true;
}
bool CTextLabel::SetStringLength(int size)
{
   bool bOk = (m_strText.GetBufferSetLength(size) != NULL);
   m_strText.ReleaseBuffer();
   return bOk;
}
void CTextLabel::SetTextColor(COLORREF c)
{
   SetChanged(TRUE);
   m_Color = c;
   if (gm_bSaveGlobal) CTextLabel::gm_Color = c;
}

void CTextLabel::SetTextAlign(UINT align)
{
   SetChanged(TRUE);
   m_nAlign = align;
   m_State.textchanged = true;
   if (gm_bSaveGlobal) CTextLabel::gm_nAlign = align;
}

void CTextLabel::SetLogFont(LOGFONT* plf)
{
   SetChanged(TRUE);
   if (((int)plf >= OEM_FIXED_FONT) && ((int)plf <= STOCK_LAST))
   {
      m_pFont = (CFont*)plf;
   }
   else
   {
      m_pFont = CLabel::FindFont(plf);
      m_pP[m_nCount].x = plf->lfWidth  << 8;
      m_pP[m_nCount].y = plf->lfHeight << 8;
      m_State.textchanged  = true;
      if (gm_bSaveGlobal)
      {
         ChangeGlobalObject(gm_pFont, m_pFont);
         gm_pFont = m_pFont;
      }
   }
}

LOGFONT CTextLabel::GetLogFont()
{
   LOGFONT LogFont;
   memset(&LogFont, 0, sizeof(LOGFONT));
   if ((int)m_pFont > STOCK_LAST) m_pFont->GetLogFont(&LogFont);
   return LogFont;
}
void CTextLabel::SetExtraSpacing(int ne)
{
   SetChanged(TRUE);
   m_nExtraSpacing = ne;
   if (gm_bSaveGlobal) gm_nExtraSpacing = ne;
   m_State.textchanged = true;
}

void CTextLabel::SetLogBrush(LOGBRUSH *plb)
{
   SetChanged(TRUE);
   m_pBrush = FindBrush(plb);
   m_State.drawbackgnd = (m_pBrush == NULL) ? false : true;
   if (gm_bSaveGlobal)
   {
      ChangeGlobalObject(gm_pBrush, m_pBrush);
      gm_pBrush     = m_pBrush;
      gm_BKColor    = m_BkColor;
      gm_TextStates = m_State;
   }
}

LOGBRUSH CTextLabel::GetLogBrush()
{
   LOGBRUSH  LogBrush ={BS_HOLLOW, 0, 0};
   if (m_pBrush) m_pBrush->GetLogBrush(&LogBrush);
   return LogBrush;
}

void CTextLabel::SetLogPen(LOGPEN* plp) 
{
   SetChanged(TRUE);
   m_pPen = CLabel::FindPen(plp);
   if (gm_bSaveGlobal)
   {
      ChangeGlobalObject(gm_pPen, m_pPen);
      gm_pPen = m_pPen;
      gm_TextStates = m_State;
   }
}

LOGPEN CTextLabel::GetLogPen() 
{
   LOGPEN  LogPen ={0, {0, 0}, 0};
   if (m_pPen) m_pPen->GetLogPen(&LogPen);
   return LogPen;
}

void CTextLabel::Serialize(CArchive &ar)
{
   #ifdef CHECK_LABEL_SERIALIZATION
   REPORT("CTextLabel::Serialize()");
   #endif
   CLabel::Serialize(ar);
   long index = 0;
   if (ar.IsStoring())
   {
      WORD wTextSize = m_strText.GetLength();
      ArchiveWrite(ar, &wTextSize, sizeof(WORD));
      WriteText(ar, wTextSize);
      ArchiveWrite(ar, &m_Color  , sizeof(COLORREF));
      ArchiveWrite(ar, &m_BkColor, sizeof(COLORREF));
      ArchiveWrite(ar, &m_nAlign , sizeof(UINT));
      index = FindPen(m_pPen);
      ASSERT(index >= 0);
      ArchiveWrite(ar, &index    , sizeof(long));
      index = FindBrush(m_pBrush);
      ArchiveWrite(ar, &index    , sizeof(long));
      index = FindFont(m_pFont);
      ASSERT(index >= 0);
      ArchiveWrite(ar, &index    , sizeof(long));
      if (gm_nFileVersion > 100)
      {
         index = (long) m_nExtraSpacing;
         ArchiveWrite(ar, &index , sizeof(long));
      }
   }
   else
   {
      WORD wTextSize;
      ArchiveRead(ar, &wTextSize, sizeof(WORD));
      VERIFY(ReadText(ar, wTextSize));
      ArchiveRead(ar, &m_Color  , sizeof(COLORREF));
      ArchiveRead(ar, &m_BkColor, sizeof(COLORREF));
      ArchiveRead(ar, &m_nAlign , sizeof(UINT));
      ArchiveRead(ar, &index    , sizeof(long));
      m_pPen = FindPen(index);
      ArchiveRead(ar, &index    , sizeof(long));
      m_pBrush = FindBrush(index);
      ArchiveRead(ar, &index    , sizeof(long));
      m_pFont = FindFont(index);
      if (gm_nFileVersion > 100)
      {
         ArchiveRead(ar, &index , sizeof(long));
         m_nExtraSpacing = index;
      }
      if (m_State.multitextlines)
      {
         m_rcClip = CLabel::GetRect();
      }
      m_State.textchanged  = true;
      m_State.delayedscale = false;
   }
}
bool CTextLabel::ReadText(CArchive &ar, WORD wLength, bool bForce)
{
   if (SetStringLength(wLength))
   {
      TCHAR * pt = (TCHAR*)LPCTSTR(m_strText);
      ArchiveRead(ar, pt, wLength, bForce);
      return true;
   }
   return false;
}
void CTextLabel::WriteText(CArchive &ar, WORD wLength)
{
   if (wLength>0)
   {
      TCHAR * pt = (TCHAR*)LPCTSTR(m_strText);
      ArchiveWrite(ar, pt, wLength);
   }
}

void CTextLabel::SetClipRect(CRect *prcClip, bool bClip)
{
   if (prcClip)
   {
      if (!m_State.multitextlines)
         m_State.textchanged = false;
      m_rcClip          = *prcClip;
      m_State.cliplabel = bClip;
   }
   else m_State.cliplabel = false;
}

int CTextLabel::ClipTextRect(CLabel *pl, void *pParam)
{
   if (pl && pl->IsKindOf(RUNTIME_CLASS(CTextLabel)) && pParam)
   {
      ProcessLabel *ppl = (ProcessLabel*) pParam;
      CDC          *pDC = (CDC*)          ppl->pParam1;
      CRect        rcClip(*((CRect*)ppl->pParam2));
      CTextLabel* ptl =(CTextLabel*)pl;
      if (!ptl->IsMultiLineText())
      {
         ptl->SetClipRect(NULL);
         ptl->SetTextChanged();
      }
      CRect rect = pl->GetRect(pDC);
      rcClip.NormalizeRect();
      bool bClip = false;
      if      (rect.left   <= (rcClip.left   + 3)) rect.left   = rcClip.left   + 3, bClip = true;
      else if (rect.right  >= (rcClip.right  - 3)) rect.right  = rcClip.right  - 3, bClip = true;
      if      (rect.top    <= (rcClip.top    + 3)) rect.top    = rcClip.top    + 3, bClip = true;
      else if (rect.bottom >= (rcClip.bottom - 3)) rect.bottom = rcClip.bottom - 3, bClip = true;
      CSize size = rect.Size();
      if ((size.cx < 0) || (size.cy < 0))                      // ist der Label außerhalb
      {
         if (pl->IsPicked())
         {
            ppl->nPos++;
            pl->SetPicked(false);
            if (ppl->nPos == 1) pl->DrawShape(pDC, false);
         }
         if (pl->IsVisible()) pl->SetHidden(true);             // als versteckt markieren
         pl->SetVisible(false);                                // und nur unsichtbar machen
      }
      else
      {
         if (pl->IsHidden())                                   // wenn es versteckt ist 
         {
            pl->SetVisible(true);                              // wieder sichtbar machen
            pl->SetHidden(false);                              // Versteckt-Flag löschen
         }
         if (bClip)
         {
            if (size.cx == 0) rect.InflateRect(1, 0);
            if (size.cy == 0) rect.InflateRect(0, 1);
            ptl->SetClipRect(&rect, true);
         }
      }
   }
   return 0;
}

void CTextLabel::SetPoint(CPoint p)
{
   if (!IsFixed())
   {
      if (m_State.multitextlines)
      {
         CLabel::SetPoint(p);
         m_rcClip = CLabel::GetRect();
      }
      else
      {
         ASSERT(m_nAct == 0);
         CPoint diff = p - m_pP[0];                               // Clipprechteck verschieben
         m_rcClip.OffsetRect(diff);
      }
      CLabel::SetPoint(p);
   }
}

UINT CTextLabel::MultiTextLineFormat()
{
   UINT nFormat = DT_NOPREFIX;
   if      ((m_nAlign & TA_VCENTER) == TA_VCENTER) nFormat |= DT_SINGLELINE|DT_VCENTER;
   else if ((m_nAlign & TA_BOTTOM ) == TA_BOTTOM)  nFormat |= DT_SINGLELINE|DT_BOTTOM;
   else                                            nFormat |= DT_WORDBREAK;
   if      ((m_nAlign & TA_CENTER) == TA_CENTER)   nFormat |= DT_CENTER;
   else if ((m_nAlign & TA_RIGHT ) == TA_RIGHT)    nFormat |= DT_RIGHT;
   else                                            nFormat |= DT_LEFT;
   return nFormat;
}

BOOL CTextLabel::DrawMultiLineText(CDC *pDC, bool bCalcRect)
{
   if (!m_strText.IsEmpty())
   {
      CRect       rc(m_rcClip);
      const TCHAR*pszNL, *psz = m_strText;
      TEXTMETRIC  Metrics;
      SIZE        textsize, szMin,
                  rectsize = m_rcClip.Size();
      int         nLen, nMaxWidth;
      bool        bWordBreak,
                  bCenter = false,
                  bYInv   = IsNegativeY(pDC);
      POINT       ptOut;

      pDC->GetTextMetrics(&Metrics);                           // Texteigenschaften
      if (bYInv) swap(rc.top, rc.bottom);                      // negative Y-Achse ?
      if (m_State.drawframe && !bCalcRect)                     // Rahmen zeichnen
         CLabel::Rectangle(pDC, &m_rcClip, m_State.drawbackgnd);

      ptOut = m_pP[0];                                         // oben links
      if (bCalcRect)                                           // beim berechnen des Textrechtecks
      {
         pDC->SetTextAlign(TA_TOP|TA_LEFT);                    // Align auf Top, Left
         szMin.cx  = m_rcClip.right - m_rcClip.left;           // Breite vormerken
         szMin.cy  = 0;                                        // Höhe wird addiert
         nMaxWidth = 0;                                        // max Breite ermitteln
      }
      else if ((m_nAlign & TA_CENTER) == TA_CENTER)            // Center alignd
      {
         pDC->SetTextAlign(TA_TOP|TA_LEFT);                    // Align auf Top, Left
         bCenter = true;                                       // Startpunkt für jede Zeile ausrechnen
      }
      else if ((m_nAlign & TA_RIGHT ) == TA_RIGHT)             // Rechts aligned
      {
         pDC->SetTextAlign(TA_TOP|TA_RIGHT);                   // Align auf Top, Right
         ptOut.x = m_pP[0].x + rectsize.cx;                    // Startpunkt rechts
      }
      else                                                     // Links aligned
      {
         pDC->SetTextAlign(TA_TOP|TA_LEFT);                    // Align auf Top, Left
      }
      do
      {
         bWordBreak = false;                                   // automatischen Zeilenumbruch, normal nicht
         pszNL = _tcsstr(psz, _T("\r\n"));                          // echten Zeilenumbruch immer
         if (pszNL) nLen = pszNL - psz;                        // Länge ermitteln
         else       nLen = _tcsclen(psz);
         textsize = pDC->GetTextExtent(psz, nLen);             // Textgröße ermitteln
         if (textsize.cx > rectsize.cx)
         {
            int nOld = nLen;
            for (; nLen>1; nLen--)
            {
               if (psz[nLen] == ' ')                           // Leerzeichen gefunden ?
               {
                  textsize = pDC->GetTextExtent(psz, nLen);    // Textgröße ermitteln
                  if (textsize.cx < rectsize.cx)               // kurz genug
                  {
                     bWordBreak = true;                        // automatischer Zeilenumbruch an
                     break;
                  }
               }
            }
            if (nLen == 1)                                     // zu kurz ?
            {
               bWordBreak = false;                             // automatischer Zeilenumbruch an
               nLen = nOld;                                    // alte Länge, Wort clippen
            }
         }
         if (bCenter)                                          // zentriert ?
         {                                                     // Startpunkt berechnen
            ptOut.x = m_pP[0].x + ((rectsize.cx - textsize.cx) / 2);
         }
         if (bCalcRect)                                        // Textrechteck ermitteln ?
         {
            if (textsize.cx > szMin.cx ) szMin.cx  = textsize.cx;
            if (textsize.cx > nMaxWidth) nMaxWidth = textsize.cx;
            szMin.cy += Metrics.tmHeight;
         }
         else                                                  // oder Ausgabe
         {
            pDC->ExtTextOut(ptOut.x, ptOut.y, ETO_CLIPPED, &rc, psz, nLen, NULL);
            if (bYInv) ptOut.y -= Metrics.tmHeight;            // nächste Zeile
            else       ptOut.y += Metrics.tmHeight;
         }
         if (bWordBreak) psz = &psz[nLen+1];                   // automatische Umbruch, hinter dem Word weiter
         else if (pszNL) psz = &pszNL[2];                      // noch eine weitere echte Zeile
         else            psz = NULL;                           // keine Zeile mehr
      }
      while(psz);                                              // fertig ?

      if (bCalcRect)                                           // Textrechteck ermitteln ?
      {
         if (szMin.cx > nMaxWidth)                             // kann verkürzt werden
         {
            szMin.cx = nMaxWidth;                              // Sicherheitslänge addieren
            if (Metrics.tmItalic) szMin.cx += Metrics.tmAveCharWidth/2;
         }
         if (nLen != 0) szMin.cy += Metrics.tmDescent;         // Sicherheitshöhe addieren
         m_rcClip.left   = m_pP[0].x;
         m_rcClip.right  = m_pP[1].x = m_pP[0].x + szMin.cx;
         if (bYInv)
         {
            m_rcClip.bottom = m_pP[0].y;
            m_rcClip.top    = m_pP[1].y = m_pP[0].y - szMin.cy;
         }
         else
         {
            m_rcClip.top    = m_pP[0].y;
            m_rcClip.bottom = m_pP[1].y = m_pP[0].y + szMin.cy;
         }
      }
//       pDC->DrawText(m_strText, &rc, MultiTextLineFormat());
      return 1;
   }
   return 0;
}

void CTextLabel::ResetGlobalObjects()
{
   gm_pBrush = NULL;
   gm_pPen   = NULL;
   gm_pFont  = NULL;
}
