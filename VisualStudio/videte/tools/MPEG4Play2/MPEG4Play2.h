// MPEG4Play2.h : Hauptheaderdatei für die MPEG4Play2-Anwendung
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // Hauptsymbole

#include "PanelDlg.h"


// CMPEG4Play2App:
// Siehe MPEG4Play2.cpp für die Implementierung dieser Klasse
//

class CMPEG4Play2App : public CWinApp
{
public:
	CMPEG4Play2App();

	CPanelDlg* m_pPanel;


// Überschreibungen
public:
	virtual BOOL InitInstance();

// Implementierung
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMPEG4Play2App theApp;
