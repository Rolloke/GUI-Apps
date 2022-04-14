// DetermineIPAddress.h : Hauptheaderdatei für die DetermineIPAddress-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error 'stdafx.h' muss vor dieser Datei in PCH eingeschlossen werden.
#endif

#include "resource.h"		// Hauptsymbole


// CDetermineIPAddressApp:
// Siehe DetermineIPAddress.cpp für die Implementierung dieser Klasse
//

class CDetermineIPAddressApp : public CWinApp
{
public:
	CDetermineIPAddressApp();

// Überschreibungen
	public:
	virtual BOOL InitInstance();

// Implementierung

	DECLARE_MESSAGE_MAP()
};

extern CDetermineIPAddressApp theApp;
