// TestEditListSubitem.h : Hauptheaderdatei für die TestEditListSubitem-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error 'stdafx.h' muss vor dieser Datei in PCH eingeschlossen werden.
#endif

#include "resource.h"		// Hauptsymbole


// CTestEditListSubitemApp:
// Siehe TestEditListSubitem.cpp für die Implementierung dieser Klasse
//

class CTestEditListSubitemApp : public CWinApp
{
public:
	CTestEditListSubitemApp();

// Überschreibungen
	public:
	virtual BOOL InitInstance();

// Implementierung

	DECLARE_MESSAGE_MAP()
};

extern CTestEditListSubitemApp theApp;
