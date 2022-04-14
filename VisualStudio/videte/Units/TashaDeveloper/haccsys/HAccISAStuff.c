/////////////////////////////////////////////////////////////////////////////
// PROJECT:		HACC-Driver
// FILE:		$Workfile: HAccISAStuff.c $
// ARCHIVE:		$Archive: /Project/Units/Tasha(Developer)/haccsys/HAccISAStuff.c $
// CHECKIN:		$Date: 5.01.04 9:52 $
// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 5.01.04 9:25 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////
			  
#include "Driver.h"

/////////////////////////////////////////////////////////////////////////////////////
// Pointer auf Latch
extern MEMSTRUCT g_Latch8;
extern MEMSTRUCT g_Latch16;
extern MEMSTRUCT g_Latch32;

/////////////////////////////////////////////////////////////////////////////////////
void ISAOutput8(DWORD dwAddr, BYTE byVal)
{
	WRITE_PORT_UCHAR((PUCHAR)dwAddr, byVal);
}

/////////////////////////////////////////////////////////////////////////////////////
void ISAOutput16(DWORD dwAddr, WORD wVal)
{
	WRITE_PORT_USHORT((PUSHORT)dwAddr, wVal);
}

/////////////////////////////////////////////////////////////////////////////////////
void ISAOutput32(DWORD dwAddr, DWORD dwVal)
{
	WRITE_PORT_ULONG((PULONG)dwAddr, dwVal);
}

/////////////////////////////////////////////////////////////////////////////////////
BYTE ISAInput8(DWORD dwAddr)
{
    return READ_PORT_UCHAR((PUCHAR)dwAddr);
}

/////////////////////////////////////////////////////////////////////////////////////
WORD ISAInput16(DWORD dwAddr)
{
    return READ_PORT_USHORT((PUSHORT)dwAddr);
}

/////////////////////////////////////////////////////////////////////////////////////
DWORD ISAInput32(DWORD dwAddr)
{
    return READ_PORT_ULONG((PULONG)dwAddr);
}

/////////////////////////////////////////////////////////////////////////////////////
void ISAOutputLatch8(DWORD dwAddr, BYTE byVal)
{
	if ((dwAddr < LATCH_SIZE) && g_Latch8.pLinAddr)
	{
		((char*)g_Latch8.pLinAddr)[dwAddr] = byVal;
		ISAOutput8(dwAddr, byVal);
	}
}

/////////////////////////////////////////////////////////////////////////////////////
void ISAOutputLatch16(DWORD dwAddr, WORD wVal)
{
	if ((dwAddr < LATCH_SIZE) && g_Latch16.pLinAddr)
	{
		((WORD*)g_Latch16.pLinAddr)[dwAddr] = wVal;
		ISAOutput16(dwAddr, wVal);
	}
}

/////////////////////////////////////////////////////////////////////////////////////
void ISAOutputLatch32(DWORD dwAddr, DWORD dwVal)
{
	if ((dwAddr < LATCH_SIZE) && g_Latch32.pLinAddr)
	{
		((DWORD*)g_Latch32.pLinAddr)[dwAddr] = dwVal;
		ISAOutput32(dwAddr, dwVal);
	}
}

/////////////////////////////////////////////////////////////////////////////////////
BYTE ISAInputLatch8(DWORD dwAddr)
{
	if ((dwAddr < LATCH_SIZE) && g_Latch8.pLinAddr)
	   return ((char*)g_Latch8.pLinAddr)[dwAddr];
	else
		return 0;
}

/////////////////////////////////////////////////////////////////////////////////////
WORD ISAInputLatch16(DWORD dwAddr)
{
	if ((dwAddr < LATCH_SIZE) && g_Latch16.pLinAddr)
	   return ((WORD*)g_Latch16.pLinAddr)[dwAddr];
	else
		return 0;
}

/////////////////////////////////////////////////////////////////////////////////////
DWORD ISAInputLatch32(DWORD dwAddr)
{
	if ((dwAddr < LATCH_SIZE) && g_Latch32.pLinAddr)
	   return ((DWORD*)g_Latch32.pLinAddr)[dwAddr];
	else
		return 0;
}

/////////////////////////////////////////////////////////////////////////////////////
BOOL ISAServiceHAccBdInt(CONFIG* pConfig)
{

	return TRUE;
}