// DCClient.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DCClient.h"
#include "DCClientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDCClientApp

BEGIN_MESSAGE_MAP(CDCClientApp, CWinApp)
	//{{AFX_MSG_MAP(CDCClientApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDCClientApp construction

CDCClientApp::CDCClientApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDCClientApp object

CDCClientApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDCClientApp initialization

BOOL CDCClientApp::InitInstance()
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

	m_pDlg = new CDCClientDlg;
	m_pMainWnd = m_pDlg;
	m_pDlg->ShowWindow(SW_SHOW);
	
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return TRUE;
}

BOOL CDCClientApp::OnIdle(LONG lCount) 
{
	((CDCClientDlg*)m_pMainWnd)->OnIdle();
	
	return CWinApp::OnIdle(lCount);
}


CDCClientDlg* CDCClientApp::GetDCClientDlg()
{
	return m_pDlg;
}

int CDCClientApp::ExitInstance() 
{
	WK_DELETE(m_pDlg);
	return 0;
}
