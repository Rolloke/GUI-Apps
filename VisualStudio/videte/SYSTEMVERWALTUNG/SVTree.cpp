// SVTree.cpp : implementation file
//

#include "stdafx.h"
#include "SystemVerwaltung.h"

#include "SVDoc.h"
#include "SVView.h"

#include "SVTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSVTree

CSVTree::CSVTree(CSVView* pParent)
{
	m_pParent       = pParent;
	m_bDisableMouse = false;
}

CSVTree::~CSVTree()
{
}


BEGIN_MESSAGE_MAP(CSVTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CSVTree)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSVTree message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CSVTree::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
//	cs.style |= DS_CONTROL | WS_CHILD; // gf dass reicht nicht
	
	return CTreeCtrl::PreCreateWindow(cs);
}
/////////////////////////////////////////////////////////////////////////////
/*
void CSVTree::OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	if (m_pParent->TreeSelchangingAllowed(pNMTreeView->itemNew.hItem) ) {
		*pResult = 0;
	}
	else {
		*pResult = 1;
	}
}
*/
/////////////////////////////////////////////////////////////////////////////
void CSVTree::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here

	m_pParent->TreeSelchanged(pNMTreeView->itemNew.hItem);
	
	*pResult = 0;
}

BOOL CSVTree::PreTranslateMessage(MSG* pMsg) 
{
	BOOL bHandled = FALSE;
	// TODO: Add your specialized code here and/or call the base class
	UINT uMsg = pMsg->message;
	if (uMsg == WM_CHAR) 
	{
		UINT uCharCode = (UINT)pMsg->wParam;	// character code 
//		long lKeyData = pMsg->lParam;			   // key data 
		if (uCharCode == VK_TAB) 
		{
			bHandled = TRUE;
		}
	}
	if (!bHandled) 
	{
		bHandled = CTreeCtrl::PreTranslateMessage(pMsg);
	}
	return bHandled;
}

LRESULT CALLBACK CSVTree::SubClassProc(HWND hwnd,UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   long lValue  = ::GetWindowLong(hwnd, GWL_USERDATA);
   switch (uMsg)
   {
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		{
			CWnd *pWnd = CWnd::FromHandle(hwnd);
			ASSERT(pWnd != NULL);
//			ASSERT_KINDOF(CSVTree, pWnd);
			if (((CSVTree*)pWnd)->m_bDisableMouse)
			{
				return 0;
			}
		}break;
/*
	   case WM_ERASEBKGND:
		{
//			::SelectObject((HDC) wParam, GetSysColorBrush(COLOR_INFOBK));
			RECT rc;
			::GetClientRect(hwnd, &rc);
			::FillRect((HDC) wParam, &rc, GetSysColorBrush(COLOR_INFOBK));
			return 1;
		}
*/
      case WM_DESTROY:
		{
         ::SetWindowLong(hwnd, GWL_WNDPROC, lValue);
      }break;
	}
   return ::CallWindowProc((WNDPROC)lValue, hwnd, uMsg, wParam, lParam);
}

int CSVTree::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	ASSERT(::GetWindowLong(m_hWnd, GWL_USERDATA) == 0);
   ::SetWindowLong(m_hWnd, GWL_USERDATA, ::SetWindowLong(m_hWnd, GWL_WNDPROC, (long)SubClassProc));
	
	return 0;
}

void CSVTree::DisableMouse(bool bDisable)
{
	m_bDisableMouse = bDisable;
}
