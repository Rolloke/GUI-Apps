/////////////////////////////////////////////////////////////////////////////
// PROJECT:		AkuUnit
// FILE:		$Workfile: AkuSys.c $
// ARCHIVE:		$Archive: /Project/Units/Aku/AkuSys/AkuSys.c $
// CHECKIN:		$Date: 5.08.98 9:53 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 5.08.98 9:53 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "Driver.h"

// Pointer auf Latch
MEMSTRUCT	g_Latch8;
MEMSTRUCT	g_Latch16;
MEMSTRUCT	g_Latch32;

// Schlüssel zur Konfiguration
WCHAR g_sDevice[] = L"\\Registry\\Machine\\Software\\DVRT\\AkuUnit\\System";

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
	DWORD				dwResource			= 0;
	WORD				wI					= 0;

    Out_Debug_String("AKU_SYS: DriverEntry\n");

	// Alle notwendigen Port-Resourcen reservieren
	ResourceConflict = TRUE;
	RtlZeroMemory(ResList, sizeof(ResList));
	pResourceList->Count					= 1;
	pResourceList->List[0].InterfaceType	= Isa;
	pResourceList->List[0].BusNumber		= 0;

	// PortIoBase der 1. Aku-Karte
	dwResource = ReadProfile(g_sDevice, L"AkuIOBase0", 0x00);
	if (dwResource != 0)
		ClaimPortResource(wI++, pResourceList, dwResource, 0x20);	

	// PortIoBase der 2. Aku-Karte
	dwResource = ReadProfile(g_sDevice, L"AkuIOBase1", 0x00);
	if (dwResource != 0)
		ClaimPortResource(wI++, pResourceList, dwResource, 0x20);	

	// PortIoBase der 3. Aku-Karte
	dwResource = ReadProfile(g_sDevice, L"AkuIOBase2", 0x00);
	if (dwResource != 0)
		ClaimPortResource(wI++, pResourceList, dwResource, 0x20);	

	// PortIoBase der 4. Aku-Karte
	dwResource = ReadProfile(g_sDevice, L"AkuIOBase3", 0x00);
	if (dwResource != 0)
		ClaimPortResource(wI++, pResourceList, dwResource, 0x20);	

	// Report our resource usage and detect conflicts
	Status = IoReportResourceUsage(
						  NULL,
						  DriverObject,
						  pResourceList,
						  sizeof(ResList),
						  NULL,
						  NULL,
						  0,
						  FALSE,
						  &ResourceConflict);

    if (ResourceConflict)
        Status = STATUS_DEVICE_CONFIGURATION_ERROR;

    if (!NT_SUCCESS(Status))
    {
        KdPrint( ("AKU_SYS: Resource reporting problem %8X\n", Status) );
        return Status;
    }

    // Initialize the driver object dispatch table.
    DriverObject->MajorFunction[IRP_MJ_CREATE]          = AkuDispatch;
    DriverObject->MajorFunction[IRP_MJ_CLOSE]           = AkuDispatch;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL]  = AkuDispatch;
    DriverObject->DriverUnload                          = AkuUnload;

    // Create our device.
    Status = AkuCreateDevice(AKU_DEVICE_NAME, AKU_TYPE, DriverObject, &DeviceObject);

    if (NT_SUCCESS(Status))
    {
        // Initialize the local driver info for each device object.
        pLocalInfo = (PLOCAL_DEVICE_INFO)DeviceObject->DeviceExtension;
        pLocalInfo->DeviceObject    = DeviceObject;
        pLocalInfo->DeviceType      = AKU_TYPE;
	}
    else
    {
        KdPrint( ("AKU_SYS: CreateDevice failed %8X\n", Status) );

		// Error creating device - release resources
		RtlZeroMemory((PVOID)pResourceList, sizeof(ResList));

		 // Unreport our resource usage
		Status = IoReportResourceUsage(
					  NULL,
					  DriverObject,
					  pResourceList,
					  sizeof(ResList),
					  NULL,
					  NULL,
					  0,
					  FALSE,
					  &ResourceConflict);  
	}
	
	// LatchBuffer anlegen
	PageAllocate(&g_Latch8,  LATCH_SIZE * sizeof(BYTE));
	PageAllocate(&g_Latch16, LATCH_SIZE * sizeof(WORD));
	PageAllocate(&g_Latch32, LATCH_SIZE * sizeof(DWORD));

	return Status;
}

/////////////////////////////////////////////////////////////////////////////////////
NTSTATUS AkuCreateDevice(	IN  PWSTR			PrototypeName,
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

    //
    // Set up the rest of the device info
    //  These are used for IRP_MJ_READ and IRP_MJ_WRITE which we don't use
    //    
    //  (*ppDevObj)->Flags |= DO_BUFFERED_IO;
    //  (*ppDevObj)->AlignmentRequirement = FILE_BYTE_ALIGNMENT;
    //

    RtlInitUnicodeString(&Win32DeviceName, DOS_DEVICE_NAME);

    Status = IoCreateSymbolicLink( &Win32DeviceName, &NtDeviceName );

    if (!NT_SUCCESS(Status))    // If we we couldn't create the link then
    {                           //  abort installation.
        IoDeleteDevice(*ppDevObj);
    }

    return Status;
}

////////////////////////////////////////////////////////////////////////////////////
NTSTATUS AkuDispatch(IN PDEVICE_OBJECT pDO, IN PIRP pIrp)
{
    PLOCAL_DEVICE_INFO pLDI;
    PIO_STACK_LOCATION pIrpStack;
    NTSTATUS Status;

    //  Set default return status
    Status = STATUS_NOT_IMPLEMENTED;

	if (!pIrp || !pDO)
		return Status;

    //  Initialize the irp info field.
    //  This is used to return the number of bytes transfered.
    pIrp->IoStatus.Information = 0;
    pLDI = (PLOCAL_DEVICE_INFO)pDO->DeviceExtension;    // Get local info struct

	if (!pLDI)
		return Status;

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
		
			switch (pIrpStack->Parameters.DeviceIoControl.IoControlCode)
			{
				case AKU_SYS_GETVERSION:
					Status = AkuIOCtlGetVersion(pIrp, pIrpStack);
					break;
				case AKU_SYS_WRITE_PORT:
					Status = AkuIOCtlWritePort(pIrp, pIrpStack);
					break;
				case AKU_SYS_READ_PORT:
					Status = AkuIOCtlReadPort(pIrp, pIrpStack);
					break;
			}
			break;
    }

    // We're done with I/O request.  Record the status of the I/O action.
    pIrp->IoStatus.Status = Status;

    // Don't boost priority when returning since this took little time.
    IoCompleteRequest(pIrp, IO_NO_INCREMENT );

    return Status;
}

///////////////////////////////////////////////////////////////////////////////////
NTSTATUS AkuIOCtlGetVersion(IN PIRP pIrp, PIO_STACK_LOCATION pIrpStack)
{
	DWORD	*pIO	= NULL;	

    Out_Debug_String("AKU_SYS: AkuIOCtlGetVersion\n");

	// Pointer Ok?
	if (!pIrp || !pIrpStack)
		return STATUS_INVALID_PARAMETER;

    pIrp->IoStatus.Information = 0;

	// Size of buffer containing data from application 
 	if (pIrpStack->Parameters.DeviceIoControl.InputBufferLength != 0)
	{
		Out_Debug_String(("AKU_SYS: ERROR AkuIOCtlGetVersion InBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

    // Size of buffer for data to be sent to application 
	if (pIrpStack->Parameters.DeviceIoControl.OutputBufferLength != sizeof(DWORD))
	{
		Out_Debug_String(("AKU_SYS: ERROR AkuIOCtlGetVersion OutBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}
   
	*(DWORD*)pIrp->AssociatedIrp.SystemBuffer = AKU_SYS_VERSION;
    pIrp->IoStatus.Information = sizeof(DWORD);
	
    return STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
NTSTATUS AkuIOCtlWritePort(IN PIRP pIrp, PIO_STACK_LOCATION pIrpStack)
{
	IOSTRUCT	*pIoIn	= NULL;	

	// Pointer Ok?
	if (!pIrp || !pIrpStack)
		return STATUS_INVALID_PARAMETER;

    pIrp->IoStatus.Information = 0;

	// Size of buffer containing data from application 
 	if (pIrpStack->Parameters.DeviceIoControl.InputBufferLength != sizeof(IOSTRUCT))
	{
		Out_Debug_String(("AKU_SYS: ERROR AkuIOCtlWritePort InBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

    // Size of buffer for data to be sent to application 
	if (pIrpStack->Parameters.DeviceIoControl.OutputBufferLength != 0)
	{
		Out_Debug_String(("AKU_SYS: ERROR AkuIOCtlWritePort OutBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}
  
	pIoIn = (IOSTRUCT*)pIrp->AssociatedIrp.SystemBuffer;
	if (!pIoIn)
	{
		Out_Debug_String("AKU_SYS: ERROR AkuIOCtlWritePort pIoIn = NULL!\n");
		return STATUS_INVALID_PARAMETER;
	}

	switch (pIoIn->wIOSize)
	{
		case IO_SIZE8:
			if (pIoIn->bLatch)
				ISAOutputLatch8(pIoIn->dwAddr, pIoIn->byVal);
			else
				ISAOutput8(pIoIn->dwAddr, pIoIn->byVal);
			break;
		case IO_SIZE16:
			if (pIoIn->bLatch)
				ISAOutputLatch16(pIoIn->dwAddr, pIoIn->wVal);
			else
				ISAOutput16(pIoIn->dwAddr, pIoIn->wVal);
			break;
		case IO_SIZE32:
			if (pIoIn->bLatch)
				ISAOutputLatch32(pIoIn->dwAddr, pIoIn->dwVal);
			else
				ISAOutput32(pIoIn->dwAddr, pIoIn->dwVal);
			break;
	}

    pIrp->IoStatus.Information = 0;

    return STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
NTSTATUS AkuIOCtlReadPort(IN PIRP pIrp, PIO_STACK_LOCATION pIrpStack)
{
	IOSTRUCT	*pIoIn	= NULL;	

	// Pointer Ok?
	if (!pIrp || !pIrpStack)
		return STATUS_INVALID_PARAMETER;

    pIrp->IoStatus.Information = 0;

	// Size of buffer containing data from application 
 	if (pIrpStack->Parameters.DeviceIoControl.InputBufferLength != sizeof(IOSTRUCT))
	{
		Out_Debug_String(("AKU_SYS: ERROR AkuIOCtlReadPort InBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

    // Size of buffer for data to be sent to application 
	if (pIrpStack->Parameters.DeviceIoControl.OutputBufferLength != sizeof(IOSTRUCT))
	{
		Out_Debug_String(("AKU_SYS: ERROR AkuIOCtlReadPort OutBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}
    
	pIoIn = (IOSTRUCT*)pIrp->AssociatedIrp.SystemBuffer;
	if (!pIoIn)
	{
		Out_Debug_String("AKU_SYS: ERROR AkuIOCtlReadPort pIoIn = NULL!\n");
		return STATUS_INVALID_PARAMETER;
	}

	switch (pIoIn->wIOSize)
	{
		case IO_SIZE8:
			if (pIoIn->bLatch)
				pIoIn->byVal = ISAInputLatch8(pIoIn->dwAddr);
			else
				pIoIn->byVal = ISAInput8(pIoIn->dwAddr);
			break;
		case IO_SIZE16:
			if (pIoIn->bLatch)
				pIoIn->wVal = ISAInputLatch16(pIoIn->dwAddr);
			else
				pIoIn->wVal = ISAInput16(pIoIn->dwAddr);
			break;
		case IO_SIZE32:
			if (pIoIn->bLatch)
				pIoIn->dwVal = ISAInputLatch32(pIoIn->dwAddr);
			else
				pIoIn->dwVal = ISAInput32(pIoIn->dwAddr);
			break;
	}

    pIrp->IoStatus.Information = sizeof(IOSTRUCT);

	return STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////*/
VOID AkuUnload(PDRIVER_OBJECT DriverObject)
{
    PLOCAL_DEVICE_INFO pLDI;
    CM_RESOURCE_LIST NullResourceList;
    BOOLEAN ResourceConflict;
    UNICODE_STRING Win32DeviceName;

    // Find our global data
    pLDI = (PLOCAL_DEVICE_INFO)DriverObject->DeviceObject->DeviceExtension;

	// Pointer Ok?
	if (!pLDI)
		return;

    // Evtl. noch aufräumen
    CleanUp();

	// Latchspeicher freigeben
	PageFree(&g_Latch8);
	PageFree(&g_Latch16);
	PageFree(&g_Latch32);
    
	// Report we're not using any hardware.  If we don't do this
    // then we'll conflict with ourselves (!) on the next load
    RtlZeroMemory((PVOID)&NullResourceList, sizeof(NullResourceList));

    IoReportResourceUsage(
              NULL,
              DriverObject,
              &NullResourceList,
              sizeof(ULONG),
              NULL,
              NULL,
              0,
              FALSE,
              &ResourceConflict );

    // Assume all handles are closed down.
    // Delete the things we allocated - devices, symbolic links
    RtlInitUnicodeString(&Win32DeviceName, DOS_DEVICE_NAME);

    IoDeleteSymbolicLink(&Win32DeviceName);

    IoDeleteDevice(pLDI->DeviceObject);
}

/////////////////////////////////////////////////////////////////////////////////////
NTSTATUS CleanUp()
{
	NTSTATUS	status	= STATUS_SUCCESS;

	Out_Debug_String("AKU_SYS: CleanUp\n");

	return status;
}

////////////////////////////////////////////////////////////////////////////////////
BOOL PageAllocate(MEMSTRUCT *pMem, DWORD dwLen)
{
	PHYSICAL_ADDRESS phys;
	PMDL	pMdl;

	// Parameter Ok?
	if ((!pMem) || (dwLen == 0))
		return FALSE;

	pMem->pLinAddr = MmAllocateNonCachedMemory(dwLen);

	if (!pMem->pLinAddr)
		return FALSE;

	phys = MmGetPhysicalAddress(pMem->pLinAddr);
	pMem->pPhysAddr = (void*)phys.LowPart;
	pMem->dwLen = dwLen;

	// Auf 0 initialisieren.
	RtlZeroMemory(pMem->pLinAddr, pMem->dwLen);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////
BOOL PageFree(MEMSTRUCT *pMem)
{
	// Parameter Ok?
	if (!pMem)
		return FALSE;

	// Codebuffer Tabelle freigeben
	if ((pMem->pLinAddr) && (pMem->dwLen > 0))
	{
		MmFreeNonCachedMemory(pMem->pLinAddr, pMem->dwLen);

		pMem->pLinAddr  = NULL;
		pMem->pPhysAddr	= NULL;
		pMem->dwLen		= 0;
	}
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////
void Out_Debug_String(char* lpString)
{
	KdPrint((lpString));
}

///////////////////////////////////////////////////////////////////////////////////
void ClaimPortResource(WORD wI, CM_RESOURCE_LIST *pRLst, DWORD dwAddr, DWORD dwCnt)
{
	if (!pRLst)
		return;

	pRLst->List[0].PartialResourceList.Count++;
	pRLst->List[0].PartialResourceList.PartialDescriptors[wI].Type					= CmResourceTypePort;
	pRLst->List[0].PartialResourceList.PartialDescriptors[wI].ShareDisposition		= CmResourceShareShared;
	pRLst->List[0].PartialResourceList.PartialDescriptors[wI].u.Port.Start.LowPart	= dwAddr;
	pRLst->List[0].PartialResourceList.PartialDescriptors[wI].u.Port.Length			= dwCnt;
	pRLst->List[0].PartialResourceList.PartialDescriptors[wI].Flags					= CM_RESOURCE_PORT_IO;
}

///////////////////////////////////////////////////////////////////////////////////
void ClaimIRQResource(WORD wI, CM_RESOURCE_LIST *pRLst, DWORD dwInterrupt)
{
	if (!pRLst)
		return;

	pRLst->List[0].PartialResourceList.Count++;
	pRLst->List[0].PartialResourceList.PartialDescriptors[wI].Type					= CmResourceTypeInterrupt;
	pRLst->List[0].PartialResourceList.PartialDescriptors[wI].ShareDisposition		= CmResourceShareDeviceExclusive;
	pRLst->List[0].PartialResourceList.PartialDescriptors[wI].u.Interrupt.Level		= dwInterrupt;
	pRLst->List[0].PartialResourceList.PartialDescriptors[wI].u.Interrupt.Vector	= dwInterrupt;
	pRLst->List[0].PartialResourceList.PartialDescriptors[wI].Flags					= CM_RESOURCE_INTERRUPT_LATCHED;
}

///////////////////////////////////////////////////////////////////////////////////
void ClaimMemoryResource(WORD wI, CM_RESOURCE_LIST *pRLst, DWORD dwAddr, DWORD dwCnt)
{
	if (!pRLst)
		return;

	pRLst->List[0].PartialResourceList.Count++;
	pRLst->List[0].PartialResourceList.PartialDescriptors[wI].Type					= CmResourceTypeMemory;
	pRLst->List[0].PartialResourceList.PartialDescriptors[wI].ShareDisposition		= CmResourceShareDeviceExclusive;
	pRLst->List[0].PartialResourceList.PartialDescriptors[wI].u.Port.Start.LowPart	= dwAddr;
	pRLst->List[0].PartialResourceList.PartialDescriptors[wI].u.Port.Length			= dwCnt;
	pRLst->List[0].PartialResourceList.PartialDescriptors[wI].Flags					= CM_RESOURCE_MEMORY_READ_WRITE;
}

///////////////////////////////////////////////////////////////////////////////////
DWORD ReadProfile(IN PWCHAR RegistryPath, PWCHAR ValueName, DWORD Default)
{ 
    RTL_QUERY_REGISTRY_TABLE    Table[2]; 
    ULONG						RetVal	= 0;

	// Parameter Ok?
	if ((!RegistryPath) || (!ValueName))
    {
		Out_Debug_String("AKU_SYS: ERROR ReadProfile RegistryPath or ValueName == NULL\n");	
		return Default;
	}

	RtlZeroMemory(&Table[0], sizeof(Table)); 

    Table[0].Flags			= RTL_QUERY_REGISTRY_DIRECT; 
    Table[0].Name			= ValueName; 
    Table[0].EntryContext	= &RetVal; 
    Table[0].DefaultType	= REG_DWORD; 
    Table[0].DefaultData	= &Default; 
    Table[0].DefaultLength	= sizeof(ULONG); 

    if (!NT_SUCCESS(RtlQueryRegistryValues(RTL_REGISTRY_ABSOLUTE, RegistryPath, &Table[0], NULL, NULL))) 
    { 
        RetVal = Default; 
    } 

	return RetVal;
} 
