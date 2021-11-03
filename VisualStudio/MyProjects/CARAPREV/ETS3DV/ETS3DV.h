// ETS3DV.h : Haupt-Header-Datei für die Anwendung ETS3DV
//

#if !defined(AFX_ETS3DV_H__579C69C3_3BA8_11D4_B6EC_0000B458D891__INCLUDED_)
#define AFX_ETS3DV_H__579C69C3_3BA8_11D4_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // Hauptsymbole

/////////////////////////////////////////////////////////////////////////////
// CETS3DVApp:
// Siehe ETS3DV.cpp für die Implementierung dieser Klasse
//

class CETS3DVApp : public CWinApp
{
public:
	CETS3DVApp();

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CETS3DVApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementierung
	COleTemplateServer m_server;
		// Server-Objekt für Dokumenterstellung
	//{{AFX_MSG(CETS3DVApp)
	afx_msg void OnAppAbout();
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_ETS3DV_H__579C69C3_3BA8_11D4_B6EC_0000B458D891__INCLUDED_)
