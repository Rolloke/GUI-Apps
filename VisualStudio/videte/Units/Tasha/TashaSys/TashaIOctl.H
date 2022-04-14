/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaSys
// FILE:		$Workfile: TashaIOctl.H $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaSys/TashaIOctl.H $
// CHECKIN:		$Date: 13.04.04 10:06 $
// VERSION:		$Revision: 6 $
// LAST CHANGE:	$Modtime: 13.04.04 9:50 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __TASHAIOCTL_H__
#define __TASHAIOCTL_H__

typedef USHORT			WORD;
typedef ULONG			DWORD;
typedef unsigned char	BYTE;
typedef int				BOOL;
typedef ULONG			HIRQ;

// Device type           -- in the "User Defined" range."
#define TASHA_TYPE 40000

// The IOCTL function codes from 0x800 to 0xFFF are for customer use.
#define TASHA_GET_DRIVERVERSION				CTL_CODE(TASHA_TYPE, 0x902, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define TASHA_WRITE_PORT					CTL_CODE(TASHA_TYPE, 0x903, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define TASHA_READ_PORT						CTL_CODE(TASHA_TYPE, 0x904, METHOD_BUFFERED, FILE_READ_ACCESS)
#define TASHA_WRITE_MEMORY					CTL_CODE(TASHA_TYPE, 0x905, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define TASHA_READ_MEMORY					CTL_CODE(TASHA_TYPE, 0x906, METHOD_BUFFERED, FILE_READ_ACCESS)
#define TASHA_GET_FRAME_BUFFER				CTL_CODE(TASHA_TYPE, 0x907, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define TASHA_GET_TRANSFER_BUFFER			CTL_CODE(TASHA_TYPE, 0x908, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
 
#endif // __TASHAIOCTL_H__

