// MiniClient.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "MiniClient.h"
#include "Imagn.h"
#include "WK_Trace.h"
#include "WK_Util.h"

#include "MainFrm.h"
#include "Ini2Reg.h"
#include "MiniClientDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMiniClientApp theApp;

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CMiniClientApp, CWinApp)
	//{{AFX_MSG_MAP(CMiniClientApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CMiniClientApp::CMiniClientApp()
{
}

/////////////////////////////////////////////////////////////////////////////
CMiniClientApp::~CMiniClientApp()
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMiniClientApp::InitInstance()
{
	CString sWndTitle;

#if _MFC_VER < 0x0700
	Enable3dControls();			// Call this when using MFC in a shared DLL
#endif

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	
	m_bMiniClientTest	= FALSE;
	m_nCamNr			= 0;

	CString sCmdLine = cmdInfo.m_strFileName;
	
	// Dispatch commands specified on the command line
	if (sCmdLine != _T("XXX"))
	{
		if (sCmdLine.Left(3) == _T("Cam"))
		{
			CString sCamNr = sCmdLine.Mid(3);
			m_nCamNr = _ttoi(sCamNr);
			m_nCamNr = min(max(m_nCamNr, 1), 16);
			m_bMiniClientTest = TRUE;
		}
		else
		{
			AfxMessageBox(IDS_MSG_WRONG_COMMANDLINE, MB_OK);
			return FALSE;
		}
	}

	// Mainframe ˆffnen
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;

	m_pMainWnd = pFrame;

	if (m_bMiniClientTest)
	{
		if (!pFrame->Create(NULL, _T("MiniClient"), WS_OVERLAPPEDWINDOW, CRect(100,100,350,350)))
			return FALSE;
	}
	else
	{
		if (!pFrame->Create(NULL, _T("MiniClient"), WS_OVERLAPPED | WS_THICKFRAME, CRect(0,0,0,0)))
			return FALSE;
	}

	pFrame->ShowWindow(CWinApp::m_nCmdShow);

	// Fenster zeigen				   
	pFrame->UpdateWindow();
						  					  
	return TRUE;
}									  

/////////////////////////////////////////////////////////////////////////////
int CMiniClientApp::ExitInstance() 
{
	WK_DELETE(m_pMainWnd);
	
	// Debugger schlieﬂen
	CloseDebugger();

	return CWinApp::ExitInstance();
}




