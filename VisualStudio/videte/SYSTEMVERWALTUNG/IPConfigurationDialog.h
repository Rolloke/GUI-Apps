#pragma once


// CIPConfigurationDialog dialog

class CIPConfigurationDialog : public CDialog
{
	DECLARE_DYNAMIC(CIPConfigurationDialog)

public:
	CIPConfigurationDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CIPConfigurationDialog();

// Dialog Data
	//{{AFX_DATA(CIPConfigurationDialog)
	enum { IDD = IDD_IP_CONFIGURATION };
	CString m_sIPaddress;
	BOOL m_nType;
	int m_iPort;
	//}}AFX_DATA

protected:
	//{{AFX_VIRTUAL(CIPConfigurationDialog)
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CIPConfigurationDialog)
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedIpRdUdp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};
