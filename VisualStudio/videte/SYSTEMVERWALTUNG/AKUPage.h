#if !defined(AFX_AKUPAGE_H__B7EE7A25_12F2_11D1_93D9_00C095ECA33E__INCLUDED_)
#define AFX_AKUPAGE_H__B7EE7A25_12F2_11D1_93D9_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// AKUPage.h : header file
//

#include "SVPage.h"

/////////////////////////////////////////////////////////////////////////////
class CInputList;
class COutputList;

/////////////////////////////////////////////////////////////////////////////
// CAKUPage dialog

class CAKUPage : public CSVPage
{
// Construction
public:
	CAKUPage(CSVView* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAKUPage)
	enum { IDD = IDD_AKU };
	CString	m_sAddressAku1;
	CString	m_sAddressAku2;
	CString	m_sAddressAku3;
	CString	m_sAddressAku4;

	BOOL	m_bAku1;
	BOOL	m_bAku2;
	BOOL	m_bAku3;
	BOOL	m_bAku4;

	CButton	m_checkAku4;
	CButton	m_checkAku3;
	CButton	m_checkAku2;
	CButton	m_checkAku1;

	CComboBox	m_cbAddressAku4;
	CComboBox	m_cbAddressAku3;
	CComboBox	m_cbAddressAku2;
	CComboBox	m_cbAddressAku1;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAKUPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Overrides
protected:
	virtual BOOL IsDataValid();
	virtual void SaveChanges();
	virtual void CancelChanges();
	virtual void SetModified(BOOL bModified = TRUE ,BOOL bServerInit = TRUE);

// Implementation
protected:
	void LoadAkuAddresses();
	void SaveAkuAddresses();
	void EnableDlgControls();
	void GetDlgAkuData();

	// Generated message map functions
	//{{AFX_MSG(CAKUPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckAku1();
	afx_msg void OnCheckAku2();
	afx_msg void OnCheckAku3();
	afx_msg void OnCheckAku4();
	afx_msg void OnSelchangeAddressAku1();
	afx_msg void OnSelchangeAddressAku2();
	afx_msg void OnSelchangeAddressAku3();
	afx_msg void OnSelchangeAddressAku4();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CInputList*				m_pInputList;
	COutputList*			m_pOutputList;

	// Einige Referenzen zur Vereinfachung
	CComboBox*	m_pcbAddressAku[4];
	CString*	m_psAddressAku[4];
	BOOL*		m_pbAku[4];
	BOOL		m_bRestartAkuUnit;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AKUPAGE_H__B7EE7A25_12F2_11D1_93D9_00C095ECA33E__INCLUDED_)
