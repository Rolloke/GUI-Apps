// TestCIPCPerformance.h : Hauptheaderdatei für die TestCIPCPerformance-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error 'stdafx.h' muss vor dieser Datei in PCH eingeschlossen werden.
#endif

#include "resource.h"		// Hauptsymbole


// CTestCIPCPerformanceApp:
// Siehe TestCIPCPerformance.cpp für die Implementierung dieser Klasse
//

class CTestCIPCPerformanceApp : public CWinApp
{
public:
	CTestCIPCPerformanceApp();

// Überschreibungen
	public:
	virtual BOOL InitInstance();

// Implementierung

	DECLARE_MESSAGE_MAP()
public:
	int m_nInstance;
	virtual int ExitInstance();
};

extern CTestCIPCPerformanceApp theApp;
