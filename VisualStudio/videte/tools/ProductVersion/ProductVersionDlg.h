// ProductVersionDlg.h : header file
//

#if !defined(AFX_PRODUCTVERSIONDLG_H__207CC328_56E2_11D1_B8CD_0060977A76F1__INCLUDED_)
#define AFX_PRODUCTVERSIONDLG_H__207CC328_56E2_11D1_B8CD_0060977A76F1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "WK_VersionInfo.h"
#include "WK_Dongle.h"

/////////////////////////////////////////////////////////////////////////////
// CProductVersionDlg dialog

class CProductVersionDlg : public CDialog
{
// Construction
public:
	CProductVersionDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CProductVersionDlg)
	enum { IDD = IDD_PRODUCTVERSION_DIALOG };
	CButton	m_ctlCompare;
	CEdit	m_ctlVersionInfos;
	CString	m_sVersion;
	CString	m_sProduct;
	CString	m_sVersionInfos;
	CString	m_sBuild;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProductVersionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CProductVersionDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	afx_msg long DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	
	DECLARE_MESSAGE_MAP()
private:
	void FillEditControl2();
	void BrowseExtension(const CString& sDir, const CString& sExt, CString& result);
	//
	BOOL m_bFileOnly;
	CWK_Dongle m_dongle;
	CFont m_Font;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRODUCTVERSIONDLG_H__207CC328_56E2_11D1_B8CD_0060977A76F1__INCLUDED_)
