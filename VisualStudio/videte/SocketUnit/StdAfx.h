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

#include <afxsock.h>		// MFC socket extensions
#include "wk.h"
#include "Cipc.h"
#include <CIPCStringDefs.h>
#include "wkclasses\WK_Trace.h"
#include "WK_Names.h"
#include "WK_Version.h"
#include "wkclasses\WK_RunTimeError.h"
#include "wkclasses\WK_RunTimeErrors.h"
#include "wkclasses\WK_Profile.h"
#include "wkclasses\WK_String.h"
#include "wkclasses\statistic.h"
#include <IPCFetch.h>
#include <wk_msg.h>

#include "wkclasses\wk_util.h"
#include "wkclasses\wk_utilities.h"

#include "Host.h"
#include "user.h"
#include "permission.h"

#include "WK_Dongle.h"
#include <Skins/Skins.h> // Skins before OemGui!
#include "OEMGUI/OemGuiApi.h"
#include "OEMGUI/Oemlogindialog.h"



