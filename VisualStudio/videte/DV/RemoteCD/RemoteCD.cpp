// RemoteCD.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "RemoteCD.h"
#include "RemoteCDDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRemoteCDApp

BEGIN_MESSAGE_MAP(CRemoteCDApp, CWinApp)
	//{{AFX_MSG_MAP(CRemoteCDApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRemoteCDApp construction

CRemoteCDApp::CRemoteCDApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CRemoteCDApp object

CRemoteCDApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CRemoteCDApp initialization

BOOL CRemoteCDApp::InitInstance()
{
	// Standard initialization
#if _MFC_VER < 0x700
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

	InitDebugger(_T("RemoteCD.log"),WAI_DV_REMOTE_CD);

	CRemoteCDDlg* pDlg = new CRemoteCDDlg;
#ifdef _DEBUG
	pDlg->ShowWindow(SW_SHOW);
#endif

	m_pMainWnd = pDlg;
	pDlg->ShowWindow(SW_HIDE);


	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return TRUE;
}

int CRemoteCDApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	CloseDebugger();

	return CWinApp::ExitInstance();
}

BOOL CRemoteCDApp::OnIdle(LONG lCount) 
{
	// TODO: Add your specialized code here and/or call the base class
	((CRemoteCDDlg*)m_pMainWnd)->OnIdle();
	
	return CWinApp::OnIdle(lCount);
}
