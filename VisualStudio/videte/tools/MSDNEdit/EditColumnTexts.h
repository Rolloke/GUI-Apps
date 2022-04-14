#if !defined(AFX_EDITCOLUMNTEXTS_H__CB1C5988_E605_4BFC_84D5_C6566E7D7DEB__INCLUDED_)
#define AFX_EDITCOLUMNTEXTS_H__CB1C5988_E605_4BFC_84D5_C6566E7D7DEB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditColumnTexts.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditColumnTexts dialog

class CEditColumnTexts : public CDialog
{
// Construction
public:
	CEditColumnTexts(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditColumnTexts)
	enum { IDD = IDD_TEXT_EDIT };
	CString	m_strEditText;
	CString	m_strEditWhat;
	int      m_nNumberOfItems;
	BOOL	m_bDeleteText;
	//}}AFX_DATA
   bool  m_bNumber;
   bool  m_bVersion;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditColumnTexts)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditColumnTexts)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITCOLUMNTEXTS_H__CB1C5988_E605_4BFC_84D5_C6566E7D7DEB__INCLUDED_)
