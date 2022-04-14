// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__6B5BBEC7_9757_11D3_A870_004005A19028__INCLUDED_)
#define AFX_STDAFX_H__6B5BBEC7_9757_11D3_A870_004005A19028__INCLUDED_

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


#include <Iprtrmib.h>
#include <Iphlpapi.h>
#include <math.h>
#include <dbt.h>
#include <locale.h>

#include <wk.h>
#include <wk_names.h>
#include <WK_Dongle.h>
#include <wkclasses\wk_profile.h>
#include <wkclasses\wk_trace.h>
#include <wkclasses\wk_file.h>
#include <wkclasses\wk_util.h>
#include <wkclasses\WK_RuntimeError.h>

#include <SecID.h>
#include <wk_msg.h>
#include <Host.h>

#include <CipcExtraMemory.h>
#include <CIPCStringDefs.h>
#include <PictureDef.h>
#include <CameraControl.h>

#include <IPCFetch.h>
#include <CIPCDataBaseClient.h>
#include <CIPCInput.h>
#include <CIPCOutput.h>
#include <CIPCInputClient.h>
#include <CIPCOutputClient.h>
#include <TimedMessage.h>
#include <PictureRecord.h>
#include <CipcOutputRecord.h>
#include <WK_Timer.h>

#include <wkclasses/wk_thread.h>
#include <wkclasses/wk_wincpp.h>
#include <wkclasses/RunService.h>
#include <wkclasses/wk_stopwatch.h>
#include <wkclasses/wk_perfmon.h>
#include <wkclasses/wk_Utilities.h>
#include <wkclasses/statistic.h>
#include <wkclasses\wkControlledProcess.h>
#include <Skins\Skins.h>

#include <DeviceDetect\DeviceDetect.h>

#include "..\LangDll\LangDll.h"

#include <vimage\cjpeg.h>
#include <vimage\mpeg4decoder.h>
#include <vimage\CAVCodec.h>
#include "Enumerations.h"

#define CAMERA_FPS_STEPS 9
#define _SWITCHABLE_LIVE_WINDOWS_ 1

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__6B5BBEC7_9757_11D3_A870_004005A19028__INCLUDED_)

