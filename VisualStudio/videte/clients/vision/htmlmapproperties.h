#if !defined(AFX_HTMLMAPPROPERTIES_H__7C9DCB81_3CE6_45CE_92B9_1CC717E032C2__INCLUDED_)
#define AFX_HTMLMAPPROPERTIES_H__7C9DCB81_3CE6_45CE_92B9_1CC717E032C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HTMLmapProperties.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHTMLmapProperties dialog

class CHTMLmapProperties : public CDialog
{
// Construction
public:
	void CheckCopyPictureFile();
	void CheckPictureFile();
	CHTMLmapProperties(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHTMLmapProperties)
	enum { IDD = IDD_HTML_MAP_PROPERTIES };
	CButton	m_cBtnOK;
	CString	m_sPictureFileName;
	CString	m_sPictureSize;
	CString	m_sTitle;
	int		m_nZoomValue;
	//}}AFX_DATA

	CString  m_sPath;
	CSize    m_szPicture;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHTMLmapProperties)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHTMLmapProperties)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnHmPictureFilename();
	afx_msg void OnChange();
	afx_msg void OnChangeZoom();
	afx_msg void OnKillfocusEdtHmTitle();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HTMLMAPPROPERTIES_H__7C9DCB81_3CE6_45CE_92B9_1CC717E032C2__INCLUDED_)
