// CurveLabel.cpp: Implementierung der Klasse CCurveLabel.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "CEtsDiv.h"
#include "CurveLabel.h"
#include "PlotLabel.h"
#include "PreviewFileHeader.h"
#include "CaraFrameWnd.h"


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

IMPLEMENT_SERIAL(CCurveLabel, CTextLabel,0);

CCurveLabel::CCurveLabel()
{
   #ifdef CHECK_LABEL_CONSTRUCTION
   REPORT("CCurveLabel::Constructor");
   #endif
   Init();
}

CCurveLabel::CCurveLabel(CCurveLabel *pcl) : CMathCurve(pcl)
{
   #ifdef CHECK_LABEL_CONSTRUCTION
   REPORT("CCurveLabel::Constructor(arg)");
   #endif
   Init();
   if (pcl)
   {
      CLabel::ResetPickStates();
      SetXUnit(pcl->m_pszUnitx);
      SetYUnit(pcl->m_pszUnity);
      SetLocusUnit(pcl->m_pszUnitLocus);
      SetLogPen(&pcl->m_LogPen);
      m_nChannel = pcl->m_nChannel;
   }
}
void CCurveLabel::Init()
{
   m_nAlign         = TA_LEFT|TA_BOTTOM;
   m_pTran          = NULL;
   m_pCurvePen      = NULL;
   m_pFont          = NULL;
   m_nLineEditPoint = -1;
   m_bCursorDrawn   = false;
}

#ifdef _DEBUG
void CCurveLabel::AssertValid() const
{
   CTextLabel::AssertValid();
}
#endif

bool CCurveLabel::Draw(CDC *pDC, int bAtr)
{
   if (m_pTran == NULL) return CTextLabel::Draw(pDC, bAtr);
   bool bPicked      = IsPicked();
   bool bPointPicked = IsPointPicked();
   SetPicked(false);                         // Zeichnen des Pickzustandes in CLabel::Draw() unterdrücken !
   if ((bPointPicked) && (bAtr != 2)) SetCursorDrawn(false);

   if (CLabel::Draw(pDC, bAtr))
   {
      int nSaveDC = -1;
      SetPointPicked(false);
      SetPicked(bPicked);                    // alten Zustand wiederherstellen

      if (IsPenChanged())
      {
         SetLogPen(&m_LogPen);
      }
      if (bAtr == 2)
      {
#if _MFC_VER < 0x0710
         LPTSTR str = ExtendCurveText();
         CTextLabel::Draw(pDC, bAtr);
         if (str)
         {
            ((CEtsString*)&m_strText)->Attach(str);
            m_State.text_extended = false;
         }
#else
	     CString sOld = m_strText;
         ExtendCurveText();
         CTextLabel::Draw(pDC, bAtr);
		 m_strText = sOld;
#endif
      }
      else 
      {
         if (bAtr) SelectPen(pDC, m_pCurvePen);
         if (m_dOffset !=0) m_pTran->SetOffset(&m_dOffset);
         if (m_pArray) m_pTran->Polyline(pDC, m_Parameter.format, 0, m_nNum, this);
         if (m_Parameter.locus)
         {
            DrawLocusValue(pDC, 0);
            DrawLocusValue(pDC, m_nNum-1);
         }
         m_pTran->SetOffset(NULL);
      }
      SetPointPicked(bPointPicked);
      if (bPicked)
      {
//         DrawShape(pDC, true);
      }
   }
   SetPicked(bPicked);
   return true;
}

bool CCurveLabel::DrawShape(CDC *pDC, bool draw)
{
   if (m_pTran == NULL) return CTextLabel::DrawShape(pDC, draw);
   if (!CLabel::DrawShape(pDC, draw)) return false;
   if (m_pTran && pDC && m_pArray)
   {
      if (IsPointPicked())
      {
         SCurve Actual = GetCurveValue(m_nAct);
         SCurve *pdpCursor = &Actual;
         if (m_dOffset !=0) m_pTran->SetOffset(&m_dOffset);
         if (m_nLineEditPoint != -1)
         {
            if ((m_nAct >= 0) && (m_nAct < m_nNum) && (m_nAct != m_nLineEditPoint))
            {
               m_pTran->Moveto(pDC, GetX_Value(m_nLineEditPoint), GetY_Value(m_nLineEditPoint));
               m_pTran->Lineto(pDC, m_dpLinePoint.x             , m_dpLinePoint.y);
               if (m_dpLinePoint.x == Actual.x)      // Außerhalb
                  pdpCursor = &m_dpLinePoint;
               else 
                  m_pTran->Lineto(pDC, Actual.x, Actual.y);
            }
         }
         else if (IsSplineMode() && (m_nAct>1) && (m_nAct<m_nNum-3))
         {
            m_pTran->Polyline(pDC, m_Parameter.format, m_nAct-2, 5, this);
         }
         else
         {
            int nStart, nCount;
            if      (m_nAct <  1       ) nStart = 0       , nCount = 2;
            else if (m_nAct >= m_nNum-2) nStart = m_nNum-3, nCount = 2;
            else                         nStart = m_nAct-1, nCount = 3;
            ASSERT(nStart >= 0);
            ASSERT((nStart+nCount) < m_nNum);
            m_pTran->Polyline(pDC, m_Parameter.format, nStart, nCount, this);
         }
         if (pdpCursor)
         {
            DrawCursor(pDC, draw, (SCurve*)pdpCursor);
         }
         m_pTran->SetOffset(NULL);
      }
      else
      {
         CRect r = GetRect(pDC);
         CBrush  brush(RGB(255-GetRValue(m_Color), 255-GetGValue(m_Color), 255-GetBValue(m_Color)));
         pDC->SaveDC();
         pDC->SelectObject(&brush);
         pDC->PatBlt(r.left, r.top, r.right-r.left, r.bottom-r.top, PATINVERT);
         pDC->RestoreDC(-1);

         if (!draw)DrawCursor(pDC, draw);
      }
   }
   return true;
}

void CCurveLabel::DrawCursor(CDC *pDC, bool bDraw, SCurve* pdpCursor)
{ 
   if (pDC && (IsCursorDrawn() != bDraw))
   {
      SCurve dpCursor;
      if (!pdpCursor)
      {
         dpCursor  = GetDPoint();
         pdpCursor = &dpCursor;
      }
      CPoint  pt;
      if (m_dOffset !=0) m_pTran->SetOffset(&m_dOffset);
      CPen *pOldPen = pDC->SelectObject(gm_pShapePen);
      m_pTran->TransformPoint((LPDPOINT)pdpCursor, &pt);
      m_pTran->SetOffset(NULL);

      pDC->MoveTo(pt.x+gm_PickPointSize.cx, pt.y-gm_PickPointSize.cy);
      pDC->LineTo(pt.x-gm_PickPointSize.cx, pt.y+gm_PickPointSize.cy);
      pDC->SetPixelV(pt.x-gm_PickPointSize.cx, pt.y+gm_PickPointSize.cy, gm_ShapePenColor);

      pDC->MoveTo(pt.x-gm_PickPointSize.cx, pt.y-gm_PickPointSize.cy);
      pDC->LineTo(pt.x+gm_PickPointSize.cx, pt.y+gm_PickPointSize.cy);
      pDC->SetPixelV(pt.x+gm_PickPointSize.cx, pt.y+gm_PickPointSize.cy, gm_ShapePenColor);

      if (pOldPen) pDC->SelectObject(pOldPen);
      SetCursorDrawn(bDraw);
   }
}

void CCurveLabel::DrawLocusValue(CDC *pDC, int n)
{
   ASSERT(m_pTran!=NULL);
   CRect rcView = m_pTran->GetView();
   rcView.InflateRect(gm_PickPointSize);
   CPoint p, p2;
   int n2;
   if (n==0)        n2 = n+1;
   if (n==m_nNum-1) n2 = n-1;
   SCurve dp  = GetDPoint(n);
   SCurve dp2 = GetDPoint(n2);
   m_pTran->TransformPoint((DPOINT*)&dp, &p);
   m_pTran->TransformPoint((DPOINT*)&dp2, &p2);
   if (rcView.PtInRect(p))
   {
      UINT           nAlign = 0;
      CString        str;
      TCHAR           text[64];
      ETSDIV_NUTOTXT ntx = {GetLocusValue(n), m_pTran->GetXNumMode()|ETSDIV_NM_NO_END_NULLS, m_pTran->GetXRound(), 64, 0.0};
      ProcessLabel   pl = {NULL, 0, pDC, &rcView};
      CRect          rcText, rcUnit;
      CPen           pen(PS_SOLID, 1, m_pTran->m_GridLineColor);
      rcView      = m_pTran->GetView();
      rcUnit      = m_pTran->m_Unit_y.GetRect(pDC);
      rcView.left = rcUnit.right;
      rcUnit      = m_pTran->m_Unit_x.GetRect(pDC);
      rcView.top  = rcUnit.bottom;
      CEtsDiv::NumberToText(&ntx, text);
      str.Format("%s [%s]", text, GetLocusUnit());
      p2 = p2 - p;
      if (p2.x > 0) nAlign |= TA_RIGHT;
      if (p2.y < 0) nAlign |= TA_BOTTOM;
      p2 = p - p2;
      CTextLabel Text(&p2, str);
      Text.SetLogFont(&m_pTran->m_Unit_x.GetLogFont());
      Text.SetTextColor(m_Color);
      Text.SetTextAlign(nAlign);
      CLabelContainer::ClipLabel(&Text, &pl);
      rcText = Text.GetRect(pDC);
      p2 = GetNearestPoint(&rcText, &p);
      Text.Draw(pDC);
      int nSave = pDC->SaveDC();
      pDC->SelectObject(&pen);
      pDC->MoveTo(p2);
      pDC->LineTo(p);
      pDC->RestoreDC(nSave);
   }
}

void CCurveLabel::Serialize(CArchive& ar)
{
   #ifdef CHECK_LABEL_SERIALIZATION
   REPORT("CCurveLabel::Serialize()");
   #endif
   CTextLabel::Serialize(ar);

   long index;
   if (ar.IsStoring())
   {
      ArchiveWrite(ar, &m_Parameter, sizeof(BSParameter));
      ArchiveWrite(ar, &m_nNum,      sizeof(int));
      if (m_pArray) ArchiveWrite(ar, m_pArray, GetArraySize());
      ArchiveWrite(ar, &m_dOffset,   sizeof(double));

      if (m_dOffset != 0.0)
      {
         int npos;
         if ((npos = m_strText.Find(" + ")) != -1) m_strText = m_strText.Left(npos);
         if ((npos = m_strText.Find(" - ")) != -1) m_strText = m_strText.Left(npos);
      }

      index = FindPen(m_pCurvePen);
      ASSERT(index >= 0);
      ArchiveWrite(ar, &index, sizeof(long));
      CLabel::WriteVariableString(ar, m_pszUnitx);
      CLabel::WriteVariableString(ar, m_pszUnity);
      int nFormat = m_Parameter.format;
      if (m_Parameter.locus)
      {
         CLabel::WriteVariableString(ar, m_pszUnitLocus);
         m_Parameter.format = CF_SINGLE_SHORT;
      }
      switch (m_Parameter.format)
      {
         case CF_SINGLE_DOUBLE: case CF_SINGLE_FLOAT: case CF_SINGLE_SHORT:
            ArchiveWrite(ar, &m_dOrg , sizeof(double));
            ArchiveWrite(ar, &m_dStep, sizeof(double));
            break;
         default: break;
      }
      m_Parameter.format = nFormat;
      if (gm_nFileVersion >= 123)
      {
         ArchiveWrite(ar, &m_ParameterEx, sizeof(m_ParameterEx));
      }
   }
   else
   {
      if (m_pszUnitx) {delete[] m_pszUnitx; m_pszUnitx = NULL;}
      if (m_pszUnity) {delete[] m_pszUnity; m_pszUnity = NULL;}
      int nNum;
      if (gm_nFileVersion >= 121)
      {
         ArchiveRead(ar, &m_Parameter, sizeof(BSParameter));
         ArchiveRead(ar, &nNum,      sizeof(int));
         if (SetSize(nNum)) ArchiveRead(ar, m_pArray, GetArraySize());
         ArchiveRead(ar, &m_dOffset,   sizeof(double));
      }
      else
      {
         ArchiveRead(ar, &nNum,      sizeof(int));
         if (SetSize(nNum)) ArchiveRead(ar, m_pArray, GetArraySize());
         ArchiveRead(ar, &m_dOffset,   sizeof(double));
         ArchiveRead(ar, &m_Parameter, sizeof(BSParameter));
      }
      ArchiveRead(ar, &index, sizeof(long));
      if (gm_pPenArray)
      {
         if ((index >= 0) && (index < gm_nPens))
            SetLogPen(&gm_pPenArray[index]);
         else
         {
			 AfxThrowArchiveException(CArchiveException::genericException, NULL);
         }
      }
      else SetColor(0xFFFFFFFF);
      m_pszUnitx = CLabel::ReadVariableString(ar);
      m_pszUnity = CLabel::ReadVariableString(ar);
      int nFormat = m_Parameter.format;
      if (m_Parameter.locus)
      {
         ASSERT(m_pszUnitLocus == NULL);
         m_pszUnitLocus = CLabel::ReadVariableString(ar);
         m_Parameter.format = CF_SINGLE_SHORT;
      }
      switch (m_Parameter.format)
      {
         case CF_SINGLE_DOUBLE: case CF_SINGLE_FLOAT: case CF_SINGLE_SHORT:
            ArchiveRead(ar, &m_dOrg , sizeof(double));
            ArchiveRead(ar, &m_dStep, sizeof(double));
            if (gm_nFileVersion < 122)
            {
               switch (GetXDivision())
               {
                  case CCURVE_LOGARITHMIC: m_dStep = pow(10.0, m_dStep); break;
                  case CCURVE_FREQUENCY:   m_dStep = pow( 2.0, m_dStep); break;
               }
            }
            break;
         default: break;
      }
      m_Parameter.format = nFormat;
      if (gm_nFileVersion >= 123)
      {
         ArchiveRead(ar, &m_ParameterEx, sizeof(m_ParameterEx));
      }

//      if (gm_nFileVersion > 100)
         TestXSorted();
   }
}

int CCurveLabel::IsOnLabel(CDC *pDC, CPoint *pP, CRect *pR)
{
   if (!IsVisible()) return CLabel::IsOutside();
   if (!m_pTran)     return CTextLabel::IsOnLabel(pDC, pP, pR);
   if (m_pTran->IsOnLabel(pDC, pP, pR)==CLabel::IsOutside())
      return CLabel::IsOutside();

   int nDist = CLabel::IsOutside();
   m_nAct = -1;
   if ((pDC != NULL) && CTextLabel::IsOnLabel(pDC, pP, pR)!=CLabel::IsOutside())// Ist der Text getroffen, ok
   {
//      return 0;
      return CLabel::IsInside();
   }
   else if (pP && m_pArray)
   {
      int      i=0;
      CPoint   p1, p2;
      Complex dpPoint;
      Complex scPoint1, scPoint2;
      if (IsSortable() && !IsSorted()) TestXSorted();

      if (IsSorted())
      {
         if (m_dOffset !=0) m_pTran->SetOffset(&m_dOffset);
         bool bRef = m_pTran->IsReferenceOn();
         m_pTran->SetReferenceOn(false);
         m_pTran->TransformPoint(pP, (DPOINT*)&dpPoint);       // Punkt in Weltkoordinaten ausrechnen
         m_pTran->SetReferenceOn(bRef);
         Get_Y_Value(dpPoint.realteil, &i);                    // mögliche Linie finden
         *((SCurve*)&scPoint1) = GetCurveValue(i);
         m_pTran->TransformPoint((DPOINT*)&scPoint1, &p1);     // 1. Punkt in Logischen Koordinaten ausrechnen
         *((SCurve*)&scPoint2) = GetCurveValue(i+1);
         m_pTran->TransformPoint((DPOINT*)&scPoint2, &p2);     // 2. Punkt in Logischen Koordinaten ausrechnen
         m_pTran->SetOffset(NULL);
         nDist = HitLine(&p1, &p2, pP, pR);
         if (nDist != CLabel::IsOutside())
         {
            double dD1 = Betrag(dpPoint - scPoint1);
            double dD2 = Betrag(dpPoint - scPoint2);
            if (dD1 < dD2) m_nAct = i;
            else           m_nAct = i+1;
         }
      }
      else
      {
         if (m_dOffset !=0) m_pTran->SetOffset(&m_dOffset);
         bool bRef = m_pTran->IsReferenceOn();
         m_pTran->SetReferenceOn(false);
         m_pTran->TransformPoint(pP, (DPOINT*)&dpPoint);       // Punkt in Weltkoordinaten ausrechnen
         m_pTran->SetReferenceOn(bRef);
         for (i=1; i<m_nNum; i++)
         {
            *((SCurve*)&scPoint1) = GetCurveValue(i-1);
            m_pTran->TransformPoint((DPOINT*)&scPoint1, &p1);   // 1. Punkt in Logischen Koordinaten ausrechnen
            *((SCurve*)&scPoint2) = GetCurveValue(i);
            m_pTran->TransformPoint((DPOINT*)&scPoint2, &p2);   // 2. Punkt in Logischen Koordinaten ausrechnen
            nDist = HitLine(&p1, &p2, pP, pR);
            if (nDist != CLabel::IsOutside())
            {
               double dD1 = Betrag(dpPoint - scPoint1);
               double dD2 = Betrag(dpPoint - scPoint2);
               if (dD1 > dD2) m_nAct = i;
               else           m_nAct = i-1;
               break;
            }
         }
         m_pTran->SetOffset(NULL);
      }
      if (nDist != CLabel::IsOutside())
      {
         m_nUndoPoint = m_nAct;
         if (m_nUndoPoint-1  > 0)      m_dpUndoPoints[0] = GetCurveValue(m_nUndoPoint-1);
         m_dpUndoPoints[1] = GetCurveValue(m_nUndoPoint);
         if (m_nUndoPoint+1  < m_nNum) m_dpUndoPoints[2] = GetCurveValue(m_nUndoPoint+1);
      }
   }
   return nDist;
}

bool CCurveLabel::SetMatchPoint(CPoint *p)                     // Kurvenwert suchen
{
   DPOINT dp;
   int i;
   if (m_pTran && m_pArray && IsEditable())                    // wenn das Transformationsobjekt vorhanden ist
   {                                                           // und die Funktion änderbar ist
      if (m_dOffset !=0) m_pTran->SetOffset(&m_dOffset);
      m_pTran->TransformPoint(p, &dp);
      if (dp.x <= GetX_Value(0)) return false;
      Get_Y_Value(dp.x, &i);
      if (IsSorted() && (i<m_nNum-1) && ((dp.x - GetX_Value(i)) > (GetX_Value(i+1) - dp.x))) i++;
      if (i < m_nNum)
      {
         m_nAct = i;
         m_nUndoPoint = m_nAct;
         if (m_nUndoPoint-1  > 0)      m_dpUndoPoints[0] = GetCurveValue(m_nUndoPoint-1);
         m_dpUndoPoints[1] = GetCurveValue(m_nUndoPoint);
         if (m_nUndoPoint+1  < m_nNum) m_dpUndoPoints[2] = GetCurveValue(m_nUndoPoint+1);
         return true;
      }
      m_pTran->SetOffset(NULL);
   }
   return false;
}

void CCurveLabel::SaveEditUndo(CArchive &ar)
{
   ar.Write(&m_nUndoPoint, sizeof(int));
   ar.Write(m_dpUndoPoints, sizeof(SCurve)*3);
   if (!UpdateLine(&ar))
   {
      int nNoLine = -1;
      ar.Write(&nNoLine, sizeof(int));
   }
}

void CCurveLabel::RestoreEditUndo(CArchive &ar)
{
   ar.Read(&m_nUndoPoint, sizeof(int));
   ar.Read(m_dpUndoPoints, sizeof(SCurve)*3);
   UndoChanges();
   int nStart;
   ar.Read(&nStart, sizeof(int));
   
   if (nStart != -1)          // -1 : nNoLine
   {
      int    i, nEnd;
      double dValue, dY_i,
             ymin = m_dpUndoPoints[0].y,
             ymax = m_dpUndoPoints[1].y;
      ar.Read(&nEnd, sizeof(int));
      if (nEnd < m_nNum)      // Der Endwert muß innerhalb dieses Bereiches liegen
      {
         if (nStart > 0       ) m_dpUndoPoints[0].x = GetX_Value(nStart-1);
         else                   m_dpUndoPoints[0].x = GetX_Value(0);
         if (nEnd   < m_nNum-1) m_dpUndoPoints[1].x = GetX_Value(nEnd+1);
         else                   m_dpUndoPoints[1].x = GetX_Value(m_nNum-1);

         for (i=nStart; i<=nEnd; i++)
         {
            ar.Read(&dValue, sizeof(double));
            dY_i = GetY_Value(i);
            if (ymin > dValue) ymin = dValue;
            if (ymax < dValue) ymax = dValue;
            if (ymin > dY_i  ) ymin = dY_i;
            if (ymax < dY_i  ) ymax = dY_i;
            SetY_Value(i, dValue);
         }
         m_dpUndoPoints[0].y = ymin;
         m_dpUndoPoints[1].y = ymax;
      }
   }
}

void CCurveLabel::UndoChanges()
{
   if (m_pArray && (m_nUndoPoint>=0) && (m_nUndoPoint<m_nNum))
   {
      double xmin, xmax, ymin, ymax, dY;
      ymin = ymax = m_dpUndoPoints[1].y;
      if (m_nUndoPoint > 0)
      {
         dY = GetY_Value(m_nUndoPoint-1);
         if (ymin > m_dpUndoPoints[0].y) ymin = m_dpUndoPoints[0].y;
         if (ymax < m_dpUndoPoints[0].y) ymax = m_dpUndoPoints[0].y;
         if (ymin > dY                 ) ymin = dY;
         if (ymax < dY                 ) ymax = dY;
         SetY_Value(m_nUndoPoint-1, m_dpUndoPoints[0].y);
         if (m_nUndoPoint > 1) xmin = GetX_Value(m_nUndoPoint-2);
         else                  xmin = GetX_Value(m_nUndoPoint-1);
      }
      else 
      {
         xmin = m_dpUndoPoints[1].x;
      }
      dY = GetY_Value(m_nUndoPoint);
      if (ymin > dY) ymin = dY;
      if (ymax < dY) ymax = dY;
      SetY_Value(m_nUndoPoint, m_dpUndoPoints[1].y);
      if (m_nUndoPoint+1 < m_nNum)
      {
         dY = GetY_Value(m_nUndoPoint+1);
         if (ymin > m_dpUndoPoints[2].y) ymin = m_dpUndoPoints[2].y;
         if (ymax < m_dpUndoPoints[2].y) ymax = m_dpUndoPoints[2].y;
         if (ymin > dY                 ) ymin = dY;
         if (ymax < dY                 ) ymax = dY;
         SetY_Value(m_nUndoPoint+1, m_dpUndoPoints[2].y);
         if (m_nUndoPoint+2 < m_nNum) xmax = GetX_Value(m_nUndoPoint+2);
         else                         xmax = GetX_Value(m_nUndoPoint+1);
      }
      else
      {
         xmax = m_dpUndoPoints[1].x;
      }
      m_nUndoPoint = -1;
      m_dpUndoPoints[0].x = xmin;
      m_dpUndoPoints[1].x = xmax;
      m_dpUndoPoints[0].y = ymin;
      m_dpUndoPoints[1].y = ymax;
   }
}

bool CCurveLabel::GetUndoRgn(int n_cm, CDC * pDC, CRgn *prgn)
{
   CRect rect(0,0,0,0);
   if (m_pTran && prgn)
   {
      CPoint *ppt = (CPoint*)&rect;
      if (m_dOffset !=0) m_pTran->SetOffset(&m_dOffset);
      m_pTran->TransformPoint((LPDPOINT)&m_dpUndoPoints[0], &ppt[0]);
      m_pTran->TransformPoint((LPDPOINT)&m_dpUndoPoints[1], &ppt[1]);
      m_pTran->SetOffset(NULL);

      rect.InflateRect(gm_PickPointSize.cx<<1, (gm_PickPointSize.cx<<1)+GetWidth());
      ::LPtoDP(pDC->m_hDC, (LPPOINT)&rect, 2);
      rect.NormalizeRect();
      if (n_cm == RGN_COPY) prgn->DeleteObject();
      if (HRGN(*prgn) == NULL) return ((prgn->CreateRectRgnIndirect(&rect)) ? true : false);
      else
      {
         CRgn  rgn;
         if (rgn.CreateRectRgnIndirect(&rect))
         {
            return ((prgn->CombineRgn(prgn, &rgn, n_cm&0x0f) == ERROR) ? false : true);
         }
      }
   }
   return false;
}
/*
void CCurveLabel::ResetTextPos()
{
   m_rcClip.OffsetRect(m_OldTextPoint - m_pP[0]);
   m_pP[0] = m_OldTextPoint;
}
*/
void CCurveLabel::SetLogPen(LOGPEN* plp)
{
   m_LogPen = *plp;
   m_LogPen.lopnWidth.y =  m_LogPen.lopnWidth.x;
   m_pCurvePen = CLabel::FindPen(plp);
   LOGPEN textpen = m_LogPen;
   textpen.lopnWidth.x = textpen.lopnWidth.y = 0;
   CTextLabel::SetLogPen(&textpen);
   m_Color  = m_LogPen.lopnColor;
}

LOGPEN CCurveLabel::GetLogPen()
{
   LOGPEN  LogPen ={0, {0, 0}, 0};
   if (m_pCurvePen) m_pCurvePen->GetLogPen(&LogPen);
   return LogPen;
}
void CCurveLabel::SetTransformation(CPlotLabel *ptran)
{
   m_pTran = ptran;
   if (m_pTran)
   {
      m_State.drawframe = m_pTran->DoDrawFrame();
   }
};

// virtuelle Funktionen zum adaptieren der Klassen CTextLabel und CCurve
void CCurveLabel::SetPoint(CPoint p)
{
   if (IsFixed()) return;
   if (m_pTran && m_pArray && IsEditable() && IsPointPicked())
   {
      SCurve dp;
      if (m_dOffset !=0) m_pTran->SetOffset(&m_dOffset);
      m_pTran->TransformPoint(&p, (LPDPOINT)&dp);
      m_pTran->SetOffset(NULL);
      SetDPoint(dp, p);
      CLabel::SetChanged(true);
   }
   else
   {
      m_nAct = 0;
      CTextLabel::SetPoint(p);
   }
}

void CCurveLabel::SetDPoint(SCurve dp, CPoint p, int n)
{
   if (m_pArray && IsEditable())
   {
      if (m_nLineEditPoint != -1)                              // Linemodus
      {
         bool bOutside = true;
         int  nLinePoint;
         if      (dp.x <= GetX_Value(0)       ) m_nAct = 0;
         else if (dp.x >= GetX_Value(m_nNum-1)) m_nAct = m_nNum-1;
         else if (SetMatchPoint(&p))            bOutside = false;
//         else if (IsOnLabel(NULL, &p) != CLabel::IsOutside()) bOutside = false;
         else                                   return;        // SetMatchPoint setzt nur innerhalb des Kurvenbereiches

         nLinePoint = m_nAct;
         if (!bOutside)
         {
            if (nLinePoint < m_nLineEditPoint) nLinePoint++;
            else                               nLinePoint--;
         }
         if (nLinePoint <       0) return;
         if (nLinePoint >= m_nNum) return;

         m_dpLinePoint.x = GetX_Value(nLinePoint);
         m_dpLinePoint.y = dp.y;
      }
      else                                                     // Spline- oder Punkt-modus
      {
         if (n == -1)
         {
            if (IsPointPicked()) n = m_nAct;
            else return;
         }
         SInterpolynom  ip;
         SetY_Value(n, dp.y);
         if (IsSplineMode() && MakeInterpolynom(n-2, n, n+2, &ip))
         {
            SetY_Value(n-1, PolynomValue(GetX_Value(n-1), &ip));
            SetY_Value(n+1, PolynomValue(GetX_Value(n+1), &ip));
         }
      }
   }
}

bool CCurveLabel::UpdateLine(CArchive *par)
{
   if (m_pArray && (m_nLineEditPoint != -1))
   {
      SCurve dp1, dp2, dpi;
      double m;
      int    i,
             nLineEditPoint = m_nLineEditPoint,                // Punktindex zwischenspeichern
             nStart         = m_nLineEditPoint,
             nEnd           = m_nAct;

      m_nLineEditPoint = -1;                                   // und sofort zurücksetzen

      if (nStart >    nEnd)                                    // x-Werte fallend ?
      {
         swap(nStart, nEnd);                                   // vertauschen
         double dXLimit = (GetX_Value(0)+GetX_Value(1))*0.5;   // Schwellwert berechnen
         if (m_dpLinePoint.x > dXLimit) nStart++;              // wenn der aktuelle X-Wert größer als der Schwellwert ist,
      }                                                        // nStart erhöhen
      else
      {
         double dXLimit = (GetX_Value(m_nNum-1)+GetX_Value(m_nNum-2))*0.5;// Schwellwert berechnen
         if (m_dpLinePoint.x < dXLimit) nEnd--;                // wenn der aktuelle X-Wert kleiner als der Schwellwert ist,
      }                                                        // nEnd verringern


      if (nStart >= nEnd) return false;                        // ein Punkt reicht nicht für eine Linie

      if (nStart <       0) nStart = 0;                        // Bereichsprüfung ?
      if (nEnd   >= m_nNum) nEnd   = m_nNum-1;

      dp1 = GetCurveValue(nLineEditPoint);
      dp2 = m_dpLinePoint;

      if (dp1.x == dp2.x) return false;                        // x2 und x1 müssen ungleich sein

      if (m_pTran->IsReferenceOn())
      {
         dp1.y  -= m_pTran->GetReferenceCurve()->Get_Y_Value(dp1.x);
         dp2.y  -= m_pTran->GetReferenceCurve()->Get_Y_Value(dp2.x);
      }

      if (GetXDivision()!=ETSDIV_LINEAR)
      {
         dp1.x = log10(dp1.x);
         dp2.x = log10(dp2.x);
      }

      if (GetYDivision()!=ETSDIV_LINEAR)
      {
         dp1.y = log10(dp1.y);
         dp2.y = log10(dp2.y);

      }
         
      m = (dp2.y-dp1.y) / (dp2.x-dp1.x);                       // Steigung berechnen

      if (par)
      {
         par->Write(&nStart, sizeof(int));
         par->Write(&nEnd  , sizeof(int));
      }

      for (i=nStart; i<=nEnd; i++)
      {
         dpi = GetCurveValue(i);
         if (GetXDivision()!=ETSDIV_LINEAR) dpi.x = log10(dpi.x);
         dpi.y = m * (dpi.x - dp1.x) + dp1.y;                  // y = m * x + b
         if (m_pTran->IsReferenceOn())
         {
            dpi.y += m_pTran->GetReferenceCurve()->Get_Y_Value(dpi.x);
         }
         if (GetYDivision()!=ETSDIV_LINEAR) dpi.y = pow(10.0, dpi.y);
         if (par)
         {
            double value = GetY_Value(i);
            par->Write(&value, sizeof(double));
         }
         SetY_Value(i, dpi.y);
      }
      return true;
   }
   return false;
}

void CCurveLabel::MoveLabel(CPoint p)
{
   if (m_pTran == NULL) CTextLabel::MoveLabel(p);
}

CRect CCurveLabel::GetRect(CDC *pDC, UINT nFlags)
{
   CRect    rect(0,0,0,0);
   if (IsPointPicked())
   {
      if (m_pArray && m_pTran)
      {
         CPoint   p;
         int i, nStart, nEnd;

         if (m_dOffset !=0) m_pTran->SetOffset(&m_dOffset);

         if (m_nLineEditPoint != -1)
         {
            nStart = m_nLineEditPoint;
            nEnd   = m_nAct;
            m_pTran->TransformPoint((LPDPOINT)&m_dpLinePoint, &p);
            rect.SetRect(p, p);
         }
         else if (IsSplineMode())
         {
            nStart = m_nAct-2;
            nEnd   = m_nAct+2;
         }
         else
         {
            if      (m_nAct < 1       ) nStart = 0       , nEnd = 1;
            else if (m_nAct > m_nNum-2) nStart = m_nNum-2, nEnd = m_nNum-1;
            else                        nStart = m_nAct-1, nEnd = m_nAct+1;
         }

         if (nStart > nEnd)
         {
            swap(nStart, nEnd);
            nStart -= 2;
            if (nStart < 0) nStart = 0;
            nEnd   += 1;
            if (nEnd > m_nNum) nEnd = m_nNum;
         }
         else
         {
            nStart -= 1;
            if (nStart < 0) nStart = 0;
            nEnd   += 2;
            if (nEnd > m_nNum) nEnd = m_nNum;
         }

         for (i=nStart; i<nEnd; i++)
         {
            SCurve sc = GetCurveValue(i);
            m_pTran->TransformPoint((LPDPOINT)&sc, &p);
            if (rect.IsRectNull())
            {
               rect.SetRect(p, p);
               continue;
            }
            if (rect.left   > p.x) rect.left   = p.x;
            if (rect.top    > p.y) rect.top    = p.y;
            if (rect.right  < p.x) rect.right  = p.x;
            if (rect.bottom < p.y) rect.bottom = p.y;
         }

         CSize szInflate = gm_PickPointSize;
         szInflate.cx += m_LogPen.lopnWidth.x;
         szInflate.cy += m_LogPen.lopnWidth.y;
         rect.InflateRect(szInflate);
         m_pTran->SetOffset(NULL);
      }
   }
   else
   {
#if _MFC_VER < 0x0710
      LPTSTR str = ExtendCurveText();
      rect = CTextLabel::GetRect(pDC, nFlags&INFLATE_RECT);
      if (str)
      {
         ((CEtsString*)&m_strText)->Attach(str);
         m_State.text_extended = false;
      }
#else
	  CString sOld = m_strText;
      ExtendCurveText();
      rect = CTextLabel::GetRect(pDC, nFlags&INFLATE_RECT);
	  m_strText = sOld;
#endif
      CPoint two  = CLabel::PixelToLogical(pDC, 2, 2);
      rect.left  -= two.x;
      rect.right += two.x;
   }

   if ((nFlags&DEV_COORD) && pDC)
   {
      ::LPtoDP(pDC->m_hDC, (LPPOINT)&rect, 2);
      rect.NormalizeRect();
   }
   return rect;
}

void CCurveLabel::Write(HANDLE hfile)
{
   m_pszDescription = (TCHAR*)LPCTSTR(m_strText);
   CCurve::Write(hfile);
   m_pszDescription = NULL;
}

void CCurveLabel::Read(HANDLE hfile)
{
   CCurve::Read(hfile);
   if (m_pszDescription)
   {
      m_strText = _T(m_pszDescription);
      delete[] m_pszDescription;
      m_pszDescription = NULL;
   }
   else m_strText.Empty();
}

void CCurveLabel::GetChecksum(CFileHeader *pFH)
{
   m_pszDescription = (TCHAR*)LPCTSTR(m_strText);
   CCurve::GetChecksum(pFH);
   m_pszDescription = NULL;
}

void CCurveLabel::SetDescription(const TCHAR *psz)
{
   CTextLabel::SetText(psz);
}

bool CCurveLabel::SetMatchPoint(int n)
{
   if (n < 0)
   {
      m_nAct = 0;
      return false;
   }
   if (n >= m_nNum)
   {
      m_nAct = m_nNum-1;
      return false;
   }
   m_nAct = n;
   return true;
}

SCurve CCurveLabel::GetDPoint(int n)
{
   SCurve dummy = {0.0, 0.0};
   if (m_pArray)
   {
      if (n!=-1)
      {
         return GetCurveValue(n);
      }
      else
      {
         n = m_nAct;
         if (m_nLineEditPoint != -1)
         {
            SCurve dpLP = {GetX_Value(n), m_dpLinePoint.y};
            return dpLP;
         }
         else return GetCurveValue(n);
      }
   }
   return dummy;
}

CPoint CCurveLabel::GetPoint(int n)
{
   CPoint dummy(0,0);
   if (m_pTran && m_pArray)
   {
      if (m_dOffset !=0) m_pTran->SetOffset(&m_dOffset);
      if (n!=-1)
      {
         SCurve sc = GetCurveValue(n);
         m_pTran->TransformPoint((LPDPOINT)&sc, &dummy);
      }
      else
      {
         n = m_nAct;
         if ((n >= 0) && (n < m_nNum))
         {
            if (m_nLineEditPoint != -1)
            {
               DPOINT dpLP = {GetX_Value(n), m_dpLinePoint.y};
               m_pTran->TransformPoint((LPDPOINT)&dpLP, &dummy);
            }
            else
            {
               SCurve sc = GetCurveValue(n);
               m_pTran->TransformPoint((LPDPOINT)&sc, &dummy);
            }
         }
      }
      m_pTran->SetOffset(NULL);
   }
   return dummy;
}

bool CCurveLabel::GetLinePoints(SCurve &p1, SCurve &p2)
{
   if (m_nLineEditPoint == -1)      return false;
   if (m_nLineEditPoint == m_nAct)  return false;
   if (!m_pArray)                   return false;

   p1 = GetCurveValue(m_nLineEditPoint);
   p2 = m_dpLinePoint;

   if (p1.x == p2.x)                return false;
   return true;
}


void CCurveLabel::SetUndoAllValues()
{
   if ((m_pArray != NULL) && (m_nNum > 3))
   {
      CPoint point(0,0);
      SCurve dpEnd = GetCurveValue(m_nNum-1);
      m_nAct = 0;
      SetLineStartPoint();
      SetDPoint(dpEnd, point);
      m_nUndoPoint = 1;
      m_dpUndoPoints[0] = GetCurveValue(0);
      m_dpUndoPoints[1] = GetCurveValue(m_nUndoPoint);
      m_dpUndoPoints[2] = GetCurveValue(2);
   }
}

LPTSTR CCurveLabel::ExtendCurveText()
{
   LPTSTR str = NULL;
   bool bOffset = (m_dOffset!=NULL) ? true : false;
   if ((m_State.text_extended == false) && (IsACopy() || bOffset))
   {
#if _MFC_VER < 0x0710
      str = ((CEtsString*)&m_strText)->Detach();
#endif
      if (bOffset)
      {
         TCHAR        pszText[10];
         ETSDIV_NUTOTXT ntx;
         ntx.nmaxl     = 10;
         ntx.nround    = 2;                                    // Rundungsstellen
         ntx.nmode     = ETSDIV_NM_STD;                        // Darstellungsmodus
         ntx.dZero     = 0;
         if (m_pTran)
         {
            ntx.nround    = m_pTran->GetXRound();              // Rundungsstellen
            ntx.nmode     = m_pTran->GetXNumModeEx();          // Darstellungsmodus
         }
         ntx.fvalue    = fabs(m_dOffset);                      // Zahl ( Offset )
         TCHAR *pszYUnit = GetYUnit();
         if (!pszYUnit) pszYUnit = "";
         if (CEtsDiv::NumberToText(&ntx, pszText))
         {
            m_strText.Format("%s%s %c %s %s",
                          IsACopy()? "#" : "",
                          str,                                 // Kurventext
                          ((m_dOffset > 0.0)?'+':'-'),         // Vorzeichen
                          pszText,                             // Zahl
                          pszYUnit);                           // Kurveneinheit
         }
      }
      else if (IsACopy())
      {
         m_strText.Format("#%s", str);
      }
      m_State.text_extended = true;
   }
   return str;
}

int CCurveLabel::GetFormatEx()
{
   if (   GetFormat() == CF_ARBITRARY
	    || GetFormat() == CF_ARBITRARY_CONTR)
   {
	   CF_GET_CURVE_PARAMS gcp = {CF_GET_FORMAT_EX, 0, this, NULL, m_pGetCrvParam};
      return m_pGetCrvParamFkt(&gcp);
   }
   return 0;
}

int CCurveLabel::WaveParamFkt(CF_GET_CURVE_PARAMS*pP)
{
   if (pP != NULL)
   {
      CWaveParameter *pParam = (CWaveParameter*)pP->pParam;
      CCurveLabel *pCurve = (CCurveLabel*)pP->pCurve;
      switch (pP->nWhat)
      {
         case CF_GET_SIZE:
            if (pParam->pWaveFmtEx->nBlockAlign)
            {
               return (pParam->pWaveFile->nFileLength-pParam->pWaveFile->nHeaderLength) / pParam->pWaveFmtEx->nBlockAlign;
            }
            return 0;
         case CF_GET_VALUE:
            pP->pValue->x = pCurve->m_dOrg + pP->nIndex * pCurve->m_dStep;
            pP->nWhat = CF_GET_YVALUE;
            return WaveParamFkt(pP);
         case CF_GET_XVALUE:
            pP->pValue->x = pCurve->m_dOrg + pP->nIndex * pCurve->m_dStep;
            return 1;
         case CF_GET_YVALUE:
            if (pParam->bSigned)
            {
               if (pParam->pWaveFmtEx->wBitsPerSample == 8)
               {
                  pP->pValue->y = (double)(((char*)pCurve->m_pArray)[pP->nIndex*pParam->pWaveFmtEx->nChannels+pCurve->m_nChannel-1]);
                  return 1;
               }
               else if (pParam->pWaveFmtEx->wBitsPerSample == 16)
               {
                  pP->pValue->y = (double)(((short*)pCurve->m_pArray)[pP->nIndex*pParam->pWaveFmtEx->nChannels+pCurve->m_nChannel-1]);
                  return 1;
               }
               else if (pParam->pWaveFmtEx->wBitsPerSample == 24)
               {
                  return 0;
               }
            }
            else
            {
               if (pParam->pWaveFmtEx->wBitsPerSample == 8)
               {
                  pP->pValue->y = (double)((((char*)pCurve->m_pArray)[pP->nIndex*pParam->pWaveFmtEx->nChannels+pCurve->m_nChannel-1])-128);
                  return 1;
               }
               else if (pParam->pWaveFmtEx->wBitsPerSample == 16)
               {
                  pP->pValue->y = (double)((((short*)pCurve->m_pArray)[pP->nIndex*pParam->pWaveFmtEx->nChannels+pCurve->m_nChannel-1])-32768);
                  return 1;
               }
               else if (pParam->pWaveFmtEx->wBitsPerSample == 24)
               {
                  return 0;
               }
            }break;
         case CF_SET_VALUE:
            pP->nWhat = CF_SET_YVALUE;
            return  WaveParamFkt(pP);
         case CF_SET_XVALUE:
            return 0;
         case CF_SET_YVALUE:
            if (pParam->bSigned)
            {
               if (pParam->pWaveFmtEx->wBitsPerSample == 8)
               {
                  ((char*)pCurve->m_pArray)[pP->nIndex*pParam->pWaveFmtEx->nChannels+pCurve->m_nChannel-1] = (char)pP->pValue->y;
                  return 1;
               }
               else if (pParam->pWaveFmtEx->wBitsPerSample == 16)
               {
                  ((short*)pCurve->m_pArray)[pP->nIndex*pParam->pWaveFmtEx->nChannels+pCurve->m_nChannel-1] = (short)pP->pValue->y;
                  return 1;
               }
               else if (pParam->pWaveFmtEx->wBitsPerSample == 24)
               {
                  return 0;
               }
            }
            else
            {
               if (pParam->pWaveFmtEx->wBitsPerSample == 8)
               {
                  ((BYTE*)pCurve->m_pArray)[pP->nIndex*pParam->pWaveFmtEx->nChannels+pCurve->m_nChannel-1] = (BYTE)(pP->pValue->y+128);
                  return 1;
               }
               else if (pParam->pWaveFmtEx->wBitsPerSample == 16)
               {
                  ((WORD*)pCurve->m_pArray)[pP->nIndex*pParam->pWaveFmtEx->nChannels+pCurve->m_nChannel-1] = (WORD)(pP->pValue->y+32768);
                  return 1;
               }
               else if (pParam->pWaveFmtEx->wBitsPerSample == 24)
               {
                  return 0;
               }
           }  break;
         case CF_GET_Y_VALUE:
            return (int) ((pP->pValue->x - pCurve->m_dOrg) / pCurve->m_dStep);
         case CF_SORT_XASC:
            return 0;
         case CF_GET_X_MIN_STEP:
            pP->pValue->x = pCurve->m_dStep;
            return 1;
         case CF_DELETE_VALUES:
            pParam->nReference--;
            if (pParam->nReference == 0)
            {
               UnmapViewOfFile(pParam->pData);
               CloseHandle(pParam->hFileMapping);
               CloseHandle(pParam->hFile);
               delete pParam;
            }
            return 1;
         case CF_SET_SIZE:
            pCurve->m_nNum = pP->nIndex;
            return 1;
         case CF_WRITE_FILE:
            return 0;
         case CF_READ_FILE:
            return 0;
         case CF_COPY_CURVE:
         {
            CCurveLabel *pSourceCurve = (CCurveLabel*)pP->pParam;
            pCurve->m_pGetCrvParam = pSourceCurve->m_pGetCrvParam;
            pParam = (CWaveParameter*)pCurve->m_pGetCrvParam;
            pCurve->m_pArray = pSourceCurve->m_pArray;
            pParam->nReference++;
         }   return 1;
         case CF_COPY_VALUES:
            return 1;
	   }
   }
   return 0;
}

/*
CRect CCurveLabel::GetRect(CDC *pDC, UINT nFlags)
{
   CRect    rect(0,0,0,0);
   if (IsPointPicked())
   {
      if (m_pArray && m_pTran)
      {
         if (m_dOffset !=0) m_pTran->SetOffset(&m_dOffset);
         CPoint   p;
         int i;
         if (m_nLineEditPoint != -1)
         {
            m_pTran->TransformPoint((LPDPOINT)&m_dpLinePoint, &p);
            rect = CRect(p, p);
            int nStart = m_nLineEditPoint,
                nEnd   = m_nAct;
            if (nStart > nEnd)
            {
               swap(nStart, nEnd);
               nStart -= 2;
               if (nStart < 0) nStart = 0;
               nEnd   += 1;
               if (nEnd > m_nNum) nEnd = m_nNum;
            }
            else
            {
               nStart -= 1;
               if (nStart < 0) nStart = 0;
               nEnd   += 2;
               if (nEnd > m_nNum) nEnd = m_nNum;
            }
            for (i=nStart; i<nEnd; i++)
            {
               SCurve sc = GetCurveValue(i);
               m_pTran->TransformPoint((LPDPOINT)&sc, &p);
               if (rect.left   > p.x) rect.left   = p.x;
               if (rect.top    > p.y) rect.top    = p.y;
               if (rect.right  < p.x) rect.right  = p.x;
               if (rect.bottom < p.y) rect.bottom = p.y;
            }
         }
         else if (IsSplineMode() && (m_nAct>1) && (m_nAct<m_nNum-2))
         {
            SCurve sc = GetCurveValue(m_nAct-2);
            m_pTran->TransformPoint((LPDPOINT)&sc, &p);
            rect = CRect(p, p);
            for (i=m_nAct-1; i<m_nAct+3; i++)
            {
               sc = GetCurveValue(i);
               m_pTran->TransformPoint((LPDPOINT)&sc, &p);
               if (rect.left   > p.x) rect.left   = p.x;
               if (rect.top    > p.y) rect.top    = p.y;
               if (rect.right  < p.x) rect.right  = p.x;
               if (rect.bottom < p.y) rect.bottom = p.y;
            }
         }
         else
         {
            SCurve sc;
            if (m_nAct>0)
            {
               sc = GetCurveValue(m_nAct-1);
               m_pTran->TransformPoint((LPDPOINT)&sc, &p);
            }
            else
            {
               sc = GetCurveValue(m_nAct);
               m_pTran->TransformPoint((LPDPOINT)&sc, &p);
            }
            rect = CRect(p, p);
            if (m_nAct>0)
            {
               sc = GetCurveValue(m_nAct);
               m_pTran->TransformPoint((LPDPOINT)&sc, &p);
               if (rect.left   > p.x) rect.left   = p.x;
               if (rect.top    > p.y) rect.top    = p.y;
               if (rect.right  < p.x) rect.right  = p.x;
               if (rect.bottom < p.y) rect.bottom = p.y;
            }
            if (m_nAct<m_nNum-1)
            {
               sc = GetCurveValue(m_nAct+1);
               m_pTran->TransformPoint((LPDPOINT)&sc, &p);
               if (rect.left   > p.x) rect.left   = p.x;
               if (rect.top    > p.y) rect.top    = p.y;
               if (rect.right  < p.x) rect.right  = p.x;
               if (rect.bottom < p.y) rect.bottom = p.y;
            }
         }
         CSize szInflate = gm_PickPointSize;
         szInflate.cx += m_LogPen.lopnWidth.x;
         szInflate.cy += m_LogPen.lopnWidth.y;
         rect.InflateRect(szInflate);
         m_pTran->SetOffset(NULL);
      }
   }
   else
   {
      rect = CTextLabel::GetRect(pDC, nFlags&INFLATE_RECT);
      CPoint   two   = CLabel::PixelToLogical(pDC, 2, 2);
      rect.left  -= two.x;
      rect.right += two.x;
   }

   if ((nFlags&DEV_COORD) && pDC)
   {
      ::LPtoDP(pDC->m_hDC, (LPPOINT)&rect, 2);
      rect.NormalizeRect();
   }
   return rect;
}
*/
