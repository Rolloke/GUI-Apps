/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: wk.h $
// ARCHIVE:		$Archive: /Project/CIPC/wk.h $
// CHECKIN:		$Date: 20.01.06 9:28 $
// VERSION:		$Revision: 22 $
// LAST CHANGE:	$Modtime: 19.01.06 17:03 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef _WK_HEADER_H_
#define _WK_HEADER_H_

#include <afxwin.h>
#include <afxtempl.h>
#include <afxmt.h>

// defines for global strings ( Registry, SharedMemoryNames etc )
#define WK_RegistryKey			_T("w+k VideoCommunication")
#define WK_SecurityKey			_T("Security\\3.00")

// delete macros, that check for NULL AND reset to NULL
#define WK_DELETE(ptr)		{if(ptr){delete ptr; ptr=NULL;}}
#define WK_DELETE_ARRAY(ptr) {if(ptr){delete [] ptr; ptr=NULL;}}

#define WK_DESTROY_WINDOW(pWnd) \
{ \
	if(pWnd) { \
		if (pWnd->m_hWnd && IsWindow(pWnd->m_hWnd)) { \
			if (pWnd->DestroyWindow()) { \
				pWnd = NULL; \
			} \
		} \
		else { \
			pWnd = NULL; \
		} \
	} \
}

// same for CloseHandle, 
// WARNING it's an inlined functions which uses a reference to a handle!
inline BOOL WK_CLOSE_HANDLE(HANDLE & h) {
	BOOL bResult = FALSE;
	if (h) {
		bResult = CloseHandle(h);
		if (bResult) {
			h = NULL;
		}
	}
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL WK_IS_WINDOW(const CWnd* pWnd)
{
	 return (pWnd && pWnd->m_hWnd && IsWindow(pWnd->m_hWnd));
}
/////////////////////////////////////////////////////////////////////////////
inline DWORD GetTimeSpan(DWORD dwStartTime)
{
	DWORD dwTC = GetTickCount();

	if (dwStartTime <= dwTC)
		return dwTC-dwStartTime;
	else
		return (((DWORD)(-1)) - dwStartTime) + dwTC;
}
/////////////////////////////////////////////////////////////////////////////
inline DWORD GetTimeSpan(DWORD dwStartTime, DWORD dwActualTime)
{
	if (dwStartTime <= dwActualTime)
		return dwActualTime - dwStartTime;
	else
		return (((DWORD)(-1)) - dwStartTime) + dwActualTime;
}
/////////////////////////////////////////////////////////////////////////////

#include "WK_Template.h"

#pragma warning(disable : 4711)	// function 'foo' selected for inline expansion
#pragma warning(disable : 4100)	// unreferenced formal parameter
#pragma warning(3:4705;4:4705)	// statement has no effect in optimized code
#pragma warning(3:4701;4:4701)	// local variable *may* be used without init
#pragma warning(3:4189;4:4189)	// initialized but not referenced
#pragma warning(3:4390;4:4390)	// empty controlled statement
#pragma warning(3:4554;4:4554)	// possible operator precedence error
#pragma warning(3:4614;4:4614)	// vararg promotion
#pragma warning(3:4035;4:4035)	// 'function' : no return value
#pragma warning(3:4702;4:4702)	// unreachable code ??? not mentioned

#pragma warning(3:4244;4:4244)	// 'conversion' conversion from 'type1' to 'type2', possible loss of data
#pragma warning(3:4311;4:4311)	// type cast truncation ??? not mentioned
#pragma warning(3:4706;4:4706)	// assignment within conditional


#ifndef _DEBUG
// only for release, since ASSERT causes a lot of warnings
#pragma warning(3:4127;4:4127)	// conditional expression is constant
#pragma warning(disable: 4710)	// function not inlined
								// C4710: 'void __cdecl AfxTrace(const char *,...)' : function not expanded/not inlined
#endif


#ifndef _DEBUG
#define WK_ASSERT(f) { if (!(f)) {WK_TRACE_ERROR(_T("ASSERT Failed in %s, line %d\n"),_T(__FILE__), __LINE__); } }
#define WK_FAILED() { WK_TRACE_ERROR(_T("ASSERT Failed in %s, line %d\n"),_T(__FILE__), __LINE__);}
#else
#define WK_ASSERT(f) ASSERT(f)
#define WK_FAILED() ASSERT(0)
#endif

#define NAME_OF_CMD(x)  case x: return _T(#x);
#define NAME_OF_ENUM(x) case x: return _T(#x);
#define RETURN_ENUM_OF_NAME(x,y) if (y==_T(#x)) return x;

#endif
