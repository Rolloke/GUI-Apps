/////////////////////////////////////////////////////////////////////////////
// PROJECT:		AkuUnit
// FILE:		$Workfile: AkuIOctl.H $
// ARCHIVE:		$Archive: /Project/Units/Aku/AkuSys/AkuIOctl.H $
// CHECKIN:		$Date: 5.08.98 9:53 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 5.08.98 9:53 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __AKUIOCTL_H__
#define __AKUIOCTL_H__

typedef USHORT			WORD;
typedef ULONG			DWORD;
typedef unsigned char	BYTE;
typedef int				BOOL;
typedef ULONG			HIRQ;

// Device type           -- in the "User Defined" range."
#define AKU_TYPE 40000

// The IOCTL function codes from 0x800 to 0xFFF are for customer use.
#define AKU_SYS_GETVERSION	CTL_CODE(AKU_TYPE, 0x900, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define AKU_SYS_WRITE_PORT	CTL_CODE(AKU_TYPE, 0x901, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define AKU_SYS_READ_PORT	CTL_CODE(AKU_TYPE, 0x902, METHOD_BUFFERED, FILE_READ_ACCESS)


#endif // __AKUIOCTL_H__

