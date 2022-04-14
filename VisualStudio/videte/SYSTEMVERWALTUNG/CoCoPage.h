#if !defined(AFX_COCOPAGE_H__6C256AE2_1220_11D1_93D9_00C095ECA33E__INCLUDED_)
#define AFX_COCOPAGE_H__6C256AE2_1220_11D1_93D9_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CoCoPage.h : header file
//

#include "SVPage.h"

/////////////////////////////////////////////////////////////////////////////
class CInputList;
class COutputList;
/////////////////////////////////////////////////////////////////////////////
// CCoCoPage dialog

class CCoCoPage : public CSVPage
{
// Construction
public:
	CCoCoPage(CSVView* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCoCoPage)
	enum { IDD = IDD_COCO };
	CComboBox	m_cbIRQCoCo;
	CComboBox	m_cbAddressExtCard3;
	CComboBox	m_cbAddressExtCard2;
	CComboBox	m_cbAddressExtCard1;
	CComboBox	m_cbAddressCoCo;
	CButton	m_checkExtCard3;
	CButton	m_checkExtCard2;
	CButton	m_checkExtCard1;
	CButton	m_checkCoCo;
	BOOL	m_bCoCo;
	BOOL	m_bExtCard1;
	BOOL	m_bExtCard2;
	BOOL	m_bExtCard3;
	CString	m_sIRQCoCo;
	CString	m_sAddressCoCo;
	CString	m_sAddressExtCard1;
	CString	m_sAddressExtCard2;
	CString	m_sAddressExtCard3;
	BOOL	m_bCoCoSoft;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCoCoPage)
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
	void LoadCoCoSettings();
	void SaveCoCoSettings();
	void GetDlgCoCoData();
	void EnableDlgControls();

	// Generated message map functions
	//{{AFX_MSG(CCoCoPage)
	afx_msg void OnCheckCoCo();
	afx_msg void OnCheckExtCard1();
	afx_msg void OnCheckExtCard2();
	afx_msg void OnCheckExtCard3();
	afx_msg void OnSelchangeAddressCoCo();
	afx_msg void OnSelchangeAddressExtCard1();
	afx_msg void OnSelchangeAddressExtCard2();
	afx_msg void OnSelchangeAddressExtCard3();
	afx_msg void OnSelchangeIrqCoCo();
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckCocoSoft();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CInputList*				m_pInputList;
	COutputList*			m_pOutputList;

	// Einige Referenzen für den einfacheren Zugriff.
	CComboBox	*m_pcbAddressExtCard[4];
	CString		*m_psAddressExtCard[4];
	BOOL		*m_pbExtCard[4];
	BOOL		m_bRestartCoCoUnit;
	BOOL		m_bRestartWindows;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COCOPAGE_H__6C256AE2_1220_11D1_93D9_00C095ECA33E__INCLUDED_)
