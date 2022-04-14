// DetermineIPAddressDlg.h : Headerdatei
//

#pragma once
#include "afxwin.h"


// CDetermineIPAddressDlg Dialogfeld
class CDetermineIPAddressDlg : public CDialog
{
// Konstruktion
public:
	CDetermineIPAddressDlg(CWnd* pParent = NULL);	// Standardkonstruktor

// Dialogfelddaten
	enum { IDD = IDD_DETERMINEIPADDRESS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung

	void	OutputString(LPCTSTR lpszFormat, ...);
	void	OutputLastError(LPCTSTR lpszError, DWORD dwError=0);
	BOOL	GetDefaultGateway(CString& sGateway);
	void	EnableControls();

// Implementierung
protected:
	HICON m_hIcon;
	CEdit m_Edit1;
	CString m_sCompAndDnsName;
	CString m_sIPAddress;
	CString m_sSubnetMask;
	BOOL m_bEnableDHCP;
	CString m_sAdapterKey;
	CString m_sIPAddressDHCPserver;
	CWKControlledProcess m_Cmd;
	int		m_nIPAdapterIndex;
	CDWordArray m_dwaNTEContexts;

	// Generierte Funktionen für die Meldungstabellen
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedDetIp();
	afx_msg void OnBnClickedDetDhcp();
	afx_msg void OnBnClickedDetClearTxt();
	afx_msg void OnBnClickedBtnSetNames();
	afx_msg void OnBnClickedBtnTest();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedBtnSetAdapterParams();
	afx_msg void OnBnClickedCkEnableDhcp();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedBtnTest2();
	afx_msg void OnBnClickedBtnTest3();
};
