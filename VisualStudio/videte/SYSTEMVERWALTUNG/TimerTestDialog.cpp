// TimerTestDialog.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "TimerTestDialog.h"

#include "SVDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
LRESULT SubClassFnc(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	long lWndProc = GetWindowLong(hwnd, GWL_USERDATA);
	ASSERT(lWndProc != 0);
	if (   (nMsg == WM_LBUTTONDOWN)
		|| (nMsg == WM_LBUTTONDBLCLK)
		|| (   (nMsg == WM_KEYDOWN)
			&& (wParam != VK_DOWN)
			&& (wParam != VK_UP)
			&& (wParam != VK_PRIOR)
			&& (wParam != VK_NEXT)
			)
		)
	{
		TRACE("wparam = %u\n", wParam);
		return 0;
	}
	else if (nMsg == WM_DESTROY)
	{
		long lResult = CallWindowProc((WNDPROC)lWndProc, hwnd, nMsg, wParam, lParam);
		SetWindowLong(hwnd, GWL_WNDPROC, lWndProc);
		return lResult;
	}
	return CallWindowProc((WNDPROC)lWndProc, hwnd, nMsg, wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////
//***************************************************************************
/////////////////////////////////////////////////////////////////////////////
// CTimerTestDialog dialog
CTimerTestDialog::CTimerTestDialog(CSVDoc* pDoc ,CWnd* pParent /*=NULL*/)
	: CWK_Dialog(CTimerTestDialog::IDD, pParent)
{
	m_pDoc = pDoc;
	m_testTime = CTime::GetCurrentTime();

	//{{AFX_DATA_INIT(CTimerTestDialog)
	//}}AFX_DATA_INIT
}
/////////////////////////////////////////////////////////////////////////////
void CTimerTestDialog::DoDataExchange(CDataExchange* pDX)
{
	CWK_Dialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTimerTestDialog)
	DDX_Control(pDX, IDC_TIMER_TEST_DATEPICKER, m_ctlDatePicker);
	DDX_Control(pDX, IDC_TIMER_TEST_TIMEPICKER, m_ctlTimePicker);
	DDX_Control(pDX, IDC_TIMERTEST_TIMER_LIST, m_ctlTimers);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CTimerTestDialog, CWK_Dialog)
	//{{AFX_MSG_MAP(CTimerTestDialog)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_TIMER_TEST_TIMEPICKER, OnDatetimechangeTimerTestTimepicker)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_TIMER_TEST_DATEPICKER, OnDatetimechangeTimerTestDatepicker)
	ON_LBN_SELCHANGE(IDC_TIMERTEST_TIMER_LIST, OnSelchangeTimerTestTimerList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
void CTimerTestDialog::DoTest() 
{
	CTime tDate;
	m_ctlDatePicker.GetTime(tDate);
	CTime tTime;
	m_ctlTimePicker.GetTime(tTime);

	m_testTime = CTime (
						tDate.GetYear(),
						tDate.GetMonth(),
						tDate.GetDay(),
						tTime.GetHour(),
						tTime.GetMinute(),
						0
						);

	// calc the timer states
	// recalc variable dates (weekday) for the given date
	m_pDoc->GetTimers()->UpdateTimers(m_testTime);

	// fill the listbox
	CSecTimer *pTimer = NULL;
	BOOL bInside = FALSE;
	for (int i=0 ; i<m_ctlTimers.GetCount() ; i++)
	{
		pTimer = (CSecTimer*)m_ctlTimers.GetItemDataPtr(i);	// read the direct pointer data

		bInside = pTimer->IsIncluded(m_testTime);
		m_ctlTimers.SetCheck(i, bInside);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTimerTestDialog::OnInitDialog() 
{
	CWK_Dialog::OnInitDialog();
	if (CSkinDialog::SetChildWindowFont(m_hWnd))
	{
		if (m_ToolTip.m_hWnd)
		{
			m_ToolTip.SetFont(CSkinDialog::GetDialogItemGlobalFont(), 0);
		}
	}

	// init date and time pickers
	m_ctlDatePicker.SetTime(&m_testTime);
	m_ctlTimePicker.SetTime(&m_testTime);

	// fill the listbox
	for (int i=0 ; i<m_pDoc->GetTimers()->GetSize() ; i++)
	{
		CSecTimer *pTimer = m_pDoc->GetTimers()->GetAt(i);
		int ix = m_ctlTimers.GetCount();
		ix = m_ctlTimers.InsertString(ix, pTimer->GetName());
		m_ctlTimers.SetItemDataPtr(ix, pTimer);	// direct pointer as ItemData, since we are in a modal dialog
	}

	if (!theApp.IsNT40())
	{
		SetWindowLong(m_ctlTimers.m_hWnd, GWL_USERDATA, SetWindowLong(m_ctlTimers.m_hWnd, GWL_WNDPROC, (long)SubClassFnc));
	}
	else
	{
		m_ctlTimers.EnableWindow(FALSE);
	}
	DoTest();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CTimerTestDialog::OnDatetimechangeTimerTestTimepicker(NMHDR* pNMHDR, LRESULT* pResult) 
{
	DoTest();
	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CTimerTestDialog::OnDatetimechangeTimerTestDatepicker(NMHDR* pNMHDR, LRESULT* pResult) 
{
	DoTest();
	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CTimerTestDialog::OnSelchangeTimerTestTimerList() 
{
//	m_ctlTimers.SetCurSel(LB_ERR);
}
/////////////////////////////////////////////////////////////////////////////
void CTimerTestDialog::OnOK() 
{
	DoTest();
}
