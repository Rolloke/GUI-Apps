#pragma once


#include "SVPage.h"

/////////////////////////////////////////////////////////////////////////////
class CInputList;
class COutputList;
/////////////////////////////////////////////////////////////////////////////
// CTashaPage dialog

class CTashaPage : public CSVPage
{
	DECLARE_DYNAMIC(CTashaPage)

public:
	CTashaPage(CSVView* pParent, int nTashaNr);   // standard constructor
	virtual ~CTashaPage();

// Dialog Data
	enum { IDD = IDD_TASHA };
	CButton m_checkTasha;
	BOOL m_bTasha;
	BOOL m_bNR;
	int	 m_iVideoNorm;
	CButton m_checkNR;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Overrides
protected:
	virtual BOOL OnInitDialog();
	virtual BOOL IsDataValid();
	virtual void SaveChanges();
	virtual void CancelChanges();
	virtual void SetModified(BOOL bModified = TRUE ,BOOL bServerInit = TRUE);
	virtual void OnIdleSVPage();

	// Implementation
protected:
	void LoadTashaSettings();
	void SaveTashaSettings();
	void EnableDlgControlsTasha();
	void GetDlgTashaData();

	DECLARE_MESSAGE_MAP()


public:

	afx_msg void OnBnClickedCheckTasha();
	afx_msg void OnBnClickedCheckNr();

private:
	CInputList*		m_pInputList;
	COutputList*	m_pOutputList;
	int				m_nTashaNr;
	CString			m_sTashaSystem;
	CString			m_sTashaGeneral;
	CString			m_sTashaMD;
	CString			m_sTashaFrontend;
	CString			m_sTashaBackend;
	CString			m_sSM_TashaOutputCameras;
	CString			m_sSM_TashaOutputRelays;
	CString			m_sSM_TashaInput;
	CString			m_sSM_TashaInputMD;
	CString			m_sOtherTashaSystem;
	CString			m_sTashaFps;

	BOOL			m_bRestartTashaUnit;
	CString			m_sAppName;
public:
};
