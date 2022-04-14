// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__C30DFED8_962B_4D55_8DD3_15A957FD7DA4__INCLUDED_)
#define AFX_STDAFX_H__C30DFED8_962B_4D55_8DD3_15A957FD7DA4__INCLUDED_

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

#include <mmsystem.h>

#include "cipc.h"
#include "WK.h"
#include "WK_Trace.h"
#include "WK_File.h"
#include "WK_Profile.h"
#include "WK_Dongle.h"

#include "Picturedef.h"

#define SETBIT(w,b)    ((DWORD)((DWORD)(w) | (1L << (b))))
#define CLRBIT(w,b)    ((DWORD)((DWORD)(w) & ~(1L << (b))))
#define TSTBIT(w,b)    ((BOOL)((DWORD)(w) & (1L << (b)) ? TRUE : FALSE))
#define TOGGLEBIT(w,b) (TSTBIT(w,b) ? CLRBIT(w,b) : SETBIT(w,b))

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__C30DFED8_962B_4D55_8DD3_15A957FD7DA4__INCLUDED_)
