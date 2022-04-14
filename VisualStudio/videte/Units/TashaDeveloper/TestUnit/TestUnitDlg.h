// TestUnitDlg.h : header file
//

#if !defined(AFX_TESTUNITDLG_H__5CB64E07_5ADD_11D3_A9C5_004005A11E32__INCLUDED_)
#define AFX_TESTUNITDLG_H__5CB64E07_5ADD_11D3_A9C5_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTestUnitDlg dialog

class CTestUnitDlg : public CDialog
{
// Construction
public:
	CTestUnitDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CTestUnitDlg)
	enum { IDD = IDD_TESTUNIT_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestUnitDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTestUnitDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnScanPCIBus();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTUNITDLG_H__5CB64E07_5ADD_11D3_A9C5_004005A11E32__INCLUDED_)
