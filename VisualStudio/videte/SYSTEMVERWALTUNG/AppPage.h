#if !defined(AFX_APPPAGE_H__920ACAA2_2505_11D1_93E1_00C095ECA33E__INCLUDED_)
#define AFX_APPPAGE_H__920ACAA2_2505_11D1_93E1_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// AppPage.h : header file
//
#include "SVPage.h"

#include "AutoLogout.h"
/////////////////////////////////////////////////////////////////////////////
// CAppPage dialog

class CAppPage : public CSVPage
{
// Construction
public:
	CAppPage(CSVView*  pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAppPage)
	enum { IDD = IDD_APP };
	CButton	m_btnAutoApp;
	CButton	m_btnAutoConn;
	CEdit	m_editConfConn;
	CEdit	m_editAutoConn;
	CEdit	m_editConfApp;
	CEdit	m_editAutoApp;
	CStatic	m_staticAutoConn;
	CStatic	m_staticConfConn;
	CStatic	m_staticConfApp;
	CStatic	m_staticAutoApp;
	BOOL	m_bAutoApp;
	BOOL	m_bAutoConn;
	CString	m_sAutoApp;
	CString	m_sAutoConn;
	CString	m_sConfApp;
	CString	m_sConfConn;
	//}}AFX_DATA

// Overrides
protected:
	virtual BOOL IsDataValid();
	virtual void SaveChanges();
	virtual void CancelChanges();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAppPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void	EnableControls();

	// Generated message map functions
	//{{AFX_MSG(CAppPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckApp();
	afx_msg void OnCheckConn();
	afx_msg void OnChangeEditAutoApp();
	afx_msg void OnChangeEditAutoConn();
	afx_msg void OnChangeEditConfApp();
	afx_msg void OnChangeEditConfConn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CAutoLogout	m_AutoLogout;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APPPAGE_H__920ACAA2_2505_11D1_93E1_00C095ECA33E__INCLUDED_)
