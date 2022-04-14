/////////////////////////////////////////////////////////////////////////////
// PROJECT:		AkuUnit
// FILE:		$Workfile: Driver.h $
// ARCHIVE:		$Archive: /Project/Units/Aku/AkuSys/Driver.h $
// CHECKIN:		$Date: 5.08.98 9:53 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 5.08.98 9:53 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __DRIVER_H__
#define __DRIVER_H__

#pragma pack(1)
#pragma optimize("gt", on)

///////////////////////////////////////////////////////////////////////////////////////// 
#include <ntddk.h>
#include <string.h>
#include <devioctl.h>
#include "AkuIoctl.h"        // Get IOCTL interface definitions
#include "..\AkuUnit\AkuDef.h"
#include "AkuSys.h"
#include "..\AkuUnit\AkuReg.h"

#endif // __DRIVER_H__
