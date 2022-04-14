#if !defined(AFX_DlgAlarmList_H__5F483494_8007_11D5_9A22_004005A19028__INCLUDED_)
#define AFX_DlgAlarmList_H__5F483494_8007_11D5_9A22_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAlarmList.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CDlgAlarmList dialog
class CDlgAlarmList : public CSkinDialog
{
	friend class CWndAlarmList;
// Construction
public:
	CDlgAlarmList(CWndAlarmList* pWnd);   // standard constructor

public:
	void DeleteAlarms();
	void AddAlarm(const CString& sTyp, const CSystemTime& st, const CStringArray& sa);
	int  GetNrOfAlarms();
	int  GetCurSelAlarm();
	BOOL GetAlarm(int i, CString& sTyp, CString& sTime, CString& sDate, CStringArray& saRest);
	BOOL GetAlarm(int i, CString& sTyp, CSystemTime& st, CStringArray& saRest);
	BOOL GetHeaders(CStringArray& saHeaders);
	void CreateColumnHeader(const CIPCFields& fields);
	int  OptimizeColumnWidth(CDC*pDC=NULL, CDWordArray* pdwaWidth=NULL);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAlarmList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL StretchElements();

	// Generated message map functions
	//{{AFX_MSG(CDlgAlarmList)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnItemchangedListAlarm(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCheckActivity();
	afx_msg void OnCheck1();
	afx_msg void OnCheck2();
	afx_msg void OnDblclkListAlarm(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dialog Data
public:
	enum  { IDD = IDD_ALARM_LIST };

protected:
	void CreateColumnHeader(CIPCField* pField, int iWidth);

private:
	//{{AFX_DATA(CDlgAlarmList)
	CSkinListCtrlX	m_ListAlarm;
	BOOL	m_bDetector1;
	BOOL	m_bDetector2;
	BOOL	m_bActivity;
	//}}AFX_DATA

	CWndAlarmList*	m_pWndAlarmList;
	int				m_iSelectedItem;
public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DlgAlarmList_H__5F483494_8007_11D5_9A22_004005A19028__INCLUDED_)
