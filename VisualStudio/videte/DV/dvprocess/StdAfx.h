// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__43C9E491_8B84_11D3_99EB_004005A19028__INCLUDED_)
#define AFX_STDAFX_H__43C9E491_8B84_11D3_99EB_004005A19028__INCLUDED_

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


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__43C9E491_8B84_11D3_99EB_004005A19028__INCLUDED_)

#include <wk.h>
#include <wk_names.h>
#include <wkclasses\wk_profile.h>
#include <wkclasses\wk_trace.h>
#include <wkclasses\wk_file.h>

#include <wkclasses\wk_util.h>
#include <WK_Timer.h>
#include <wkclasses\WK_runtimeerror.h>

#include "CIPCStringDefs.h"
#include <CIPCInt64.h>
#include <CIPC.h>
#include <CIPCServerControl.h>
#include <CIPCServerControlClientSide.h>
#include <CIPCInputRecord.h>
#include <CIPCInput.h>
#include <IPCInputFileUpdate.h>
#include <CIPCOutput.h>
#include <CIPCOutputRecord.h>
#include <CIPCDataBaseClient.h>
#include <CIPCMedia.h>
#include <PictureDef.h>
#include <CipcExtraMemory.h>
#include <PictureRecord.h>
#include <IPCFetch.h>
#include <SystemTime.h>

#include <User.h>
#include <Permission.h>
#include <Host.h>
#include <ArchivInfo.h>
#include <SecTimer.h>

#include <wkclasses/wk_stopwatch.h>
#include <wkclasses/wk_thread.h>
#include <wkclasses/rsa.h>
#include <wk_msg.h>
#include <wkclasses/wk_wincpp.h>
#include <wkclasses/WK_PerfMon.h>
#include <wkclasses/wk_debugger.h>

#include <vimage\vimage.h>
#include <vimage\cavcodec.h>
#include <HardwareInfo\\CBoards.h>
