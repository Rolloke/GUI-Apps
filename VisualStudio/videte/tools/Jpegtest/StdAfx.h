// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__B3389337_4A42_11D3_8D9F_004005A11E32__INCLUDED_)
#define AFX_STDAFX_H__B3389337_4A42_11D3_8D9F_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "cipc.h"
#include "picturedef.h"
#include "picturerecord.h"
#include <WK_Names.h>
#include <WK_File.h>
#include <WK_Profile.h>
#include "WK_RuntimeError.h"
#include "WK_Util.h"
#include <wk_trace.h>
#include <WKClasses\WK_PerfMon.h>
#include <wkclasses\WK_Thread.h>
#include <Jpeglib\CJpeg.h>


#include <H263enc/H26xEncoder.h>
#include <H263dec/H263.h>
#include <H263dec/dib.h>

#undef __INLINE
#include <ipl/ipl.h>


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__B3389337_4A42_11D3_8D9F_004005A11E32__INCLUDED_)
