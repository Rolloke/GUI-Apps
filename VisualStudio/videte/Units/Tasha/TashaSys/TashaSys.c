/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaSys
// FILE:		$Workfile: TashaSys.c $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaSys/TashaSys.c $
// CHECKIN:		$Date: 21.04.04 10:04 $
// VERSION:		$Revision: 12 $
// LAST CHANGE:	$Modtime: 20.04.04 15:49 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "Driver.h"

MEMSTRUCT	g_FrameBuffer[MAX_TASHA_BOARDS]		= {0};
MEMSTRUCT	g_TransferBuffer[MAX_TASHA_BOARDS]	= {0};

BOOL	g_bHasSFence	= FALSE;
BOOL	g_bHasMMX		= FALSE;

//////////////////////////////////////////////////////////////////////////////////
NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
    PHYSICAL_ADDRESS	PortAddress;
    PLOCAL_DEVICE_INFO	pLocalInfo			= NULL;  
    NTSTATUS			Status				= STATUS_SUCCESS;
    PDEVICE_OBJECT		DeviceObject		= NULL;
	CM_RESOURCE_LIST	ResList[8]			= {0};
	PCM_RESOURCE_LIST	pResourceList		= ResList;
    BOOLEAN				ResourceConflict	= TRUE;
	int					nI					= 0;

	TashaReportEvent(TASHA_MSG_LOGGING_ENABLED, TASHA_ERRORLOG_INIT, (PVOID)DriverObject, NULL, NULL, 0, NULL, 0);				// No strings
    Out_Debug_String("Tasha Driver: DriverEntry\n");
    Out_Debug_String("Tasha Version: 1.0\n");

	g_bHasSFence = SupportCheckForSFence();
	g_bHasMMX	 = HasMMX();
 
	// Initialize the driver object dispatch table.
    DriverObject->MajorFunction[IRP_MJ_CREATE]          = TashaDispatch;
    DriverObject->MajorFunction[IRP_MJ_CLOSE]           = TashaDispatch;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL]  = TashaDispatch;
    DriverObject->DriverUnload                          = TashaUnload;

    // Create our device.
    Status = TashaCreateDevice(TASHA_DEVICE_NAME, TASHA_TYPE, DriverObject, &DeviceObject);

    if (NT_SUCCESS(Status))
    {
        // Initialize the local driver info for each device object.
        pLocalInfo = (PLOCAL_DEVICE_INFO)DeviceObject->DeviceExtension;
        pLocalInfo->DeviceObject    = DeviceObject;
        pLocalInfo->DeviceType      = TASHA_TYPE;

		// Alle benötigten Buffer einmal statisch anlegen.
		for (nI = 0; nI < MAX_TASHA_BOARDS; nI++)
		{
			if (!PageAllocate(&g_FrameBuffer[nI], MAX_FRAME_BUFFERS*CHANNEL_COUNT*CHANNEL_SIZE, TRUE))	//DMA-Buffers
			{
				Out_Debug_String("Tasha Driver: ERROR PageAllocating of Framebuffer failed\n\r");
				return STATUS_NO_MEMORY;
			}
			if (!PageAllocate(&g_TransferBuffer[nI], UNIVERSAL_BUFFER_SIZE, TRUE))	// 512KByte Universal Buffer.
			{
				Out_Debug_String("Tasha Driver: ERROR PageAllocating of 1MB Transferbuffer failed\n\r");
				return STATUS_NO_MEMORY;
			}
		}
	}
    else
    {
		TashaReportEvent(TASHA_MSG_CANT_CREATE_DEVICE, TASHA_ERRORLOG_INIT, (PVOID)DriverObject, NULL, NULL, 0, NULL, 0);				// No strings
        KdPrint( ("Tasha Driver: CreateDevice failed %8X\n", Status) );

		// Error creating device - release resources
		RtlZeroMemory((PVOID)pResourceList, sizeof(ResList));
	}

	// Indicating drive startup
	TashaReportEvent(TASHA_MSG_DRIVER_STARTING, TASHA_ERRORLOG_INIT, (PVOID)DriverObject, NULL, NULL, 0, NULL, 0);				// No strings

	return Status;
}

/////////////////////////////////////////////////////////////////////////////////////
NTSTATUS TashaCreateDevice(	IN  PWSTR			PrototypeName,
							IN  DEVICE_TYPE		DeviceType,
							IN  PDRIVER_OBJECT	DriverObject,
							OUT PDEVICE_OBJECT	*ppDevObj)
{
    NTSTATUS		Status		= STATUS_SUCCESS;
    UNICODE_STRING	NtDeviceName;
    UNICODE_STRING	Win32DeviceName;

    // Get UNICODE name for device.
    RtlInitUnicodeString(&NtDeviceName, PrototypeName);

    Status = IoCreateDevice(                             // Create it.
                    DriverObject,
                    sizeof(LOCAL_DEVICE_INFO),
                    &NtDeviceName,
                    DeviceType,
                    0,
                    FALSE,								// Not Exclusive
                    ppDevObj
                    );

    if (!NT_SUCCESS(Status))
        return Status;             // Give up if create failed.

    // Clear local device info memory
    RtlZeroMemory((*ppDevObj)->DeviceExtension, sizeof(LOCAL_DEVICE_INFO));

    //  (*ppDevObj)->Flags |= DO_BUFFERED_IO;
    //  (*ppDevObj)->AlignmentRequirement = FILE_BYTE_ALIGNMENT;
    RtlInitUnicodeString(&Win32DeviceName, DOS_DEVICE_NAME);

    Status = IoCreateSymbolicLink( &Win32DeviceName, &NtDeviceName );

    if (!NT_SUCCESS(Status))    // If we we couldn't create the link then
    {                           //  abort installation.
        IoDeleteDevice(*ppDevObj);
    }

    return Status;
}

////////////////////////////////////////////////////////////////////////////////////
NTSTATUS TashaDispatch(IN PDEVICE_OBJECT pDO, IN PIRP pIrp)
{
    PIO_STACK_LOCATION	pIrpStack;
    NTSTATUS			Status = STATUS_NOT_IMPLEMENTED;
	IOCtlStruct			IOCtl;

	if (!pIrp || !pDO)
		return Status;

    //  Initialize the irp info field.
    //  This is used to return the number of bytes transfered.
    pIrp->IoStatus.Information = 0;
    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);

	if (!pIrpStack)
		return Status;

    // Dispatch based on major fcn code.
    switch (pIrpStack->MajorFunction)
    {
        case IRP_MJ_CREATE:
			Status = STATUS_SUCCESS;
			break;
        case IRP_MJ_CLOSE:
			Status = STATUS_SUCCESS;
			break;
		case IRP_MJ_DEVICE_CONTROL:
			IOCtl.pInBuffer			= pIrp->AssociatedIrp.SystemBuffer;
			IOCtl.pOutBuffer		= pIrp->AssociatedIrp.SystemBuffer;
			IOCtl.dwInBufferSize	= pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
			IOCtl.dwOutBufferSize	= pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
			IOCtl.pBytesToReturn	= &pIrp->IoStatus.Information;
			Status = IOCtlDispatch(pIrpStack->Parameters.DeviceIoControl.IoControlCode, IOCtl);
			break;
	}

    // We're done with I/O request.  Record the status of the I/O action.
    pIrp->IoStatus.Status = Status;

    // Don't boost priority when returning since this took little time.
    IoCompleteRequest(pIrp, IO_NO_INCREMENT );

    return Status;
}

////////////////////////////////////////////////////////////////////////////////////*/
VOID TashaUnload(PDRIVER_OBJECT DriverObject)
{
    PLOCAL_DEVICE_INFO pLDI;
    CM_RESOURCE_LIST NullResourceList;
    BOOLEAN ResourceConflict;
    UNICODE_STRING Win32DeviceName;
	int nI = 0;

    Out_Debug_String("Tasha Driver: TashaUnload\n");

    // Find our global data
    pLDI = (PLOCAL_DEVICE_INFO)DriverObject->DeviceObject->DeviceExtension;

	// Pointer Ok?
	if (!pLDI)
		return;

	CleanUp();

	// Assume all handles are closed down.
    // Delete the things we allocated - devices, symbolic links
    RtlInitUnicodeString(&Win32DeviceName, DOS_DEVICE_NAME);

    IoDeleteSymbolicLink(&Win32DeviceName);

    IoDeleteDevice(pLDI->DeviceObject);
	TashaReportEvent(TASHA_MSG_DRIVER_STOPPING, TASHA_ERRORLOG_UNLOAD, (PVOID)DriverObject, NULL, NULL, 0, NULL, 0);				// No strings
}

////////////////////////////////////////////////////////////////////////////////////
BOOL PageAllocate(MEMSTRUCT *pMem, DWORD dwLen, BOOL bContiguousMemory)
{
	PHYSICAL_ADDRESS phys;
	PHYSICAL_ADDRESS HighestAcceptableAddress;
	
	PMDL	pMdl;
	// Parameter Ok?
	if ((!pMem) || (dwLen == 0))
		return FALSE;

//	HighestAcceptableAddress.QuadPart = 0x3FFFFFFF; // 1GByte
	HighestAcceptableAddress.QuadPart = 0x07FFFFFF; // 128MByte
	if (bContiguousMemory)
		pMem->pLinAddr	= MmAllocateContiguousMemory(dwLen, HighestAcceptableAddress); // in den unteren 32MB
	else
		pMem->pLinAddr	= MmAllocateNonCachedMemory(dwLen);
	
	pMem->pPhysAddr	= NULL;
	pMem->dwLen		= dwLen;

	if (!pMem->pLinAddr)
		return FALSE;

	phys = MmGetPhysicalAddress(pMem->pLinAddr);
	pMem->pPhysAddr = (void*)phys.LowPart;
	pMem->dwLen		= dwLen;

	// Auf 0 initialisieren.
	RtlZeroMemory(pMem->pLinAddr, pMem->dwLen);

    Out_Debug_String("PageAllocated\n");

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////
BOOL PageFree(MEMSTRUCT *pMem, BOOL bContiguousMemory)
{
	// Parameter Ok?
	if (!pMem || !pMem->pLinAddr || pMem->dwLen == 0)
		return FALSE;

	// Speicher freigeben
	if (bContiguousMemory)
		MmFreeContiguousMemory(pMem->pLinAddr); 
	else
		MmFreeNonCachedMemory(pMem->pLinAddr, pMem->dwLen);

	pMem->pLinAddr  = NULL;
	pMem->pPhysAddr	= NULL;
	pMem->dwLen		= 0;
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////
void Sleep(DWORD dwSleep)
{
	KeStallExecutionProcessor(dwSleep*1000);
}

/////////////////////////////////////////////////////////////////////////////////////
void Out_Debug_String(char* lpString)
{
	DbgPrint((lpString));
	//KdPrint((lpString));
}

/////////////////////////////////////////////////////////////////////////////////////
BOOL CleanUp()
{
	int nI = 0;

	Out_Debug_String("Tasha Driver: CleanUp\n\r");
	
	for (nI = 0; nI < MAX_TASHA_BOARDS; nI++)
	{
		PageFree(&g_FrameBuffer[nI], TRUE);		// Die 8MByte großen Framebuffer freigeben
		PageFree(&g_TransferBuffer[nI], TRUE);	// Die 1MByte großen universalbuffer freigeben
	}

	return TRUE;
}
