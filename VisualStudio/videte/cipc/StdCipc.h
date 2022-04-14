
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxmt.h>
#include <afxtempl.h>
#include <afxcmn.h>

#ifndef _AFX_NO_AFXCMN_SUPPORT
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <stdlib.h>

// #define _WK_CLASSES_DLL

#ifdef _WK_CLASSES_DLL
 #define WK_AFX_EXT_CLASS AFX_EXT_CLASS
#else
 #define WK_AFX_EXT_CLASS
#endif

#include "wk.h"
#include "wk_dongle.h"
#include "WKClasses\wk_string.h"
#include "WKClasses\wk_util.h"
#include "WKClasses\wk_trace.h"
#include "WKClasses\wk_file.h"
#include "wkclasses\CLogFile.h"
#include "wkclasses\getlongpathname.h"
#include "wkclasses\wk_profile.h"
#include <malloc.h>

#ifdef _UNICODE
 #define UTRACE  TRACE
#else
 #define UTRACE //
#endif

#if _MSC_VER < 1300
	#ifndef __FUNCTION__
		#define __FUNCTION__ __FILE__
	#endif
#endif


