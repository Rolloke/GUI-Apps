// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__348AC803_D017_11D4_9983_004005A19028__INCLUDED_)
#define AFX_STDAFX_H__348AC803_D017_11D4_9983_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#define VC_EXTRALEAN        // Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxsock.h>		// MFC socket extensions

#include <stdio.h>
#include <stdlib.h>
#include <c4lib/d4all.hpp>

#ifdef _DEBUG
#undef printf
#define printf TRACE
#endif

#include <cgi\tag.h>
#include <cgi\environment.h>
#include <cgi\email.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__348AC803_D017_11D4_9983_004005A19028__INCLUDED_)
