// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__DC0CC411_57FB_4D7B_84AC_A31978F75095__INCLUDED_)
#define AFX_STDAFX_H__DC0CC411_57FB_4D7B_84AC_A31978F75095__INCLUDED_

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

#include "cipc.h"
#include "WK.h"
#include "WK_Trace.h"
#include "WK_Names.h"
#include "WK_File.h"
#include "WK_Util.h"
#include "WK_RuntimeError.h"
#include "WKClasses\WK_PerfMon.h"
#include "WK_Profile.h"
#include "WK_Dongle.h"
#include "WK_Timer.h"
#include "WK_Msg.h"
#include "DeviceDetect\DeviceDetect.h"
#include "cameracontrol.h"

#include <oemgui\oemguiapi.h>

#include "..\MotionDetection\CMotionDetection.h"


#define SETBIT(w,b)    ((DWORD)((DWORD)(w) | (1L << (b))))
#define CLRBIT(w,b)    ((DWORD)((DWORD)(w) & ~(1L << (b))))
#define TSTBIT(w,b)    ((BOOL)((DWORD)(w) & (1L << (b)) ? TRUE : FALSE))
#define TOGGLEBIT(w,b) (TSTBIT(w,b) ? CLRBIT(w,b) : SETBIT(w,b))

#define EVENT_UPDATE_FRAME		200
#define TIMER_DEV_CHANGED		201
#define TIMER_DEV_UPDATE		202
#define TIMER_RESET      		203

#define EVENT_VIDEO_JOB_MIN	204
#define EVENT_VIDEO_JOB_MAX	EVENT_VIDEO_JOB_MIN + MAX_CAMERAS

#define TIMER_PTZ_CAM_MIN 		EVENT_VIDEO_JOB_MAX + 2
#define TIMER_PTZ_CAM_MAX  	TIMER_PTZ_CAM_MIN   + MAX_CAMERAS

#define TIMER_ALARM_MIN 		TIMER_PTZ_CAM_MAX   + 2
#define TIMER_ALARM_MAX       TIMER_ALARM_MIN     + MAX_CAMERAS

#define TEST_WITH_SAVIC 1

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__DC0CC411_57FB_4D7B_84AC_A31978F75095__INCLUDED_)
