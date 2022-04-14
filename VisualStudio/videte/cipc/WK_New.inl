#ifndef __WK_NEW_HEADER_INCLUDED__
#define __WK_NEW_HEADER_INCLUDED__

#ifndef _DEBUG

#include "UnhandledException.h"
#include "WK_Trace.h"

#ifndef _MAX_WK_NEW_SIZE
 #define _MAX_WK_NEW_SIZE (10*1024*1024)
#endif

void* AFX_CDECL operator new(size_t nSize)
{
	if (nSize > _MAX_WK_NEW_SIZE)
	{
		WK_TRACE(_T("new size to large %d \n"), nSize);
		CUnhandledException::TraceCallStack(NULL);
		return NULL;
	}
	else
	{
		return malloc(nSize);
	}
}

void* AFX_CDECL operator new[](size_t nSize)
{
	if (nSize > _MAX_WK_NEW_SIZE)
	{
		WK_TRACE(_T("new size to large %d \n"), nSize);
		CUnhandledException::TraceCallStack(NULL);
		return NULL;
	}
	else
	{
		return malloc(nSize);
	}
}
#endif // _DEBUG
#endif // __WK_NEW_HEADER_INCLUDED__
