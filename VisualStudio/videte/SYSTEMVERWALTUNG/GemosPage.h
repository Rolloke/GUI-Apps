#if !defined(AFX_GEMOSPAGE_H__B90A2FA3_5619_11D3_9988_004005A19028__INCLUDED_)
#define AFX_GEMOSPAGE_H__B90A2FA3_5619_11D3_9988_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GemosPage.h : header file
//

#include "SVPage.h"

/////////////////////////////////////////////////////////////////////////////
// CGemosPage dialog

class CGemosPage : public CSVPage
{
// Construction
public:
	CGemosPage(CSVView* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGemosPage)
	enum { IDD = IDD_GEMOS };
	CButton	m_btnComParameters;
	CComboBox	m_cbComGemos;
	CString		m_sComGemos;
	BOOL	m_bGemosUnit;
	//}}AFX_DATA

// Overrides
protected:
	virtual BOOL IsDataValid();
	virtual void SaveChanges();
	virtual void CancelChanges();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGemosPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	void EnableDlgControls();

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGemosPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnComParameter();
	afx_msg void OnCheckGemos();
	afx_msg void OnSelchangeComGemos();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CInputList*				m_pInputList;
	CComParameters			m_ComParameters;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GEMOSPAGE_H__B90A2FA3_5619_11D3_9988_004005A19028__INCLUDED_)
