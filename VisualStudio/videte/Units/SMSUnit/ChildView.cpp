// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "SMSUnit.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildView
CChildView::CChildView()
{
	m_bShowLog = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
CChildView::~CChildView()
{
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CChildView,CListView)
	//{{AFX_MSG_MAP(CChildView)
	ON_COMMAND(ID_SHOW_LOG, OnShowLog)
	ON_UPDATE_COMMAND_UI(ID_SHOW_LOG, OnUpdateShowLog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CChildView message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CListView::PreCreateWindow(cs))
		return FALSE;

	cs.style &= ~LVS_ICON;
	cs.style |= LVS_LIST;

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CChildView::Clear()
{
	if (!m_bShowLog)
	{
		GetListCtrl().DeleteAllItems();
	}
	else
	{
		GetListCtrl().InsertItem(GetListCtrl().GetItemCount(),_T(""));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CChildView::Add(const CString& s)
{
	GetListCtrl().InsertItem(GetListCtrl().GetItemCount(),s);
	WK_TRACE(_T("%s\n"),LPCTSTR(s));
}
/////////////////////////////////////////////////////////////////////////////
void CChildView::PostNcDestroy() 
{
}

void CChildView::OnInitialUpdate() 
{
	CListView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	CListCtrl& ListCtrl = GetListCtrl();

// insert column
	/*int i = */ListCtrl.InsertColumn(0, _T(""), LVCFMT_LEFT, -1, -1);
}
/////////////////////////////////////////////////////////////////////////////
void CChildView::OnShowLog() 
{
	// TODO: Add your command handler code here
	m_bShowLog = !m_bShowLog;
	if (!m_bShowLog)
	{
		GetListCtrl().DeleteAllItems();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CChildView::OnUpdateShowLog(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_bShowLog);
}
