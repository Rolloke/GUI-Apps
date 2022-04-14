// SystemVerwaltungDll.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxdllx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// static AFX_EXTENSION_MODULE SystemVerwaltungDllDLL = { NULL, NULL };

extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);
	UNREFERENCED_PARAMETER(hInstance);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("SystemVerwaltungDll.DLL Initializing!\n");
		
		// RKE: Special behaviour
		// This CDynLinkLibrary object will not be attached to the
		// Regular DLL's resource chain
		// it is used as the standard resource handle in the application
		// Do not initialize it !
//		if (!AfxInitExtensionModule(SystemVerwaltungDllDLL, hInstance))
//			return 0;
//		new CDynLinkLibrary(SystemVerwaltungDllDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("SystemVerwaltungDll.DLL Terminating!\n");

		// RKE: Do not Terminate, because it has not been initialized
//		AfxTermExtensionModule(SystemVerwaltungDllDLL);
	}
	return 1;   // ok
}
