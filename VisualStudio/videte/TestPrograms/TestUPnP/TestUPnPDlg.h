// TestUPnPDlg.h : Headerdatei
//

#pragma once
#include "afxwin.h"


// CTestUPnPDlg Dialogfeld
class CTestUPnPDlg : public CDialog
{
// Konstruktion
public:
	CTestUPnPDlg(CWnd* pParent = NULL);	// Standardkonstruktor

// Dialogfelddaten
public:
	enum { IDD = IDD_TESTUPNP_DIALOG };
protected:
	CComboBox m_cComboSearch;
	CEdit m_cReport;
	CString m_sSearch;
private:
	CWinThread*m_pUpnpThread;

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
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtnClear();
	DECLARE_MESSAGE_MAP()


	void ReportError(ErrorStruct*ps, BOOL bMsgBox);
	static UINT CALLBACK UpnpThread(void*);

public:
};
