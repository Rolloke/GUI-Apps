#if !defined(AFX_SEARCHFILES_H__4BF13072_4B25_4003_AA87_33BAA6298D38__INCLUDED_)
#define AFX_SEARCHFILES_H__4BF13072_4B25_4003_AA87_33BAA6298D38__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SearchFiles.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSearchFiles dialog

class CSearchFiles : public CDialog
{
// Construction
public:
	CSearchFiles(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSearchFiles)
	enum { IDD = IDD_DVHOOK_SEARCH_FILES };
	CListCtrl	m_ListFiles;
	//}}AFX_DATA
	CString m_sSearch;
	CWnd *m_pTempMain;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSearchFiles)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSearchFiles)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void FindFile(CString sSearch);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEARCHFILES_H__4BF13072_4B25_4003_AA87_33BAA6298D38__INCLUDED_)
