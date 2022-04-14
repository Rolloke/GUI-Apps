// StandardEntriesDlg.h : header file
//
#if !defined(AFX_STANDARDENTRIESDLG_H__A40F9783_CD57_11D2_B081_004005A1D890__INCLUDED_)
#define AFX_STANDARDENTRIESDLG_H__A40F9783_CD57_11D2_B081_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
class CSDITestDoc;
class CSDITestView;

/////////////////////////////////////////////////////////////////////////////
// CStandardEntriesDlg dialog
class CStandardEntriesDlg : public CWK_Dialog
{
// Construction
public:
	CStandardEntriesDlg(CSDITestDoc* pDocument, CSDITestView* pView);   // standard constructor

// Attributes
	inline CSDITestDoc* GetDocument();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStandardEntriesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
public:
	void	InitializeData();
	void	ShowHide();
protected:

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CStandardEntriesDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnCheckBitError();
	afx_msg void OnCheckLimited();
	afx_msg void OnCheckRepeat();
	afx_msg void OnKillfocusEditLimited();
	afx_msg void OnKillfocusEditRepeatTime();
	afx_msg void OnRadioRepeatHow();
	afx_msg void OnCheckBlz();
	afx_msg void OnCheckKonto();
	afx_msg void OnKillfocusEditBlz();
	afx_msg void OnKillfocusEditKonto();
	afx_msg void OnCheckDate();
	afx_msg void OnKillfocusEditDate();
	afx_msg void OnCheckTime();
	afx_msg void OnKillfocusEditTime();
	afx_msg void OnChangeEditRepeatTime();
	afx_msg void OnChangeEditLimited();
	afx_msg void OnChangeEditBlz();
	afx_msg void OnChangeEditKonto();
	afx_msg void OnChangeEditTime();
	afx_msg void OnChangeEditDate();
	afx_msg void OnCheckBetrag();
	afx_msg void OnChangeEditBetrag();
	afx_msg void OnCheckTAN();
	afx_msg void OnChangeEditTAN();
	afx_msg void OnCheckCurrency();
	afx_msg void OnCheckWsid();
	afx_msg void OnEditchangeComboCurrency();
	afx_msg void OnSelchangeComboCurrency();
	afx_msg void OnChangeEditWsid();
	afx_msg void OnKillfocusEditWsid();
	afx_msg void OnKillfocusComboCurrency();
	afx_msg void OnKillfocusEditTan();
	afx_msg void OnCheckSendSingleByte();
	afx_msg void OnChangeEditSendDelay();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dialog Data
	//{{AFX_DATA(CStandardEntriesDlg)
	enum { IDD = IDD_STANDARD };
	CEdit	m_editSendDelay;
	CButton	m_btnSendSingleBytes;
	CEdit	m_editWSID;
	CButton	m_btnWSID;
	CComboBox	m_cbCurrency;
	CButton	m_btnCurrency;
	CEdit	m_editTime;
	CEdit	m_editDate;
	CButton	m_btnTime;
	CButton	m_btnDate;
	CButton	m_btnRepeat;
	CEdit	m_editRepeatTime;
	CButton	m_btnChosenUnits;
	CButton	m_btnRandom;
	CButton	m_btnSuccessive;
	CButton	m_btnLimited;
	CEdit	m_editLimited;
	CButton	m_btnBitError;
	CButton	m_btnKonto;
	CEdit	m_editKonto;
	CButton	m_btnBLZ;
	CEdit	m_editBLZ;
	CEdit	m_editTAN;
	CButton	m_btnTAN;
	CButton	m_btnBetrag;
	CEdit	m_editBetrag;
	BOOL	m_bRepeat;
	UINT	m_uRepeatTime;
	int		m_iRepeatHow;
	BOOL	m_bLimited;
	UINT	m_uLimited;
	BOOL	m_bBitError;
	BOOL	m_bBLZ;
	BOOL	m_bKonto;
	CString	m_sBLZ;
	CString	m_sKonto;
	BOOL	m_bDate;
	BOOL	m_bTime;
	CString	m_sDate;
	CString	m_sTime;
	BOOL	m_bBetrag;
	CString	m_sBetrag;
	BOOL	m_bTAN;
	CString	m_sTAN;
	BOOL	m_bCurrency;
	CString	m_sCurrency;
	BOOL	m_bWSID;
	CString	m_sWSID;
	BOOL	m_bSendSingleBytes;
	UINT	m_uSendDelay;
	//}}AFX_DATA
	CSDITestDoc*	m_pDocument;
	CSDITestView*	m_pView;
};
/////////////////////////////////////////////////////////////////////////////
inline CSDITestDoc* CStandardEntriesDlg::GetDocument()
{
	return (CSDITestDoc*)m_pDocument;
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STANDARDENTRIESDLG_H__A40F9783_CD57_11D2_B081_004005A1D890__INCLUDED_)
