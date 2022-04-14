// TestUPnP.h : Hauptheaderdatei für die TestUPnP-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error 'stdafx.h' muss vor dieser Datei in PCH eingeschlossen werden.
#endif

#include "resource.h"		// Hauptsymbole


// CTestUPnPApp:
// Siehe TestUPnP.cpp für die Implementierung dieser Klasse
//

class CTestUPnPApp : public CWinApp
{
public:
	CTestUPnPApp();

// Überschreibungen
	public:
	virtual BOOL InitInstance();

// Implementierung

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CTestUPnPApp theApp;
