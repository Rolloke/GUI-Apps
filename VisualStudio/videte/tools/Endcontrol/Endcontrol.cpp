// Endcontrol.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Endcontrol.h"
#include "EndcontrolDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEndcontrolApp

BEGIN_MESSAGE_MAP(CEndcontrolApp, CWinApp)
	//{{AFX_MSG_MAP(CEndcontrolApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEndcontrolApp construction

CEndcontrolApp::CEndcontrolApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CEndcontrolApp object

CEndcontrolApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CEndcontrolApp initialization

BOOL CEndcontrolApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
	
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
	


	if(__argc == 1)
	{
		CEndcontrolDlg dlg;
		m_pMainWnd = &dlg;
		int nResponse = dlg.DoModal();
		if (nResponse == IDOK)
		{
			// TODO: Place code here to handle when the dialog is
			//  dismissed with OK
			
		}
		else if (nResponse == IDCANCEL)
		{
			// TODO: Place code here to handle when the dialog is
			//  dismissed with Cancel
		}
		
	}
	else
	{
		// in Kommandline ist mehr als ein Argument == Clone
		m_bIamTheClone = TRUE;
	}
	
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CEndcontrolApp::ExitInstance()
{
///////

	//Programm ist Clone und läuft nur dann hier rein
	if (m_bIamTheClone) {
		//clone-exe: Lösche Original-exe nach der Terminierung
		HANDLE hProcessOrig = (HANDLE) _ttoi(__targv[1]);
		// warte, bis sich das Original beendet hat 
		WaitForSingleObject(hProcessOrig, INFINITE);
		// lösche das Original
		DeleteFile(__targv[2]);
	}
	else {
		// Original exe:
		// Start clone
		TCHAR szPathOrig[_MAX_PATH], szPathClone[_MAX_PATH];
		GetModuleFileName(NULL, szPathOrig, _MAX_PATH);
		GetTempPath(_MAX_PATH, szPathClone);
		GetTempFileName(szPathClone, __TEXT("Clo"), 0, szPathClone);
		// kopiere Original zum Clone
		CopyFile(szPathOrig, szPathClone, FALSE);
		// öffne clone-exe mit FILE_FLAG_DELETE_ON_CLOSE, sodaß es sich beim 
		// Beenden selbst löscht
		HANDLE	hFileClone = CreateFile(szPathClone, 
								0, 
								FILE_SHARE_READ, 
								NULL,
								OPEN_EXISTING,
								FILE_FLAG_DELETE_ON_CLOSE,
								NULL);
		// starte clone. Übergib auf der Kommandozeile die Nummer des Prozesses
		// und den vollständigen Pfadnamen der Originaldatei
		TCHAR szCmdLine[512];
		HANDLE hProcessOrig = OpenProcess(SYNCHRONIZE, TRUE, GetCurrentProcessId());
		wsprintf(szCmdLine, __TEXT("%s %d \"%s\""), szPathClone, hProcessOrig, szPathOrig);
		STARTUPINFO si;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		PROCESS_INFORMATION pi;
		// starte Clone Prozess
		if (CreateProcess(NULL,
						  szCmdLine,
						  NULL,
						  NULL,
						  TRUE,
						  0,
						  NULL,
						  NULL,
						  &si,
						  &pi) != 0 )
		{
			TRACE("clone process created\n");
		}
		// sleep ist nötig, da der Clone sich erst komplett angelegt haben 
		// muß (CreateProcess dauert so seine Zeit), und dann erst darf 
		// sich das Original beenden. Der Clone wartet nämlich zunächst
		// darauf, daß das Original beendet wurde (WaitForSingleObject) und 
		// läuft dann erst los. Ist das Original aber schon beendet, bevor
		// der Clone sich im WaitForSingleObject befindet, gibt es einen Absturz,
		// weil der Zeiger dann ins Leere (NULL) zeigt.
		Sleep(500);
	}

///////

//	CloseDebugger();


	return CWinApp::ExitInstance();
}
