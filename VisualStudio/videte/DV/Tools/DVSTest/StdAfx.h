// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__C13369D9_8673_11D5_9A29_004005A19028__INCLUDED_)
#define AFX_STDAFX_H__C13369D9_8673_11D5_9A29_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <wkclasses/wk_dialog.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#include <wk.h>
#include <wkclasses\wk_trace.h>
#include <cipcdatabaseclient.h>
#include <IPCFetch.h>
#include <host.h>
#include <wkclasses\wk_util.h>
#include <wk_dongle.h>

#endif // !defined(AFX_STDAFX_H__C13369D9_8673_11D5_9A29_004005A19028__INCLUDED_)

