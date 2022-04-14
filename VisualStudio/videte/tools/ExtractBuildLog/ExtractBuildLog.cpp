// ExtractBuildLog.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ExtractBuildLog.h"
#include "ExtractBuildLogDlg.h"
#include "CommandLineInfoExtractBuildLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtractBuildLogApp

BEGIN_MESSAGE_MAP(CExtractBuildLogApp, CWinApp)
	//{{AFX_MSG_MAP(CExtractBuildLogApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtractBuildLogApp construction

CExtractBuildLogApp::CExtractBuildLogApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_eFileType = FT_DOCOMATIC_LIST;
	m_sWorkspaceDir	= "D:\\Doc_O_Matic";
	m_sWorkspaceName = "CIPC";
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CExtractBuildLogApp object

CExtractBuildLogApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CExtractBuildLogApp initialization

BOOL CExtractBuildLogApp::InitInstance()
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

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfoExtractBuildLog cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
//	if (!ProcessShellCommand(cmdInfo))
//		return FALSE;

	CExtractBuildLogDlg dlg(m_eFileType, m_sWorkspaceDir, m_sWorkspaceName);
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
