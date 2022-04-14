// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__CFA641EF_A66E_4D33_A6F8_0FBF83114CF2__INCLUDED_)
#define AFX_STDAFX_H__CFA641EF_A66E_4D33_A6F8_0FBF83114CF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#define _AFX_NO_DB_SUPPORT
#define _AFX_NO_DAO_SUPPORT

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT


#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_STDAFX_H__CFA641EF_A66E_4D33_A6F8_0FBF83114CF2__INCLUDED_)

#include "WK_Profile.h"
#include "WK_File.h"
#include "WK_RuntimeError.h"
#include "CIPCStringDefs.h"
#include <WKClasses\WK_PerfMon.h>
#include <WKClasses\WK_String.h>
#include <WKClasses\WK_Utilities.h>

#include "TashaTrace.h"
#include "..\TashaUnit\TashaUnitDefs.h"
#include "..\TashaUnit\TashaProfile.h"
#include "..\TashaFirmware\TashaFirmware535\SharedMemoryDef.h"
#include "..\TashaFirmware\Include\TMMessages.h"

#include "mdsdsp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



