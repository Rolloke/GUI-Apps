// ETSPaint.h : Haupt-Header-Datei für die Anwendung ETSPAINT
//

#if !defined(AFX_ETSPAINT_H__A5602814_9494_11D4_B6EC_0000B458D891__INCLUDED_)
#define AFX_ETSPAINT_H__A5602814_9494_11D4_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // Hauptsymbole

/////////////////////////////////////////////////////////////////////////////
// CETSPaintApp:
// Siehe ETSPaint.cpp für die Implementierung dieser Klasse
//

class CETSPaintApp : public CWinApp
{
public:
	CETSPaintApp();

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CETSPaintApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementierung
	//{{AFX_MSG(CETSPaintApp)
	afx_msg void OnAppAbout();
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_ETSPAINT_H__A5602814_9494_11D4_B6EC_0000B458D891__INCLUDED_)
