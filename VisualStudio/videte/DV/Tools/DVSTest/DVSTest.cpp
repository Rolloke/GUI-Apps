// DVSTest.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DVSTest.h"
#include "DVSTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDVSTestApp

BEGIN_MESSAGE_MAP(CDVSTestApp, CWinApp)
	//{{AFX_MSG_MAP(CDVSTestApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDVSTestApp construction

CDVSTestApp::CDVSTestApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDVSTestApp object

CDVSTestApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDVSTestApp initialization

BOOL CDVSTestApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#if _MFC_VER < 0x700
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

	InitDebugger(_T("DVSText.log"),WAI_DATABASE_CLIENT);

	CWK_Profile wkp;

	m_sStart = wkp.GetString(_T("DVSTest"),_T("Start"),_T(""));
	m_sEnd = wkp.GetString(_T("DVSTest"),_T("End"),_T(""));
	m_sPath = wkp.GetString(_T("DVSTest"),_T("Path"),_T(""));

	CDVSTestDlg dlg;
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

int CDVSTestApp::ExitInstance() 
{
	CWK_Profile wkp;

	wkp.WriteString(_T("DVSTest"),_T("Start"),m_sStart);
	wkp.WriteString(_T("DVSTest"),_T("End"),m_sEnd);
	wkp.WriteString(_T("DVSTest"),_T("Path"),m_sPath);

	CloseDebugger();
	
	return 0;
}
