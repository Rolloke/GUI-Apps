// RemoteTextReceiver.cpp : Implementation of CRemoteTextReceiverApp and DLL registration.

#include "stdafx.h"
#include "RemoteTextReceiver.h"
#include "RemoteTextReceiverCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CRemoteTextReceiverApp NEAR theApp;

#ifdef USE_REGISTER_SAFETY
HRESULT CreateComponentCategory(CATID catid, WCHAR* catDescription);
HRESULT RegisterCLSIDInCategory(REFCLSID clsid, CATID catid, BOOL bRegister=TRUE);
GUID SafeID = CRemoteTextReceiverCtrl::guid;
#endif


const GUID CDECL BASED_CODE _tlid =
		{ 0x12403ECC, 0x2EF1, 0x40F2, { 0x8B, 0xD4, 0xE8, 0x56, 0x4B, 0xC8, 0x2D, 0xE6 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;



// CRemoteTextReceiverApp::InitInstance - DLL initialization

BOOL CRemoteTextReceiverApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	if (bInit)
	{
      m_bSocketInitialized = AfxSocketInit(NULL);
	}

	return bInit;
}



// CRemoteTextReceiverApp::ExitInstance - DLL termination

int CRemoteTextReceiverApp::ExitInstance()
{
	// TODO: Add your own module termination code here.

	return COleControlModule::ExitInstance();
}



// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

#ifdef USE_REGISTER_SAFETY
    if (FAILED( CreateComponentCategory(CATID_SafeForScripting, L"Controls that are safely scriptable")))
        return ResultFromScode(SELFREG_E_CLASS);

    if (FAILED( CreateComponentCategory(CATID_SafeForInitializing, L"Controls safely initializable from persistent data") ))
        return ResultFromScode(SELFREG_E_CLASS);

    if (FAILED( RegisterCLSIDInCategory(SafeID, CATID_SafeForScripting)))
        return ResultFromScode(SELFREG_E_CLASS);

    if (FAILED( RegisterCLSIDInCategory(SafeID, CATID_SafeForInitializing)))
        return ResultFromScode(SELFREG_E_CLASS);
#endif

	return NOERROR;
}



// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

#ifdef USE_REGISTER_SAFETY
    if (FAILED( RegisterCLSIDInCategory(SafeID, CATID_SafeForScripting, FALSE)))
        return ResultFromScode(SELFREG_E_CLASS);

    if (FAILED( RegisterCLSIDInCategory(SafeID, CATID_SafeForInitializing, FALSE)))
        return ResultFromScode(SELFREG_E_CLASS);
#endif

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}

#ifdef USE_REGISTER_SAFETY

HRESULT CreateComponentCategory(CATID catid, WCHAR* catDescription)
{
    ICatRegister* pcr = NULL ;
    HRESULT hr = S_OK ;

    hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr,
                        NULL,
                        CLSCTX_INPROC_SERVER,
                        IID_ICatRegister,
                        (void**)&pcr);
    if (FAILED(hr))
    return hr;

    // Make sure the HKCR\Component Categories\{..catid...}
    // key is registered
    CATEGORYINFO catinfo;
    catinfo.catid = catid;
    catinfo.lcid = 0x0409 ; // english

    // Make sure the provided description is not too long.
    // Only copy the first 127 characters if it is
    int len = wcslen(catDescription);
    if (len>127)
    len = 127;
    wcsncpy(catinfo.szDescription, catDescription, len);
    // Make sure the description is null terminated
    catinfo.szDescription[len] = '\0';

    hr = pcr->RegisterCategories(1, &catinfo);
    pcr->Release();

    return hr;
}

// Helper function to register a CLSID as belonging to a component
// category
HRESULT RegisterCLSIDInCategory(REFCLSID clsid, CATID catid, BOOL bRegister)
{
    // Register your component categories information.
    ICatRegister* pcr = NULL ;
    HRESULT hr = S_OK ;
    hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr,
                        NULL,
                        CLSCTX_INPROC_SERVER,
                        IID_ICatRegister,
                        (void**)&pcr);
    if (SUCCEEDED(hr))
    {
    // Register this category as being "implemented" by
    // the class.
        CATID rgcatid[1] ;
        rgcatid[0] = catid;
        if (bRegister)
        {
            hr = pcr->RegisterClassImplCategories(clsid, 1, rgcatid);
        }
        else
        {
            hr = pcr->UnRegisterClassImplCategories(clsid, 1, rgcatid);
        }
    }

    if (pcr != NULL)
        pcr->Release();

    return hr;
}
#endif
