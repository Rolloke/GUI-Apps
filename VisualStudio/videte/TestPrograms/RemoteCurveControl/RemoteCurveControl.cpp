// RemoteCurveControl.cpp : Implementation of CRemoteCurveControlApp and DLL registration.

#include "stdafx.h"
#include "RemoteCurveControl.h"
#include "RemoteCurveControlCtrl.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning (disable : 4996)

CRemoteCurveControlApp NEAR theApp;

#ifdef USE_REGISTER_SAFETY
HRESULT CreateComponentCategory(CATID catid, WCHAR* catDescription);
HRESULT RegisterCLSIDInCategory(REFCLSID clsid, CATID catid, BOOL bRegister=TRUE);
//extern GUID SafeID;
GUID SafeID = CRemoteCurveControlCtrl::guid;
#endif

const GUID CDECL BASED_CODE _tlid =
        { 0x494E8D4A, 0xD219, 0x4FB2, { 0x8B, 0x6F, 0x6B, 0xA4, 0x3D, 0xB5, 0x8, 0x8A } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;

CRemoteCurveControlApp::CRemoteCurveControlApp():
m_bSocketInitialized(FALSE)
{
}

// CRemoteCurveControlApp::InitInstance - DLL initialization

BOOL CRemoteCurveControlApp::InitInstance()
{
    BOOL bInit = COleControlModule::InitInstance();

    if (bInit)
    {
        //
    }

    return bInit;
}

BOOL CRemoteCurveControlApp::InitSocket()
{
    if (!m_bSocketInitialized)
    {
        m_bSocketInitialized = AfxSocketInit(NULL);
    }
    return m_bSocketInitialized;
}


// CRemoteCurveControlApp::ExitInstance - DLL termination

int CRemoteCurveControlApp::ExitInstance()
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
