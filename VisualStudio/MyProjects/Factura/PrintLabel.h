#if !defined(AFX_PRINTLABEL_H__B024B340_6FA1_11D4_B6EC_0000B458D891__INCLUDED_)
#define AFX_PRINTLABEL_H__B024B340_6FA1_11D4_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PrintLabel.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CPrintLabel 

class CPrintLabel : public CDialog
{
// Konstruktion
public:
	CPrintLabel(CWnd* pParent = NULL);   // Standardkonstruktor
	virtual ~CPrintLabel();

// Dialogfelddaten
	//{{AFX_DATA(CPrintLabel)
	enum { IDD = IDD_PRINT_LABEL };
	CSpinButtonCtrl	m_cSpinCount;
	CEdit	m_cText;
	int		m_nLabelCount;
	int		m_nLength;
	int		m_nLines;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CPrintLabel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CPrintLabel)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSavesettings();
	virtual void OnCancel();
   afx_msg LRESULT OnHelp(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_PRINTLABEL_H__B024B340_6FA1_11D4_B6EC_0000B458D891__INCLUDED_
