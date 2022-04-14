/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: StdAfx.h $
// ARCHIVE:		$Archive: /Project/Clients/SDIConfig/StdAfx.h $
// CHECKIN:		$Date: 15.01.03 14:31 $
// VERSION:		$Revision: 10 $
// LAST CHANGE:	$Modtime: 15.01.03 14:29 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__3716C0EA_5B7E_11D1_8143_EAF88A4ACC28__INCLUDED_)
#define AFX_STDAFX_H__3716C0EA_5B7E_11D1_8143_EAF88A4ACC28__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <wk.h>
#include <WK_DebugOptions.h>
#include <WK_Trace.h>
#include <WK_Dongle.h>
#include <wk_profile.h>
#include <wk_names.h>
#include <wk_version.h>

#include <user.h>
#include <permission.h>
#include <host.h>
#include <WK_RuntimeError.h>
#include <CipcStringDefs.h>
#include <ConnectionRecord.h>
#include <CIPCFetchResult.h>
#include <SecID.h>

#include <oemgui\oemguiapi.h>
#include <oemgui\oemlogindialog.h>
#include <oemgui\oemcoolbar.h>
#include <wkclasses\coolmenu.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__3716C0EA_5B7E_11D1_8143_EAF88A4ACC28__INCLUDED_)
