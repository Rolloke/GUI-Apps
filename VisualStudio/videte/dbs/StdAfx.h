// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__AB5B2367_3834_11D2_B58E_00C095EC9EFA__INCLUDED_)
#define AFX_STDAFX_H__AB5B2367_3834_11D2_B58E_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcview.h>
#include <afxdisp.h>        // MFC OLE automation classes
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <dbt.h>

// #define _WK_CLASSES_DLL

#ifdef _WK_CLASSES_DLL
 #define WK_AFX_EXT_CLASS AFX_EXT_CLASS
 #pragma message("WK_Classes as DLL")
#else
 #define WK_AFX_EXT_CLASS
 #pragma message("WK_Classes as Lib")
#endif

#include <wk.h>
#include <wk_names.h>
#include <wkclasses\wk_profile.h>
#include <wkclasses\wk_trace.h>
#include <wk_dongle.h>
#include <wkclasses\wk_file.h>
#include <wkclasses\wk_util.h>

#include <wk_msg.h>

#include <CIPCInt64.h>
#include <CIPCDrives.h>
#include <CIPC.h>
#include <CIPCServerControl.h>
#include <CIPCDataBase.h>
#include <PictureDef.h>
#include <CipcExtraMemory.h>
#include <PictureRecord.h>
#include <SystemTime.h>
#include <cipcstringdefs.h>

#include <User.h>
#include <Permission.h>
#include <Host.h>
#include <ArchivInfo.h>

#include <oemgui/oemguiapi.h>
#include "oemgui\autorun.h"
#include <wkclasses/wk_stopwatch.h>
#include <wkclasses/wk_thread.h>
#include <wkclasses/statistic.h>
#include <wkclasses/wk_debugger.h>
#include <wkclasses/wk_wincpp.h>
#include <wkclasses/wk_string.h>

#if _MSC_VER >= 1300
	#include <codebase65/d4all.hpp>
#else
	#include <c4lib/d4all.hpp>
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__AB5B2367_3834_11D2_B58E_00C095EC9EFA__INCLUDED_)
