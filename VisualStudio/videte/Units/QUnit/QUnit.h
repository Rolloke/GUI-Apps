// QUnit.h : Hauptheaderdatei für die QUnit-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error 'stdafx.h' muss vor dieser Datei in PCH eingeschlossen werden.
#endif

#include "resource.h"		// Hauptsymbole


// CQUnitApp:
// Siehe QUnit.cpp für die Implementierung dieser Klasse
//

class CQUnitApp : public CWinApp
{
public:
	CQUnitApp();

// Überschreibungen
	public:
	virtual BOOL InitInstance();

// Implementierung
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CQUnitApp theApp;
