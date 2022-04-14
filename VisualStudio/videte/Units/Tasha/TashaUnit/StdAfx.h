/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaUnit
// FILE:		$Workfile: StdAfx.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaUnit/StdAfx.h $
// CHECKIN:		$Date: 6.12.04 12:09 $
// VERSION:		$Revision: 16 $
// LAST CHANGE:	$Modtime: 6.12.04 12:05 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__6742D40D_CF0D_45B0_9A6F_B940D23FDEA0__INCLUDED_)
#define AFX_STDAFX_H__6742D40D_CF0D_45B0_9A6F_B940D23FDEA0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#include <mmsystem.h>
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <cipc.h>
#include <WKClasses\WK_PerfMon.h>
#include <WKClasses\WK_Utilities.h>

#include <WK_Names.h>
#include <WK_Timer.h>

#include <WK_RuntimeError.h>
#include <videojob.h>
#include <VImage\mpeg4decoder.h>

#include "TashaUnitDefs.h"
#include "TashaProfile.h"
#include "..\TashaDll\CDebug.h"
#include "..\TashaDll\CCodec.h"


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__6742D40D_CF0D_45B0_9A6F_B940D23FDEA0__INCLUDED_)
