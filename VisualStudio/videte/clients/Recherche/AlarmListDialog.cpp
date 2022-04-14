// AlarmListDialog.cpp : implementation file
//

#include "stdafx.h"
#include "recherche.h"
#include "AlarmListDialog.h"
#include "AlarmListWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAlarmListDialog dialog
CAlarmListDialog::CAlarmListDialog(CAlarmListWindow* pParent)
	: CWK_Dialog(CAlarmListDialog::IDD, pParent)
{
	m_pAlarmListWindow = pParent;
	m_iSelectedItem = -1;
	//{{AFX_DATA_INIT(CAlarmListDialog)
	m_bDetector1 = TRUE;
	m_bDetector2 = TRUE;
	m_bActivity = TRUE;
	//}}AFX_DATA_INIT
}
/////////////////////////////////////////////////////////////////////////////
void CAlarmListDialog::DoDataExchange(CDataExchange* pDX)
{
	CWK_Dialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAlarmListDialog)
	DDX_Control(pDX, IDC_LIST_ALARM, m_ListAlarm);
	DDX_Check(pDX, IDC_CHECK_1, m_bDetector1);
	DDX_Check(pDX, IDC_CHECK_2, m_bDetector2);
	DDX_Check(pDX, IDC_CHECK_ACTIVITY, m_bActivity);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CAlarmListDialog, CWK_Dialog)
	//{{AFX_MSG_MAP(CAlarmListDialog)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ALARM, OnItemchangedListAlarm)
	ON_BN_CLICKED(IDC_CHECK_ACTIVITY, OnCheckActivity)
	ON_BN_CLICKED(IDC_CHECK_1, OnCheck1)
	ON_BN_CLICKED(IDC_CHECK_2, OnCheck2)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ALARM, OnDblclkListAlarm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CAlarmListDialog message handlers
void CAlarmListDialog::OnCancel() 
{
	EndDialog(IDCANCEL);
}
/////////////////////////////////////////////////////////////////////////////
void CAlarmListDialog::OnOK() 
{
	EndDialog(IDOK);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAlarmListDialog::StretchElements()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAlarmListDialog::OnInitDialog() 
{
	CWK_Dialog::OnInitDialog();
	
	CRect rect;
	m_ListAlarm.GetClientRect(rect);

	m_ListAlarm.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	CString s;
	// Calculate ListControl unit to match coloumn width
	int iUnit = rect.Width()/5;
	s.LoadString(IDS_TYP);
	m_ListAlarm.InsertColumn(0,s,LVCFMT_LEFT,rect.Width()-4*iUnit);
	s.LoadString(IDS_DATE);
	m_ListAlarm.InsertColumn(1,s,LVCFMT_LEFT,2*iUnit);
	s.LoadString(IDS_TIME);
	m_ListAlarm.InsertColumn(2,s,LVCFMT_LEFT,2*iUnit);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CAlarmListDialog::DeleteAlarms()
{
	m_ListAlarm.DeleteAllItems();
}
/////////////////////////////////////////////////////////////////////////////
void CAlarmListDialog::AddAlarm(const CString& sTyp, const CString& sTime, const CString& sDate)
{
	if (   (sTyp == _T("A") && m_bActivity)
		|| (sTyp == _T("1") && m_bDetector1)
		|| (sTyp == _T("2") && m_bDetector2)
		)
	{
		CSystemTime st;
		st.SetDBDate(sDate);
		st.SetDBTime(sTime);
		int i = m_ListAlarm.InsertItem(m_ListAlarm.GetItemCount(),sTyp);
		m_ListAlarm.SetItemText(i,1,st.GetDate());
		m_ListAlarm.SetItemText(i,2,st.GetTime());
	}
}
/////////////////////////////////////////////////////////////////////////////
int CAlarmListDialog::GetNrOfAlarms()
{
	return m_ListAlarm.GetItemCount();
}
/////////////////////////////////////////////////////////////////////////////
int CAlarmListDialog::GetCurSelAlarm()
{
	return m_iSelectedItem;
}
/////////////////////////////////////////////////////////////////////////////
void CAlarmListDialog::OnItemchangedListAlarm(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	m_iSelectedItem = pNMListView->iItem;
	TRACE(_T("Cur sel is %d\n"),m_iSelectedItem);
	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAlarmListDialog::GetAlarm(int i, CString& sTyp, CSystemTime& st)
{
	CString sDate,sTime;
	if (GetAlarm(i,sTyp,sTime,sDate))
	{
		st.SetDBDate(sDate);
		st.SetDBTime(sTime);
		return TRUE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAlarmListDialog::GetAlarm(int i, CString& sTyp, CString& sTime, CString& sDate)
{
	if (i>=0
		&& i<m_ListAlarm.GetItemCount())
	{
		CString s;
		s = m_ListAlarm.GetItemText(i,0);
		sTyp = s;
		s = m_ListAlarm.GetItemText(i,1);
		sDate = s.Right(4) + s.Mid(3,2) + s.Left(2);
		s = m_ListAlarm.GetItemText(i,2);
		sTime = s.Left(2) + s.Mid(3,2) + s.Right(2);

		return TRUE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CAlarmListDialog::OnCheckActivity() 
{
	UpdateData();
	ASSERT(m_pAlarmListWindow);
	m_pAlarmListWindow->UpdateAlarmList();
}
/////////////////////////////////////////////////////////////////////////////
void CAlarmListDialog::OnCheck1() 
{
	UpdateData();
	ASSERT(m_pAlarmListWindow);
	m_pAlarmListWindow->UpdateAlarmList();
}
/////////////////////////////////////////////////////////////////////////////
void CAlarmListDialog::OnCheck2() 
{
	UpdateData();
	ASSERT(m_pAlarmListWindow);
	m_pAlarmListWindow->UpdateAlarmList();
}

void CAlarmListDialog::OnDblclkListAlarm(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_pAlarmListWindow->PostMessage(WM_COMMAND,ID_NAVIGATE_FORWARD);
	
	*pResult = 0;
}
