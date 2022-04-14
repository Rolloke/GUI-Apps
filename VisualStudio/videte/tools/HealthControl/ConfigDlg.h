#pragma once
#include "afxwin.h"


// CConfigDlg dialog
#define TYPE_INT 0
#define TYPE_FLOAT 1


class CConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(CConfigDlg)

public:
	CConfigDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CConfigDlg();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfigDlg)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Dialog Data
public:
	//{{AFX_DATA(CConfigDlg)
	enum { IDD = IDD_DEVICE_MIN_MAX };
	CComboBox m_cDevices;
	int		m_nDeviceNum;
	BOOL*	m_pbSendRTE;
	int*	m_pnDevice;
	int*	m_pnMinValue;
	int*	m_pnMaxValue;
	float*	m_pfMinValue;
	float*	m_pfMaxValue;
	//}}AFX_DATA

	CString m_sValue;
	CString m_sUnit;

protected:
	// Generierte Funktionen für die Meldungstabellen
	//{{AFX_MSG(CConfigDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};
