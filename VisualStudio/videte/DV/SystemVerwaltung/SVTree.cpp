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
	m_pParent = pParent;
}

CSVTree::~CSVTree()
{
}


BEGIN_MESSAGE_MAP(CSVTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CSVTree)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
//	ON_NOTIFY_REFLECT(TVN_SELCHANGING, OnSelchanging)
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
	if (uMsg == WM_CHAR) {
		TCHAR chCharCode = (TCHAR)pMsg->wParam;	// character code 
//		long lKeyData = pMsg->lParam;			// key data 
		if (chCharCode == VK_TAB) {
			bHandled = TRUE;
		}
	}
	if (!bHandled) {
		bHandled = CTreeCtrl::PreTranslateMessage(pMsg);
	}
	return bHandled;
}
