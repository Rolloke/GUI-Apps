// MPEG4Play2.cpp : Definiert das Klassenverhalten für die Anwendung.
//

#include "stdafx.h"
#include "MPEG4Play2.h"
#include "MainFrm.h"

#include "MPEG4Play2Doc.h"
#include "MPEG4Play2View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMPEG4Play2App

BEGIN_MESSAGE_MAP(CMPEG4Play2App, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// Dateibasierte Standarddokumentbefehle
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standarddruckbefehl "Seite einrichten"
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// CMPEG4Play2App-Erstellung

CMPEG4Play2App::CMPEG4Play2App()
{
	m_pPanel = NULL;
}


// Das einzige CMPEG4Play2App-Objekt

CMPEG4Play2App theApp;

// CMPEG4Play2App Initialisierung

BOOL CMPEG4Play2App::InitInstance()
{
	// InitCommonControls() ist für Windows XP erforderlich, wenn ein Anwendungsmanifest
	// die Verwendung von ComCtl32.dll Version 6 oder höher zum Aktivieren
	// von visuellen Stilen angibt. Ansonsten treten beim Erstellen von Fenstern Fehler auf.
	InitCommonControls();

	CWinApp::InitInstance();

	// OLE-Bibliotheken initialisieren
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// Standardinitialisierung
	// Wenn Sie diese Features nicht verwenden und die Größe
	// der ausführbaren Datei verringern möchten, entfernen Sie
	// die nicht erforderlichen Initialisierungsroutinen.
	// Ändern Sie den Registrierungsschlüssel unter dem Ihre Einstellungen gespeichert sind.
	// TODO: Ändern Sie diese Zeichenfolge entsprechend,
	// z.B. zum Namen Ihrer Firma oder Organisation.
	SetRegistryKey(_T("MPEG4Play2"));
	LoadStdProfileSettings(4);  // Standard INI-Dateioptionen laden (einschließlich MRU)
	// Dokumentvorlagen der Anwendung registrieren. Dokumentvorlagen
	//  dienen als Verbindung zwischen Dokumenten, Rahmenfenstern und Ansichten.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CMPEG4Play2Doc),
		RUNTIME_CLASS(CMainFrame),       // Haupt-SDI-Rahmenfenster
		RUNTIME_CLASS(CMPEG4Play2View));
	AddDocTemplate(pDocTemplate);
	// Befehlszeile parsen, um zu prüfen auf Standardumgebungsbefehle DDE, Datei offen
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Verteilung der in der Befehlszeile angegebenen Befehle. Es wird FALSE zurückgegeben, wenn
	// die Anwendung mit /RegServer, /Register, /Unregserver oder /Unregister gestartet wurde.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// Das einzige Fenster ist initialisiert und kann jetzt angezeigt und aktualisiert werden.
	//m_pMainWnd->CenterWindow();
	m_pMainWnd->ShowWindow(SW_HIDE);
	//m_pMainWnd->ShowWindow(SW_SHOW);
	//m_pMainWnd->UpdateWindow();

	// Panel erstellen und anzeigen
	CPanelDlg* pPanel;
	pPanel = new CPanelDlg();
	pPanel->ShowWindow(SW_SHOW);
	//m_pPanel = new CPanelDlg();
	//m_pPanel->ShowWindow(SW_SHOW);

	// Falls bei Aufruf des Programms ein Paramter mit einem gueltigen
	// Dateinamen uebergeben wurde, wird diese Datei geoeffnet.
	if(cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen)
	{
		//m_pPanel->SetFilename(cmdInfo.m_strFileName);
		//m_pPanel->OnBnClickedButtonOpen();
		pPanel->SetFilename(cmdInfo.m_strFileName);
		pPanel->OnBnClickedButtonOpen();
	}

	// Rufen Sie DragAcceptFiles nur auf, wenn eine Suffix vorhanden ist.
	//  In einer SDI-Anwendung ist dies nach ProcessShellCommand erforderlich
	return TRUE;
}



// CAboutDlg-Dialogfeld für Anwendungsbefehl 'Info'

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialogfelddaten
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

// Implementierung
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// Anwendungsbefehl zum Ausführen des Dialogfelds
void CMPEG4Play2App::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CMPEG4Play2App Meldungshandler

