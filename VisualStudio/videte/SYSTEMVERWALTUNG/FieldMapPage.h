#if !defined(AFX_FIELDMAPPAGE_H__4E84B9C1_17E0_11D2_8C1B_00C095EC9EFA__INCLUDED_)
#define AFX_FIELDMAPPAGE_H__4E84B9C1_17E0_11D2_8C1B_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FieldMapPage.h : header file
//

#include "SVPage.h"

/////////////////////////////////////////////////////////////////////////////
// CFieldMapPage dialog

class CFieldMapPage : public CSVPage
{
// Construction
public:
	CFieldMapPage(CSVView* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFieldMapPage)
	enum { IDD = IDD_FIELDMAP };
	CEdit	m_editDtpDate;
	CEdit	m_editTANr;
	CEdit	m_editKtoNr;
	CEdit	m_editGANr;
	CEdit	m_editDTPTime;
	CEdit	m_editBLZ;
	CEdit	m_editBetrag;
	BOOL	m_bKontoNr;
	BOOL	m_bBetrag;
	BOOL	m_bGANr;
	BOOL	m_bTANr;
	BOOL	m_bBLZ;
	BOOL	m_bDTPTime;
	CString	m_sKontoNr;
	CString	m_sBetrag;
	CString	m_sGANr;
	CString	m_sTANr;
	CString	m_sBLZ;
	CString	m_sDTPTime;
	BOOL	m_bDTPDate;
	CString	m_sDTPDate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFieldMapPage)
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
	void LoadStandard();
	void EnableControls();

	// Generated message map functions
	//{{AFX_MSG(CFieldMapPage)
	afx_msg void OnCheckBetrag();
	afx_msg void OnCheckBlz();
	afx_msg void OnCheckDtpZeit();
	afx_msg void OnCheckGanr();
	afx_msg void OnCheckKtonr();
	afx_msg void OnCheckTanr();
	afx_msg void OnCheckDtpDate();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonStandard();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg void OnChange();

private:
	CString m_sVersion;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIELDMAPPAGE_H__4E84B9C1_17E0_11D2_8C1B_00C095EC9EFA__INCLUDED_)
