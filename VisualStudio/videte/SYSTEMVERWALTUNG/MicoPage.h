#if !defined(AFX_MICOPAGE_H__B7EE7A24_12F2_11D1_93D9_00C095ECA33E__INCLUDED_)
#define AFX_MICOPAGE_H__B7EE7A24_12F2_11D1_93D9_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MiCoPage.h : header file
//

#include "SVPage.h"

/////////////////////////////////////////////////////////////////////////////
class CInputList;
class COutputList;

/////////////////////////////////////////////////////////////////////////////
// CMiCoPage dialog

class CMiCoPage : public CSVPage
{
// Construction
public:
	CMiCoPage(CSVView* pParent, int nMiCoNr);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMiCoPage)
	enum { IDD = IDD_MICO };
	CButton	m_checkUse2Fields;
	CButton		m_radioNtscSquare;
	CButton		m_radioNtscCCIR;
	CButton		m_radioPalSquare;
	CButton		m_radioPalCCIR;
	CButton		m_checkMiCoPCI;
	CButton		m_checkSync;
	CButton		m_checkUseGlobalColorSettings;
	CButton		m_checkExtCard3;
	CButton		m_checkExtCard2;
	CButton		m_checkExtCard1;
	CButton		m_checkMiCoISA;
	CComboBox	m_cbAddressExtCard3;
	CComboBox	m_cbAddressExtCard2;
	CComboBox	m_cbAddressExtCard1;
	CComboBox	m_cbAddressMiCo;
	CComboBox	m_cbIRQ;
	CString		m_sIRQ;
	CString		m_sAddressMiCo;
	CString		m_sAddressExtCard1;
	CString		m_sAddressExtCard2;
	CString		m_sAddressExtCard3;
	BOOL		m_bMiCoISA;
	BOOL		m_bExtCard1;
	BOOL		m_bExtCard2;
	BOOL		m_bExtCard3;
	BOOL		m_bSyncCam;
	BOOL		m_bUseGlobalColorSettings;
	BOOL		m_bMiCoPCI;
	int			m_nRadioVideoFormat;
	BOOL		m_bUse2Fields;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMiCoPage)
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
	void LoadMiCoSettings();
	void SaveMiCoSettings();
	void EnableDlgControlsMiCo();
	void GetDlgMiCoData();

	// Generated message map functions
	//{{AFX_MSG(CMiCoPage)
	afx_msg void OnSelchangeAddressMiCo();
	afx_msg void OnSelchangeAddressExtCard1();
	afx_msg void OnSelchangeAddressExtCard2();
	afx_msg void OnSelchangeAddressExtCard3();
	afx_msg void OnCheckMiCoISA();
	afx_msg void OnCheckExtCard1();
	afx_msg void OnCheckExtCard2();
	afx_msg void OnCheckExtCard3();
	afx_msg void OnSelchangeIrq();
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckSync();
	afx_msg void OnCheckUseGlobalColorSetting();
	afx_msg void OnCheckMicoPCI();
	afx_msg void OnRadioNtscCcir();
	afx_msg void OnRadioNtscSquare();
	afx_msg void OnRadioPalCcir();
	afx_msg void OnRadioPalSquare();
	afx_msg void OnCheckUse2Fields();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CInputList*		m_pInputList;
	COutputList*	m_pOutputList;
	int				m_nMiCoNr;
	int				m_nOtherMiCoNr;
	BOOL			m_bMiCo;

	CString			m_sMiCoSystem;
	CString			m_sMiCoGeneral;
	CString			m_sMiCoFrontend;
	CString			m_sMiCoBackend;
	CString			m_sSM_MiCoOutputCameras;
	CString			m_sSM_MiCoOutputRelays;
	CString			m_sSM_MiCoInput;
	CString			m_sMelderDefaultName;
	CString			m_sCameraDefaultname;
	CString			m_sRelayDefaultName;
	CString			m_sOtherMiCoSystem;
	CString			m_sMiCoFps;

	// Einige Referenzen zur Codevereinfachung
	CComboBox*		m_pcbAddressExtCard[4];
	CString*		m_psAddressExtCard[4];
	BOOL*			m_pbExtCard[4];
	BOOL			m_bRestartMiCoUnit;
	CString			m_sAppName;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MICOPAGE_H__B7EE7A24_12F2_11D1_93D9_00C095ECA33E__INCLUDED_)
