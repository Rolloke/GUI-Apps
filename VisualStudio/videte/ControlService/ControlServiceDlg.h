// ControlServiceDlg.h : Headerdatei
//

#pragma once

#include "MyService.h"
class CMyService;

// CControlServiceDlg Dialogfeld
class CControlServiceDlg : public CDialog
{
// Konstruktion
public:
	CControlServiceDlg(CMyService* pService, CWnd* pParent = NULL);	// Standardkonstruktor

// Dialogfelddaten
	enum { IDD = IDD_CONTROLSERVICE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung


// Implementierung
protected:
	HICON m_hIcon;
	CMyService* m_pService;

	// Generierte Funktionen für die Meldungstabellen
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedInstallControlService();
	afx_msg void OnBnClickedUninstallControlService();
	afx_msg void OnBnClickedStartControlService();
	afx_msg void OnBnClickedStopControlService();
	afx_msg void OnBnClickedPauseControlService();
	afx_msg void OnBnClickedContinueControlService();
	BOOL UpdateDialog(void);
	// Filename des Programms, welches überwacht werden soll
	CString m_sCtrlApp;
	CString m_sRootRegistryKey;
	afx_msg void OnBnClickedCheckAutoClose();
	BOOL m_bAutoClose;
};
