// ExpansionCodeDlg.h : header file
//

#include "afxwin.h"
#if !defined(AFX_EXPANSIONCODEDLG_H__14A282A6_521C_494C_8D15_E71BA0261845__INCLUDED_)
#define AFX_EXPANSIONCODEDLG_H__14A282A6_521C_494C_8D15_E71BA0261845__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CExpansionCodeDlg dialog

class CExpansionCodeDlg : public CDialog
{
// Construction
public:
	CExpansionCodeDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CExpansionCodeDlg)
	enum { IDD = IDD_EXPANSIONCODE_DIALOG };
	CStatic m_Static_Options;
	BOOL	m_bISDN;
	BOOL	m_bLAN;
	BOOL m_bIdipTimer;
	BOOL m_bIdipReset;
	BOOL m_bIdipPTZ;
	BOOL m_bIdipMonitor2;
	BOOL m_bIdipMaps;
	int m_iIdipHost;
	CString	m_sCode;
	CString	m_sSerial;
	BOOL	m_bBackup;
	BOOL	m_bDVD;

	int	m_iNrQCameras;
	int m_iNrIPCameras;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExpansionCodeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

	protected:
		void CalcNewCodeDTSTransmitter();
		void CalcNewCodeIdipReceiver();
		void CalcNewCodeIdipTransmitter();
		void ShowHideDlgItems(int nDlg, BOOL bShow, int nGrpSection, BOOL bHideAll = FALSE);
		DWORD CreateDongleChecksum(DWORD dwDongle);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CExpansionCodeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnCheckIsdn();
	afx_msg void OnCheckLan();
	afx_msg void OnChangeEditSerial();
	afx_msg void OnCheckCdrw();
	afx_msg void OnCheckDvd();
	afx_msg void OnCheckIdipTimer();
	afx_msg void OnCheckIdipPTZ();
	afx_msg void OnCheckIdipDualhead();
	afx_msg void OnCheckIdipMaps();
	afx_msg void OnCheckIdipPLReceiver();
	afx_msg void OnCheckIdipXLReceiver();
	afx_msg void OnCheckDTSTransmitter();
	afx_msg void OnCbnSelchangeComboHost();
	afx_msg void OnBnClickedRadioQCam8();
	afx_msg void OnBnClickedRadioQCam16();
	afx_msg void OnBnClickedOk();
	afx_msg void OnDongleDlgItemRange(UINT);
	afx_msg void OnBnClickedRadioQcam4();
	afx_msg void OnCheckIdipXLTransmitter();
	afx_msg void OnCbnSelchangeDongleComboNrQCameras();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:

private:
	void ShowHideControls(int nType);
	void ShowIdipReceiver();
	void ShowDTSTransmitter();
	void ShowIdipTransmitter();
	void EnableDisableDlgItem(int nDlgID, BOOL bEnable);

public:

	CButton m_ctrl_Idip_Reset;
	int m_iQCams;

	afx_msg void OnCbnSelchangeDongleComboNrIpCameras();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXPANSIONCODEDLG_H__14A282A6_521C_494C_8D15_E71BA0261845__INCLUDED_)
