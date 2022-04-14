// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#pragma once

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "WK_Msg.h"

#define ZERO_INIT(X) ZeroMemory(&X, sizeof(X))

#define UNITS_100NS_TO_MILLISECONDS(T) (long)(T / 10000)

#ifdef _DEBUG
  #define DEBUG_TRACE(s) CPushSource::Trace(s);
#else
  #define DEBUG_TRACE //
#endif
#define RELEASE_TRACE(s) CPushSource::Trace(s);
