#if !defined(AFX_TIMERTESTDIALOG_H__84F8CD83_0FCF_11D1_B8C8_0060977A76F1__INCLUDED_)
#define AFX_TIMERTESTDIALOG_H__84F8CD83_0FCF_11D1_B8C8_0060977A76F1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TimerTestDialog.h : header file
//

#include "SecTimer.h"
#include <afxdtctl.h>

/////////////////////////////////////////////////////////////////////////////
// CTimerTestDialog dialog

class CTimerTestDialog : public CDialog
{
// Construction
public:
	CTimerTestDialog(CWnd* pParent = NULL);   // standard constructor

	CSecTimerArray m_timers;
	CTime m_testTime;
// Dialog Data
	//{{AFX_DATA(CTimerTestDialog)
	enum { IDD = IDD_TIMER_TEST };
	CDateTimeCtrl	m_ctlTestDate;
	CCheckListBox	m_ctlTimerList;
	int		m_iTestIntervall;
	BOOL	m_bShowMids;
	//}}AFX_DATA


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
	virtual void OnOK();
	afx_msg void OnDeselectAll();
	afx_msg void OnSelectAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMERTESTDIALOG_H__84F8CD83_0FCF_11D1_B8C8_0060977A76F1__INCLUDED_)
