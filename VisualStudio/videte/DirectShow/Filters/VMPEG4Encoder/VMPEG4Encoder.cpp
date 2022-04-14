//
// VMPEG4Encoder.cpp
//
// VMPEG4Encoder

#include <streams.h>     // DirectShow (includes windows.h)

#include <initguid.h>    // declares DEFINE_GUID to declare an EXTERN_C const.
                         // Use once per project.
#include "VMPEG4Encoder.h"

//
// Common DLL routines and tables
//

// Object table - all com objects in this DLL
CFactoryTemplate g_Templates[]=
{   { L"VMPEG4Encoder"
    , &CLSID_VMPEG4ENCODER
    , CVMPEG4Encoder::CreateInstance
    , NULL
    , &CVMPEG4Encoder::sudFilter 
    }
};
int g_cTemplates = sizeof(g_Templates)/sizeof(g_Templates[0]);

// The streams.h DLL entrypoint.
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);

// The entrypoint required by the MSVC runtimes. This is used instead
// of DllEntryPoint directly to ensure global C++ classes get initialised.
BOOL WINAPI DllMain(HANDLE hDllHandle, DWORD dwReason, LPVOID lpreserved) {

    return DllEntryPoint(reinterpret_cast<HINSTANCE>(hDllHandle), dwReason, lpreserved);
}


STDAPI DllRegisterServer()
{
    return AMovieDllRegisterServer2(TRUE);
}

STDAPI DllUnregisterServer()
{
    return AMovieDllRegisterServer2(FALSE);
}



//
// Filter VMPEG4Encoder routines
//

// setup data - allows the self-registration to work.
const AMOVIESETUP_FILTER CVMPEG4Encoder::sudFilter = {
    g_Templates[0].m_ClsID    // clsID
  , g_Templates[0].m_Name     // strName
  , MERIT_NORMAL              // dwMerit
  // TODO: Add pin details here.
  , 0                         // nPins
  , NULL                      // lpPin
};  


//
// Constructor
//
// If any part of construction fails *phr should be set to 
// a failure code.
CVMPEG4Encoder::CVMPEG4Encoder(TCHAR *tszName, LPUNKNOWN punk, HRESULT *phr)
: CBaseFilter(tszName, punk, &m_FilterLock, *sudFilter.clsID)
, m_FilterLock()
{
}


//
// Destructor
//
CVMPEG4Encoder::~CVMPEG4Encoder() {
}


//
// CreateInstance
//
// Provide the way for the COM support routines in <streams.h>
// to create a CVMPEG4Encoder object
CUnknown * WINAPI CVMPEG4Encoder::CreateInstance(LPUNKNOWN punk, HRESULT *phr) {

    CUnknown *pNewObject = new CVMPEG4Encoder(NAME("VMPEG4Encoder Object"), punk, phr );
    if (pNewObject == NULL) {
        *phr = E_OUTOFMEMORY;
    }

    return pNewObject;
}

//
// GetPinCount
//
// return the number of pins this filter has
int CVMPEG4Encoder::GetPinCount() {

    // TODO: replace this if you create variable numbers of pins at runtime
    return sudFilter.nPins;
}


//
// GetPin
//
// return pointer to a pin.
CBasePin* CVMPEG4Encoder::GetPin(int n) {

    // TODO: Create each pin, and return
    // a pointer to the requested pin.
    return NULL;
}
