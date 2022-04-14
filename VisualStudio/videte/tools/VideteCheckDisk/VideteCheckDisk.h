// VideteCheckDisk.h : Hauptheaderdatei für die VideteCheckDisk-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error 'stdafx.h' muss vor dieser Datei in PCH eingeschlossen werden.
#endif

#include "resource.h"		// Hauptsymbole


// CVideteCheckDiskApp:
// Siehe VideteCheckDisk.cpp für die Implementierung dieser Klasse
//

class CVideteCheckDiskDlg;

class CVideteCheckDiskApp : public CWinApp
{
public:
	CVideteCheckDiskApp();
	virtual ~CVideteCheckDiskApp();

// Überschreibungen
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementierung
	void DisplayHelp();
	void DisplayInfo();

	DECLARE_MESSAGE_MAP()

public:
	int m_nReturnCode;
private:
	CString m_sLogFilesSource;
	CString m_sLogFilesDestination;
	HMODULE m_hOemGUI;
	CVideteCheckDiskDlg *m_pDlg;
};

extern CVideteCheckDiskApp theApp;
