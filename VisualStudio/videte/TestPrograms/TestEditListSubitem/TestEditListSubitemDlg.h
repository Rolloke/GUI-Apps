// TestEditListSubitemDlg.h : Headerdatei
//

#pragma once
#include "afxcmn.h"
#include "EdtSubItemListCtrl.h"


// CTestEditListSubitemDlg Dialogfeld
class CTestEditListSubitemDlg : public CDialog
{
// Konstruktion
public:
	CTestEditListSubitemDlg(CWnd* pParent = NULL);	// Standardkonstruktor

// Dialogfelddaten
	enum { IDD = IDD_TESTEDITLISTSUBITEM_DIALOG };

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
public:
	CEdtSubItemListCtrl m_List;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtnOptimizeColumnWidth();
};
