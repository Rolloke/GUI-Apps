// LanguageSetupDlg.h : header file
//

#if !defined(AFX_LANGUAGESETUPDLG_H__D9B27FF8_4F75_4E12_A067_F35A741E2680__INCLUDED_)
#define AFX_LANGUAGESETUPDLG_H__D9B27FF8_4F75_4E12_A067_F35A741E2680__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CLanguageSetupDlg dialog

class CLanguageSetupDlg : public CDialog
{
// Construction
public:
	LONG ReadRegistryString(HKEY hKey, LPCTSTR strSection, CString &strValue);
	LONG WriteRegistryString(HKEY hKey, LPCTSTR sName, LPCTSTR sValue);
	CLanguageSetupDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CLanguageSetupDlg)
	enum { IDD = IDD_LANGUAGESETUP_DIALOG };
	CComboBox	m_cKBLanguage;
	CComboBox	m_cLanguage;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLanguageSetupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CString m_strLangID;
	CString m_strKBLangID;
	int     m_nSelection;
	int     m_nKBSelection;
	// Generated message map functions
	//{{AFX_MSG(CLanguageSetupDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangeComboLanguages();
	virtual void OnOK();
	afx_msg void OnSelchangeComboKBLanguage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LANGUAGESETUPDLG_H__D9B27FF8_4F75_4E12_A067_F35A741E2680__INCLUDED_)
