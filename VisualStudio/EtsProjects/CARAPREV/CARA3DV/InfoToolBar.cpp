// InfoToolBar.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "CARA3DV.h"
#include "InfoToolBar.h"
#include "..\ColorArray.h"
#include "..\resource.h"

#ifdef ETSDEBUG_REPORT
 #define  ETSDEBUG_INCLUDE 
#endif
#include "CEtsDebug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorBar
extern "C" __declspec(dllimport) HINSTANCE g_hCaraPreviewInstance;

#define MIN_COLORVIEWSIZE  6
#define DOCKED_EXTEND      8
#define GRIPBAR_SIZE       10
#define UPDATE_FLOATING    0x00000001
#define SCALE_CHANGED      0x00000002
#define SCALESIZE_CHANGED  0x00000004
#define CALC_NEW_LAYOUT    0x00000010
#define BAR_IS_FLOATING    0x00000020
#define IS_VISIBLE_BY_USER 0x00000040
#define TOOLBAR_HTGRIPPER  0x00001000

CColorBar::CColorBar()
{
   m_szFloating.cx =  0;
   m_szFloating.cy =  0;
   m_szScale.cx    = 50;
   m_szScale.cy    = 14;
   m_dwStates      = UPDATE_FLOATING;
   SetButtonSize(CSize(24, 23));
   SetMinMaxLevel(0.0, 1.0);
   m_wndScale.m_ScalePainter.m_Div.stepwidth = 30;
   m_wndScale.m_ScalePainter.m_bHorz         = true;
}

CColorBar::~CColorBar()
{
}


BEGIN_MESSAGE_MAP(CColorBar, CControlBar)
	//{{AFX_MSG_MAP(CColorBar)
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SYSCOLORCHANGE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


bool CColorBar::AttachColorArray(CColorArray *pCA)
{
   CColorArray * pOld = m_wndColors.Detach();
   if (pOld) delete pOld;
   return m_wndColors.Attach(pCA);
}

CColorArray * CColorBar::DetachColorArray()
{
   return m_wndColors.Detach();
}

void CColorBar::SetMinMaxLevel(double dMin, double dMax)
{
   m_wndScale.m_ScalePainter.m_Div.f1 = dMin;
   m_wndScale.m_ScalePainter.m_Div.f2 = dMax;
   m_wndScale.m_bValidSize = false;
   m_dwStates |= SCALE_CHANGED;
   m_dwStates |= CALC_NEW_LAYOUT;
}

void CColorBar::SetUnit(char *pszText)
{
   m_wndScale.m_ScalePainter.SetUnit(pszText);
   m_wndScale.m_bValidSize = false;
   m_dwStates |= SCALE_CHANGED;
   m_dwStates |= CALC_NEW_LAYOUT;
}

void CColorBar::SetNumRepresentation(int nMode, int nRound)
{
   m_wndScale.m_ScalePainter.m_Ntx.nmode  = nMode;
   m_wndScale.m_ScalePainter.m_Ntx.nround = nRound;
   m_wndScale.m_bValidSize = false;
   m_dwStates |= SCALE_CHANGED;
   m_dwStates |= CALC_NEW_LAYOUT;
}

void CColorBar::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
   UNUSED(lpDIS);
}
void CColorBar::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
   UNUSED(lpMIS);
}

int CColorBar::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
   DWORD *pdwStates = (unsigned long *)&m_dwStates;
   if (( m_wndScale.m_ScalePainter.m_bHorz && (point.x > 10) ||
        !m_wndScale.m_ScalePainter.m_bHorz && (point.y > 10)))
   {
      *pdwStates &= ~TOOLBAR_HTGRIPPER;
      if (pTI)
      {
         pTI->uId    = IDR_CARA_COLOR;
         pTI->uFlags = TTF_CENTERTIP|TTF_ALWAYSTIP;
         return pTI->uId;                          // Tooltip
      }
      else     return IDR_CARA_COLOR+HID_BASE_COMMAND;// Help
   }
   else
   {
      *pdwStates |= TOOLBAR_HTGRIPPER;
   }
   return CWnd::OnToolHitTest(point, pTI);
}

BOOL CColorBar::Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID)
{
	return CreateEx(pParentWnd, 0, dwStyle, CRect(m_cxLeftBorder, m_cyTopBorder, m_cxRightBorder, m_cyBottomBorder), nID);
}

BOOL CColorBar::CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle, DWORD dwStyle, CRect rcBorders, UINT nID)
{
	ASSERT_VALID(pParentWnd);   // must have a parent
	ASSERT (!((dwStyle & CBRS_SIZE_FIXED) && (dwStyle & CBRS_SIZE_DYNAMIC)));

	SetBorders(rcBorders);

   // save the style
	m_dwStyle = (dwStyle & CBRS_ALL);
	if (nID == AFX_IDW_TOOLBAR)
		m_dwStyle |= CBRS_HIDE_INPLACE;

	dwStyle &= ~CBRS_ALL;
	dwStyle |= CCS_NOPARENTALIGN|CCS_NOMOVEY|CCS_NODIVIDER|CCS_NORESIZE;
	dwStyle |= dwCtrlStyle;

   InitSysColors();
   // create the HWND
	CRect rect; rect.SetRectEmpty();
	if (!CWnd::Create(TOOLBARCLASSNAME, NULL, dwStyle, rect, pParentWnd, nID))
		return FALSE;

   if (!m_wndColors.Create(NULL, WS_CHILD|WS_VISIBLE, rect, this))
      return false;

   if (!m_wndScale.Create(NULL, WS_CHILD|WS_VISIBLE, rect, this))
      return false;
   m_szScale = m_wndScale.GetMinSize();
   return TRUE;
}

void CColorBar::SetButtonSize(CSize &szButton)
{
   m_szButton = szButton;
   m_szMinSize.cx = max(szButton.cx, m_szScale.cx + MIN_COLORVIEWSIZE) + m_cxLeftBorder + m_cxRightBorder;
   m_szMinSize.cy = max(szButton.cy, m_szScale.cy + MIN_COLORVIEWSIZE) + m_cyTopBorder  + m_cyBottomBorder;
}

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CColorBar 

CSize CColorBar::CalcDynamicLayout(int nLength, DWORD nMode)
{
//   BEGIN("CColorBar::CalcDynamicLayout");

   CSize szReturn(m_szFloating);
/*
   REPORT("Length : %d, %s|%s|%s|%s|%s|%s|%s|", nLength,       // Höhe bzw. Breite
          (nMode&LM_STRETCH)  ? "STRETCH"  : " ",
          (nMode&LM_HORZ)     ? "HORZ"     : " ",              // Horizontal
          (nMode&LM_MRUWIDTH) ? "MRUWIDTH" : " ",              // Setzen der gespeicherten Breite und Länge
          (nMode&LM_HORZDOCK) ? "HORZDOCK" : " ",              // horizontal gedockt
          (nMode&LM_VERTDOCK) ? "VERTDOCK" : " ",              // vertikal gedockt
          (nMode&LM_LENGTHY)  ? "LENGTHY"  : " ",              // Vertikale länge
          (nMode&LM_COMMIT)   ? "COMMIT"   : " ");             // Übernehmen der eingestellten Länge bzw. Breite
*/

   if (nMode & LM_COMMIT) szReturn = CommitFloating();         // Übernahme der Fensterpositionen Floating
   else if (nLength > 0)                                       // Größe beim Ändern der Größe überprüfen
   {
      szReturn = CalcFloatingLayout(nLength, (nMode & LM_LENGTHY) ? false : true);
   }
   else if (nLength == 0)
   {
      if (nMode&LM_MRUWIDTH)                                   // Floating Größe überprüfen
      {
         CalcFloatingLayout(m_szFloating.cx, true);            // Horizontal
         CalcFloatingLayout(m_szFloating.cy, false);           // Vertikal
         szReturn = m_szFloating;
      }
      else if (nMode&(LM_HORZDOCK|LM_VERTDOCK))                // Größe beim Docken überprüfen
      {
         szReturn = CalcFixedLayout((nMode&LM_STRETCH)?true:false, (nMode&LM_HORZ)?true:false);
      }
   }
   else if (nLength == -1)
   {
      if (nMode&LM_MRUWIDTH)                                   // Übernahme der Fensterpositionen Floating
      {
         szReturn = CommitFloating();
         m_dwStates |= BAR_IS_FLOATING;
      }
      else if (nMode&(LM_HORZDOCK|LM_VERTDOCK))                // Übernahme der Fensterpositionen gedockt
      {
         szReturn = CommitDocked((nMode&LM_HORZDOCK) ? true : false);
         m_dwStates &= ~BAR_IS_FLOATING;
      }
   }

   return szReturn;
}

CSize CColorBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
//   BEGIN("CColorBar::CalcFixedLayout");
   CSize szWnd(0,0);
   if (bStretch)
   {
      CWnd *pWnd = GetParent();
      if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CDockBar)))
      {
         CRect     rcWnd, rcBar, rcThis;
         CPoint    ptCenter;
         CDockBar *pDock = (CDockBar*)pWnd;
         int       i, nRemove = -1, nCount = pDock->GetDockedVisibleCount();
         pWnd->GetWindowRect(&rcWnd);
         GetWindowRect(&rcThis);
         szWnd = rcWnd.Size()+CSize(4,4);
         ptCenter = rcThis.CenterPoint();
         if (nCount > 1)
         {
            nCount = pDock->m_arrBars.GetSize();
            for (i=0; i<nCount; i++)
            {
               if ((pDock->m_arrBars[i]) && (pDock->m_arrBars[i] != (void*) this))
               {
                  CWnd *pBar = (CWnd*) pDock->m_arrBars[i];
                  if (((DWORD)pBar >= (DWORD)0x00400000))      // Ist es eine Adresse ?
                  {
                     ASSERT(::IsWindow(pBar->m_hWnd));
                     pBar->GetWindowRect(&rcBar);
                     if (bHorz && (ptCenter.y > rcBar.top) && (ptCenter.y < rcBar.bottom))
                     {
                        szWnd.cx -= rcBar.Width();
                     }
                     else if ((ptCenter.x > rcBar.left) && (ptCenter.x < rcBar.right))
                     {
                        szWnd.cy -= rcBar.Height();
                     }
                  }
                  else                                         // Index merken
                  {
                     nRemove = i;
                  }
               }
            }
            if (nRemove > -1)                                  // Löschen
            {
               pDock->m_arrBars.RemoveAt(nRemove, 1);          // vielleicht doch nicht ?
            }
         }
      }
   }
   if (bHorz) return CSize(max(szWnd.cx, m_szButton.cx * DOCKED_EXTEND), m_szMinSize.cy);
   else       return CSize(m_szMinSize.cx, max(szWnd.cy, m_szButton.cy * DOCKED_EXTEND));
}

CSize CColorBar::CommitDocked(bool bHorz)
{
//   BEGIN("CColorBar::CommitDocked");
   CRect rcColor, rcScale;
   CSize szDocked;
   InitSize();
   m_wndScale.m_ScalePainter.m_bHorz = bHorz;
   if (bHorz)
   {
      szDocked  = CalcFixedLayout(true, bHorz);
      int nLeft = m_cxLeftBorder;
      m_cxLeftBorder = GRIPBAR_SIZE;
      CSize szView   = szDocked-CSize(m_cxRightBorder+m_cxLeftBorder, m_cyTopBorder + m_cyBottomBorder);
      CalcHorzWindowPos(rcColor, rcScale, szView);
      m_cxLeftBorder = nLeft;
   }
   else
   {
      szDocked = CalcFixedLayout(true, bHorz);
      int nTop = m_cyTopBorder;
      m_cyTopBorder  = GRIPBAR_SIZE;
      CSize szView   = szDocked-CSize(m_cxRightBorder+m_cxLeftBorder, m_cyTopBorder + m_cyBottomBorder);
      CalcVertWindowPos(rcColor, rcScale, szView);
      m_cyTopBorder  = nTop;
   }

   CommitWindowPositions(rcColor, rcScale);
   m_dwStates |= UPDATE_FLOATING;
   m_dwStates &= ~CALC_NEW_LAYOUT;
   return szDocked;
}

CSize CColorBar::CalcFloatingLayout(int nLength, bool bHorz)
{
   if (bHorz) m_szFloating.cx = max(m_szMinSize.cx, nLength);    // horizontal
   else       m_szFloating.cy = max(m_szMinSize.cy, nLength);    // vertikal
   m_nMRUWidth = MAKELONG(m_szFloating.cx, m_szFloating.cy);
   m_dwStates |= UPDATE_FLOATING;
   return m_szFloating;
}

CSize CColorBar::CommitFloating()
{
//   BEGIN("CColorBar::CommitFloating");
   if (m_dwStates & UPDATE_FLOATING)
   {
      CRect rcColor, rcScale;
      InitSize();
      CSize szView  = m_szFloating - CSize(m_cxRightBorder+m_cxLeftBorder, m_cyTopBorder + m_cyBottomBorder);

      CSize szColor = CSize(szView.cx-m_szScale.cx, szView.cy-m_szScale.cy);
      m_wndScale.m_ScalePainter.m_bHorz = (szColor.cx > szColor.cy) ? true : false;

      if (m_wndScale.m_ScalePainter.m_bHorz)
         CalcHorzWindowPos(rcColor, rcScale, szView);
      else                    
         CalcVertWindowPos(rcColor, rcScale, szView);

      CommitWindowPositions(rcColor, rcScale);
      m_dwStates &= ~UPDATE_FLOATING;
   }
   return m_szFloating;
}

void  CColorBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
   if (m_wndColors.IsChanged() && m_wndColors.m_hWnd)
   {
      ASSERT(::IsWindow(m_wndColors.m_hWnd));
      TRACE("CColorBar::IsChanged\n");
      m_wndColors.InvalidateRect(NULL);
   }
   if (m_wndScale.m_hWnd)
   {
      ASSERT(::IsWindow(m_wndScale.m_hWnd));
      if (m_dwStates & SCALE_CHANGED)
      {
         TRACE("CColorBar::SCALE_CHANGED\n");
         m_wndScale.InvalidateRect(NULL);
         m_dwStates &= ~SCALE_CHANGED;
         m_dwStates |= CALC_NEW_LAYOUT;
      }
      if (!m_wndScale.m_bValidSize)
      {
         TRACE("CColorBar::SCALESIZE_CHANGED\n");
         m_szScale = m_wndScale.GetMinSize();
         SetButtonSize(m_szButton);
      }
   }
   if (m_dwStates & CALC_NEW_LAYOUT)
   {
      TRACE("CColorBar::CALC_NEW_LAYOUT\n");
      if (m_dwStyle & CBRS_FLOATING)
      {
         CalcFloatingLayout(m_szFloating.cx, true);            // Horizontal
         CalcFloatingLayout(m_szFloating.cy, false);           // Vertikal
         CommitFloating();
      }
      else
      {
         CommitDocked((m_dwStyle & CBRS_ORIENT_HORZ) ? true : false);
      }
	   CFrameWnd* pFrameWnd = GetParentFrame();
	   ASSERT(pFrameWnd != NULL);
	   pFrameWnd->RecalcLayout();
      m_dwStates &= ~CALC_NEW_LAYOUT;
   }
}


void CColorBar::InitSize()
{
   if ((m_szFloating.cx == 0) && (m_szFloating.cy == 0))
   {
      if (m_nMRUWidth == 32767)
      {
         m_szFloating.cx = m_szButton.cx * DOCKED_EXTEND;
         m_szFloating.cy = m_szMinSize.cy;
      }
      else
      {
         m_szFloating.cx = LOWORD(m_nMRUWidth);
         m_szFloating.cy = HIWORD(m_nMRUWidth);
      }
   }
}

void CColorBar::DoPaint(CDC* pDC)
{
   ASSERT_KINDOF(CPaintDC, pDC);
   CPaintDC *pPaint = (CPaintDC *)pDC;
   CBrush brush;
   brush.CreateSysColorBrush(COLOR_BTNFACE);
   pDC->FillRect(&pPaint->m_ps.rcPaint, &brush);
   CControlBar::DoPaint(pDC);
}

void CColorBar::CalcHorzWindowPos(CRect &rcColor, CRect &rcScale, CSize &szView)
{
   rcColor.left   = rcScale.left    = m_cxLeftBorder;
   rcColor.right  = rcScale.right   = rcColor.left   + szView.cx;
   rcColor.top                      = m_cyTopBorder;
   rcScale.bottom                   = rcColor.top    + szView.cy;
   rcScale.top    = rcColor.bottom  = rcScale.bottom - m_szScale.cy;
   m_wndScale.m_ScalePainter.m_Div.stepwidth = MulDiv(m_szScale.cx, 3, 2);
}

void CColorBar::CalcVertWindowPos(CRect &rcColor, CRect &rcScale, CSize &szView)
{
   rcColor.top    = rcScale.top     = m_cyTopBorder;
   rcColor.bottom = rcScale.bottom  = rcColor.top   + szView.cy;
   rcColor.left                     = m_cxLeftBorder;
   rcScale.right                    = rcColor.left  + szView.cx;
   rcScale.left   = rcColor.right   = rcScale.right - m_szScale.cx;
   m_wndScale.m_ScalePainter.m_Div.stepwidth       = MulDiv(m_szScale.cy, 3, 2);
}

void CColorBar::CommitWindowPositions(CRect &rcColor, CRect &rcScale)
{
   m_wndColors.MoveWindow(&rcColor, false);
   m_wndColors.InvalidateRect(NULL);
   m_wndScale.MoveWindow(&rcScale, false);
   m_wndScale.InvalidateRect(NULL);
   InvalidateRect(NULL);
}

BOOL CColorBar::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
   if (m_dwStates & TOOLBAR_HTGRIPPER)
   {
      SetCursor(LoadCursor(g_hCaraPreviewInstance, MAKEINTRESOURCE(IDC_OPENHANDCURSOR)));
      return true;
   }
   return CControlBar::OnSetCursor(pWnd, nHitTest, message);
}

bool CColorBar::IsVisiblebyUserCmd()
{
   return ((m_dwStates & IS_VISIBLE_BY_USER) != 0) ? true : false;
}

void CColorBar::VisiblebyUser(bool bVisible)
{
   if (bVisible) m_dwStates |=  IS_VISIBLE_BY_USER;
   else          m_dwStates &= ~IS_VISIBLE_BY_USER;
}


void CColorBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
   if (m_dwStates & TOOLBAR_HTGRIPPER)
      SetCursor(LoadCursor(g_hCaraPreviewInstance, MAKEINTRESOURCE(IDC_TRACKCURSOR)));

   CControlBar::OnLButtonDown(nFlags, point);

   if (m_dwStates & TOOLBAR_HTGRIPPER)
      SetCursor(LoadCursor(g_hCaraPreviewInstance, MAKEINTRESOURCE(IDC_OPENHANDCURSOR)));
}

void CColorBar::SetFloating(bool bFloating)
{
   if (bFloating) m_dwStates |=  BAR_IS_FLOATING;
   else           m_dwStates &= ~BAR_IS_FLOATING;
}

bool CColorBar::GetFloating()
{
   return ((m_dwStates & BAR_IS_FLOATING) != 0) ? true : false;
}

void CColorBar::OnSysColorChange() 
{
	CControlBar::OnSysColorChange();
   InitSysColors();
}

void CColorBar::InitSysColors()
{
   NONCLIENTMETRICS ncm;
   ncm.cbSize = sizeof(NONCLIENTMETRICS);
   ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
   m_wndScale.m_ScalePainter.m_LogFont   = ncm.lfStatusFont;
   m_wndScale.m_ScalePainter.m_TextColor = ::GetSysColor(COLOR_BTNTEXT);
}
