// NeroShrink.cpp : Definiert das Klassenverhalten für die Anwendung.
//

#include "stdafx.h"
#include "NeroShrink.h"
#include "NeroShrinkDlg.h"
#include ".\neroshrink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNeroShrinkApp

BEGIN_MESSAGE_MAP(CNeroShrinkApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CNeroShrinkApp-Erstellung

CNeroShrinkApp::CNeroShrinkApp()
{
	// TODO: Hier Code zur Konstruktion einfügen
	// Alle wichtigen Initialisierungen in InitInstance positionieren
}


// Das einzige CNeroShrinkApp-Objekt

CNeroShrinkApp theApp;


// CNeroShrinkApp Initialisierung

BOOL CNeroShrinkApp::InitInstance()
{
	// InitCommonControls() ist für Windows XP erforderlich, wenn ein Anwendungsmanifest
	// die Verwendung von ComCtl32.dll Version 6 oder höher zum Aktivieren
	// von visuellen Stilen angibt. Ansonsten treten beim Erstellen von Fenstern Fehler auf.
	InitCommonControls();

	CWinApp::InitInstance();

	// Standardinitialisierung
	// Wenn Sie diese Features nicht verwenden und die Größe
	// der ausführbaren Datei verringern möchten, entfernen Sie
	// die nicht erforderlichen Initialisierungsroutinen.
	// Ändern Sie den Registrierungsschlüssel unter dem Ihre Einstellungen gespeichert sind.
	// TODO: Ändern Sie diese Zeichenfolge entsprechend,
	// z.B. zum Namen Ihrer Firma oder Organisation.
	SetRegistryKey(_T("Vom lokalen Anwendungs-Assistenten generierte Anwendungen"));

	CNeroShrinkDlg dlg;
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
/////////////////////////////////////////////////////////////////////////////
CString GetLastErrorString()
{
	LONG uOsError = GetLastError();
	CString sRet;
	LPVOID lpMsgBuf;

	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		uOsError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
		);

	sRet.Format(_T("%d,%s"),uOsError,lpMsgBuf);

	// Free the buffer.
	LocalFree( lpMsgBuf );

	return sRet;
}
/////////////////////////////////////////////////////////////////////////////
CString GetTempPath()
{
	CString str;
	LPTSTR sBuffer = str.GetBufferSetLength(MAX_PATH);
	GetTempPath(MAX_PATH, sBuffer);
	GetLongPathName(sBuffer, sBuffer, MAX_PATH);
	str.ReleaseBuffer();

	return str;
}

int CNeroShrinkApp::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class

	return CWinApp::ExitInstance();
}
