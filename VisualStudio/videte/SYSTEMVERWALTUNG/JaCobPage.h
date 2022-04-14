#if !defined(AFX_JACOBPAGE_H__B7EE7A24_12F2_11D1_93D9_00C095ECA33E__INCLUDED_)
#define AFX_JACOBPAGE_H__B7EE7A24_12F2_11D1_93D9_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// JaCobPage.h : header file
//

#include "SVPage.h"

/////////////////////////////////////////////////////////////////////////////
class CInputList;
class COutputList;

/////////////////////////////////////////////////////////////////////////////
// CJaCobPage dialog

class CJaCobPage : public CSVPage
{
// Construction
public:
	CJaCobPage(CSVView* pParent, int nJaCobNr);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CJaCobPage)
	enum { IDD = IDD_JACOB };
	CButton		m_checkMDOn;
	CButton		m_checkJaCob;
	CButton		m_checkUse2Fields;
	CButton		m_checkSync;
	CButton		m_checkUseGlobalColorSettings;
	BOOL		m_bSyncCam;
	BOOL		m_bUseGlobalColorSettings;
	BOOL		m_bJaCob;
	BOOL		m_bUse2Fields;
	BOOL		m_bMDOn;
	int		m_iVideoNorm;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJaCobPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Overrides
protected:
	virtual BOOL IsDataValid();
	virtual void SaveChanges();
	virtual void CancelChanges();
	virtual void SetModified(BOOL bModified = TRUE ,BOOL bServerInit = TRUE);
	virtual void OnIdleSVPage();

	// Implementation
protected:
	void LoadJaCobSettings();
	void SaveJaCobSettings();
	void EnableDlgControlsJaCob();
	void GetDlgJaCobData();

	// Generated message map functions
	//{{AFX_MSG(CJaCobPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckSync();
	afx_msg void OnCheckUseGlobalColorSetting();
	afx_msg void OnCheckJaCob();
	afx_msg void OnCheckUse2Fields();
	afx_msg void OnMdConfig();
	afx_msg void OnCheckMdOn();
	afx_msg void OnRadioAuto();
	afx_msg void OnRadioNtsc();
	afx_msg void OnRadioPal();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CInputList*		m_pInputList;
	COutputList*	m_pOutputList;
	int				m_nJaCobNr;
	//int				m_nOtherJaCobNr;

	CString			m_sJaCobSystem;
	CString			m_sJaCobGeneral;
	CString			m_sJaCobMD;
	CString			m_sJaCobFrontend;
	CString			m_sJaCobBackend;
	CString			m_sSM_JaCobOutputCameras;
	CString			m_sSM_JaCobOutputRelays;
	CString			m_sSM_JaCobInput;
	CString			m_sSM_JaCobInputMD;
	CString			m_sOtherJaCobSystem;
	CString			m_sJaCobFps;

	BOOL			m_bRestartJaCobUnit;
	CString			m_sAppName;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JACOBPAGE_H__B7EE7A24_12F2_11D1_93D9_00C095ECA33E__INCLUDED_)
