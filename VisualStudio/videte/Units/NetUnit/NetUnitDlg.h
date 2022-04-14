// NetUnitDlg.h : header file
//

#if !defined(AFX_NETUNITDLG_H__ABCDD07C_5A48_11D5_8700_004005A26A3B__INCLUDED_)
#define AFX_NETUNITDLG_H__ABCDD07C_5A48_11D5_8700_004005A26A3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CNetUnitDlg dialog


#include "NetworkDetailsDlg.h"
#include "afxwin.h"
#include "nfc\IPBook.h"

class CNetworkDetailsDlg;
class CIPCControlInterface;
class CIPBook;

class CNetUnitDlg : public CDialog
{
//Konstruktor / Destruktor/////////////////////////////////////////////////////////////////////////
public:
	CNetUnitDlg(CWnd* pParent = NULL);	// standard constructor
	~CNetUnitDlg();

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNetUnitDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

//Methoden/////////////////////////////////////////////////////////////////////////////////////////
// Implementation
private : 
	void OnCloseAllConnections();
	BOOL LoadHostListFromRegistry();
	BOOL SaveHostListToRegistry();
	BOOL SaveChanges();
	void SetSelection(int nLine, COLORREF colRGB);
	BOOL CheckLine(int nLine);
	BOOL CheckIPList();
	BOOL IsHostInHostList(const CString &sHost);
	BOOL CheckIPSyntax(const CString &sHost);
	BOOL ChangeExistingDUNEntry();
	BOOL CreateNewDUNEntry();
	void EnDisableAllDialogMembers(BOOL state);
	BOOL InitDialogMembers();
	BOOL ChangeDUN();

protected:
	// Generated message map functions
	//{{AFX_MSG(CNetUnitDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnAppAbout();
	afx_msg void OnChangeRichEditIPList();
	afx_msg void OnChangeNetUnitParams();
	afx_msg void OnButtonReinit();
	afx_msg void OnButtonDetails();
	afx_msg void OnDblclkListDuns();
	afx_msg void OnSelchangeListDuns();
	afx_msg void OnButtonNewDUNViaAssist();
	afx_msg void OnButtonNewDUN();
	afx_msg void OnButtonDeleteDUN();
	afx_msg void OnButtonCancelNewDUN();
	afx_msg void OnButtonChangeDUN();
	afx_msg void OnChangeEditDUNName();
	afx_msg void OnChangeEditUsername();
	afx_msg void OnChangeEditPassword();
	afx_msg void OnChangeEditTelnum();
	afx_msg void OnSelchangeComboDundevice();
	afx_msg void OnIplistClicked();
	afx_msg void OnCopyTcpToList();
	afx_msg LRESULT DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonNetworkdetails();
	afx_msg LRESULT OnTrayClicked(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLanguageChanged(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

//Attribute////////////////////////////////////////////////////////////////////////////////////////
// Dialog Data
public:
	HICON					m_hIcon;
	CString					m_strDUNDeviceName;
	BOOL					m_bIsNewDUNEntry;
	CIPCControlInterface	*m_pCIPCControlInterface;
	CIPBook					*m_pIPBook[10];
	CButton					m_buttonNetworkDetails;		// Button zur Anzeige der Netzwerkdetails

private: 
	//{{AFX_DATA(CNetUnitDlg)
	enum { IDD = IDD_NETUNIT_DIALOG };
	CButton	m_buttonFirewall;
	CRichEditCtrl	m_ctrlRichEditIPHostList;
	CButton	m_buttonChangeDUN;
	CButton	m_buttonCancelNewDUN;
	CListBox	m_listBoxDUNs;
	CEdit	m_editTelnum;
	CEdit	m_editPassword;
	CEdit	m_editUserName;
	CEdit	m_editDUNName;
	CButton	m_buttonIPList;
	CEdit	m_editSync;
	CButton	m_buttonDoBeep;
	CComboBox	m_comboBoxDUNDevice;
	CComboBox	m_comboBoxBitrate;
	CButton	m_buttonNewDUN;
	CButton	m_buttonDUNDetails;
	CButton	m_buttonDeleteDUN;
	CButton	m_buttonAllowOutgoingCalls;
	CButton	m_buttonAllowIncomingCalls;
	CButton	m_buttonClose;
	CButton	m_buttonReinit;
	BOOL	m_bAllowIncomingCalls;
	BOOL	m_bDoBeep;
	BOOL	m_bAllowOutgoingCalls;
	BOOL	m_bUseHostList;
	CString	m_strDUNName;
	CString	m_strUserName;
	CString	m_strPassword;
	CString	m_strTelnum;
	CString	m_strSync;
	//}}AFX_DATA
public:
	afx_msg void OnSettings();
	afx_msg void OnUpdateSettings(CCmdUI *pCmdUI);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NETUNITDLG_H__ABCDD07C_5A48_11D5_8700_004005A26A3B__INCLUDED_)
