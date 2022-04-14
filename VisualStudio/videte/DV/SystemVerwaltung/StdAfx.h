// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <wk.h>
#include <wk_names.h>
#include <wk_util.h>
#include <WK_DebugOptions.h>
#include <wk_dongle.h>
#include <wk_file.h>
#include <wk_msg.h>

#include <CIPCInput.h>
#include <CIPCOutput.h>
#include <CIPCDatabase.h>
#include <IPCFetch.h>
#include <CipcServerControlClientSide.h>
#include <PictureDef.h>
#include <CameraControl.h>

#include <wk_profile.h>
#include <SecID.h>
#include <Host.h>
#include <User.h>
#include <Permission.h>
#include <ArchivInfo.h>
#include <NotificationMessage.h>

#include <Skins/Skins.h> // Skins before OemGui!

#include <oemgui\oemguiapi.h>
#include <oemgui\OemFileDialog.h>
#include <oemgui\OemLoginDialog.h>
#include <oemgui\OemConnectionDialog.h>
#include <oemgui\OemCoolbar.h>

#include <wkclasses\WK_Dialog.h>
#include <wkclasses\wk_wincpp.h>
#include <wkclasses\SDIProtocol.h>
#include <wkclasses\ComParameters.h>
#include <wkclasses\Params.h>
#include <wkclasses\coolmenu.h>
#include <wkclasses\WK_Thread.h>

#include <CIPCDrives.h>