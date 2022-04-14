/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaDA
// FILE:		$Workfile: TashaDA.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaDA/TashaDA.cpp $
// CHECKIN:		$Date: 26.03.04 12:13 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 25.03.04 11:10 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TashaDA.h"

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
// CTashaDAApp
BEGIN_MESSAGE_MAP(CTashaDAApp, CWinApp)
	//{{AFX_MSG_MAP(CTashaDAApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTashaDAApp construction

CTashaDAApp::CTashaDAApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTashaDAApp object

CTashaDAApp theApp;

/////////////////////////////////////////////////////////////////////////////
BOOL CTashaDAApp::InitInstance() 
{
	// Gibt Auskunft darüber ob Windows95 oder Windows NT läuft.
	OSVERSIONINFO VersionInformation;
	VersionInformation.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&VersionInformation);

	if (VersionInformation.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		// Lade den WinNT Gerätetreiber
		g_hDriver	= CreateFile(_T("\\\\.\\Tasha.sys"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		g_bWin95	= FALSE;
	}

	if (g_hDriver == INVALID_HANDLE_VALUE)
		FALSE;
							
//	g_hWndHook = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)HookProc, AfxGetInstanceHandle(), 0);
    	
	return CWinApp::InitInstance();
}

/////////////////////////////////////////////////////////////////////////////
int CTashaDAApp::ExitInstance() 
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

									  