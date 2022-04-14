#pragma once

// RemoteCurveControl.h : main header file for RemoteCurveControl.DLL

#if !defined( __AFXCTL_H__ )
#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols


// CRemoteCurveControlApp : See RemoteCurveControl.cpp for implementation.

class CRemoteCurveControlApp : public COleControlModule
{
public:
    CRemoteCurveControlApp();
	BOOL InitInstance();
	int ExitInstance();
    BOOL InitSocket();

    BOOL m_bSocketInitialized;

};

extern CRemoteCurveControlApp NEAR theApp;

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

