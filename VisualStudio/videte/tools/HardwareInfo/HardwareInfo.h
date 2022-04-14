// HardwareInfo.h : Hauptheaderdatei für die HardwareInfo-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error 'stdafx.h' muss vor dieser Datei in PCH eingeschlossen werden.
#endif

#include "resource.h"		// Hauptsymbole


// CHardwareInfoApp:
// Siehe HardwareInfo.cpp für die Implementierung dieser Klasse
//

class CHardwareInfoApp : public CWinApp
{
public:
	CHardwareInfoApp();

// Überschreibungen
	public:
	virtual BOOL InitInstance();

// Implementierung

	DECLARE_MESSAGE_MAP()
};

extern CHardwareInfoApp theApp;
