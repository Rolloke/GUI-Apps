#if !defined(AFX_DlgHTMLmap_H__7C9DCB81_3CE6_45CE_92B9_1CC717E032C2__INCLUDED_)
#define AFX_DlgHTMLmap_H__7C9DCB81_3CE6_45CE_92B9_1CC717E032C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgHTMLmap.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgHTMLmap dialog

class CDlgHTMLmap : public CSkinDialog
{
// Construction
public:
	void CheckCopyPictureFile();
	void CheckPictureFile();
	CDlgHTMLmap(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgHTMLmap)
	enum  { IDD = IDD_HTML_MAP_PROPERTIES };
	CSkinButton	m_cBtnOK;
	CString	m_sPictureFileName;
	CString	m_sPictureSize;
	CString	m_sTitle;
	int		m_nZoomValue;
	//}}AFX_DATA

	CString  m_sPath;
	CSize    m_szPicture;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgHTMLmap)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgHTMLmap)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnHmPictureFilename();
	afx_msg void OnChange();
	afx_msg void OnChangeZoom();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DlgHTMLmap_H__7C9DCB81_3CE6_45CE_92B9_1CC717E032C2__INCLUDED_)
