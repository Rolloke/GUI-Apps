
// SchrittmotorLPT.h: Hauptheaderdatei für die PROJECT_NAME-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error "'stdafx.h' vor dieser Datei für PCH einschließen"
#endif

#include "resource.h"		// Hauptsymbole


// CSchrittmotorLPTApp:
// Siehe SchrittmotorLPT.cpp für die Implementierung dieser Klasse
//

class CSchrittmotorLPTApp : public CWinApp
{
public:
	CSchrittmotorLPTApp();

// Überschreibungen
	public:
	virtual BOOL InitInstance();

// Implementierung

	DECLARE_MESSAGE_MAP()
};

extern CSchrittmotorLPTApp theApp;