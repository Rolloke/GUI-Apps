// AddIn.cpp : Implementierung von DLL-Exporten.

#include "stdafx.h"
#include "resource.h"
#include "AddIn.h"

CAddInModule _AtlModule;


// DLL-Einstiegspunkt
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	_AtlModule.SetResourceInstance(hInstance);
	return _AtlModule.DllMain(dwReason, lpReserved); 
}


// Wird verwendet, um festzustellen, ob die DLL von OLE entladen werden kann.
STDAPI DllCanUnloadNow(void)
{
	return _AtlModule.DllCanUnloadNow();
}


// Gibt eine Klassenfactory zurück, um ein Objekt vom angeforderten Typ zu erstellen.
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


// DllRegisterServer - Fügt der Systemregistrierung Einträge hinzu.
STDAPI DllRegisterServer(void)
{
	// Registriert Objekt, Typelib und alle Schnittstellen in Typelib.
	HRESULT hr = _AtlModule.DllRegisterServer();
	return hr;
}


// DllUnregisterServer - Entfernt Einträge aus der Systemregistrierung.
STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer();
	return hr;
}
