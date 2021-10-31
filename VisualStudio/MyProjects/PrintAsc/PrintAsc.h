// PrintAsc.h : Haupt-Header-Datei für die Anwendung PRINTASC
//

#if !defined(AFX_PRINTASC_H__CAC08F24_25F8_11D2_9DB9_0000B458D891__INCLUDED_)
#define AFX_PRINTASC_H__CAC08F24_25F8_11D2_9DB9_0000B458D891__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// Hauptsymbole

/////////////////////////////////////////////////////////////////////////////
// CPrintAscApp:
// Siehe PrintAsc.cpp für die Implementierung dieser Klasse
//

class CPrintAscApp : public CWinApp
{
public:
	CPrintAscApp();

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CPrintAscApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementierung

	//{{AFX_MSG(CPrintAscApp)
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // !defined(AFX_PRINTASC_H__CAC08F24_25F8_11D2_9DB9_0000B458D891__INCLUDED_)
