// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__D92F673B_5514_45FC_BE9B_97239A44F520__INCLUDED_)
#define AFX_STDAFX_H__D92F673B_5514_45FC_BE9B_97239A44F520__INCLUDED_

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


#include <wk.h>
#include <wk_names.h>
#include <wk_msg.h>
#include <WK_Dongle.h>
#include <WK_Profile.h>
#include <wk_trace.h>
#include <wk_file.h>
#include <wk_util.h>
#include <WKClasses\WK_PerfMon.h>
#include <Skins\Skins.h>
#include <oemgui\oemguiapi.h>

#include <i7000.h>


#define REGISTRY_LOCATION_PORTS _T("DVRT\\DV\\ICP_Client\\Ports")
#define REG_LOC_PORTS_UNIT      _T("DVRT\\ICPConUnit\\Ports")
#define ICP_ITEMID              _T("ItemID")
#define ICP_MODE                _T("Mode")
#define ICP_BAUDRATE	           _T("Baudrate")
#define ICP_NAME	              _T("Name")
#define ICP_INPUTS              _T("Inputs")
#define ICP_OUTPUTS             _T("Outputs")
#define ICP_CONTROL             _T("Control")
#define ICP_MULTIPLE_ACCESS     _T("MultipleAccess")
#define ICP_POLL_STATE			  _T("PollState")
#define ICP_MODULE_LOCATION	  _T("ModuleLocation")

#define REGISTRY_LOCATION       _T("DVRT\\DV\\ICP_Client")
#define REG_LOC_UNIT			     _T("DVRT\\ICPConUnit")
#define SETTINGS_SECTION        _T("Settings")
#define ICP_SKINCOLOR           _T("SkinColor")
#define ICP_BUTTON_SHAPE        _T("ButtonShape")
#define ICP_BUTTON_DX           _T("ButtonDX")
#define ICP_BUTTON_DY           _T("ButtonDY")
#define ICP_TIMEOUT             _T("TimeOut")
#define ICP_COMPORT             _T("ComPort")
#define ICP_MINIMIZE_TIME       _T("MinimizeTime")
#define ICP_POLLTIME            _T("Polltime")
#define ICP_TIMEOUT_EDGE        _T("TimeoutEdge")
#define ICP_MINIMZE_POS         _T("MinimizePosition")
#define ICP_BAUDRATES           _T("Baudrates")
#define ICP_WINDOWRECT			  _T("WindowRect")

#define FLAG_MULTIPLE_ACCESS		     0x00000001
#define FLAG_POLL_STATE		        0x00000002

#define ComportClosed 0
#define ComportOpened 1

#define REG_CLOSE_KEY(hKey) {if (hKey) {RegCloseKey(hKey);hKey = NULL;}}

#define USER_MSG_MINIMIZE           1
#define USER_MSG_RESTORE            2
#define USER_MSG_KILLTIMER          3
#define USER_MSG_POLL_THREADS_READY 4

#define SAFE_KILL_TIMER(nT) { if (nT) { KillTimer(nT); nT = 0; }}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__D92F673B_5514_45FC_BE9B_97239A44F520__INCLUDED_)
