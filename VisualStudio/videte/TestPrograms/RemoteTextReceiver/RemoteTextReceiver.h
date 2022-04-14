#pragma once

// RemoteTextReceiver.h : main header file for RemoteTextReceiver.DLL

#if !defined( __AFXCTL_H__ )
#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols


// CRemoteTextReceiverApp : See RemoteTextReceiver.cpp for implementation.

class CRemoteTextReceiverApp : public COleControlModule
{
public:
   CRemoteTextReceiverApp() {m_bSocketInitialized = FALSE;};
	BOOL InitInstance();
	int ExitInstance();

   BOOL m_bSocketInitialized;
};

extern CRemoteTextReceiverApp NEAR theApp;

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

