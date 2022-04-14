// pwgenDlg.h : header file
//

#if !defined(AFX_PWGENDLG_H__0E92A149_D873_11D2_B5AD_004005A19028__INCLUDED_)
#define AFX_PWGENDLG_H__0E92A149_D873_11D2_B5AD_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPwgenDlg dialog

class CPwgenDlg : public CDialog
{
// Construction
public:
	CPwgenDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPwgenDlg)
	enum { IDD = IDD_PWGEN_DIALOG };
	CString	m_sPassword;
	CString	m_sTime;
	CString	m_sDWORD;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPwgenDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPwgenDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PWGENDLG_H__0E92A149_D873_11D2_B5AD_004005A19028__INCLUDED_)
