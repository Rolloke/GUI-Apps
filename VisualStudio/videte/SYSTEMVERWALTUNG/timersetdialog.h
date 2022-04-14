/* GlobalReplace: TimeEditorHourCtrl --> TimerEditorHourCtrl */
/* GlobalReplace: HourCtrl --> TimerEditorHourCtrl */
#if !defined(AFX_TIMERSETDIALOG_H__B9370A04_2789_11D2_B8FD_00C095ECA33E__INCLUDED_)
#define AFX_TIMERSETDIALOG_H__B9370A04_2789_11D2_B8FD_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TimerSetDialog.h : header file
//
#include "TimerEditorHourCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CTimerSetDialog dialog

class CTimerSetDialog : public CWK_Dialog
{
// Construction
public:
	CTimerSetDialog(CWnd* pParent = NULL);   // standard constructor

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimerSetDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTimerSetDialog)
	afx_msg void OnTimerSetTypeWeekDay();
	afx_msg void OnTimerSetTypeDate();
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelchangeTimerWeekDayCombo();
	afx_msg void OnUpdateTimerDay();
	afx_msg void OnUpdateTimerMonth();
	afx_msg void OnUpdateTimerYear();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void FillHoursFromString(const CString &sHours);	// CAVEAT fixed format
	void SetDayType(const CString &sDay);
	void SetTimerName(const CString &sName);
	CString GetHoursAsString() const;
	CString GetDayAsString() const;

private:
	void DisableAllWeekWindows();
	void DisableAllDateWindows();
	BOOL IsDataValid();

private:
// Dialog Data
	//{{AFX_DATA(CTimerSetDialog)
	enum { IDD = IDD_TIMER_SET_DIALOG };
	CEdit	m_ctlHoursText;
	CEdit	m_ctlYear;
	CEdit	m_ctlMonth;
	CEdit	m_ctlDay;
	CComboBox	m_ctlWeekdayCombo;
	int		m_iType;
	CString	m_sWeekDay;
	CString	m_sDay;
	CString	m_sMonth;
	CString	m_sYear;
	CString	m_sTimerName;
	//}}AFX_DATA

	CTimerEditorHourCtrl	m_wndHours;
	int						m_iWeekDayIndex;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMERSETDIALOG_H__B9370A04_2789_11D2_B8FD_00C095ECA33E__INCLUDED_)
