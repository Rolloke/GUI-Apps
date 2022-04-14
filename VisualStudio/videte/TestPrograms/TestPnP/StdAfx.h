// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__06FFAB33_B1D5_48CD_A4B9_20A2D6C0E477__INCLUDED_)
#define AFX_STDAFX_H__06FFAB33_B1D5_48CD_A4B9_20A2D6C0E477__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#ifdef _DEBUG
   #define WINVER 0x0501
#endif

//#include <Windows.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls

#include <DeviceDetect\DeviceDetect.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__06FFAB33_B1D5_48CD_A4B9_20A2D6C0E477__INCLUDED_)
