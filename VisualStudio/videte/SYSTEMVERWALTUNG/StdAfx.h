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

// #define _WK_CLASSES_DLL

#ifdef _WK_CLASSES_DLL
 #define WK_AFX_EXT_CLASS AFX_EXT_CLASS
 #pragma message("WK_Classes as DLL")
#else
 #define WK_AFX_EXT_CLASS
 #pragma message("WK_Classes as Lib")
#endif

// CIPC
// include
#include <wk.h>
#include <wk_msg.h>

// cipc
#include <wk_names.h>
#include <WK_DebugOptions.h>
#include <wk_dongle.h>
#include <CIPCInput.h>
#include <CIPCOutput.h>
#include <CIPCDatabase.h>
#include <IPCFetch.h>
#include <CipcServerControlClientSide.h>
#include <PictureDef.h>
#include <CIPCDrives.h>
#include <CameraControl.h>
#include <CIPCMedia.h>
#include <SecID.h>
#include <ArchivInfo.h>
#include <NotificationMessage.h>
#include <Host.h>			// User/Permission
#include <User.h>
#include <Permission.h>

// Skins
#include <Skins/Skins.h> // Skins before OemGui!

// OEMGUI
#include <oemgui\oemguiapi.h>
#include <oemgui\OemFileDialog.h>
#include <oemgui\OemLoginDialog.h>
#include <oemgui\OemConnectionDialog.h>
#include <oemgui\OemCoolbar.h>
#include <oemgui\OEMPasswordDialog.h>

// WK Classes
#include <wkclasses\wk_util.h>
#include <wkclasses\wk_file.h>
#include <wkclasses\WK_Dialog.h>
#include <wkclasses\wk_wincpp.h>
#include <wkclasses\SDIProtocol.h>
#include <wkclasses\SDIControl.h>
#include <wkclasses\ComParameters.h>
#include <wkclasses\Params.h>
#include <wkclasses\coolmenu.h>
#include <wkclasses\WK_Thread.h>
#include <wkclasses\MonitorInfo.h>
#include <wkclasses\wk_rs232.h>
#include <wkclasses\wk_time.h>
#include <wkclasses\wk_string.h>
#include <wkclasses\CEwf.h>
#include <wkclasses\joystickdx.h>
#include <wkclasses\wk_profile.h>
#include <wkclasses\wk_ascii.h>

// Misc
#include <process.h>
#include <math.h>
#include <DeviceDetect\DeviceDetect.h>


// #define SDI_SAMPLE_READ_IN		WM_USER + 1 // -> defined in SDIConfigurationDialog.h
#define WM_SELECT_PAGE     WM_USER + 2
//      LPARAM                      WPARAM
#define SELECT_FROM_STRING_ID 0x00000001  // Resourcen ID
#define SELECT_FROM_SEC_ID    0x00000002  // CSecID
#define SELECT_FROM_STRING    0x00000004  // String
#define SELECT_TEST_ONLY      0x00010000  // Nur Testen

#define WM_SET_WIZARD_SIZE WM_USER + 3
#define WM_GET_WPAGE_PARAM WM_USER + 4
