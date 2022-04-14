#if !defined(AFX_GAAPAGE_H__EB347BA6_17C0_11D1_93E0_00C095ECA33E__INCLUDED_)
#define AFX_GAAPAGE_H__EB347BA6_17C0_11D1_93E0_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// GAAPage.h : header file
//

#include "SVPage.h"

class CInputList;

/////////////////////////////////////////////////////////////////////////////
// CGAAPage dialog

class CGAAPage : public CSVPage
{
// Construction
public:
	CGAAPage(CSVView* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGAAPage)
	enum { IDD = IDD_GAA };
	CComboBox	m_cbCurrency;
	CComboBox	m_cbComGAA;
	CButton		m_checkGAA;
	BOOL		m_bGAA;
	CString		m_sComGAA;
	CString	m_sCurrency;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGAAPage)
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
	void EnableDlgControls();

	// Generated message map functions
	//{{AFX_MSG(CGAAPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComGaa();
	afx_msg void OnCheckGAA();
	afx_msg void OnEditchangeComboCurrency();
	afx_msg void OnSelchangeComboCurrency();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CInputList*				m_pInputList;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GAAPAGE_H__EB347BA6_17C0_11D1_93E0_00C095ECA33E__INCLUDED_)
