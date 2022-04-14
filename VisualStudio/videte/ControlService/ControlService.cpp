// ControlService.cpp : Definiert das Klassenverhalten für die Anwendung.
//

#include "stdafx.h"
#include "ControlService.h"
#include "ControlServiceDlg.h"
#include "MyService.h"
#include "CProfile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define SERVICE_NAME _T("VCService")
#define SERVICE_DISPLAY_NAME _T("VideteControlService")
#define CONTROL_PROGRAM _T("c:\\windows\\Notepad.exe")
//#define CONTROL_PROGRAM _T("c:\\security\\SecurityLauncher.exe")
//#define CONTROL_PROGRAM _T("c:\\Security\\SocketUnit.exe")

#define SERVICE_NAME _T("VCService")
#define SERVICE_DISPLAY_NAME _T("VideteControlService")
#define SERVICE_HELP _T("Überwacht Prozesse und startet sie ggf. neu")

// Das einzige CControlServiceApp-Objekt
///////////////////////////////////////////////////////////////////////////
CControlServiceApp theApp;

// CControlServiceApp

BEGIN_MESSAGE_MAP(CControlServiceApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CControlServiceApp-Erstellung
///////////////////////////////////////////////////////////////////////////
CControlServiceApp::CControlServiceApp()
{
	// TODO: Hier Code zur Konstruktion einfügen
	// Alle wichtigen Initialisierungen in InitInstance positionieren
}

// CControlServiceApp Initialisierung
///////////////////////////////////////////////////////////////////////////
BOOL CControlServiceApp::InitInstance()
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

	m_sRootRegistryKey = _T("Software\\DVRT\\");
	m_sRootRegistryKey += AfxGetAppName();

	CProfile prof(m_sRootRegistryKey);

	prof.ReadProfileString(_T("ControlApplication"), _T(""), m_sCtrlApp);
	prof.WriteProfileString(_T("ControlApplication"), m_sCtrlApp);

	BOOL bAutoClose = prof.ReadProfileFlag(_T("AutoClose"), TRUE);
	prof.WriteProfileFlag(_T("AutoClose"), bAutoClose);

	CMyService Service(bAutoClose, m_sCtrlApp, AfxGetAppName(), SERVICE_HELP);

	CString sCmdLine(m_lpCmdLine);
	Service.FindRunningProcess(_T("1martin.exe"));

	if (sCmdLine.Find(_T("-i")) != -1)
	{
		if (Service.InstallService())
			TRACE(_T("\n\nService Installed Sucessfully\n"));
		else
			TRACE(_T("\n\nError Installing Service\n"));
	}
	else if (sCmdLine.Find(_T("-u")) != -1)
	{
		if (Service.DeleteService())
			TRACE(_T("\n\nService UnInstalled Sucessfully\n"));
		else
			TRACE(_T("\n\nError UnInstalling Service\n"));
	}
	else if (sCmdLine.Find(_T("-s")) != -1)
	{
		if (Service.StartService())
			TRACE(_T("\n\nService start Sucessfully\n"));
		else
			TRACE(_T("\n\nError starting Service\n"));
	}
	else if (sCmdLine.Find(_T("-e")) != -1)
	{
		if (Service.EndService())
			TRACE(_T("\n\nService ended Sucessfully\n"));
		else
			TRACE(_T("\n\nError ending Service\n"));
	}
	else if (sCmdLine.Find(_T("-c")) != -1)
	{
		CControlServiceDlg dlg(&Service);
		m_pMainWnd = &dlg;
		INT_PTR nResponse = dlg.DoModal();
	}
	else
	{
		Service.StartDispatcher();
	}


	// Da das Dialogfeld geschlossen wurde, FALSE zurückliefern, so dass wir die
	//  Anwendung verlassen, anstatt das Nachrichtensystem der Anwendung zu starten.
	return FALSE;
}
