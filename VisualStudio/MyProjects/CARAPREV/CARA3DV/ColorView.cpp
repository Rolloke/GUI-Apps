// ColorView.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "CARA3DV.h"
#include "ColorView.h"
#include "LevelColor.h"
#include "..\ColorArray.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorView
CColorView::CColorView()
{
   m_FirstColor        = 0;
   m_LastColor         = 0;
   m_nCurrentIndexUp   = 0;
   m_nCurrentIndexDown =-1;
   m_pColors           = NULL;
}

CColorView::~CColorView()
{
   if (m_pColors) delete m_pColors;
}

bool CColorView::Attach(CColorArray *pCol)
{
   ASSERT(m_pColors == NULL);
   if (m_pColors == NULL)
   {
      m_pColors = pCol;
      return true;
   }
   return false;
}

CColorArray * CColorView::Detach()
{
   ASSERT(m_pColors != NULL);
   CColorArray *pTemp = m_pColors;
   m_pColors = NULL;
   return pTemp;
}


void CColorView::OnDestroy()
{
   CStatic::OnDestroy();
}

BEGIN_MESSAGE_MAP(CColorView, CStatic)
	//{{AFX_MSG_MAP(CColorView)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_DESTROY()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CColorView 
BOOL CColorView::OnEraseBkgnd(CDC* pDC) 
{
   return true;
}

void CColorView::OnPaint()
{
   CPaintDC    dc(this);
   if (GetNumColor() > 0)
   {
      CRect rect;
      GetClientRect(&rect);
      int nTemp = rect.top;
                  rect.top = rect.bottom;
                             rect.bottom = nTemp;
      dc.SaveDC();
      m_pColors->Draw(&dc, &rect);
      dc.RestoreDC(-1);
   }

}

bool CColorView::SetArraySize(int nS)
{
   ASSERT(m_pColors!=NULL);
   if (m_pColors) return m_pColors->SetArraySize(nS);
   return false;
}

void CColorView::operator = (CColorView& pCopy)
{
   ASSERT(pCopy.m_pColors!=NULL);
   if (pCopy.m_pColors)
   {
      if (m_pColors == NULL) m_pColors = new CColorArray;
      *m_pColors = *(pCopy.m_pColors);
   }
}

void CColorView::operator = (CColorArray &pCopy)
{
   if (m_pColors == NULL) m_pColors = new CColorArray;
   *m_pColors = pCopy;
}

void CColorView::operator = (CLevelColor &pCopy)
{
   if (m_pColors == NULL) m_pColors = new CColorArray;
   m_pColors->SetArraySize(pCopy.GetNumColor());
   m_pColors->SetColors((COLORREF*)pCopy.GetColorBuffer(), pCopy.GetNumColor());
   m_pColors->SetColorMode(pCopy.GetColorMode());
}

bool CColorView::SetColors(COLORREF *pColors, int nColors)
{
   ASSERT(m_pColors!=NULL);
   if (m_pColors) return m_pColors->SetColors(pColors, nColors);
   return false;
}

void CColorView::SetColorMode(UINT nMode)
{
   ASSERT(m_pColors!=NULL);
   if (m_pColors) m_pColors->SetColorMode(nMode);
}

/******************************************************************************
* Name       : SetColor                                                       *
* Zweck      : Setzen einer oder mehrerer Farben des Farbdialogfeldes mit     *
*              automatischem Update des Dialogfeldes.                         *
* Definition : void SetColor(COLORREF, int);                                  *
* Aufruf     : SetColor(color, nIndex);                                       *
* Parameter  :                                                                *
* color  (E) : Farbwert, der gesetzt werden soll.                             *
* nIndex (E) : Index des Farbfeldes, wenn eine einzelne Farbe gesetzt werden  *
*              soll. Oder setzen des:                                         *
*         -1 : Farbfeldes an der Position m_nCurrentIndexUp.                  *
*         -2 : ersten (letzten) Farbfeldes mit Berechnung der Zwischenfarben  *
*              zum letzten (ersten) Farbfeld, abhängig von m_nCurrentIndexUp  *
*              in der ersten Hälfte der Dialogfeldes oder der (zweiten Hälfte)*
*         -3 : ersten Farbfeldes mit Berechnung der Zwischenfarben zum letzen *
*              Farbfeld.                                                      *
*         -4 : letzten Farbfeldes mit Berechnung der Zwischenfarben zur ersten*
*              Farbfeld.                                                      *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CColorView::SetColor(COLORREF color, int nIndex)
{
   ASSERT(m_pColors!=NULL);
   int nColors = GetNumColor();
   if      (m_pColors->GetColorMode() & CV_PROTECTED) return;
   else if ((nIndex >= -1) && (m_nCurrentIndexDown == -1))     // Einzelfarben von Außen
   {
      if (nIndex == -1) nIndex = m_nCurrentIndexUp;            // Index ist der aktuelle
      TRACE("Current Index : %d\n", nIndex);
      if ((nIndex >=  0) && (nIndex < nColors))                // oder der von Außen übergebene
      {
         m_pColors->SetColor(nIndex) = color;
         InvalidateRect(NULL);
      }
   }
   else                                                        // Setzen von mehreren Farben
   {
      int nFirstColor=0, nLastColor = nColors-1;
      if (m_nCurrentIndexDown != -1)                           // Farben zwischen zwei Indizes innerhalb des ColorViews setzen
      {
         if (m_nCurrentIndexDown < m_nCurrentIndexUp)          // Index muß steigen
         {
            nFirstColor  = m_nCurrentIndexDown;
            nLastColor   = m_nCurrentIndexUp;
         }
         else                                                  // deshalb tauschen
         {
            nFirstColor  = m_nCurrentIndexUp;
            nLastColor   = m_nCurrentIndexDown;
         }
         m_FirstColor = m_pColors->GetColor(nFirstColor);      // Farben für die Interpolation zuweisen
         m_LastColor  = m_pColors->GetColor(nLastColor);
      }
      else if (nIndex == -2)                                   // Farben vom ersten bis zum letzten Index setzen
      {                                                        // über die rechte Maustaste in diesen Colorview gezogen
         if (m_nCurrentIndexUp < GetNumColor()>>1) m_FirstColor = color;
         else                                  m_LastColor  = color;
      }
      else if (nIndex == -3) m_FirstColor = color;             // über Shift + linke Maustaste in einem anderen ColorView gedrückt
      else if (nIndex == -4) m_LastColor  = color;             // über Shift + rechte Maustaste in einem anderen ColorView gedrückt

      if (nFirstColor < nLastColor)
      {
         if (nFirstColor <  0      ) nFirstColor = 0;          // Bereichsübertretung prüfen
         if (nLastColor  >= nColors) nLastColor  = nColors-1;

         if (m_pColors->GetColorMode() & CV_INTERPOLATE)       // linear Interpolieren zwischen zwei Farbwerten
            m_pColors->InterpolateColors(nFirstColor, nLastColor, m_FirstColor, m_LastColor);
         else                                                  // Farbwerte aus dem FarbIndex bestimmen
            m_pColors->SetIndexColors(nFirstColor, nLastColor, m_FirstColor, m_LastColor);
         InvalidateRect(NULL);
      }
   }
   m_pColors->SetPaletteColors();
   m_nCurrentIndexDown = -1;
}

/******************************************************************************
* Name       : SetCurrentFromPoint                                            *
* Zweck      : Setzen des Farbfeldindex zur späteren Auswertung und/oder      *
*              Ermittlung der Farbfeldindex an der übergebenen Position.      *
* Definition : int SetCurrentFromPoint(CPoint, int);                          *
* Aufruf     : SetCurrentFromPoint(point, nCurrent);                          *
* Parameter  :                                                                *
* point   (E): Position des Cursors.                                 (CPoint) *
* nCurrent(E): Speicherung des Index (0, CURRENT_UP, CURRENT_DOWN)   (int)    *
*  0            : keine Speicherung                                           *
*  CURRENT_UP   : als Index für Mausbutton-Up                                 *
*  CURRENT_DOWN : als Index für Mausbutton-Down                               *
* Funktionswert : Index des Farbfeldes                               (int)    *
******************************************************************************/
int CColorView::SetCurrentFromPoint(CPoint point, int nCurrent)
{
   ASSERT(m_pColors!=NULL);
   int nCol = GetNumColor();
   if (nCol>0)
   {
      CRect rcClient;
      GetClientRect(&rcClient);
      double dInvWidth = (double)nCol / (double)(rcClient.right - rcClient.left);
      int    nIndex    = (int)(dInvWidth * (double)point.x);
      if      (nCurrent == CURRENT_UP  ) m_nCurrentIndexUp   = nIndex;
      else if (nCurrent == CURRENT_DOWN) m_nCurrentIndexDown = nIndex;
      return nIndex;
   }
   return -1;
}

int CColorView::GetNumColor()
{
   if (m_pColors) return m_pColors->GetNumColor();
   return 0;
}

/******************************************************************************
* Name       : GetColor                                                       *
* Zweck      : Ermittlung des Farbwertes in einem Farbfeld                    *
* Definition : COLORREF GetColor(int);                                        *
* Aufruf     : GetColor(nIndex);                                              *
* Parameter  :                                                                *
* nIndex (E) : Index des Farbfeldes bzw. :                              (int) *
*  LAST_COLOR   : Farbe des letzten Farbfeldes                                *
*  CURRENT_DOWN : Farbe des Farbfeldes[m_nCurrentIndexDown]                   *
*  CURRENT_UP   : Farbe des Farbfeldes[m_nCurrentIndexUp]                     *
* Funktionswert : Farbwert, bei Fehler -1                          (COLORREF) *
******************************************************************************/
COLORREF CColorView::GetColor(int nIndex)
{
   ASSERT(m_pColors!=NULL);
   if (nIndex == LAST_COLOR  ) nIndex = GetNumColor()-1;
   if (nIndex == CURRENT_DOWN) nIndex = m_nCurrentIndexDown;
   if (nIndex == CURRENT_UP  ) nIndex = m_nCurrentIndexUp;
   return m_pColors->GetColor(nIndex);   
}

/******************************************************************************
* Name       : CreateColorArray                                               *
* Zweck      : Erzeugen eines Farbarrays mit den Farben des Farbkreises       *
* Definition : bool CreateColorArray();                                       *
* Aufruf     : CreateColorArray();                                            *
* Parameter  : keine                                                          *
* Funktionswert : (true, false)                                      (bool)   *
******************************************************************************/
bool CColorView::CreateColorArray()
{
   ASSERT(m_pColors!=NULL);
   CRect       rcClient;
   GetClientRect(&rcClient);
   if (SetArraySize(rcClient.right))
   {
      int i = 0, nColors = GetNumColor();
      m_pColors->SetColor(i++) = RGB(255,255,255);               // Weiß
      m_pColors->SetColor(i++) = RGB(255,255,255);               // Weiß
      double dfactor = 1553.0 / (double)nColors;
      nColors -= 2;
      for (; i<nColors; i++)
      {
         m_pColors->SetColor(i) = GetColorFromIndex((int)(i*dfactor), 1.0);
      }
      m_pColors->SetColor(i++) = 0;
      m_pColors->SetColor(i++) = 0;
      m_pColors->SetPaletteColors();
      return true;
   }
   return false;
}

// Mouse-Messages
void CColorView::OnLButtonDown(UINT nFlags, CPoint point)
{
   SetCurrentFromPoint(point, CURRENT_DOWN);
   TRACE("OnLButtonDown %d, iDown: %d, iUp: %d", this, m_nCurrentIndexDown, m_nCurrentIndexUp);
   GetParent()->SendMessage(WM_COLORVIEW_MOUSE_EVENT,
                            MAKELONG((WORD)nFlags, (WORD)CV_LBUTTONDOWN),
                            (LPARAM)this);
   TRACE(", After: iDown: %d, iUp: %d\n", m_nCurrentIndexDown, m_nCurrentIndexUp);
   CStatic::OnLButtonDown(nFlags, point);
}
void CColorView::OnLButtonUp(UINT nFlags, CPoint point) 
{
   SetCurrentFromPoint(point, CURRENT_UP);
   TRACE("OnLButtonUp %d, iDown: %d, iUp: %d", this, m_nCurrentIndexDown, m_nCurrentIndexUp);
   GetParent()->SendMessage(WM_COLORVIEW_MOUSE_EVENT,
                            MAKELONG((WORD)nFlags, (WORD)CV_LBUTTONUP),
                            (LPARAM)this);
   TRACE(", After: iDown: %d, iUp: %d\n", m_nCurrentIndexDown, m_nCurrentIndexUp);
   CStatic::OnLButtonUp(nFlags, point);
}
void CColorView::OnRButtonDown(UINT nFlags, CPoint point) 
{
   SetCurrentFromPoint(point, CURRENT_DOWN);
   GetParent()->SendMessage(WM_COLORVIEW_MOUSE_EVENT,
                            MAKELONG((WORD)nFlags, (WORD)CV_RBUTTONDOWN),
                            (LPARAM)this);
   CStatic::OnRButtonDown(nFlags, point);
}
void CColorView::OnRButtonUp(UINT nFlags, CPoint point) 
{
   SetCurrentFromPoint(point, CURRENT_UP);
   GetParent()->SendMessage(WM_COLORVIEW_MOUSE_EVENT,
                            MAKELONG((WORD)nFlags, (WORD)CV_RBUTTONUP),
                            (LPARAM)this);
   CStatic::OnRButtonUp(nFlags, point);
}
void CColorView::OnMouseMove(UINT nFlags, CPoint point) 
{
   if ((nFlags & MK_SHIFT) && (nFlags & (MK_LBUTTON|MK_RBUTTON)))
      SetCurrentFromPoint(point, CURRENT_DOWN);

   GetParent()->SendMessage(WM_COLORVIEW_MOUSE_EVENT,
                            MAKELONG((WORD)nFlags, (WORD)CV_MOUSEMOVE),
                            (LPARAM)this);
   CStatic::OnMouseMove(nFlags, point);
}

int CColorView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;
   CRect rcClient;
   GetClientRect(&rcClient);
   if (m_pColors == NULL) m_pColors = new CColorArray;
	return 0;
}

CColorArray & CColorView::GetColorArray()
{
   ASSERT(m_pColors!=NULL);
   return *m_pColors;
}

bool CColorView::IsChanged()
{
   if (m_pColors) return m_pColors->IsChanged();
   return false;
}
