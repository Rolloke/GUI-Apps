// idipuserDlg.h : header file
//

#if !defined(AFX_IDIPUSERDLG_H__87C2E73D_BA78_4A13_B3FE_CD34D7B4E7B5__INCLUDED_)
#define AFX_IDIPUSERDLG_H__87C2E73D_BA78_4A13_B3FE_CD34D7B4E7B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CIdipuserDlg dialog

class CIdipuserDlg : public CDialog
{
// Construction
public:
	CIdipuserDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CIdipuserDlg)
	enum { IDD = IDD_IDIPUSER_DIALOG };
	CListBox	m_listPermission;
	CListBox	m_listUser;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIdipuserDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CIdipuserDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IDIPUSERDLG_H__87C2E73D_BA78_4A13_B3FE_CD34D7B4E7B5__INCLUDED_)
