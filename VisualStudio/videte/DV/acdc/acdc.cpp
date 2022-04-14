// acdc.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "acdc.h"
#include "acdcDlg.h"

#include "IPCServerControlACDC.h"
#include "NeroThread.h"



#include "BurnContext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAcdcApp

BEGIN_MESSAGE_MAP(CAcdcApp, CWinApp)
	//{{AFX_MSG_MAP(CAcdcApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAcdcApp construction

CAcdcApp::CAcdcApp()
{
	// Place all significant initialization in InitInstance
	m_pServerControl = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAcdcApp object

CAcdcApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAcdcApp initialization

BOOL CAcdcApp::InitInstance()
{
	if (!WK_ALONE(WK_APP_NAME_ACDC))
	{
		return FALSE;
	}
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

	// Exceptionhandler umbiegen
	m_pAcdcException = new CAcdcException(WAI_AC_DC, this);

	InitDebugger(_T("acdc.log"),WAI_AC_DC,m_pAcdcException);

	m_pDlg = new CAcdcDlg;
	m_pMainWnd = m_pDlg;
//	m_pDlg->ShowWindow(SW_SHOW);

	m_pServerControl = new CIPCServerControlACDC();

	m_pDrive = NULL;
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return TRUE;
}



int CAcdcApp::ExitInstance() 
{
	CloseDebugger();
	WK_DELETE(m_pAcdcException);

	WK_DELETE(m_pServerControl);
	WK_DELETE(m_pDlg);

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
BOOL CAcdcApp::OnIdle(LONG lCount) 
{
	((CAcdcDlg*)m_pMainWnd)->OnIdle();

	return CWinApp::OnIdle(lCount);
}

/////////////////////////////////////////////////////////////////////////////
CAcdcDlg* CAcdcApp::GetAcdcDlg()
{
	return m_pDlg;
}

/////////////////////////////////////////////////////////////////////////////
CIPCServerControlACDC* CAcdcApp::GetServerControl()
{
	return m_pServerControl;
}

//////////////////////////////////////////////////////////////////////
void CAcdcApp::OnAppExit()
{
	WK_TRACE(_T("OnAppExit was called\n"));
	if(m_pDlg)
		m_pDlg->OnAppExit();
}

