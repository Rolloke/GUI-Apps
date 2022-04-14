// CreateControlledProcess.h : Hauptheaderdatei für die CreateControlledProcess-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error 'stdafx.h' muss vor dieser Datei in PCH eingeschlossen werden.
#endif

#include "resource.h"		// Hauptsymbole

// CCreateControlledProcessApp:
// Siehe CreateControlledProcess.cpp für die Implementierung dieser Klasse
//

class CCreateControlledProcessApp : public CWinApp
{
public:
	CCreateControlledProcessApp();

// Überschreibungen
	public:
	virtual BOOL InitInstance();

// Implementierung

	DECLARE_MESSAGE_MAP()
};

extern CCreateControlledProcessApp theApp;
