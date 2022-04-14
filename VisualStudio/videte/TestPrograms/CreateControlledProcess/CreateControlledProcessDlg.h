// CreateControlledProcessDlg.h : Headerdatei
//

#pragma once
#include "afxwin.h"

class CWKControlledProcess;
//////////////////////////////////////////////////////////////////////
// CCreateControlledProcessDlg Dialogfeld
class CCreateControlledProcessDlg : public CDialog
{
// Konstruktion
public:
	CCreateControlledProcessDlg(CWnd* pParent = NULL);	// Standardkonstruktor
	virtual ~CCreateControlledProcessDlg();	// Standarddestruktor

// Dialogfelddaten
	enum { IDD = IDD_CREATECONTROLLEDPROCESS_DIALOG };
protected:
	CButton m_btnSendCmd;
	CEdit m_edtSendCmd;
	CEdit m_edtOutput;

	CString m_sProcessName;
	CString m_sCmdLine;
	CString m_sSendCmd;

	BOOL m_bTerminate;
	BOOL m_bOutput;
	BOOL m_bInput;
	BOOL m_bInputReturn;
	BOOL m_bTestOutput;
	BOOL m_bShowWindow;
   UINT m_nKeepAliveTimer;
	CWKControlledProcess *m_pProcess;
    CString m_sOldText;

#if _TEST_DISK_PARTITION == 1
	CStringArray m_saFreeDrives,
				 m_saAssigned,
				 m_saDisks,
				 m_saPartitions;
	CString m_sPartition, 
			m_sExtended;
	int m_nDisks;
	int m_nLinesCounted;
#endif

	CFont m_FixedFont;
	HICON m_hIcon;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung

// Implementierung
protected:
	void SetOutputString(LPCTSTR sOut, BOOL bCR=TRUE);

	// Generierte Funktionen für die Meldungstabellen
	virtual BOOL OnInitDialog();
	//{{AFX_MSG(CCreateControlledProcessDlg)
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedBtnSendCmd();
	afx_msg void OnBnClickedTerminate();
	afx_msg void OnBnClickedCkInput();
	afx_msg void OnBnClickedBtnClear();
	afx_msg long OnTrayClicked(WPARAM wParam, LPARAM lParam);
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};
