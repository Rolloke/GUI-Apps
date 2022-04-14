// RemoteControl.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include <afxsock.h>
#include "RemoteControlDef.h"
#include "RemoteControl.h"
#include "RemoteControlDlg.h"
#include "..\\MiniClient\\MiniClientDef.h"
#include "oemgui\oemguiapi.h"

#include "WK_Trace.h"
#include "WK_Dongle.h"
#include "WK_Names.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only CRemoteControlApp object

CRemoteControlApp theApp;

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CRemoteControlApp, CWinApp)
	//{{AFX_MSG_MAP(CRemoteControlApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CRemoteControlApp::CRemoteControlApp()
{
 	m_AppID	= WAI_LAUNCHER;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CRemoteControlApp::InitInstance()
{		   
	Enable3dControls();			// Call this when using MFC in a shared DLL
	
	WK_ALONE(WK_APP_NAME_LAUNCHER);

	InitDebugger(_T("RemoteCtrl.log"), m_AppID, NULL);
	WK_TRACE(_T("Starting RemoteControl...\n"));

	COemGuiApi::SplashDialog(1);

	TCHAR szWinPath[MAX_PATH ];
	CString sWinPath;
	if (!GetWindowsDirectory(szWinPath, MAX_PATH))
	{	WK_TRACE_ERROR(_T("Can't find WindowDirectory\n"));
		return FALSE;
	}

	// Dongleobjekt anhand des Softdongles anlegen.
	sWinPath = szWinPath;
	m_pDongle = new CWK_Dongle(/*sWinPath + _T("\\SoftDongle.dcf")*/);
	if (!m_pDongle)
	{
		WK_TRACE_ERROR(_T("Dongle-Klasse konnte nicht angelegt werden\n"));
		return FALSE;
	}

	// Sockets initialisieren.
	if (!AfxSocketInit())
	{
		WK_TRACE_ERROR(_T("AfxSocketInit failed\n"));
		return FALSE;
	}
							    
	CRemoteControlDlg dlg;			     
	m_pMainWnd = &dlg;
									   
	dlg.DoModal();				 
										  
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////
int CRemoteControlApp::ExitInstance() 
{
	WK_DELETE(m_pDongle);

	CloseDebugger();	

	return CWinApp::ExitInstance();
}

