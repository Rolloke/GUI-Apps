// LangDll.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxdllx.h>
#include "LangDll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////
static AFX_EXTENSION_MODULE LangDllDLL = { NULL, NULL };

//////////////////////////////////////////////////////////////////////
extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE(_T("DVLanguage.DLL %s Initializing!\n"), GetLanguageName());
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(LangDllDLL, hInstance))
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

		new CDynLinkLibrary(LangDllDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE(_T("DVLanguage.DLL %s Terminating!\n"), GetLanguageName());
		// Terminate the library before destructors are called
		AfxTermExtensionModule(LangDllDLL);
	}
	return 1;   // ok
}
//////////////////////////////////////////////////////////////////////
extern "C" LPCTSTR GetLanguageName(void)
{
// CAVEAT must be extended for new languages
#ifdef DV_CHS
	#ifdef _DEBUG
		return _T("Chinese_Debug");
   #else
		return _T("Chinese");
	#endif
#elif DV_CSY
	#ifdef _DEBUG
		return _T("Cestina_Debug");
   #else
		return _T("Cestina");
	#endif
#elif DV_DEU
	#ifdef _DEBUG
		return _T("Deutsch_Debug");
   #else
		return _T("Deutsch");
	#endif
#elif DV_ENU
	#ifdef _DEBUG
		return _T("English_Debug");
   #else
		return _T("English");
	#endif
#elif DV_ESP
	return _T("Español");
#elif DV_FRA
	return _T("Français");
#elif DV_ITA
	return _T("Italiano");
#elif DV_NLD
	return _T("Nederlands");
#elif DV_NLB
	return _T("Vlaams");
#elif DV_NOR
	return _T("Norge");
#elif DV_PLK
	return _T("Polski");
#elif DV_PTG
	return _T("Português");
#elif DV_SVE
	return _T("Svenska");
#elif DV_WLS
	return _T("Cymraeg");
#elif DV_RUS
	#ifdef _DEBUG
		return _T("Russkij_Debug");
   #else
		return _T("Russkij");
	#endif
#elif DV_JPN
	#ifdef _DEBUG
		return _T("Japanese_Debug");
   #else
		return _T("Japanese");
	#endif
#else
	// must be empty, dll will be ignored then
	TRACE(_T("DVLanguage.DLL Language UNKNOWN\n"));
	return _T("");
#endif
}
//////////////////////////////////////////////////////////////////////
extern "C" LPCTSTR GetLanguageAbbreviation(void)
{
// CAVEAT must be extended for new languages
#ifdef DV_CHS
	return _T("chs");
#elif DV_CSY
	return _T("csy");
#elif DV_DEU
	return _T("deu");
#elif DV_ENU
	return _T("enu");
#elif DV_ESP
	return _T("esp");
#elif DV_FRA
	return _T("fra");
#elif DV_ITA
	return _T("ita");
#elif DV_NLD
	return _T("nld");
#elif DV_NLB
	return _T("nlb");
#elif DV_NOR
	return _T("nor");
#elif DV_PLK
	return _T("plk");
#elif DV_PTG
	return _T("ptg");
#elif DV_SVE
	return _T("sve");
#elif DV_WLS
	return _T("wls");
#elif DV_RUS
	return _T("rus");
#elif DV_JPN
	return _T("jpn");
#else
	// must be empty, dll will be ignored then
	TRACE(_T("DVLanguage.DLL Abbreviation UNKNOWN\n"));
	return _T("");
#endif
}
//////////////////////////////////////////////////////////////////////
extern "C" LPCTSTR GetCodePage(void)
{
// CAVEAT must be extended for new languages
#ifdef DV_CHS
	return _T("936");
#elif DV_CSY
	return _T("1250");
#elif DV_DEU
	return _T("1252");
#elif DV_ENU
	return _T("1252");
#elif DV_ESP
	return _T("1252");
#elif DV_FRA
	return _T("1252");
#elif DV_ITA
	return _T("1252");
#elif DV_NLD
	return _T("1252");
#elif DV_NLB
	return _T("1252");
#elif DV_NOR
	return _T("1252");
#elif DV_PLK
	return _T("1250");
#elif DV_PTG
	return _T("1252");
#elif DV_SVE
	return _T("1252");
#elif DV_WLS
	return _T("1252");
#elif DV_RUS
	return _T("1251");
#elif DV_JPN
	return _T("932");
#else
	// must be empty, dll will be ignored then
	TRACE(_T("DVLanguage.DLL CodePage UNKNOWN\n"));
	return _T("");
#endif
}

extern "C" DWORD GetCodePageBits(void)
{
// CAVEAT must be extended for new languages
#ifdef DV_CHS
	return 1 << 18; // Chinese: Simplified chars PRC and Singapore
#elif DV_CSY
	return 0;
#elif DV_DEU
	return 0;
#elif DV_ENU
	return 0;
#elif DV_ESP
	return 0;
#elif DV_FRA
	return 0;
#elif DV_ITA
	return 0;
#elif DV_NLD
	return 0;
#elif DV_NLB
	return 0;
#elif DV_NOR
	return 0;
#elif DV_PLK
	return 0;
#elif DV_PTG
	return 0;
#elif DV_SVE
	return 0;
#elif DV_WLS
	return 0;
#elif DV_RUS
	return 1 << 1; // Cyrillic 
#elif DV_JPN
	return 1 << 17; // Japanese, Shift-JIS
#else
	// must be empty, dll will be ignored then
	TRACE(_T("DVLanguage.DLL CodePageBits UNKNOWN\n"));
	return _T("");
#endif
}
