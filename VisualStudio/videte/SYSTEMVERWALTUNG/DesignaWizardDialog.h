#pragma once
#include "afxwin.h"


// CDesignaWizardDialog dialog

class CDesignaWizardDialog : public CDialog
{
	DECLARE_DYNAMIC(CDesignaWizardDialog)

public:
	CDesignaWizardDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDesignaWizardDialog();

// Dialog Data
	enum { IDD = IDD_WIZ_DESIGNA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	static void SetCardreaderBarrier(CDWordArray& alarm,CDWordArray& xml);
	static void SetCashBox(CDWordArray& alarm,CDWordArray& xml);
	static BOOL Find(const CDWordArray& a, DWORD dw);

public:
	CEdit m_editBFR;
	CEdit m_editTCC;
	CCheckListBox m_listAlarm;
	CCheckListBox m_listXML;

public:
	SDIControlType m_SDICT;
	CString m_sCameraName;
	DWORD m_dwBFR;
	DWORD m_dwTCC;
	CDWordArray m_PM100Alarm;
	CDWordArray m_PM100XML;
	CComboBox m_comboType;
	afx_msg void OnCbnSelchangeComboType();
};
