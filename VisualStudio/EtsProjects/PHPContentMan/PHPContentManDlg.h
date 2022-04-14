// PHPContentManDlg.h : Header-Datei
//

#if !defined(AFX_PHPCONTENTMANDLG_H__810384FC_021E_4439_BBB1_83BFB4CC0C0E__INCLUDED_)
#define AFX_PHPCONTENTMANDLG_H__810384FC_021E_4439_BBB1_83BFB4CC0C0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPHPContentManDlg Dialogfeld

class CPHPContentManDlg : public CDialog
{
// Konstruktion
public:
	CPHPContentManDlg(CWnd* pParent = NULL);	// Standard-Konstruktor

// Dialogfelddaten
	//{{AFX_DATA(CPHPContentManDlg)
	enum { IDD = IDD_PHPCONTENTMAN_DIALOG };
		// HINWEIS: der Klassenassistent fügt an dieser Stelle Datenelemente (Members) ein
	//}}AFX_DATA

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CPHPContentManDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	HICON m_hIcon;

	// Generierte Message-Map-Funktionen
	//{{AFX_MSG(CPHPContentManDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_PHPCONTENTMANDLG_H__810384FC_021E_4439_BBB1_83BFB4CC0C0E__INCLUDED_)
