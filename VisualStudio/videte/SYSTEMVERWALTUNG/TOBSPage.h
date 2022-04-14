#if !defined(AFX_TOBSPAGE_H__D33B6884_8FBA_11D4_9E6C_004005A19028__INCLUDED_)
#define AFX_TOBSPAGE_H__D33B6884_8FBA_11D4_9E6C_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TOBSPage.h : header file
//
#include "SVPage.h"

/////////////////////////////////////////////////////////////////////////////
// CTOBSPage dialog

class CTOBSPage : public CSVPage
{
// Construction
public:
	CTOBSPage(CSVView* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTOBSPage)
	enum { IDD = IDD_TOBS };
	CEdit	m_editPrefixMinimum;
	CEdit	m_editPrefix;
	BOOL	m_bTOBS;
	CString	m_sPrefix;
	int		m_iMinimumPrefix;
	//}}AFX_DATA

	// Overrides
protected:
	virtual BOOL IsDataValid();
	virtual void SaveChanges();
	virtual void CancelChanges();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTOBSPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void OnChange();
	void ShowHide();

	// Generated message map functions
	//{{AFX_MSG(CTOBSPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckTobs();
	afx_msg void OnChangeEditPrefix();
	afx_msg void OnChangeEditPrefixMinimum();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOBSPAGE_H__D33B6884_8FBA_11D4_9E6C_004005A19028__INCLUDED_)
