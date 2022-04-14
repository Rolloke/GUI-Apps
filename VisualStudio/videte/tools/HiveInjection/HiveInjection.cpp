// HiveInjection.cpp : Definiert das Klassenverhalten für die Anwendung.
//

#include "stdafx.h"
#include "HiveInjection.h"
#include "HiveInjectionDlg.h"
#include ".\hiveinjection.h"
#include "CProfile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CHiveInjectionApp

///////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CHiveInjectionApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

VOID CALLBACK FileIOCompletionRoutine(
  DWORD dwErrorCode,
  DWORD dwNumberOfBytesTransfered,
  LPOVERLAPPED lpOverlapped
)
{
	TRACE(_T("FileIOCompletionRoutine"));
}


// CHiveInjectionApp-Erstellung

///////////////////////////////////////////////////////////////////////////////////
CHiveInjectionApp::CHiveInjectionApp()
{
	// TODO: Hier Code zur Konstruktion einfügen
	// Alle wichtigen Initialisierungen in InitInstance positionieren
}


// Das einzige CHiveInjectionApp-Objekt

CHiveInjectionApp theApp;


// CHiveInjectionApp Initialisierung

///////////////////////////////////////////////////////////////////////////////////
BOOL CHiveInjectionApp::InitInstance()
{
	// InitCommonControls() ist für Windows XP erforderlich, wenn ein Anwendungsmanifest
	// die Verwendung von ComCtl32.dll Version 6 oder höher zum Aktivieren
	// von visuellen Stilen angibt. Ansonsten treten beim Erstellen von Fenstern Fehler auf.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Standardinitialisierung
	// Wenn Sie diese Features nicht verwenden und die Größe
	// der ausführbaren Datei verringern möchten, entfernen Sie
	// die nicht erforderlichen Initialisierungsroutinen.
	// Ändern Sie den Registrierungsschlüssel unter dem Ihre Einstellungen gespeichert sind.
	// TODO: Ändern Sie diese Zeichenfolge entsprechend,
	// z.B. zum Namen Ihrer Firma oder Organisation.
	SetRegistryKey(_T("Vom lokalen Anwendungs-Assistenten generierte Anwendungen"));
	CString sAppName = AfxGetAppName();

	WK_ApplicationId	AppID	= WAI_INVALID;
	WK_ALONE(sAppName);	 
	// Logfilename erzeugen
	CString sLogFile = sAppName+_T(".log");
	// Debugger öffnen
	InitDebugger(sLogFile,AppID,NULL);

	CHiveInjectionDlg dlg;
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

///////////////////////////////////////////////////////////////////////////////////
int CHiveInjectionApp::ExitInstance()
{
	TRACE(_T("CHiveInjectionApp::ExitInstance()\n"));
	// Debugger schließen
	CloseDebugger();

	return CWinApp::ExitInstance();
}

