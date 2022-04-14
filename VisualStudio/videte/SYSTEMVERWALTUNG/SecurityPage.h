#if !defined(AFX_SECURITYPAGE_H__F00224D3_B4F6_11D2_B567_004005A19028__INCLUDED_)
#define AFX_SECURITYPAGE_H__F00224D3_B4F6_11D2_B567_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SecurityPage.h : header file
//

#include "SVPage.h"

/////////////////////////////////////////////////////////////////////////////
// CSecurityPage dialog

class CSecurityPage : public CSVPage
{
// Construction
public:
	CSecurityPage(CSVView* pParent);   // standard constructor

// Overrides
protected:
	virtual BOOL IsDataValid();
	virtual void SaveChanges();
	virtual void CancelChanges();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSecurityPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSecurityPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditMinPassword();
	afx_msg void OnChangeEditMinAlpha();
	afx_msg void OnChangeEditMinNumeric();
	afx_msg void OnChangeEditTimeLimit();
	afx_msg void OnChangeEditMaxFailed();
	afx_msg void OnChangeEditFailedLockTime();
	afx_msg void OnUseTpwl();
	afx_msg void OnChangeEditTpwl();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL LoadTrivialListFromRegistry();
	BOOL SaveTrivialListToRegistry();
	void ShowHide();

// Dialog Data
private:
	//{{AFX_DATA(CSecurityPage)
	enum { IDD = IDD_SECURITY };
	CEdit	m_editMinPasswordAlpha;
	CEdit	m_editMinPasswordLength;
	CEdit	m_editMaxLoginTries;
	CRichEditCtrl	m_EditTPWL;
	UINT	m_dwMinPasswordLength;
	UINT	m_dwMinPasswordAlpha;
	UINT	m_dwMinPasswordNum;
	UINT	m_dwTimeLimit;
	UINT	m_dwMaxLoginTries;
	UINT	m_dwUserLockTime;
	BOOL	m_bUseTPWL;
	CString	m_sTrivialPasswordList;
	//}}AFX_DATA
	BOOL m_bLoadingTPWL;
	CFont m_fixedFont;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SECURITYPAGE_H__F00224D3_B4F6_11D2_B567_004005A19028__INCLUDED_)
