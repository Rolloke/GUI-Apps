// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__517E1EFC_9D66_11D1_8C1B_00C095EC9EFA__INCLUDED_)
#define AFX_STDAFX_H__517E1EFC_9D66_11D1_8C1B_00C095EC9EFA__INCLUDED_

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

// gf ???
#include <afxdtctl.h>


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
#include <IPCDataCarrier.h>

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
#include <wkclasses\MonitorInfo.h>
#include <wkclasses\wk_string.h>

#include <Skins/Skins.h> // Skins before OemGui!
#include <oemgui/oemguiapi.h>
#include <oemgui/OemFileDialog.h>
#include <oemgui/OemLoginDialog.h>
#include <oemgui/OemConnectionDialog.h>
#include <oemgui\OEMCoolBar.h>

#include <vimage/vimage.h>

/////////////////////////////////////////////////////////////////////////////
// Messages
#define WM_USER_UPDATE_AND_ENABLE_CONTROLS		WM_USER + 0x0002
#define WM_USER_HANDLE_CD_AND_DVD_INFO			WM_USER + 0x0003
#define WM_USER_BACKUP_DATA_CARRIER_FINISHED	WM_USER + 0x0004

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__517E1EFC_9D66_11D1_8C1B_00C095EC9EFA__INCLUDED_)
