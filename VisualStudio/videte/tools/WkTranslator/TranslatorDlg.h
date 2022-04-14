#pragma once


// CTranslatorDlg dialog

class CTranslatorDlg : public CDialog
{
	DECLARE_DYNAMIC(CTranslatorDlg)

public:
	CTranslatorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTranslatorDlg();

// Dialog Data
	//{{AFX_DATA(CTranslateDialog)
	enum { IDD = IDD_TRANSLATOR };
	CString m_sOrgText;
	CString m_sTransText;
	BOOL m_bMBCS;
	int m_nCodePage;
	//}}AFX_DATA

protected:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTranslateDialog)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CTranslateDialog)
	afx_msg void OnBnClickedBtnConvert();
	afx_msg void OnBnClickedBtnCodepage();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCkMbcs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};
