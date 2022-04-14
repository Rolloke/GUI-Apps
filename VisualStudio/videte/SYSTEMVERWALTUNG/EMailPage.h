#if !defined(AFX_EMAILPAGE_H__93967051_1A92_11D4_A104_004005A19028__INCLUDED_)
#define AFX_EMAILPAGE_H__93967051_1A92_11D4_A104_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EMailPage.h : header file
//
#include "SVPage.h"

/////////////////////////////////////////////////////////////////////////////
// CEMailPage dialog

class CEMailPage : public CSVPage
{
// Construction
public:
	CEMailPage(CSVView* pParent);   // standard constructor

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEMailPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
protected:
	virtual BOOL IsDataValid();
	virtual void SaveChanges();
	virtual void CancelChanges();

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CEMailPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioLan();
	afx_msg void OnRadioDfue();
	afx_msg void OnCheckEmail();
	afx_msg void OnChangeEditSmtp();
	afx_msg void OnChangeEditPort();
	afx_msg void OnSelchangeComboDfue();
	afx_msg void OnChangeEditEmail();
	afx_msg void OnChangeEditPassword();
	afx_msg void OnChangeEditUser();
	afx_msg void OnCheckPop();
	afx_msg void OnChangeEditPopServer();
	afx_msg void OnChangeEditPopPort();
	afx_msg void OnChangeEditPopUser();
	afx_msg void OnChangeEditPopPassword();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL CheckRasAPI();
	void ShowHide();
	void OnChange();
	void FillCombo();

// Dialog Data
private:
	//{{AFX_DATA(CEMailPage)
	enum { IDD = IDD_EMAIL };
	CButton	m_checkPOP;
	CEdit	m_editPOPServer;
	CEdit	m_editPOPPort;
	CEdit	m_editPOPPasswd;
	CEdit	m_editPOPUsername;
	CEdit	m_editUser;
	CEdit	m_editPassword;
	CEdit	m_editEmail;
	CButton	m_radioLAN;
	CButton	m_radioDFUE;
	CEdit	m_editSMTP;
	CComboBox	m_comboDFUE;
	CEdit	m_editPort;
	BOOL	m_bEMail;
	int		m_iDFUEon;
	int		m_iPort;
	CString	m_sSMTPServer;
	CString	m_sEmail;
	CString	m_sDFUEPassword;
	CString	m_sDFUEUser;
	BOOL	m_bPOP;
	CString	m_sPOPPassword;
	CString	m_sPOPUser;
	UINT	m_uPOPPort;
	CString	m_sPOPServer;
	int		m_iDFUE;
	//}}AFX_DATA
	CString	m_sDFUE;

	typedef DWORD(CALLBACK *MyRasEnumEntries)(LPTSTR reserved,LPTSTR lpszPhonebook,void* lprasentryname,LPDWORD lpcb,  LPDWORD lpcEntries);
	MyRasEnumEntries m_fpRasEnumEntries;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EMAILPAGE_H__93967051_1A92_11D4_A104_004005A19028__INCLUDED_)
