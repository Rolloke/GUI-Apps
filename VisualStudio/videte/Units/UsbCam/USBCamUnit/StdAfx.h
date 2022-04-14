// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__382A7E61_51C2_4B3A_B8EE_D02D0E15D87D__INCLUDED_)
#define AFX_STDAFX_H__382A7E61_51C2_4B3A_B8EE_D02D0E15D87D__INCLUDED_

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

#include <strmif.h>
#include <uuids.h>
#include <dshow.h>

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

#include "Picturedef.h"

// User/Permission
#include "User.h"
#include "Permission.h"
#include "oemgui\oemlogindialog.h"
#include "DeviceDetect\DeviceDetect.h"

#define WAI_USBCAMUNIT_1 60
#define WAI_USBCAMUNIT_2 61
#define WAI_USBCAMUNIT_3 62
#define WAI_USBCAMUNIT_4 63

#define WK_APP_NAME_USBCAMUNIT1 "USBCamUnit1"
#define WK_APP_NAME_USBCAMUNIT2 "USBCamUnit2"
#define WK_APP_NAME_USBCAMUNIT3 "USBCamUnit3"
#define WK_APP_NAME_USBCAMUNIT4 "USBCamUnit4"

#define SM_USBCAM_INPUT         "USBCamUnitInput"
#define SM_USBCAM_MD_INPUT		  "USBCamUnitMdInput"

#define MAKEAPPID(ID) (WK_ApplicationId)ID

#define MAX_CAMERAS	16

#define SETBIT(w,b)    ((DWORD)((DWORD)(w) | (1L << (b))))
#define CLRBIT(w,b)    ((DWORD)((DWORD)(w) & ~(1L << (b))))
#define TSTBIT(w,b)    ((BOOL)((DWORD)(w) & (1L << (b)) ? TRUE : FALSE))
#define TOGGLEBIT(w,b) (TSTBIT(w,b) ? CLRBIT(w,b) : SETBIT(w,b))

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__382A7E61_51C2_4B3A_B8EE_D02D0E15D87D__INCLUDED_)
