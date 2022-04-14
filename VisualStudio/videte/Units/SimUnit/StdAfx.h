// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows 95 Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "wk.h"
#include "wk_msg.h"
#include "systemtime.h"
#include "wkclasses\wk_dialog.h"
#include <WKClasses\wk_string.h>
#include <WKClasses\wk_util.h>
#include <WKClasses\wk_trace.h>
#include "WK_names.h"
#include "WK_Dongle.h"
#include "wkclasses\wk_utilities.h"
#include "OEMGui/oemguiapi.h"

#include <Skins\Skins.h> // must be included before oemgui!
#include <OEMGui\oemguiapi.h>
