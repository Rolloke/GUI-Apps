// PlotLabel.cpp: Implementierung der Klasse CPlotLabel.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "resource.h"
#include "PlotLabel.h"
#include "CurveLabel.h"
#include "CircleLabel.h"
#include "CEtsDiv.h"
#include "PreviewFileHeader.h"
#include "CaraWinApp.h"
#include "ListDlg.h"

#ifdef ETSDEBUG_REPORT
 #define  ETSDEBUG_INCLUDE 
#endif
#include "CEtsDebug.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
 
#ifndef M_PI
  #define M_PI         3.1415926535898
#endif

#ifndef M_180_D_PI
  #define M_180_D_PI  57.2957795130823
#endif

#ifndef M_200_D_PI
  #define M_200_D_PI  63.6619772367581
#endif

#define IDC_SELECT_ZOOM_RANGE           1301

#define SCATTER_PLOT 0x00000001

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
SStandardPlotSettings CPlotLabel::gm_StdPltSets = 
{
   // Font für die Überschrift
   {-30, 13,                                 // Height, Width,
      0,  0,                                 // Escapement, Orientation,
      400,                                   // Weight,
      0, 0, 0,                               // Italic, Underline, Strikeout, 
      0,                                     // Charset,
      0,                                     // OutPrecision
      0,                                     // ClipPrecision
      0,                                     // Quality
      0,                                     // Pitch and Family
      _T("Arial")                                // FaceName
   },
   RGB(  0,  0,255),                         // Überschriftfarbe Blau
   // Font für die Gittertexte
   {-26, 11,                                 // Height, Width,
      0,  0,                                 // Escapement, Orientation,
      400,                                   // Weight,
      0, 0, 0,                               // Italic, Underline, Strikeout, 
      0,                                     // Charset,
      0,                                     // OutPrecision
      0,                                     // ClipPrecision
      0,                                     // Quality
      0,                                     // Pitch and Family
      _T("Arial")                                // FaceName
   },
   RGB(0,128,0),                             // Gittertextfarbe
   RGB(0,128,0),                             // Gitterlinienfarbe
   // Font für die Kurventexte
   {-26, 11,                                 // Height, Width,
      0,  0,                                 // Escapement, Orientation,
      400,                                   // Weight,
      0, 0, 0,                               // Italic, Underline, Strikeout, 
      0,                                     // Charset,
      0,                                     // OutPrecision
      0,                                     // ClipPrecision
      0,                                     // Quality
      0,                                     // Pitch and Family
      _T("Arial")                                // FaceName
   },
   {                                         // Standard Farbreihenfolge für die Kurven
      RGB(255,  0,  0),                      //  1: Lightred
      RGB(  0,255,  0),                      //  2: Lightgreen
      RGB(  0,  0,255),                      //  3: Lightblue
      RGB(  0,255,255),                      //  4: Lightcyan
      RGB(255,  0,255),                      //  5: Lightmagenta
      RGB(196, 10, 10),                      //  6: Red
      RGB( 10,196, 10),                      //  7: Green
      RGB( 10, 10,196),                      //  8: Blue
      RGB(  0,196,196),                      //  9: Cyan
      RGB(196,  0,196),                      // 10: Magenta
      RGB(255,255,  0),                      // 11: Lightyellow
      RGB(196,196,  0),                      // 12: Yellow
      RGB(100,150,200),                      // 13: Blue  ?
      RGB(150,200,100),                      // 14: Green ?
      RGB(200,150,100),                      // 15: Red   ?
      RGB(  0,  0,  0),                      // 16: Black
   },
   {PS_SOLID, {1, 1}, 0},                    // Rahmen des Plots
   {BS_HOLLOW, 0x00FFFFFF, 0},               // Hintergrund des Plots
   0,                                        // Hintergrundfarbe
   (char)(0x80|ETSDIV_NM_STD),               // Zahlendarstellung x
   (char)(0x80|ETSDIV_NM_STD),               // Zahlendarstellung y
   2,2,                                      // Nachkommastellen (x, y)
   150, 100,                                 // Gitterabstand in 10-tel [mm]
   0                                         // Kurvenrahmen zeichnen
};

IMPLEMENT_SERIAL(CPlotLabel, CRectLabel,1);

CPlotLabel::CPlotLabel()
{
   #ifdef CHECK_LABEL_CONSTRUCTION
   REPORT("CPlotLabel::Constructor");
   #endif
   InitPlot();
}

CPlotLabel::CPlotLabel(CPoint *pl) : CRectLabel(pl)
{
   #ifdef CHECK_LABEL_CONSTRUCTION
   REPORT("CPlotLabel::Constructor(arg)");
   #endif
   InitPlot();
}

void CPlotLabel::InitPlot()
{
   LOGBRUSH lb = {BS_HOLLOW, 0, 0};
   LOGPEN   lp = {PS_NULL, {0, 0}, 0};
   // Default Überschriftattribute :
   m_PlotHeading.SetTextAlign(TA_LEFT|TA_TOP);                 // links oben im Plotrechteck
   m_PlotHeading.SetLogPen(&lp);                               // Nullstift
   m_PlotHeading.SetLogBrush(&lb);                             // kein Hintergrund
   // Defaultattribute für Gitterlinien im Viewbereich
   m_Gridlines_x.SetDeleteContent(true);                       // Linien bei Programmende löschen
   m_Gridlines_y.SetDeleteContent(true);

   // Defaultattribute für Gittertexte im Viewbereich
   m_Gridtext_x.SetDeleteContent(true);                        // Texte bei Programmende löschen
   m_Gridtext_y.SetDeleteContent(true);
   m_Unit_x.SetLogPen(&lp);                                    // kein Textrahmen
   m_Unit_y.SetLogPen(&lp);
   m_Unit_x.SetLogBrush(&lb);                                  // kein Hintergrund
   m_Unit_y.SetLogBrush(&lb);
   m_Unit_y.SetTextAlign(TA_TOP|TA_LEFT);                      // Y-Einheit links oben im Viewrechteck
   // Kurventeilung
   m_Parameter.xDivision = ETSDIV_LINEAR;                      // Achsenteilung
   m_Parameter.yDivision = ETSDIV_LINEAR;
   // Defaultattribute für View-Weltkoordinaten
   m_RangePos            = NULL;                               // Listenzeiger ist 0
   m_DefaultRange        = m_Range;
   SetClip(true);
   m_Curves.SetDeleteContent(true);                            // Kurven bei Programmende löschen
   m_bReferenceOn        = false;                              // Referenzkurve subtrahieren ist auf false
   m_pRefCurve           = NULL;                               // Referenzkurve
   m_pdOffset            = NULL;
   m_pdMarker            = NULL;
   m_bHeadingPicked      = false;
   SetStandards();
}

#ifdef _DEBUG
void CPlotLabel::AssertValid() const
{
   CLabel::AssertValid();
   ASSERT_VALID(&m_PlotHeading);
   ASSERT_VALID(&m_Gridlines_x);
   ASSERT_VALID(&m_Gridlines_y);
   ASSERT_VALID(&m_Gridtext_x);
   ASSERT_VALID(&m_Gridtext_y);
   ASSERT_VALID(&m_Unit_x);
   ASSERT_VALID(&m_Unit_y);
   ASSERT_VALID(&m_Curves);
   ASSERT_VALID(&m_RangeList);
   if (!m_RangeList.IsEmpty())                                 // Liste durchsuchen
   {
      POSITION  pos = m_RangeList.GetHeadPosition();
      RANGE    *pRtest;
      while (pos)
      {
         pRtest =(RANGE*) m_RangeList.GetPrev(pos);
         ASSERT(pRtest!=NULL);
         ASSERT(AfxIsMemoryBlock(pRtest, sizeof(RANGE)));
      }
   }
}
#endif
/******************************************************************************
* Name       : SetStandards                                                   *
* Definition : void SetStandards();                                           *
* Zweck      : Setzen der Standard-Ploteinstellungen für einen Plot           *
* Aufruf     : SetStandards();                                                *
* Parameter  :  keine                                                         *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CPlotLabel::SetStandards()
{
   // Überschrift
   m_PlotHeading.SetTextColor(gm_StdPltSets.sm_HeadingColor);  // Standardeinstellungen
   m_PlotHeading.SetLogFont(&gm_StdPltSets.sm_HeadingLogFont); // Standardeinstellungen
   // Gitterlinien
   m_GridLineColor = gm_StdPltSets.sm_GridLineColor;           // Standardeinstellungen
   m_wGridStep_x   = gm_StdPltSets.sm_wGridStep_x;             // Standardeinstellungen
   m_wGridStep_y   = gm_StdPltSets.sm_wGridStep_y;             // Standardeinstellungen
   // Gittertexte
   m_Unit_x.SetTextColor(gm_StdPltSets.sm_GridTextColor);      // Standardeinstellungen
   m_Unit_y.SetTextColor(gm_StdPltSets.sm_GridTextColor);      // Standardeinstellungen
   m_Unit_x.SetLogFont(&gm_StdPltSets.sm_GridTextLogFont);     // Standardeinstellungen
   m_Unit_y.SetLogFont(&gm_StdPltSets.sm_GridTextLogFont);     // Standardeinstellungen
   // Zahlendarstellung
   m_Parameter.xNumMode  = (gm_StdPltSets.sm_cNumMode_x&0x03); // Standardeinstellungen
   m_Parameter.yNumMode  = (gm_StdPltSets.sm_cNumMode_y&0x03); // Standardeinstellungen
   m_cRound_x            = gm_StdPltSets.sm_cRound_x;          // Standardeinstellungen
   m_cRound_y            = gm_StdPltSets.sm_cRound_y;          // Standardeinstellungen
   m_Parameter.xNoEndNulls = (gm_StdPltSets.sm_cNumMode_x&0x80) ? 1 : 0;
   m_Parameter.yNoEndNulls = (gm_StdPltSets.sm_cNumMode_y&0x80) ? 1 : 0;
   // Plotrahmen
   m_pPen    = CLabel::FindPen(&gm_StdPltSets.sm_PlotFrame);
   CRectLabel::SetLogBrush(&gm_StdPltSets.sm_PlotBrush);
   m_BkColor = gm_StdPltSets.sm_BkColor;
   // Kurventext
   m_CurveLogFont        = gm_StdPltSets.sm_CurveLogFont;      // Standardeinstellungen
   m_State.drawframe     = (unsigned char)gm_StdPltSets.sm_nDrawframe;
   // Kurvenfarben
   int i, count = m_Curves.GetCount();
   for (i=0; i<count; i++)
   {
      CCurveLabel* pcl = (CCurveLabel*) m_Curves.GetLabel(i);
      pcl->SetColor(gm_StdPltSets.sm_CurveColors[i%NUM_CURVE_COLORS]);
      pcl->SetLogFont(&m_CurveLogFont);
   }
}

CPlotLabel::~CPlotLabel()
{  
   #ifdef CHECK_LABEL_DESTRUCTION
   REPORT("CPlotLabel::Destructor");
   #endif
   DestroyRangeList();
   SAFE_DELETE(m_pdMarker);
}

/******************************************************************************
* Name       : Draw                                                           *
* Definition : virtual bool Draw(CDC *, bool)                                 *
* Zweck      : Zeichnet den Plot mit Überschrift, Kurven, Kurventexten,       *
*              Gitterlinien, Gittertexten, Einheiten.                         *
* Aufruf     : Draw(pDC, bAtr);                                               *
* Parameter  :                                                                *
* pDC    (E) : Zeiger auf Gerätekontextklasse                        (CDC*)   *
* bAtr   (E) : Attribute des Labels einstellen (1, 0)                (int)    *
* Funktionswert : Funktion ausgeführt (true, false)                  (bool)   *
******************************************************************************/
bool CPlotLabel::Draw(CDC *pDC, int bAtr)
{
   UpdateChanges(pDC);
   CRect rcPaint(0,0,0,0);
   if (pDC && pDC->IsKindOf(RUNTIME_CLASS(CPaintDC)))
   {
      rcPaint = (CRect) ((CPaintDC *) pDC)->m_ps.rcPaint;
      rcPaint.InflateRect(1,1);
   }

   bool bPicked = IsPicked();
   if (!m_State.previewmode) SetPicked(false);
   if (CLabel::Draw(pDC, 0))
   {
      LOGPEN plotframe = {0,{0,0},0};
      CPen *pExternalPen = NULL;
      if (bAtr!=0)
      {
         if (m_pPen) m_pPen->GetLogPen(&plotframe);
         if (m_State.drawbackgnd)
         {
            SelectBrush(pDC, m_pBrush);
            pDC->SetBkMode(OPAQUE);
            pDC->SetBkColor(m_BkColor);
            CLabel::Rectangle(pDC, m_pP[0].x, m_pP[0].y, m_pP[1].x, m_pP[1].y, true);
         }
         else
         {
            pDC->SetBkMode(TRANSPARENT);
         }
      }
      else
      {
         pExternalPen = (CPen*) pDC->SelectStockObject(NULL_PEN);// Plot-Hintergrund zeichnen
         if (pExternalPen) pExternalPen->GetLogPen(&plotframe);
         CLabel::Rectangle(pDC, m_pP[0].x, m_pP[0].y, m_pP[1].x, m_pP[1].y, true);
      }

      CRect unitxrect = m_Unit_x.GetRect(pDC);
      CRect unityrect = m_Unit_y.GetRect(pDC);
      pDC->DPtoLP(&rcPaint);                                   // Clipregion in Devicekoordinaten
      rcPaint.NormalizeRect();
      rcPaint.IntersectRect(&rcPaint, &m_View);
      if (unityrect.bottom > unitxrect.top)                    // den Viewbereich nur zeichnen, wenn auch alles hineinpaßt
      {
         m_PlotHeading.Draw(pDC);                              // Überschrift

         HRGN hOldRgn     = CreateRectRgn(0,0,0,0);            // Region erstellen zum ermitteln der im DC vorhandenen
         int  nClipResult = GetClipRgn(pDC->m_hDC, hOldRgn);   // alte Clipregion ermitteln

         DPOINT p1, p2;
         RANGE oldClip = m_ClippRange;
         // nicht Klippen wenn das Klipprechteck leer ist oder bei Referenzkurven
         bool bDifferent = false;
         ProcessLabel ProcL = {this, 0, &bDifferent, NULL};
         m_Curves.ProcessWithLabels(NULL, &ProcL, CPlotLabel::CheckCurveDivision);
         if (!pDC->IsPrinting() && !m_bReferenceOn && !m_pRefCurve && !bDifferent && !rcPaint.IsRectEmpty())
         {
            int nDiv = m_Parameter.xDivision;
            if (m_Parameter.xDivision == ETSDIV_POLAR) m_Parameter.xDivision = ETSDIV_LINEAR;
            TransformPoint(&rcPaint.TopLeft(),     &p1);
            TransformPoint(&rcPaint.BottomRight(), &p2);
            m_ClippRange = CTransformation::MakeRange(p1, p2);
            m_Parameter.xDivision = nDiv;
         }

         CRect viewrect = m_View;                              // Clippen am Viewrechteck
         viewrect.DeflateRect(plotframe.lopnWidth.x, plotframe.lopnWidth.x, plotframe.lopnWidth.x,-plotframe.lopnWidth.x);// am Rand Clippen
         ::LPtoDP(pDC->m_hDC, (LPPOINT)&viewrect, 2);

         if (pDC->m_hDC == pDC->m_hAttribDC)                   // dies ist der Normalfall. Anderfalls ist es ein MetaFileDC
         {
            CRgn cliprgn;                                      // CRgn Objekt
            cliprgn.CreateRectRgnIndirect(&viewrect);          // Cliping Region erstellen
            pDC->SelectClipRgn(&cliprgn, RGN_AND);             // Clipregion in den Devicekontext selectieren
            cliprgn.DeleteObject();
         }

         m_Gridlines_x.Draw(pDC);                              // Gitterlinien zeichnen
         m_Gridlines_y.Draw(pDC);

         m_Curves.Draw(pDC, 1);                                // Kurven zeichnen

         m_Gridtext_x.Draw(pDC);                               // Beschriftungstexte ausgeben
         m_Gridtext_y.Draw(pDC);
         if (nClipResult == 1)                                 // es gab eine alte Region
         {
            nClipResult = SelectClipRgn(pDC->m_hDC, hOldRgn);  // alte Clipregion wiederherstellen 
            ASSERT(nClipResult!=ERROR);
         }
         else if (nClipResult == 0)
         {
            pDC->SelectClipRgn(NULL);
         }
         DeleteObject(hOldRgn);                                // und löschen

         if ((m_Parameter.xDivision == ETSDIV_POLAR)||(unityrect.top > unitxrect.bottom))
         {
            m_Unit_x.Draw(pDC);                                // Einheiten der Kurven
            m_Unit_y.Draw(pDC);
         }

         m_Curves.Draw(pDC, 2);                                // Kurventexte zeichnen

         if (bAtr==1)
         {
            SelectPen(pDC, m_pPen);
         }
         else
         {
            pDC->SelectObject(pExternalPen);                   // pExternalPen sichert ein von Außen gesetztes Pen-Object
         }

         pDC->SetBkMode(TRANSPARENT);
         pDC->SelectStockObject(HOLLOW_BRUSH);

         if (m_pdMarker)
         {
            CPoint pt;
            DPOINT dp;
            pDC->SelectStockObject(BLACK_PEN);
            dp.y = m_ClippRange.y1;

            dp.x = m_pdMarker[0];
            TransformPoint(&dp, &pt);
            pt.y = m_View.bottom;
            pDC->MoveTo(pt);
            pt.y = m_View.top;
            pDC->LineTo(pt);

            dp.x = m_pdMarker[1];
            TransformPoint(&dp, &pt);
            pt.y = m_View.bottom;
            pDC->MoveTo(pt);
            pt.y = m_View.top;
            pDC->LineTo(pt);
         }                         
         pDC->MoveTo(m_pP[0].x, m_View.top);
         pDC->LineTo(m_pP[1].x, m_View.top);
         pDC->MoveTo(m_pP[0].x, m_View.bottom);
         pDC->LineTo(m_pP[1].x, m_View.bottom);

         if (m_Parameter.xDivision == ETSDIV_POLAR)
         {
            if (abs(m_pP[1].x - m_pP[0].x) > abs(m_pP[1].y - m_pP[0].y))
            {
               pDC->MoveTo(m_View.right, m_View.top);
               pDC->LineTo(m_View.right, m_View.bottom);
            }
            else
            {
               pDC->MoveTo(m_View.left , m_View.bottom);
               pDC->LineTo(m_View.right, m_View.bottom);
            }
         }
         m_ClippRange = oldClip;
      }

      CLabel::Rectangle(pDC, m_pP[0].x, m_pP[0].y, m_pP[1].x, m_pP[1].y, false);
      if (bPicked)
      {
         SetPicked(true);
         DrawShape(pDC, true);
      }
      return true;
   }
   SetPicked(bPicked);
   return false;
}   

/******************************************************************************
* Name       : DrawShape                                                      *
* Definition : bool DrawShape(CDC*, bool);                                    *
* Zweck      : Zeichnen des Umrisses eines markierten Plots                   *
* Aufruf     : DrawShape(pDC, bdraw);                                         *
* Parameter  :                                                                *
* pDC    (E) : Zeiger auf Gerätekontextklasse                     (CDC*)      *
* bDraw  (E) : Umriß zeichnen (true) oder löschen (false).        (bool)      *
* Funktionswert : Funktion ausgeführt (true, false)               (bool)      *
******************************************************************************/
bool CPlotLabel::DrawShape(CDC* pDC, bool bdraw)
{
   if (m_State.previewmode) return CRectLabel::DrawShape(pDC, bdraw);// nur den Rahmen mit Pickpunkten zeichnen

   if (!CLabel::DrawShape(pDC, bdraw)) return false;
   LOGPEN plotframe = {0,{0,0},0};
   if (m_pPen) m_pPen->GetLogPen(&plotframe);
   CPoint one        = CLabel::PixelToLogical(pDC, 1, 1);
   CPoint two        = CLabel::PixelToLogical(pDC, 2, 2);
   CPoint framewidth = CLabel::PixelToLogical(pDC,plotframe.lopnWidth.x,plotframe.lopnWidth.x);
   framewidth.x = MulDiv(framewidth.x, 35, 100);
   framewidth.y = MulDiv(framewidth.y, 35, 100);
   pDC->SaveDC();

   CRect plotrect    = GetRect(pDC);
   CRect headingrect = m_PlotHeading.GetRect(pDC);
   const TCHAR * heading    = m_PlotHeading.GetText();
   COLORREF color = m_PlotHeading.GetTextColor();
   CBrush  brush(RGB(255-GetRValue(color), 255-GetGValue(color), 255-GetBValue(color)));
   pDC->SelectObject(&brush);
   if (heading && _tcsclen(heading) && (headingrect.top!=headingrect.bottom))
   {
      headingrect.top  += framewidth.y;
      headingrect.left  = plotrect.left  + (framewidth.x<<1);
      headingrect.right = plotrect.right - (framewidth.x<<1);
      pDC->PatBlt(headingrect.left,headingrect.top,headingrect.right-headingrect.left,headingrect.bottom-headingrect.top,PATINVERT);
   }
   else                                                        // Ist keine Überschrift da
   {                                                           // Plot invertieren
      pDC->PatBlt(m_View.left,m_View.top,m_View.right-m_View.left,m_View.bottom-m_View.top,PATINVERT);
   }
   pDC->RestoreDC(-1);
   return true;
}

/******************************************************************************
* Name       : UpdateChanges                                                  *
* Definition : void UpdateChanges(CDC *pDC);                                  *
* Zweck      : Aktualisierung aller Abhängigkeiten von der Größe und          *
*              Positionierung des Plots und des Bereiches des Plots.          *
* Aufruf     : UpdateChanges(pDC);                                            *
* Parameter  :                                                                *
* pDC    (E) : Zeiger auf Gerätekontextklasse                     (CDC*)      *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CPlotLabel::UpdateChanges(CDC *pDC)
{
   DeterminViewRect(pDC);
   if (m_bChanged)
   {
      RANGE oldRange = m_Range;
      if (m_Parameter.xDivision == ETSDIV_POLAR)
      {
         m_Range = m_ClippRange; // Polarkoordinatentransformationsbereich setzen
      }
      SetDevicetran();
      if (m_Parameter.xDivision == ETSDIV_POLAR) m_Range = oldRange;

      CalculateViewDivision(pDC);
   }
}

void CPlotLabel::SetGroupXNumbers(bool bG)
{
   if (bG) m_cRound_x |=  0x80;
   else    m_cRound_x &= ~0x80;
}

void CPlotLabel::SetGroupYNumbers(bool bG)
{
   if (bG) m_cRound_y |=  0x80;
   else    m_cRound_y &= ~0x80;
}

void CPlotLabel::SetLocaleXNumbers(bool bG)
{
   if (bG) m_cRound_x |=  0x40;
   else    m_cRound_x &= ~0x40;
}

void CPlotLabel::SetLocaleYNumbers(bool bG)
{
   if (bG) m_cRound_y |=  0x40;
   else    m_cRound_y &= ~0x40;
}

void CPlotLabel::SetXNumModeEx(int nMode)
{
   SetXNumMode(LOWORD(nMode));
   SetXNoEndNulls(  (nMode&ETSDIV_NM_NO_END_NULLS)?true:false);
   SetGroupXNumbers((nMode&ETSDIV_NM_GROUPING    )?true:false);
   SetLocaleXNumbers((nMode&ETSDIV_NM_LOCALE      )?true:false);
}

int  CPlotLabel::GetXNumModeEx()
{
   int nMode = GetXNumMode();
   if (nMode == ETSDIV_NM_STD) nMode |= ETSDIV_NM_AUTO_SCI;
   if (NoXEndNulls())          nMode |= ETSDIV_NM_NO_END_NULLS;
   if (LocaleXNumbers())       nMode |= ETSDIV_NM_LOCALE;
   if (GroupXNumbers())        nMode |= ETSDIV_NM_GROUPING;
   return nMode;
}

void CPlotLabel::SetYNumModeEx(int nMode)
{
   SetYNumMode(LOWORD(nMode));
   SetYNoEndNulls(   (nMode&ETSDIV_NM_NO_END_NULLS)?true:false);
   SetGroupYNumbers( (nMode&ETSDIV_NM_GROUPING    )?true:false);
   SetLocaleYNumbers((nMode&ETSDIV_NM_LOCALE      )?true:false);
}

int  CPlotLabel::GetYNumModeEx()
{
   int nMode = GetYNumMode();
   if (nMode == ETSDIV_NM_STD) nMode |= ETSDIV_NM_AUTO_SCI;
   if (NoYEndNulls())          nMode |= ETSDIV_NM_NO_END_NULLS;
   if (LocaleYNumbers())       nMode |= ETSDIV_NM_LOCALE;
   if (GroupYNumbers())        nMode |= ETSDIV_NM_GROUPING;
   return nMode;
}

/******************************************************************************
* Name       : DeterminDefaultRange                                           *
* Definition : bool DeterminDefaultRange();                                   *
* Zweck      : Ermittelt den Defaultbereich aus allen eingefügten Kurven.     *
* Aufruf     : DeterminDefaultRange();                                        *
* Parameter  : keine                                                          *
* Funktionswert : Defaultbereich gesetzt (true, false)                 (bool) *
******************************************************************************/
bool CPlotLabel::DeterminDefaultRange()
{
   ETSDIV_DIV  div;
   RANGE range = DeterminRange();
   if ((range.x2 - range.x1) < 1.0e-13)
   {
      range.x1 -= 0.001;
      range.x2 += 0.001;
   }
   if ((range.y2 - range.y1) < 1.0e-13)
   {
      range.y1 -= 0.001;
      range.y2 += 0.001;
   }

   ZeroMemory(&div, sizeof(ETSDIV_DIV));
   div.f1     = range.x1;
   div.f2     = range.x2;
   div.divfnc = m_Defaults.xDivision;
   switch(div.divfnc)
   {
      case ETSDIV_LINEAR: case ETSDIV_POLAR: 
         div.stepwidth = 10;
         break;
      case ETSDIV_LOGARITHMIC:
         break;
      case ETSDIV_FREQUENCY:
         div.stepwidth = 2;
         break;
   }
   if (CEtsDiv::RoundMinMaxValues(&div))
   {
      range.x1 = div.f1;
      range.x2 = div.f2;
   }

   ZeroMemory(&div, sizeof(ETSDIV_DIV));
   div.f1     = range.y1;
   div.f2     = range.y2;
   div.divfnc = m_Defaults.yDivision;
   switch(div.divfnc)
   {
      case ETSDIV_LINEAR: 
         div.stepwidth = 10;
         break;
      case ETSDIV_LOGARITHMIC:
         break;
      case ETSDIV_FREQUENCY:
         div.stepwidth = 2;
         break;
   }
   if (CEtsDiv::RoundMinMaxValues(&div))
   {
      range.y1 = div.f1;
      range.y2 = div.f2;
   }

   RANGE oldRange = GetRange();
   DestroyRangeList();
   if (SetRange(&range))   return true;                        // ermittelten Bereich setzen
   else                                                        // bei Fehler
   {
      //AfxMessageBox(IDE_DEFAULT_RANGE_ERROR, MB_OK|MB_ICONEXCLAMATION);
      return SetRange(&oldRange);                              // alten Bereich setzen
   }
}
/******************************************************************************
* Name       : DeterminRange                                                  *
* Definition : RANGE DeterminRange();                                         *
* Zweck      : Bestimmen der minimal- und maximal-Bereiche des Plots aus den  *
*              Wertebereichen der vorhandenen Kurven.                         *
* Aufruf     : DeterminRange();                                               *
* Parameter  :                                                                *
* Funktionswert : Ein Bereich, der alle Kurvenwerte einschließt (RANGE)       *                                                      *
******************************************************************************/
RANGE CPlotLabel::DeterminRange()
{
   int i, numCurves = m_Curves.GetCount();
   double temp, dOffset;
   RANGE  range= {0,0,0,0};
   CCurveLabel *pcl = (CCurveLabel*) m_Curves.GetLabel(0);

   if (pcl)
   {
      dOffset  = pcl->GetOffset();
      range.x2 = pcl->GetXMax();
      range.x1 = pcl->GetXMin();
      range.y2 = pcl->GetYMax()+dOffset;
      range.y1 = pcl->GetYMin()+dOffset;
   }
   for (i=1; i<numCurves; i++)
   {
      pcl = (CCurveLabel*)m_Curves.GetLabel(i);
      dOffset = pcl->GetOffset();
      if ((temp=pcl->GetXMax()) > range.x2) range.x2 = temp;
      if ((temp=pcl->GetXMin()) < range.x1) range.x1 = temp;
      if ((temp=pcl->GetYMax()+dOffset) > range.y2) range.y2 = temp;
      if ((temp=pcl->GetYMin()+dOffset) < range.y1) range.y1 = temp;
   }
   return range;
}
/******************************************************************************
* Name       : DeterminViewRect                                               *
* Definition : void DeterminViewRect(CDC *);                                  *
* Zweck      : Ermitteln der Größe des Viewrechtecks im Plot abhängig von der *
*              Überschrift und den Kurventexten.                              *
* Aufruf     : DeterminViewRect(pDC);                                         *
* Parameter  :                                                                *
* pDC    (E) : Zeiger auf Gerätekontextklasse                        (CDC*)   *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CPlotLabel::DeterminViewRect(CDC *pDC)
{
   if ((!IsChanged()) || (!pDC)) return;                       // wenn nichts geändert wurde, nichts tun
//   REPORT("DeterminViewRect(%d)", pDC);
   CRect  newview = CLabel::GetRect(pDC);
   CPoint one     = CLabel::PixelToLogical(pDC, 1, 1);
   CPoint two     = CLabel::PixelToLogical(pDC, 2, 2);
   LOGPEN plotframe = {0,{0,0},0};
   if (m_pPen) m_pPen->GetLogPen(&plotframe);
   CPoint   framewidth(plotframe.lopnWidth.x,-plotframe.lopnWidth.x);

   const TCHAR *heading = m_PlotHeading.GetText();
   if (heading && _tcsclen(heading))                             // Überschrift berücksichtigen
   {
      UINT align = m_PlotHeading.GetTextAlign();
      if      ((align&TA_CENTER) != 0) align = (newview.left + newview.right)/2;
      else if ((align&TA_RIGHT)  != 0) align = newview.right-two.x-framewidth.x;
      else                             align = newview.left +two.x+framewidth.x;
      m_PlotHeading.SetPoint(CPoint(align, newview.bottom+one.y+framewidth.y));
      m_PlotHeading.SetClipRect(NULL);
      CRect headingrect = m_PlotHeading.GetRect(pDC);
      if (headingrect.Width() > newview.Width())
      {
         CRect cliprect(headingrect);
         cliprect.left  = newview.left;
         cliprect.right = newview.right;
         m_PlotHeading.SetClipRect(&cliprect, true);
      }
      newview.bottom = headingrect.top + one.y;
   }

   newview.top = DeterminCurveTextPositions(pDC);

   if (m_Parameter.xDivision == CCURVE_POLAR)                  // Beim Polarview
   {  // muß der Viewbereich immer quadratisch sein, das sonst die konzentrischen Kreise
      int cx = newview.Width(), cy = abs(newview.Height());    // zu konzentrischen Ellipsen werden
      if (cx > cy) newview.right = newview.left + cy;
      else         newview.top = newview.bottom - cx;
   }
   m_Unit_x.SetTextAlign(TA_BOTTOM|TA_RIGHT);                  // X-Einheit rechts unten im Viewrechteck
   m_Unit_y.SetPoint(CPoint(newview.left  + two.x + framewidth.x, newview.bottom + two.y + framewidth.y));
   m_Unit_x.SetPoint(CPoint(newview.right - two.x - framewidth.x, newview.top    - two.y - framewidth.y));
   SetView(&newview);
   SetChanged(false);                                          // Änderungen angepaßt
}

/******************************************************************************
* Name       : DeterminCurveTextPositions                                     *
* Definition : int DeterminCurveTextPositions(CDC *);                         *
* Zweck      : Ermittelt die Positionen der Kurventexte und clippt die Texte, *
*              wenn nötig.                                                    *
* Aufruf     : DeterminCurveTextPositions(pDC);                               *
* Parameter  :                                                                *
* pDC    (E) : Zeiger auf Gerätekontextklasse                        (CDC*)   *
* Funktionswert : Obere Kante der Kurventexte                        (int)    *
******************************************************************************/
int CPlotLabel::DeterminCurveTextPositions(CDC *pDC)
{
   CRect    plot = GetRect(pDC);

   CPoint   two   = CLabel::PixelToLogical(pDC, 2, 2);
   CPoint   three = CLabel::PixelToLogical(pDC, 3, 3);
   LOGPEN   plotframe = {0,{0,0},0};
   if (m_pPen) m_pPen->GetLogPen(&plotframe);
   CPoint   framewidth(plotframe.lopnWidth.x,-plotframe.lopnWidth.x);

   CPoint   curvetextpos(plot.left+three.x+framewidth.x, plot.top-framewidth.y-three.y);

   int      count = 0, nvisiblecurves=0;
   m_Curves.ProcessWithLabels(NULL, &count, CLabelContainer::CountLabel);
   m_Curves.ProcessWithLabels(NULL, &nvisiblecurves, CLabelContainer::CountVisibleLabel);

   if (count && nvisiblecurves)                                // für die Anzahl der Kurventexte
   {
      CCurveLabel *ptl(0);
      CRect        textrect;
      int  CurvetextUpper = plot.top;;
      int  nCurvesLeft    = nvisiblecurves;
      int  nPlotSize      = (plot.right-plot.left) - (nCurvesLeft+1)*three.x;
      int  maxtextlen     = nPlotSize / nCurvesLeft;
      int  i, nSizeX, nClip = 0;

      for (i=0; i<count; i++)
      {
         ptl = (CCurveLabel*) m_Curves.GetLabel(i);
         if (!ptl->IsVisible()) continue;
         ptl->SetDrawFrame(m_State.drawframe);
         ptl->SetPoint(curvetextpos);
         ptl->SetClipRect(NULL, false);
         textrect = ptl->GetRect(pDC);
         nSizeX   = textrect.Size().cx;
         if (CurvetextUpper < textrect.bottom) CurvetextUpper = textrect.bottom;
         if (nSizeX < maxtextlen)
         {
            curvetextpos.x = textrect.right+three.x;
            nCurvesLeft--;
            nPlotSize -= (nSizeX-three.x);
            if (nCurvesLeft > 0) maxtextlen = nPlotSize / nCurvesLeft;
         }
         else nClip++;
      }

      if (nClip > 0)                                           // sind die Kurventexte zu lang
      {                              
         if (nClip == nvisiblecurves)                          // maximale Länge neu berechnen, wenn alle geclippt werden
            maxtextlen = ((plot.right-plot.left) - (nvisiblecurves+1)*three.x) / nvisiblecurves;
         curvetextpos.x = plot.left+three.x+framewidth.x;      // vom linken Rand anfangen
         for (i=0; i<count; i++)
         {
            ptl = (CCurveLabel*) m_Curves.GetLabel(i);
            if (!ptl->IsVisible()) continue;
            ptl->SetPoint(curvetextpos);
            textrect = ptl->GetRect(pDC);
            if (CurvetextUpper < textrect.bottom) CurvetextUpper = textrect.bottom;
            if (textrect.Size().cx > maxtextlen)
            {
               textrect.right = textrect.left+maxtextlen+three.x;
               ptl->SetClipRect(&textrect, true);
            }
            curvetextpos.x = textrect.right+three.x;
         }
         if (textrect.right > plot.right-three.x)
         {
            textrect.right =  plot.right-three.x;
            ptl->SetClipRect(&textrect, true);
         }
      }
      return CurvetextUpper-two.y;                             // Viewrect bottom-Position zurückgeben
   }
   else return plot.top;
}
#define MAXGRIDTEXTLEN 40
/******************************************************************************
* Name       : CalculateViewDivision                                          *
* Definition : bool CalculateViewDivision(CDC*);                              *
* Zweck      : Berechnet die Aufteilung der Gitterlinien und die Position der *
*              Texte zur Beschriftung derselben.                              *
* Aufruf     : CalculateViewDivision(pDC);                                    *
* Parameter  :                                                                *
* pDC    (E) : Zeiger auf Gerätekontextklasse                        (CDC*)   *
* Funktionswert : Funktion ausgeführt (true, false)                  (bool)   *
******************************************************************************/
bool CPlotLabel::CalculateViewDivision(CDC* pDC)
{
   DPOINT         dp;
   ETSDIV_DIV     div;
   ETSDIV_NUTOTXT ntx = {0};
   CPoint two = CLabel::PixelToLogical(pDC, 2, 2), lp;
   CRect  textrect, testrect,
          unitxrect  = m_Unit_x.GetRect(pDC),
          unityrect  = m_Unit_y.GetRect(pDC);
   double fStep, fStart, fLogVal, fEnd;
   int    i, j, ntext, nline, ntest, ytextposleft, maxtextwidth, ncut(0), noff(0);
   bool   bFrequ;
   LOGPEN lpsolid   = {PS_SOLID,   {1,1}, m_GridLineColor};
   LOGPEN lpdot     = {PS_DOT,     {1,1}, m_GridLineColor};
   LOGPEN lpdash    = {PS_DASH,    {1,1}, m_GridLineColor};
   LOGPEN lpdashdot = {PS_DASHDOT, {1,1}, m_GridLineColor};
   LOGPEN plotframe = {0,{0,0},0};
   if (m_pPen) m_pPen->GetLogPen(&plotframe);
   CPoint      framewidth(plotframe.lopnWidth.x,-plotframe.lopnWidth.x);
   CTextLabel   *ptl, dummytext(&CPoint(0,0), _T("M"));
   CLineLabel   *pll, dummyline;
   CCircleLabel *pcl, dummycircle;
   CLabelContainer lcut;
   bool    bRefOn = m_bReferenceOn;
   m_bReferenceOn = false;
   CPoint        ptM, ptC;
   int           nSizey;

   CLabel::SetSaveGlobal(true);
   LOGFONT  oldGridLF      = dummytext.GetLogFont();
   LOGPEN   oldGridTextLP  = dummytext.GetLogPen();
   COLORREF oldTextColor   = dummytext.GetTextColor();
   COLORREF oldTextBkColor = dummytext.GetBkColor();
   UINT     oldTextAlign   = dummytext.GetTextAlign();
   BSState  oldTextState   = dummytext.GetStates();
   LOGPEN   oldGridLineLP  = dummyline.GetLogPen();
   LOGPEN   oldGridCircleLP= dummycircle.GetLogPen();
   dummytext.SetLogFont(&m_Unit_x.GetLogFont());
   dummytext.SetLogPen(&m_Unit_x.GetLogPen());
   dummytext.SetTextColor(m_Unit_x.GetTextColor());
   dummytext.SetDrawBkGnd(false);
   dummytext.SetDrawFrame(false);
   dummytext.SetLogBrush(NULL);
   dummyline.SetLogPen(&lpdot);
   dummycircle.SetLogPen(&lpsolid);
   m_Gridlines_x.DeleteAll();
   m_Gridlines_y.DeleteAll();
   m_Gridtext_x.DeleteAll();
   m_Gridtext_y.DeleteAll();

/************************************************************************************************
* Aufteilung fuer die Y-Achse                                                                   *
************************************************************************************************/
   ZeroMemory(&div, sizeof(ETSDIV_DIV));
   div.stepwidth = m_wGridStep_y & GRID_LINE_MASK;             // Schrittweite einstellbar
   if (pDC->GetMapMode() == MM_HIMETRIC) div.stepwidth *= 10;
   div.f1        = m_Range.y1;
   div.f2        = m_Range.y2;
   bFrequ        = false;
   ntx.nmaxl     = MAXGRIDTEXTLEN;
   ntx.nround    = GetYRound();                                 // Zahlenausgabe einstellbar
   ntx.nmode     = GetYNumModeEx();

   ntx.dZero     = 0;
   dp.x          = 1;
   nSizey        = abs(unityrect.Height());
   dummytext.SetTextAlign(TA_VCENTER|TA_LEFT);

   if (m_Parameter.xDivision == ETSDIV_POLAR)
   {
      ptM = CPoint((m_View.left+m_View.right)>>1, (m_View.top+m_View.bottom)>>1);
      dummytext.SetTextAlign(TA_BOTTOM|TA_LEFT);
      unityrect.right  = m_View.right;
      unityrect.left   = m_View.left;
      unityrect.bottom = m_View.bottom;
      unityrect.top    = m_View.bottom+two.y;
      lp.x  = ptM.x + two.x;
      div.l1 = 0;
      div.l2 = m_View.Width() >> 1;
   }
   else
   {
      div.l1        = m_View.top;
      div.l2        = m_View.bottom;
   }

   CFloatPrecision _fp;
   if (m_Parameter.yDivision == ETSDIV_LINEAR)
   {
      if (CEtsDiv::CalculateLinDivision(&div, fStep, fStart))
      {
         ntx.dZero = div.dZero;
         for (ntx.fvalue=fStart; ntx.fvalue < m_Range.y2; ntx.fvalue+=fStep)
         {  
            if (m_Parameter.xDivision == ETSDIV_POLAR)
            {
               dp.x = dp.y = ntx.fvalue - m_Range.y1;
               if (dp.x < 0) continue;
               m_Parameter.xDivision = ETSDIV_LINEAR;
               TransformPoint(&dp, &ptC);
               m_Parameter.xDivision = ETSDIV_POLAR;
               if (ptC.x > m_View.right) break;
               lp.y = ptC.y;
               Set_Y_DivText(&ntx, &unityrect, &lp, pDC);
               if (m_wGridStep_y & HIDE_GRID_LINE) continue;
               ptC = ptM - ptC;
               CRect rcCircle(ptM.x - ptC.x, ptM.y + ptC.y, ptM.x + ptC.x, ptM.y - ptC.y);
               pcl = new CCircleLabel(&rcCircle);
               pcl->SetdValue(ntx.fvalue);
               m_Gridlines_y.InsertLabel(pcl);
            }
            else
            {
               dp.y = ntx.fvalue;
               TransformPoint(&dp, &lp);
               if (lp.y < unitxrect.bottom) continue;
               lp.x = m_View.left + two.x + framewidth.x;
               Set_Y_DivText(&ntx, &unityrect, &lp, pDC);
               if ((lp.y > unityrect.top) && (lp.y < unityrect.bottom)) lp.x = unityrect.right;
               if (m_wGridStep_y & HIDE_GRID_LINE) continue;
               pll = new CLineLabel(&lp);
               pll->SetdValue(ntx.fvalue);
               lp.x = m_View.right;
               pll->SetPoint(lp,1);
               m_Gridlines_y.InsertLabel(pll);
            }
         }
         if (m_Parameter.xDivision == ETSDIV_POLAR)
            ntx.fvalue -= fStep;                               // Äußeren Ring merken

         ncut = CEtsDiv::Round(0.5+(double)nSizey/(double)div.stepwidth);
         if (ncut<=1) ncut = 0;
         noff = 0;
      }
      else
      {
         REPORT("%s(%d) :Error %x:", __FILE__, __LINE__, GetLastError());
      }
   }
   else if (m_Parameter.yDivision == ETSDIV_LOGARITHMIC)
   {
      if ((m_Range.y2-m_Range.y1) > 0.8)
      {
         if (CEtsDiv::CalculateLogDivision(&div, fStep, fStart))
         {
            int textheight = unitxrect.Size().cy;
            fEnd = pow((double)10, m_Range.y2);
            for (ntx.fvalue=fStart; ntx.fvalue < fEnd; ntx.fvalue+=fStep)
            {
               if (m_Parameter.xDivision == ETSDIV_POLAR)
               {
                  CPoint ptTemp;
                  dp.y = ntx.fvalue;
                  dp.x = 0;
                  TransformPoint(&dp, &ptC);
                  if (ptC.y > m_View.bottom) break;
                  lp.y = ptC.y;
                  ptC = ptM - ptC;
                  if (ptC.y >= 0) continue;
                  ntest = CEtsDiv::TestLogValue(log10(ntx.fvalue));
                  if ((ntest==1)&&(fStep < ntx.fvalue)) fStep *= 10.0;
                  if (ntest==1)                                   // 10'er Schritte immer
                     Set_Y_DivText(&ntx, &unityrect, &lp, pDC);
                  if ((textheight<div.width)&&((ntest==2)||(ntest==5)))// 2'er und 5'er wenn der Platz reicht
                     Set_Y_DivText(&ntx, &unityrect, &lp, pDC);
                  if ((textheight*7 < div.width)&&(!ntest))       // alle anderen wenn der Platz reicht
                     Set_Y_DivText(&ntx, &unityrect, &lp, pDC);
                  if (m_wGridStep_y & HIDE_GRID_LINE) continue;
                  CRect rcCircle(ptM.x - ptC.y, ptM.y + ptC.y, ptM.x + ptC.y, ptM.y - ptC.y);
                  pcl = new CCircleLabel(&rcCircle);
                  pcl->SetdValue(ntx.fvalue);
                  m_Gridlines_y.InsertLabel(pcl);
               }
               else
               {
                  dp.y = ntx.fvalue;
                  TransformPoint(&dp, &lp);
                  lp.x = m_View.left + two.x + framewidth.x;
                  ntest = CEtsDiv::TestLogValue(log10(ntx.fvalue));
                  if ((ntest==1)&&(fStep < ntx.fvalue)) fStep *= 10.0;
                  if (lp.y < unitxrect.bottom) continue;
                  if (ntest==1)                                   // 10'er Schritte immer
                     Set_Y_DivText(&ntx, &unityrect, &lp, pDC);
                  if ((textheight<div.width)&&((ntest==2)||(ntest==5)))// 2'er und 5'er wenn der Platz reicht
                     Set_Y_DivText(&ntx, &unityrect, &lp, pDC);
                  if ((textheight*7 < div.width)&&(!ntest))       // alle anderen wenn der Platz reicht
                     Set_Y_DivText(&ntx, &unityrect, &lp, pDC);
                  if ((lp.y > unityrect.top) && (lp.y < unityrect.bottom)) lp.x = unityrect.right;
                  if (m_wGridStep_y & HIDE_GRID_LINE) continue;
                  pll = new CLineLabel(&lp);
                  pll->SetdValue(ntx.fvalue);
                  lp.x = m_View.right;
                  pll->SetPoint(lp,1);
                  m_Gridlines_y.InsertLabel(pll);
               }
            }
            if (m_Parameter.xDivision == ETSDIV_POLAR)
               ntx.fvalue -= fStep;                               // Äußeren Ring merken
            ncut = 0;
         }
      }
      else bFrequ = true;
   }
   if (bFrequ || m_Parameter.yDivision == ETSDIV_FREQUENCY)
   {
      if (CEtsDiv::CalculateFrqDivision(&div, fStep, fStart))
      {
         int nokt =-1, nterz=-1;
         for (fLogVal=fStart, i=0; fLogVal < m_Range.y2; fLogVal+=fStep)
         {
            if (m_Parameter.xDivision == ETSDIV_POLAR)
            {
               ntx.fvalue =pow((double)10, fLogVal);
               dp.x = 0;
               dp.y = ntx.fvalue;
               TransformPoint(&dp, &ptC);
               if (ptC.y > m_View.bottom) break;
               lp.y = ptC.y;
               ptC = ptM - ptC;
               if (ptC.y >= 0) continue;
               Set_Y_DivText(&ntx, &unityrect, &lp, pDC);
               if (m_wGridStep_y & HIDE_GRID_LINE) continue;
               CRect rcCircle(ptM.x - ptC.y, ptM.y + ptC.y, ptM.x + ptC.y, ptM.y - ptC.y);
               pcl = new CCircleLabel(&rcCircle);
               pcl->SetdValue(ntx.fvalue);
               m_Gridlines_y.InsertLabel(pcl);
               ntest = CEtsDiv::TestFrqValue(fLogVal);
               if      (ntest==3) {pcl->SetLogPen(&lpdashdot); if (nterz==-1) nterz=i;}
               else if (ntest==2) {pcl->SetLogPen(&lpdash);    if (nokt ==-1) nokt =i;}
               else                pcl->SetLogPen(&lpdot);
               i++;
            }
            else
            {
               ntx.fvalue =pow((double)10, fLogVal);
               dp.y = ntx.fvalue;
               TransformPoint(&dp, &lp);
               if (lp.y < unitxrect.bottom) continue;
               lp.x = m_View.left + two.x + framewidth.x;
               Set_Y_DivText(&ntx, &unityrect, &lp, pDC);
               if ((lp.y > unityrect.top) && (lp.y < unityrect.bottom)) lp.x = unityrect.right;
               if (m_wGridStep_y & HIDE_GRID_LINE) continue;
               pll = new CLineLabel(&lp);
               pll->SetdValue(ntx.fvalue);
               ntest = CEtsDiv::TestFrqValue(fLogVal);
               if      (ntest==3) {pll->SetLogPen(&lpdashdot); if (nterz==-1) nterz=i;}
               else if (ntest==2) {pll->SetLogPen(&lpdash);    if (nokt ==-1) nokt =i;}
               else                pll->SetLogPen(&lpdot);
               lp.x = m_View.right;
               pll->SetPoint(lp,1);
               m_Gridlines_y.InsertLabel(pll);
               i++;
            }
         }

         if      (nokt !=-1) noff = nokt;                         // Beschriftung orientiert sich stärker an Oktaven
         else if (nterz!=-1) noff = nterz;                        // als an Terzen
         else noff = 0;                                           // keine echten Terzen oder Oktaven : egal

         ncut = CEtsDiv::Round(0.5+(double)unityrect.Size().cy/(double)div.stepwidth);
         if (ncut<=1) ncut = 0;                                   // paßt eine Texthöhe zwischen zwei Linien
         else
         {
            if((div.division%3)==0) ncut = CEtsDiv::Teiler(ncut, 3);   // geeignete Teilung ermitteln
            else               ncut = CEtsDiv::Teiler(ncut, 2);
            noff%=ncut;
         }
      }
      else
      {
         REPORT("%s(%d) :Error %x:", __FILE__, __LINE__, GetLastError());
      }
   }

   ntext = m_Gridtext_y.GetCount();
   ytextposleft = m_View.left;
   for (i=0; i<ntext; i++)
   {
      ptl = (CTextLabel*) m_Gridtext_y.GetLabel(i);
      if (ncut)
      {
         ntest = i;
         ntest  %= ncut;
         if (ntest!=noff) {lcut.InsertLabel(ptl); continue;}
      }
      if (m_Parameter.yDivision == ETSDIV_POLAR) continue;
      textrect = ptl->GetRect(pDC);
      nline = m_Gridlines_y.GetCount();
      for (j=0; j<nline; j++)
      {
         pll = (CLineLabel*) m_Gridlines_y.GetLabel(j);
         lp = pll->GetPoint(0);
         if ((lp.y < textrect.bottom) && (lp.y > textrect.top))
         {
            lp.x = textrect.right;
            if (lp.x > ytextposleft) ytextposleft = lp.x;
            pll->SetPoint(lp, 0);
         }                  
      }
   }
   m_Gridtext_y.RemoveList(&lcut);
   lcut.DeleteAll();
/************************************************************************************************
* Aufteilung fuer die X-Achse                                                                   *
************************************************************************************************/
   div.l1        = m_View.left;
   div.l2        = m_View.right;
   div.stepwidth = m_wGridStep_x & GRID_LINE_MASK;             // Schrittweite einstellbar
   if (pDC->GetMapMode() == MM_HIMETRIC) div.stepwidth *= 10;
   div.f1        = m_Range.x1;
   div.f2        = m_Range.x2;
   bFrequ        = false;

   ntx.nround    = GetXRound();                                // Zahlenausgabe einstellbar
   ntx.nmode     = GetXNumModeEx();

   ntx.dZero     = 0;
   dummytext.SetTextAlign(TA_BOTTOM|TA_CENTER);

   dp.y = 1;
   maxtextwidth = 0;
   if (m_Parameter.xDivision == ETSDIV_POLAR)
   {
      if (ntx.fvalue > 0)
      {                                                        // Mittelpunkt
         CPoint ptM((m_View.left+m_View.right)>>1, (m_View.top+m_View.bottom)>>1);// Äußeren Ring merken
         double dMaxRadius;
         if (m_Parameter.yDivision == ETSDIV_LINEAR) dMaxRadius = ntx.fvalue - m_Range.y1;
         else                                        dMaxRadius = log10(ntx.fvalue) - m_Range.y1;
         bool   bText(false), bSolid, bDrawText = (m_Gridtext_y.GetCount() != 0)? true:false;
         int nIndex   = CEtsDiv::GetSineCosineOfAngle(0, 0, 0, dp.x, dp.y);// Winkelaufteilung in 1°Schritten
         int nOldyDiv = m_Parameter.yDivision;
         if (nIndex != 361) nIndex = CEtsDiv::GetSineCosineOfAngle(361, ETSDIV_CALC_POLAR_VALUES, 0, dp.x, dp.y);
         if (nIndex == -1) return false;                       // Keine Winkelwerte : Fehler
         dummyline.SetLogPen(&lpsolid);                        // Darstellungsattribute
         dummytext.SetTextAlign(TA_TOP|TA_LEFT);

         div.f1 = 0;
         div.f2 = 360;
         fStart = 0.0;
         fStep  = 5.0;
         m_Parameter.xDivision = ETSDIV_LINEAR;                // lineare Transformation einstellen
         m_Parameter.yDivision = ETSDIV_LINEAR;
         div.stepwidth = m_wGridStep_x & GRID_LINE_MASK;       // Schrittweite einstellbar
         if (pDC->GetMapMode() == MM_HIMETRIC) div.stepwidth *= 10;
         div.division = CEtsDiv::Round((double)(div.stepwidth * 72) / ((double)m_View.Width() * M_PI));
         switch (div.division)                                 // Aufteilung in X°-Schritte
         {
            case 0:                   div.division =  1;       //  5°
            case 1: case 2: case 3: case 6: case 9:      break;//  5°, 10°, 15°, 30°, 45°
            case 4:                   div.division =  3; break;// 15°
            case 5: case 7:           div.division =  6; break;// 30°
            case 8: case 10: case 11:
            case 12:case 13: case 14: div.division =  9; break;// 45°
            default:                  div.division = 18; break;// 90°
         }
         fStep *= div.division;                                // Initialisierung der Schleifenvariablen
         ntx.dZero = div.dZero;
         for (ntx.fvalue=fStart; ntx.fvalue<div.f2; ntx.fvalue+=fStep)
         {
            nIndex = (int)ntx.fvalue;                          // Index des Winkels übergeben
            if (CEtsDiv::GetSineCosineOfAngle(nIndex, ETSDIV_POLAR_FROM_INDEX, 0, dp.x, dp.y) == -1) break;
            dp.x *= dMaxRadius;                                // Werte mit Radius multiplizieren
            dp.y *= dMaxRadius;
            TransformPoint(&dp, &lp);                          // Lineare transformation
            if (bDrawText && ((m_wGridStep_x & HIDE_GRID_TEXT)==0)) 
               bText = true;
            bSolid = true;
            switch (nIndex)                                    // bei diesen Winkeln korrigeren
            {                                                  // und Text zeichnen
               case  0: case 360: case 180: lp.x  = ptM.x; break;
               case 90: case 270:           lp.y  = ptM.y; break;
               default: bSolid = false;     bText = false; break;
            }
            if (bText)
            {
               TCHAR szText[32];
               switch (nIndex)                                 // Textausrichtung einstellen
               {
                  case   0: case  90:
                  case 360: dummytext.SetTextAlign(TA_RIGHT|TA_TOP   ); break;
                  case 180: dummytext.SetTextAlign(TA_RIGHT|TA_BOTTOM); break;
                  case 270: dummytext.SetTextAlign(TA_LEFT |TA_TOP   ); break;
               }
               wsprintf(szText, _T("%d°"), nIndex);
               m_Gridtext_x.InsertLabel(new CTextLabel(&lp, szText));
            }
            if (m_wGridStep_x & HIDE_GRID_LINE) continue;
            pll = new CLineLabel(&ptM);                        // Linie zeichnen
            if (!bSolid) pll->SetDDAPen(15, NULL, lpsolid.lopnColor);
            pll->SetdValue(ntx.fvalue);                        // Winkelwert eingeben
            pll->SetPoint(lp,1);
            m_Gridlines_x.InsertLabel(pll);
         }
         ncut = 0;
         noff = 0;
         m_Parameter.xDivision = ETSDIV_POLAR;                 // Polartransformation einstellen
         m_Parameter.yDivision = nOldyDiv;
      }
   }
   else if (m_Parameter.xDivision == ETSDIV_LINEAR)
   {
      if (CEtsDiv::CalculateLinDivision(&div, fStep, fStart))
      {
         ntx.dZero = div.dZero;
         for (ntx.fvalue=fStart; ntx.fvalue < m_Range.x2; ntx.fvalue+=fStep)
         {  
            dp.x = ntx.fvalue;
            TransformPoint(&dp, &lp);
            if (lp.x < ytextposleft) continue;
            lp.y = m_View.top - two.y - framewidth.y;
            Set_X_DivText(&ntx, &unitxrect, &lp, pDC, maxtextwidth);
            if ((lp.x > unitxrect.left) && (lp.x < unitxrect.right)) lp.y = unitxrect.bottom;
            if (m_wGridStep_x & HIDE_GRID_LINE) continue;
            pll = new CLineLabel(&lp);
            pll->SetdValue(ntx.fvalue);
            lp.y = m_View.bottom;
            pll->SetPoint(lp,1);
            m_Gridlines_x.InsertLabel(pll);
         }
         textrect = dummytext.GetRect(pDC);                    // In dummytext ist ein "M"
         ncut = CEtsDiv::Round(0.5+(double)(maxtextwidth + abs(textrect.Width()))/(double)div.width);
         if (ncut<=1) ncut = 0;                                // paßt eine Textlänge + "M" zwischen zwei Linien
         noff = 0;
      }
      else
      {
         REPORT("%s(%d) :Error %x:", __FILE__, __LINE__, GetLastError());
      }
   }
   else if (m_Parameter.xDivision == ETSDIV_LOGARITHMIC)
   {
      if ((m_Range.x2-m_Range.x1) > 0.8)
      {
         if (CEtsDiv::CalculateLogDivision(&div, fStep, fStart))
         {
            fEnd = pow((double)10, m_Range.x2);
            for (ntx.fvalue=fStart, i=0; ntx.fvalue < fEnd; ntx.fvalue+=fStep)
            {
               dp.x = ntx.fvalue;
               TransformPoint(&dp, &lp);
               ntest = CEtsDiv::TestLogValue(log10(ntx.fvalue));
               if ((ntest==1)&&(fStep < ntx.fvalue)) fStep *= 10.0;
               if (lp.x < ytextposleft) continue;
               lp.y = m_View.top - two.y - framewidth.y;
               if (ntest==1)                                         // 10'er Schritte immer
                  Set_X_DivText(&ntx, &unitxrect, &lp, pDC, maxtextwidth);
               if ((maxtextwidth<div.width)&&((ntest==2)||(ntest==5)))// 2'er und 5'er wenn der Platz reicht
                  Set_X_DivText(&ntx, &unitxrect, &lp, pDC, maxtextwidth);
               if ((maxtextwidth*7 < div.width)&&(!ntest))           // alle anderen wenn der Platz immer noch reicht
                  Set_X_DivText(&ntx, &unitxrect, &lp, pDC, maxtextwidth);
               if ((lp.x > unitxrect.left) && (lp.x < unitxrect.right)) lp.y = unitxrect.bottom;
               if (m_wGridStep_x & HIDE_GRID_LINE) continue;
               pll = new CLineLabel(&lp);
               pll->SetdValue(ntx.fvalue);
               lp.y = m_View.bottom;
               pll->SetPoint(lp,1);
               m_Gridlines_x.InsertLabel(pll);
            }
            ncut =0;
         }
      }
      else bFrequ = true;
   }
   if (bFrequ || m_Parameter.xDivision == ETSDIV_FREQUENCY)
   {
      if (CEtsDiv::CalculateFrqDivision(&div, fStep, fStart))
      {
         int nokt =-1, nterz=-1;
         for (fLogVal=fStart, i=0; fLogVal < m_Range.x2; fLogVal+=fStep)
         {
            ntx.fvalue =pow((double)10, fLogVal);
            dp.x = ntx.fvalue;
            TransformPoint(&dp, &lp);
            if (lp.x < ytextposleft) continue;
            lp.y = m_View.top - two.y - framewidth.y;
            ntest = CEtsDiv::TestFrqValue(fLogVal);
            Set_X_DivText(&ntx, &unitxrect, &lp, pDC, maxtextwidth);
            if ((lp.x > unitxrect.left) && (lp.x < unitxrect.right)) lp.y = unitxrect.bottom;
            if (m_wGridStep_x & HIDE_GRID_LINE) continue;
            pll = new CLineLabel(&lp);
            pll->SetdValue(ntx.fvalue);
            if      (ntest==3) {pll->SetLogPen(&lpdashdot); if (nterz==-1) nterz=i;}
            else if (ntest==2) {pll->SetLogPen(&lpdash);    if (nokt ==-1) nokt =i;}
            else               {pll->SetLogPen(&lpdot);}
            lp.y = m_View.bottom;
            pll->SetPoint(lp,1);
            m_Gridlines_x.InsertLabel(pll);
            i++;
         }   

         if      (nokt !=-1) noff = nokt;                         // Beschriftung orientiert sich stärker an Oktaven
         else if (nterz!=-1) noff = nterz;                        // als an Terzen
         else noff = 0;                                           // keine echten Terzen oder Oktaven : Offset egal

         textrect = dummytext.GetRect(pDC);                       // In dummytext ist ein "M"
         ncut = CEtsDiv::Round(0.5+(double)(maxtextwidth + textrect.Size().cx)/(double)div.width);
         if (ncut<=1) ncut = 0;                                   // paßt eine Textlänge + "M" zwischen zwei Linien
         else
         {
            if((div.division%3)==0) ncut = CEtsDiv::Teiler(ncut, 3);  // geeignete Teilung ermitteln
            else               ncut = CEtsDiv::Teiler(ncut, 2);
            noff%=ncut;
         }
      }
      else
      {
         REPORT("%s(%d) :Error %x:", __FILE__, __LINE__, GetLastError());
      }
   }

   ntext = m_Gridtext_x.GetCount();
   for (i=0; i<ntext; i++)
   {
      ptl = (CTextLabel*) m_Gridtext_x.GetLabel(i);
      if (ncut)
      {
         ntest  = i;
         ntest %= ncut;
         if (ntest!=noff) {lcut.InsertLabel(ptl); continue;}
      }
      textrect = ptl->GetRect(pDC);
      if (textrect.left < ytextposleft) {lcut.InsertLabel(ptl); continue;}
      nline = m_Gridlines_x.GetCount();
      for (j=0; j<nline; j++)
      {
         pll = (CLineLabel*) m_Gridlines_x.GetLabel(j);
         lp = pll->GetPoint(0);
         if ((textrect.left < lp.x ) && (lp.x < textrect.right))
         {
            lp.y = textrect.bottom;
            pll->SetPoint(lp, 0);
         }                  
      }
   }
   m_Gridtext_x.RemoveList(&lcut);
   lcut.DeleteAll();


   dummytext.SetLogFont(&oldGridLF);
   dummytext.SetLogPen(&oldGridTextLP);
   dummytext.SetTextColor(oldTextColor);
   dummytext.SetTextAlign(oldTextAlign);
   dummytext.SetBkColor(oldTextBkColor);
   dummytext.SetStates(oldTextState);
   dummytext.SetLogBrush(NULL);
   dummyline.SetLogPen(&oldGridLineLP);
   dummycircle.SetLogPen(&oldGridCircleLP);
   CLabel::SetSaveGlobal(false);

   m_bReferenceOn = bRefOn;
   return  true;
}

/******************************************************************************
* Name       : Set_Y_DivText                                                  *
* Definition : void Set_Y_DivText(ETSDIV_NUTOTXT *, CRect *, CPoint *, CDC *);*
* Zweck      : Erstellt die Y-Gittertexte.                                    *
* Aufruf     : Set_Y_DivText(pntx, pr, pp, pDC);                              *
* Parameter  :                                                                *
* pntx   (E) : Zeiger auf ETSDIV_NUTOTXT-Struktur           (ETSDIV_NUTOTXT*) *
* pr     (E) : reserviertes Rechteck für Einheitstext             (CRect*)    *
* pp     (E) : Textposition und p1 der Gitterlinie.               (CPoint*)   *
* pDC    (E) : Zeiger auf Gerätekontextklasse                     (CDC*)      *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CPlotLabel::Set_Y_DivText(ETSDIV_NUTOTXT *pntx, CRect *pr, CPoint *pp, CDC *pDC)
{
   if (m_wGridStep_y & HIDE_GRID_TEXT) return;
   TCHAR        pszDivText[MAXGRIDTEXTLEN];
   CTextLabel *ptl;
   CRect       textrect(0,0,0,0), testrect;

   if (CEtsDiv::NumberToText(pntx, pszDivText))
   {
      ptl = new CTextLabel(pp, pszDivText);
      textrect = ptl->GetRect(pDC);
      if (testrect.IntersectRect(textrect, *pr)) delete ptl;
      else                                       m_Gridtext_y.InsertLabel(ptl);
   }
}
/******************************************************************************
* Name       : Set_X_DivText                                                  *
* Definition : void Set_X_DivText(ETSDIV_NUTOTXT*,CRect*,CPoint*,CDC*,int&);  *
* Zweck      : Erstellt die X-Gittertexte.                                    *
* Aufruf     : Set_X_DivText(pntx, pr, pp, pDC, maxwidth);                    *
* Parameter  :                                                                *
* pntx   (E) : Zeiger auf ETSDIV_NUTOTXT-Struktur           (ETSDIV_NUTOTXT*) *
* pr     (E) : reserviertes Rechteck für Einheitstext             (CRect*)    *
* pp     (E) : Textposition und p1 der Gitterlinie.               (CPoint*)   *
* pDC    (E) : Zeiger auf Gerätekontextklasse                     (CDC*)      *
* maxwidth(EA):Textgröße: wird erhöht wenn der ermittelte Text    (int&)      *
*              länger ist.                                                    *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CPlotLabel::Set_X_DivText(ETSDIV_NUTOTXT *pntx, CRect *pr, CPoint *pp, CDC *pDC, int& maxwidth)
{
   if (m_wGridStep_x & HIDE_GRID_TEXT) return;
   TCHAR        pszDivText[MAXGRIDTEXTLEN];
   int         textwidth;
   CTextLabel *ptl;
   CRect       textrect, testrect;

   if (CEtsDiv::NumberToText(pntx, pszDivText))
   {
      ptl = new CTextLabel(pp, pszDivText);
      textrect  = ptl->GetRect(pDC);
      textwidth = textrect.Size().cx;
      if (maxwidth < textwidth) maxwidth = textwidth;
      if (testrect.IntersectRect(textrect, *pr)) delete ptl;
      else                                       m_Gridtext_x.InsertLabel(ptl);
   }
}

/******************************************************************************
* Name       : SetRangeInLogical                                              *
* Definition : bool SetRangeInLogical(CDC, CRect);                            *
* Zweck      : Setzen des Plotbereiches aus einem Rechteck in logischen       *
*              Koordinaten.                                                   *
* Aufruf     : SetRangeInLogical(pDC, zoomrect);                              *
* Parameter  :                                                                *
* pDC    (E) : Zeiger auf Gerätekontextklasse                     (CDC*)      *
* zoomrect(E): Rechteck des neuen Zoombereiches in logischen      (CRect)     *
*              Koordinaten.                                                   *
* Funktionswert : Bereich gesetzt (true, false)                   (bool)      *                                                      *
******************************************************************************/
bool CPlotLabel::SetRangeInLogical(CDC *pDC, CRect zoomrect)
{
   DPOINT dp1, dp2, dpCatched;
   CPoint p1 = zoomrect.TopLeft(),
          p2 = zoomrect.BottomRight();
   int    rvalue = 0;
   bool bRef = m_bReferenceOn;
   m_bReferenceOn = false;

   TransformPoint(&p1, &dp1);
   rvalue = CatchGridValue(pDC, &p1, &dpCatched);
   if (rvalue & 1) dp1.x = dpCatched.x;
   if (rvalue & 2) dp1.y = dpCatched.y;

   TransformPoint(&p2, &dp2);
   rvalue = CatchGridValue(pDC, &p2, &dpCatched);
   if (rvalue & 1) dp2.x = dpCatched.x;
   if (rvalue & 2) dp2.y = dpCatched.y;

   m_bReferenceOn = bRef;
   RANGE range = CTransformation::MakeRange(dp1, dp2);
   return SetRange(&range);
}
/******************************************************************************
* Name       : SetMarker                                                      *
* Definition : void SetMarker(int, double);                                   *
* Zweck      : Setzen einer Markerposition für die X-Achse                    *
* Aufruf     : SetMarker(iM, dPos);                                           *
* Parameter  :                                                                *
* iM    (E)  : Marker (0, 1) (links, rechts)                      (int)       *
* dPos  (E)  : Position des Markers in Weltkoordinaten            (double)    *
******************************************************************************/
void CPlotLabel::SetMarker(int iM, double dPos)
{
   if (iM == 0 || iM == 1)
   {
      if (m_pdMarker == NULL)
      {
         m_pdMarker = new double[2];
         RANGE r = GetRange();
         m_pdMarker[0] = r.x1;
         m_pdMarker[1] = r.x2;
      }
      m_pdMarker[iM] = dPos;
   }
   else
   {
      SAFE_DELETE(m_pdMarker);
   }
}
BOOL CPlotLabel::GetMarker(int iM, double &dMr)
{
   if (m_pdMarker)
   {
      dMr = m_pdMarker[iM];
      return TRUE;
   }
   return FALSE;
}
/******************************************************************************
* Name       : CatchGridValue                                                 *
* Definition : int CatchGridValue(CDC, CPoint, LPDPOINT);                     *
* Zweck      : Fangen des Mauscursors an den Gitterlinien.                    *
* Aufruf     : CatchGridValue(pDC, p, pdp);                                   *
* Parameter  :                                                                *
* pDC    (E) : Zeiger auf Gerätekontextklasse                     (CDC*)      *
* p      (E) : Mauscursorkoordinaten in logischen Koordianten     (CPoint*)   *
* pdp    (E) : Ermittelter Wert zur Gitterlinie in Weltkoordinaten(LPDPOINT)  *
* Funktionswert : Gitterlinie gefangen                            (int)       *
* 0 keine Gitterlinie gefangen                                                *
* Bit 1: X-Gitterlinie gefangen                                               *
* Bit 2: Y-Gitterlinie gefangen                                               *
******************************************************************************/
int CPlotLabel::CatchGridValue(CDC *pDC, CPoint *p, LPDPOINT pdp)
{
   CLabel     *pl;
   int         rvalue=0;
   if (pDC && p && pdp)
   {
      // Fangen an den X-Gitterlinien
      pl = m_Gridlines_x.HitLabel(p, pDC);
      if (pl && pl->IsKindOf(RUNTIME_CLASS(CLineLabel)))
      {
         pdp->x = ((CLineLabel*)pl)->GetdValue();
         rvalue = 1;
      }
      // Fangen an den Y-Gitterlinien
      int nDist = 0;
      pl = m_Gridlines_y.HitLabel(p, pDC, &nDist);
      if (pl && (nDist < CLabel::MaxPickDistance()))
      {
         if (pl->IsKindOf(RUNTIME_CLASS(CLineLabel)))
            pdp->y = ((CLineLabel*)pl)->GetdValue();
         else if (pl->IsKindOf(RUNTIME_CLASS(CCircleLabel)))
            pdp->y = ((CCircleLabel*)pl)->GetdValue();
         rvalue |= 2;
      }
      return rvalue;
   }
   return 0;
}
/******************************************************************************
* Name       : SetRange                                                       *
* Definition : virtual bool SetRange(LPRANGE);                                *
* Zweck      : Setzt den Weltkoordinatenbereich des Plots unter Berücksichtig-*
*              ung der eingestellten Achsenteilung (linear, logarithmisch).   *
* Aufruf     : SetRange(prange);                                              *
* Parameter  :                                                                *
* prange (E) : Neuer Weltkoordinatenbereich                          (LPRANGE)*
* Funktionswert : Neuer Bereich gesetzt (true, false)                  (bool) *
******************************************************************************/
bool CPlotLabel::SetRange(LPRANGE prange)
{
   RANGE range = *prange;

   if (CTransformation::SetClippRange(&range))                 // die Klipping-Koordinaten sind immer echte
   {                                                           // Weltkoordinaten
      switch (m_Parameter.xDivision)
      {
         case ETSDIV_FREQUENCY:                                // bei logarithmischer Teilung sind auch
         case ETSDIV_LOGARITHMIC:                              // die zur Transformation benötigten
            if (range.x1 <= 0)                 return false;   // Weltkoordinaten logarithmisch
            if (range.x2 <= 0)                 return false;
            range.x1 = log10(range.x1);
            range.x2 = log10(range.x2);
            break;
         case ETSDIV_POLAR:                                    // Polarkoordinatentransformation
         {
            double dRangeY;                                    // der gesamte Y-Bereich wird auf den Radius abgebildet
            if (m_Parameter.yDivision == ETSDIV_LINEAR)
               dRangeY = prange->y2 - prange->y1;
            else
               dRangeY = log10(prange->y2) - log10(prange->y1);
            if (dRangeY < 0) dRangeY = -dRangeY;               // negativen Radius gibt es nicht
            m_ClippRange.x1 = m_ClippRange.y1 = -dRangeY;      // Ausdehnung um den (0,0)-Punkt des
            m_ClippRange.x2 = m_ClippRange.y2 =  dRangeY;      // Weltkoordinatensystems
         } break;
      }

      switch (m_Parameter.yDivision)
      {
         case ETSDIV_FREQUENCY:                                // dito: siehe X-Achse
         case ETSDIV_LOGARITHMIC:
            if (range.y1 <= 0)                 return false;
            if (range.y2 <= 0)                 return false;
            range.y1 = log10(range.y1);
            range.y2 = log10(range.y2);
            break;
      }

      if (CTransformation::SetRange(&range))                   // Bereich prüfen und setzen
      {
         AddRange(prange);                                     // Zoombereich in Zoomliste einfügen
         return true;
      }
   }
   return false;
}
void CPlotLabel::SetDefaultRange(RANGE *pRange)
{
   if (pRange)                                                 // neuer Defaultbereich
   {
	  if (SetRange(pRange))                                     // prüfen
	  {
         m_DefaultRange = *pRange;                             // stzen
         POSITION pos = m_RangeList.GetHeadPosition();
         if ((pos!=NULL) && (m_RangePos!=NULL) && (pos!=m_RangePos))
         {
            RANGE *pR1 = (RANGE*)m_RangeList.GetAt(pos);       // Positionen in der Liste suchen
            RANGE *pR2 = (RANGE*)m_RangeList.GetAt(m_RangePos);
            m_RangeList.SetAt(pos, pR2);                       // und vertauschen
            m_RangeList.SetAt(m_RangePos, pR1);
         }
     }
   }
   else SetRange(&m_DefaultRange);
}
RANGE CPlotLabel::GetRange()
{
   RANGE range = CTransformation::GetRange();
   switch (m_Parameter.xDivision)
   {
      case ETSDIV_FREQUENCY:                                // bei logarithmischer Teilung sind auch
      case ETSDIV_LOGARITHMIC:                              // die zur Transformation benötigten
         range.x1 = pow((double)10, range.x1);
         range.x2 = pow((double)10, range.x2);
         break;
   }
   switch (m_Parameter.yDivision)                           // das gilt auch für die y-Achse
   {
      case ETSDIV_FREQUENCY:
      case ETSDIV_LOGARITHMIC:
         range.y1 = pow((double)10, range.y1);
         range.y2 = pow((double)10, range.y2);
         break;
   }
   return range;
}
/******************************************************************************
* Name       : SetDefaults                                                    *
* Zweck      : Setzt die default Parameter, die bei Einfügen der ersten Kurve *
*              gesetzt wurden. Löscht die Zoomliste bis auf das erste Element *
*              in welchem der default Zoom-Bereich steht. Setzt die Refenrenz-*
*              kurve auf NULL und den Referenzschalter auf false.             *
* Aufruf     : SetDefaults();                                                 *
* Parameter  :                                                                *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CPlotLabel::SetDefaults()
{
   DestroyRangeList();
   m_Parameter.xDivision = m_Defaults.xDivision;
   m_Parameter.yDivision = m_Defaults.yDivision;
   m_Parameter.xNumMode  = m_Defaults.xNumMode;
   m_Parameter.yNumMode  = m_Defaults.yNumMode;
   m_bReferenceOn        = false;
   m_pRefCurve           = NULL;
/*
   if (m_Parameter.xDivision != ETSDIV_POLAR)
   {
      int i, count = m_Curves.GetCount();
      for (i=0; i< count; i++)
      {
         ((CCurveLabel*)m_Curves.GetLabel(i))->SetXDivision(m_Defaults.xDivision);
         ((CCurveLabel*)m_Curves.GetLabel(i))->SetYDivision(m_Defaults.yDivision);
      }
   }
*/
   SetRange(&m_DefaultRange);
}

void CPlotLabel::SetNewDefaults()
{
   m_Defaults.xDivision = m_Parameter.xDivision;
   m_Defaults.yDivision = m_Parameter.yDivision;
   m_Defaults.xNumMode  = m_Parameter.xNumMode;
   m_Defaults.yNumMode  = m_Parameter.yNumMode;
}

/******************************************************************************
* Name       : AddRange                                                       *
* Definition : void AddRange(RANGE *);                                        *
* Zweck      : Fügt einen neuen Zoom-Bereich in die Zoomliste ein. Ist der    *
*              Zeiger auf die Variable pRange schon in der Liste, wird kein   *
*              neues Element eingefügt. Ist der Bereich schon einmal          *
*              vorhanden, ebenso nicht.                                       *
* Aufruf     : AddRange(pRange);                                              *
* Parameter  :                                                                *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CPlotLabel::AddRange(RANGE *pRange)
{
   RANGE *pRtest;
   if (!m_RangeList.IsEmpty())                                 // Liste durchsuchen
   {
      POSITION oldpos, pos = m_RangeList.GetTailPosition();
      while (pos)  
      {
         oldpos = pos;
         pRtest =(RANGE*) m_RangeList.GetPrev(pos);
         if (pRtest == pRange)
         {
            m_RangePos = oldpos;
            return;// gleiche Pointer: Element schon vorhanden, Ende
         }
         if (memcmp(pRtest, pRange, sizeof(RANGE)) == 0)
         {
            m_RangePos = oldpos;
            return;// gleiche Bereiche: Platzverschwendung, Ende
         }
      }
   }
   else m_DefaultRange = *pRange;

   pRtest = new RANGE;
   *pRtest = *pRange;
   m_RangePos = m_RangeList.AddTail((void*)pRtest);
   m_RangePos = m_RangeList.GetTailPosition();
}
// Löscht die gesamte Zoomliste
void CPlotLabel::DestroyRangeList()
{
   if (m_RangeList.IsEmpty()) return;
   while (m_RangeList.GetHeadPosition()) delete m_RangeList.RemoveHead();
   m_RangePos = NULL;
}
// Setzt den nächsten Bereich, der in der Zoomliste steht
bool CPlotLabel::Zoom()
{
   if (CheckZoom() && m_RangePos)
   {
      m_RangeList.GetNext(m_RangePos);
      if (m_RangePos) return SetRange((RANGE*)m_RangeList.GetAt(m_RangePos));
   }
   return false;
}
// Setzt den vorhergehenden Bereich, der in der Zoomliste steht
bool CPlotLabel::ReZoom()
{
   if (CheckReZoom() && m_RangePos)
   {
      m_RangeList.GetPrev(m_RangePos);
      if (m_RangePos) return SetRange((RANGE*)m_RangeList.GetAt(m_RangePos));
   }
   return false;
}

bool CPlotLabel::SelectZoom()
{
   CListDlg LDlg;
   LDlg.m_strSelected.LoadString(IDS_SELECT_ZOOMRANGE);
   LDlg.m_pCallBack   = SelectZoom;
   LDlg.m_pParam      = this;
   LDlg.m_szSize.cx   = 300;
   LDlg.m_szSize.cy   = 150;
   LDlg.m_bOkButton   = true;
   LDlg.m_bResize     = true;
   if (LDlg.DoModal() == IDOK) return true;
   return !LDlg.m_bResize;
}

int CPlotLabel::SelectZoom(CListDlg *pDlg, int nReason)
{
   CPlotLabel *pThis = (CPlotLabel*) pDlg->m_pParam;
   POSITION    pos;
   if (nReason == ONINITDIALOG)
   {
      int i = 0, nSel = 0, nCount = pThis->m_RangeList.GetCount();
      if (nCount < 2) return 0;
      RANGE    *pR;
      CRect     rc;
      TCHAR      szDivText[MAXGRIDTEXTLEN];
      ETSDIV_NUTOTXT  ntx, nty;
      CString   str;
      ntx.nmaxl  = nty.nmaxl = MAXGRIDTEXTLEN;
      ntx.dZero  = nty.dZero = 0;
      ntx.nround = pThis->m_cRound_x;
      ntx.nmode  = pThis->m_Parameter.xNumMode;
      if (pThis->m_Parameter.xNoEndNulls) ntx.nmode |= ETSDIV_NM_NO_END_NULLS;
      nty.nround = pThis->m_cRound_y;
      nty.nmode  = pThis->m_Parameter.yNumMode;
      if (pThis->m_Parameter.yNoEndNulls) nty.nmode |= ETSDIV_NM_NO_END_NULLS;

      pDlg->m_List.ModifyStyle(0, LVS_EDITLABELS, 0);
      pDlg->m_List.GetClientRect(&rc);
      rc.right -= 15;
      rc.right /= 4;
      pDlg->m_List.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 
         LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT,
         LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

      pDlg->m_List.SetWindowContextHelpId(IDC_SELECT_ZOOM_RANGE|HID_BASE_DISPATCH);

      pDlg->m_List.InsertColumn(0, "", LVCFMT_LEFT, 15, 0);
      str.Format(IDS_PLOT_FROM, pThis->m_Unit_x.GetText());
      pDlg->m_List.InsertColumn(1, str, LVCFMT_LEFT, rc.right, 0);
      str.Format(IDS_PLOT_TO, pThis->m_Unit_x.GetText());
      pDlg->m_List.InsertColumn(2, str, LVCFMT_LEFT, rc.right, 0);
      str.Format(IDS_PLOT_FROM, pThis->m_Unit_y.GetText());
      pDlg->m_List.InsertColumn(3, str, LVCFMT_LEFT, rc.right, 0);
      str.Format(IDS_PLOT_TO, pThis->m_Unit_y.GetText());
      pDlg->m_List.InsertColumn(4, str, LVCFMT_LEFT, rc.right, 0);

      pos = pThis->m_RangeList.GetHeadPosition();
      while (pos)
      {
         pR = (RANGE*)pThis->m_RangeList.GetNext(pos);
         ntx.fvalue = pR->x1;
         wsprintf(szDivText, _T("%d"), i+1);
         pDlg->m_List.InsertItem(i, szDivText);
         CEtsDiv::NumberToText(&ntx, szDivText);
         pDlg->m_List.SetItemText(i, 1, szDivText);
         ntx.fvalue = pR->x2;
         CEtsDiv::NumberToText(&ntx, szDivText);
         pDlg->m_List.SetItemText(i, 2, szDivText);
         nty.fvalue = pR->y1;
         CEtsDiv::NumberToText(&nty, szDivText);
         pDlg->m_List.SetItemText(i, 3, szDivText);
         nty.fvalue = pR->y2;
         CEtsDiv::NumberToText(&nty, szDivText);
         pDlg->m_List.SetItemText(i, 4, szDivText);
         pDlg->m_List.SetItemData(i, (DWORD)pR);
         i++;
         if (pos == pThis->m_RangePos) nSel = i;
      }
      pDlg->m_List.SetItemState(nSel, LVIS_SELECTED, LVIS_SELECTED);
      pDlg->m_List.EnsureVisible(nSel, false);
      return 1;
   }
   else if (nReason == ONOK)
   {
      int nReturn = 0;
      int i, nCount = pDlg->m_List.GetItemCount();
      for (i=0; i<nCount; i++)
      {
         RANGE *pR  = (RANGE*) pDlg->m_List.GetItemData(pDlg->m_nCursel);
         if (pR!=NULL)
         {
            if (pDlg->m_List.GetCheck(pDlg->m_nCursel)!=-1)
            {
               TCHAR text[256];
               pDlg->m_List.GetItemText(pDlg->m_nCursel, 1, text, 256);
               pR->x1 = _ttof(text);
               pDlg->m_List.GetItemText(pDlg->m_nCursel, 2, text, 256);
               pR->x2 = _ttof(text);
               pDlg->m_List.GetItemText(pDlg->m_nCursel, 3, text, 256);
               pR->y1 = _ttof(text);
               pDlg->m_List.GetItemText(pDlg->m_nCursel, 4, text, 256);
               pR->y2 = _ttof(text);
            }
            if (i == pDlg->m_nCursel)
            {
               pThis->SetRange(pR);
               pDlg->m_bResize = false;
               nReturn = 1;
            }
         }
      }
      return nReturn;
   }
   else if (nReason == ONBEGINLABELEDITLSTLIST)
   {
      //LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pDlg->m_pNMHdr;
      return 0;
   }
   else if (nReason == ONENDLABELEDITLSTLIST)
   {
      LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pDlg->m_pNMHdr;
      bool   bOk = false;
      RANGE *pR  = (RANGE*) pDlg->m_List.GetItemData(pDlg->m_nCursel);
      if ((pR != NULL) && (pDispInfo->item.mask & LVIF_TEXT))
      {
         CEdit* pEdit = pDlg->m_List.GetEditControl();
         if (pEdit)
         {
            double dValue;
            CDataExchange dx(&pDlg->m_List, true);
	         TRY
	         {
         	   DDX_Text(&dx, pEdit->GetDlgCtrlID(), dValue);
               bOk = true;
               switch (pDlg->m_nSubItem)
               {
                  case 1: if (dValue > pR->x2) bOk = false;
                     break;
                  case 2: if (dValue < pR->x1) bOk = false;
                     break;
                  case 3: if (dValue > pR->y2) bOk = false;
                     break;
                  case 4: if (dValue < pR->y1) bOk = false;
                     break;
               }
	         }
	         CATCH_ALL(e)
	         {
               if (!e->m_bAutoDelete) e->Delete();
            }
      	   END_CATCH_ALL
         }

         if (bOk)
         {  
            if (pDlg->m_nSubItem > 0)
            {
               pDlg->m_List.SetItemText(pDlg->m_nCursel, pDlg->m_nSubItem, pDispInfo->item.pszText);
               bOk = false;
            }
//            TCHAR      szDivText[MAXGRIDTEXTLEN];
//            wsprintf(szDivText, _T("%d*"), pDlg->m_nCursel+1);
//            pDlg->m_List.SetItemText(pDlg->m_nCursel, 0, szDivText);
            pDlg->m_List.SetCheck(pDlg->m_nCursel, true);
         }
      }
      return bOk;
   }
   else if (nReason == ONRCLICKLSTLIST)
   {
      int nCursel = pDlg->m_List.GetSelectionMark();
      if (nCursel > 0)
      {
         pos = pThis->m_RangeList.FindIndex(nCursel);
         if (pos!=NULL)
         {
            if (AfxMessageBox(IDS_REQUEST_DELETE_RANGE, MB_YESNO|MB_ICONQUESTION) == IDYES)
            {
               if (pos == pThis->m_RangePos)
               {
                  int i = 0, nSel = 0;
                  pThis->DeleteCurrentRange();
                  pos = pThis->m_RangeList.GetHeadPosition();
                  while (pos)
                  {
                     pThis->m_RangeList.GetNext(pos);
                     i++;
                     if (pos == pThis->m_RangePos) nSel = i;
                  }
                  pDlg->m_List.SetItemState(nSel, LVIS_SELECTED, LVIS_SELECTED);
                  pDlg->m_List.EnsureVisible(nSel, false);
                  pDlg->m_bResize = false;
               }
               else
               {
                  RANGE*pR = (RANGE*)pThis->m_RangeList.GetAt(pos);
                  pThis->m_RangeList.RemoveAt(pos);
                  delete pR;
               }
               pDlg->m_List.DeleteItem(nCursel);
            }
         }
      }
   }
   return 0;
}

int CPlotLabel::GetNoOfRanges()
{
   return m_RangeList.GetCount();
}
// Setzt den Bereich, der in der Zoomliste steht erneut
bool CPlotLabel::UpdateZoom()
{
   if (m_RangePos)
   {
      if (m_RangePos)
      {
         RANGE range;
         if (m_bReferenceOn && m_pRefCurve)
         {
            range = *(RANGE*)m_RangeList.GetAt(m_RangePos);
            double dy_halbe = (range.y2 - range.y1) * 0.5;           // Bereich auf +/- dy/2 festlegen
            range.y1 = -dy_halbe;
            range.y2 =  dy_halbe;
         }
         else
         {
            range = m_DefaultRange;
         }
         return SetRange(&range);
      }
   }
   return false;
}

/******************************************************************************
* Name       : Serialize                                                      *
* Definition : virtual void Serialize(CArchive&);                             *
* Zweck      : Serialisizerung des Plotobjektes zum Abspeichern oder Laden    *
*              (in) eine(r) Datei.                                            *
* Aufruf     : Serialize(ar);                                                 *
* Parameter  :                                                                *
* ar     (E) : CArchive-Objekt                                    (CArchive&) *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CPlotLabel::Serialize(CArchive& ar)
{
   #ifdef CHECK_LABEL_SERIALIZATION
   REPORT("CPlotLabel::Serialize()");
   #endif
   CRectLabel::Serialize(ar);
   int      i, count;
   RANGE   *pRtemp = NULL;
   POSITION pos;

   if (ar.IsStoring())
   {
      m_PlotHeading.Serialize(ar);
      count = m_RangeList.GetCount();
      i = 0;
      if (count)
      {
         ArchiveWrite(ar, &count, sizeof(int));
         pos = m_RangeList.GetHeadPosition();
         while (pos)
         {
            pRtemp = (RANGE*) m_RangeList.GetNext(pos);
            ArchiveWrite(ar, pRtemp, sizeof(RANGE));
            if (memcmp(pRtemp, &m_ClippRange, sizeof(RANGE)) == 0) count = i;
            i++;
         }
      }
      else
      {
         count = 1;
         ArchiveWrite(ar, &count,   sizeof(int));
         ArchiveWrite(ar, &m_DefaultRange, sizeof(RANGE));
      }
      ArchiveWrite(ar, &count, sizeof(int));
      ArchiveWrite(ar, &m_Parameter, sizeof(BSParameter));
      ArchiveWrite(ar, &m_Defaults, sizeof(BSPlotDefaults));
      ArchiveWrite(ar, &m_cRound_x, sizeof(char));
      ArchiveWrite(ar, &m_cRound_y, sizeof(char));
      ArchiveWrite(ar, &m_wGridStep_x, sizeof(WORD));
      ArchiveWrite(ar, &m_wGridStep_y, sizeof(WORD));
      ArchiveWrite(ar, &m_GridLineColor, sizeof(COLORREF));
      m_Unit_x.Serialize(ar);
      m_Unit_y.Serialize(ar);
      ArchiveWrite(ar, &m_CurveLogFont, sizeof(LOGFONT));
      m_Curves.Serialize(ar);
   }
   else
   {
      m_PlotHeading.Serialize(ar);
      ArchiveRead(ar, &count, sizeof(int));
      for (i=0; i<count; i++)
      {
         ArchiveRead(ar, &m_Range, sizeof(RANGE));
         AddRange(&m_Range);
         if (i==0) m_DefaultRange = m_Range;
      }
      ArchiveRead(ar, &i, sizeof(int));
      ArchiveRead(ar, &m_Parameter, sizeof(BSParameter));
      pos = m_RangeList.FindIndex(i);
      if (pos) pRtemp = (RANGE*) m_RangeList.GetAt(pos);
      if (pRtemp) SetRange(pRtemp);
      else        SetRange(&m_DefaultRange);
      ArchiveRead(ar, &m_Defaults, sizeof(BSPlotDefaults));
      ArchiveRead(ar, &m_cRound_x, sizeof(char));
      ArchiveRead(ar, &m_cRound_y, sizeof(char));
      ArchiveRead(ar, &m_wGridStep_x, sizeof(WORD));
      ArchiveRead(ar, &m_wGridStep_y, sizeof(WORD));
      ArchiveRead(ar, &m_GridLineColor, sizeof(COLORREF));
      m_Unit_x.Serialize(ar);
      m_Unit_y.Serialize(ar);
      ArchiveRead(ar, &m_CurveLogFont, sizeof(LOGFONT));
      m_Curves.Serialize(ar);
      m_Curves.SetDeleteContent(true);
      count = m_Curves.GetCount();
      for (i=0; i<count; i++)
      {
         ((CCurveLabel*) m_Curves.GetLabel(i))->SetTransformation(this);
      }
   }
}
/******************************************************************************
* Name       : Read                                                           *
* Definition : void Read(HANDLE);                                             *
* Zweck      : Lesen der Plotdaten aus einer Datei und zugleich Schnittstelle *
*              zu nicht MFC-Programmen.                                       *
* Aufruf     : Read(hfile);                                                   *
* Parameter  :                                                                *
* hfile  (E) : File-Handle auf die zu lesende Datei                  (HANDLE) *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CPlotLabel::Read(HANDLE hfile)
{
   if (hfile==INVALID_HANDLE_VALUE) return;
   DWORD       dwactbytes = 0;
   int         i, count, size;
   CCurveLabel *pcl;
   DestroyRangeList();

   if (!ReadFile(hfile, &size, sizeof(int), &dwactbytes, NULL)) return;
   if (size)
   {
      
      TCHAR *pszDummy;
      m_PlotHeading.SetStringLength(size);
      pszDummy = (TCHAR*)m_PlotHeading.GetText();
      if (!pszDummy) return;
      if (!ReadFile(hfile, pszDummy, size, &dwactbytes, NULL)) return;
      pszDummy[size] = 0;
   }
   // Zoombereiche
   if (!ReadFile(hfile, &count,   sizeof(int),   &dwactbytes, NULL))    return;
   RANGE rtemp;
   for (i=0; i<count; i++)
   {
      if (!ReadFile(hfile, &rtemp, sizeof(RANGE), &dwactbytes, NULL)) return;
      if (i==0) m_DefaultRange = rtemp;
      AddRange(&rtemp);
   }
   // Kurven
   if (!ReadFile(hfile, &count,   sizeof(int),   &dwactbytes, NULL))    return;
   for (i=0; i<count; i++)
   {
      pcl = new CCurveLabel;
      pcl->Read(hfile);
      InsertCurve(pcl, false, MAKELONG(-1, FIT_INSERTED_CURVE_ID));
   }
   SetRange(&m_DefaultRange);
}
/******************************************************************************
* Name       : Write                                                          *
* Definition : void Write(HANDLE);                                            *
* Zweck      : Schreiben der Plotdaten in eine Datei und zugleich Schnitt-    *
*              stelle zu nicht MFC-Programmen.                                *
* Aufruf     : Write(hfile);                                                  *
* Parameter  :                                                                *
* hfile  (E) : File-Handle auf die zu schreibende Datei              (HANDLE) *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CPlotLabel::Write(HANDLE hfile)
{
   if (hfile==INVALID_HANDLE_VALUE) return;
   DWORD dwactbytes = 0;
   const TCHAR *pszDummy;
   pszDummy = m_PlotHeading.GetText();
   int   i, count, size;
   CCurveLabel *pcl;

   if (pszDummy) size = _tcsclen(pszDummy);
   else          size = 0;
   WriteFile(hfile, &size, sizeof(int), &dwactbytes, NULL);
   if (size) WriteFile(hfile, pszDummy, size, &dwactbytes, NULL);
   // Zoombereiche   
   count = m_RangeList.GetCount();
   if (count)
   {
      WriteFile(hfile, &count, sizeof(int), &dwactbytes, NULL);
      POSITION pos = m_RangeList.GetHeadPosition();
      RANGE *rtemp;
      while (pos)
      {
         rtemp = (RANGE*) m_RangeList.GetNext(pos);
         WriteFile(hfile, rtemp, sizeof(RANGE), &dwactbytes, NULL);
      }
   }
   else
   {
      count = 1;
      WriteFile(hfile, &count,   sizeof(int),   &dwactbytes, NULL);
      WriteFile(hfile, &m_DefaultRange, sizeof(RANGE), &dwactbytes, NULL);
   }
   // Kurven
   count = m_Curves.GetCount();
   WriteFile(hfile, &count,   sizeof(int),   &dwactbytes, NULL);
   for (i=0; i<count; i++)
   {
      pcl = (CCurveLabel*) m_Curves.GetLabel(i);
      pcl->Write(hfile);
   }
}
/******************************************************************************
* Name       : GetChecksum                                                    *
* Definition : virtual void GetChecksum(CFileHeader *);                       *
* Zweck      : Ermitteln der Checksumme der Objektdaten.                      *
* Aufruf     : GetChecksum(pFH);                                              *
* Parameter  :                                                                *
* pFH    (E) : Zeiger auf ein CFileHeader-Objekt               (CFileHeader*) *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CPlotLabel::GetChecksum(CFileHeader *pFH)
{
   if (!pFH) return;
   const TCHAR * heading = m_PlotHeading.GetText();
   int  i, size = 0, count = m_Curves.GetCount();

   if (heading)
   {
      size = _tcsclen(heading);
      pFH->CalcChecksum(&size,   sizeof(int));
      pFH->CalcChecksum(heading, size);
   }
   else  pFH->CalcChecksum(&size, sizeof(int));

   pFH->CalcChecksum(&m_DefaultRange, sizeof(RANGE));
   pFH->CalcChecksum(&count,   sizeof(int));

   for (i=0; i<count; i++)
      ((CCurveLabel*)m_Curves.GetLabel(i))->GetChecksum(pFH);
}

/******************************************************************************
* Name       : InsertCurve                                                    *
* Definition : bool InsertCurve(CCurveLabel *);                               *
* Zweck      : Einfügen einer Kurve in den Plot und Überprüfung der Gültigkeit*
*              der Kurvenparameter.                                           *
* Aufruf     : InsertCurve(pcl);                                              *
* Parameter  :                                                                *
* pcl    (E) : Zeiger auf einzufügende Kurve                   (CCurveLabel*) *
* [bTest](E) : Nur testen ob die Kurve eingefügt werden kann   (bool)         *
*              (true, false)                                                  *
* [nPos] (E) : Position, an der die Kurve eingefügt werden soll(int)          *
*    LOWORD(nPos)  = -1: Kurve anhängen                                       *
*    LOWORD(nPos) >=  0: Kurve an dieser Position einfügen                    *
*    HIWORD(nPos)  = DONT_DELETE_INSERTED_CURVE                               *
*       eingefügte Kurve mit ID darf nicht gelöscht werden                    *
*    HIWORD(nPos)  = FIT_INSERTED_CURVE_ID                                    *
*       Hat die Kurve eine ID so ersetzt sie ausschließlich die Kurve mit der *
*       gleichen ID.                                                          *
*    HIWORD(nPos)  = REPLACE_CURVE_TXT                                        *
*       Hat die Kurve eine ID so ersetzt sie auch den Kurventext der Kurve    *
*       mit der gleichen ID.                                                  *
*    Wenn Kurven im Plot ID-Nummern haben, so wird eine Kurve mit einer       *
*    entsprechenden ID nicht eingefügt, sondern sie ersetzt die vorhandenen   *
*    Kurvenwerte.                                                             *
*    Wird keine Kurve mit derselben ID gefunden, so wird die Kurve hinten     *
*    angehängt.                                                               *
*    Der Bereich der ID-Nummern ist (1,..,15)                                 *
* Funktionswert : Kurve eingefügt  (true, false)               (bool)         *
******************************************************************************/
bool  CPlotLabel::InsertCurve(CCurveLabel *pcl, bool bTest, long nPos)
{
   if (pcl)
   {
      ASSERT_VALID(pcl);
      TCHAR *textold, *textnewx, *textnewy, text[100];
      textnewx = pcl->GetXUnit();
      textnewy = pcl->GetYUnit();
      int count = m_Curves.GetCount();                         // Anzahl der vorhandenen Kurven ermitteln
      if (count > 64) return false; 
      if (count==0)                                            // die erste Kurve setzt die Anfangs und Defaultparameter
      {
         if (bTest) return true;                               // bei Test true zurückgeben, da sie eingefügt werden kann.
         m_Defaults.xDivision    = m_Parameter.xDivision = pcl->GetXDivision();
         m_Defaults.yDivision    = m_Parameter.yDivision = pcl->GetYDivision();
         m_Defaults.xNumMode     = m_Parameter.xNumMode  = pcl->GetXNumMode();
         m_Defaults.yNumMode     = m_Parameter.yNumMode  = pcl->GetYNumMode();
         m_Parameter.angleformat = pcl->GetAngleFormat();
         m_Parameter.editable    = !pcl->IsWriteProtect();
         if (textnewx)                                         // hat die neue Kurve eine x-Einheit ?
         {
            sprintf(text, _T("[%s]"), textnewx);
            m_Unit_x.SetText(text);
         }
         else m_Unit_x.SetText(_T("[]"));
         if (textnewy)                                         // hat die neue Kurve eine x-Einheit ?
         {
            sprintf(text, _T("[%s]"), textnewy);
            m_Unit_y.SetText(text);
         }
         else m_Unit_y.SetText(_T("[]"));
      }
      else                                                     // weitere Kurven müssen bestimmte Kriterien erfüllen
      {
         bool bRValue = true;
         int nInsertCurves = 0;
         m_Curves.ProcessWithLabels(RUNTIME_CLASS(CCurveLabel), &nInsertCurves, CPlotLabel::CheckInsertCurves);
         if ((nInsertCurves&NO_LOGARITHMIC_CURVES) && (pcl->GetXDivision() > CCURVE_LINEAR))
         {
            return false;                                      // Polardiagramkurven nicht mit logarithmischen Kurven mischen
         }
         if ((nInsertCurves&NO_POLAR_CURVES) && (pcl->GetXDivision() == CCURVE_POLAR))
         {
            return false;                                      // logarithmische Kurven nicht mit Polardiagramkurven mischen
         }
         if (m_Curves.GetIndex(pcl) != -1) return false;       // Zeiger auf Kurve schon vorhanden
         if (pcl->IsWriteProtect()) m_Parameter.editable = false;// ist eine Kurve schreibgeschützt, so darf auch der Plot nicht mehr editiert werden.
                                                               // logaritmische Achsenteilung nicht mit linearer Achsenteilung mischen

         textold = (TCHAR*)m_Unit_x.GetText();                  // die Einheit der x-Achse einfügen
         if (textnewx)                                         // hat die neue Kurve eine x-Einheit ?
         {
            sprintf(text, _T("[%s]"), textnewx);
            if (textold)                                       // hat der Plot schon eine x-Einheit ?
            {
               if (strcmp(textold, text)) return false;        // wenn die X-Einheiten unterschiedlich sind,
            }                                                  // wird die Kurve nicht eingefügt
            else if (!bTest)                                   // hat der Plot noch keine
            {
               SetXUnit(text);                                 // so bekommt er jetzt eine
            }
         }

         textold = (TCHAR*)m_Unit_y.GetText();                  // die Einheit der y-Achse einfügen
         if (textnewy)                                         // hat die neue Kurve eine y-Einheit ?
         {
            sprintf(text, _T("[%s]"), textnewy);
            if (textold)                                       // hat der Plot schon eine y-Einheit ?
            {
               if (strcmp(textold, text)) return false;        // wenn die Y-Einheiten unterschiedlich sind
            }                                                  // wird die Kurve nicht eingefügt
            else if (!bTest)                                   // hat der Plot noch keine
            {
               SetYUnit(text);                                 // so bekommt er jetzt eine
            }
         }      
         if (bTest) return bRValue;
      }

      int nNewTargetFnNo = pcl->GetTargetFuncNo();
      if (nNewTargetFnNo!=0)                                   // evtl. Zielfunktion ersetzen
      {
         int nTargetFnNo, i;
         bool bTakeAny = false;
         for (i=0; i<count; i++)
         {
            CCurveLabel *pold = (CCurveLabel *) m_Curves.GetLabel(i);
            if (pold)                                          // nach einer vorhandenen
            {
               nTargetFnNo = pold->GetTargetFuncNo();          // Zielfunktion suchen
               if ((bTakeAny && nTargetFnNo) ||
                   (nTargetFnNo == nNewTargetFnNo))
               {
                  pold->SetUndoAllValues();
                  CWinApp*pApp = AfxGetApp();
                  if (pApp->IsKindOf(RUNTIME_CLASS(CCaraWinApp)))
                  {
                    ((CCaraWinApp*)pApp)->SetUndoCmd(LAST_CMD_CURVE_EDIT, pold, NULL, 0, NULL);
                  }
                  pold->CopyValues(pcl);                       // Werte kopieren
                  if (HIWORD(nPos) & REPLACE_CURVE_TEXT)
                  {
                     pold->SetText(pcl->GetText());
                  }
                  if (!(HIWORD(nPos) & DONT_DELETE_INSERTED_CURVE))// wenn die Kurve gelöscht werden darf
                  {
                     delete pcl;                               // neue Kurve löschen
                  }
                  return true;                                 // Kurve eingefügt bzw. Werte ersetzt
               }
            }
            if (!(HIWORD(nPos) & FIT_INSERTED_CURVE_ID) && (i == count-1))
            {
               bTakeAny = true;
               i = -1;
            }
         }
      }

      if (LOWORD(nPos) == 0xffff) nPos = -1;
      if (m_Curves.InsertLabel(pcl, nPos))             // Kurve einfügen
      {
         pcl->SetTransformation(this);                         // Transformationsobjekt mit der Kurve verknüpfen
         pcl->SetLogFont(&m_CurveLogFont);                     // Kurvenschrift setzen, damit sie im Plot einheitlich ist
      }
      else ASSERT(false);
   }
   else return false;
   return true;
}

/******************************************************************************
* Name       : RemoveCurve                                                    *
* Definition : bool RemoveCurve(CCurveLabel *);                               *
* Zweck      : Entfernen einer Kurve aus dem Plot.                            *
* Aufruf     : RemoveCurve(pcl)                                               *
* Parameter  :                                                                *
* pcl    (E) : Zeiger auf zu entfernende Kurve                 (CCurveLabel*) *
* nPos   (A) : Position der entfernten Kurve                   (int&)         *
* Funktionswert : Kurve entfernt (true, false)                 (bool)         *
******************************************************************************/
bool  CPlotLabel::RemoveCurve(CCurveLabel *pcl, int &nPos)
{
   bool bReturn = false;
   ASSERT_VALID(pcl);
   if (pcl)
   {
      int i, count;
      count = m_Curves.GetCount();
      if (count)
      {
         nPos = m_Curves.GetIndex(pcl);
         if (m_Curves.RemoveLabel(pcl))
         {
            if (pcl == m_pRefCurve) SetReferenceCurve(NULL);
            bReturn = true;
            SetChanged(true);
            pcl->SetTransformation(NULL);
            count = m_Curves.GetCount();
            bool bEdit = true;
            for (i=0; i<count; i++)                               // ist nur eine Kurve schreibgeschützt,
               if (((CCurveLabel*)m_Curves.GetLabel(i))->IsWriteProtect()) bEdit = false;
            m_Parameter.editable = bEdit;                         // so ist auch der Plot nicht mehr editierbar.
            if (count==0)
            {
               m_Unit_x.SetText(NULL);
               m_Unit_y.SetText(NULL);
            }
         }
      }
   }
   return bReturn;
}

//Parameterfunktionen
void CPlotLabel::SetGridLineColor(COLORREF color)
{
   m_GridLineColor = color;
   SetChanged(true);
}
void CPlotLabel::SetHeadingLogFont(LOGFONT logfont)
{
   m_PlotHeading.SetLogFont(&logfont);
   SetChanged(true);
}
void CPlotLabel::SetHeadingColor(COLORREF color)
{
   m_PlotHeading.SetTextColor(color);
   SetChanged(true);
}
void CPlotLabel::SetGridTextLogFont(LOGFONT logfont)
{
   m_Unit_x.SetLogFont(&logfont);
   m_Unit_y.SetLogFont(&logfont);
   SetChanged(true);
}
void CPlotLabel::SetGridTextColor(COLORREF color)
{
   m_Unit_x.SetTextColor(color);
   m_Unit_y.SetTextColor(color);
   SetChanged(true);
}
void CPlotLabel::SetCurveLogFont(LOGFONT logfont)
{
   int i, count = m_Curves.GetCount();
   for (i=0; i< count; i++)
   {
      CCurveLabel* pcl = (CCurveLabel*) m_Curves.GetLabel(i);
      pcl->SetLogFont(&logfont);
      pcl->SetDrawFrame(DoDrawFrame());
   }
   m_CurveLogFont = logfont;
   SetChanged(true);
}
void CPlotLabel::SetXDivision(int nd) 
{
   m_Parameter.xDivision = nd;
   SetChanged(true);
}
void CPlotLabel::SetYDivision(int nd) 
{
   m_Parameter.yDivision = nd;
   SetChanged(true);
}
void CPlotLabel::SetXUnit(TCHAR *psz)
{
   m_Unit_x.SetText(psz);
   int i, count = m_Curves.GetCount();
   TCHAR *unit;
   TCHAR *ptr1 = _tcsstr(psz, _T("["));
   if (ptr1) unit = &ptr1[1];
   else      unit = psz;
   TCHAR *ptr2 = _tcsstr(psz, _T("]"));
   if (ptr2) *ptr2 = 0;
   for (i=0; i< count; i++)
      ((CCurveLabel*)m_Curves.GetLabel(i))->SetXUnit(unit);
   if (ptr2) *ptr2 = _T(']');
   SetChanged(true);
}
void CPlotLabel::SetYUnit(TCHAR *psz)
{
   m_Unit_y.SetText(psz);
   int i, count = m_Curves.GetCount();
   TCHAR *unit;
   TCHAR *ptr1 = _tcsstr(psz, _T("["));
   if (ptr1) unit = &ptr1[1];
   else      unit = psz;
   TCHAR *ptr2 = _tcsstr(psz, _T("]"));
   if (ptr2) *ptr2 = 0;
   for (i=0; i< count; i++)
      ((CCurveLabel*)m_Curves.GetLabel(i))->SetYUnit(unit);
   if (ptr2) *ptr2 = _T(']');
   SetChanged(true);
}
void CPlotLabel::SaveStandards()
{
   gm_StdPltSets.sm_HeadingLogFont  = m_PlotHeading.GetLogFont();
   gm_StdPltSets.sm_HeadingColor    = m_PlotHeading.GetTextColor();
   gm_StdPltSets.sm_GridTextLogFont = m_Unit_x.GetLogFont();
   gm_StdPltSets.sm_GridTextColor   = m_Unit_x.GetTextColor();
   gm_StdPltSets.sm_GridLineColor   = m_GridLineColor;
   gm_StdPltSets.sm_CurveLogFont    = m_CurveLogFont;
   gm_StdPltSets.sm_PlotFrame       = CRectLabel::GetLogPen();
   gm_StdPltSets.sm_PlotBrush       = CRectLabel::GetLogBrush();
   gm_StdPltSets.sm_BkColor         = m_BkColor;
   gm_StdPltSets.sm_cNumMode_x      = m_Parameter.xNumMode;
   gm_StdPltSets.sm_cNumMode_y      = m_Parameter.yNumMode;
   gm_StdPltSets.sm_cNumMode_x     |= (m_Parameter.xNoEndNulls) ? 0x80:0;
   gm_StdPltSets.sm_cNumMode_y     |= (m_Parameter.yNoEndNulls) ? 0x80:0;
   gm_StdPltSets.sm_cRound_x        = m_cRound_x;
   gm_StdPltSets.sm_cRound_y        = m_cRound_y;
   gm_StdPltSets.sm_wGridStep_x     = m_wGridStep_x;
   gm_StdPltSets.sm_wGridStep_y     = m_wGridStep_y;
   gm_StdPltSets.sm_nDrawframe      = m_State.drawframe;
}

void CPlotLabel::CheckCurveColors(bool bTest)
{
   int i, numCurves = m_Curves.GetCount();
   CCurveLabel* pcl;
   for (i=0; i<numCurves; i++)
   {
      pcl = (CCurveLabel*)m_Curves.GetLabel(i);
      if (bTest)
      {
         pcl->SetInterupted(false);
         pcl->SetSorted(false);
      }
      if (!pcl->IsSorted())
      {
         pcl->TestXSorted();
      }
      if (pcl->GetColor() == 0xFFFFFFFF) 
         pcl->SetColor(gm_StdPltSets.sm_CurveColors[i%NUM_CURVE_COLORS]);
   }
}

/******************************************************************************
* Name       : GetStepValue                                                   *
* Definition : double GetStepValue(int, int, double);                         *
* Zweck      : Ermittlung einer gerundeten Schrittweite und eines             *
*              Positionswertes in Weltkoordinaten aus einer vorgegebenen      *
*              Schrittweite und einer Position in logischen Koordinaten.      *
* Aufruf     : GetStepValue(nStep, nPos, dRound);                             *
* Parameter  :                                                                *
* nStep   (E): Schrittweite in logischen Koordinaten                 (int)    *
* nPos    (E): Position auf der jeweiligen Achse                     (int)    *
* dRound  (A): gerundete Position                                    (double&)*
* Funktionswert : gerundete Schrittweite                             (double) *
******************************************************************************/
double CPlotLabel::GetStepValue(int nStep, int nPos, double &dRound)
{
   ETSDIV_DIV div;
   ZeroMemory(&div, sizeof(ETSDIV_DIV));
   div.l2        = 10;
   div.stepwidth = 1;
   double   dStep;
   CPoint   ptStep(0, nPos);
   DPOINT   dpStep;

   TransformPoint(&ptStep, &dpStep);
   div.f1    = dpStep.y;
   ptStep.y += nStep;
   TransformPoint(&ptStep, &dpStep);
   div.f2    = dpStep.y;

   if (CEtsDiv::CalculateLinDivision(&div, dStep, dRound))
      return dStep;
   else
      return 0.0;
}

/******************************************************************************
* Name       : DeleteCurrentRange                                             *
* Definition : void DeleteCurrentRange();                                     *
* Zweck      : Löschen des aktuellen Bereiches                                *
* Aufruf     : DeleteCurrentRange();                                          *
* Parameter  : keine                                                          *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CPlotLabel::DeleteCurrentRange()
{
   if ((m_RangePos != NULL) && (m_RangeList.GetCount() > 1) && (m_RangePos != m_RangeList.GetHeadPosition()))
   {
      POSITION pos = m_RangePos;
      delete m_RangeList.GetPrev(m_RangePos);
      SetRange((LPRANGE) m_RangeList.GetAt(m_RangePos));
      m_RangeList.RemoveAt(pos);
   }
}

/******************************************************************************
* Name       : CheckDeleteCurrentRange                                        *
* Definition : bool CheckDeleteCurrentRange();                                *
* Zweck      : Prüfen, ob der aktuelle Bereich gelöscht werden kann.          *
* Aufruf     : CheckDeleteCurrentRange()                                      *
* Parameter  : keine                                                          *
* Funktionswert : (true, false)                                        (bool) *
******************************************************************************/
bool CPlotLabel::CheckDeleteCurrentRange()
{
   return ((m_RangePos != NULL) && (m_RangeList.GetCount() > 1) && (m_RangePos != m_RangeList.GetHeadPosition())) ? true : false;
}

/******************************************************************************
* Name       : CheckCurveDivision                                             *
* Definition : int CheckCurveDivision(CLabel *, void *);                      *
* Zweck      : Überprüft den Plot, ob Kurven mit anderer Teilungen            *
*              dargestellt werden sollen.                                     *
* Aufruf     : CheckCurveDivision(pl, pParam);                                *
* Parameter  :                                                                *
* Funktionswert :                                                    (int)    *
******************************************************************************/
int CPlotLabel::CheckCurveDivision(CLabel *pl, void *pParam)
{
   ASSERT(pl     != NULL);
   ASSERT_KINDOF(CCurveLabel, pl);
   ASSERT(pParam != NULL);
   ProcessLabel* pProcL = (ProcessLabel*)pParam;
   ASSERT(pProcL->pl != 0);
   ASSERT_KINDOF(CPlotLabel, pProcL->pl);
   if (pProcL->pParam1 && pl->IsVisible())
   {
      bool *pDifferent = (bool*)pProcL->pParam1;
      pDifferent[0]    = ((((CPlotLabel*)pProcL->pl)->GetXDivision()==ETSDIV_LINEAR) ^ (((CCurveLabel*)pl)->GetXDivision() == ETSDIV_LINEAR));
      if (((CCurveLabel*)pl)->IsInterupted()) pDifferent[0] = 1;
      if (pDifferent[0]) return STOP_PROCESSING;
   }
   return 0;
}

/******************************************************************************
* Name       : SetReferenceCurve                                              *
* Definition : bool SetReferenceCurve(CCurveLabel *);                         *
* Zweck      : Setzt die Referenzkurve eines Plots                            *
* Aufruf     : SetReferenceCurve(pcl)                                         *
* Parameter  :                                                                *
* pcl     (E): Neue Refernzkurve, oder NULL, wenn die aktuelle Referenz       *
*              gelöscht werden soll.                                          *
* Funktionswert : (true, false)                                         (bool)*
******************************************************************************/
bool CPlotLabel::SetReferenceCurve(CCurveLabel *pcl)
{
   if (pcl != m_pRefCurve)
   {
      if (pcl)         pcl->SetReference(true);
      if (m_pRefCurve) m_pRefCurve->SetReference(false);
      m_pRefCurve = pcl;
      return true;
   }
   return false;
}

/******************************************************************************
* Name       : SetReferenceOn                                                 *
* Definition : void SetReferenceOn(bool);                                     *
* Zweck      : Ein- bzw. ausschalten der Referenzdarstellung                  *
* Aufruf     : SetReferenceOn(b);                                             *
* Parameter  :                                                                *
* b       (E): Kurve ein/aus (true,false)                            (bool)   *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CPlotLabel::SetReferenceOn(bool b)
{
   m_bReferenceOn = b;
   if (m_pRefCurve) m_pRefCurve->SetReference(m_bReferenceOn);
};

bool CPlotLabel::ZoomPolarPlot(CDC *pDC, CPoint ptMouse, int nMsg)
{
   CRect  rcCircle;
   CPoint ptM((m_View.left+m_View.right)>>1, (m_View.top+m_View.bottom)>>1), ptC;
   CPoint ptDiff = ptM - ptMouse; 
   int nRadius = (int)hypot((double)ptDiff.x, (double)ptDiff.y);
   DPOINT dp;
   rcCircle.left   = ptM.x - nRadius;
   rcCircle.right  = ptM.x + nRadius;
   rcCircle.top    = ptM.y + nRadius;
   rcCircle.bottom = ptM.y - nRadius;

   TransformPoint(&ptMouse, &dp);
   if (nMsg == WM_LBUTTONDOWN)
   {
      CCircleLabel *pcl;
      pcl = new CCircleLabel(&rcCircle);
      LOGPEN pen;
      pen.lopnColor   = CLabel::gm_ShapePenColor;
      pen.lopnWidth.x = pen.lopnWidth.y = 1;
      pen.lopnStyle   = PS_SOLID;
      pcl->SetLogPen(&pen);
      pcl->SetPicked(true);
      pcl->SetdValue(dp.y);
      pcl->SetDrawPickPoints(false);
      pcl->DrawShape(pDC, true);
      m_Gridlines_y.InsertLabel(pcl);

      pcl = new CCircleLabel(&rcCircle);
      pcl->SetLogPen(&pen);
      pcl->SetPicked(true);
      pcl->SetDrawPickPoints(false);
      pcl->SetdValue(dp.y);
      m_Gridlines_y.InsertLabel(pcl);
      pcl->DrawShape(pDC, true);
   }
   else if (nMsg == WM_MOUSEMOVE)
   {
      CLabel *pl = m_Gridlines_y.GetLabel(m_Gridlines_y.GetCount()-1);
      if (pl && pl->IsKindOf(RUNTIME_CLASS(CCircleLabel)))
      {
         CCircleLabel *pcl = (CCircleLabel*) pl;
         pcl->DrawShape(pDC, false);
         pcl->SetdValue(dp.y);
         pcl->CLabel::SetPoint(rcCircle.TopLeft(), 0);
         pcl->CLabel::SetPoint(rcCircle.BottomRight(), 1);
         pcl->DrawShape(pDC, true);
      }
   }
   else if (nMsg == WM_LBUTTONUP)
   {
      RANGE range = GetRange();
      CLabel *pl1 = m_Gridlines_y.GetLabel(m_Gridlines_y.GetCount()-2);
      if (pl1 && pl1->IsKindOf(RUNTIME_CLASS(CCircleLabel)))
      {
         range.y1 = ((CCircleLabel*)pl1)->GetdValue();
      }
      CLabel *pl2 = m_Gridlines_y.GetLabel(m_Gridlines_y.GetCount()-1);
      if (pl2 && pl2->IsKindOf(RUNTIME_CLASS(CCircleLabel)))
      {
         range.y2 = ((CCircleLabel*)pl2)->GetdValue();
      }
      if (range.y1 > range.y2) swap(range.y1, range.y2);
      if (!SetRange(&range))
      {
         if (pl1)
         {
            m_Gridlines_y.RemoveLabel(pl1);
            pl1->DrawShape(pDC, false);
            delete pl1;
         }
         if (pl2)
         {
            m_Gridlines_y.RemoveLabel(pl2);
            pl2->DrawShape(pDC, false);
            delete pl2;
         }
         return false;
      }
   }
   return true;
}

bool CPlotLabel::PanZoom(int nZoom)
{
   if (m_RangePos)
   {
      RANGE *pR = (RANGE*)m_RangeList.GetAt(m_RangePos);
      double dX = pR->x2 - pR->x1;
      double dY = pR->y2 - pR->y1;
      switch (nZoom)
      {
         case PAN_ZOOM_RIGHT:
            if (pR->x2 + dX > m_DefaultRange.x2)
            {
               return false;
            }
            pR->x1 += dX;
            pR->x2 += dX;
            break;
         case PAN_ZOOM_LEFT:
            if (pR->x1 - dX < m_DefaultRange.x1)
            {
               return false;
            }
            pR->x1 -= dX;
            pR->x2 -= dX;
            break;
         case PAN_ZOOM_UP:
            if (pR->y2 + dY > m_DefaultRange.y2)
            {
               return false;
            }
            pR->y1 += dY;
            pR->y2 += dY;
            break;
         case PAN_ZOOM_DOWN:
            if (pR->y1 - dY < m_DefaultRange.y1)
            {
               return false;
            }
            pR->y1 -= dY;
            pR->y2 -= dY;
            break;
      }
      return SetRange(pR);
   }
   return false;
}

int CPlotLabel::CheckInsertCurves(CLabel *pl, void *pParam)
{
   ASSERT(pl != NULL);
   ASSERT(pParam != NULL);
   ASSERT_KINDOF(CCurveLabel, pl);
   CCurveLabel* pcl = (CCurveLabel*)pl;

   switch (pcl->GetXDivision())
   {
      case CCURVE_POLAR: 
         *((int*)pParam) |= NO_LOGARITHMIC_CURVES;
         break;
      case CCURVE_LOGARITHMIC: 
      case CCURVE_FREQUENCY: 
         *((int*)pParam) |= NO_POLAR_CURVES;
         break;
   }
   return 0;
}

LRESULT CPlotLabel::GetHelpCmd(CDC *pDC, CPoint p)
{
   DPOINT dp;
   CRect rc;

   if (m_View.PtInRect(p))
   {
      if ((m_Unit_x.IsOnLabel(pDC, &p, NULL) != CLabel::IsOutside()) || 
          (m_Unit_y.IsOnLabel(pDC, &p, NULL) != CLabel::IsOutside()))
      {
         return HID_BASE_COMMAND + ID_PLOT_UNITS;
      }
      if ((m_Gridtext_x.IsOnLabel(pDC, &p, NULL) != CLabel::IsOutside()) || 
          (m_Gridtext_y.IsOnLabel(pDC, &p, NULL) != CLabel::IsOutside()))
      {
         return HID_BASE_COMMAND + ID_PLOT_GRIDTEXT;
      }
      if (CatchGridValue(pDC, &p, &dp))
      {
         return HID_BASE_COMMAND + ID_PLOT_GRIDLINES;
      }
   }
   else if (p.y > m_View.bottom)
   {
      return HID_BASE_COMMAND + ID_PLOT_HEADING;
   }
   else if (p.y < m_View.top)
   {
      return HID_BASE_COMMAND + ID_PLOT_CURVETEXT;
   }

   return HID_BASE_COMMAND + ID_PLOT_PLOT;
}

/******************************************************************************
* Name       : TransformPoint                                                 *
* Definition : virtual void TransformPoint(LPDPOINT , CPoint *);              *
* Zweck      : Transformieren eines Punktes von Weltkoordinaten in Logische   *
*              Koordinaten unter Berücksichtigung der Achseneinteilung        *
*              (linear, logarithmisch) und einer Referenzkurve, die definiert *
*              werden kann.                                                   *
* Aufruf     : TransformPoint(pdp, pnp);                                      *
* Parameter  :                                                                *
* pdp    (E) : Weltkoordinatenpunkt                                 (DPOINT*) *
* pnp    (A) : Logischer Koordinatenpunkt                           (CPoint*) *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CPlotLabel::TransformPoint(LPDPOINT pdp, CPoint * pnp)
{
   DPOINT dp ={pdp->x, pdp->y};
   if (m_bReferenceOn && m_pRefCurve)
   {
      dp.y -= m_pRefCurve->Get_Y_Value(dp.x);
      double dOffset = m_pRefCurve->GetOffset();
      if (dOffset!=0.0) dp.y -= dOffset;
   }

   if (m_pdOffset) dp.y += *m_pdOffset;

   if (m_Parameter.xDivision == ETSDIV_POLAR)
   {
      PolarToCartesian(&dp);
   }
   else
   {
      switch (m_Parameter.xDivision)
      {
         case ETSDIV_LOGARITHMIC: case ETSDIV_FREQUENCY:
            if (dp.x > 0) dp.x  = log10(dp.x);
            else          dp.x  = m_Range.x1 - 1;
            break;
         case ETSDIV_LINEAR:
         default: break;
      }

      switch (m_Parameter.yDivision)
      {
         case ETSDIV_LOGARITHMIC: case ETSDIV_FREQUENCY:
            if (dp.y > 0) dp.y  = log10(dp.y);
            else          dp.y  = m_Range.y1 - 1;
            break;
         case ETSDIV_LINEAR: case ETSDIV_POLAR:
         default: break;
      }
   }

   CTransformation::TransformPoint(&dp, pnp);
}

void CPlotLabel::PolarToCartesian(DPOINT *pdp)
{
   double dRadius;
   if (m_Parameter.yDivision!=ETSDIV_LINEAR)
   {
      if (pdp->y > 0) pdp->y = log10(pdp->y);
      else            pdp->y = m_Range.y1;
      dRadius = pdp->y - m_Range.y1; 
   }
   else
   {
      dRadius = pdp->y - m_Range.y1;
   }
   DWORD  dwFlags = ETSDIV_CALC_POLAR_VALUE;
   switch (m_Parameter.angleformat)
   {
      case CCURVE_GRAD: dwFlags |= ETSDIV_POLAR_GRAD; break;
      case CCURVE_RAD:  dwFlags |= ETSDIV_POLAR_RAD;  break;
      case CCURVE_GON:  dwFlags |= ETSDIV_POLAR_GON;  break;
   }

   if (dRadius < 0)                                            // negativer Radius
   {
      pdp->x = pdp->y = 0.0;                                   // wird Null
   }
   else
   {
      CEtsDiv::GetSineCosineOfAngle(0, dwFlags, pdp->x, pdp->x, pdp->y);
      pdp->x *= dRadius;                                      // in lineare Koordinaten umrechnen
      pdp->y *= dRadius;
   }
}

/******************************************************************************
* Name       : TransformPoint                                                 *
* Definition : virtual void TransformPoint(CPoint *, LPDPOINT);               *
* Zweck      : Transformieren eines Punktes von Logischen Koordinaten in Welt-*
*              koordinaten unter Berücksichtigung der Achseneinteilung        *
*              (linear, logarithmisch) und einer definierbaren Referenzkurve. *
* Aufruf     : TransformPoint(pnp, pdp);                                      *
* Parameter  :                                                                *
* pnp    (E) : Logischer Koordinatenpunkt                           (CPoint*) *
* pdp    (A) : Weltkoordinatenpunkt                                 (DPOINT*) *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CPlotLabel::TransformPoint(CPoint *pnp, LPDPOINT pdp)
{
   CTransformation::TransformPoint(pnp, pdp);

   if (m_Parameter.xDivision == ETSDIV_POLAR)
   {
      CartesianToPolar(pdp);
   }
   else
   {
      switch (m_Parameter.yDivision)
      {
         case ETSDIV_LOGARITHMIC: case ETSDIV_FREQUENCY:
            pdp->y  = pow((double)10, pdp->y);
            break;
         case ETSDIV_LINEAR: case ETSDIV_POLAR:
         default: break;
      }

      switch (m_Parameter.xDivision) 
      {
         case ETSDIV_LOGARITHMIC: case ETSDIV_FREQUENCY:
            pdp->x  = pow((double)10, pdp->x);
            break;
         case ETSDIV_LINEAR:
         default: break;
      }
   }
   if (m_bReferenceOn && m_pRefCurve)
   {
      pdp->y += m_pRefCurve->Get_Y_Value(pdp->x);
      double dOffset = m_pRefCurve->GetOffset();
      if (dOffset != 0.0) pdp->y += dOffset;
   }

   if (m_pdOffset) pdp->y -= *m_pdOffset;
}

void CPlotLabel::CartesianToPolar(DPOINT *pdp)
{
   double dRadius = hypot(pdp->x, pdp->y);
   pdp->x = atan2(pdp->x, pdp->y);
   if (pdp->x < 0) pdp->x += 2*M_PI;
   switch (m_Parameter.angleformat)
   {
      case CCURVE_GRAD: pdp->x *= M_180_D_PI; break;
      case CCURVE_GON:  pdp->x *= M_200_D_PI; break;
   }
   pdp->y = dRadius + m_Range.y1;
   if (m_Parameter.yDivision!=ETSDIV_LINEAR)
   {
      pdp->y = pow((double)10, pdp->y);
   }
}

/******************************************************************************
* Name       : Polyline                                                       *
* Definition : bool Polyline(CDC*,int,int,int,CCurveLabel*);                  *
* Zweck      : Zeichnen der Kurvenlinie, in Abhängikkeit von ihren Parametern *
* Aufruf     : Polyline(pDC, nFormat, nStart, nCount, pcl);                   *
* Parameter  :                                                                *
* Funktionswert : Kurve gezeichnet (true, false)                      (bool)  *
******************************************************************************/
bool CPlotLabel::Polyline(CDC *pDC, int /* nFormat */, int nStart, int nCount, CCurveLabel *pcl)
{
   CFloatPrecision _fp;
   if (m_Parameter.xDivision == ETSDIV_POLAR)
   {
      int i, nEnd = nCount + nStart;
      double *pdOffset = NULL;
      DPOINT dp;
      bool   bRefOn  = m_bReferenceOn,
             bOffset = false,
             bStart  = true;
      m_bReferenceOn = false;                                  // Referenzkurve subtrahieren
      m_Parameter.xDivision = ETSDIV_LINEAR;                   // lineare Transformation einstellen (cliping)
      int nOldyDiv = m_Parameter.yDivision;

      if (m_pdOffset!=NULL)                                    // Offset addieren
      {
         pdOffset   = m_pdOffset;
         bOffset    = true;
         m_pdOffset = NULL;
      }

      for (i=nStart; i<nEnd; i++)
      {
         dp = *((DPOINT*)&pcl->GetDPoint(i));

         if (bRefOn && m_pRefCurve)
         {
            dp.y -= m_pRefCurve->GetY_Value(i);
         }
         if (bOffset) dp.y += *pdOffset;

         m_Parameter.yDivision = nOldyDiv;
         PolarToCartesian(&dp);
         m_Parameter.yDivision = ETSDIV_LINEAR;
         if (bStart)
         {
            Moveto(pDC, &dp);
            bStart = false;
         }
         else if (!Lineto(pDC, &dp))
         {
            REPORT("Error Lineto");
            return false;
         }
      }
      m_pdOffset            = pdOffset;
      m_bReferenceOn        = bRefOn;
      m_Parameter.xDivision = ETSDIV_POLAR;                    // Polartransformation zurückstellen
      m_Parameter.yDivision = nOldyDiv;
   }
   else if (pcl->IsSorted() && !pcl->IsInterupted() && !pcl->IsScatter())
   {
      bool   bStart = true,
             bReference = false;
      double dXValOrg, dXVal, dY, dYMin(0), dYMax(0), dXOld(0), dYOld(0),
            *pdOffset = NULL;
      int    nXVal, i, 
             nXOld   = 1 << 30, 
             nXCount = 0,
             nEnd    = nCount + nStart;
      if (m_bReferenceOn)
      {
         bReference     = true;
         m_bReferenceOn = false;
      }
      if (m_pdOffset)
      {
         pdOffset = m_pdOffset;
         m_pdOffset = NULL;
      }
      if ((nStart == 0) && (nCount==pcl->GetNumValues()))
      {
         pcl->Get_Y_Value(m_ClippRange.x1, &nStart);
         nStart = DecreaseLimited(nStart, 3, 0);
         pcl->Get_Y_Value(m_ClippRange.x2, &nEnd);
         nEnd   = IncreaseLimited(nEnd, 3, pcl->GetNumValues());
      }
      for (i=nStart; i<nEnd; i++)
      {
         dXVal = dXValOrg = pcl->GetX_Value(i);
         if (m_Parameter.xDivision != ETSDIV_LINEAR)
         {
            if (dXVal > 0) dXVal = log10(dXVal);
            else           dXVal = m_Range.x1 - 1;
         }
         dY = pcl->GetY_Value(i);
         dXVal  = m_flTran[0][0] * dXVal + m_flTran[0][1] * dY + m_flTran[0][2];
         if (bReference && m_pRefCurve)
         {
            dY -= m_pRefCurve->Get_Y_Value(dXValOrg);
            double dOffset = m_pRefCurve->GetOffset();
            if (dOffset!=0.0) dY -= dOffset;
         }
         if (pdOffset)  dY += *pdOffset;

         dXVal -= (nXVal = (long) dXVal);
         if (dXVal >= 0.5) nXVal++;
         if (dXVal <=-0.5) nXVal--;
         if (bStart)
         {
            if (!Moveto(pDC, dXValOrg, dY)) return false;
            bStart = false;
         }
         else
         {
            if (nXOld == nXVal)
            {
               if (nXCount == 0)
               {
                  dYMax = dYMin = dYOld;
                  dXOld = dXValOrg;
               }
               if (dYMin > dY) dYMin = dY;
               if (dYMax < dY) dYMax = dY;
               nXCount++;
            }
            else if ((nXCount) && (dYMin != dYMax))
            {
               if (fabs(dY-dYMax) > fabs(dY-dYMin)) swap(dYMax, dYMin);
               if (!Lineto(pDC, dXOld   , dYMin)) return false;
               if (!Lineto(pDC, dXOld   , dYMax)) return false;
               if (!Lineto(pDC, dXValOrg, dY   )) return false;
               nXCount = 0;
            }
            else
            {
               if (!Lineto(pDC, dXValOrg, dY)) return false;
               nXCount = 0;
            }
         }
         nXOld = nXVal;
         dYOld = dY;
         if (bReference && (m_pRefCurve == pcl) && (i != nEnd-1))
         {
            i = nEnd - 2;
         }
      }
      m_bReferenceOn = bReference;
      m_pdOffset     = pdOffset;
   }
   else
   {
      int    i, nEnd = nCount + nStart;
      bool   bStart = true;
      CPoint pt;
      CPen   pen, *pOld = NULL;
      LOGPEN lp = pcl->GetLogPen();
      SIZE   sz={0};
      //int    nScatterForm = pcl->GetScatterForm();
      if (pcl->IsScatter())
      {
         sz.cx = lp.lopnWidth.x;
         if (sz.cx < 2) sz.cx = 2;
         sz.cy = sz.cx;
         lp.lopnWidth.x = lp.lopnWidth.y = 1;
         pen.CreatePenIndirect(&lp);
         pOld = pDC->SelectObject(&pen);
      }
      
//      if (pcl->IsInterupted()) REPORT(_T("Interrupted %s"), pcl->GetText());

      for (i=nStart; i<nEnd; i++)
      {
         if (!pcl->IsValue(i))
         {
            bStart = true;
            continue;
         }
         SCurve sc = pcl->GetCurveValue(i);
         if (pcl->IsScatter())
         {
            TransformPoint((LPDPOINT)&sc, &pt);
            CRect rc(pt.x-sz.cx, pt.y-sz.cy, pt.x+sz.cx, pt.y+sz.cy);
            switch (pcl->GetScatterForm())
            {
               case 0: pDC->Rectangle(&rc); break;
               case 1: pDC->Ellipse(&rc); break;
               case 2: pDC->RoundRect(&rc, CPoint(sz.cx, sz.cy)); break;
               case 3: pDC->Arc(&rc, rc.BottomRight(), rc.TopLeft()); break;
               case 4: pDC->Arc(&rc, rc.TopLeft(), rc.BottomRight()); break;
            }
            continue;
         }
         if (bStart)
         {
            Moveto(pDC,  (DPOINT*)&sc);
            bStart = false;
         }
         if (!Lineto(pDC, (DPOINT*)&sc))
         {
            REPORT("Error Lineto");
            return false;
         }
      }
      if (pOld) pDC->SelectObject(pOld);
   }
   return true;
}

bool CPlotLabel::IsOnHeading(CDC *pDC, CPoint *pP)
{
   if (!m_PlotHeading.IsEmpty())
   {
      CRect rcHeading = m_PlotHeading.GetRect(pDC);
      CRect rcPlot    = GetRect(pDC);
      rcHeading.left  =  rcPlot.left;
      rcHeading.right =  rcPlot.right;
      return (rcHeading.PtInRect(*pP) != 0) ? true : false;
   }
   return (IsOnLabel(pDC, pP, NULL) != CLabel::IsOutside()) ? true : false;
}

