// MiCoDLRTestDlg.h : header file
//

#if !defined(AFX_MICODLRTESTDLG_H__3284E1E7_E73E_11D2_8CFD_004005A11E32__INCLUDED_)
#define AFX_MICODLRTESTDLG_H__3284E1E7_E73E_11D2_8CFD_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMiCoDLRTestDlg dialog

class CMiCoDLRTestDlg : public CDialog
{
// Construction
public:
	CMiCoDLRTestDlg(CWnd* pParent = NULL);	// standard constructor

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMiCoDLRTestDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void	SetTestEntries();
	void	ClearTestEntries();

	// Generated message map functions
	//{{AFX_MSG(CMiCoDLRTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dialog Data
protected:
	//{{AFX_DATA(CMiCoDLRTestDlg)
	enum { IDD = IDD_MICODLRTEST_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	HICON m_hIcon;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MICODLRTESTDLG_H__3284E1E7_E73E_11D2_8CFD_004005A11E32__INCLUDED_)
