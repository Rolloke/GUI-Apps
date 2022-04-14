// LogExtract.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "LogExtract.h"
#include "LogExtractDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLogExtractApp

BEGIN_MESSAGE_MAP(CLogExtractApp, CWinApp)
	//{{AFX_MSG_MAP(CLogExtractApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogExtractApp construction

CLogExtractApp::CLogExtractApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CLogExtractApp object

CLogExtractApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CLogExtractApp initialization

BOOL CLogExtractApp::InitInstance()
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


	InitDebugger("LogExtract.log",WAI_INVALID);

	// WK_TODO CString sTitle = COemGuiApi::GetApplicationName(WAI_INVALID);
	// WK_TODO m_pszAppName = _tcsdup(sTitle);    // human readable title

	// Never touch this line manually
	WK_TRACE("Version %s\n",
				PLAIN_TEXT_VERSION(7, 5, 9, 53, "07/05/99 , 09:53\0", // @AUTO_UPDATE
				0)
				);

	WK_STAT_PEAK("Reset", 2, "Active");
	WK_STAT_LOG("Reset", 1, "Active");
	WK_TRACE_USER("LogExtract program started...\n");


	CLogExtractDlg dlg;
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

int CLogExtractApp::ExitInstance()
{
	WK_TRACE_USER("LogCount program end...\n");
	WK_STAT_LOG("Reset", 0, "Active");
	CloseDebugger();

	return CWinApp::ExitInstance();
}
