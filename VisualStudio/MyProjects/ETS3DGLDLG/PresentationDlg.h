#if !defined(AFX_PRESENTATIONDLG_H__993A0460_8AE0_11D3_B6EC_0000B458D891__INCLUDED_)
#define AFX_PRESENTATIONDLG_H__993A0460_8AE0_11D3_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PresentationDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CPresentationDlg 

class CPresentationDlg : public CDialog
{
// Konstruktion
public:
	~CPresentationDlg();
	CPresentationDlg(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CPresentationDlg)
	enum { IDD = IDD_PRESENTATION_DLG };
	BOOL	   m_b2DVMenuBmp;
	int		m_n2DVMenuScale;
	BOOL	   m_b3DVMenuBmp;
	int		m_n3DVMenuScale;
	BOOL	   m_bFillPath;
	int		m_n2DVRecentFiles;
	int		m_n3DVRecentFiles;
	float	m_fPrinterOffsetX;
	float	m_fPrinterOffsetY;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CPresentationDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	HICON m_hIcon;

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CPresentationDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_PRESENTATIONDLG_H__993A0460_8AE0_11D3_B6EC_0000B458D891__INCLUDED_
