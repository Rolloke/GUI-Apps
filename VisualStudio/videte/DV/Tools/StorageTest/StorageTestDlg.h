// StorageTestDlg.h : header file
//

#if !defined(AFX_STORAGETESTDLG_H__76F763BB_6B5F_4250_9FE2_B689BDEC38B2__INCLUDED_)
#define AFX_STORAGETESTDLG_H__76F763BB_6B5F_4250_9FE2_B689BDEC38B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CStorageTestDlg dialog

class CStorageTestDlg : public CDialog
{
// Construction
public:
	CStorageTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CStorageTestDlg)
	enum { IDD = IDD_STORAGETEST_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStorageTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CStorageTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STORAGETESTDLG_H__76F763BB_6B5F_4250_9FE2_B689BDEC38B2__INCLUDED_)
