// CaraToolbar.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "CaraToolbar.h"
#include "resource.h"
#include "CaraWinApp.h"
#include "CARAPREV.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define  TOOLBAR_REPEAT_TIMER 815
/////////////////////////////////////////////////////////////////////////////
// CCaraToolbar
bool CCaraToolbar::gm_bUserCmd = true;

CCaraToolbar::CCaraToolbar()
{
   m_nMessageTime = 0;
   m_tbbPressed.idCommand = 0;
   m_nFlags       = TOOLBAR_REPEAT_ALL|TOOLBAR_USERCMD_VISIBLE;
}

CCaraToolbar::~CCaraToolbar()
{
}


BEGIN_MESSAGE_MAP(CCaraToolbar, CToolBar)
	//{{AFX_MSG_MAP(CCaraToolbar)
   ON_WM_NCHITTEST()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CCaraToolbar 
CBitmap *CCaraToolbar::GetButtonBitmap(int nID, CSize szBmp)
{
   CBitmap *pBitmap=NULL;
   if (m_hbmImageWell)
   {
      TBBUTTON tbb;
      int i = CommandToIndex(nID);
      if (i != -1)
      {
         DefWindowProc(TB_GETBUTTON, i, (LPARAM)&tbb);
         HDC hDC = ::GetDC(NULL);
         HDC hMem1 = CreateCompatibleDC(hDC);
         HDC hMem2 = CreateCompatibleDC(hDC);
         HBITMAP hOld1 = (HBITMAP) ::SelectObject(hMem1, m_hbmImageWell);
         pBitmap = new CBitmap;
         pBitmap->Attach(::CreateCompatibleBitmap(hDC, szBmp.cx, szBmp.cy));
         HBITMAP hOld2 = (HBITMAP) ::SelectObject(hMem2, pBitmap->m_hObject);
         if ((m_sizeImage.cx < szBmp.cx) || (m_sizeImage.cy < szBmp.cy))
         {
            ::StretchBlt(hMem2,                          0, 0,       szBmp.cx,       szBmp.cy, 
                         hMem1, tbb.iBitmap*m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, SRCCOPY);
         }
         else
         {
            ::BitBlt(hMem2,                          0, 0, m_sizeImage.cx, m_sizeImage.cy,
                     hMem1, tbb.iBitmap*m_sizeImage.cx, 0, SRCCOPY);
         }
         ::SelectObject(hMem1, hOld1);
         ::SelectObject(hMem2, hOld2);
         ::DeleteDC(hMem1);
         ::DeleteDC(hMem2);
         ::ReleaseDC(NULL, hDC);
      }
   }

   return pBitmap;
}

void CCaraToolbar::OnTimer(UINT nIDEvent) 
{
   if (nIDEvent == TOOLBAR_REPEAT_TIMER)
   {
      TBBUTTON tbb;
      int i, nCount = DefWindowProc(TB_BUTTONCOUNT, 0, 0);
      for (i = 0; i < nCount; i++)
      {
         DefWindowProc(TB_GETBUTTON, i, (LPARAM)&tbb);
         if ((MAKELONG(tbb.fsStyle,tbb.fsState)& TBBS_PRESSED)!=0)
            break;
      }
      if (i<nCount) 
      {
         m_nTimerCount++;
         if (m_nTimerCount > 1)
            GetParent()->SendMessage(WM_COMMAND, tbb.idCommand);
         if (m_nTimerCount == 10)
         {
            KillTimer(TOOLBAR_REPEAT_TIMER);
            SetTimer(TOOLBAR_REPEAT_TIMER, m_nMessageTime>>2, NULL);
         }
         else if (m_nTimerCount == 30)
         {
            KillTimer(TOOLBAR_REPEAT_TIMER);
            SetTimer(TOOLBAR_REPEAT_TIMER, m_nMessageTime>>4, NULL);
         }
      }
      else
      {
         KillTimer(TOOLBAR_REPEAT_TIMER);
         m_nTimerCount = 0;
      }
   }
   else
   {
      CToolBar::OnTimer(nIDEvent);
   }   
}

void CCaraToolbar::SetRepeatTime(int nTime)
{
   KillTimer(TOOLBAR_REPEAT_TIMER);
   m_nMessageTime = nTime;
}

void CCaraToolbar::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
   OnLButtonDown(nFlags, point);
}

void CCaraToolbar::OnLButtonDown(UINT nFlags, CPoint point) 
{
   if (m_nFlags & TOOLBAR_HTGRIPPER)
      SetCursor(LoadCursor(g_hCaraPreviewInstance, MAKEINTRESOURCE(IDC_TRACKCURSOR)));

   CToolBar::OnLButtonDown(nFlags, point);
   if (m_nMessageTime>0)
   {
      if (m_nFlags & TOOLBAR_REPEAT_ALL)
      {
         SetTimer(TOOLBAR_REPEAT_TIMER, m_nMessageTime, NULL);
         m_nTimerCount = 0;
      }
      else if (m_nFlags & TOOLBAR_REPEAT_SOME)
      {
         long lTimer = GetWindowLong(m_hWnd, GWL_USERDATA);
         TBBUTTON tbb;
         int      i, nCount = DefWindowProc(TB_BUTTONCOUNT, 0, 0);
         for (i = 0; i < nCount; i++)
         {
            if (((lTimer & (1 << i)) != 0) &&                  // Timerbutton
                DefWindowProc(TB_GETBUTTON, i, (LPARAM)&tbb)&& // Info holen
                !(tbb.fsStyle&TBSTYLE_SEP) &&                  // kein Separator
                 ((MAKELONG(tbb.fsStyle,tbb.fsState)& TBBS_PRESSED)!=0))// gedrückt

            {
               SetTimer(TOOLBAR_REPEAT_TIMER, m_nMessageTime, NULL);
               m_nTimerCount = 0;
            }
         }
      }
   }
   if (m_nFlags & TOOLBAR_HTGRIPPER)
      SetCursor(LoadCursor(g_hCaraPreviewInstance, MAKEINTRESOURCE(IDC_OPENHANDCURSOR)));
}
void CCaraToolbar::OnLButtonUp(UINT nFlags, CPoint point) 
{
   CToolBar::OnLButtonUp(nFlags, point);
   if (m_nMessageTime>0)
   {
      KillTimer(TOOLBAR_REPEAT_TIMER);
      m_nTimerCount = 0;
   }
}

void CCaraToolbar::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
   OnRButtonDown(nFlags, point);
}

void CCaraToolbar::OnRButtonDown(UINT nFlags, CPoint point) 
{
   CToolBar::OnRButtonDown(nFlags, point);
   CRect    rect;
   TBBUTTON tbb;
   int      i, nCount = DefWindowProc(TB_BUTTONCOUNT, 0, 0);
   for (i = 0; i < nCount; i++)
   {
      if (DefWindowProc(TB_GETITEMRECT, i, (LPARAM)&rect) && rect.PtInRect(point) &&
          DefWindowProc(TB_GETBUTTON  , i, (LPARAM)&tbb ) && (tbb.fsState & TBSTATE_ENABLED))break;
   }
   if (i<nCount) 
   {
      CCaraWinApp *pApp = (CCaraWinApp*)AfxGetApp();
      if (pApp->m_pMainWnd)        pApp->m_pMainWnd->SendMessage(WM_COMMAND, tbb.idCommand+ID_TB_RIGHT_EXT);
      else if (pApp->m_pActiveWnd) pApp->m_pActiveWnd->SendMessage(WM_COMMAND, tbb.idCommand+ID_TB_RIGHT_EXT);
   }
}
void CCaraToolbar::OnDestroy() 
{
   KillTimer(TOOLBAR_REPEAT_TIMER);
   CToolBar::OnDestroy();
}
void CCaraToolbar::OnRButtonUp(UINT nFlags, CPoint point) 
{
   CToolBar::OnRButtonUp(nFlags, point);
}

LRESULT CCaraToolbar::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
   if (gm_bUserCmd && (message == WM_WINDOWPOSCHANGED))
   {
      WINDOWPOS* pwp = (WINDOWPOS*) lParam;
      if      (pwp->flags & SWP_SHOWWINDOW)
      {
         m_nFlags |=  TOOLBAR_USERCMD_VISIBLE;
         if (IsFloating())
         {
            m_nFlags |=  TOOLBAR_USERCMD_FLOATING;
            TRACE("Floating\n");
         }
         else
         {
            m_nFlags &= ~TOOLBAR_USERCMD_FLOATING;
            TRACE("Docked\n");
         }
         TRACE("Show\n");
         UpdateWindow();
      }
      else if (pwp->flags & SWP_HIDEWINDOW)
      {
         m_nFlags &= ~TOOLBAR_USERCMD_VISIBLE;
         TRACE("Hide\n");
      }
   }
   return CToolBar::DefWindowProc(message, wParam, lParam);
}

void CCaraToolbar::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
   UNUSED(lpDIS);
}

void CCaraToolbar::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
   UNUSED(lpMIS);
}

UINT CCaraToolbar::IdFromPoint(CPoint point, bool bPress)
{
   CRect    rect;
   TBBUTTON tbb, tbbHit;
   tbbHit.idCommand = 0;
   int      i, nCount = DefWindowProc(TB_BUTTONCOUNT, 0, 0);
   for (i = 0; i < nCount; i++)
   {
      if (DefWindowProc(TB_GETITEMRECT, i, (LPARAM)&rect) && 
          DefWindowProc(TB_GETBUTTON  , i, (LPARAM)&tbb)  &&
          !(tbb.fsStyle&TBSTYLE_SEP))
      {
         if (rect.PtInRect(point))          tbbHit     = tbb;
         if (tbb.fsState & TBSTATE_PRESSED) m_tbbPressed = tbb;
      }
   }
   if (tbbHit.idCommand)
   {
      if (tbbHit.fsState & TBSTATE_ENABLED)
      {
         if (bPress)
            DefWindowProc(TB_SETSTATE, tbbHit.idCommand, MAKELONG(tbbHit.fsState|TBSTATE_PRESSED , 0));
         else 
         {
            tbbHit.fsState &= ~TBSTATE_PRESSED;
            DefWindowProc(TB_SETSTATE, tbbHit.idCommand, MAKELONG(tbbHit.fsState, 0));
         }
      }
      else tbbHit.idCommand = 1;
   }

   if (m_tbbPressed.idCommand != tbbHit.idCommand)
      UnpressOld();

   return tbbHit.idCommand;
}

void CCaraToolbar::UnpressOld()
{
   if (m_tbbPressed.idCommand)
   {
      m_tbbPressed.fsState &= ~TBSTATE_PRESSED;
      DefWindowProc(TB_SETSTATE, m_tbbPressed.idCommand, MAKELONG(m_tbbPressed.fsState, 0));
      m_tbbPressed.idCommand = 0;
   }
}

bool CCaraToolbar::IsButtonPressed(UINT nID)
{
   long lRet = DefWindowProc(TB_GETSTATE, (WPARAM) nID, 0); 
   if ((lRet != -1) && (lRet & TBSTATE_CHECKED))
      return true;
   return false;
}

void CCaraToolbar::ModifyFlags(UINT nSet, UINT nRemove)
{
   m_nFlags &= ~nRemove;
   m_nFlags |= nSet;
}

bool CCaraToolbar::SetButtonTimer(UINT nID, bool bSet)
{
   int  i, nCount = DefWindowProc(TB_BUTTONCOUNT, 0, 0);
   long lTimer = GetWindowLong(m_hWnd, GWL_USERDATA);
   TBBUTTON tbb;
   for (i = 0; i < nCount; i++)
   {
      if (DefWindowProc(TB_GETBUTTON, i, (LPARAM)&tbb) && (tbb.idCommand == (int)nID))
      {
         if (bSet) lTimer |=  (1 << i);
         else      lTimer &= ~(1 << i);
         SetWindowLong(m_hWnd, GWL_USERDATA, lTimer);
         return true;
      }
   }
   return false;
}

bool CCaraToolbar::IsFloating()
{
   if (m_pDockBar)
   {
      return (m_pDockBar->m_bFloating != 0) ? true: false;
   }
   return false;
}

bool CCaraToolbar::IsVisiblebyUserCmd()
{
   return ((m_nFlags & TOOLBAR_USERCMD_VISIBLE) != 0) ? true : false;
}

bool CCaraToolbar::IsFloatbyUserCmd()
{
   return ((m_nFlags & TOOLBAR_USERCMD_FLOATING) != 0) ? true : false;
}

LRESULT CCaraToolbar::OnNcHitTest(CPoint pt)
{
   ScreenToClient(&pt);
   
   if ((pt.x < 0) || (pt.y < 0))
      m_nFlags |= TOOLBAR_HTGRIPPER;
   else
      m_nFlags &= ~TOOLBAR_HTGRIPPER;

	return HTCLIENT;
}

BOOL CCaraToolbar::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
   if (m_nFlags & TOOLBAR_HTGRIPPER)
   {
      SetCursor(LoadCursor(g_hCaraPreviewInstance, MAKEINTRESOURCE(IDC_OPENHANDCURSOR)));
      return true;
   }
	return CToolBar::OnSetCursor(pWnd, nHitTest, message);
}

void CCaraToolbar::EnableUserCmds(bool bEnable)
{
   gm_bUserCmd = bEnable;
}

BOOL CCaraToolbar::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// TODO: Speziellen Code hier einfügen und/oder Basisklasse aufrufen
	NMHDR *pNotify = (NMHDR*) lParam;
   if (pNotify->code == TTN_NEEDTEXT)
   {
      wParam = wParam;
   }
	return CToolBar::OnNotify(wParam, lParam, pResult);
}

/*
   TBBUTTONINFO tbbi;
   ZeroMemory(&tbbi, sizeof(TBBUTTONINFO));
   tbbi.cbSize = sizeof(TBBUTTONINFO);
   tbbi.dwMask = TBIF_STYLE;

   if (DefWindowProc(TB_GETBUTTONINFO, nID, (LPARAM)&tbbi)!=-1)
   {
      tbbi.fsStyle &= ~bRemove;
      tbbi.fsStyle |= bSet;
      if (DefWindowProc(TB_SETBUTTONINFO, nID, (LPARAM)&tbbi) != -1)
         return true;
   }
*/


