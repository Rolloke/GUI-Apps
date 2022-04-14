#if !defined __CTREE_VIEW_H_INCLUDED_
#define __CTREE_VIEW_H_INCLUDED_

#ifndef _WINDOWS_
   #define  STRICT
   #include <WINDOWS.H>
#endif
#include <commctrl.h>

#include "MyDebug.h"

#if defined _DEBUG
   void TraceNotificationMessage(int);
   #define TRACE_NOTIFICATIONMESSAGE TraceNotificationMessage
#else
   #define TRACE_NOTIFICATIONMESSAGE    ((void)(0))
#endif

typedef int  (*TVFUNCTION)(HWND, HTREEITEM, WPARAM, LPARAM);

class CTreeView
{
public:
   CTreeView();
   ~CTreeView();
   void Init(HWND hwndTree, int nID);
   HWND GetWindowHandle() {return m_hwndTree;};
   void SetbmImageList(int nID, int nWidth);

   HTREEITEM InsertItem(TV_INSERTSTRUCT *ptvis);
   bool DeleteItem(HTREEITEM hTreeItem = TVI_ROOT);
	int  GetItemCount();
	int  ProcessWithItems(TVFUNCTION pfn, WPARAM wParam, LPARAM lParam, HTREEITEM hTvItem = TVI_ROOT);
      
private:
   static LRESULT CALLBACK SubClass(HWND, UINT, WPARAM, LPARAM);
   static LRESULT (CALLBACK *gm_pOldWF)(HWND, UINT, WPARAM, LPARAM);
   int         m_nItemID; 
   HWND        m_hwndTree;
   HIMAGELIST  m_hImageList;
};

#endif