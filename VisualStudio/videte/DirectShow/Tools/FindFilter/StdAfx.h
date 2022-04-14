// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__21C4655C_7E1C_4F88_B79B_896A5F0DD766__INCLUDED_)
#define AFX_STDAFX_H__21C4655C_7E1C_4F88_B79B_896A5F0DD766__INCLUDED_

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

#include <math.h>
#include <atlbase.h>
#include "Streams.h"

#include "common\mfcutil.h"
#include "common\dshowutil.h"
#include "include\dmoreg.h"
#include "common\namedguid.h"

#include "qedit.h"
#include <KS.h>
#include <KSMEDIA.h>

#define FIND_FILTER 1

struct ErrorStruct
{
	ErrorStruct(HRESULT hr, int nLine, const char*sFile) 
	{
		m_hr = hr;
		m_nLine = nLine;
		m_sFile = sFile;
	}
	int         m_nLine;
	HRESULT     m_hr;
	const char* m_sFile;
};

#define HRESULT_ERROR_AT_POS(hr) new ErrorStruct(hr, __LINE__, __FILE__)


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__21C4655C_7E1C_4F88_B79B_896A5F0DD766__INCLUDED_)
