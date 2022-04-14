// DecodeApp.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DecodeApp.h"
#include "DecodeAppDlg.h"
#include "CDump.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDump* pDebug;

/////////////////////////////////////////////////////////////////////////////
// CDecodeAppApp

BEGIN_MESSAGE_MAP(CDecodeAppApp, CWinApp)
	//{{AFX_MSG_MAP(CDecodeAppApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDecodeAppApp construction

CDecodeAppApp::CDecodeAppApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDecodeAppApp object

CDecodeAppApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDecodeAppApp initialization

BOOL CDecodeAppApp::InitInstance()
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

	pDebug = new CDump(TRUE);
	if (!pDebug)
		return FALSE;
	
	if (!pDebug->IsValid())
		return FALSE;

	CDecodeAppDlg* pDlg;

	pDlg = new CDecodeAppDlg();

	if (pDlg && pDlg->IsValid())
	{
		m_pMainWnd = pDlg;
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}

int CDecodeAppApp::ExitInstance() 
{
	delete pDebug;
	pDebug = NULL;

	return CWinApp::ExitInstance();
}
