// SerialEdit.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "SerialEdit.h"
#include "SerialEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSerialEditApp

BEGIN_MESSAGE_MAP(CSerialEditApp, CWinApp)
	//{{AFX_MSG_MAP(CSerialEditApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSerialEditApp construction

CSerialEditApp::CSerialEditApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSerialEditApp object

CSerialEditApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSerialEditApp initialization

BOOL CSerialEditApp::InitInstance()
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

	CSerialEditDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK) {
		dlg.WriteToFile();
	} else if (nResponse == IDCANCEL) {
		// canceled, nothing to do
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
