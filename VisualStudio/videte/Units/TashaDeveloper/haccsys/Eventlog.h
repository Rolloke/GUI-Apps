/////////////////////////////////////////////////////////////////////////////
// PROJECT:		HAcc
// FILE:		$Workfile: Eventlog.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha(Developer)/haccsys/Eventlog.h $
// CHECKIN:		$Date: 5.01.04 9:52 $
// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 5.01.04 9:25 $
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
#define HACC_ERRORLOG_INIT					1
#define HACC_ERRORLOG_DISPATCH				2
#define HACC_ERRORLOG_STARTIO				3
#define HACC_ERRORLOG_CONTROLLER_CONTROL	4
#define HACC_ERRORLOG_ADAPTER_CONTROL		5
#define HACC_ERRORLOG_ISR					6
#define HACC_ERRORLOG_DPC_FOR_ISR			7
#define HACC_ERRORLOG_UNLOAD				8

// Largest number of insertion strings
// allowed in one message
#define HACC_MAX_INSERTION_STRINGS		20

// Prototypes for globally defined functions...
ULONG HAccGetStringSize(IN PWSTR String);

BOOLEAN	HAccReportEvent(
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

