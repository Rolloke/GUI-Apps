#if !defined(AFX_ALARMLISTDIALOG_H__5F483494_8007_11D5_9A22_004005A19028__INCLUDED_)
#define AFX_ALARMLISTDIALOG_H__5F483494_8007_11D5_9A22_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AlarmListDialog.h : header file
//

class CAlarmListWindow;

/////////////////////////////////////////////////////////////////////////////
// CAlarmListDialog dialog
class CAlarmListDialog : public CWK_Dialog
{
// Construction
public:
	CAlarmListDialog(CAlarmListWindow* pWnd);   // standard constructor

public:
	void DeleteAlarms();
	void AddAlarm(const CString& sTyp, const CString& sTime, const CString& sDate);
	int  GetNrOfAlarms();
	int  GetCurSelAlarm();
	BOOL GetAlarm(int i, CString& sTyp, CString& sTime, CString& sDate);
	BOOL GetAlarm(int i, CString& sTyp, CSystemTime& st);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlarmListDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL StretchElements();

	// Generated message map functions
	//{{AFX_MSG(CAlarmListDialog)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnItemchangedListAlarm(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCheckActivity();
	afx_msg void OnCheck1();
	afx_msg void OnCheck2();
	afx_msg void OnDblclkListAlarm(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dialog Data
public:
	enum { IDD = IDD_ALARM_LIST };
private:
	//{{AFX_DATA(CAlarmListDialog)
	CListCtrl	m_ListAlarm;
	BOOL	m_bDetector1;
	BOOL	m_bDetector2;
	BOOL	m_bActivity;
	//}}AFX_DATA

	CAlarmListWindow*	m_pAlarmListWindow;
	int					m_iSelectedItem;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALARMLISTDIALOG_H__5F483494_8007_11D5_9A22_004005A19028__INCLUDED_)
