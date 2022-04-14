// CenterWindowDlg.h : header file
//

#if !defined(AFX_CENTERWINDOWDLG_H__40FF6907_7632_11D3_BA88_00400531137E__INCLUDED_)
#define AFX_CENTERWINDOWDLG_H__40FF6907_7632_11D3_BA88_00400531137E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CCenterWindowDlg dialog

class CCenterWindowDlg : public CWK_Dialog
{
// Construction
public:
	CCenterWindowDlg(CWnd* pParent = NULL);	// standard constructor

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCenterWindowDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCenterWindowDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dialog Data
protected:
	HICON m_hIcon;

	//{{AFX_DATA(CCenterWindowDlg)
	enum { IDD = IDD_CENTERWINDOW_DIALOG };
	CString	m_sWindowTitle;
	CString	m_sStatus;
	//}}AFX_DATA
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CENTERWINDOWDLG_H__40FF6907_7632_11D3_BA88_00400531137E__INCLUDED_)
