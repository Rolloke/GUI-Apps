/* GlobalReplace: Resrtict --> Restrict */
// TimerEditorCombi.cpp : implementation file
//

#include "stdafx.h"
#include "SystemVerwaltung.h"
#include "TimerEditorCombi.h"

#include "TimerEditorPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTimerEditorCombi dialog


CTimerEditorCombi::CTimerEditorCombi(CTimerEditorPage* pParent)
	: CWK_Dialog(CTimerEditorCombi::IDD, pParent)
{
	m_pParent = pParent;
	m_pSelectedTimer = NULL;
	//{{AFX_DATA_INIT(CTimerEditorCombi)
	//}}AFX_DATA_INIT
	Create(IDD,m_pParent);	// CAVEAT order is important, calls OnInitDialog
}


void CTimerEditorCombi::DoDataExchange(CDataExchange* pDX)
{
	CWK_Dialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTimerEditorCombi)
	DDX_Control(pDX, IDC_TIMER_COMBI_SOURCE_BLOCKED, m_ctlTimerListSourceBlocked);
	DDX_Control(pDX, IDC_TIMER_COMBI_SOURCE, m_ctlTimerListSource);
	DDX_Control(pDX, IDC_BUTTON_TIMER_COMI_RESTRICT, m_ctlButtonRestrict);
	DDX_Control(pDX, IDC_BUTTON_TIMER_COMI_INCLUDE, m_ctlButtonInclude);
	DDX_Control(pDX, IDC_BUTTON_TIMER_COMI_EXCLUDE, m_ctlButtonExclude);
	DDX_Control(pDX, IDC_BUTTON_TIMER_COMI_BACK, m_ctlButtonBack);
	DDX_Control(pDX, IDC_TIMER_COMBI_EXCLUDE, m_ctlTimerListExclude);
	DDX_Control(pDX, IDC_TIMER_COMBI_INCLUDE, m_ctlTimerListInclude);
	DDX_Control(pDX, IDC_TIMER_COMBI_RESRTICT, m_ctlTimerListRestriction);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTimerEditorCombi, CWK_Dialog)
	//{{AFX_MSG_MAP(CTimerEditorCombi)
	ON_NOTIFY(NM_SETFOCUS, IDC_TIMER_COMBI_INCLUDE, OnSetfocusTimerCombiInclude)
	ON_NOTIFY(NM_SETFOCUS, IDC_TIMER_COMBI_RESRTICT, OnSetfocusTimerCombiRestrict)
	ON_NOTIFY(NM_SETFOCUS, IDC_TIMER_COMBI_EXCLUDE, OnSetfocusTimerCombiExclude)
	ON_BN_CLICKED(IDC_BUTTON_TIMER_COMI_INCLUDE, OnButtonTimerCombiInclude)
	ON_BN_CLICKED(IDC_BUTTON_TIMER_COMI_RESTRICT, OnButtonTimerCombiRestrict)
	ON_BN_CLICKED(IDC_BUTTON_TIMER_COMI_EXCLUDE, OnButtonTimerCombiExclude)
	ON_BN_CLICKED(IDC_BUTTON_TIMER_COMI_BACK, OnButtonTimerCombiBack)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_TIMER_COMBI_SOURCE, OnItemchangedTimerCombiSource)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_TIMER_COMBI_INCLUDE, OnItemchangedTimerCombiInclude)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_TIMER_COMBI_EXCLUDE, OnItemchangedTimerCombiExclude)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_TIMER_COMBI_RESRTICT, OnItemchangedTimerCombiRestrict)
	ON_NOTIFY(NM_SETFOCUS, IDC_TIMER_COMBI_SOURCE, OnSetfocusTimerCombiSource)
	ON_NOTIFY(NM_DBLCLK, IDC_TIMER_COMBI_INCLUDE, OnDblclkTimerCombiInclude)
	ON_NOTIFY(NM_DBLCLK, IDC_TIMER_COMBI_EXCLUDE, OnDblclkTimerCombiExclude)
	ON_NOTIFY(NM_DBLCLK, IDC_TIMER_COMBI_RESRTICT, OnDblclkTimerCombiResrtict)
	ON_NOTIFY(NM_DBLCLK, IDC_TIMER_COMBI_SOURCE, OnDblclkTimerCombiSource)
	ON_NOTIFY(NM_SETFOCUS, IDC_TIMER_COMBI_SOURCE_BLOCKED, OnSetfocusTimerCombiSourceBlocked)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimerEditorCombi message handlers

void CTimerEditorCombi::PostNcDestroy() 
{
	delete this;
}
void CTimerEditorCombi::OnOK()
{
	DestroyWindow();
}
void CTimerEditorCombi::OnCancel()
{
	DestroyWindow();
}


void CTimerEditorCombi::FillFromTimer(CSecTimer * pTimer, const CSecTimerArray & timers)
{
	m_pSelectedTimer = pTimer;

	// clear all lists
	m_ctlTimerListSource.DeleteAllItems();
	m_ctlTimerListSourceBlocked.DeleteAllItems();

	m_ctlTimerListInclude.DeleteAllItems();
	m_ctlTimerListExclude.DeleteAllItems();
	m_ctlTimerListRestriction.DeleteAllItems();

	// fill the sources excluding 'self', and used ids
	int i=0;
	for (i=0;i<timers.GetSize();i++) {
		const CSecTimer *pOneTimer = timers[i];
		CSecID id ;
		if (pOneTimer) {
			id = pOneTimer->GetID();
			// calc the accoording listCtrl dynamically
			// CAVEAT order is important here! First check for direct/first level usage
			// THEN check via IsTimerUsed
			CListCtrl *pList = & m_ctlTimerListSource;
			if (pTimer->GetPlusIDs().Has(id)) {
				pList = & m_ctlTimerListInclude;
			} else if (pTimer->GetMinusIDs().Has(id)) {
				pList = & m_ctlTimerListExclude;
			} else if (pTimer->GetRestrictIDs().Has(id)) {
				pList = & m_ctlTimerListRestriction;
			} else if (pOneTimer->IsTimerUsed(pTimer->GetID(),timers)) {	// also checks for own id
				pList = & m_ctlTimerListSourceBlocked;
			}
			// insert the item
			int ix = pList->GetItemCount();
			ix = pList->InsertItem(ix, pOneTimer->GetName());
			pList->SetItemData(ix, pOneTimer->GetID().GetID());
		}
	}
}

static void ClearSelection(CListCtrl &list)
{
	int iCount = list.GetItemCount();
	for (int i=0;i<iCount;i++) {
		DWORD dwState = list.GetItemState(i,LVIS_SELECTED);

		if (dwState & LVIS_SELECTED) {
			// deselect the added items
			list.SetItemState(i,0,LVIS_SELECTED);	// set to zero masked by LVIS_...
		}
	}


}
// NOT YET some kind of lib
static int GetSelectedItemFromList(const CListCtrl &listCtrl)
{
	int ix = -1;

	int iCount = listCtrl.GetItemCount();

	for (int i=0;ix==-1 && i<iCount;i++) {
		DWORD dwState = listCtrl.GetItemState(i,LVIS_SELECTED);

		if (dwState & LVIS_SELECTED) {
			ix = i;
		}
	}

	return ix;
}


void CTimerEditorCombi::UpdateActionButtons(HWND hWnd /* = NULL */)
{
	// activate the action buttons depending from the current selection/focus
	BOOL bEnableActions=FALSE;
	BOOL bEnableBack=FALSE;

	if (hWnd==NULL) {
		hWnd = ::GetFocus();
	}

	if (hWnd == m_ctlTimerListSource.m_hWnd ) {
		int ix = GetSelectedItemFromList(m_ctlTimerListSource);
		if (ix != -1) {
			bEnableActions = TRUE;
		}
		// clear the other selections
		ClearSelection(m_ctlTimerListInclude);
		ClearSelection(m_ctlTimerListExclude);
		ClearSelection(m_ctlTimerListRestriction);
	} else if (hWnd == m_ctlTimerListInclude.m_hWnd ) {
		int ix = GetSelectedItemFromList(m_ctlTimerListInclude);
		if (ix != -1) {
			bEnableBack = TRUE;
		}
		// clear the other selections
		ClearSelection(m_ctlTimerListSource);
		ClearSelection(m_ctlTimerListExclude);
		ClearSelection(m_ctlTimerListRestriction);
	} else if (hWnd == m_ctlTimerListExclude.m_hWnd ) {
		int ix = GetSelectedItemFromList(m_ctlTimerListExclude);
		if (ix != -1) {
			bEnableBack= TRUE;
		}
		// clear the other selections
		ClearSelection(m_ctlTimerListSource);
		ClearSelection(m_ctlTimerListInclude);
		ClearSelection(m_ctlTimerListRestriction);
	} else if (hWnd == m_ctlTimerListRestriction.m_hWnd ) {
		int ix = GetSelectedItemFromList(m_ctlTimerListRestriction);
		if (ix != -1) {
			bEnableBack = TRUE;
		}
		// clear the other selections
		ClearSelection(m_ctlTimerListSource);
		ClearSelection(m_ctlTimerListInclude);
		ClearSelection(m_ctlTimerListExclude);
	} else {
		// other non-list window
	}

	m_ctlButtonInclude.EnableWindow(bEnableActions);
	m_ctlButtonExclude.EnableWindow(bEnableActions);
	m_ctlButtonRestrict.EnableWindow(bEnableActions);

	m_ctlButtonBack.EnableWindow(bEnableBack);
}

void CTimerEditorCombi::OnSetfocusTimerCombiSource(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	UpdateActionButtons();
}

void CTimerEditorCombi::OnSetfocusTimerCombiInclude(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	UpdateActionButtons();
}

void CTimerEditorCombi::OnSetfocusTimerCombiRestrict(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	UpdateActionButtons();
}

void CTimerEditorCombi::OnSetfocusTimerCombiExclude(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	UpdateActionButtons();
}

void CTimerEditorCombi::OnButtonTimerCombiInclude() 
{
	TranferTimers(m_ctlTimerListSource,m_ctlTimerListInclude);
}

void CTimerEditorCombi::OnButtonTimerCombiRestrict() 
{
	TranferTimers(m_ctlTimerListSource,m_ctlTimerListRestriction);	
}

void CTimerEditorCombi::OnButtonTimerCombiExclude() 
{
	TranferTimers(m_ctlTimerListSource,m_ctlTimerListExclude);
}

void CTimerEditorCombi::OnButtonTimerCombiBack() 
{
	if ( GetSelectedItemFromList(m_ctlTimerListInclude) !=-1 ) {
		TranferTimers(m_ctlTimerListInclude,m_ctlTimerListSource);
	} else if ( GetSelectedItemFromList(m_ctlTimerListExclude) !=-1 ) {
		TranferTimers(m_ctlTimerListExclude,m_ctlTimerListSource);
	} else if ( GetSelectedItemFromList(m_ctlTimerListRestriction) !=-1 ) {
		TranferTimers(m_ctlTimerListRestriction,m_ctlTimerListSource);
	} else {
		// OOPS
	}
}

void CTimerEditorCombi::TranferTimers(CListCtrl &src, CListCtrl &destination)
{
	// transfer all selected item from src to destination

	CString sName;
	
	// deselect current selection
	ClearSelection(destination);	// OOP needed ?

	int iCount = src.GetItemCount();

	for (int i=iCount-1;i>=0;i--) {	// reverse loop for delete of items
		// move it
		DWORD dwState = src.GetItemState(i,LVIS_SELECTED);

		if (dwState & LVIS_SELECTED) {
			DWORD dwData = src.GetItemData(i);
			
			sName = src.GetItemText(i,0);
			// insert selected, SetItemState did not work
			destination.InsertItem( 
				LVIF_TEXT  | LVIF_PARAM  | LVIF_STATE,  // UINT nMask, NOT YET | LVIF_IMAGE  
				0, // oldIx, // int nItem, 
				sName, // LPCTSTR lpszItem, 
				LVIS_SELECTED,	// UINT nState, 
				LVIS_SELECTED,	//UINT nStateMask//
				0, // int nImage, 
				dwData // LPARAM lParam 
				);
			src.DeleteItem(i);

			if (m_pSelectedTimer) {
				CSecID id(dwData);
				if ( destination==m_ctlTimerListInclude ) {
					m_pSelectedTimer->AddPlusID(id);
				} else if ( destination==m_ctlTimerListExclude ) {
					m_pSelectedTimer->AddMinusID(id);
				} else if ( destination==m_ctlTimerListRestriction) {
					m_pSelectedTimer->AddRestrictID(id);
				} else if ( destination==m_ctlTimerListSource) {
					// back a move
					if ( src==m_ctlTimerListInclude ) {
						m_pSelectedTimer->RemovePlusID(id);
					} else if ( src==m_ctlTimerListExclude ) {
						m_pSelectedTimer->RemoveMinusID(id);
					} else if ( src==m_ctlTimerListRestriction) {
						m_pSelectedTimer->RemoveRestrictID(id);
					} 
				}
			}
		}
	}	// end of loop over selected items (copy loop)


	m_pParent->SetModified(TRUE,TRUE);	// needs server reset
	m_pParent->m_pTimers->CalcTotalSpans(*m_pParent->m_pTimers);	// also have to update all (combi) timers

	UpdateActionButtons(destination);
}

static void InitListBox(CListCtrl &listCtrl)
{
	// set special styles
	DWORD dw = ListView_GetExtendedListViewStyle(listCtrl);
	dw |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	ListView_SetExtendedListViewStyle(listCtrl,dw);
	listCtrl.InsertColumn(0,_T("InvisibleName"),LVCFMT_LEFT,200);
}

BOOL CTimerEditorCombi::OnInitDialog() 
{
	CWK_Dialog::OnInitDialog();
	if (CSkinDialog::SetChildWindowFont(m_hWnd))
	{
		if (m_ToolTip.m_hWnd)
		{
			m_ToolTip.SetFont(CSkinDialog::GetDialogItemGlobalFont(), 0);
		}
	}
	

	InitListBox(m_ctlTimerListSource);
	InitListBox(m_ctlTimerListInclude);
	InitListBox(m_ctlTimerListExclude);
	InitListBox(m_ctlTimerListRestriction);
	InitListBox(m_ctlTimerListSourceBlocked);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CTimerEditorCombi::StretchElements()
{
	// return FALSE;
	return TRUE;
}

void CTimerEditorCombi::OnItemchangedTimerCombiSource(NMHDR* pNMHDR, LRESULT* pResult) 
{
//	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	UpdateActionButtons(m_ctlTimerListSource);	

	*pResult = 0;
}

void CTimerEditorCombi::OnItemchangedTimerCombiInclude(NMHDR* pNMHDR, LRESULT* pResult) 
{
//	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	UpdateActionButtons(m_ctlTimerListInclude);

	*pResult = 0;
}

void CTimerEditorCombi::OnItemchangedTimerCombiExclude(NMHDR* pNMHDR, LRESULT* pResult) 
{
//	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	UpdateActionButtons(m_ctlTimerListExclude);
	
	*pResult = 0;
}

void CTimerEditorCombi::OnItemchangedTimerCombiRestrict(NMHDR* pNMHDR, LRESULT* pResult) 
{
//	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	UpdateActionButtons(m_ctlTimerListRestriction);
	
	*pResult = 0;
}


void CTimerEditorCombi::OnDblclkTimerCombiInclude(NMHDR* pNMHDR, LRESULT* pResult) 
{

	TranferTimers(m_ctlTimerListInclude,m_ctlTimerListSource);
	*pResult = 0;
}

void CTimerEditorCombi::OnDblclkTimerCombiExclude(NMHDR* pNMHDR, LRESULT* pResult) 
{
	
	TranferTimers(m_ctlTimerListExclude,m_ctlTimerListSource);
	*pResult = 0;
}

void CTimerEditorCombi::OnDblclkTimerCombiResrtict(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TranferTimers(m_ctlTimerListRestriction,m_ctlTimerListSource);
	*pResult = 0;
}

void CTimerEditorCombi::OnDblclkTimerCombiSource(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TranferTimers(m_ctlTimerListSource,m_ctlTimerListInclude);
	
	*pResult = 0;
}

void CTimerEditorCombi::OnSetfocusTimerCombiSourceBlocked(NMHDR* pNMHDR, LRESULT* pResult) 
{
UpdateActionButtons();
	
	*pResult = 0;
}
