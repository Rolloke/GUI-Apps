// PerformanceLogger.h : Hauptheaderdatei für die PerformanceLogger-Anwendung
//
#pragma once

#ifndef __AFXWIN_H__
	#error 'stdafx.h' muss vor dieser Datei in PCH eingeschlossen werden.
#endif

#include "resource.h"       // Hauptsymbole


// CPerformanceLoggerApp:
// Siehe PerformanceLogger.cpp für die Implementierung dieser Klasse
//

class CPerformanceLoggerApp : public CWinApp
{
public:
	CPerformanceLoggerApp();


// Überschreibungen
public:
	virtual BOOL InitInstance();

// Implementierung
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CPerformanceLoggerApp theApp;
