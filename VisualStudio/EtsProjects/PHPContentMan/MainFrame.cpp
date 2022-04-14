// MainFrame.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "PHPContentMan.h"
#include "MainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

CMainFrame::CMainFrame()
{
   m_nContextMenu = 0;
}

CMainFrame::~CMainFrame()
{
   if (m_hWnd) DestroyWindow();
}


BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_QUERYENDSESSION()
	ON_WM_INITMENUPOPUP()
	//}}AFX_MSG_MAP
   ON_MESSAGE(WM_APP+1, OnAppMessage1)
	ON_WM_MENUSELECT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CMainFrame 


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
   NOTIFYICONDATA nid;
   nid.cbSize = sizeof(NOTIFYICONDATA);
   nid.uFlags = NIF_ICON|NIF_TIP|NIF_MESSAGE;
   nid.hWnd   = m_hWnd;
   nid.hIcon  = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
   ::LoadString(AfxGetInstanceHandle(), IDS_ICON_INFO_TIP, nid.szTip, 63);
   nid.uID    = 0;
   SetIcon(nid.hIcon, true);
   nid.uCallbackMessage = WM_APP + 1;
   if (!Shell_NotifyIcon(NIM_ADD, &nid))
   {
      if (nid.hIcon) ::DestroyIcon(nid.hIcon);
      return -1;
   }
	
	return 0;
}

LRESULT CMainFrame::OnAppMessage1(WPARAM wParam, LPARAM lParam)
{
   switch (lParam)
   {
      case WM_CONTEXTMENU: case WM_RBUTTONUP:
      {
         CMenu menu;
         if (menu.LoadMenu(IDR_CONTEXT_MENU))
         {
            CMenu *pSub = menu.GetSubMenu(m_nContextMenu);
            if (pSub)
            {
               CPoint ptCursor;
               ::GetCursorPos(&ptCursor);
               pSub->TrackPopupMenu(0, ptCursor.x, ptCursor.y, this, NULL);
            }
         }
      }break;
      case WM_MOUSEMOVE:
      {
         wParam = wParam;
         return 0;
      } break;
   }
   return 0;
}


void CMainFrame::OnDestroy() 
{
   NOTIFYICONDATA nid;
   ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
   nid.cbSize = sizeof(NOTIFYICONDATA);
   nid.hWnd   = m_hWnd;
   nid.hIcon  = GetIcon(true);
   if (!Shell_NotifyIcon(NIM_DELETE, &nid)) throw (int)3;
   if (nid.hIcon) ::DestroyIcon(nid.hIcon);
	CFrameWnd::OnDestroy();
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{
	
	CFrameWnd::OnTimer(nIDEvent);
}

BOOL CMainFrame::OnQueryEndSession() 
{
	if (!CFrameWnd::OnQueryEndSession())
		return FALSE;

   PostMessage(WM_CLOSE);
	
	return TRUE;
}

void CMainFrame::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu)
{
   nItemID = nItemID;
}

void CMainFrame::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
{
	CFrameWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
}
