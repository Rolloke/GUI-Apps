#if !defined(AFX_MESZPAGE_H__EECD7FA3_186A_11D1_93E0_00C095ECA33E__INCLUDED_)
#define AFX_MESZPAGE_H__EECD7FA3_186A_11D1_93E0_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MESZPage.h : header file
//

#include "SVPage.h"

/////////////////////////////////////////////////////////////////////////////
// CMESZPage dialog

class CMESZPage : public CSVPage
{
// Construction
public:
	CMESZPage(CSVView* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMESZPage)
	enum { IDD = IDD_MESZ };
	CEdit	m_editFunkUhr;
	CButton	m_btnAutoMESZ;
	CButton	m_btnDCF;
	CComboBox	m_cbMonthMEZ;
	CComboBox	m_cbMonthMESZ;
	BOOL	m_bAutoMESZ;
	int		m_monthMESZ;
	int		m_monthMEZ;
	BOOL	m_bDCF;
	CString	m_sFunkUhr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMESZPage)
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
	void	SetOEMSpecificControls();
	void	EnableDlgControls();

	// Generated message map functions
	//{{AFX_MSG(CMESZPage)
	afx_msg void OnCheckMesz();
	afx_msg void OnSelchangeMonthMEZ();
	afx_msg void OnSelchangeMonthMESZ();
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckDcf();
	afx_msg void OnChangeEditFunkuhr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MESZPAGE_H__EECD7FA3_186A_11D1_93E0_00C095ECA33E__INCLUDED_)
