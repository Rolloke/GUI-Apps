//======================================================================
//
// KeyHook
//
// Copyright (C) 1996-1999 Mark Russinovich
//
// Hook onto the keyboard I/O path and massage the input stream
// converting caps-locks into controls. This example works on
// NT 4 and Win2K and the Win2K version is very losely based on the
// i8042 port filter driver sample, kbfiltr, from the Win2K DDK.
//
// For every function I list whether the function gets called
// under NT 4, WIN2K, or both.
//
// File: ctrl2cap.c
//
//======================================================================
#include <ntddk.h>
#include <ntddkbd.h>
#include "stdarg.h"
#include "stdio.h"
#include "keyhook.h"
#include "ntddkbd.h"

#define DebugTrace DbgPrint

BOOL bAlt			= FALSE;
BOOL bCtrl			= FALSE;
BOOL bDel			= FALSE;
BOOL bCtrlAltDel	= FALSE;
BOOL bWin			= FALSE;
BOOL bF4			= FALSE;

BOOL	g_bIsWinNT4		= TRUE;

//----------------------------------------------------------------------
//
// DriverEntry
//
// Installable driver initialization. Here we just set ourselves up.
// 
// Called: NT4
//
//----------------------------------------------------------------------
NTSTATUS DriverEntry(IN PDRIVER_OBJECT  DriverObject, IN PUNICODE_STRING RegistryPath)
{
    ULONG i;
	ULONG				MajorVersion		= 0;
	ULONG				MinorVersion		= 0;

    DebugTrace("KeyHook.SYS: entering DriverEntry\n");

	// WinNT?, dann kein PnP möglich
	PsGetVersion(&MajorVersion, &MinorVersion, NULL, NULL);
	g_bIsWinNT4 = ((MajorVersion == 4) && (MinorVersion == 0));

	if (MajorVersion==4 && 	MinorVersion==0)
		DebugTrace("SaVic Driver: OSVersion=%d.%d (WindowsNT 4)\n", MajorVersion, MinorVersion);
	else if (MajorVersion==4 && MinorVersion==10)
		DebugTrace("SaVic Driver: OSVersion=%d.%d (Windows 98)\n", MajorVersion, MinorVersion);
	else if (MajorVersion==4 && MinorVersion==90)
		DebugTrace("SaVic Driver: OSVersion=%d.%d (Windows Me)\n", MajorVersion, MinorVersion);
	else if (MajorVersion==5 &&	MinorVersion==0)
		DebugTrace("SaVic Driver: OSVersion=%d.%d (Windows 2000)\n", MajorVersion, MinorVersion);
	else if (MajorVersion==5 &&	MinorVersion==1)
		DebugTrace("SaVic Driver: OSVersion=%d.%d (Windows XP)\n", MajorVersion, MinorVersion);
	else
		DebugTrace("SaVic Driver: OSVersion:Unknown\n");

    // 
    // Fill in all the dispatch entry points with the pass through function
    // and the explicitly fill in the functions we are going to intercept
    // 
    for (i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++) {

        DriverObject->MajorFunction[i] = KeyHookDispatchGeneral;
    }

    //
    // Our read function is where we do our real work.
    //
    DriverObject->MajorFunction[IRP_MJ_READ] = KeyHookDispatchRead;

	if (g_bIsWinNT4)
	{
		//
		// Under NT 4 we go out and hook the keyboard class device for 
		// keyboard 0.
		//
		return KeyHookInit( DriverObject );
	}
	else  // Win2K
	{
		//
		// Power IRPs are the only ones we have to handle specially under
		// Win2k since they require the special PoCallDriver and 
		// PoStartNextPowerIrp function calls.
		// Damit der KeyHook.sys auch unter WinNT4 läuft, darf 'PoStartNextPowerIrp' nicht verwendet
		// werden. Deshalb KEIN PowerManagement einbinden
		DriverObject->MajorFunction [IRP_MJ_POWER]  = NULL; //KeyHookPower;
		//
		// The only reason we need to handle PnP IRPs is to know when
		// a device we've attached to disappears (is removed).
		//
		DriverObject->MajorFunction [IRP_MJ_PNP]  = KeyHookPnP;

		//
		// Under Win2K we get told about the presence of keyboard 
		// devices through our AddDevice entry point.
		//
		DriverObject->DriverUnload = KeyHookUnload;
		DriverObject->DriverExtension->AddDevice = KeyHookAddDevice;

		return STATUS_SUCCESS;
	}
}


//----------------------------------------------------------------------
//
// KeyHookInit
//
// Hook onto the keyboard's path. Why does this routine return
// status success even if there's a problem? I've found that if it 
// doesn't, the keyboard won't respond!
// 
// Called: NT4
//
//----------------------------------------------------------------------
NTSTATUS KeyHookInit(IN PDRIVER_OBJECT DriverObject)
{
    CCHAR		      ntNameBuffer[64];
    STRING		      ntNameString;
    UNICODE_STRING    ntUnicodeString;
    PDEVICE_OBJECT    device;
    NTSTATUS          status;
    PDEVICE_EXTENSION devExt;
    WCHAR             messageBuffer[]  = L"KeyHook Initialized\n";
    UNICODE_STRING    messageUnicodeString;

    //
    // Only hook onto the first keyboard's chain.
    //
    sprintf( ntNameBuffer, "\\Device\\KeyboardClass0" );
    RtlInitAnsiString( &ntNameString, ntNameBuffer );
    RtlAnsiStringToUnicodeString( &ntUnicodeString, &ntNameString, TRUE );

    //
    // Create device object for the keyboard.
    //
    status = IoCreateDevice( DriverObject,
                             sizeof(DEVICE_EXTENSION),
                             NULL,
                             FILE_DEVICE_KEYBOARD,
                             0,
                             FALSE,
                             &device );

    if (!NT_SUCCESS(status))
	{

        DebugTrace("KeyHook: Keyboard hook failed to create device!\n");

        RtlFreeUnicodeString( &ntUnicodeString );
        return STATUS_SUCCESS;
    }

    RtlZeroMemory(device->DeviceExtension, sizeof(DEVICE_EXTENSION));

    devExt = (PDEVICE_EXTENSION) device->DeviceExtension;

    //
    // Keyboard uses buffered I/O so we must as well.
    //
    device->Flags |= DO_BUFFERED_IO;
    device->Flags &= ~DO_DEVICE_INITIALIZING;

    //
    // Attach to the keyboard chain.
    //
    status = IoAttachDevice( device, &ntUnicodeString, &devExt->TopOfStack );

    if (!NT_SUCCESS(status))
	{

        DebugTrace("KeyHook: Connect with keyboard failed!\n");
        IoDeleteDevice( device );
        RtlFreeUnicodeString( &ntUnicodeString );
        return STATUS_SUCCESS;
    }

    //
    // Done! Just free our string and be on our way...
    //
    RtlFreeUnicodeString(&ntUnicodeString );
    DebugTrace("KeyHook: Successfully connected to keyboard device\n");

    //
    // This line simply demonstrates how a driver can print
    // stuff to the bluescreen during system initialization. 
    //
    RtlInitUnicodeString(&messageUnicodeString, messageBuffer);
//    ZwDisplayString( &messageUnicodeString );

	return STATUS_SUCCESS;
}

//----------------------------------------------------------------------
//
// KeyHookAddDevice
//
// The PnP Manager calls us for each keyboard present on the system.
// We attach to each one so that we can flip caps lock to controls.
//
// Called: WIN2K
//
//----------------------------------------------------------------------
NTSTATUS KeyHookAddDevice(IN PDRIVER_OBJECT Driver, IN PDEVICE_OBJECT PDO)
{
    PDEVICE_EXTENSION        devExt;
    IO_ERROR_LOG_PACKET      errorLogEntry;
    PDEVICE_OBJECT           device;
    NTSTATUS                 status = STATUS_SUCCESS;

    //
    // Create a filter device and attach it to the device stack.
    //
    status = IoCreateDevice(Driver,                   
                            sizeof(DEVICE_EXTENSION), 
                            NULL,                    
                            FILE_DEVICE_KEYBOARD,   
                            0,                     
                            FALSE,                
                            &device              
                            );

    if (!NT_SUCCESS(status)) {

        return (status);
    }

    RtlZeroMemory(device->DeviceExtension, sizeof(DEVICE_EXTENSION));

    devExt = (PDEVICE_EXTENSION) device->DeviceExtension;
    devExt->TopOfStack = IoAttachDeviceToDeviceStack(device, PDO);

    ASSERT(devExt->TopOfStack);

    device->Flags |= (DO_BUFFERED_IO | DO_POWER_PAGABLE);
    device->Flags &= ~DO_DEVICE_INITIALIZING;
    return status;
}


//----------------------------------------------------------------------
//
// KeyHookPnP
//
// We have to handle PnP IRPs so that we detach from target
// devices when appropriate.
//
// Called: WIN2K
//
//----------------------------------------------------------------------
NTSTATUS KeyHookPnP(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
    PDEVICE_EXTENSION           devExt; 
    PIO_STACK_LOCATION          irpStack;
    NTSTATUS                    status = STATUS_SUCCESS;
    KIRQL                       oldIrql;
    KEVENT                      event;        

    devExt = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    irpStack = IoGetCurrentIrpStackLocation(Irp);

    switch (irpStack->MinorFunction) {
    case IRP_MN_REMOVE_DEVICE:
        
        //
        // Detach from the target device after passing the IRP
        // down the devnode stack.
        //
        IoSkipCurrentIrpStackLocation(Irp);
        IoCallDriver(devExt->TopOfStack, Irp);

        IoDetachDevice(devExt->TopOfStack); 
        IoDeleteDevice(DeviceObject);

        status = STATUS_SUCCESS;
        break;

    case IRP_MN_SURPRISE_REMOVAL:

        //
        // Same as a remove device, but don't call IoDetach or IoDeleteDevice.
        //
        IoSkipCurrentIrpStackLocation(Irp);
        status = IoCallDriver(devExt->TopOfStack, Irp);
        break;

    case IRP_MN_START_DEVICE: 
    case IRP_MN_QUERY_REMOVE_DEVICE:
    case IRP_MN_QUERY_STOP_DEVICE:
    case IRP_MN_CANCEL_REMOVE_DEVICE:
    case IRP_MN_CANCEL_STOP_DEVICE:
    case IRP_MN_FILTER_RESOURCE_REQUIREMENTS: 
    case IRP_MN_STOP_DEVICE:
    case IRP_MN_QUERY_DEVICE_RELATIONS:
    case IRP_MN_QUERY_INTERFACE:
    case IRP_MN_QUERY_CAPABILITIES:
    case IRP_MN_QUERY_DEVICE_TEXT:
    case IRP_MN_QUERY_RESOURCES:
    case IRP_MN_QUERY_RESOURCE_REQUIREMENTS:
    case IRP_MN_READ_CONFIG:
    case IRP_MN_WRITE_CONFIG:
    case IRP_MN_EJECT:
    case IRP_MN_SET_LOCK:
    case IRP_MN_QUERY_ID:
    case IRP_MN_QUERY_PNP_DEVICE_STATE:
    default:
        //
        // Pass these through untouched
        //
        IoSkipCurrentIrpStackLocation(Irp);
        status = IoCallDriver(devExt->TopOfStack, Irp);
        break;
    }

    return status;
}


//----------------------------------------------------------------------
//
// KeyHookPower
//
// We have to handle Power IRPs specially.
//
// Called: WIN2K
//
//----------------------------------------------------------------------
NTSTATUS KeyHookPower(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
    PDEVICE_EXTENSION   devExt;
    
    devExt = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    //
    // Let the next power IRP out of the gate
    // 
    PoStartNextPowerIrp( Irp );
    
    //
    // Pass this power IRP to the keyboard class driver
    //
    IoSkipCurrentIrpStackLocation( Irp );
    
    return PoCallDriver( devExt->TopOfStack, Irp );
}


//----------------------------------------------------------------------
// 
// KeyHookUnload
//
// Our Win2K PnP unload function. We don't need to do anything.
//
// Called: WIN2K
//
//----------------------------------------------------------------------
VOID KeyHookUnload(IN PDRIVER_OBJECT Driver)
{
    UNREFERENCED_PARAMETER(Driver);

    ASSERT(NULL == Driver->DeviceObject);
}

//----------------------------------------------------------------------
// 
// KeyHookReadComplete
//
// Gets control after a read operation has completed.
//
// Called: WIN2K, NT4
//
//----------------------------------------------------------------------
NTSTATUS KeyHookReadComplete(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp, IN PVOID Context)
{
    PIO_STACK_LOCATION        IrpSp;
    PKEYBOARD_INPUT_DATA      KeyData;
    int                       numKeys, i, j;

	//
    // Request completed - look at the result.
    //
    IrpSp = IoGetCurrentIrpStackLocation( Irp );
    if( NT_SUCCESS( Irp->IoStatus.Status ) ) {

        //
        // Do caps-lock down and caps-lock up. Note that
        // just frobbing the MakeCode handles both the up-key
        // and down-key cases since the up/down information is specified
        // seperately in the Flags field of the keyboard input data 
        // (0 means key-down, 1 means key-up).
        //

		KeyData = Irp->AssociatedIrp.SystemBuffer;
        numKeys = Irp->IoStatus.Information / sizeof(KEYBOARD_INPUT_DATA);

        for (i = 0; i < numKeys; i++)
		{
//            DebugTrace("ScanCode: %x %s Flags=0x%x", KeyData[i].MakeCode, (KeyData[i].Flags & 0x01 == KEY_UP) ? "Up" : "Down", KeyData[i].Flags);
//     		DebugTrace("ScanCode: 0x%x\n", KeyData[i].MakeCode);

			if ((KeyData[i].Flags & 0x01) == KEY_DOWN)
				OnKeyDown(KeyData[i].MakeCode);
     		if ((KeyData[i].Flags & 0x01) == KEY_UP)
				OnKeyUp(KeyData[i].MakeCode);
			if ((bCtrl && bAlt && bDel) || // Ctrl Alt Del
				(bAlt && bF4) ||		   // Alt F4
				bWin)					   // WindowsTaste
			{
				KeyData[i].MakeCode = KEY_ROLL;
			}
		}					
	}					

    //
    // Mark the Irp pending if required
    //
    if( Irp->PendingReturned ) {

        IoMarkIrpPending( Irp );
    }
    return Irp->IoStatus.Status;
}

//----------------------------------------------------------------------
//
// KeyHookDispatchRead
//
// Sets up to look at the read request completion so that we can
// massage the input queue on IO completion.
//
// Called: WIN2K, NT4
//
//----------------------------------------------------------------------
NTSTATUS KeyHookDispatchRead(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp )
{
    PDEVICE_EXTENSION   devExt;
    PIO_STACK_LOCATION  currentIrpStack;
    PIO_STACK_LOCATION  nextIrpStack;

    //
    // Gather our variables.
    //
    devExt = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    currentIrpStack = IoGetCurrentIrpStackLocation(Irp);
    nextIrpStack = IoGetNextIrpStackLocation(Irp);    

    //
    // Push params down for keyboard class driver.
    //
    *nextIrpStack = *currentIrpStack;

    //  
    // Set the completion callback, so we can "frob" the keyboard data.
    //	    
    IoSetCompletionRoutine( Irp, KeyHookReadComplete, 
                            DeviceObject, TRUE, TRUE, TRUE );

    //
    // Return the results of the call to the keyboard class driver.
    //
    return IoCallDriver( devExt->TopOfStack, Irp );
}


//----------------------------------------------------------------------
//
// KeyHookDispatchGeneral
//
// This handles several functions we are not interested in 
// along to the keyboard class driver. 
//
// Called: WIN2K, NT4
//
//----------------------------------------------------------------------
NTSTATUS KeyHookDispatchGeneral(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
    //
    // This is the equivalent of the IoSkipCurrentIrpStackLocation macro,
    // which doesn't exist in the NT 4 DDK.
    //
	if (g_bIsWinNT4)
	{	
		Irp->CurrentLocation++;
		Irp->Tail.Overlay.CurrentStackLocation++;
	}
	else
	{
	    IoSkipCurrentIrpStackLocation(Irp);
	}

    return IoCallDriver(((PDEVICE_EXTENSION) DeviceObject->DeviceExtension)->TopOfStack, Irp);
}

//----------------------------------------------------------------------
VOID OnKeyDown(USHORT uScanCode)
{
	switch (uScanCode)
	{
		case KEY_CTRL:
            if (!bCtrl)
				DebugTrace("Ctrl down\n");
			bCtrl = TRUE;
			break;
		case KEY_ALT:
            if (!bAlt)
	            DebugTrace("Alt down\n");
			bAlt = TRUE;
			break;
		case KEY_DEL:
			if (!bDel)
				DebugTrace("Del down\n");
			if (bCtrl && bAlt && !bDel)
				DebugTrace("-->CtrlAltDel");
			bDel = TRUE;
			break;
		case KEY_F4:
			if (!bF4)
				DebugTrace("F4 down\n");
			if (bAlt && !bF4)
				DebugTrace("-->Alt F4\n");
			bF4 = TRUE;
			break;
		case KEY_LWIN:
		case KEY_RWIN1:
			if (!bWin)
				DebugTrace("LWin/RWin down\n");
			bWin = TRUE;
			break;
	}
}

//----------------------------------------------------------------------
VOID OnKeyUp(USHORT uScanCode)
{
	switch (uScanCode)
	{
		case KEY_CTRL:
   			if (bCtrl)
				DebugTrace("Ctrl up\n");
			bCtrl = FALSE;
			break;
		case KEY_ALT:
   			if (bAlt)
	            DebugTrace("Alt up\n");
			bAlt = FALSE;
			break;
		case KEY_DEL:
   			if (bDel)
	            DebugTrace("Del up\n");
			bDel = FALSE;
			break;
		case KEY_F4:
			if (!bF4)
				DebugTrace("F4 up\n");
			bF4 = FALSE;
			break;
		case KEY_LWIN:
		case KEY_RWIN1:
			if (!bWin)
				DebugTrace("LWin/RWin up\n");
			bWin = FALSE;
			break;
	}
}

