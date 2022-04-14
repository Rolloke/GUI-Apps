// IdipClientDll.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxdllx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// static AFX_EXTENSION_MODULE IdipClientDllDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH) 
	{
		TRACE0("IdipClientDll.DLL Initializing!\n");
		
//		if (!AfxInitExtensionModule(IdipClientDllDLL, hInstance))
//			return 0;
//		new CDynLinkLibrary(IdipClientDllDLL);

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("IdipClientDll.DLL Terminating!\n");

		// Terminate the library before destructors are called
//		AfxTermExtensionModule(IdipClientDllDLL);
	}
	return 1;   // ok
}
