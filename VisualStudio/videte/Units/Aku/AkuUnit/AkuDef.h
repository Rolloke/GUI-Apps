/////////////////////////////////////////////////////////////////////////////
// PROJECT:		AkuUnit
// FILE:		$Workfile: AkuDef.h $
// ARCHIVE:		$Archive: /Project/Units/Aku/AkuUnit/AkuDef.h $
// CHECKIN:		$Date: 5.08.98 9:52 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 5.08.98 9:52 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __MICODEFS_H__
#define __MICODEFS_H__

// Io-Größenangabe 8Bit, 16Bit 32Bit
#define IO_SIZE8	1
#define IO_SIZE16	2
#define IO_SIZE32	3

typedef struct
{
	BOOL  bLatch;
	WORD  wIOSize;
	DWORD dwAddr;
	DWORD dwVal;
	WORD  wVal;
	BYTE  byVal;
} IOSTRUCT;


typedef struct
{
	PVOID pPhysAddr;
	PVOID pLinAddr;
	DWORD dwLen;
} MEMSTRUCT;

#endif //  __MICODEFS_H__