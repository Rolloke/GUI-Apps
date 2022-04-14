/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: StdAfx.h $
// ARCHIVE:		$Archive: /Project/Capi/ISDNUnit/StdAfx.h $
// CHECKIN:		$Date: 20.01.06 12:11 $
// VERSION:		$Revision: 22 $
// LAST CHANGE:	$Modtime: 20.01.06 10:56 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

/////////////////////////////////////////////////////////////////////////////
//	stdafx.h :	include file for standard system include files,
//				or project specific include files that are used frequently,
//				but are changed infrequently
//

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows 95 Common Controls
#include <afxmt.h>			// MFC support for Windows 95 Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#ifdef USE_DEBUGGER
	#undef USE_DEBUGGER
#endif

#pragma warning(default: 4705)  // statement has no effect in optimized code
#pragma warning(default: 4701)  // local variable *may* be used without init
#pragma warning(default: 4189)  // initialized but unused variable
#pragma warning(default: 4390)  // empty controlled statement
#pragma warning(default: 4554)  // possible operator precedence error
#pragma warning(default: 4614)  // vararg promotion
#pragma warning(default: 4035)  // 'function' : no return value
#pragma warning(default: 4702)  // unreachable code

#pragma warning(default: 4244)	// 'conversion' conversion from 'type1' to 'type2', possible loss of data
#pragma warning(default: 4311)   // type cast truncation
#pragma warning(default: 4706) // assignment within conditional


#ifndef _DEBUG
// only for release, since ASSERT causes a lot of warnings
#pragma warning(default: 4127)  // possible operator precedence error
#pragma warning(disable: 4710)  // C4710: 'void __cdecl AfxTrace(const char *,...)' : function not expanded/not inlined
#endif

// wkclasses
#include <wkclasses\wk_util.h>
#include <wkclasses\WK_Trace.h>
#include <WKClasses\WK_String.h>
#include <WKClasses\WK_RunTimeError.h>	// for WAI_.....

// cipc
#include <wk.h>
#include <wk_names.h>
#include <Cipc.h>
#include <SystemTime.h>
// include
#include <Host.h>
#include <User.h>
#include <Permission.h>
#include <wk_msg.h>

// Skins before OemGui
#include <Skins/Skins.h>
#include <OemGui/OemGuiApi.h>
#include <OemGui/Oemlogindialog.h>
#include <DeviceDetect/DeviceDetect.h>

