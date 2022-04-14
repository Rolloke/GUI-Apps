// ModifyStyle.h : Haupt-Header-Datei für die Anwendung MODIFYSTYLE
//

#if !defined(AFX_MODIFYSTYLE_H__C30A77D7_4F2D_4F81_83B3_F9F43BB8CCA8__INCLUDED_)
#define AFX_MODIFYSTYLE_H__C30A77D7_4F2D_4F81_83B3_F9F43BB8CCA8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// Hauptsymbole

/////////////////////////////////////////////////////////////////////////////
// CModifyStyleApp:
// Siehe ModifyStyle.cpp für die Implementierung dieser Klasse
//

class CModifyStyleApp : public CWinApp
{
public:
	CModifyStyleApp();

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CModifyStyleApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementierung

	//{{AFX_MSG(CModifyStyleApp)
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_MODIFYSTYLE_H__C30A77D7_4F2D_4F81_83B3_F9F43BB8CCA8__INCLUDED_)
