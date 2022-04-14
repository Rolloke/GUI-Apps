// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__6742D40D_CF0D_45B0_9A6F_B940D23FDEA0__INCLUDED_)
#define AFX_STDAFX_H__6742D40D_CF0D_45B0_9A6F_B940D23FDEA0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#include <mmsystem.h>
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <cipc.h>
#include <WK_PerfMon.h>
#include <WK_Names.h>
#include <WK_Timer.h>

#include "WK_RuntimeError.h"
#include <videojob.h>
#include "mdsdsp.h"
#include "TashaUnitDefs.h"
#include "TashaProfile.h"
#include "..\TashaFirmware\TashaFirmware535\SharedMemoryDef.h"
#include "..\TashaFirmware\Include\TMMessages.h"
#include "..\HAccDll\DirectAccess.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__6742D40D_CF0D_45B0_9A6F_B940D23FDEA0__INCLUDED_)
