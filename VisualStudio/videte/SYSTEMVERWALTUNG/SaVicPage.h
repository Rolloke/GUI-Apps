#if !defined(AFX_SAVICPAGE_H__B7EC7A24_12F2_11D1_93D9_02C095ECA38E__INCLUDED_)
#define AFX_SAVICPAGE_H__B7EC7A24_12F2_11D1_93D9_02C095ECA38E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SaVicPage.h : header file
//

#include "SVPage.h"

/////////////////////////////////////////////////////////////////////////////
class CInputList;
class COutputList;

/////////////////////////////////////////////////////////////////////////////
// CSaVicPage dialog

class CSaVicPage : public CSVPage
{
// Construction
public:
	CSaVicPage(CSVView* pParent, int nSaVicNr);   // standard constructor

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSaVicPage)
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
	void LoadSaVicSettings();
	void SaveSaVicSettings();
	void EnableDlgControlsSaVic();
	void GetDlgSaVicData();

	// Generated message map functions
	//{{AFX_MSG(CSaVicPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckSync();
	afx_msg void OnCheckUseGlobalColorSetting();
	afx_msg void OnCheckSaVic();
	afx_msg void OnCheckUse2Fields();
	afx_msg void OnMdConfig();
	afx_msg void OnRadioVideoNorm();
	afx_msg void OnRadioResolution();
	afx_msg void OnCheckNr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	enum EnumResolution
	{
		SAVIC_RESOLUTION_HIGH = 0,
		SAVIC_RESOLUTION_LOW = 1,
	};

// Dialog Data
	//{{AFX_DATA(CSaVicPage)
	enum { IDD = IDD_SAVIC };
	CButton	m_checkNR;
	CButton		m_checkSaVic;
	CButton		m_checkUse2Fields;
	CButton		m_checkSync;
	CButton		m_checkUseGlobalColorSettings;
	BOOL		m_bSyncCam;
	BOOL		m_bUseGlobalColorSettings;
	BOOL		m_bSaVic;
	BOOL		m_bUse2Fields;
	int			m_iVideoNorm;
	int			m_iResolution;
	BOOL	m_bNR;
	//}}AFX_DATA

	CInputList*		m_pInputList;
	COutputList*	m_pOutputList;
	int				m_nSaVicNr;
	//int				m_nOtherSaVicNr;

	CString			m_sSaVicSystem;
	CString			m_sSaVicGeneral;
	CString			m_sSaVicMD;
	CString			m_sSaVicFrontend;
	CString			m_sSaVicBackend;
	CString			m_sSM_SaVicOutputCameras;
	CString			m_sSM_SaVicOutputRelays;
	CString			m_sSM_SaVicInput;
	CString			m_sSM_SaVicInputMD;
	CString			m_sOtherSaVicSystem;
	CString			m_sSaVicFps;

	BOOL			m_bRestartSaVicUnit;
	CString			m_sAppName;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAVICPAGE_H__B7EC7A24_12F2_11D1_93D9_02C095ECA38E__INCLUDED_)
