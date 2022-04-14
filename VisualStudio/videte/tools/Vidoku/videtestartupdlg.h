#if !defined(AFX_VIDETESTARTUPDLG_H__0FE41D30_5A3C_4B43_A989_4F042B55ED80__INCLUDED_)
#define AFX_VIDETESTARTUPDLG_H__0FE41D30_5A3C_4B43_A989_4F042B55ED80__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VideteStartupDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVideteStartupDlg dialog

class CVideteStartupDlg : public CDialog
{
// Construction
public:
	CVideteStartupDlg(CWnd* pParent = NULL);   // standard constructor
	int m_nCloseStartupTimeout;

// Dialog Data
	//{{AFX_DATA(CVideteStartupDlg)
	enum { IDD = IDD_STARTUP };
	//}}AFX_DATA
   bool m_bAutoDelete;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVideteStartupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVideteStartupDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();
	afx_msg void OnCkStartDisable();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	HFONT  m_hFontObj;
	HBRUSH m_hBrush;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIDETESTARTUPDLG_H__0FE41D30_5A3C_4B43_A989_4F042B55ED80__INCLUDED_)
