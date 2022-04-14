// TestJoystick.cpp : Definiert das Klassenverhalten für die Anwendung.
//

#include "stdafx.h"
#include "TestJoystick.h"
#include "TestJoystickDlg.h"
#include ".\testjoystick.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestJoystickApp

BEGIN_MESSAGE_MAP(CTestJoystickApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CTestJoystickApp-Erstellung

CTestJoystickApp::CTestJoystickApp()
{
	// TODO: Hier Code zur Konstruktion einfügen
	// Alle wichtigen Initialisierungen in InitInstance positionieren
}


// Das einzige CTestJoystickApp-Objekt

CTestJoystickApp theApp;


// CTestJoystickApp Initialisierung
BOOL CTestJoystickApp::InitInstance()
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

	m_pMainWnd = new CTestJoystickDlg;
	if (((CTestJoystickDlg*)m_pMainWnd)->Create(CTestJoystickDlg::IDD))
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CTestJoystickApp::OnIdle(LONG lCount)
{
//	if (m_pMainWnd)
//	{
//		m_pMainWnd->SendMessage(WM_USER, WM_USER);
//	}

	return CWinApp::OnIdle(lCount);
}
