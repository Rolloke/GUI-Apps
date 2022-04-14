// TimerTestDialog.cpp : implementation file
//

#include "stdafx.h"
#include "secanalyzer.h"
#include "TimerTestDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTimerTestDialog dialog


CTimerTestDialog::CTimerTestDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CTimerTestDialog::IDD, pParent)
{
	// default is oneWeek==1
	//{{AFX_DATA_INIT(CTimerTestDialog)
	m_iTestIntervall = 1;
	m_bShowMids = FALSE;
	//}}AFX_DATA_INIT
}


void CTimerTestDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTimerTestDialog)
	DDX_Control(pDX, IDC_TIMERTEST_DATEPICKER, m_ctlTestDate);
	DDX_Control(pDX, IDC_TIMER_LIST, m_ctlTimerList);
	DDX_Radio(pDX, IDC_TEST_ONE_DAY, m_iTestIntervall);
	DDX_Check(pDX, IDC_TIMER_TEST_SHOW_MIDS, m_bShowMids);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTimerTestDialog, CDialog)
	//{{AFX_MSG_MAP(CTimerTestDialog)
	ON_BN_CLICKED(IDC_DESELECT_ALL, OnDeselectAll)
	ON_BN_CLICKED(IDC_SELECT_ALL, OnSelectAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimerTestDialog message handlers

BOOL CTimerTestDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// fill timer list box m_ctlTimerList
	CTime ct = CTime::GetCurrentTime();
	CWK_Profile wkp;
	m_timers.Load(ct,wkp,FALSE);
	m_timers.SetAutoDelete(TRUE);

	for (int i=0;i<m_timers.GetSize();i++) {
		CSecTimer *pTimer = m_timers[i];
		int ix = m_ctlTimerList.GetCount();
		ix = m_ctlTimerList.InsertString(ix, pTimer->GetName());
		m_ctlTimerList.SetItemData (ix, (DWORD) pTimer);
		m_ctlTimerList.SetCheck(ix,TRUE);
	}

	m_ctlTestDate.SetTime(&ct);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTimerTestDialog::OnOK() 
{
	// read the check list and remove unwanted timers

	
	CTime tDate,tTime;
	m_ctlTestDate.GetTime(tDate);

	m_testTime = CTime(tDate.GetYear(),tDate.GetMonth(),tDate.GetDay(),
		tTime.GetHour(),tTime.GetMinute(),tTime.GetSecond()
		);


	for (int i=m_ctlTimerList.GetCount()-1;i>=0;i--) {
		if (m_ctlTimerList.GetCheck(i)==FALSE) {
			CSecTimer * pTimer = (CSecTimer *)m_ctlTimerList.GetItemData(i);
			m_timers.Remove(pTimer);
			WK_DELETE(pTimer);
		}
	}
	
	CDialog::OnOK();
}

void CTimerTestDialog::OnDeselectAll() 
{
	for (int i=m_ctlTimerList.GetCount()-1;i>=0;i--) {
		m_ctlTimerList.SetCheck(i,FALSE);
	}
}

void CTimerTestDialog::OnSelectAll() 
{
	for (int i=m_ctlTimerList.GetCount()-1;i>=0;i--) {
		m_ctlTimerList.SetCheck(i,FALSE);
	}
}
