// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxole.h>
#include <afxcview.h>
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows 95 Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


#include <wk.h>
#include <wk_names.h>
#include <wkclasses\wk_profile.h>
#include <wkclasses\wk_trace.h>
#include <wk_msg.h>

#include <SecID.h>
#include <Host.h>
#include <User.h>
#include <Permission.h>

#include <CipcInputClient.h>
#include <CipcExtraMemory.h>
#include <CIPCStringDefs.h>
#include <IPCFetch.h>

#include <wkclasses\logziplib\LogZip.h>
#include <wkclasses\wk_file.h>
#include <wkclasses\wk_util.h>

#include <Skins/Skins.h> // Skins before OemGui!

#include <oemgui/oemguiapi.h>
#include <oemgui/OemFileDialog.h>
#include <oemgui/OemLoginDialog.h>
#include <oemgui/OemConnectionDialog.h>

#include "wkclasses\wk_string.h"


