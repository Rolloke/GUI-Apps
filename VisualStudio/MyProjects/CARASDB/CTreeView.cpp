/********************************************************************/
/* Funktionen der Klasse CTreeView                                   */
/********************************************************************/

#include "CTreeView.h"
#include "MyDebug.h"
#include "resource.h"

extern HINSTANCE g_hInstance;
LRESULT (CALLBACK *CTreeView::gm_pOldWF)(HWND, UINT, WPARAM, LPARAM) = NULL;

CTreeView::CTreeView()
{
   m_nItemID    = 0;
   m_hwndTree   = NULL;
   m_hImageList = NULL;
}

CTreeView::~CTreeView()
{
   if (m_hImageList) VERIFY(ImageList_Destroy(m_hImageList));
}

HTREEITEM CTreeView::InsertItem(TV_INSERTSTRUCT *ptvis)
{
   if (IsWindow(m_hwndTree))
      return (HTREEITEM) SendMessage(m_hwndTree, TVM_INSERTITEM, 0, (LPARAM)ptvis);
   else 
      return NULL;
}

bool CTreeView::DeleteItem(HTREEITEM hTreeItem)
{
   if (IsWindow(m_hwndTree))
      return (SendMessage(m_hwndTree, TVM_DELETEITEM, 0, (LPARAM)hTreeItem)!=0) ?  true : false;
   else 
      return false;
}

void CTreeView::Init(HWND hwndTree, int nID)
{
   m_hwndTree = hwndTree;
   m_nItemID  = nID;
   gm_pOldWF  = (LRESULT (CALLBACK *)(HWND, UINT, WPARAM, LPARAM)) SetWindowLong(m_hwndTree, GWL_WNDPROC, (LONG) CTreeView::SubClass);
}

LRESULT CALLBACK CTreeView::SubClass(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
   ASSERT(gm_pOldWF != NULL);
   if (nMsg == WM_RBUTTONDOWN) return 0;
   return CallWindowProc(gm_pOldWF, hWnd, nMsg, wParam, lParam);
}

/******************************************************************************
* Name       : SetbmImageList                                                 *
* Zweck      : Setzt die Imageliste eines TreeViews aus einer Resource.       *
* Aufruf     : SetbmImageList(nID, nWidth);                                   *
* Parameter  :                                                                *
* nID     (E): Resourcen-ID der ImageListe                              (int) *
* nWidth  (E): Breite der einzelnen Images                              (int) *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CTreeView::SetbmImageList(int nID, int nWidth)
{
   ASSERT(IsWindow(m_hwndTree));
   if (m_hImageList)
   {
      SendMessage(m_hwndTree, TVM_SETIMAGELIST, (WPARAM)TVSIL_NORMAL, NULL);
      VERIFY(ImageList_Destroy(m_hImageList));
   }
   m_hImageList = ImageList_LoadImage(g_hInstance, MAKEINTRESOURCE(nID),
      nWidth, 1, ::GetSysColor(COLOR_WINDOW), IMAGE_BITMAP, LR_DEFAULTCOLOR|LR_LOADTRANSPARENT);
   ASSERT(m_hImageList != NULL);
   SendMessage(m_hwndTree, TVM_SETIMAGELIST, (WPARAM)TVSIL_NORMAL, (LPARAM) m_hImageList);
}

int CTreeView::GetItemCount()
{
   return SendMessage(m_hwndTree, TVM_GETCOUNT, 0, 0);
}

int CTreeView::ProcessWithItems(TVFUNCTION pfn, WPARAM wParam, LPARAM lParam, HTREEITEM hTvItem)
{
   int nResult = 0;
   HTREEITEM hNextTvItem = hTvItem;
   HTREEITEM hChildTvItem;
   if (!IsWindow(m_hwndTree)) return nResult;
   while (hNextTvItem)
   {
      if (hNextTvItem == TVI_ROOT) hNextTvItem = NULL;
      else hNextTvItem = (HTREEITEM)SendMessage(m_hwndTree, TVM_GETNEXTITEM, (WPARAM)TVGN_NEXT, (LPARAM)hNextTvItem);
      nResult = pfn(m_hwndTree, hTvItem, wParam, lParam);
      if (nResult == -1) return nResult;
      if (nResult ==  0)
      {
         hTvItem = hNextTvItem;
         continue;
      }

      hChildTvItem = (HTREEITEM)SendMessage(m_hwndTree, TVM_GETNEXTITEM, (WPARAM)TVGN_CHILD, (LPARAM)hTvItem);
      if (hChildTvItem)
      {
         nResult = ProcessWithItems(pfn, wParam, lParam, hChildTvItem);
         if (nResult == -1) return nResult;
      }
      hTvItem = hNextTvItem;
   }
   return nResult;
}

#if defined _DEBUG
void TraceNotificationMessage(int nMessage)
{
   switch(nMessage)
   {
      case TVN_BEGINDRAG:        TRACE("TVN_BEGINDRAG\n");     break;
      case TVN_BEGINLABELEDIT:   TRACE("TVN_BEGINLABELEDIT\n");break;
      case TVN_BEGINRDRAG:       TRACE("TVN_BEGINRDRAG\n");    break;
      case TVN_DELETEITEM:       TRACE("TVN_DELETEITEM\n");    break;
      case TVN_ENDLABELEDIT:     TRACE("TVN_ENDLABELEDIT\n");  break;
      case TVN_GETDISPINFO:      TRACE("TVN_GETDISPINFO\n");   break;
      case TVN_ITEMEXPANDED:     TRACE("TVN_ITEMEXPANDED\n");  break;
      case TVN_ITEMEXPANDING:    TRACE("TVN_ITEMEXPANDING\n"); break;
      case TVN_KEYDOWN:          TRACE("TVN_KEYDOWN\n");       break;
      case TVN_SELCHANGING:      TRACE("TVN_SELCHANGING\n");   break;
      case TVN_SELCHANGED:       TRACE("TVN_SELCHANGED\n");    break;
      case TVN_SETDISPINFO:      TRACE("TVN_SETDISPINFO\n");   break;
      case TVM_HITTEST:          TRACE("TVM_HITTEST\n");       break;
      default:                   TRACE("Unknown(%d)\n", nMessage);  break;
   }
}
#endif

