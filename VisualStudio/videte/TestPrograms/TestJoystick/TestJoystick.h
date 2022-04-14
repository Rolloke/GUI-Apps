// TestJoystick.h : Hauptheaderdatei für die TestJoystick-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error 'stdafx.h' muss vor dieser Datei in PCH eingeschlossen werden.
#endif

#include "resource.h"		// Hauptsymbole


// CTestJoystickApp:
// Siehe TestJoystick.cpp für die Implementierung dieser Klasse
//

class CTestJoystickApp : public CWinApp
{
public:
	CTestJoystickApp();

// Überschreibungen
	public:
	virtual BOOL InitInstance();

// Implementierung

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);
};

extern CTestJoystickApp theApp;
