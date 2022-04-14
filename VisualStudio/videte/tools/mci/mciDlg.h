// mciDlg.h : header file
//

#if !defined(AFX_MCIDLG_H__0488AD3B_483A_11D4_8602_004005A26A3B__INCLUDED_)
#define AFX_MCIDLG_H__0488AD3B_483A_11D4_8602_004005A26A3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "MCIDevice.h"
/////////////////////////////////////////////////////////////////////////////
// CMciDlg dialog

class CMciDlg : public CDialog
{
// Construction
public:
	CMciDlg(CWnd* pParent = NULL);	// standard constructor
    ~CMciDlg();

// Dialog Data
	//{{AFX_DATA(CMciDlg)
	enum { IDD = IDD_MCI_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMciDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMciDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CMCIDevice * m_pMCIDevice2;
	CMCIDevice* m_pMCIDevice;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MCIDLG_H__0488AD3B_483A_11D4_8602_004005A26A3B__INCLUDED_)
