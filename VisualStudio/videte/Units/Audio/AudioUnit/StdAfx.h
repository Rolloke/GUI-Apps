// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__D37CB68F_A26E_4495_99DC_A7130AFCD62A__INCLUDED_)
#define AFX_STDAFX_H__D37CB68F_A26E_4495_99DC_A7130AFCD62A__INCLUDED_

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

// include 
#include <WK.h>
#include <WK_Msg.h>

// cipc
#include <cipc.h>
#include <WK_Names.h>
#include <WK_Dongle.h>
#include <WK_Timer.h>
#include <User.h>		// User/Permission
#include <Permission.h>
#include <CIPCMedia.h>
#include <MediaSampleRecord.h>
#include <WK_Template.h>

// WKClasses
#include <WKClasses\WK_Trace.h>
#include <WKClasses\WK_File.h>
#include <WKClasses\WK_Util.h>
#include <WKClasses\WK_RuntimeError.h>
#include <WKClasses\WK_PerfMon.h>
#include <WKClasses\WK_Profile.h>
#include <WKClasses\wk_string.h>

// misc
#include <Skins/Skins.h> // Skins before OemGui!
#include <oemgui\oemlogindialog.h>
#include <DeviceDetect\DeviceDetect.h>

#define AUDIO_INPUT  1
#define AUDIO_OUTPUT 2


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__D37CB68F_A26E_4495_99DC_A7130AFCD62A__INCLUDED_)
