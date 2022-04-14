// EwfControl.h : Hauptheaderdatei für die EwfControl-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error 'stdafx.h' muss vor dieser Datei in PCH eingeschlossen werden.
#endif

#include "resource.h"		// Hauptsymbole


// CEwfControlApp:
// Siehe EwfControl.cpp für die Implementierung dieser Klasse
//

class CEwfControlApp : public CWinApp
{
public:
	CEwfControlApp();
// Datenzugriff
// Überschreibungen
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementierung

	DECLARE_MESSAGE_MAP()
};

extern CEwfControlApp theApp;
