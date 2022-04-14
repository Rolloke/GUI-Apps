// HardwareInfoDlg.h : Headerdatei
//

#pragma once
#include "afxwin.h"


// CHardwareInfoDlg Dialogfeld
class CHardwareInfoDlg : public CDialog
{
// Konstruktion
public:
	CHardwareInfoDlg(CWnd* pParent = NULL);	// Standardkonstruktor
	virtual ~CHardwareInfoDlg();	// Standarddestruktor

	void DebugOutput(TCHAR *tszErr, ...);

// Dialogfelddaten
	enum { IDD = IDD_HARDWAREINFO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung


// Implementierung
protected:
	HICON m_hIcon;

	// Generierte Funktionen für die Meldungstabellen
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

    IWbemLocator *m_pLocator;
    IWbemServices *m_pServices;

public:
	CEdit m_edtOut;
};
