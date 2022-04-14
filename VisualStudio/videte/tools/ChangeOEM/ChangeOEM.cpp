// ChangeOEM.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ChangeOEM.h"
#include "ChangeOEMDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CChangeOEMApp

BEGIN_MESSAGE_MAP(CChangeOEMApp, CWinApp)
	//{{AFX_MSG_MAP(CChangeOEMApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangeOEMApp construction

CChangeOEMApp::CChangeOEMApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CChangeOEMApp object

CChangeOEMApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CChangeOEMApp initialization

BOOL CChangeOEMApp::InitInstance()
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
	CChangeOEMDlg dlg;
//	dlg.m_pOEMApp = &theApp;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();

	HANDLE hOEMOK;
	if(nResponse == IDOK)
	{
		//Im OK-Fall Event erzeugen, daß vom Programm "Endcontrol" benötigt wird
		//um zu sehen, ob die OEM erfolgreich eingetragen wurde
		hOEMOK = CreateEvent(NULL, true, true, "OEMOK_");

		//Falls ChangeOEM nicht aus "Endcontrol" heraus aufgerufen wird, muß
		//das Handle auch wieder geschlossen werden.
		//Dies geschieht sonst in "Endcontrol"
		Sleep(500);
		CloseHandle(hOEMOK);
	}


	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
