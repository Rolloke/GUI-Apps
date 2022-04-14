/////////////////////////////////////////////////////////////////////////////
// PROJECT:		HAccDll
// FILE:		$Workfile: HAccDll.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha(Developer)/HAccDll/HAccDll.cpp $
// CHECKIN:		$Date: 5.01.04 9:52 $
// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 5.01.04 9:25 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HAccDll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Handle des Gerätetreibers
HANDLE	g_hDriver					= INVALID_HANDLE_VALUE;

// TRUE -> Win95
BOOL	g_bWin95					= TRUE;

HHOOK g_hWndHook	= NULL;
 
/////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		TRACE("MessageHook(%d, %lu, %lu)\n", nCode, wParam, lParam);
	}

	return CallNextHookEx(g_hWndHook, nCode, wParam, lParam);
}									   

/////////////////////////////////////////////////////////////////////////////
// CHAccApp
BEGIN_MESSAGE_MAP(CHAccApp, CWinApp)
	//{{AFX_MSG_MAP(CHAccApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHAccApp construction

CHAccApp::CHAccApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CHAccApp object

CHAccApp theApp;

/////////////////////////////////////////////////////////////////////////////
BOOL CHAccApp::InitInstance() 
{
	// Gibt Auskunft darüber ob Windows95 oder Windows NT läuft.
	OSVERSIONINFO VersionInformation;
	VersionInformation.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&VersionInformation);

	if (VersionInformation.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		// Lade den WinNT Gerätetreiber
		g_hDriver	= CreateFile("\\\\.\\HAcc.sys", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		g_bWin95	= FALSE;
	}
	else
	{
		// Lade den Win95 Gerätetreiber
		g_hDriver	= CreateFile("\\\\.\\HAcc.vxd", 0,0,NULL, OPEN_EXISTING, FILE_FLAG_DELETE_ON_CLOSE, 0);
		g_bWin95	= TRUE;
	}

	if (g_hDriver == INVALID_HANDLE_VALUE)
		FALSE;
							
//	g_hWndHook = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)HookProc, AfxGetInstanceHandle(), 0);
    	
	return CWinApp::InitInstance();
}

/////////////////////////////////////////////////////////////////////////////
int CHAccApp::ExitInstance() 
{
	if (g_hDriver != INVALID_HANDLE_VALUE)
	{
		// Entlade den Gerätetreiber
		if(CloseHandle(g_hDriver))
			g_hDriver = INVALID_HANDLE_VALUE;
	}

//	UnhookWindowsHookEx(g_hWndHook);
	g_hWndHook = NULL;

	return CWinApp::ExitInstance();
}

									  