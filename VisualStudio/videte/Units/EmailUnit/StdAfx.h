// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__662471B9_0EB0_11D4_A9E6_004005A26A3B__INCLUDED_)
#define AFX_STDAFX_H__662471B9_0EB0_11D4_A9E6_004005A26A3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxsock.h>		// MFC socket extensions
#include <ras.h>
#include <CIPC.h>
#include <CIPCServerControl.h>
#include <WK_names.h>
#include <CIPCInputClient.h>
#include <CIPCOutputClient.h>

#include "constants.h"

#include "wkclasses\WK_Trace.h"
#include <wk.h>
#include <wk_msg.h>
#include <wkclasses\WK_THREAD.h>
#include <wkclasses\wk_util.h>
#include <wkclasses\wk_applicationid.h>
#include <NotificationMessage.h>
#include <wkclasses\wk_profile.h>
#include <vimage\vimage.h>
#include <crypto\base64.h>
#include <Skins/Skins.h> // must be included before oemgui!
#include <oemgui/oemguiapi.h>
#include <wkclasses\wk_file.h>
#include <wkclasses\wk_string.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__662471B9_0EB0_11D4_A9E6_004005A26A3B__INCLUDED_)
