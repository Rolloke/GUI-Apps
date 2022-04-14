// SoftDongleDlg.h : header file
//

#if !defined(AFX_SOFTDONGLEDLG_H__85E32CB7_02DD_11D3_8D21_004005A11E32__INCLUDED_)
#define AFX_SOFTDONGLEDLG_H__85E32CB7_02DD_11D3_8D21_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CSoftDongleDlg dialog

class CSoftDongleDlg : public CDialog
{
// Construction
public:
	CSoftDongleDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSoftDongleDlg)
	enum { IDD = IDD_SOFTDONGLE_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSoftDongleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSoftDongleDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOFTDONGLEDLG_H__85E32CB7_02DD_11D3_8D21_004005A11E32__INCLUDED_)
