#if !defined(AFX_MCLT_H__1F6B1756_D657_4470_A023_A93887F05E34__INCLUDED_)
#define AFX_MCLT_H__1F6B1756_D657_4470_A023_A93887F05E34__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// MCLT.h : main header file for MCLT.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CMCLTApp : See MCLT.cpp for implementation.

class CMCLTApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MCLT_H__1F6B1756_D657_4470_A023_A93887F05E34__INCLUDED)
