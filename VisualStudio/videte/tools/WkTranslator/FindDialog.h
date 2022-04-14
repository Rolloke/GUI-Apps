#if !defined(AFX_FINDDIALOG_H__E089A176_CA87_11D4_BAFC_00400531137E__INCLUDED_)
#define AFX_FINDDIALOG_H__E089A176_CA87_11D4_BAFC_00400531137E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FindDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFindDialog dialog

class CFindDialog : public CDialog
{
// Construction
public:
	CFindDialog(const CString& sSearchFor, CWnd* pParent = NULL);   // standard constructor

// Attributes
	inline const CString&	SearchFor();
	inline BOOL				CaseSensitive();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFindDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dialog Data
protected:
	//{{AFX_DATA(CFindDialog)
	enum { IDD = IDD_FIND };
	CEdit	m_cFind;
	CString	m_sFind;
	BOOL	m_bCaseSensitive;
	//}}AFX_DATA
};
/////////////////////////////////////////////////////////////////////////////
const CString& CFindDialog::SearchFor()
{
	return m_sFind;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CFindDialog::CaseSensitive()
{
	return m_bCaseSensitive;
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDDIALOG_H__E089A176_CA87_11D4_BAFC_00400531137E__INCLUDED_)
