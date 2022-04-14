
#ifndef _CPTUnitPage_H
#define _CPTUnitPage_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// PTUnitPage.h : header file
//

#include "SVPage.h"

/////////////////////////////////////////////////////////////////////////////
// CPTUnitPage dialog

class CPTUnitPage : public CSVPage
{
// Construction
public:
	CPTUnitPage(CSVView* pParent, int nPtNr);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPTUnitPage)
	enum { IDD = IDD_PTUNIT };
	CButton	m_btnPTUnit;
	CButton m_btnNr;
	CButton m_btnAll;
	CButton m_btnNothing;
	CButton	m_btnLow;
	CButton	m_btnMedium;
	CButton	m_btnHigh;
	CButton	m_btnKnock;
	CEdit	m_editRufnummer;
	CString	m_sRufNummer;
	BOOL	m_bKnock;
	BOOL	m_bSupportMSN;
	int		m_iSystem;
	int		m_iAccept;
	BOOL	m_bPTUnit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPTUnitPage)
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
	void	ShowHide();

	// Generated message map functions
	//{{AFX_MSG(CPTUnitPage)
	afx_msg void OnCheckPTUnit();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeRufnummer();
	afx_msg void OnChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CString m_sSection;
	CString m_sAppName;
	BOOL	m_bPTUnitOther; // Ist die jeweils andere PTUnit aktiviert?
	int		m_nPtNr;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif
