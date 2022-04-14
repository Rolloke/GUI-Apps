/////////////////////////////////////////////////////////////////////////////
// PROJECT:		HAcc.sys
// FILE:		$Workfile: Eventlog.c $
// ARCHIVE:		$Archive: /Project/Units/Tasha(Developer)/haccsys/Eventlog.c $
// CHECKIN:		$Date: 5.01.04 9:52 $
// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 5.01.04 9:25 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "Driver.h"

/////////////////////////////////////////////////////////////////////////////////////
BOOLEAN HAccReportEvent(IN NTSTATUS	ErrorCode,
						IN ULONG	UniqueErrorValue,
						IN PVOID	IoObject,
						IN PIRP		Irp,
						IN ULONG	DumpData[],
						IN ULONG	DumpDataCount,
						IN PWSTR	Strings[],
						IN ULONG	StringCount
						)
{
	PIO_ERROR_LOG_PACKET Packet;
	PLOCAL_DEVICE_INFO pDE;
	PIO_STACK_LOCATION IrpStack;
	PUCHAR pInsertionString;
	UCHAR PacketSize;
	UCHAR StringSize[HACC_MAX_INSERTION_STRINGS];
	ULONG i;
	ULONG Len = ERROR_LOG_MAXIMUM_SIZE;


	// Start with minimum required packet size
	PacketSize = sizeof( IO_ERROR_LOG_PACKET );

	// Add in any dump data. Remember that the
	// standard error log packet already has one
	// slot in its DumpData array.
	if ( DumpDataCount > 0 )
		PacketSize += (UCHAR)(sizeof(ULONG) *(DumpDataCount - 1)); 

	// Determine total space needed for any 
	// insertion strings.
	if (StringCount > 0)
	{
		// Take the lesser of what the caller sent
		// and what this routine can handle
		if (StringCount > HACC_MAX_INSERTION_STRINGS )
			StringCount = HACC_MAX_INSERTION_STRINGS;

		for (i = 0; i < StringCount; i++)
		{
			// Keep track of individual string sizes
			StringSize[i] =	(UCHAR)HAccGetStringSize(Strings[i]);
			PacketSize += StringSize[i];
		}
	}
	  
	// Try to allocate the packet
	Packet = (PIO_ERROR_LOG_PACKET)IoAllocateErrorLogEntry(IoObject, PacketSize);

	if (Packet == NULL)
		return FALSE;

	// Fill in standard parts of the packet
	Packet->ErrorCode = ErrorCode;
	Packet->UniqueErrorValue = UniqueErrorValue;

	// If there's an IRP, then the IoObject must
	// be a Device object. In that case, use the
	// IRP and the Device Extension to fill in
	// additional parts of the packet. Otherwise,
	// set these additional fields to zero.
	if (Irp != NULL)
	{
		IrpStack = IoGetCurrentIrpStackLocation(Irp);
		
		pDE = (PLOCAL_DEVICE_INFO) ((PDEVICE_OBJECT)IoObject)->DeviceExtension;

		Packet->MajorFunctionCode	= IrpStack->MajorFunction;
		Packet->RetryCount			= pDE->IrpRetryCount;
		Packet->FinalStatus			= Irp->IoStatus.Status;
		Packet->SequenceNumber		= pDE->IrpSequenceNumber;

		if ( IrpStack->MajorFunction == IRP_MJ_DEVICE_CONTROL ||
			IrpStack->MajorFunction == IRP_MJ_INTERNAL_DEVICE_CONTROL )
		{
			Packet->IoControlCode =	IrpStack->Parameters.DeviceIoControl.IoControlCode;
		}
		else
			Packet->IoControlCode = 0;
	}
	else	// No IRP
	{
		Packet->MajorFunctionCode	= 0;
		Packet->RetryCount			= 0;
		Packet->FinalStatus			= 0;
		Packet->SequenceNumber		= 0;
		Packet->IoControlCode		= 0;
	}

	// Add the dump data
	if(DumpDataCount > 0)
	{
		Packet->DumpDataSize = (USHORT)(sizeof(ULONG) * DumpDataCount);

		for (i = 0; i < DumpDataCount; i++)
			Packet->DumpData[i] = DumpData[i];
	}
	else
		Packet->DumpDataSize = 0;

	// Add the insertion strings
	Packet->NumberOfStrings = (USHORT)StringCount;

	if (StringCount > 0)
	{
		// The strings always go just after
		// the DumpData array in the packet
		Packet->StringOffset = sizeof(IO_ERROR_LOG_PACKET) + (DumpDataCount - 1) * sizeof(ULONG);

		pInsertionString = (PUCHAR)Packet + Packet->StringOffset;

		for(i = 0; i < StringCount; i++)
		{
			// Add each new string to the end
			// of the existing stuff
			RtlCopyBytes(pInsertionString, Strings[i], StringSize[i]);
			pInsertionString += StringSize[i];
		}
	}

	// Log the message
	IoWriteErrorLogEntry(Packet);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////
ULONG HAccGetStringSize(IN PWSTR String)
{
	UNICODE_STRING TempString;

	// Use an RTL routine to get the length
	RtlInitUnicodeString( &TempString, String );

	// Size is accutally two greater because
	// of the UNICODE_NULL at the end.
	return (TempString.Length + sizeof(WCHAR));
}
