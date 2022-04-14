// PTZTest.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PTZTest.h"
#include "PTZTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPTZTestApp

BEGIN_MESSAGE_MAP(CPTZTestApp, CWinApp)
	//{{AFX_MSG_MAP(CPTZTestApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPTZTestApp construction

CPTZTestApp::CPTZTestApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_iResponse = RESPONSE_IGNORE;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPTZTestApp object

CPTZTestApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPTZTestApp initialization

BOOL CPTZTestApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	InitDebugger("PTZTest.log", WAI_INVALID);

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CPTZTestDlg dlg;
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
/////////////////////////////////////////////////////////////////////////////
int CPTZTestApp::GetResponse()
{
	int iReturn = FALSE;
	m_csAck.Lock();
	iReturn = m_iResponse;
	m_csAck.Unlock();
	return iReturn;
}
/////////////////////////////////////////////////////////////////////////////
int CPTZTestApp::SetResponse(int iResponse)
{
	int iReturn = FALSE;
	m_csAck.Lock();
	m_iResponse = iResponse;
	iReturn = m_iResponse;
	m_csAck.Unlock();
	return iReturn;
}

int CPTZTestApp::ExitInstance() 
{
	CloseDebugger();
	return CWinApp::ExitInstance();
}
