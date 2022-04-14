/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaSys
// FILE:		$Workfile: Eventlog.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaSys/Eventlog.h $
// CHECKIN:		$Date: 25.03.04 11:26 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 25.03.04 10:40 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __EVENTLOG_H__
#define __EVENTLOG_H__

//
// Header files
//
#include <ntddk.h>

// Unique values identifying location
// of an error within the driver 
#define TASHA_ERRORLOG_INIT					1
#define TASHA_ERRORLOG_DISPATCH				2
#define TASHA_ERRORLOG_STARTIO				3
#define TASHA_ERRORLOG_CONTROLLER_CONTROL	4
#define TASHA_ERRORLOG_ADAPTER_CONTROL		5
#define TASHA_ERRORLOG_ISR					6
#define TASHA_ERRORLOG_DPC_FOR_ISR			7
#define TASHA_ERRORLOG_UNLOAD				8

// Largest number of insertion strings
// allowed in one message
#define TASHA_MAX_INSERTION_STRINGS		20

// Prototypes for globally defined functions...
ULONG TashaGetStringSize(IN PWSTR String);

BOOLEAN	TashaReportEvent(
	IN NTSTATUS	ErrorCode,
	IN ULONG	UniqueErrorValue,
	IN PVOID	IoObject,
	IN PIRP		Irp,
	IN ULONG	DumpData[],
	IN ULONG	DumpDataCount,
	IN PWSTR	Strings[],
	IN ULONG	StringCount
	);

#endif	// __EVENTLOG_H__

