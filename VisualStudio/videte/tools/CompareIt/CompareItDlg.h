// CompareItDlg.h : header file
//

#if !defined(AFX_COMPAREITDLG_H__9F09D448_886D_11D2_86F1_004005517E7A__INCLUDED_)
#define AFX_COMPAREITDLG_H__9F09D448_886D_11D2_86F1_004005517E7A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CCompareItDlg dialog

class CCompareItDlg : public CDialog
{
// Construction
public:
	CCompareItDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CCompareItDlg)
	enum { IDD = IDD_COMPAREIT_DIALOG };
	CListBox	m_Diffs;
	CString	m_sFile1;
	CString	m_sFile2;
	CString	m_sProperties1;
	CString	m_sProperties2;
	CString	m_sResult;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCompareItDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CCompareItDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnFile1();
	afx_msg void OnFile2();
	afx_msg void OnCompare();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMPAREITDLG_H__9F09D448_886D_11D2_86F1_004005517E7A__INCLUDED_)
