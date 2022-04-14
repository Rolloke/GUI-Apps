// DBXCheckDlg.h : header file
//

#if !defined(AFX_DBXCHECKDLG_H__1DF3A429_4238_11D2_B929_00C095ECA33E__INCLUDED_)
#define AFX_DBXCHECKDLG_H__1DF3A429_4238_11D2_B929_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CDBXCheckDlg dialog

class CDBXCheckDlg : public CDialog
{
// Construction
public:
	CDBXCheckDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	// CListCtrl	m_ctlDriveList;
	//{{AFX_DATA(CDBXCheckDlg)
	enum { IDD = IDD_DBXCHECK_DIALOG };
	CListBox	m_ctlReport;
	CCheckListBox	m_ctlDriveList;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDBXCheckDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

	void AddLine(const CString &sLine);
// Implementation
public:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDBXCheckDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDoTest();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBXCHECKDLG_H__1DF3A429_4238_11D2_B929_00C095ECA33E__INCLUDED_)
