// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__C2210977_F324_11D2_8D05_004005A11E32__INCLUDED_)
#define AFX_STDAFX_H__C2210977_F324_11D2_8D05_004005A11E32__INCLUDED_

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

#include <direct.h>
#include <io.h>

#include "wk.h"
#include "wk_trace.h"
#include "wk_names.h"
#include "wk_file.h"

#include "CIPC.h"
#include "Host.h"
#include "NotificationMessage.h"
#include "User.h"
#include "Permission.h"
#include "Imagn.h"
#include "oemgui\oemguiapi.h"
#include "oemgui\oemlogindialog.h"


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__C2210977_F324_11D2_8D05_004005A11E32__INCLUDED_)
