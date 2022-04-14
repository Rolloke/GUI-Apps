// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__46825268_6AF6_11D1_93E4_00C095EC9EFA__INCLUDED_)
#define AFX_STDAFX_H__46825268_6AF6_11D1_93E4_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC OLE automation classes
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcview.h>
#include <afxmt.h>
#include <afxpriv.h>

// GF include für VS_70 <occimpl.h>
#if _MFC_VER == 0x0600
	#include <..\src\afximpl.h>
#else if _MFC_VER >= 0x0700
	#include <afximpl.h>
	#include <afxocc.h>
	#include <ocdb.h>
#endif


#include <math.h>

#include <wk.h>
#include <WK_Names.h>
#include <WK_DebugOptions.h>
#include <WK_RunTimeError.h>
#include <wk_profile.h>
#include <wk_dongle.h>
#include <wk_file.h>
#include <wk_util.h>
#include <wk_msg.h>
#include <wk_timer.h>
#include <wkclasses\wk_wincpp.h>
#include <ErrorMessage.h>

#include <SecID.h>

#include <CipcExtraMemory.h>
#include <CIPCStringDefs.h>
#include <PictureDef.h>
#include <CameraControl.h>

#include <IPCFetch.h>
#include <CipcServerControl.h>
#include <CipcServerControlClientSide.h>
#include <CIPCDataBase.h>
#include <CIPCDataBaseClient.h>
#include <CIPCInput.h>
#include <CIPCOutput.h>
#include <CIPCInputClient.h>
#include <CIPCOutputClient.h>
#include <TimedMessage.h>
#include <PictureRecord.h>
#include <CipcOutputRecord.h>
#include <CIPCMediaRecord.h>

// User/Permission
#include <User.h>
#include <Permission.h>
#include <Host.h>
#include <AutoLogout.h>
#include <NotificationMessage.h>

#include <wkclasses\wk_dialog.h>
#include <wkclasses\wk_string.h>
#include <wkclasses\coolmenu.h>
#include <Skins/Skins.h> // Skins before OemGui!
#include <oemgui\oemguiapi.h>
#include <oemgui\oemconnectiondialog.h>
#include <oemgui\oemlogindialog.h>
#include <oemgui\OEMCoolBar.h>
#include <oemgui\oemdatetimedialog.h>
#include <wkclasses\MonitorInfo.h>

#include <vimage/vimage.h>


#define VISION_SETTINGS _T("Vision\\Settings")
#define	USE_MONITOR		_T("UseMonitor")

#define GET_MONITOR_RECT			0x1000
#define REJECT_ANNOUNCEMENT			0x2000
#define NO_MEDIA_RECEIVE_CONFIRM	0x3000

#ifdef _DTS
	#undef _HTML_ELEMENTS
#else
	#define _HTML_ELEMENTS
#endif

#ifndef IDC_HAND
  #define IDC_HAND            MAKEINTRESOURCE(32649)
#endif

void DDV_MinChars(CDataExchange* pDX, CString const& value, int nChars, int nID);

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__46825268_6AF6_11D1_93E4_00C095EC9EFA__INCLUDED_)
