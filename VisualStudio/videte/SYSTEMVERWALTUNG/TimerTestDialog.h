#if !defined(AFX_TIMERTESTDIALOG_H__59644014_A925_11D2_B997_00400531137E__INCLUDED_)
#define AFX_TIMERTESTDIALOG_H__59644014_A925_11D2_B997_00400531137E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TimerTestDialog.h : header file
//

#include <afxdtctl.h>

/////////////////////////////////////////////////////////////////////////////
class CSVDoc;

/////////////////////////////////////////////////////////////////////////////
// CTimerTestDialog dialog
class CTimerTestDialog : public CWK_Dialog
{
// Construction
public:
	CTimerTestDialog(CSVDoc* pDoc , CWnd* pParent = NULL);   // standard constructor

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimerTestDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTimerTestDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDatetimechangeTimerTestTimepicker(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDatetimechangeTimerTestDatepicker(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeTimerTestTimerList();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void DoTest();

// Dialog Data
private:
	//{{AFX_DATA(CTimerTestDialog)
	enum { IDD = IDD_TIMER_TEST };
	CDateTimeCtrl	m_ctlDatePicker;
	CDateTimeCtrl	m_ctlTimePicker;
	CCheckListBox	m_ctlTimers;
	//}}AFX_DATA
	CSVDoc* m_pDoc;
	CTime m_testTime;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMERTESTDIALOG_H__59644014_A925_11D2_B997_00400531137E__INCLUDED_)
