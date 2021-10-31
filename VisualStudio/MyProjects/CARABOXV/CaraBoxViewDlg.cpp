// CaraBoxViewDlg.cpp: Implementierung der Klasse CCaraBoxViewDlg.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "resource.h"
#include "Ets3DGL.h"
#include "CaraBoxViewDlg.h"
#include "EtsAppWndMsg.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

#define  BUTTON_TIMER_EVENT 0x0ade              // TimereventID für die Button- bzw. Updatesteuerung
#define  BUTTON_TIMER_DELAY    60               // Wiederholungszeit des Timers

extern HINSTANCE g_hInstance;                   // globaler Instance-Handle

LRESULT (CALLBACK *CCaraBoxViewDlg::gm_pOldWF)(HWND, UINT, WPARAM, LPARAM) = NULL;

bool CCaraBoxViewDll::Create()
{
   if ((m_hThread == 0) && (m_nThreadID == 0))
   {
      unsigned int nThreadId;
      m_hThread = (HANDLE)_beginthreadex(NULL, 0, CCaraBoxViewDll::Run, this, 0, &nThreadId);
      if ((m_hThread != INVALID_HANDLE_VALUE) && (nThreadId != 0))
      {
         m_nThreadID = nThreadId;
         return true;
      }
   }
   else if (IsWindow(m_hWndDlg))
   {
      ::PostMessage(m_hWndDlg, WM_SETDLGTOTOP, 0, 0);
   }
   return false;
}

unsigned __stdcall CCaraBoxViewDll::Run(void* pParam)
{
   if (pParam)
   {
      CCaraBoxViewDlg CBVDlg;
      CBVDlg.m_pThreadConnect = (CCaraBoxViewDll*) pParam;
      CBVDlg.Init(g_hInstance, IDD_SHOW_BOX, NULL);
      CBVDlg.DoModal();
      ::CloseHandle(CBVDlg.m_pThreadConnect->m_hThread);
      CBVDlg.m_pThreadConnect->m_hThread   = NULL;
      CBVDlg.m_pThreadConnect->m_nThreadID = 0;
   }
   return 0;
}

CCaraBoxViewDlg::CCaraBoxViewDlg()
{
   Constructor();
}

CCaraBoxViewDlg::CCaraBoxViewDlg(HINSTANCE hInstance, int nID, HWND hwndParent) : CEtsDialog(hInstance, nID, hwndParent)
{
   Constructor();
}

void CCaraBoxViewDlg::Constructor()
{
   m_BoxPos.pdPos[0] = 0;
   m_BoxPos.pdPos[1] = 0;
   m_BoxPos.pdPos[2] = 0;
   m_pBoxView        = NULL;
   m_nButtonTimer    = 0;
   m_bBoxOk          = false;
   m_bTop            = true;
}

CCaraBoxViewDlg::~CCaraBoxViewDlg()
{
   if (m_pBoxView)
   {
      delete m_pBoxView;
   }
}

bool CCaraBoxViewDlg::OnInitDialog(HWND hWnd)
{
   if (!m_pThreadConnect) return false;
   RECT rcDlg;
   RECT rcItem;
   HICON hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_BIGICON));
   if (hIcon) SendMessage(hWnd, WM_SETICON, 1, (LPARAM) hIcon);
   m_pThreadConnect->m_hWndDlg = hWnd;

   ::SetWindowPos(hWnd, HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

   gm_pOldWF  = (LRESULT (CALLBACK *)(HWND, UINT, WPARAM, LPARAM)) SetWindowLong(hWnd, GWL_WNDPROC, (LONG) SubClass);

   CEtsDialog::OnInitDialog(hWnd);
   AutoInitBtnBmp(16, 16);

   m_BoxPos.nPhi      = 30;                                    // Winkel auf sinnvolle Anfangswerte
   m_BoxPos.nTheta    = 0;
   m_BoxPos.nPsi      = 0;

   if (m_pBoxView == NULL)                                     // wenn noch nicht vorhanden
   {
      m_pBoxView = new CCaraWalk;                              // Boxen 3D-View neu anlegen
   }

   if (m_pBoxView)
   {
      HWND hwndBoxView = GetDlgItem(IDC_BOX_VIEW);
      RECT rcClient;
      GetClientRect(hwndBoxView, &rcClient);
      InflateRect(&rcClient, -1, -1);
      DWORD dwModes[2] = {1<<ETS3D_OPENGL_STATE_SHIFT ,0};
      m_pBoxView->DoDataExchange(CARAWALK_SET3DMODE, sizeof(dwModes), &dwModes);
      m_hwndGLView = m_pBoxView->Create(BOX_VIEW, 0, &rcClient, hwndBoxView);
      m_pBoxView->DoDataExchange(CARAWALK_SETANGLE, 0, &m_BoxPos);
   }

   GetWindowRect(m_hWnd, &rcDlg);
   m_szMin.cx = rcDlg.right-rcDlg.left;
   m_szMin.cy = rcDlg.bottom-rcDlg.top;

   GetClientRect(m_hWnd, &rcDlg);
   GetWindowRect(GetDlgItem(IDC_BOX_VIEW), &rcItem);
   MapWindowPoints(NULL, m_hWnd, (LPPOINT) &rcItem, 2);
   m_rcBoxView.top    = rcItem.top;
   m_rcBoxView.left   = rcItem.left;
   m_rcBoxView.right  = rcDlg.right  - rcItem.right;
   m_rcBoxView.bottom = rcDlg.bottom - rcItem.bottom;

   GetWindowRect(GetDlgItem(IDC_PHILEFT), &rcItem);
   MapWindowPoints(NULL, m_hWnd, (LPPOINT) &rcItem, 2);
   m_szButton.cx      = rcItem.right  - rcItem.left;
   m_szButton.cy      = rcItem.bottom - rcItem.top;
   m_nPi              = rcItem.left;
   m_nTop             = rcDlg.bottom - rcItem.top;

   GetWindowRect(GetDlgItem(IDC_THETADOWN), &rcItem);
   MapWindowPoints(NULL, m_hWnd, (LPPOINT) &rcItem, 1);
   m_nTheta           = rcItem.left;

   GetWindowRect(GetDlgItem(IDC_PSILEFT), &rcItem);
   MapWindowPoints(NULL, m_hWnd, (LPPOINT) &rcItem, 1);
   m_nPsi             = rcItem.left;

   if (m_nButtonTimer == 0)                                    // Timer starten
   {
      m_nButtonTimer = ::SetTimer(hWnd, BUTTON_TIMER_EVENT, BUTTON_TIMER_DELAY, NULL);
   }

   return true;
}

void CCaraBoxViewDlg::OnEndDialog(int nResult)
{
   if (m_pBoxView)
   {
      delete m_pBoxView;
      m_pBoxView   = NULL;
      m_hwndGLView = NULL;
   }

   if (m_nButtonTimer)                                         // Timer beenden
   {
      KillTimer(m_nButtonTimer);
      m_nButtonTimer = 0;
   }

   DeleteAutoBtnBmp();

   m_pThreadConnect->m_hWndDlg = NULL;

   HICON hIcon = (HICON) SendMessage(m_hWnd, WM_GETICON, ICON_SMALL, 0);
   if (hIcon) DestroyIcon(hIcon);

   CEtsDialog::OnEndDialog(nResult);                           // Funktion der Basisklasse aufrufen
}

int CCaraBoxViewDlg::OnOk(WORD)
{
   static int nButtonID[6] = {IDC_PHILEFT, IDC_PHIRIGHT, IDC_THETADOWN, IDC_THETAUP, IDC_PSILEFT, IDC_PSIRIGHT};
   HWND hwndFocus = GetFocus();

   for (int i=0; i<6; i++)
   {
      if (hwndFocus == GetDlgItem(nButtonID[i]))
      {
         OnCommand(nButtonID[i], 0, 0);
         return 0;
      }
   }
   SetFocus(GetNextDlgTabItem(m_hWnd, hwndFocus, false));
   return 0;
}

int CCaraBoxViewDlg::OnCancel()
{
   return IDCANCEL;
}

int CCaraBoxViewDlg::OnCommand(WORD nID , WORD nNotifyCode, HWND hwndControl)
{
   switch(nID)
   {
/****************************************************************************************/
      case IDC_PSILEFT:
         m_BoxPos.nPsi += 90;
         if (m_BoxPos.nPsi  >  360) m_BoxPos.nPsi  = 90;
         break;
      case IDC_PSIRIGHT:
         m_BoxPos.nPsi -= 90;
         if (m_BoxPos.nPsi  <    0) m_BoxPos.nPsi = 270;
         break;
      default:
      return CEtsDialog::OnCommand(nID , nNotifyCode, hwndControl);
   }
   UpdateAngle();
   return 0;
}

int CCaraBoxViewDlg::OnMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
   switch(nMsg)
   {
/*
      case WM_NCPAINT:
      {
         HDC hdc;
         RECT rc;
//         ::BeginPaint(m_hWnd, &ps); 
         ::GetClientRect(m_hWnd, &rc);
         rc.left = rc.right - 20;
         rc.top  = rc.bottom - 20;
         hdc = GetDC(m_hWnd);//, (HRGN)wParam, DCX_WINDOW|DCX_INTERSECTRGN);
         DrawFrameControl(hdc, &rc, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);
//         ::EndPaint(m_hWnd, &ps);
         ReleaseDC(m_hWnd, hdc);
      }break;
*/
      case WM_SIZE:
      {
         SIZE szView = {LOWORD(lParam), HIWORD(lParam)};
         return OnSize(szView);
      }

      case WM_GETMINMAXINFO:
      {
         MINMAXINFO *pMMI = (MINMAXINFO*)lParam;
         pMMI->ptMinTrackSize.x = m_szMin.cx;
         pMMI->ptMinTrackSize.y = m_szMin.cy;
      } break;

      case WM_TIMER: return OnTimer(wParam);

      case WM_SYSCOMMAND:
         if (wParam != IDS_TOPMOST) return 0;

      case WM_SETDLGTOTOP:
         m_bTop = !m_bTop;
         ::SetWindowPos(m_hWnd, m_bTop ? HWND_TOPMOST: HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
         break;
      case WM_CONTEXTMENU:
      {
         POINT pt;
         POINTSTOPOINT(pt, lParam);
         ::MapWindowPoints(m_hWnd, m_hwndGLView, &pt, 1);
         if (wParam == (WPARAM)m_hWnd)
         {
            POINT ptC = pt;
            RECT rcClient;
            ScreenToClient(m_hWnd, &ptC);
            if (::PtInRect(&rcClient, ptC))
               return ::SendMessage(m_hwndGLView, nMsg, (WPARAM)m_hwndGLView, MAKELONG(pt.x,pt.y));
         }
      }break;
      case WM_OPENGL_STATE:
         ::PostMessage(m_pThreadConnect->m_hWndParent, nMsg, wParam, lParam);
         return 0;
      case WM_LBUTTONDOWN:
      case WM_LBUTTONUP:
      case WM_MOUSEMOVE:
      {
         POINT pt;
         POINTSTOPOINT(pt, lParam);
         ::MapWindowPoints(m_hWnd, m_hwndGLView, &pt, 1);
         return ::SendMessage(m_hwndGLView, nMsg, wParam, MAKELONG(pt.x,pt.y));
      }
      case WM_ENTERSIZEMOVE: case WM_EXITSIZEMOVE:
         ::SendMessage(m_hwndGLView, nMsg, wParam, lParam);
         break;

      case WM_INITMENUPOPUP:
         return OnInitMenuPopup((HMENU)wParam, (UINT)LOWORD(lParam), (BOOL)HIWORD(lParam));

      case WM_SETBOXDATA: return OnSetBoxData((char*)wParam, (LRESULT*)lParam);

      default: return CEtsDialog::OnMessage(nMsg, wParam, lParam);
   }
   return 0;
}


LRESULT CCaraBoxViewDlg::OnTimer(WPARAM nIDEvent)
{
   bool bChanged = false;
   if (SendDlgItemMessage(IDC_PHILEFT, BM_GETSTATE,0,0) & BST_PUSHED)
   {
      m_BoxPos.nPhi++;
      if (m_BoxPos.nPhi  >= 360) m_BoxPos.nPhi = 0;
      bChanged = true;
   }
   if (SendDlgItemMessage(IDC_PHIRIGHT, BM_GETSTATE,0,0) & BST_PUSHED)
   {
      m_BoxPos.nPhi--;
      if (m_BoxPos.nPhi   <=  0) m_BoxPos.nPhi = 360;
      bChanged = true;
   }
   if (SendDlgItemMessage(IDC_THETAUP, BM_GETSTATE,0,0) & BST_PUSHED)
   {
      if (m_BoxPos.nTheta <  90) m_BoxPos.nTheta++;
      bChanged = true;
   }
   if (SendDlgItemMessage(IDC_THETADOWN, BM_GETSTATE,0,0) & BST_PUSHED)
   {
      if (m_BoxPos.nTheta > -90) m_BoxPos.nTheta--;
      bChanged = true;
   }
   if (bChanged) UpdateAngle();
   return 0;   
}

LRESULT CCaraBoxViewDlg::OnSize(SIZE szView)
{
   SIZE szBoxView = {szView.cx-m_rcBoxView.right-m_rcBoxView.left, szView.cy-m_rcBoxView.bottom-m_rcBoxView.top};
   ::MoveWindow(GetDlgItem(IDC_BOX_VIEW), m_rcBoxView.left, m_rcBoxView.top, szBoxView.cx, szBoxView.cy, true);
/*
   if (m_pBoxView)
   {
      m_pBoxView->MoveWindow(1, 1,  szBoxView.cx-2, szBoxView.cy-2, false);
   }
*/
   ::MoveWindow(GetDlgItem(IDC_PHILEFT)  , m_nPi                 , szView.cy - m_nTop, m_szButton.cx, m_szButton.cy, true);
   ::MoveWindow(GetDlgItem(IDC_PHIRIGHT) , m_nPi-m_szButton.cx   , szView.cy - m_nTop, m_szButton.cx, m_szButton.cy, true);
   ::MoveWindow(GetDlgItem(IDC_THETADOWN), m_nTheta              , szView.cy - m_nTop, m_szButton.cx, m_szButton.cy, true);
   ::MoveWindow(GetDlgItem(IDC_THETAUP)  , m_nTheta-m_szButton.cx, szView.cy - m_nTop, m_szButton.cx, m_szButton.cy, true);
   ::MoveWindow(GetDlgItem(IDC_PSILEFT)  , m_nPsi                , szView.cy - m_nTop, m_szButton.cx, m_szButton.cy, true);
   ::MoveWindow(GetDlgItem(IDC_PSIRIGHT) , m_nPsi  -m_szButton.cx, szView.cy - m_nTop, m_szButton.cx, m_szButton.cy, true);

   return 0;
}
 
LRESULT CCaraBoxViewDlg::OnSetBoxData(char *pszBoxName, LRESULT *lParam)
{
   if (m_pBoxView && pszBoxName)
   {
      if (m_pBoxView->DoDataExchange(CARAWALK_SETBOXDATA, MAKELONG(CARAWALK_SHOW_URP_AND_SCALE, 0xffff), (void*)pszBoxName))
      {
         m_pBoxView->DoDataExchange(CARAWALK_SETANGLE, 0, &m_BoxPos);
         return 1;
      }
   }
   return 0;
}

LRESULT CCaraBoxViewDlg::OnInitMenuPopup(HMENU hMenu, UINT nPos, BOOL bSysMenu)
{
   int nItems = ::GetMenuItemCount(hMenu);
   if (::GetMenuItemID(hMenu, nItems-1) != IDS_TOPMOST)
   {
      char text[64];
      ::LoadString(m_hInstance, IDS_TOPMOST, text, 64);
      ::InsertMenu(hMenu, IDS_TOPMOST, MF_BYCOMMAND|MF_CHECKED, IDS_TOPMOST, text);
   }
   else
   {
      ::CheckMenuItem(hMenu, IDS_TOPMOST, m_bTop ? MF_CHECKED: MF_UNCHECKED);
   }
   return 0;
}

LRESULT CALLBACK CCaraBoxViewDlg::SubClass(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
   ASSERT(gm_pOldWF != NULL);
   if (nMsg == WM_ERASEBKGND)
   {
      HWND hwndBoxView = ::GetDlgItem(hWnd, IDC_BOX_VIEW);
      RECT rcWnd, rcClient;
      ::GetClientRect(hWnd, &rcClient);
      ::GetWindowRect(hwndBoxView, &rcWnd);
      ::ScreenToClient(hWnd, &((POINT*)&rcWnd)[0]);
      ::ScreenToClient(hWnd, &((POINT*)&rcWnd)[1]);
      LOGBRUSH lb = {BS_SOLID, ::GetSysColor(COLOR_BTNFACE), 0};
      HBRUSH hBr = ::CreateBrushIndirect(&lb);
      if (hBr)
      {
         HRGN hrgnWnd    = ::CreateRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom);
         HRGN hrgnClient = ::CreateRectRgn(rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);
         HRGN hrgnFill   = ::CreateRectRgn(0, 0, 1, 1);
         ::CombineRgn(hrgnFill, hrgnClient, hrgnWnd, RGN_DIFF);
         ::FillRgn((HDC)wParam, hrgnFill, hBr);
         ::DeleteObject(hrgnWnd);
         ::DeleteObject(hrgnClient);
         ::DeleteObject(hrgnFill);
         ::DeleteObject(hBr);
      }
      return 1;
   }
   return CallWindowProc(gm_pOldWF, hWnd, nMsg, wParam, lParam);
}

void CCaraBoxViewDlg::UpdateAngle()
{
   if (m_pBoxView)
   {
      m_pBoxView->DoDataExchange(CARAWALK_SETANGLE, 0, &m_BoxPos);
   }
}
