/* GlobalReplace: IDC_GRP_DUMMY --> IDC_TIMER_GRP_DUMMY */
// TimerEditorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TimerEditor.h"
#include "TimerEditorDlg.h"

#include "SecTimer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CTimerEditorDlg dialog

int GetSelectedItemFromList(const CListCtrl &listCtrl)
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

CTimerEditorDlg::CTimerEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTimerEditorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTimerEditorDlg)
	m_iTypeSelected = -1;
	m_sSelectedName = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_iTypeSelected = 0;
	m_TimerEditorSpan = NULL;
	m_TimerEditorCombi = NULL;
	m_pSelectedTimer = NULL;

	CTime ct = CTime::GetCurrentTime();
	CWK_Profile wkProfile;
	m_timers.Load(ct, wkProfile);
	m_timers.CalcTotalSpans();
}

void CTimerEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTimerEditorDlg)
	DDX_Control(pDX, IDC_TIMER_GRP_DUMMY, m_groupDummy);
	DDX_Control(pDX, IDC_TIMER_LIST, m_listTimer);
	DDX_Radio(pDX, IDC_RADIO_TIMER_TYPE_SPAN, m_iTypeSelected);
	DDX_Text(pDX, IDC_TIMER_SELECTED_NAME, m_sSelectedName);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTimerEditorDlg, CDialog)
	//{{AFX_MSG_MAP(CTimerEditorDlg)
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RADIO_TIMER_TYPE_COMBINATION, OnRadioTimerTypeCombination)
	ON_BN_CLICKED(IDC_RADIO_TIMER_TYPE_SPAN, OnRadioTimerTypeSpan)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_TIMER_LIST, OnItemchangedTimerList)
	ON_BN_CLICKED(IDC_TIMER_TMP_NEW, OnTimerNew)
	ON_BN_CLICKED(IDC_TIMER_TMP_DELETE, OnTimerDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimerEditorDlg message handlers

BOOL CTimerEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// hide the dummy
	m_groupDummy.ShowWindow(SW_HIDE);
		
	// fill timer list
	m_listTimer.DeleteAllItems();

	CRect rect;
	m_listTimer.GetWindowRect(rect);
	ScreenToClient(rect);

	// set special styles
	DWORD dw = ListView_GetExtendedListViewStyle(m_listTimer);
	dw |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	ListView_SetExtendedListViewStyle(m_listTimer,dw);

	// fill the ListCtrl
	// NOT YET LoadString
	m_listTimer.InsertColumn(0,"Name",LVCFMT_LEFT,rect.Width()-4);	// OOPS guessed border/frame width

	for (int i=0;i<m_timers.GetSize();i++) {
		CSecTimer *pTimer = m_timers[i];
		int ix = m_listTimer.GetItemCount();
		ix = m_listTimer.InsertItem(ix, pTimer->GetName());
		m_listTimer.SetItemData(ix,(DWORD)pTimer);	// direct pointer no ID
	}
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}



// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTimerEditorDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTimerEditorDlg::OnRadioTimerTypeCombination() 
{
	int nResponse = AfxMessageBox("NOT YET LoadString",
					MB_YESNO | MB_ICONQUESTION
					);

	if (nResponse == IDYES) {
		UpdateData();
		ShowCombiEditor();
	} else {
		m_iTypeSelected = 0;
		UpdateData(FALSE);
		// editor remains
	}
}

void CTimerEditorDlg::OnRadioTimerTypeSpan() 
{
	// CTimerWarning dlg;
	int nResponse = AfxMessageBox("NOT YET LoadString",
					MB_YESNO | MB_ICONQUESTION
					);
	if (nResponse == IDYES) {
		UpdateData();
		ShowSpanEditor();
	} else {
		m_iTypeSelected = 1;
		UpdateData(FALSE);
		// editor remains
	}
}

CTimerEditorDlg::~CTimerEditorDlg()
{
	m_timers.DeleteAll();
}

void CTimerEditorDlg::ShowSpanEditor()
{

	if (m_TimerEditorCombi) {
		m_TimerEditorCombi->DestroyWindow();
		m_TimerEditorCombi = NULL;
	}

	// also update the radio buttons
	m_iTypeSelected = 0;
	UpdateData(FALSE);

	if (m_TimerEditorSpan==NULL) {
		m_TimerEditorSpan = new  CTimerEditorSpan(this);
		CRect rect;

		GetDlgItem(IDC_TIMER_GRP_DUMMY)->GetWindowRect(rect);
		ScreenToClient(rect);
		m_TimerEditorSpan->MoveWindow(rect);
		m_TimerEditorSpan->ShowWindow(SW_SHOW);
	}
}

void CTimerEditorDlg::ShowCombiEditor()
{
	if (m_TimerEditorSpan)
	{
		m_TimerEditorSpan->DestroyWindow();
		m_TimerEditorSpan = NULL;
	}

	// also update the radio buttons
	m_iTypeSelected = 1;
	UpdateData(FALSE);


	if (m_TimerEditorCombi==NULL) {
		m_TimerEditorCombi = new CTimerEditorCombi(this);
		CRect rect;
		GetDlgItem(IDC_TIMER_GRP_DUMMY)->GetWindowRect(rect);
		ScreenToClient(rect);
		m_TimerEditorCombi->MoveWindow(rect);
		m_TimerEditorCombi->ShowWindow(SW_SHOW);


		int ix = GetSelectedItemFromList(m_listTimer);
		if (ix != -1) {
			CSecTimer *pTimer = (CSecTimer *)m_listTimer.GetItemData(ix);
			m_TimerEditorCombi->FillFromTimer(pTimer,m_timers);
		} else {
			WK_TRACE_ERROR("Selection error\n");
		}
	}

}


void CTimerEditorDlg::OnItemchangedTimerList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	if (pNMListView->uNewState & LVIS_SELECTED) {
		TRACE("Item Selected\n");
		ShowSpanEditor();	// NOT YET type check
		if (m_TimerEditorSpan) {
			m_pSelectedTimer = (CSecTimer *)m_listTimer.GetItemData(pNMListView->iItem);
			
			m_iTypeSelected = -1;
			if (m_pSelectedTimer) {
				m_iTypeSelected = 0;
				// NOT YET combiTimerType m_iTypeSelected = 1;
				m_sSelectedName = m_pSelectedTimer->GetName();
			} else {
				WK_TRACE_ERROR("InternalError: Timer is NULL!?\n");
			}

			UpdateData(FALSE);

			m_TimerEditorSpan->FillFromTimer(m_pSelectedTimer);
		}
	}

	
	m_listTimer.SetFocus();

	*pResult = 0;
}


void CTimerEditorDlg::OnTimerNew() 
{
	WK_TRACE("NOT YET new timer\n");	
}

void CTimerEditorDlg::OnTimerDelete() 
{
	WK_TRACE("NOT YET delete timer\n");	
}
