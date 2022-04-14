// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__60D2705C_9E3A_11D2_B54A_004005A19028__INCLUDED_)
#define AFX_STDAFX_H__60D2705C_9E3A_11D2_B54A_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcview.h>
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


#include <wk.h>
#include <wk_names.h>
#include <wkclasses/wk_profile.h>
#include <wkclasses/wk_trace.h>
#include <wk_dongle.h>
#include <wkclasses/wk_util.h>
#include <wkclasses/UnhandledException.h>
#include <wk_msg.h>

#include <CIPC.h>
#include <CIPCServerControl.h>
#include <NotificationMessage.h>

#include <oemgui/oemguiapi.h>
#include <luca/luca.h>

#include <wkclasses/wk_ascii.h>
#include <wkclasses/wk_string.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__60D2705C_9E3A_11D2_B54A_004005A19028__INCLUDED_)
