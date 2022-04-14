// IPSender.h : Hauptheaderdatei für die IPSender-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error 'stdafx.h' muss vor dieser Datei in PCH eingeschlossen werden.
#endif

#include "resource.h"		// Hauptsymbole


// CIPSenderApp:
// Siehe IPSender.cpp für die Implementierung dieser Klasse
//

class CIPSenderApp : public CWinApp
{
public:
	CIPSenderApp();

// Überschreibungen
	public:
	virtual BOOL InitInstance();

// Implementierung

	DECLARE_MESSAGE_MAP()
};

extern CIPSenderApp theApp;
