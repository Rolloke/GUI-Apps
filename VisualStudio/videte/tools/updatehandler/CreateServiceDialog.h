#if !defined(AFX_CREATESERVICEDIALOG_H__5F035682_56A2_11D1_93E4_00C095ECA33E__INCLUDED_)
#define AFX_CREATESERVICEDIALOG_H__5F035682_56A2_11D1_93E4_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CreateServiceDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCreateServiceDialog dialog

class CCreateServiceDialog : public CDialog
{
// Construction
public:
	CCreateServiceDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCreateServiceDialog)
	enum { IDD = IDD_SERVICE };
	CListBox	m_lbFiles;
	CString	m_sTitel;
	int		m_iKind;
	CString	m_sVersion;
	//}}AFX_DATA

	CStringArray	m_Files;
	CString			m_InitialDir;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreateServiceDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCreateServiceDialog)
	virtual void OnOK();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CBrush	m_EmptyBrush;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CREATESERVICEDIALOG_H__5F035682_56A2_11D1_93E4_00C095ECA33E__INCLUDED_)
