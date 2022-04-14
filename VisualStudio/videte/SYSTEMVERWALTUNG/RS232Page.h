#if !defined(AFX_RS232_H__EB347BA5_17C0_11D1_93E0_00C095ECA33E__INCLUDED_)
#define AFX_RS232_H__EB347BA5_17C0_11D1_93E0_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// RS232.h : header file
//

#include "SVPage.h"

class COutputList;
/////////////////////////////////////////////////////////////////////////////
// CRS232Page dialog

class CRS232Page : public CSVPage
{
// Construction
public:
	CRS232Page(CSVView* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRS232Page)
	enum { IDD = IDD_RS232 };
	CComboBox	m_cbRS232;
	CButton		m_checkRS232;
	BOOL		m_bRS232;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRS232Page)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	virtual BOOL IsDataValid();
	virtual void SaveChanges();
	virtual void CancelChanges();
// Implementation
protected:
	void EnableDlgControls();

	// Generated message map functions
	//{{AFX_MSG(CRS232Page)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboRs232();
	afx_msg void OnCheckRs232();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	COutputList*			m_pOutputList;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RS232_H__EB347BA5_17C0_11D1_93E0_00C095ECA33E__INCLUDED_)
