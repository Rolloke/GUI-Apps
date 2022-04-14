#if !defined(AFX_PASSWORDDLG_H__440F6AE0_7426_4503_AF2A_5F2E9734AA29__INCLUDED_)
#define AFX_PASSWORDDLG_H__440F6AE0_7426_4503_AF2A_5F2E9734AA29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PasswordDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg dialog

class CPasswordDlg : public CDialog
{
// Construction
public:
	CPasswordDlg(CWnd* pParent = NULL);		// standard constructor
//	virtual ~CPasswordDlg();					// standard destructor

// Dialog Data
	//{{AFX_DATA(CPasswordDlg)
	enum { IDD = IDD_DVHOOK_PASSWORD };
	CString	m_strPassword;
	//}}AFX_DATA
	CWnd *m_pTempMain;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPasswordDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPasswordDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBtnDel();
	//}}AFX_MSG
	afx_msg void OnBtn(UINT);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PASSWORDDLG_H__440F6AE0_7426_4503_AF2A_5F2E9734AA29__INCLUDED_)
