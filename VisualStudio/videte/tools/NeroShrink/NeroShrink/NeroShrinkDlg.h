// NeroShrinkDlg.h : Headerdatei
//

#pragma once



// CNeroShrinkDlg Dialogfeld
class CNeroShrinkDlg : public CDialog
{
// Konstruktion
public:
	CNeroShrinkDlg(CWnd* pParent = NULL);	// Standardkonstruktor
	CString GetNeroPath(CString sToFind);
	BOOL DoesFileExist(LPCTSTR szFileOrDirectory);
	BOOL CopyNecessaryNeroFiles();
	BOOL DelFiles(CString sdir);

// Dialogfelddaten
	enum { IDD = IDD_NEROSHRINK_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung


// Implementierung
protected:
	HICON m_hIcon;

	// Generierte Funktionen für die Meldungstabellen
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButton1();

	CString m_sErrorText;

};
