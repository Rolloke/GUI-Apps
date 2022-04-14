// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A287F4BA_E872_4BF0_A8B0_E7B8FCD89216__INCLUDED_)
#define AFX_STDAFX_H__A287F4BA_E872_4BF0_A8B0_E7B8FCD89216__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(push, 3)
#pragma warning (disable:4514) // unreferenced inline function has been removed
#pragma warning (disable:4512) // assignment operator could not be generated
#pragma warning (disable:4511) // copy constructor could not be generated
#pragma warning (disable:4127) // conditional expression is constant
#pragma warning (disable:4514) // conditional expression is constant

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <iostream>

#ifdef _UNICODE
 #define wkstrcmp     wcscmp
#else
 #define wkstrcmp     strcmp
#endif

#pragma warning(pop)
// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A287F4BA_E872_4BF0_A8B0_E7B8FCD89216__INCLUDED_)
