#if !defined(AFX_MISCELANEOUSDLG_H__7C502D8B_BA0B_4D09_9A7A_EDED31D6B027__INCLUDED_)
#define AFX_MISCELANEOUSDLG_H__7C502D8B_BA0B_4D09_9A7A_EDED31D6B027__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MiscelaneousDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMiscelaneousDlg dialog

class CMiscelaneousDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CMiscelaneousDlg)

// Construction
public:
	CMiscelaneousDlg();
	~CMiscelaneousDlg();

// Dialog Data
	//{{AFX_DATA(CMiscelaneousDlg)
	enum { IDD = IDD_MISCELANEOUS };
	CComboBox	m_cLanguage;
	int		m_nDriverSigning;
	int		m_nMenuShowDelay;
	BOOL	m_bDevMgrShowDetails;
	BOOL	m_bNoInteractiveServices;
	CString	m_strRegisteredOrganisation;
	CString	m_strRegisteredOwner;
	BOOL	m_bRegDone;
	CString	m_strProductID;
	CString	m_strLangID;
	//}}AFX_DATA
	bool m_bChanged;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMiscelaneousDlg)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMiscelaneousDlg)
	afx_msg void OnBtnSave();
	afx_msg void OnBtnRestore();
	afx_msg void OnChange();
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnHelp(WPARAM, LPARAM);
	afx_msg void OnSelchangeComboLanguages();
	afx_msg void OnBtnSet();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MISCELANEOUSDLG_H__7C502D8B_BA0B_4D09_9A7A_EDED31D6B027__INCLUDED_)
