#if !defined(AFX_SYSTEMPOLICIES_H__9A57DD41_E711_4A6D_9B95_A32D8FBDED29__INCLUDED_)
#define AFX_SYSTEMPOLICIES_H__9A57DD41_E711_4A6D_9B95_A32D8FBDED29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SystemPolicies.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSystemPolicies dialog

class CSystemPolicies : public CPropertyPage
{
	DECLARE_DYNCREATE(CSystemPolicies)

// Construction
public:
	CSystemPolicies();
	~CSystemPolicies();

// Dialog Data
	//{{AFX_DATA(CSystemPolicies)
	enum { IDD = IDD_SYSTEM_POLICIES };
	CListCtrl	m_List;
	BOOL	m_bRemoveMyDocumentsIcon;
	BOOL	m_bDisableCmdPrompt;
	BOOL	m_bRemoveMyComputerIcon;
	//}}AFX_DATA
	bool m_bChanged;


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSystemPolicies)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CImageList m_ImageList;
	// Generated message map functions
	//{{AFX_MSG(CSystemPolicies)
	afx_msg void OnChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickLstPolicyExplorer(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnChanged();
	afx_msg void OnBtnRestore();
	afx_msg void OnBtnSave();
	afx_msg LRESULT OnHelp(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYSTEMPOLICIES_H__9A57DD41_E711_4A6D_9B95_A32D8FBDED29__INCLUDED_)
