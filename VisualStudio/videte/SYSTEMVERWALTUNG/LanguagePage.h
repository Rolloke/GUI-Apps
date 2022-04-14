#pragma once

#include "SVPage.h"
#include "resource.h"
#include "afxwin.h"

// CLanguagePage dialog

class CLanguagePage : public CSVPage
{
	DECLARE_DYNAMIC(CLanguagePage)

// Construction
public:
	CLanguagePage(CSVView* pParent = NULL);   // standard constructor
	virtual ~CLanguagePage();

// Dialog Data
	//{{AFX_DATA(CSMSPage)
	enum { IDD = IDD_LANGUAGE_DEFS };
	CComboBox m_comboCodePage;
	CComboBox m_comboCodePageBits;
	CComboBox m_comboKeyBoard;
	CComboBox m_comboInitialKeyBoard;
	CStatic m_txtLanguageName;
	float m_dFixedFontSize;
	float m_dVariableFontSize;
	float m_dDialogFontSize;
	//}}AFX_DATA

	CStringArray m_saLangAbbreviations;
	CStringArray m_saCodePages;


// Overrides
protected:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLanguagePage)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

	virtual void Initialize();
	virtual void SaveChanges();
	virtual void CancelChanges();

protected:
	// Generated message map functions
	//{{AFX_MSG(CLanguagePage)
	afx_msg void OnCbnSelchangeCombo();
	afx_msg void OnBnClickedCkFontSizes();
	afx_msg void OnEnChangeEdtFont();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bFontSizes;
	void EnableCtrls(void);
};
