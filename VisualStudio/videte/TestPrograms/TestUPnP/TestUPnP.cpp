// TestUPnP.cpp : Definiert das Klassenverhalten für die Anwendung.
//

#include "stdafx.h"
#include "TestUPnP.h"
#include "TestUPnPDlg.h"
#include ".\testupnp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestUPnPApp

BEGIN_MESSAGE_MAP(CTestUPnPApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CTestUPnPApp-Erstellung

CTestUPnPApp::CTestUPnPApp()
{
	// TODO: Hier Code zur Konstruktion einfügen
	// Alle wichtigen Initialisierungen in InitInstance positionieren
}


// Das einzige CTestUPnPApp-Objekt

CTestUPnPApp theApp;


// CTestUPnPApp Initialisierung

BOOL CTestUPnPApp::InitInstance()
{
	// InitCommonControls() ist für Windows XP erforderlich, wenn ein Anwendungsmanifest
	// die Verwendung von ComCtl32.dll Version 6 oder höher zum Aktivieren
	// von visuellen Stilen angibt. Ansonsten treten beim Erstellen von Fenstern Fehler auf.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	SetRegistryKey(_T("Videte"));

	CString sAppName = m_pszAppName;
	// Logfilename erzeugen
	CString sLogFile = sAppName+_T(".log");

	// Debugger öffnen
	InitDebugger(sLogFile, WAI_INVALID, NULL);

	WK_TRACE(_T("Starting UPnPtest..."));

	CTestUPnPDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Fügen Sie hier Code ein, um das Schließen des
		//  Dialogfelds über OK zu steuern
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Fügen Sie hier Code ein, um das Schließen des
		//  Dialogfelds über "Abbrechen" zu steuern
	}

	// Da das Dialogfeld geschlossen wurde, FALSE zurückliefern, so dass wir die
	//  Anwendung verlassen, anstatt das Nachrichtensystem der Anwendung zu starten.
	return FALSE;
}

int CTestUPnPApp::ExitInstance()
{
	WK_TRACE(_T("ExitInstance\n"));
	CloseDebugger();

	return CWinApp::ExitInstance();
}
