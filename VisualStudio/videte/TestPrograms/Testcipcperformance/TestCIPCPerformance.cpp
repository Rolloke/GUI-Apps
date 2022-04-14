// TestCIPCPerformance.cpp : Definiert das Klassenverhalten für die Anwendung.
//

#include "stdafx.h"
#include "TestCIPCPerformance.h"
#include "TestCIPCPerformanceDlg.h"
#include ".\testcipcperformance.h"

#include "wkclasses\wk_utilities.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestCIPCPerformanceApp

BEGIN_MESSAGE_MAP(CTestCIPCPerformanceApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CTestCIPCPerformanceApp-Erstellung

CTestCIPCPerformanceApp::CTestCIPCPerformanceApp()
{
	XTIB::SetThreadName(_T("MainThread"));
	m_nInstance = 0;
}


// Das einzige CTestCIPCPerformanceApp-Objekt

CTestCIPCPerformanceApp theApp;


// CTestCIPCPerformanceApp Initialisierung

BOOL CTestCIPCPerformanceApp::InitInstance()
{
	CString sApp;
	do
	{
		m_nInstance++;
		if (m_nInstance == 4)
		{
			return FALSE;
		}
		sApp.Format(_T("%s%d"), m_pszAppName, m_nInstance);
	}
	while (WK_IS_RUNNING(sApp));
	WK_ALONE(sApp);
	sApp += _T(".log");
	InitDebugger(sApp, WAI_INVALID);

	XTIB::SetThreadName(m_pszAppName);
	// InitCommonControls() ist für Windows XP erforderlich, wenn ein Anwendungsmanifest
	// die Verwendung von ComCtl32.dll Version 6 oder höher zum Aktivieren
	// von visuellen Stilen angibt. Ansonsten treten beim Erstellen von Fenstern Fehler auf.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	sApp.Format(_T("videte\\%s%d"), m_pszAppName, m_nInstance);
	SetRegistryKey(sApp);

	CTestCIPCPerformanceDlg dlg;
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

int CTestCIPCPerformanceApp::ExitInstance()
{
	CWK_Criticalsection::TraceLockedSections();
	CloseDebugger();
	return CWinApp::ExitInstance();
}
