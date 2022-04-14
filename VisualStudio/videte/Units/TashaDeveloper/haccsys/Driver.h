/////////////////////////////////////////////////////////////////////////////
// PROJECT:		HAcc
// FILE:		$Workfile: Driver.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha(Developer)/haccsys/Driver.h $
// CHECKIN:		$Date: 5.01.04 9:52 $
// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 5.01.04 9:25 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __DRIVER_H__
#define __DRIVER_H__

#pragma pack(1)

#pragma optimize("gt", on)

///////////////////////////////////////////////////////////////////////////////////////// 
#ifdef _WIN95_
#pragma warning( disable : 4761 )
#pragma warning( disable : 4229 )
#define WANTVXDWRAPS
#include <basedef.h>    
#include <vmm.h>
#include <vwin32.h>
#include <debug.h>
#include <vxdwraps.h>
#include <winerror.h>
#include <vpicd.h>
#include <vtd.h>
#include <shell.h>
#include "..\HAccVxD\HAccVxD.h" 
#pragma VxD_LOCKED_CODE_SEG
#pragma VxD_LOCKED_DATA_SEG
///////////////////////////////////////////////////////////////////////////////////////// 
#else	// WinNT
#include <ntddk.h>
#include <string.h>
#include <devioctl.h>
#include "HAccSys.h"
#include "HAccMsg.h"
#include "Eventlog.h"
#endif
///////////////////////////////////////////////////////////////////////////////////////// 


#include "..\HAccSys\HAccIoctl.h"
#include "..\HAccSys\HAccCommon.h"
#endif // __DRIVER_H__
