// OEMSkinOptions.h : Hauptheaderdatei für die OEMSkinOptions-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error 'stdafx.h' muss vor dieser Datei in PCH eingeschlossen werden.
#endif

#include "resource.h"		// Hauptsymbole


// COEMSkinOptionsApp:
// Siehe OEMSkinOptions.cpp für die Implementierung dieser Klasse
//

class COEMSkinOptionsApp : public CWinApp
{
public:
	COEMSkinOptionsApp();

// Überschreibungen
	public:
	virtual BOOL InitInstance();

// Implementierung

	DECLARE_MESSAGE_MAP()
	afx_msg void OnFileRestoreSysColors();
	afx_msg void OnUpdateFileRestoreSysColors(CCmdUI *pCmdUI);
	BOOL m_bSysColorsSaved;
};

extern COEMSkinOptionsApp theApp;
