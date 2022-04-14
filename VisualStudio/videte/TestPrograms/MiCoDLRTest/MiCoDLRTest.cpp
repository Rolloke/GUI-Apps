// MiCoDLRTest.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "MiCoDLRTest.h"
#include "MiCoDLRTestDlg.h"
#include "wk.h"
#include "CIpc.h"
#include "wk_util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMiCoDLRTestApp

BEGIN_MESSAGE_MAP(CMiCoDLRTestApp, CWinApp)
	//{{AFX_MSG_MAP(CMiCoDLRTestApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMiCoDLRTestApp construction

CMiCoDLRTestApp::CMiCoDLRTestApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMiCoDLRTestApp object

CMiCoDLRTestApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMiCoDLRTestApp initialization

BOOL CMiCoDLRTestApp::InitInstance()
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

	// Debugger öffnen
	InitDebugger("MiCoDLRTest.log",WAI_MICOUNIT, NULL);

	WK_TRACE("Starting MiCoUnitPCI...");
	WK_PRINT_VERSION(22, 3, 11, 32, "22/03/99,11:32\0", // @AUTO_UPDATE
		0);	// DO NOT TOUCH THIS LINE

	CMiCoDLRTestDlg dlg;
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

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CMiCoDLRTestApp::ExitInstance() 
{
	// Debugger schließen
	CloseDebugger();

	return CWinApp::ExitInstance();
}
