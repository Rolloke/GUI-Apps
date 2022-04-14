// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxmt.h>
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#include <afxsock.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "wkclasses\wk_profile.h"
#include "wk_names.h"
#include "wk_version.h"
#include "wk.h"
#include "wkclasses\wk_util.h"
#include "wkclasses\WK_Trace.h"
#include "WK_DebugOptions.h"
#include "WK_Dongle.h"
#include "WK_Msg.h"
#include "wkclasses\UnhandledException.h"
#include "wkclasses\WK_Xml.h"

#include "oemgui\\oemguiapi.h"
#include "wkclasses\wk_ascii.h"
#include "wkclasses\SDIProtocol.h"
#include "wkclasses\Params.h"
#include "wkclasses\wk_rs232.h"
#include "wkclasses\sdicontrol.h"
#include "wkclasses\wk_Time.h"
#include "wkclasses\wk_String.h"
#include "SecID.h"
#include "CipcField.h"
#include "SystemTime.h"
