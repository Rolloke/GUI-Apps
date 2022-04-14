/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaSys
// FILE:		$Workfile: Driver.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaSys/Driver.h $
// CHECKIN:		$Date: 17.05.04 16:41 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 17.05.04 11:04 $
// BY AUTHOR:	$Author: Martin.lueck $
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
#include "TashaSys.h"
#include "TashaMsg.h"
#include "Eventlog.h"
///////////////////////////////////////////////////////////////////////////////////////// 


#include "..\TashaSys\TashaIoctl.h"
#include "..\TashaSys\TashaCommon.h"
#include "..\TashaUnit\TashaUnitDefs.h"

#endif // __DRIVER_H__
