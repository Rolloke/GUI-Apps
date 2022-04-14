// QUnit.cpp : Definiert das Klassenverhalten für die Anwendung.
//

#include "stdafx.h"
#include "QUnit.h"
#include "QUnitDlg.h"
#include ".\qunit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Das einzige CQUnitApp-Objekt
CQUnitApp theApp;

BEGIN_MESSAGE_MAP(CQUnitApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CQUnitApp-Erstellung

CQUnitApp::CQUnitApp()
{
	// Alle wichtigen Initialisierungen in InitInstance positionieren
}

// CQUnitApp Initialisierung

BOOL CQUnitApp::InitInstance()
{
	// InitCommonControls() ist für Windows XP erforderlich, wenn ein Anwendungsmanifest
	// die Verwendung von ComCtl32.dll Version 6 oder höher zum Aktivieren
	// von visuellen Stilen angibt. Ansonsten treten beim Erstellen von Fenstern Fehler auf.
	InitCommonControls();

	CWinApp::InitInstance();

	WK_ApplicationId	AppID	= WAI_QUNIT;
	XTIB::SetThreadName(_T("MainThread"));
						   
	CString sAppName = GetAppnameFromId(AppID);
	if (!WK_ALONE(sAppName)) 
	{
		return FALSE;
	}
	CString sTitle = sAppName;
	m_pszAppName = _tcsdup(sTitle);    // human readable title
	CWK_RunTimeError::SetDefaultID(AppID);

	// Logfilename erzeugen
	CString sLogFile = sAppName+_T(".log");

	// Debugger öffnen
	InitDebugger(sLogFile,AppID,NULL);

	CQUnitDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		//  Dialogfelds über OK zu steuern
	}
	else if (nResponse == IDCANCEL)
	{
		//  Dialogfelds über "Abbrechen" zu steuern
	}

	// Da das Dialogfeld geschlossen wurde, FALSE zurückliefern, so dass wir die
	//  Anwendung verlassen, anstatt das Nachrichtensystem der Anwendung zu starten.
	return FALSE;
}

int CQUnitApp::ExitInstance()
{
	WK_TRACE(_T("ExitInstance\n"));

	// Debugger schließen
	CloseDebugger();

	return CWinApp::ExitInstance();
}
