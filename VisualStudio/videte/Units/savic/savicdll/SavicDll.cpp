/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: SavicDll.cpp $
// ARCHIVE:		$Archive: /Project/Units/SaVic/SavicDll/SavicDll.cpp $
// CHECKIN:		$Date: 20.06.03 12:01 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 20.06.03 11:35 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <afxdllx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HINSTANCE g_hInstance = NULL;

static AFX_EXTENSION_MODULE SaVicDll = { NULL, NULL };


extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("SaVic.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(SaVicDll, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(SaVicDll);
		g_hInstance = hInstance;				  
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{													   
		TRACE0("SaVic.DLL Terminating!\n");
	
		// Der letzte macht das Licht aus.
		// Wichtig: Funktioniert nur, wenn der Shraredmemoryname _T('SaVicUnit') lautet.
		HGLOBAL	hShared = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, TRUE, _T("SaVicUnit"));
		if (hShared != NULL)
		{
			DWORD* pShared = (DWORD*) MapViewOfFile(hShared, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
			if (pShared)
				UnmapViewOfFile(pShared);
		}
		
		// Terminate the library before destructors are called
		AfxTermExtensionModule(SaVicDll);
	}
	return 1;   // ok
}
