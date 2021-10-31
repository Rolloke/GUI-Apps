#if !defined(AFX_PRINTFROMTO_H__799FD4E6_5D2F_42D0_87F1_CE4CB9CB0E3F__INCLUDED_)
#define AFX_PRINTFROMTO_H__799FD4E6_5D2F_42D0_87F1_CE4CB9CB0E3F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PrintFromTo.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CPrintFromTo 

class CPrintFromTo : public CDialog
{
// Konstruktion
public:
	CPrintFromTo(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CPrintFromTo)
	enum { IDD = IDD_PRINT_FROM_TO };
	int		m_nFrom;
	int		m_nTo;
	int		m_nLingeLength;
	BOOL	m_bPrintHidden;
	//}}AFX_DATA
	int		m_nRangeFrom;
	int		m_nRangeTo;


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CPrintFromTo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CPrintFromTo)
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_PRINTFROMTO_H__799FD4E6_5D2F_42D0_87F1_CE4CB9CB0E3F__INCLUDED_
