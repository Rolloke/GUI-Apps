// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__332722AD_04ED_432B_BE74_B390CEFCAAF6__INCLUDED_)
#define AFX_STDAFX_H__332722AD_04ED_432B_BE74_B390CEFCAAF6__INCLUDED_

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

/////////////////////////////////////////////////////////////////////////////
#include <afxmt.h>
#include <afxtempl.h>
/////////////////////////////////////////////////////////////////////////////
// CIPC
#include <wk.h>
#include <wk_template.h>
#include <wk_profile.h>
#include <wk_trace.h>
#include <CameraControl.h>

#include <wkclasses/wk_rs232.h>
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__332722AD_04ED_432B_BE74_B390CEFCAAF6__INCLUDED_)
