/////////////////////////////////////////////////////////////////////////////
// PROJECT:		HAccDll
// FILE:		$Workfile: StdAfx.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha(Developer)/HAccDll/StdAfx.h $
// CHECKIN:		$Date: 5.01.04 9:52 $
// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 5.01.04 9:25 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__5CFCAAC8_97C1_11D1_BC01_00A024D29804__INCLUDED_)
#define AFX_STDAFX_H__5CFCAAC8_97C1_11D1_BC01_00A024D29804__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#include <winioctl.h>

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC OLE automation classes
#endif // _AFX_NO_OLE_SUPPORT


#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "DirectAccess.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__5CFCAAC8_97C1_11D1_BC01_00A024D29804__INCLUDED_)
