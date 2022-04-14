#if !defined(AFX_SMSPAGE_H__60D27067_9E3A_11D2_B54A_004005A19028__INCLUDED_)
#define AFX_SMSPAGE_H__60D27067_9E3A_11D2_B54A_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SMSPage.h : header file
//

#include "SVPage.h"
/////////////////////////////////////////////////////////////////////////////
// CSMSPage dialog

class CSMSPage : public CSVPage
{
// Construction
public:
	CSMSPage(CSVView* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSMSPage)
	enum { IDD = IDD_SMS };
	CEdit	m_editOwnNumber;
	CEdit	m_editEplus;
	CEdit	m_editPrefix;
	CEdit	m_editD2;
	CEdit	m_editD1;
	BOOL	m_bSMS;
	CString	m_sD1;
	CString	m_sD2;
	CString	m_sPrefix;
	CString	m_sEplus;
	CString	m_sOwnNumber;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSMSPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Overrides
protected:
	virtual BOOL IsDataValid();
	virtual void SaveChanges();
	virtual void CancelChanges();

	// Implementation
protected:
	void OnChange();
	void ShowHide();

	// Generated message map functions
	//{{AFX_MSG(CSMSPage)
	afx_msg void OnCheckSms();
	afx_msg void OnChangeEditD1();
	afx_msg void OnChangeEditD2();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditPrefix();
	afx_msg void OnChangeEditEplus();
	afx_msg void OnChangeEditOwnNumber();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SMSPAGE_H__60D27067_9E3A_11D2_B54A_004005A19028__INCLUDED_)
