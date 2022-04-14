// SoftDongle.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "SoftDongle.h"
#include "SoftDongleDlg.h"
#include "WK_Trace.h"
#include "WK_Dongle.h"
#include "WK_Names.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSoftDongleApp

BEGIN_MESSAGE_MAP(CSoftDongleApp, CWinApp)
	//{{AFX_MSG_MAP(CSoftDongleApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSoftDongleApp construction

CSoftDongleApp::CSoftDongleApp()
{
	m_pDongle = NULL;
}

CSoftDongleApp::~CSoftDongleApp()
{
	WK_DELETE(m_pDongle);
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSoftDongleApp object

CSoftDongleApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSoftDongleApp initialization

BOOL CSoftDongleApp::InitInstance()
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
 	m_pDongle = new CWK_Dongle(_T("SoftDongle.dcf"));

	CSoftDongleDlg dlg;
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
