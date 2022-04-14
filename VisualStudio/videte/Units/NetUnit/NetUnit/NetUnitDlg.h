// NetUnitDlg.h : header file
//

#if !defined(AFX_NETUNITDLG_H__740BBC73_7A01_44BA_A142_EE62A765DBD4__INCLUDED_)
#define AFX_NETUNITDLG_H__740BBC73_7A01_44BA_A142_EE62A765DBD4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CNetUnitDlg dialog

class CNetUnitDlg : public CDialog
{
// Construction
public:
	CNetUnitDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CNetUnitDlg)
	enum { IDD = IDD_NETUNIT_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNetUnitDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CNetUnitDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NETUNITDLG_H__740BBC73_7A01_44BA_A142_EE62A765DBD4__INCLUDED_)
