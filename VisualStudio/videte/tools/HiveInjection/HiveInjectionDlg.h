// HiveInjectionDlg.h : Headerdatei
//

#pragma once
#include "chive.h" 

// Maximal Anzahl der überwachten Verzeichnisse kann hier jederzeit geändert werden.
#define MAX_OBD 255

// CHiveInjectionDlg Dialogfeld
class CObserveDirectory;
class CHiveInjectionDlg : public CDialog
{
// Konstruktion
public:
	CHiveInjectionDlg(CWnd* pParent = NULL);	// Standardkonstruktor

// Dialogfelddaten
	enum { IDD = IDD_HIVEINJECTION_DIALOG };

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
	afx_msg long OnTrayClicked(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
		CString InitObservedDirectoryValues(CProfile &prof, LPCTSTR strProg, LPCTSTR strCommand, bool bForce/*=false*/);

public:
	afx_msg void OnBnClickedBackup();
	afx_msg void OnBnClickedRestore();
	afx_msg void OnDestroy();

private:
	CHive	m_hive;
	CObserveDirectory* m_pObDir[MAX_OBD];
	int		m_nCountOBDs;

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
