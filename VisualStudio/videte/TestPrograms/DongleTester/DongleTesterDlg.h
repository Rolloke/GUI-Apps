// DongleTesterDlg.h : header file
//

#if !defined(AFX_DONGLETESTERDLG_H__5FFEF748_493E_11D1_B8C8_0060977A76F1__INCLUDED_)
#define AFX_DONGLETESTERDLG_H__5FFEF748_493E_11D1_B8C8_0060977A76F1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CDongleTesterDlg dialog

class CDongleTesterDlg : public CDialog
{
// Construction
public:
	CDongleTesterDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDongleTesterDlg)
	enum { IDD = IDD_DONGLETESTER_DIALOG };
	DWORD	m_dwNumTests;
	CString	m_sNumTests;
	CString	m_sElapsedTime;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDongleTesterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDongleTesterDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDoTest();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DONGLETESTERDLG_H__5FFEF748_493E_11D1_B8C8_0060977A76F1__INCLUDED_)
