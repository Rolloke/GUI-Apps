#if !defined(AFX_SPECIALCHARACTERSDIALOG_H__ABDED545_2219_11D2_9F29_0000C036AC0D__INCLUDED_)
#define AFX_SPECIALCHARACTERSDIALOG_H__ABDED545_2219_11D2_9F29_0000C036AC0D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SpecialCharactersDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSpecialCharactersDialog dialog

class CSpecialCharactersDialog : public CWK_Dialog
{
// Construction
public:
	CSpecialCharactersDialog(CWnd* pParent = NULL);   // standard constructor

// Attributes
public:
	CString	GetSpecialCharacter();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpecialCharactersDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSpecialCharactersDialog)
	afx_msg void OnSelchangeListSpecialCharacters();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListSpecialCharacters();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void	FillListbox();

// Dialog Data
protected:
	//{{AFX_DATA(CSpecialCharactersDialog)
	enum { IDD = IDD_SPECIAL_CHARACTERS };
	CListBox	m_lbSpecialCharacters;
	CString	m_sSpecialCharacter;
	//}}AFX_DATA
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPECIALCHARACTERSDIALOG_H__ABDED545_2219_11D2_9F29_0000C036AC0D__INCLUDED_)
