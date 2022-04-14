// HiveInjection.h : Hauptheaderdatei für die HiveInjection-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error 'stdafx.h' muss vor dieser Datei in PCH eingeschlossen werden.
#endif

#include "resource.h"		// Hauptsymbole


// CHiveInjectionApp:
// Siehe HiveInjection.cpp für die Implementierung dieser Klasse
//
class CProfile;
class CHiveInjectionApp : public CWinApp
{
public:
	CHiveInjectionApp();

// Überschreibungen
	public:
	virtual BOOL InitInstance();

// Implementierung

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CHiveInjectionApp theApp;
