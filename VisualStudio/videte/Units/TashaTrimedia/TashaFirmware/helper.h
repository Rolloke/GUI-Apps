/////////////////////////////////////////////////////////////////////////////
// PROJECT:		JaCobDll
// FILE:		$Workfile: helper.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/helper.h $
// CHECKIN:		$Date: 11.12.01 8:13 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 11.12.01 8:13 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __HELPER_H__
#define __HELPER_H__

typedef unsigned long	DWORD;
typedef unsigned short	WORD;
typedef unsigned char	BYTE;
typedef const char*		LPCSTR;
typedef int				BOOL;
typedef long			LONG;

typedef struct _LARGE_INTEGER
{
    ULONG LowPart;
    LONG HighPart;
} LARGE_INTEGER, *PLARGE_INTEGER;

// delete macros, that check for NULL AND reset to NULL
#define WK_DELETE(ptr)			{if(ptr){delete ptr; ptr=NULL;}}
#define WK_DELETE_ARRAY(ptr)	{if(ptr){delete [] ptr; ptr=NULL;}}
#define MAX(a,b)				(((a)>(b))?(a):(b))
#define MIN(a,b)				(((a)<(b))?(a):(b))
#define max(a,b)				MAX(a,b)
#define min(a,b)				MIN(a,b)
#define MAX_DEBUG_STRING_LEN	4096	 

BOOL InitDebuger(UInt32 hDSP);
BOOL ExitDebugger();

DWORD GetTickCount();
DWORD GetTimeSpan(DWORD dwStartTime);
void WK_TRACE(LPCSTR lpszFormat, ...);
void WK_TRACE_ERROR(LPCSTR lpszFormat, ...);
void WK_TRACE_WARNING(LPCSTR lpszFormat, ...);

///////////////////////////////////////////////////////////////////////////
class CPoint
{
public:
	CPoint(){m_cx = 0; m_cy = 0;};
	CPoint(int cx, int cy){m_cx = cx; m_cy = cy;};

	int m_cx;
	int m_cy;
};

#endif //__HELPER_H__
