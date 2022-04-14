// BootIniPatchDlg.h : header file
//

#if !defined(AFX_ABOUTDLG_H__B4C60735_772E_435F_B850_569C1653C404__INCLUDED_)
#define AFX_ABOUTDLG_H__B4C60735_772E_435F_B850_569C1653C404__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTxtAppVersion();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CString GetFileVersion(const CString& sFileName);

// Dialog Data
protected:
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CStatic	m_txtCopyright;
	CStatic	m_txtAppVersion;
	//}}AFX_DATA
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABOUTDLG_H__B4C60735_772E_435F_B850_569C1653C404__INCLUDED_)
