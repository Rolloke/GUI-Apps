// ControlService.h : Hauptheaderdatei für die ControlService-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error 'stdafx.h' muss vor dieser Datei in PCH eingeschlossen werden.
#endif

#include "resource.h"		// Hauptsymbole


// CControlServiceApp:
// Siehe ControlService.cpp für die Implementierung dieser Klasse
//

class CControlServiceApp : public CWinApp
{
public:
	CControlServiceApp();

// Überschreibungen
	public:
	virtual BOOL InitInstance();

// Implementierung

	DECLARE_MESSAGE_MAP()

private:
	CString m_sRootRegistryKey;
	CString	m_sCtrlApp;
};

extern CControlServiceApp theApp;
