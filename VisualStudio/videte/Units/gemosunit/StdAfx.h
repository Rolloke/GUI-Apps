// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__B1CFA5E7_4B32_11D3_9978_004005A19028__INCLUDED_)
#define AFX_STDAFX_H__B1CFA5E7_4B32_11D3_9978_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WM_CMD_RECEIVED WM_USER + 1

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#include <afxmt.h>
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

// include
#include <wk.h>
#include <wk_msg.h>

 // cipc
#include <wk_names.h>
#include <CipcServerControl.h>
#include <CipcServerControlClientSide.h>
#include <CIPCField.h>
#include <WK_Dongle.h>
#include <WK_DebugOptions.h>
#include <host.h>

// wkclasses
#include <wkclasses\wk_util.h>
#include <wkclasses\wk_profile.h>
#include <wkclasses\wk_runtimeerror.h>
#include <wkclasses\wk_ascii.h>
#include <wkclasses\wk_rs232.h>
#include <wkclasses\wk_file.h>
#include <wkclasses\wk_string.h>

#include <oemgui\oemguiapi.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__B1CFA5E7_4B32_11D3_9978_004005A19028__INCLUDED_)
