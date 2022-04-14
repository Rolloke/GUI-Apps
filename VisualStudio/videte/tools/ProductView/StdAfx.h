// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__3688BAF9_1955_11D1_B8C8_0060977A76F1__INCLUDED_)
#define AFX_STDAFX_H__3688BAF9_1955_11D1_B8C8_0060977A76F1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <afxmt.h>
#include <afxtempl.h>

#endif // _AFX_NO_AFXCMN_SUPPORT

#include "wk_msg.h"
#include "wkclasses\WK_Trace.h"
#include "SystemTime.h"
#include "OEMGui/OemGuiApi.h"
#include "wkclasses\WK_File.h"
#include "wkclasses\WK_util.h"
#include "wkclasses/rsa.h"
#include "wkclasses/wk_utilities.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__3688BAF9_1955_11D1_B8C8_0060977A76F1__INCLUDED_)
