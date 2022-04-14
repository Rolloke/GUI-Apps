/////////////////////////////////////////////////////////////////////////////
// PROJECT:		HAcc
// FILE:		$Workfile: HAccSys.c $
// ARCHIVE:		$Archive: /Project/Units/Tasha(Developer)/haccsys/HAccSys.c $
// CHECKIN:		$Date: 5.01.04 9:52 $
// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 5.01.04 9:25 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "Driver.h"

// Kopie der Configpointer zwecks Aufräumarbeiten (falls nötig)
CONFIG* g_pBackupConfig[4]	= {NULL, NULL, NULL, NULL};

// Pointer auf Latch
MEMSTRUCT	g_Latch8;
MEMSTRUCT	g_Latch16;
MEMSTRUCT	g_Latch32;
MEMSTRUCT	g_FrameBuffer;

// Schlüssel zur Konfiguration
WCHAR	g_sDevice1[] = L"\\Registry\\Machine\\Software\\HACC\\Device1\\System";
WCHAR	g_sDevice2[] = L"\\Registry\\Machine\\Software\\HACC\\Device2\\System";
WCHAR	g_sDevice3[] = L"\\Registry\\Machine\\Software\\HACC\\Device3\\System";
WCHAR	g_sDevice4[] = L"\\Registry\\Machine\\Software\\HACC\\Device4\\System";

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

	HAccReportEvent(HACC_MSG_LOGGING_ENABLED, HACC_ERRORLOG_INIT, (PVOID)DriverObject, NULL, NULL, 0, NULL, 0);				// No strings
    Out_Debug_String("HAcc Driver: DriverEntry\n");

	g_bHasSFence = SupportCheckForSFence();
	g_bHasMMX	 = HasMMX();

	// Alle notwendigen Resourcen der HACC(s) reservieren
	ResourceConflict = TRUE;
	RtlZeroMemory(ResList, sizeof(ResList));
	pResourceList->Count					= 1;
	pResourceList->List[0].InterfaceType	= PCIBus; //Isa;
	pResourceList->List[0].BusNumber		= 0;

	ClaimHAccXResource(g_sDevice1, pResourceList, TRUE);
	ClaimHAccXResource(g_sDevice2, pResourceList, FALSE);
	ClaimHAccXResource(g_sDevice3, pResourceList, FALSE);
	ClaimHAccXResource(g_sDevice4, pResourceList, FALSE);

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
		HAccReportEvent(HACC_MSG_CANT_CLAIM_RESOURCES, HACC_ERRORLOG_INIT, (PVOID)DriverObject, NULL, NULL, 0, NULL, 0);				// No strings
        KdPrint( ("HAcc Driver: Resource reporting problem %8X\n", Status) );
        return Status;
    }

    // Initialize the driver object dispatch table.
    DriverObject->MajorFunction[IRP_MJ_CREATE]          = HAccDispatch;
    DriverObject->MajorFunction[IRP_MJ_CLOSE]           = HAccDispatch;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL]  = HAccDispatch;
    DriverObject->DriverUnload                          = HAccUnload;

    // Create our device.
    Status = HAccCreateDevice(HACC_DEVICE_NAME, HACC_TYPE, DriverObject, &DeviceObject);

    if (NT_SUCCESS(Status))
    {
        // Initialize the local driver info for each device object.
        pLocalInfo = (PLOCAL_DEVICE_INFO)DeviceObject->DeviceExtension;
        pLocalInfo->DeviceObject    = DeviceObject;
        pLocalInfo->DeviceType      = HACC_TYPE;
	}
    else
    {
		HAccReportEvent(HACC_MSG_CANT_CREATE_DEVICE, HACC_ERRORLOG_INIT, (PVOID)DriverObject, NULL, NULL, 0, NULL, 0);				// No strings
        KdPrint( ("HAcc Driver: CreateDevice failed %8X\n", Status) );

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
	PageAllocate(&g_Latch8,  LATCH_SIZE * sizeof(BYTE), FALSE);
	PageAllocate(&g_Latch16, LATCH_SIZE * sizeof(WORD), FALSE);
	PageAllocate(&g_Latch32, LATCH_SIZE * sizeof(DWORD), FALSE);

	if (!PageAllocate(&g_FrameBuffer, 16*(8*64*1024), TRUE))	// 8MByte Buffer.
    {
		Out_Debug_String("HAcc Driver: ERROR PageAllocating of 8MB Framebuffer failed\n\r");
		return STATUS_NO_MEMORY;
	}

	// Indicating drive startup
	HAccReportEvent(HACC_MSG_DRIVER_STARTING, HACC_ERRORLOG_INIT, (PVOID)DriverObject, NULL, NULL, 0, NULL, 0);				// No strings

	return Status;
}

/////////////////////////////////////////////////////////////////////////////////////
NTSTATUS HAccCreateDevice(	IN  PWSTR			PrototypeName,
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
NTSTATUS HAccDispatch(IN PDEVICE_OBJECT pDO, IN PIRP pIrp)
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
VOID HAccUnload(PDRIVER_OBJECT DriverObject)
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
    CleanUp(g_pBackupConfig[0]);
    CleanUp(g_pBackupConfig[1]);
    CleanUp(g_pBackupConfig[2]);
    CleanUp(g_pBackupConfig[3]);

	// Latchspeicher freigeben
	PageFree(&g_Latch8,  FALSE);
	PageFree(&g_Latch16, FALSE);
	PageFree(&g_Latch32, FALSE);
    
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
	HAccReportEvent(HACC_MSG_DRIVER_STOPPING, HACC_ERRORLOG_UNLOAD, (PVOID)DriverObject, NULL, NULL, 0, NULL, 0);				// No strings
}

///////////////////////////////////////////////////////////////////////////////////
BOOL MapPhysToLin(MEMSTRUCT *pMem)
{
	PHYSICAL_ADDRESS	phys;

	// Parameter Ok?
	if (!pMem || !pMem->pPhysAddr || pMem->dwLen == 0)
		return FALSE;

	phys.LowPart = (ULONG)pMem->pPhysAddr;
	phys.HighPart= 0;

	pMem->pLinAddr = MmMapIoSpace(phys,	pMem->dwLen, MmNonCached); //FALSE);
	if (!pMem->pLinAddr)
	{
		Out_Debug_String(("HAcc Driver: ERROR MapPhysToLin pMem->pLinAddr = NULL\n"));
		return FALSE;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL UnmapPhysToLin(MEMSTRUCT *pMem)
{
	MmUnmapIoSpace((void*)pMem->pLinAddr, pMem->dwLen);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL MapLinToPhys(MEMSTRUCT *pMem)
{
	PHYSICAL_ADDRESS	phys;

	// Parameter Ok?
	if (!pMem || !pMem->pLinAddr)
		return FALSE;

	phys = MmGetPhysicalAddress(pMem->pLinAddr);

	pMem->pPhysAddr = (void*)phys.LowPart;
	if (!pMem->pPhysAddr)
	{
		Out_Debug_String(("HAcc Driver: ERROR MapLinToPhys pMem->pPhysAddr = NULL\n"));
		return FALSE;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL UnmapIOSpace(CONFIG* pConfig)
{
	// Parameter Ok?
	if (!pConfig)
		return FALSE;

	if (!pConfig->dwIOBase)
		return FALSE;

	MmUnmapIoSpace((void*)pConfig->dwIOBase, 4096);

	// Pointer löschen
	pConfig->dwIOBase		= 0;

	return TRUE;
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

	HighestAcceptableAddress.QuadPart = 0x3FFFFFFF; // 1GByte
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

////////////////////////////////////////////////////////////////////////////////////
BOOL InstallISR(CONFIG *pConfig)
{
    static KIRQL	MaxInterruptRQL = 0;
	PKINTERRUPT		IntObj			= NULL;
    KAFFINITY		Affinity		= 0;
    KIRQL			InterruptRQL	= 0;
    KINTERRUPT_MODE InterruptMode	= Latched;
	ULONG			Vector			= 0;
    NTSTATUS		Status			= STATUS_INVALID_PARAMETER;
	BOOLEAN			bSharedIRQ		= FALSE;
	INTERFACE_TYPE	BusType			= Isa;

	if (!pConfig || pConfig->wIRQ == 0)
		return FALSE;

	// Diese Parameter sind vom Bustype abhängig
	bSharedIRQ		= (pConfig->wBusType == BUSTYPE_ISA ? FALSE : TRUE);
	BusType			= (pConfig->wBusType == BUSTYPE_ISA ? Isa	: PCIBus);
	InterruptMode	= (pConfig->wBusType == BUSTYPE_ISA ? Latched	: LevelSensitive);
	Vector			= HalGetInterruptVector(BusType, 0, pConfig->wIRQ, pConfig->wIRQ, &InterruptRQL, &Affinity);

	if (InterruptRQL >= MaxInterruptRQL)
		MaxInterruptRQL = InterruptRQL;

	Status = IoConnectInterrupt(&IntObj,
								(PKSERVICE_ROUTINE)ServiceHAccBdInt,
								(PVOID)pConfig,
								NULL,
								Vector,
								InterruptRQL,
								MaxInterruptRQL,
								InterruptMode,
								bSharedIRQ,
								Affinity,
								FALSE);
	
	if (Status == STATUS_SUCCESS)
		pConfig->pInterruptObject = IntObj;
	else
		pConfig->pInterruptObject = NULL;


    return ((BOOL) NT_SUCCESS(Status));
}

/////////////////////////////////////////////////////////////////////////////////////
void RemoveISR(CONFIG *pConfig)
{
	if (!pConfig)
		return;

	// ISR-ausklinken
	if (pConfig->pInterruptObject)
	{
		Out_Debug_String("HAcc Driver: RemoveISR\n");

		IoDisconnectInterrupt((PKINTERRUPT)pConfig->pInterruptObject);
		pConfig->pInterruptObject = NULL;
	}
}

////////////////////////////////////////////////////////////////////////////////
BOOL CloseAllEventHandles(CONFIG *pConfig)
{
	DWORD  dwRet	= 0;

	if (!pConfig)
		return FALSE;

	if (pConfig->hDebugEvent)    
	{	
		ObDereferenceObject((void*)pConfig->hDebugEvent);
		pConfig->hDebugEvent = 0;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////
void Sleep(DWORD dwSleep)
{
	KeStallExecutionProcessor(dwSleep*1000);
}

/////////////////////////////////////////////////////////////////////////////////////
BOOL ServiceHAccBdInt(IN PKINTERRUPT pInterruptObject,IN PVOID Context)
{
	CONFIG  *pConfig	= NULL;

	// Referenzdata holen
	pConfig = (CONFIG*)Context;

	if (!pConfig)
		return FALSE;

	// Kommt der Interrupt von einer HAcc
	if ((pConfig == g_pBackupConfig[0]) ||
		(pConfig == g_pBackupConfig[1])	||
		(pConfig == g_pBackupConfig[2])	||
		(pConfig == g_pBackupConfig[3]))
	{
		if (pConfig->wBusType == BUSTYPE_ISA)
			ISAServiceHAccBdInt(pConfig);					// ISA-ISR
		else if (pConfig->wBusType == BUSTYPE_PCI)
			PCIServiceHAccBdInt(pConfig);					// PCI-ISR
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////
BOOL SetDebugEvent(CONFIG* pConfig)
{
	if (!pConfig)
		return FALSE;

	KeSetEvent((PKEVENT)pConfig->hDebugEvent, 0, FALSE); 
 
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////
BOOL ResetEvent(DWORD hEvent)
{
	KeClearEvent((PKEVENT)hEvent);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////
void Out_Debug_String(char* lpString)
{
	KdPrint((lpString));
}

///////////////////////////////////////////////////////////////////////////////////
DWORD CreateRing0Event(DWORD hR3Event)
{
	HANDLE		hR0Event = 0;
	NTSTATUS	status	 = STATUS_NOT_IMPLEMENTED;

	if (!hR3Event)
		return 0;
   
	status = ObReferenceObjectByHandle((HANDLE)hR3Event, SYNCHRONIZE, NULL, KernelMode, &hR0Event, NULL);

	if (!NT_SUCCESS(status))
	   KdPrint(("\tUnable to reference User-Mode Event object, Error = 0x%x\n", status));

    return (DWORD)hR0Event;
}

///////////////////////////////////////////////////////////////////////////////////
WORD ClaimHAccXResource(WCHAR* sDevice, PCM_RESOURCE_LIST pResourceList, BOOL bFirst)
{
	DWORD  dwResource	= 0;
	static WORD wI		= 0;

	// Am Anfang auf 0 initialisieren
	if (bFirst)
		wI = 0;

	// PortIoBase der MiCoISA
	dwResource = ReadProfile(sDevice, L"MiCoIOBase", 0x00);
	if (dwResource != 0)
		ClaimPortResource(wI++, pResourceList, dwResource, 0x20);	

	// MemoryIOBase der HAcc
	dwResource = ReadProfile(sDevice, L"MemoryIOBase", 0x00);
	if (dwResource != 0)
		ClaimMemoryResource(wI++, pResourceList, dwResource, 0x1000);	

	// IRQ der HAcc
	dwResource = ReadProfile(sDevice, L"IRQ", 0x00);
	if (dwResource != 0)
		ClaimIRQResource(wI++, pResourceList, dwResource);	

	// PortIoBase der 1. CoVi8
	dwResource = ReadProfile(sDevice, L"ExtCard1IOBase", 0x00);
	if (dwResource != 0)
		ClaimPortResource(wI++, pResourceList, dwResource, 0x04);	

	// PortIoBase der 2. CoVi8
	dwResource = ReadProfile(sDevice, L"ExtCard2IOBase", 0x00);
	if (dwResource != 0)
		ClaimPortResource(wI++, pResourceList, dwResource, 0x04);	

	// PortIoBase der 3. CoVi8
	dwResource = ReadProfile(sDevice, L"ExtCard3IOBase", 0x00);
	if (dwResource != 0)
		ClaimPortResource(wI++, pResourceList, dwResource, 0x04);	

	return wI;
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
	pRLst->List[0].PartialResourceList.PartialDescriptors[wI].ShareDisposition		= CmResourceShareShared;
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
	pRLst->List[0].PartialResourceList.PartialDescriptors[wI].ShareDisposition		= CmResourceShareShared;
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
		Out_Debug_String("HAcc Driver: ERROR ReadProfile RegistryPath or ValueName == NULL\n");	
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

