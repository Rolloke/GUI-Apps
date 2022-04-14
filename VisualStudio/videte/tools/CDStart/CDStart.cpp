// CDStart.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "CDStart.h"
#include "CDStartDlgBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCDStartApp
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CCDStartApp, CWinApp)
	//{{AFX_MSG_MAP(CCDStartApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CCDStartApp construction
/////////////////////////////////////////////////////////////////////////////
CCDStartApp::CCDStartApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CCDStartApp object

CCDStartApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CCDStartApp initialization
BOOL CCDStartApp::InitInstance()
{
	RegisterWindowClass();
	// nur eine Instanz starten
	if ( WK_ALONE(szAppName)==FALSE )
	{
		HWND hWndFirstInstance = FindWindow(szAppName,NULL);
		// Trotz RegisterWindowClass wird kein Fenster gefunden!
		if (hWndFirstInstance)
		{
			SetWindowPos(hWndFirstInstance,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
			SetWindowPos(hWndFirstInstance,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
		}
		return FALSE;
	}
#ifdef _DEBUG
	InitDebugger(_T("CDStart.log"),WAI_INVALID);
	SET_WK_STAT_TICK_COUNT(GetTickCount());
#endif

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#if _MFC_VER < 0x0700
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

	// Main window is a dlg
	CCDStartDlgBase dlg;

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
void CCDStartApp::RegisterWindowClass()
{
	WNDCLASS  wndclass;

	// register window class
	wndclass.style =         CS_DBLCLKS|CS_HREDRAW;
	wndclass.lpfnWndProc =   DefWindowProc;
	wndclass.cbClsExtra =    0;
	wndclass.cbWndExtra =    0;
	wndclass.hInstance =     m_hInstance;
	wndclass.hCursor =       LoadCursor(IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1) ;
	wndclass.lpszMenuName =  0L;
	wndclass.lpszClassName = szAppName;
	wndclass.hIcon = LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME));

	AfxRegisterClass(&wndclass);
}
/////////////////////////////////////////////////////////////////////////////
int CCDStartApp::ExitInstance() 
{
#ifdef _DEBUG
	CloseDebugger();
#endif
	
	return CWinApp::ExitInstance();
}
