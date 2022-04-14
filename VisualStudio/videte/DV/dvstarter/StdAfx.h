// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__2E0EDA39_A6F8_11D3_8DFA_004005A11E32__INCLUDED_)
#define AFX_STDAFX_H__2E0EDA39_A6F8_11D3_8DFA_004005A11E32__INCLUDED_

//ML{
#if _MSC_VER < 1300
#define _WIN32_WINNT 0x0400
#endif
//ML}
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define SETBIT(w,b)    ((DWORD)((DWORD)(w) | (1L << (b))))
#define CLRBIT(w,b)    ((DWORD)((DWORD)(w) & ~(1L << (b))))
#define TSTBIT(w,b)    ((BOOL)((DWORD)(w) & (1L << (b)) ? TRUE : FALSE))

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxsock.h>
#include <winuser.h>
#include "PSAPI.H"

#include "WK.h"
#include "wkclasses\WK_Trace.h"
#include "WK_Names.h"
#include "WK_Dongle.h"
#include "WK_Timer.h"
#include "WK_Msg.h"
#include "wkclasses\WK_File.h"
#include "wkclasses\WK_Util.h"
#include "wkclasses\WK_RuntimeErrors.h"

#include "wkclasses\wk_wincpp.h"
#include "wkclasses\Runservice.h"
#include "wkclasses\cata.h"
#include "wkclasses\csmart.h"
#include "wkclasses\wk_string.h"
#include <wkclasses\cdrive.h>
#include <wkclasses\cewf.h>
#include <wkclasses\CHDDTApi.h>

#include "SystemTime.h"

#include "..\Lib\Skins\Skins.h"

#ifdef _OEM_GUI_API_H
#pragma warning("Oemgui in DVStarter cause dongle problems")
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__2E0EDA39_A6F8_11D3_8DFA_004005A11E32__INCLUDED_)
