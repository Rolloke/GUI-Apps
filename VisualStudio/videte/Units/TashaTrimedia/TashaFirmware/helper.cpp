/////////////////////////////////////////////////////////////////////////////
// PROJECT:		JaCobDll
// FILE:		$Workfile: helper.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/helper.cpp $
// CHECKIN:		$Date: 12.12.01 14:46 $
// VERSION:		$Revision: 6 $
// LAST CHANGE:	$Modtime: 12.12.01 14:42 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "tm.h"
#include "helper.h"
#include <WINNT\tmmanapi.h>

///////////////////////////////////////////////////////////////////////////////////
DWORD GetTickCount()
{
	return (CInt64(CYCLES(), HICYCLES()) / (CInt64)CLOCK_SPEED);
}

///////////////////////////////////////////////////////////////////////////////////
DWORD GetTimeSpan(DWORD dwStartTime)
{
	DWORD dwTC = GetTickCount();

	if (dwStartTime <= dwTC)
		return dwTC-dwStartTime;
	else
		return (((DWORD)(-1)) - dwStartTime) + dwTC;
}
						
///////////////////////////////////////////////////////////////////////////
void WK_TRACE(LPCSTR lpszFormat, ...)
{
	static char szString[MAX_DEBUG_STRING_LEN] = {0};

	va_list args;
	va_start(args, lpszFormat);
	vsprintf(szString, lpszFormat, args);
    va_end(va);
#ifdef _DEBUG
	printf(szString);
#else
	DP((szString));
#endif
}

///////////////////////////////////////////////////////////////////////////
void WK_TRACE_ERROR(LPCSTR lpszFormat, ...)
{
	static char szString[MAX_DEBUG_STRING_LEN] = {0};

	va_list args;
	va_start(args, lpszFormat);
	vsprintf(szString, lpszFormat, args);
    va_end(va);
#ifdef _DEBUG
	printf("ERROR: %s", szString);
#else
	DP((szString));
#endif
}

///////////////////////////////////////////////////////////////////////////
void WK_TRACE_WARNING(LPCSTR lpszFormat, ...)
{
	static char szString[MAX_DEBUG_STRING_LEN] = {0};

	va_list args;
	va_start(args, lpszFormat);
	vsprintf(szString, lpszFormat, args);
    va_end(va);
	
#ifdef _DEBUG
	printf("WARNING: %s", szString);
#else
	DP((szString));
#endif
}

