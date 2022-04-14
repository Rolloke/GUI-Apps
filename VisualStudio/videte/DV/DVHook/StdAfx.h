// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__43389139_6C29_11D4_8F46_004005A11E32__INCLUDED_)
#define AFX_STDAFX_H__43389139_6C29_11D4_8F46_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "Psapi.h"

#include <wk.h>
#include <wk_names.h>
#include <wkclasses\wk_trace.h>
#include <wkclasses\wk_util.h>
#include <wkclasses\wk_applicationid.h>
#include <wk_msg.h>
#include <wkclasses\wk_profile.h>
#include "..\Lib\Hooks\CHook.h"
#include <wkclasses/wk_wincpp.h>

#define DV_HIDE			_T("Hide")
#define DV_SHOW			_T("Show")
#define DV_IGNORE			_T("Ignore")
#define DV_CLOSE			_T("Close")
#define DV_CONTROL		_T("Control")
#define DV_PASSWORD		_T("PASSWORD")
#define DV_PARENT			_T("Parent")
#define DV_ENABLE			_T("ENABLE")
#define DV_GETTEXT		_T("GETTEXT")
#define DV_SETTEXT		_T("SETTEXT")
#define DV_COMMAND		_T("COMMAND")
#define DV_SYSCOMMAND	_T("SYSCOMMAND")
#define DV_FINDFILE		_T("FINDFILE")
#define DV_TIMEOUT		_T("TimeOut")
#define DV_TOP				_T("Top")
#define DV_TOPMOST		_T("TopMost")
#define DV_BOTTOM			_T("Bottom")
#define DV_NOTOPMOST		_T("NoTopMost")


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__43389139_6C29_11D4_8F46_004005A11E32__INCLUDED_)
