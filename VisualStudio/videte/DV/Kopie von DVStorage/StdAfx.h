// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__F4B60C0D_8ACE_11D3_99EA_004005A19028__INCLUDED_)
#define AFX_STDAFX_H__F4B60C0D_8ACE_11D3_99EA_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

//}}AFX_INSERT_LOCATION

#include <dbt.h>

#include <wk.h>
#include <wk_names.h>
#include <wkclasses\wk_profile.h>
#include <wkclasses\wk_trace.h>
#include <wkclasses\wk_file.h>
#include <wkclasses\wk_runtimeerror.h>

#include <wkclasses\wk_util.h>

#include <CIPCInt64.h>
#include <CIPCDrives.h>
#include <CIPC.h>
#include <CIPCStringDefs.h>
#include <CIPCServerControl.h>
#include <CIPCDataBase.h>
#include <PictureDef.h>
#include <CipcExtraMemory.h>
#include <PictureRecord.h>
#include <SystemTime.h>

#include <User.h>
#include <Permission.h>
#include <Host.h>
#include <ArchivInfo.h>

#include <wkclasses/wk_stopwatch.h>
#include <wkclasses/wk_thread.h>
#include <wkclasses/rsa.h>
#include <wkclasses/wk_wincpp.h>
#include <wkclasses/wk_string.h>
#include <wkclasses/statistic.h>

#if _MSC_VER >= 1300
	#include <codebase65/d4all.hpp>
#else
	#include <c4lib/d4all.hpp>
#endif
#include <wk_msg.h>

#include <vimage\vimage.h>

#ifdef _UNICODE
 #define UTRACE  TRACE
#else
 #define UTRACE //
#endif

#endif // !defined(AFX_STDAFX_H__F4B60C0D_8ACE_11D3_99EA_004005A19028__INCLUDED_)


