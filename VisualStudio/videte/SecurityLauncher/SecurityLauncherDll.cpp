// SecurityLauncherDll.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxdllx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//static AFX_EXTENSION_MODULE SecurityLauncherDllDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("SecurityLauncherDll.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
//		if (!AfxInitExtensionModule(SecurityLauncherDllDLL, hInstance))
//			return 0;
//		new CDynLinkLibrary(SecurityLauncherDllDLL);

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("SecurityLauncherDll.DLL Terminating!\n");

		// Terminate the library before destructors are called
//		AfxTermExtensionModule(SecurityLauncherDllDLL);
	}
	return 1;   // ok
}
