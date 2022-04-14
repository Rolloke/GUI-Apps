// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__026DFF3A_E8CB_40EC_A597_10D6529FCC16__INCLUDED_)
#define AFX_STDAFX_H__026DFF3A_E8CB_40EC_A597_10D6529FCC16__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcview.h>
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <math.h>
#include <dbt.h>

#include <wk.h>
#include <WK_Names.h>
#include <WK_DebugOptions.h>
#include <WK_RunTimeError.h>
#include <wk_profile.h>
#include <wk_file.h>
#include <wk_dongle.h>
#include <wk_msg.h>

#include <ErrorMessage.h>

#include <SecID.h>

#include <CipcExtraMemory.h>
#include <CIPCStringDefs.h>
#include <PictureDef.h>
#include <CameraControl.h>

#include <IPCFetch.h>
#include <CIPCDataBase.h>
#include <CIPCDataBaseClient.h>
#include <TimedMessage.h>
#include <PictureRecord.h>
#include <SystemTime.h>

#include <wk_util.h>

// User/Permission
#include <User.h>
#include <Permission.h>
#include <Host.h>
#include <NotificationMessage.h>
#include <AutoLogout.h>

#include <wkclasses/coolmenu.h>
#include <wkclasses/wk_dialog.h>
#include <wkclasses/wk_thread.h>
#include <wkclasses\wk_wincpp.h>
#include <wkclasses\explorermenu.h>
#include <wkclasses/wk_perfmon.h>

#include <oemgui/oemguiapi.h>
#include <oemgui/OemFileDialog.h>
#include <oemgui/OemLoginDialog.h>
#include <oemgui/OemConnectionDialog.h>
#include <oemgui\OEMCoolBar.h>


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__026DFF3A_E8CB_40EC_A597_10D6529FCC16__INCLUDED_)
