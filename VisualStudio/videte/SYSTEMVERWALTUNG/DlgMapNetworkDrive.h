#pragma once
#include "afxwin.h"


// CDlgMapNetworkDrive dialog


class CDlgMapNetworkDrive : public CWK_Dialog
{
	DECLARE_DYNAMIC(CDlgMapNetworkDrive)

public:
	CDlgMapNetworkDrive(CWK_Profile&wkp, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgMapNetworkDrive();
	void SetVolume(CString&sVolume);
	void SetRoot(CString&sRoot);
	CIPCDrive*GetNewDrive(BOOL bDetach=FALSE);

// Dialog Data
private:
	enum { IDD = IDD_MAP_NETWORK_DRIVE };
	CComboBox m_cDriveLetters;
	CString m_sPath;
	CString m_sDomain;
	CString m_sUser;
	CString m_sPassword;

	CWK_Profile *m_pProfile;
	CString	m_sRoot;
	CIPCDrive *m_pNewDrive;

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void OnBnClickedOk();
	afx_msg void OnChange();
	DECLARE_MESSAGE_MAP()

private:

};
