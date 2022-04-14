#ifndef AFX_CATCHGRIDDLG_H__B5540EE2_2DD3_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_CATCHGRIDDLG_H__B5540EE2_2DD3_11D2_9DB9_0000B458D891__INCLUDED_

// CatchGridDlg.h : Header-Datei
//
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CCatchGridDlg 

class CCatchGridDlg : public CDialog
{
// Konstruktion
public:
	CCatchGridDlg(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CCatchGridDlg)
	enum { IDD = IDD_CATCH_GRID };
	float	m_fGridstepX;
	float	m_fGridstepY;
	float	m_fGridOrgX;
	float	m_fGridOrgY;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CCatchGridDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CCatchGridDlg)
		// HINWEIS: Der Klassen-Assistent fügt hier Member-Funktionen ein
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_CATCHGRIDDLG_H__B5540EE2_2DD3_11D2_9DB9_0000B458D891__INCLUDED_
