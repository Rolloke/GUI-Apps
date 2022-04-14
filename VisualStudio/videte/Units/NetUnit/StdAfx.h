// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__ABCDD07E_5A48_11D5_8700_004005A26A3B__INCLUDED_)
#define AFX_STDAFX_H__ABCDD07E_5A48_11D5_8700_004005A26A3B__INCLUDED_

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

////////////////////////////////////////////////////////////////////////////////////////////////////
//	   SOCKET	WICHTIGE   ÄNDERUNG !!  (RM)
/*
http://support.microsoft.com/support/kb/articles/Q257/4/60.ASP

When setting or getting socket options at the IPPROTO_IP level in a C/C++ Winsock application 
targeted for Windows NT, Windows 2000, Windows 95, or Windows 98, including the correct header 
and library file in the program project is critical. If the header and library files are not 
properly matched, setsockopt or getsockopt may fail with runtime error 10042 (WSAENOPROTOOPT). 
In certain cases, even if the API returns successfully, the option value that you set or get
may not be what you would have expected.
To avoid this problem, observe the following guidelines: 

A program that includes Winsock.h should only link with Wsock32.lib.
A program that includes Ws2tcpip.h may link with either Ws2_32.lib or Wsock32.lib. 
Note that Ws2tcpip.h must be explicitly included after Winsock2.h in order to use socket options 
at this level.
*/

#include <winsock2.h>
#include <ipifcons.h>

#include "wk.h"
#include "WK_Trace.h"
#include "WK_Names.h"
#include <wk_msg.h>

#include "Host.h"
#include "user.h"
#include "permission.h"
#include <Skins/Skins.h> // Skins before OemGui!
#include "OEMGUI/OemGuiApi.h"
#include "OEMGUI/Oemlogindialog.h"



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__ABCDD07E_5A48_11D5_8700_004005A26A3B__INCLUDED_)
