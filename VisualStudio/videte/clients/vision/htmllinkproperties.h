#if !defined(AFX_HTMLLINKPROPERTIES_H__D4594386_B71E_4F80_AB2A_00387848E167__INCLUDED_)
#define AFX_HTMLLINKPROPERTIES_H__D4594386_B71E_4F80_AB2A_00387848E167__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HTMLlinkProperties.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHTMLlinkProperties dialog

class CHTMLlinkProperties : public CDialog
{
// Construction
public:
	CHTMLlinkProperties(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHTMLlinkProperties)
	enum { IDD = IDD_HTML_LINK_PROPERTIES };
	CButton	m_cOK;
	CString	m_sHTMLlinkInfo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHTMLlinkProperties)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHTMLlinkProperties)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEdtHlInfo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HTMLLINKPROPERTIES_H__D4594386_B71E_4F80_AB2A_00387848E167__INCLUDED_)
