// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__B0D14219_49F3_11D5_99F0_004005A19028__INCLUDED_)
#define AFX_STDAFX_H__B0D14219_49F3_11D5_99F0_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WM_NOTIFY_CONNECT			WM_USER + 4
#define WM_NOTIFY_DISCONNECT		WM_USER + 5
#define WM_NOTIFY_ALARMLIST_UPDATE	WM_USER + 6

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "wk.h"
#include "systemtime.h"
#include "connectionrecord.h"
#include "cipcstringdefs.h"
#include "ipcfetch.h"
#include "cipcdatabaseclient.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__B0D14219_49F3_11D5_99F0_004005A19028__INCLUDED_)

#include "..\Lib\Skins\Skins.h"
