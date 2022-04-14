// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__7643F030_2B62_4D1C_BA90_AD3A20726C54__INCLUDED_)
#define AFX_STDAFX_H__7643F030_2B62_4D1C_BA90_AD3A20726C54__INCLUDED_

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
#include <process.h>

//cipc
#include "cipc.h"
#include "WK_Names.h"
#include "WK_Dongle.h"
#include "WK_Timer.h"

// include
#include "WK.h"
#include "WK_Msg.h"

// wkclasses
#include "WKClasses\WK_Trace.h"
#include "WKClasses\WK_File.h"
#include "WKClasses\WK_Util.h"
#include "WKClasses\WK_RuntimeError.h"
#include "WKClasses\WK_PerfMon.h"
#include "WKClasses\WK_Profile.h"
#include "WKClasses\MonitorInfo.h"

// misc
#include "Skins\Skins.h"
#include "oemgui\oemguiapi.h"

#define POLL_TIMER     0x0815

#define USER_MSG_POLL_THREADS_READY 1
#define USER_MSG_ALARM					2
#define USER_MSG_PORT_ERROR			3
#define USER_MSG_MODULE_ERROR			4
#define USER_MSG_MODULE_LAST_ERROR	USER_MSG_MODULE_ERROR + 24
#define USER_MSG_MINIMIZE           30
#define USER_MSG_RESTORE            31
#define USER_MSG_REQUEST_RESET		32
#define USER_MSG_RESTART_TIMER		33
#define USER_MSG_INIT_LIST				34

#define STAND_ALONE		_T("StandAlone")

#define SAFE_KILL_TIMER(nT) { if (nT) { KillTimer(nT); nT = 0; }}

//#define _TEST_TRACES 1


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__7643F030_2B62_4D1C_BA90_AD3A20726C54__INCLUDED_)
