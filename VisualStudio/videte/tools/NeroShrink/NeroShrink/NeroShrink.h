// NeroShrink.h : Hauptheaderdatei für die NeroShrink-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error 'stdafx.h' muss vor dieser Datei in PCH eingeschlossen werden.
#endif

#include "resource.h"		// Hauptsymbole


// CNeroShrinkApp:
// Siehe NeroShrink.cpp für die Implementierung dieser Klasse
//

CString GetLastErrorString();
CString GetTempPath();
class CNeroShrinkApp : public CWinApp
{
public:
	CNeroShrinkApp();

// Überschreibungen
	public:
	virtual BOOL InitInstance();

// Implementierung

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CNeroShrinkApp theApp;
