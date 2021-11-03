// VollOleApp.h : Haupt-Header-Datei für die Anwendung VOLLOLEAPP
//

#if !defined(AFX_VOLLOLEAPP_H__E1E8E845_3E20_11D4_B6EC_0000B458D891__INCLUDED_)
#define AFX_VOLLOLEAPP_H__E1E8E845_3E20_11D4_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // Hauptsymbole

/////////////////////////////////////////////////////////////////////////////
// CVollOleAppApp:
// Siehe VollOleApp.cpp für die Implementierung dieser Klasse
//

class CVollOleAppApp : public CWinApp
{
public:
	CVollOleAppApp();

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CVollOleAppApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementierung
	COleTemplateServer m_server;
		// Server-Objekt für Dokumenterstellung
	//{{AFX_MSG(CVollOleAppApp)
	afx_msg void OnAppAbout();
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_VOLLOLEAPP_H__E1E8E845_3E20_11D4_B6EC_0000B458D891__INCLUDED_)
