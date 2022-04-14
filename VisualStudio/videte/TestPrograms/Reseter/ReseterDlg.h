// ReseterDlg.h : header file
//

#if !defined(AFX_RESETERDLG_H__0D1F8B67_217F_11D2_9FB5_00C095ECA33E__INCLUDED_)
#define AFX_RESETERDLG_H__0D1F8B67_217F_11D2_9FB5_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CReseterDlg dialog

class CTestThread;

class CReseterDlg : public CDialog
{
// Construction
public:
	CReseterDlg(CWnd* pParent = NULL);	// standard constructor
	~CReseterDlg();

// Dialog Data
	//{{AFX_DATA(CReseterDlg)
	enum { IDD = IDD_RESETER_DIALOG };
	UINT	m_dwPauseTime;
	BOOL	m_bDoServer;
	BOOL	m_bDoTest;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReseterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CReseterDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDoTest();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CTestThread *m_pTestThread;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESETERDLG_H__0D1F8B67_217F_11D2_9FB5_00C095ECA33E__INCLUDED_)
