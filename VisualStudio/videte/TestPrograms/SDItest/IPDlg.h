#pragma once


// CIPDlg dialog

class CIPDlg : public CDialog
{
	DECLARE_DYNAMIC(CIPDlg)

public:
	CIPDlg(UINT iIPPort, CString sAddress, UINT iIPType);   // standard constructor
	virtual ~CIPDlg();

// Dialog Data
	enum { IDD = IDD_CONFIG_IP };
	UINT m_iIPPort;
	UINT m_iIPType;
	CString m_sIPAddress;
	CComboBox m_cbType;
	CIPAddressCtrl m_ipCtrl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
