// MenuToolTip.h : Hauptheaderdatei für die MenuToolTip-Anwendung
//
#pragma once

#ifndef __AFXWIN_H__
	#error 'stdafx.h' muss vor dieser Datei in PCH eingeschlossen werden.
#endif

#include "resource.h"       // Hauptsymbole


// CMenuToolTipApp:
// Siehe MenuToolTip.cpp für die Implementierung dieser Klasse
//

class CMenuToolTipApp : public CWinApp
{
public:
	CMenuToolTipApp();


// Überschreibungen
public:
	virtual BOOL InitInstance();

// Implementierung
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMenuToolTipApp theApp;
