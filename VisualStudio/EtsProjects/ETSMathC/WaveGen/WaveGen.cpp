/********************************************************************/
/*                                                                  */
/*                     Programm WaveGen                             */
/*                                                                  */
/*     Rauschgenerator, Funktionsgenerator mit Fourieranalyse       */
/*     und Filterung von Wavedateien.                               */
/*                                                                  */
/*     programmed by Rolf Kary-Ehlers                               */
/*                                                                  */
/*     Version 2.2            04.04.2002                            */
/*                                                                  */
/********************************************************************/
// WaveGen.cpp : Legt das Klassenverhalten für die Anwendung fest.
//

#include "stdafx.h"
#include "WaveGen.h"

#include "MainFrm.h"
#include "WaveGenDoc.h"
#include "WaveGenView.h"
#include "CEtsMathClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWaveGenApp

BEGIN_MESSAGE_MAP(CWaveGenApp, CCaraWinApp)
	//{{AFX_MSG_MAP(CWaveGenApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// HINWEIS - Hier werden Mapping-Makros vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG_MAP
	// Dateibasierte Standard-Dokumentbefehle
	ON_COMMAND(ID_FILE_NEW, CCaraWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CCaraWinApp::OnFileOpen)
	// Standard-Druckbefehl "Seite einrichten"
	ON_COMMAND(ID_FILE_PRINT_SETUP, CCaraWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_PROGRESS_STEP, ProgressDlgStep)
END_MESSAGE_MAP()

BOOL CWaveDocManager::DoPromptFileName(CString& fileName, UINT nIDSTitle, DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate)
{
   CSingleDocTemplate dt2(IDR_MAINFRAME2, NULL, NULL, NULL);
   if (pTemplate)
   {
      POSITION pos = pTemplate->GetFirstDocPosition();
      if (pos)
      {
         CWaveGenDoc *pDoc = (CWaveGenDoc*) pTemplate->GetNextDoc(pos);
         if (pDoc)
         {
            if (pDoc->m_bTimeFunction)
               ((CWaveDocTemplate*)pTemplate)->SetResourceID(IDR_MAINFRAME2);
            else
               ((CWaveDocTemplate*)pTemplate)->SetResourceID(IDR_MAINFRAME);

            pTemplate->LoadTemplate();
         }
      }
   }
   else
   {
      AddDocTemplate(&dt2);
   }

   BOOL bReturn = CDocManager::DoPromptFileName(fileName, nIDSTitle, lFlags, bOpenFileDialog, pTemplate);

   CWaveDocManager::RemoveDocTemplate(&dt2);

   if (pTemplate)
   {
      ((CWaveDocTemplate*)pTemplate)->SetResourceID(IDR_MAINFRAME);
      pTemplate->LoadTemplate();
   }
   return bReturn;
}

void CWaveDocManager::RemoveDocTemplate(CDocTemplate*pdt)
{
   POSITION pos = NULL;
   pos = m_templateList.Find(pdt);
	if (pos != NULL)
	{
      m_templateList.RemoveAt(pos);
	}
}

void CWaveDocTemplate::SetResourceID(int nId)
{
   m_nIDResource = nId;
   m_strDocStrings.Empty();
}

/////////////////////////////////////////////////////////////////////////////
// CWaveGenApp Konstruktion
CWaveGenApp::CWaveGenApp()
{
	// ZU ERLEDIGEN: Hier Code zur Konstruktion einfügen
	// Alle wichtigen Initialisierungen in InitInstance platzieren
}

/////////////////////////////////////////////////////////////////////////////
// Das einzige CWaveGenApp-Objekt

CWaveGenApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CWaveGenApp Initialisierung

BOOL CWaveGenApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standardinitialisierung
	// Wenn Sie diese Funktionen nicht nutzen und die Größe Ihrer fertigen 
	//  ausführbaren Datei reduzieren wollen, sollten Sie die nachfolgenden
	//  spezifischen Initialisierungsroutinen, die Sie nicht benötigen, entfernen.

#ifdef _AFXDLL
	Enable3dControls();			// Diese Funktion bei Verwendung von MFC in gemeinsam genutzten DLLs aufrufen
#else
	Enable3dControlsStatic();	// Diese Funktion bei statischen MFC-Anbindungen aufrufen
#endif

   CCaraWinApp::InitInstance();


	// Dokumentvorlagen der Anwendung registrieren. Dokumentvorlagen
	//  dienen als Verbindung zwischen Dokumenten, Rahmenfenstern und Ansichten.
   m_pDocManager = new CWaveDocManager;
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CWaveGenDoc),
		RUNTIME_CLASS(CMainFrame),       // Haupt-SDI-Rahmenfenster
		RUNTIME_CLASS(CWaveGenView));
   AddDocTemplate(pDocTemplate);

	// Befehlszeile parsen, um zu prüfen auf Standard-Umgebungsbefehle DDE, Datei offen
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Verteilung der in der Befehlszeile angegebenen Befehle
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// Das einzige Fenster ist initialisiert und kann jetzt angezeigt und aktualisiert werden.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg-Dialog für Info über Anwendung

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialogdaten
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// Überladungen für virtuelle Funktionen, die vom Anwendungs-Assistenten erzeugt wurden
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	//{{AFX_MSG(CAboutDlg)
		// Keine Nachrichten-Handler
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// Keine Nachrichten-Handler
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Anwendungsbefehl zum Ausführen des Dialogfelds
void CWaveGenApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CWaveGenApp-Nachrichtenbehandlungsroutinen

int CWaveGenApp::Run() 
{
   CETSMathClass EtsMathClass;
   int nResult = CCaraWinApp::Run();
   EtsMathClass.Destructor();
   return nResult;
}

void CWaveGenApp::CreateProgressDlg(char *pszText)
{
   m_ProgressDlg.Create(IDD_PROGRESS_DLG, theApp.m_pMainWnd);
   m_ProgressDlg.SetWindowText(pszText);
}

void CWaveGenApp::ProgressDlgStep()
{
   m_ProgressDlg.Step();
}

void CWaveGenApp::DestroyProgressDlg()
{
   m_ProgressDlg.DestroyWindow();
}

void CWaveGenApp::ProgressDlgSetRange(int nR)
{
   m_ProgressDlg.SetRange(nR);
}

void CWaveGenApp::OnFileOpen()
{
   CCaraWinApp::OnFileOpen();
}

BOOL CWaveGenApp::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
   if (nID==ID_PROGRESS_SETRANGE)
   {
      long lMEI = ::GetMessageExtraInfo();
      m_ProgressDlg.SetRange(lMEI);
      return true;
   }
	return CCaraWinApp::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
