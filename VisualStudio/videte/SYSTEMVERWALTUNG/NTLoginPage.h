#if !defined(AFX_NTLOGINPAGE_H__A24020D3_A14B_11D1_B423_00C095EC9EFA__INCLUDED_)
#define AFX_NTLOGINPAGE_H__A24020D3_A14B_11D1_B423_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// NTLoginPage.h : header file
//

#include "SVPage.h"

#define REGKEY_LOGON_NT          _T("Microsoft\\Windows NT\\CurrentVersion")
#define REGKEY_LOGON_95          _T("Microsoft\\Windows\\CurrentVersion")
#define REGKEY_WINLOGON          _T("Winlogon")
#define REGKEY_AUTOADMINLOGON    _T("AutoAdminLogon")
#define REGKEY_DEFAULTUSERNAME   _T("DefaultUserName")
#define REGKEY_DEFAULTDOMAINNAME _T("DefaultDomainName")
#define REGKEY_DEFAULTPASSWORD   _T("DefaultPassword")
#define REGKEY_DONTDISPLAYLASTUSERNAME _T("DontDisplayLastUserName")
/////////////////////////////////////////////////////////////////////////////
// CNTLoginPage dialog

class CNTLoginPage : public CSVPage
{
// Construction
public:
	CNTLoginPage() {};
	CNTLoginPage(CSVView* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNTLoginPage)
	enum { IDD = IDD_NTLOGON };
	CEdit	m_editUser;
	CEdit	m_editPassword;
	CEdit	m_editDomain;
	CString	m_sDomain;
	CString	m_sPassword;
	CString	m_sUser;
	BOOL	m_bAutoLogin;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNTLoginPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Overrides
protected:
	virtual BOOL IsDataValid();
	virtual void SaveChanges();
	virtual void CancelChanges();

// Implementation
public:
	void LoadSettings() {CancelChanges();};
	void SaveSettings() {SaveChanges();};
	
protected:
	void EnableDlgControls();

	// Generated message map functions
	//{{AFX_MSG(CNTLoginPage)
	afx_msg void OnCheckOn();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditDomain();
	afx_msg void OnChangeEditPassword();
	afx_msg void OnChangeEditUser();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NTLOGINPAGE_H__A24020D3_A14B_11D1_B423_00C095EC9EFA__INCLUDED_)
