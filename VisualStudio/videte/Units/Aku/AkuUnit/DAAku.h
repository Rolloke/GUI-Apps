/////////////////////////////////////////////////////////////////////////////
// PROJECT:		AkuUnit
// FILE:		$Workfile: DAAku.h $
// ARCHIVE:		$Archive: /Project/Units/Aku/AkuUnit/DAAku.h $
// CHECKIN:		$Date: 5.08.98 9:52 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 5.08.98 9:52 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __DAAAKU_H__
#define __DAAAKU_H__

#define LATCH_SIZE 1024

BOOL OpenDevice();
BOOL CloseDevice();
void DAWritePort8(DWORD dwAddr, BYTE byVal, BOOL bLatch);
void DAWritePort16(DWORD dwAddr, WORD wVal, BOOL bLatch);
BYTE DAReadPort8(DWORD dwAddr, BOOL bLatch);
WORD DAReadPort16(DWORD dwAddr, BOOL bLatch);

#endif // __DAAAKU_H__