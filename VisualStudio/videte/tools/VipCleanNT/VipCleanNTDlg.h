// VipCleanNTDlg.h : header file
//

#if !defined(AFX_VIPCLEANNTDLG_H__C08581A9_FFC4_11D3_A9D7_004005A26A3B__INCLUDED_)
#define AFX_VIPCLEANNTDLG_H__C08581A9_FFC4_11D3_A9D7_004005A26A3B__INCLUDED_

#include "VipCleanReg.h"	// Added by ClassView
#include "afxwin.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CVipCleanNTDlg dialog

enum OS_TYPE
{
		OS_NOT	= 0,
		OS_95	= 1,
		OS_98	= 2,
		OS_ME	= 3,
		OS_NT	= 4,
		OS_2K	= 5,
		OS_XP	= 6,
};

class CVipCleanNTDlg : public CDialog
{
// Construction
public:
	CString m_scam;
	void OnRadio16kam();
	void OnRadio8kam();
	void OnRadio4kam();
	CVipCleanNTDlg(CWnd* pParent = NULL);	// standard constructor
	BOOL IsOSVersionXP();
	void CheckResetMsgBox();

// Dialog Data
	//{{AFX_DATA(CVipCleanNTDlg)
	enum { IDD = IDD_VIPCLEANNT_DIALOG };
	CStatic	m_StaticKam16_Tasha;
	CStatic	m_StaticKam8_Tasha;
	CButton	m_ctrlCheckDVD;
	CButton	m_ctrlCheckBackup;
	CStatic	m_StaticNero;
	CEdit	m_CtrlNero6;
	CEdit	m_CtrlNero5;
	CEdit	m_CtrlNero4;
	CEdit	m_CtrlNero3;
	CEdit	m_CtrlNero2;
	CEdit	m_CtrlNero1;
	CStatic	m_StaticXPeLicense;
	CEdit	m_CtrlXPeLicense;
	CEdit	m_EditTemp;
	CStatic	m_StaticTemp;
	CStatic	m_StaticKam16;
	CStatic	m_StaticKam8;
	CStatic	m_StaticKam4;
	CStatic	m_StaticOEM2;
	CStatic	m_StaticOEM1;
	CComboBox	m_editOrgaCombo;
	CComboBox	m_editOrgaPullDown;
	CStatic	m_CtrlPingOK;
	CStatic	m_CtrlPingNotYet;
	CIPAddressCtrl	m_IP;
	CEdit	m_LoaderTimeout;
	CEdit	m_asshell;
	CEdit	m_Hostname;
	CButton	m_writeReg;
	CEdit	m_editOEM3;
	CEdit	m_editOEM2;
	CEdit	m_editOEM1;
	CString	m_sOEM1;
	CString	m_sOEM2;
	CString	m_sOEM3;
	CString	m_shostname;
	CString	m_del;
	CButton	m_Radio4;
	CButton	m_Radio8;
	CButton	m_Radio16;
	int		m_iLoaderTimeout;
	CString	m_sScreen;
	CString	m_sTempDir;
	BOOL	m_bISDN;
	BOOL	m_bNet;
	BOOL	m_bBackup;
	CString	m_sHz;
	CString	m_sOrgaPullDown;
	CString	m_sOrgaCombo;
	CString	m_sXPeLicense;
	CString	m_sNero2;
	CString	m_sNero3;
	CString	m_sNero4;
	CString	m_sNero5;
	CString	m_sNero6;
	CString	m_sDisplay;
	CString	m_sNero1;
	BOOL	m_bBackupDVD;
	int	    m_iNrOfAllowedQUnitCams;
	CString	m_sStaticNeroVersion;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVipCleanNTDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CVipCleanNTDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnKillfocusOem1();
	afx_msg void OnKillfocusOem2();
	afx_msg void OnKillfocusOem3();
	afx_msg void OnWriteReg();
	afx_msg void OnChangeOem1();
	afx_msg void OnChangeOem2();
	afx_msg void OnChangeOem3();
	afx_msg void OnChangeHostname();
	afx_msg void OnChangeOrga();
	afx_msg void OnDeldir();
	afx_msg void OnChangeLoadertimeout();
	afx_msg void OnKillfocusLoadertimeout();
	afx_msg void OnSetfocusOem1();
	afx_msg void OnSetfocusOem2();
	afx_msg void OnSetfocusOem3();
	afx_msg void OnKillfocusOrganisation();
	virtual void OnOK();
	afx_msg void OnOk2();
	afx_msg void OnCheckIsdn();
	afx_msg void OnCheckNet();
	afx_msg void OnCheckBackup();
	afx_msg void OnPing();
	afx_msg void OnFrequence();
	afx_msg void OnKillfocusOrgaCombo();
	afx_msg void OnSelchangeOrgaCombo();
	afx_msg void OnChangeEditXpe();
	afx_msg void OnKillfocusNero1();
	afx_msg void OnChangeNero2();
	afx_msg void OnKillfocusNero2();
	afx_msg void OnKillfocusNero3();
	afx_msg void OnChangeNero3();
	afx_msg void OnChangeNero4();
	afx_msg void OnKillfocusNero4();
	afx_msg void OnChangeNero5();
	afx_msg void OnKillfocusNero5();
	afx_msg void OnChangeNero6();
	afx_msg void OnKillfocusNero6();
	afx_msg void OnChangeNero1();
	afx_msg void OnCheckDvd();
	afx_msg void OnCkAudioTest();
	afx_msg void OnBtnAudioVolumes();
	afx_msg void OnStnClickedKam8();
	afx_msg void OnBnClickedBtnCfInit();
	afx_msg void OnBnClickedBtnCfUninit();
	afx_msg void OnBnClickedBtnInitDisks();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bRegAktual;
	BOOL m_bDirDeleted;
	int m_iCamCount;
	CString m_sOEMMid;
	void GetDataFromReg();
	void GetXPDataFromReg();
	void InitXPButtons();
	CVipCleanReg m_Reg;
	DEVMODE m_lpDevMode; //beinhaltet alle Monitoreinstellungen
	BOOL m_bSaveAsSerialNumberKey;

	//liesst alle MOnitoreinstellungen aus der Registry aus
	void GetMonitorConfigurations();
	BOOL m_bIsXP;
	DWORD m_iNrOfJacobs;
	DWORD m_iNrOfSavics;
	DWORD m_iNrOfTashas;
	DWORD m_iNrOfQs;
	CString	m_sNeroLicense; //gesamt string der Nero Lizenz aus Registry
	int		m_iNeroVersion;
	BOOL m_bResetMsgBox;
public:
private:
	CStatic m_ctrlComPort_OK;
	CStatic m_ctrlComPort_NotOK;
public:
	CStatic m_Static_Kam_QUnit;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIPCLEANNTDLG_H__C08581A9_FFC4_11D3_A9D7_004005A26A3B__INCLUDED_)
