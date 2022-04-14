/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: StdAfx.h $
// ARCHIVE:		$Archive: /Project/TestPrograms/SDITest/StdAfx.h $
// CHECKIN:		$Date: 20.01.06 13:39 $
// VERSION:		$Revision: 14 $
// LAST CHANGE:	$Modtime: 20.01.06 13:38 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__2D22C51B_A2C7_11D1_9F29_0000C036AC0D__INCLUDED_)
#define AFX_STDAFX_H__2D22C51B_A2C7_11D1_9F29_0000C036AC0D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <afxsock.h>
#endif // _AFX_NO_AFXCMN_SUPPORT

//#include "idipapi.h"
#include "wk.h"
#include "WK_DebugOptions.h"
#include "wkclasses\WK_Trace.h"
//#include "WK_Dongle.h"
#include "wkclasses\wk_profile.h"
#include "wk_names.h"
#include "wk_version.h"
#include "oemgui\\oemguiapi.h"
//#include "WK_RuntimeError.h"
//#include "CipcStringDefs.h"
//#include "PostFetch.h"
//#include "SecID.h"
#include "ascii.h"
#include "wkclasses\wk_rs232.h"
#include "wkclasses\wk_dialog.h"
#include "wkclasses\SDIControl.h"
#include "wkclasses\SDIProtocol.h"
#include "wkclasses\wk_file.h"
#include "wkclasses\wk_util.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__2D22C51B_A2C7_11D1_9F29_0000C036AC0D__INCLUDED_)
