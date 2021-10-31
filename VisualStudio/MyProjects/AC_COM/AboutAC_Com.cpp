// AboutAC_Com.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "AC_COM.h"
#include "AboutAC_Com.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CAboutAC_Com 


CAboutAC_Com::CAboutAC_Com(CWnd* pParent /*=NULL*/)
	: CDialog(CAboutAC_Com::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAboutAC_Com)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
   m_hFont      = 0;
   m_hHoverCtrl = NULL;
   m_bTrackMouseEvent = 0;
}


void CAboutAC_Com::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutAC_Com)
		// HINWEIS: Der Klassen-Assistent fügt hier DDX- und DDV-Aufrufe ein
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAboutAC_Com, CDialog)
	//{{AFX_MSG_MAP(CAboutAC_Com)
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_DESTROY()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
   ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CAboutAC_Com 

BOOL CAboutAC_Com::OnInitDialog() 
{
	CDialog::OnInitDialog();
   LOGFONT lf;
   TRACKMOUSEEVENT tme;
   tme.cbSize      = sizeof(TRACKMOUSEEVENT);
   tme.dwFlags     = TME_LEAVE;
   tme.hwndTrack   = m_hWnd;
   tme.dwHoverTime = HOVER_DEFAULT;
   _TrackMouseEvent(&tme);

   m_hFont = (HFONT)::SendMessage(m_hWnd, WM_GETFONT, 0, 0);
   ::GetObject(m_hFont, sizeof(LOGFONT), &lf);
   lf.lfUnderline = 1;
   m_hFont = ::CreateFontIndirect(&lf);
   ::SetWindowLong(m_hWnd, GWL_USERDATA, (long) this);

   EnumChildWindows(m_hWnd, EnumChildWndFunc, (LPARAM) this);
	return TRUE;
}

BOOL CALLBACK CAboutAC_Com::EnumChildWndFunc(HWND hwnd, LPARAM lParam)
{
   long lValue = GetWindowLong(hwnd, GWL_ID);
   switch(lValue)
   {
      case IDC_ABOUT_LNK_CARA: 
      case IDC_ABOUT_LNK_CARA_US:
      case IDC_ABOUT_LNK_HIFISH:
      case IDC_ABOUT_EMAIL_CARA:
      case IDC_ABOUT_EMAIL_CARA_US:
      case IDC_ABOUT_EMAIL_HIFISH:
         lValue = ::GetWindowLong(hwnd, GWL_USERDATA);
         if (lValue == 0)
         {
            ::SetWindowLong(hwnd, GWL_USERDATA, ::SetWindowLong(hwnd, GWL_WNDPROC, (long)CAboutAC_Com::SubClassProc));
         }
         break;
      default: break;
   }
   return 1;
}

LRESULT CALLBACK CAboutAC_Com::SubClassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   long lValue = ::GetWindowLong(hwnd, GWL_USERDATA);
   ASSERT(lValue != 0);
   switch (uMsg)
   {
      case WM_MOUSEMOVE:
      {
         CAboutAC_Com *pThis = (CAboutAC_Com*)::GetWindowLong(::GetParent(hwnd), GWL_USERDATA);
         ASSERT(pThis != NULL);
         if (hwnd != pThis->m_hHoverCtrl)
         {
            pThis->ClearHoverWnd();
            ::InvalidateRect(hwnd, NULL, true);
         }
         pThis->m_hHoverCtrl = hwnd;
         wParam |= MK_FROMCHILDWND ;
         return ::SendMessage(::GetParent(hwnd), uMsg, wParam, lParam);
      }
      case WM_LBUTTONDOWN:
      {
         CAboutAC_Com *pThis = (CAboutAC_Com*)::GetWindowLong(::GetParent(hwnd), GWL_USERDATA);
         ASSERT(pThis != NULL);
         bool bMail = false;
         switch (::GetWindowLong(hwnd, GWL_ID))
         {
            case IDC_ABOUT_EMAIL_CARA:  case IDC_ABOUT_EMAIL_CARA_US: case IDC_ABOUT_EMAIL_HIFISH:
               bMail = true;
               break;
            default:break;
         }
         char text[MAX_PATH];
         ::GetWindowText(hwnd, text, MAX_PATH);
         if (bMail)
         {
            char mailto[MAX_PATH];
            wsprintf(mailto, "mailto:%s?subject=AC-Com", text);
            ::ShellExecute(::GetParent(hwnd), "open", mailto, NULL, NULL, SW_SHOWNORMAL);
         }
         else
         {
            ::ShellExecute(::GetParent(hwnd), "open", text, NULL, NULL, SW_SHOWNORMAL);
         }
      } break;
      case WM_PAINT:
      {
         CAboutAC_Com *pThis = (CAboutAC_Com*)::GetWindowLong(::GetParent(hwnd), GWL_USERDATA);
         ASSERT(pThis != NULL);
         PAINTSTRUCT ps;
         char text[MAX_PATH];
         int  nSave, nLen  = ::GetWindowText(hwnd, text, MAX_PATH);
         COLORREF cBkgnd   = ::GetSysColor(COLOR_MENU);
         COLORREF cTextHi  = RGB(255,0,0);
         COLORREF cText    = RGB(0,0,255);
         if (CAC_COMApp::GetColorDiff(cTextHi, cBkgnd) < 50) cTextHi = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
         if (CAC_COMApp::GetColorDiff(cText  , cBkgnd) < 50) cText   = ::GetSysColor(COLOR_MENUTEXT);
         ::BeginPaint(hwnd, &ps);
         ::GetClientRect(hwnd, &ps.rcPaint);
         nSave = ::SaveDC(ps.hdc);
         ::SetBkColor(ps.hdc, cBkgnd);
         if (hwnd == pThis->m_hHoverCtrl) ::SetTextColor(ps.hdc, cTextHi);
         else                             ::SetTextColor(ps.hdc, cText);
         ::SelectObject(ps.hdc, pThis->m_hFont);
         ::DrawText(ps.hdc, text, nLen, &ps.rcPaint, DT_CENTER);
         ::RestoreDC(ps.hdc, nSave);
         ::EndPaint(hwnd, &ps);
         return 0;
      }
      case WM_DESTROY:
         ::SetWindowLong(hwnd, GWL_WNDPROC, lValue);
         break;
   }
   return ::CallWindowProc((WNDPROC)lValue, hwnd, uMsg, wParam, lParam);
}

void CAboutAC_Com::OnMouseMove(UINT nFlags, CPoint point) 
{
   if (nFlags & MK_FROMCHILDWND)
      m_bTrackMouseEvent |= 2;
   else
      ClearHoverWnd();

   if ((m_bTrackMouseEvent & 1)==0)
   {
      TRACKMOUSEEVENT tme;
      tme.cbSize      = sizeof(TRACKMOUSEEVENT);
      tme.dwFlags     = TME_LEAVE;
      tme.hwndTrack   = m_hWnd;
      tme.dwHoverTime = HOVER_DEFAULT;
      _TrackMouseEvent(&tme);
      m_bTrackMouseEvent |= 1;
   }

	CDialog::OnMouseMove(nFlags, point);
}

LRESULT CAboutAC_Com::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
   if ((m_bTrackMouseEvent&2)==0)
      ClearHoverWnd();
   m_bTrackMouseEvent = 0;
   return 0;
}

BOOL CAboutAC_Com::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
   if (m_hHoverCtrl)
   {
      SetCursor(::LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_OPENHANDCURSOR)));
      return true;
   }
	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

void CAboutAC_Com::ClearHoverWnd()
{
   if (m_hHoverCtrl != NULL)
   {
      HWND hWndOld = m_hHoverCtrl;
      m_hHoverCtrl = NULL;
      ::InvalidateRect(hWndOld, NULL, true);
   }
}

void CAboutAC_Com::OnDestroy() 
{
	CDialog::OnDestroy();
	
   if (m_hFont)
   {
      ::DeleteObject(m_hFont);
      m_hFont = NULL;
   }
}

void CAboutAC_Com::OnKillFocus(CWnd* pNewWnd) 
{
	CDialog::OnKillFocus(pNewWnd);
   ClearHoverWnd();
}

