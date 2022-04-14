// Autoformat.h : Hauptheaderdatei für die Autoformat-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error 'stdafx.h' muss vor dieser Datei in PCH eingeschlossen werden.
#endif

#include "resource.h"		// Hauptsymbole


// CAutoformatApp:
// Siehe Autoformat.cpp für die Implementierung dieser Klasse
//

class CAutoformatApp : public CWinApp
{
public:
	CAutoformatApp();

// Überschreibungen
	public:
	virtual BOOL InitInstance();

// Implementierung

	DECLARE_MESSAGE_MAP()
};

extern CAutoformatApp theApp;
