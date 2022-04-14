
//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include <vckernel.h>
#include "vckpriv.h"
#include <mico.h>
#include <cmico.h>
#include "cbt856.h"
#include "cbt829.h"
#include "zr050.h"
#include "h55_tbls.h"
#include "hardware.h"
  			    
//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------
#define LATCH_SIZE      2048            // Latch Register storage
#define BT829INDEX      26              // Largest Index in BT829 Array
#define BT856INDEX      9               // Largest Index in BT856 Array

// Zoran Stuff
#define NO_OF_COMPONENTS          0x3                     //Y,U,V
#define BASELINE_PRECISION        0x8                     // ???
#define NO_OF_SCAN1_COMPONENTS    0x3                     //Y,U,V

//-----------------------------------------------------------------------------
// Static Storage
//-----------------------------------------------------------------------------

// Einige globale Variablen
BOOL	bGlobalSwitchAnalogVideo	= FALSE;
WORD	wGlobalSkipFieldCnt			= 0;
DWORD	dwGlobalFieldID				= 0;

BOOLEAN delay_op;
BOOLEAN Step_D;

BYTE irq_table[16] = {0,0,0,0,0,1,0,0,0,0,2,3,4,0,5,6};
BYTE Latchbuffer[LATCH_SIZE];

BYTE BT856_58MHZ_REG[BT856INDEX] = {0xce,0xd0,0xd2,0xd4,0xd6,0xd8,
                                    0xda,0xdc,0xde};

BYTE BT856_59MHZ_REG[BT856INDEX] = {0xce,0xd0,0xd2,0xd4,0xd6,0xd8,
                                    0xda,0xdc,0xde};

BYTE BT856_58MHZ[BT856INDEX] = {0,0,0,0,0,0,0,0x5e,0};

BYTE BT856_59MHZ[BT856INDEX] = {0,0,0,0,0,0,0,0x5e,0};

BYTE BT829_58MHZ_REG[BT829INDEX] = {0,1,2,3,4,5,6,7,8,9,0xa,0xb,0xc,0xd,0xe,0xf,
                                    0x10,0x11,0x12,0x13,0x14,0x16,0x18,0x19,
                                    0x1a,0x1b};
BYTE BT829_59MHZ_REG[BT829INDEX] = {0,1,2,3,4,5,6,7,8,9,0xa,0xb,0xc,0xd,0xe,0xf,
                                    0x10,0x11,0x12,0x13,0x14,0x16,0x18,0x19,
                                    0x1a,0x1b};

BYTE BT829_58MHZ[BT829INDEX][4] = {0x00,0x00,0x00,0x00,
                                   0x59,0x7b,0x59,0x7b,
                                   0x00,0x00,0x00,0x00,
                                   0x12,0x22,0x12,0x22,
                                   0x16,0x16,0x16,0x16,
                                   0xE0,0x40,0xE0,0x40,
                                   0x78,0x98,0x80,0x92,
                                   0x80,0xF0,0xD0,0xD0,
                                   0x01,0x03,0x00,0x04,
                                   0x02,0x9E,0xF8,0x7D,
                                   0x00,0x00,0x00,0x00,
                                   0xa3,0xa3,0xa3,0xa3,
                                   0xD8,0xD8,0xD8,0xD8,
                                   0xFE,0xFE,0xFE,0xFE,
                                   0xB4,0xB4,0xB4,0xB4,
                                   0x00,0x00,0x00,0x00,
                                   0x00,0x00,0x00,0x00,
                                   0x00,0x00,0x00,0x00,
                                   0x05,0x05,0x05,0x05,
                                   0x20,0x20,0x20,0x20,
                                   0x00,0x00,0x00,0x00,
                                   0x00,0x00,0x00,0x00,
                                   0x68,0x7F,0x68,0x7F,
                                   0x5D,0x72,0x5D,0x72,
                                   0x80,0x80,0x80,0x80,
                                   0x60,0x60,0x60,0x60};

BYTE BT829_59MHZ[BT829INDEX][4] = {0x00,0x00,0x00,0x00,
                                   0x59,0x7b,0x59,0x7b,
                                   0x00,0x00,0x00,0x00,
                                   0x12,0x23,0x12,0x22,
                                   0x16,0x16,0x16,0x16,
                                   0xE0,0x40,0xE0,0x40,
                                   0x78,0x9C,0x80,0x92,
                                   0x80,0x00,0xD0,0xD0,
                                   0x01,0x03,0x00,0x04,
                                   0x02,0x36,0xF8,0x7D,
                                   0x00,0x00,0x00,0x00,
                                   0xa3,0xa3,0xa3,0xa3,
                                   0xD8,0xD8,0xD8,0xD8,
                                   0xFE,0xFE,0xFE,0xFE,
                                   0xB4,0xB4,0xB4,0xB4,
                                   0x00,0x00,0x00,0x00,
                                   0x00,0x00,0x00,0x00,
                                   0x00,0x00,0x00,0x00,
                                   0x05,0x05,0x05,0x05,
                                   0x20,0x20,0x20,0x20,
                                   0x00,0x00,0x00,0x00,
                                   0x00,0x00,0x00,0x00,
                                   0x68,0x7F,0x68,0x7F,
                                   0x5D,0x72,0x5D,0x72,
                                   0x80,0x80,0x80,0x80,
                                   0x60,0x60,0x60,0x60};

//-----------------------------------------------------------------------------
// Zoran Coder Storage
//-----------------------------------------------------------------------------

BYTE QTid[8] = {0, 1, 1, 0, 0, 0, 0, 0};

BYTE SCAN1_HDCid[8] = {0, 1, 1, 0, 0, 0, 0, 0};
BYTE SCAN1_HACid[8] = {0, 1, 1, 0, 0, 0, 0, 0};

DWORD SOFMarkerSize;
DWORD SOSMarkerSize;
DWORD APPMarkerSize;
DWORD COMMarkerSize;
DWORD DRIMarkerSize;
DWORD DQTMarkerSize;
DWORD DHTMarkerSize;
DWORD DNLMarkerSize;
DWORD DQTIMarkerSize;
DWORD DHTIMarkerSize;

///////////////////////////////////////////////////////////////////////////////////
NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriverObject,IN PUNICODE_STRING RegistryPathName)
//
//
// Routine Description
//
//     NT-specific device load-time initialisation. This function registers with
//     vckernel.lib and fills in entries in the callback table. It is also
//     responsible for hardware detection and initialisation. This is the only
//     function in the hardware-specific code that is NT-specific.
//
// Arguments
//     pDriverObject - pointer to a driver object
//     RegistryPathName - path to the services node for this driver
//
// Return Value
//
//     Final status from initialisation.
//

{
	PDEVICE_INFO pDevInfo;
	PHWINFO pHw;
	PVC_CALLBACK pCallback;
	DWORD dwPort, dwInterrupt, sts, dwFrame, Diag_Test, i;
	KIRQL SaveIRQL;
	NTSTATUS Status;
	UCHAR ResBuffer[5 * (sizeof(CM_RESOURCE_LIST))];
	PCM_RESOURCE_LIST ResourceList;
	PCM_PARTIAL_RESOURCE_DESCRIPTOR Descriptor;
	BOOLEAN ResourceConflict;

	dprintf1(("DriverEntry called"));
	//
	//  Initialise the device object and allocate device-extension data
	//

	pDevInfo = VC_Init(pDriverObject,
	RegistryPathName,
	sizeof(HWINFO));

	if (pDevInfo == NULL)
	{
		VC_WriteProfile(pDevInfo, PARAM_ERROR, VC_ERR_CREATEDEVICE);
		return(STATUS_UNSUCCESSFUL);
	}

	//
	//  Read from the registry to get the default port/interrupt.
	//  Configuration parameters for the mico board include:
	//   - port address
	//   - interrupt
	//   - 8kb memory window for the VC Kernel to map frames
	//		     

	dwPort = VC_ReadProfile(pDevInfo, PARAM_PORT, DEF_PORT);
	dwInterrupt = VC_ReadProfile(pDevInfo, PARAM_INTERRUPT, DEF_INTERRUPT);
	dwFrame = VC_ReadProfile(pDevInfo, PARAM_FRAME, DEF_FRAME);

	dprintf1(("DriverEntry: board at port 0x%x, int %d, frame 0x%x", dwPort, dwInterrupt, dwFrame));


	//
	//  Now we can access the IO Port at the right Address....
	//  Map the port and frame buffer memory, and report the usage
	//  of the port, interrupt and physical memory addresses
	//
	if (!VC_GetResources(
		pDevInfo,
		pDriverObject,
		(PUCHAR) dwPort,
		NUMBER_OF_PORTS,
		dwInterrupt,
		FALSE,
		(PUCHAR) dwFrame,
		FRAME_WINDOW_SIZE))
		{
			VC_WriteProfile(pDevInfo, PARAM_ERROR, VC_ERR_CONFLICT);
			VC_Cleanup(pDriverObject);
			return(STATUS_UNSUCCESSFUL);
		}

	//
	//
	//  Fill in the callbacks before hw_init because hardware
	//  init can generate interrupts, and without the callback in
	//  place these will not be handled.
	//
	pCallback = VC_GetCallbackTable(pDevInfo);
	//
	//  ISR Routine
	//
	pCallback->InterruptAcknowledge = HW_InterruptAck;
	//
	//  Configure Input Source
	//
	pCallback->ConfigSourceFunc = HW_ConfigSource;
	//
	//  Device Open
	//
	pCallback->DeviceOpenFunc = HW_DeviceOpen;
	//
	//  Configure Video Format
	//
	pCallback->ConfigFormatFunc = HW_ConfigFormat;
	//
	//  Capture and Stream Functions
	//
	pCallback->CaptureFunc = HW_Capture;
	pCallback->StreamInitFunc = HW_StreamInit;
	pCallback->StreamFiniFunc = HW_StreamFini;
	pCallback->StreamStartFunc = HW_StreamStart;
	pCallback->StreamStopFunc = HW_StreamStop;
	pCallback->StreamGetPositionFunc = HW_StreamGetPosition;
	pCallback->CaptureService = HW_CaptureService;
	//
	//  Driver Unload
	//
	pCallback->CleanupFunc = HW_Cleanup;
	pCallback->DeviceCloseFunc = HW_Close;


	pHw = VC_GetHWInfo(pDevInfo);

	//
	//  TMP Set Registry
	//
#if 1
	MICOSaveRegistry(pDevInfo);
#endif
	//
	//  End of TMP
	//

	pHw->dwExtCardIO[0] = (DWORD)pDevInfo->PortBase;
	pHw->dwExtCardIO[1] = 0;
	pHw->dwExtCardIO[2] = 0;
	pHw->dwExtCardIO[3] = 0;
	//
	//  Set "Card present Bits" from Registry.
	//
	pHw->dwHardwareState = SETBIT(pHw->dwHardwareState,0);
	sts = VC_ReadProfile(pDevInfo, PARAM_EXC1, 0);
	if (sts != 0)
	{
		pHw->dwHardwareState = SETBIT(pHw->dwHardwareState,1);
		HW_MapPort(pDevInfo,(WORD)sts,0x1f);
		pHw->dwExtCardIO[1] = (DWORD) pDevInfo->PortBase;
	}

	sts = VC_ReadProfile(pDevInfo, PARAM_EXC2, 0);
	if (sts != 0)
	{
		pHw->dwHardwareState = SETBIT(pHw->dwHardwareState,2);
		HW_MapPort(pDevInfo,(WORD)sts,0x1f);
		pHw->dwExtCardIO[2] = (DWORD) pDevInfo->PortBase;
	}

	sts = VC_ReadProfile(pDevInfo, PARAM_EXC3, 0);
	if (sts != 0)
	{
		pHw->dwHardwareState = SETBIT(pHw->dwHardwareState,3);
		HW_MapPort(pDevInfo,(WORD)sts,0x1f);
		pHw->dwExtCardIO[3] = (DWORD) pDevInfo->PortBase;
	}

	//
	//  Claim Port - check Resource (Base plus potential ExtCards as defined in Registry)
	//
	ResourceList = (PCM_RESOURCE_LIST) ResBuffer;
	Descriptor = ResourceList->List[0].PartialResourceList.PartialDescriptors;
	ResourceConflict = TRUE;
	RtlZeroMemory(ResBuffer, sizeof(ResBuffer));
	ResourceList->Count = 1;
	ResourceList->List[0].InterfaceType = pDevInfo->BusType;
	ResourceList->List[0].BusNumber = pDevInfo->BusNumber;
	ResourceList->List[0].PartialResourceList.Count++;
	Descriptor->Type = CmResourceTypePort;
	Descriptor->ShareDisposition = CmResourceShareShared;
	Descriptor->u.Port.Start.LowPart = (DWORD) pHw->dwExtCardIO[0];
	Descriptor->u.Port.Length = 0x20;
	Descriptor->Flags = CM_RESOURCE_PORT_IO;
	Descriptor++;

	ResourceList->List[0].PartialResourceList.Count++;
	Descriptor->Type = CmResourceTypeInterrupt;
	Descriptor->ShareDisposition = CmResourceShareDeviceExclusive;
	Descriptor->u.Interrupt.Level = dwInterrupt;
	Descriptor->u.Interrupt.Vector = dwInterrupt;
	Descriptor->Flags = (WORD)CM_RESOURCE_INTERRUPT_LATCHED;
	Descriptor++;

	if (pHw->dwExtCardIO[1] != 0)
	{
		ResourceList->List[0].PartialResourceList.Count++;
		Descriptor->Type = CmResourceTypePort;
		Descriptor->ShareDisposition = CmResourceShareShared;
		Descriptor->u.Port.Start.LowPart = (DWORD) pHw->dwExtCardIO[1];
		Descriptor->u.Port.Length = 0x20;
		Descriptor->Flags = CM_RESOURCE_PORT_IO;
		Descriptor++;
	}

	if (pHw->dwExtCardIO[2] != 0)
	{
		ResourceList->List[0].PartialResourceList.Count++;
		Descriptor->Type = CmResourceTypePort;
		Descriptor->ShareDisposition = CmResourceShareShared;
		Descriptor->u.Port.Start.LowPart = (DWORD) pHw->dwExtCardIO[2];
		Descriptor->u.Port.Length = 0x20;
		Descriptor->Flags = CM_RESOURCE_PORT_IO;
		Descriptor++;
	}

	if (pHw->dwExtCardIO[3] != 0)
	{
		ResourceList->List[0].PartialResourceList.Count++;
		Descriptor->Type = CmResourceTypePort;
		Descriptor->ShareDisposition = CmResourceShareShared;
		Descriptor->u.Port.Start.LowPart = (DWORD) pHw->dwExtCardIO[3];
		Descriptor->u.Port.Length = 0x20;
		Descriptor->Flags = CM_RESOURCE_PORT_IO;
		Descriptor++;
	}
	Status = IoReportResourceUsage(NULL,
				  pDriverObject,
				  ResourceList,
				  (PUCHAR)Descriptor - (PUCHAR)ResourceList,
				  NULL,
				  NULL,
				  0,
				  FALSE,
				  &ResourceConflict);

	if (((BOOLEAN) NT_SUCCESS(Status)))
		dprintf(("IoReportResource passed."));

	if (ResourceConflict)
	{
		dprintf(("Resource conflict for port %x",pDevInfo->PortBase));
		VC_Cleanup(pDriverObject);
		return(STATUS_UNSUCCESSFUL);
	}

	pDevInfo->PortBase = (PUCHAR) pHw->dwExtCardIO[0];
	dprintf(("Base Port = 0x%x",pDevInfo->PortBase));

	//
	//  Init Latchbuffer to 0
	//
	for (i=0; i< LATCH_SIZE; i++) Latchbuffer[i] = 0;

	//
	//  Set IRQ
	//
	HW_SetIRQ(pDevInfo,pHw,dwInterrupt);

	dprintf1(("-----------  Starting Diagnostics -----------"));
	Diag_Test = 0;


	if (!HW_Init(pDevInfo))
	{
		dprintf1(("[2.?] Board init failed"));
		VC_WriteProfile(pDevInfo, PARAM_FAIL, 2);
	}
	else
	{
		Diag_Test++;
		dprintf1(("[2..] Board init passed"));
		VC_WriteProfile(pDevInfo, PARAM_DIAGS, 2);
	}

	//
	//  Connect the interrupt, now that we have initialised the hardware
	//  to prevent any random interrupts.
	//
	if (!VC_ConnectInterrupt(pDevInfo, dwInterrupt, FALSE))
	{
		VC_Cleanup(pDriverObject);
		return(STATUS_UNSUCCESSFUL);
	}
	//
	//  Check that we really can get interrupts
	//

	pHw->dwInterruptCount = 0;
	pHw->dwZRInterruptCount = 0;

	//    if (!HW_TestInterrupts(pDevInfo, pHw))
	//       {
	//       dprintf1(("[1.?] Interrupt Test failed"));
	//       VC_WriteProfile(pDevInfo, PARAM_FAIL, 1);
	//       VC_WriteProfile(pDevInfo, PARAM_ERROR, VC_ERR_INTERRUPT);
	//       VC_Cleanup(pDriverObject);
	//       return(STATUS_UNSUCCESSFUL);
	//       }
	//    else
	//       {
	Diag_Test++;
	dprintf1(("[1..] Interrupt Test passed"));
	VC_WriteProfile(pDevInfo, PARAM_DIAGS, 1);
	//       }
	//
	//  Disable Interrupts for now
	//
	HW_DisableInts(pDevInfo, pHw);

	dprintf1(("-----------  Diagnostics Complete -----------"));

	//
	//  Signal user-mode driver that we loaded ok
	//
	VC_WriteProfile(pDevInfo, PARAM_ERROR, VC_ERR_OK);
	dprintf1(("DriverEntry: Driver loaded"));
	return (STATUS_SUCCESS);
}


//
//  Initialise the hardware to a known state.
//
BOOLEAN HW_Init(PDEVICE_INFO pDevInfo)
{
	PHWINFO pHw;
	int sts;
	BYTE reset;

	pHw = VC_GetHWInfo(pDevInfo);

	//
	//  Read the hardware revision register.
	//
	sts = HW_RdByte(pDevInfo,MICO_ID_OFFSET,0);
	dprintf1(("MICO version 0x%x found - 0x22 = 58MHz, 0x30 = 59MHz",sts));
	pHw->mico_id = sts;
	VC_WriteProfile(pDevInfo, PARAM_VERSION, sts);
	VC_WriteProfile(pDevInfo, PARAM_SVERSION, DRVR_VERSION);
	dprintf1(("Setting initial Registry Parameter Key"));

	//
	//  Reset Video IF
	//
	reset = HW_ReadFromLatch(pDevInfo, RESET_OFFSET,0);
	reset = (BYTE)CLRBIT(reset, PARAM_RESET_VIF_BIT);
	HW_WriteToLatch(pDevInfo, RESET_OFFSET, reset,0);
	VC_Stall(100000);
	
	reset = (BYTE)SETBIT(reset, PARAM_RESET_VIF_BIT);
	HW_WriteToLatch(pDevInfo, RESET_OFFSET, reset,0);
	VC_Stall(100000);

	//
	//  Disable Watchdog, Relais off
	//
	HW_WrByte(pDevInfo,WATCHDOG_OFFSET,0x0,0);
	if (pHw->dwExtCardIO[1] != 0)
	   HW_WrByte(pDevInfo,RELAY_COVI_OFFSET,0,1);
	
	if (pHw->dwExtCardIO[2] != 0)
	   HW_WrByte(pDevInfo,RELAY_COVI_OFFSET,0,2);
	
	if (pHw->dwExtCardIO[3] != 0)
	   HW_WrByte(pDevInfo,RELAY_COVI_OFFSET,0,3);
	
	//
	//  Reset/Init Zoran Chipset
	//
	ZRXInit(pDevInfo);
	//
	//  Reset/Init i2c Bus controller
	//
	if (!PCD8584Init(pDevInfo))
	   return FALSE;
	//
	//  Reset/Init Frontend Chips
	//
	BT829Init(pDevInfo,MICO_FE0);
	BT829Init(pDevInfo,MICO_FE1);
	//
	//  Reset/Init Backend Chip
	//
	BT856Init(pDevInfo);

	return(TRUE);
}

//-----------------------------------------------------------------------------
// ---- Support functions -----
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Low level
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
VOID HW_WrByte(PDEVICE_INFO pDevInfo, WORD addr, UCHAR data, WORD Card)
{
	WORD IOAddr;
	PHWINFO pHw;

	pHw = VC_GetHWInfo(pDevInfo);

	if (Card != 0)
		IOAddr = (WORD)pHw->dwExtCardIO[Card];
	else
		IOAddr = (WORD)pDevInfo->PortBase;

	IOAddr += addr;
	WRITE_PORT_UCHAR((PUCHAR)IOAddr,data);
}

///////////////////////////////////////////////////////////////////////////////
VOID HW_WrShort(PDEVICE_INFO pDevInfo, WORD addr, WORD data, WORD Card)
{
	WORD IOAddr;
	PHWINFO pHw;

	pHw = VC_GetHWInfo(pDevInfo);

	if (Card != 0)
		IOAddr = (WORD)pHw->dwExtCardIO[Card];
	else
		IOAddr = (WORD)pDevInfo->PortBase;

	IOAddr += addr;
	WRITE_PORT_USHORT((PWORD)IOAddr,data);
}

///////////////////////////////////////////////////////////////////////////////
UCHAR HW_RdByte(PDEVICE_INFO pDevInfo, WORD addr, WORD Card)
{
	WORD IOAddr;
	PHWINFO pHw;

	pHw = VC_GetHWInfo(pDevInfo);

	if (Card != 0)
		IOAddr = (WORD)pHw->dwExtCardIO[Card];
	else
		IOAddr = (WORD)pDevInfo->PortBase;
	IOAddr += addr;

	return READ_PORT_UCHAR((PUCHAR)IOAddr);
}

///////////////////////////////////////////////////////////////////////////////
WORD HW_RdShort(PDEVICE_INFO pDevInfo, WORD addr, WORD Card)
{
	WORD IOAddr;
	PHWINFO pHw;

	pHw = VC_GetHWInfo(pDevInfo);

	if (Card != 0)
		IOAddr = (WORD)pHw->dwExtCardIO[Card];
	else
		IOAddr = (WORD)pDevInfo->PortBase;
	IOAddr += addr;

	return READ_PORT_USHORT((PWORD)IOAddr);
}

///////////////////////////////////////////////////////////////////////////////
BYTE HW_ReadFromLatch(PDEVICE_INFO pDevInfo, WORD port, WORD Card)
{
	PHWINFO pHw;
	BYTE sts;
	WORD IOBase;

	pHw = VC_GetHWInfo(pDevInfo);

	if (Card != 0)
		IOBase = (WORD)pHw->dwExtCardIO[Card];
	else
		IOBase = (WORD)pDevInfo->PortBase;

	if (port+IOBase > LATCH_SIZE)
	{
		dprintf1(("Port number larger then Latch Size: %d",port));
		sts = 0xff;
	}
	else
	{
		sts = Latchbuffer[port+IOBase];
	}

	return sts;
}

///////////////////////////////////////////////////////////////////////////////
BYTE HW_WriteToLatch(PDEVICE_INFO pDevInfo, WORD port, BYTE data, WORD Card)
{
	PHWINFO pHw;
	BYTE tmp;
	WORD IOBase;

	pHw = VC_GetHWInfo(pDevInfo);

	if (Card != 0)
		IOBase = (WORD)pHw->dwExtCardIO[Card];
	else
		IOBase = (WORD)pDevInfo->PortBase;

	if (port+IOBase > LATCH_SIZE)
	{
		dprintf1(("Port number larger then Latch Size: %d",port));
		return 0xff;
	}
	else
	{
		HW_WrByte(pDevInfo,port,data,Card);
		Latchbuffer[port+IOBase] = data;
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////  
BOOLEAN HW_WriteToI2C(PDEVICE_INFO pDevInfo, BYTE bSubAddress, BYTE bData,
                      BYTE bSlaveAddress)
{
    PCD8584WaitUntilBusReady(pDevInfo);
//
//  Write the slave address in register S0
//
    PCD8584Out8(pDevInfo, I2C_DATA_REG, bSlaveAddress);
    PCD8584Wait(100);
//
//  Generate a start in S1
//
    PCD8584Out8(pDevInfo, I2C_CONTROL_REG, I2C_SERIAL_BUS_ON | I2C_START_CMD | I2C_ACK_BIT);
    PCD8584WaitUntilPinFree(pDevInfo);
    PCD8584TestAck(pDevInfo);
//
//  Write the slave address in register S0
//
    PCD8584Out8(pDevInfo, I2C_DATA_REG, bSubAddress);
    PCD8584WaitUntilPinFree(pDevInfo);
    PCD8584TestAck(pDevInfo);
//
//  Write next value in S0
//
    PCD8584Out8(pDevInfo, I2C_DATA_REG, bData);
    PCD8584WaitUntilPinFree(pDevInfo);
    PCD8584TestAck(pDevInfo);
//
//  Send stop condition
//
    PCD8584Out8(pDevInfo, I2C_CONTROL_REG, I2C_PIN_BIT | I2C_SERIAL_BUS_ON | I2C_STOP_CMD | I2C_ACK_BIT);
    PCD8584Wait(100);

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
BYTE HW_ReadFromI2C(PDEVICE_INFO pDevInfo, BYTE bSubAddress, BYTE bSlaveAddress)
{
	UCHAR bData;

	PCD8584WaitUntilBusReady(pDevInfo);
	//
	//  Write the slave address in register S0
	//
	PCD8584Out8(pDevInfo, I2C_DATA_REG, bSlaveAddress);
	PCD8584Wait(100);
	//
	//  Generate a start in S1
	//
	PCD8584Out8(pDevInfo, I2C_CONTROL_REG, I2C_SERIAL_BUS_ON | I2C_START_CMD | I2C_ACK_BIT);
	PCD8584WaitUntilPinFree(pDevInfo);
	PCD8584TestAck(pDevInfo);
	//
	//  Write the subadress in register S0
	//
	PCD8584Out8(pDevInfo, I2C_DATA_REG, bSubAddress);
	PCD8584WaitUntilPinFree(pDevInfo);
	PCD8584TestAck(pDevInfo);
	//
	//  Send stop condition
	//
	PCD8584Out8(pDevInfo, I2C_CONTROL_REG, I2C_PIN_BIT | I2C_SERIAL_BUS_ON |    I2C_STOP_CMD | I2C_ACK_BIT);
	PCD8584Wait(100);
	//
	//  Write the slave address
	//
	PCD8584Out8(pDevInfo, I2C_DATA_REG, (BYTE)(bSlaveAddress+1));
	PCD8584Wait(100);
	//
	//  Generate a start in S1
	//
	PCD8584Out8(pDevInfo, I2C_CONTROL_REG, I2C_SERIAL_BUS_ON | I2C_START_CMD | I2C_ACK_BIT);
	PCD8584WaitUntilPinFree(pDevInfo);
	PCD8584TestAck(pDevInfo);

	bData = PCD8584In8(pDevInfo, I2C_DATA_REG);
	PCD8584WaitUntilPinFree(pDevInfo);
	PCD8584TestAck(pDevInfo);
	//
	//  Disable ACK + ES2=0
	//
	PCD8584Out8(pDevInfo, I2C_CONTROL_REG, I2C_SERIAL_BUS_ON);
	bData = PCD8584In8(pDevInfo, I2C_DATA_REG);
	PCD8584WaitUntilPinFree(pDevInfo);
	//
	//  Send stop condition
	//
	PCD8584Out8(pDevInfo, I2C_CONTROL_REG, I2C_PIN_BIT | I2C_SERIAL_BUS_ON |    I2C_STOP_CMD | I2C_ACK_BIT);
	PCD8584Wait(100);

	return bData;
}

// Map an IO Port
///////////////////////////////////////////////////////////////////////////////
BOOLEAN HW_MapPort(PDEVICE_INFO pDevInfo, WORD PortBase, DWORD NrOfPorts)
{
	PHYSICAL_ADDRESS PortAddress;
	PHYSICAL_ADDRESS MappedAddress;
	BOOLEAN status;
	NTSTATUS status_bus;

	pDevInfo->BusType = Isa;
	status_bus = VC_GetBus(pDevInfo->BusType, &pDevInfo->BusNumber);
	if (!NT_SUCCESS(status_bus))
	{
		pDevInfo->BusType = Eisa;
		status_bus = VC_GetBus(pDevInfo->BusType, &pDevInfo->BusNumber);
		if (!NT_SUCCESS(status_bus))
		{
			dprintf1(("HW_MapPort: Failed to determine Bus Number"));
			return FALSE;
		}
	}
	pDevInfo->PortMemType = 1;
	PortAddress.LowPart = (DWORD) PortBase;
	PortAddress.HighPart = 0;
	status = HalTranslateBusAddress(pDevInfo->BusType,
						pDevInfo->BusNumber,
						PortAddress,
						&pDevInfo->PortMemType,
						&MappedAddress);
	if (!status)
	{
		dprintf1(("HW_MapPort: Failed to translate BusAddress"));
		return FALSE;
	}

	if (pDevInfo->PortMemType == 0)
	{
		pDevInfo->PortBase = MmMapIoSpace(MappedAddress, NrOfPorts, FALSE);
		dprintf(("Mapped memory port at 0x%x",pDevInfo->PortBase));
	}
	else
	{
		pDevInfo->PortBase = (PUCHAR) MappedAddress.LowPart;
		dprintf(("Mapped IO port at 0x%x",pDevInfo->PortBase));
	}
	pDevInfo->NrOfPorts = NrOfPorts;
	return TRUE;
}

// Check that we can receive interrupts by requesting one and waiting for
// it to happen.
///////////////////////////////////////////////////////////////////////////////
BOOLEAN HW_TestInterrupts(PDEVICE_INFO pDevInfo, PHWINFO pHw)
{
	int i;
	WORD status;

	//
	//  Check that interrupts are occuring properly
	//   - enable interrupts and wait for one to happen.
	//  The interrupt ack routine will disable interrupts if
	//  bVideoIn is FALSE, but will first increment InterruptCount.
	//
	pHw->dwInterruptCount = 0;
	pHw->dwZRInterruptCount = 0;
	pHw->bVideoIn = FALSE;
	HW_EnableInts(pDevInfo,pHw);
	//
	//  Wait up to half a second for an interrupt - they should be
	//  every 1/25th sec or more frequently
	//

	for (i = 0; i < 10; i++)
	{
		if (pHw->dwInterruptCount > 0)
		{
			break;
		}
		VC_Delay(40);                   // Wait 1/25th sec
	}


	if (pHw->dwInterruptCount == 0)
	{
		HW_DisableInts(pDevInfo, pHw);
		dprintf1(("HW_TestInterrupts: Timeout waiting for interrupt"));
		return(FALSE);
	}
	return TRUE;
}

// Set IRQ Value
///////////////////////////////////////////////////////////////////////////////
VOID HW_SetIRQ(PDEVICE_INFO pDevInfo, PHWINFO pHw, DWORD Irq)
{
	UCHAR tmp;

	tmp = HW_RdByte(pDevInfo,MICO_IER_OFFSET,0);
	tmp &= 0xf8;
	HW_WrByte(pDevInfo,MICO_IER_OFFSET,(UCHAR)(tmp | irq_table[Irq]),0);
}


// Enable Interrupts
///////////////////////////////////////////////////////////////////////////////
VOID HW_EnableInts(PDEVICE_INFO pDevInfo, PHWINFO pHw)
{
	WORD tmp;

	tmp = HW_RdByte(pDevInfo,MICO_IER_OFFSET,0);
	tmp = SETBIT(tmp,ZR_IE_BIT);
	tmp = CLRBIT(tmp,VI_IE_BIT);	// Video IRQ sperren
	tmp = CLRBIT(tmp,VID_AES_BIT);
	HW_WrByte(pDevInfo,MICO_IER_OFFSET,(UCHAR)tmp,0);
}

// Disable Interrupts
///////////////////////////////////////////////////////////////////////////////
VOID HW_DisableInts(PDEVICE_INFO pDevInfo, PHWINFO pHw)
{
	WORD tmp;

	tmp = HW_RdByte(pDevInfo,MICO_IER_OFFSET,0);
	tmp = CLRBIT(tmp,VI_IE_BIT);
	tmp = CLRBIT(tmp,ZR_IE_BIT);
	HW_WrByte(pDevInfo,MICO_IER_OFFSET,(UCHAR)tmp,0);
}

// Disable Zoran Interrupts
///////////////////////////////////////////////////////////////////////////////
VOID HW_DisableZRInts(PDEVICE_INFO pDevInfo, PHWINFO pHw)
{
	WORD tmp;

	tmp = HW_RdByte(pDevInfo,MICO_IER_OFFSET,0);
	tmp = CLRBIT(tmp,ZR_IE_BIT);
	HW_WrByte(pDevInfo,MICO_IER_OFFSET,(UCHAR)tmp,0);
}

// Enable Zoran Interrupts
///////////////////////////////////////////////////////////////////////////////
VOID HW_EnableZRInts(PDEVICE_INFO pDevInfo, PHWINFO pHw)
{
	WORD tmp;

	tmp = HW_RdByte(pDevInfo,MICO_IER_OFFSET,0);
	tmp = SETBIT(tmp,ZR_IE_BIT);
	HW_WrByte(pDevInfo,MICO_IER_OFFSET,(UCHAR)tmp,0);
}

//-----------------------------------------------------------------------------
// I2C Controller Routines
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
VOID PCD8584Reset(PDEVICE_INFO pDevInfo)
{
	BYTE bReset;
	//
	//  I2C-Bus Controller reset
	//
	bReset = HW_ReadFromLatch(pDevInfo, RESET_OFFSET,0);
	bReset = (BYTE)CLRBIT(bReset, PARAM_RESET_PCF8584_BIT);
	HW_WriteToLatch(pDevInfo, RESET_OFFSET, bReset,0);
	VC_Stall(100000);

	bReset = (BYTE)SETBIT(bReset, PARAM_RESET_PCF8584_BIT);
	HW_WriteToLatch(pDevInfo, RESET_OFFSET, bReset,0);
	VC_Stall(100000);
}


///////////////////////////////////////////////////////////////////////////////
BOOLEAN PCD8584Init(PDEVICE_INFO pDevInfo)
{
	BYTE bErr;

	PCD8584Reset(pDevInfo);
	//
	//  This is the "fake" write needed b the 8584 after reset.
	//
	PCD8584Out8(pDevInfo, I2C_CONTROL_REG, 0x80);
	PCD8584Wait(100);
	//
	//  Program the 8584 slave address
	//
	PCD8584Out8(pDevInfo, I2C_OWN_ADDRESS_REG, 0x55);
	PCD8584Wait(100);
	//
	//  Setup for indirect write to clock control register S2
	//
	PCD8584Out8(pDevInfo, I2C_CONTROL_REG, I2C_ES1_BIT);
	PCD8584Wait(100);
	//
	//  Write our clock rates to the        S0 register(base)
	//
	PCD8584Out8(pDevInfo, I2C_DATA_REG, (BYTE)(I2C_SERIAL_CLOCK_90KHZ | I2C_CLOCK_6MHZ));
	PCD8584Wait(100);
	//
	//  Read back value of I2C clock, 5 bits
	//
	bErr = PCD8584In8(pDevInfo, I2C_DATA_REG) & 0x1F;
	PCD8584Wait(100);

	if (bErr != (BYTE)(I2C_SERIAL_CLOCK_90KHZ | I2C_CLOCK_6MHZ))
	{
		dprintf1(("Bad readback test of I2C Controller PDC8584 (%x) exp %x",(WORD)bErr,(I2C_SERIAL_CLOCK_90KHZ+I2C_CLOCK_6MHZ)));
		return FALSE;
	}
	//
	//  Enable I2C serial interface with acknowledge
	//
	PCD8584Out8(pDevInfo, I2C_CONTROL_REG,I2C_PIN_BIT |  I2C_SERIAL_BUS_ON | I2C_ACK_BIT);
	PCD8584Wait(100);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
BYTE PCD8584In8(PDEVICE_INFO pDevInfo, BYTE bRegister)
{
	BYTE bRet;

	bRet = (BYTE)HW_RdByte(pDevInfo,(UCHAR)(PCF8584_OFFSET+bRegister),0);
	return bRet;
}

///////////////////////////////////////////////////////////////////////////////
VOID PCD8584Out8(PDEVICE_INFO pDevInfo, BYTE bRegister, BYTE bData)
{
	HW_WrByte(pDevInfo,(UCHAR)(PCF8584_OFFSET+bRegister),(UCHAR)bData,0);
}

// Test the PIN bit of the PCD8584
///////////////////////////////////////////////////////////////////////////////
WORD PCD8584WaitUntilPinFree(PDEVICE_INFO pDevInfo)
{
	WORD wI;

	wI = 0;

	//  Test if the PIN bit is free
	while ((PCD8584In8(pDevInfo, I2C_CONTROL_REG) & I2C_PIN_BIT) != 0)
	{
		wI++;
		if (wI == 0xFFFF)
		{
			dprintf1(("PCD8584WaitUntilPinFree\tTimeOut"));
			break;
		}
	}
	return(wI);
}

//  Test if PCD8584 has the bus busy
///////////////////////////////////////////////////////////////////////////////
WORD PCD8584WaitUntilBusReady(PDEVICE_INFO pDevInfo)
{
	WORD wI;

	wI = 0;

	//  Test if the chip is free
	while ((PCD8584In8(pDevInfo, I2C_CONTROL_REG) & I2C_BB_N) == 0)
	{
		wI++;
		if (wI == 0xFFFF)
		{
			dprintf1(("PCD8584WaitUntilBusReady\tTimeOut"));
			break;
		}
	}
	return(wI);
}

///////////////////////////////////////////////////////////////////////////////
BOOLEAN PCD8584TestAck(PDEVICE_INFO pDevInfo)
{
	if (PCD8584In8(pDevInfo, I2C_CONTROL_REG) & I2C_LRB_BIT)
	{
		dprintf1(("PCD8584TestAck\tNO SLAVE ACK"));
		return FALSE;
	}
	else
		return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
VOID PCD8584Wait(int nCount)
{
	VC_Stall(nCount);
}


//-----------------------------------------------------------------------------
// BT856 Routines
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
VOID BT856Reset(PDEVICE_INFO pDevInfo)
{
	BYTE bReset;

	//  BT856 Controller reset
	bReset = HW_ReadFromLatch(pDevInfo,RESET_OFFSET,0);
	bReset = (BYTE)CLRBIT(bReset, PARAM_RESET_BT856_BIT);
	HW_WriteToLatch(pDevInfo, RESET_OFFSET, bReset,0);
	VC_Stall(100000);
	bReset = (BYTE)SETBIT(bReset, PARAM_RESET_BT856_BIT);
	HW_WriteToLatch(pDevInfo, RESET_OFFSET, bReset,0);
	VC_Stall(100000);
}

///////////////////////////////////////////////////////////////////////////////
BOOL BT856Init(PDEVICE_INFO pDevInfo)
{
	int i,j;
	PHWINFO pHw;

	pHw = VC_GetHWInfo(pDevInfo);

	BT856Reset(pDevInfo);

	for (i = 0; i <= BT856INDEX; i++)
	{
		if (pHw->mico_id == MHZ59)
			j = BT856_59MHZ[i];
		else
			j = BT856_58MHZ[i];

		if (pHw->mico_id == MHZ59)
			BT856Out(pDevInfo, BT856_59MHZ_REG[i], (BYTE)j);
		else
			BT856Out(pDevInfo, BT856_58MHZ_REG[i], (BYTE)j);
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
BOOL BT856Out(PDEVICE_INFO pDevInfo, BYTE bSubAddress, BYTE bData)
{
	BOOL bRet;

	if (bSubAddress > BT856_MAX_REG)
	{
		dprintf1(("BT856Out\tRegister number invalid"));
		bRet = FALSE;
	}

	bRet = HW_WriteToI2C(pDevInfo, bSubAddress, bData, BT856_SLAVE_ADR);

	return bRet;
}

///////////////////////////////////////////////////////////////////////////////
BYTE BT856In(PDEVICE_INFO pDevInfo, BYTE bSubAddress)
{
	BYTE bRet = 0;
	int i,j;
	PHWINFO pHw;

	pHw = VC_GetHWInfo(pDevInfo);

	if (bSubAddress == BT856_GETID)
		bRet = HW_ReadFromI2C(pDevInfo,bSubAddress,BT856_SLAVE_ADR);
	else
	{

		if (bSubAddress > BT856_MAX_REG)
		{
			dprintf1(("Registernummer invalid"));
			bRet = 0xff;
		}

		j = 0;
		for (i=0; i<BT856INDEX; i++)
		{
			if (pHw->mico_id == MHZ59)
			{
				if (bSubAddress == BT856_59MHZ_REG[i])
					j = i;
			}
			else
			{
				if (bSubAddress == BT856_58MHZ_REG[i])
					j = i;
			}
		}

		if (j==0)
			dprintf1(("Cannot find BT856 Register - setting junk"));

		if (pHw->mico_id == MHZ59)
			bRet = BT856_59MHZ[j];
		else
			bRet = BT856_58MHZ[j];
	}

	return bRet;
}

///////////////////////////////////////////////////////////////////////////////
WORD BT856SetBeVideoFormat(PDEVICE_INFO pDevInfo, WORD wBeVideoFormat)
{
	WORD wOldBeVideoFormat = BT856GetBeVideoFormat(pDevInfo);
	BYTE bControl;
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	bControl = BT856In(pDevInfo, BT856_MODE_CONTROL);

	switch (wBeVideoFormat)
	{
		case MICO_PAL_CCIR:
			bControl = (BYTE)SETBIT(bControl, 2);
			bControl = (BYTE)CLRBIT(bControl, 0);
			break;
		case MICO_NTSC_CCIR:
			bControl = (BYTE)CLRBIT(bControl, 2);
			bControl = (BYTE)CLRBIT(bControl, 0);
			break;
		case MICO_PAL_SQUARE:
			bControl = (BYTE)SETBIT(bControl, 2);
			bControl = (BYTE)SETBIT(bControl, 0);
			break;
		case MICO_NTSC_SQUARE:
			bControl = (BYTE)CLRBIT(bControl, 2);
			bControl = (BYTE)SETBIT(bControl, 0);
			break;
		default:
			return wOldBeVideoFormat;
	}

	BT856Out(pDevInfo, BT856_MODE_CONTROL, bControl);
	pHw->wBeVideoFormat = wBeVideoFormat;

	return wOldBeVideoFormat;
}

///////////////////////////////////////////////////////////////////////////////
WORD BT856GetBeVideoFormat(PDEVICE_INFO pDevInfo)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);
	return pHw->wBeVideoFormat;
}

//-----------------------------------------------------------------------------
// BT829 Routines
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
BOOL BT829Out(PDEVICE_INFO pDevInfo, BYTE bSubAddress, BYTE bData, DWORD FE_Number)
{
	BYTE bVal, bRet;

	bVal = HW_ReadFromLatch(pDevInfo, CSVINDN_OFFSET,0);
	if (FE_Number == MICO_FE0)
		bVal = (BYTE) CLRBIT(bVal, SELECT_I2C_BIT);
	else if (FE_Number == MICO_FE1)
		bVal = (BYTE) SETBIT(bVal,SELECT_I2C_BIT);

	HW_WriteToLatch(pDevInfo, CSVINDN_OFFSET, bVal,0);
	bRet = HW_WriteToI2C(pDevInfo, bSubAddress, bData, BT829_SLAVE_ADR);

	return bRet;
}

///////////////////////////////////////////////////////////////////////////////
BYTE BT829In(PDEVICE_INFO pDevInfo, BYTE bSubAddress, DWORD FE_Number)
{
	BYTE bVal,bRet;

	bVal = HW_ReadFromLatch(pDevInfo, CSVINDN_OFFSET,0);
	if (FE_Number == MICO_FE0)
		bVal = (BYTE) CLRBIT(bVal, SELECT_I2C_BIT);
	else if (FE_Number == MICO_FE1)
		bVal = (BYTE) SETBIT(bVal,SELECT_I2C_BIT);

	HW_WriteToLatch(pDevInfo, CSVINDN_OFFSET, bVal,0);
	bRet = HW_ReadFromI2C(pDevInfo, bSubAddress, BT829_SLAVE_ADR);

	return bRet;
}

///////////////////////////////////////////////////////////////////////////////
VOID BT829Reset(PDEVICE_INFO pDevInfo)
{
	BYTE bReset;

	//  BT829 Controller reset
	bReset = HW_ReadFromLatch(pDevInfo, RESET_OFFSET,0);
	bReset = (BYTE)CLRBIT(bReset, PARAM_RESET_BT829_BIT);
	HW_WriteToLatch(pDevInfo, RESET_OFFSET, bReset,0);
	VC_Stall(100000);

	bReset = (BYTE)SETBIT(bReset, PARAM_RESET_BT829_BIT);
	HW_WriteToLatch(pDevInfo, RESET_OFFSET, bReset,0);
	VC_Stall(100000);
}

///////////////////////////////////////////////////////////////////////////////
VOID BT829Init(PDEVICE_INFO pDevInfo, DWORD FE_Number)
{
	int i,j;
	PHWINFO pHw;

	pHw = VC_GetHWInfo(pDevInfo);

	if (FE_Number == MICO_FE0)
	{
		dprintf(("Resetting BT829 chips"));
		BT829Reset(pDevInfo);
	}

	// NTSC (640x480), PAL (768x576), NTSC (720x480), PAL (720x576)
	for (i=0; i < BT829INDEX; i++)
	{
		for (j = MICO_NTSC_SQUARE; j <= MICO_PAL_CCIR; j++)
		{
			if (j == MICO_PAL_CCIR)
			{
				if (pHw->mico_id == MHZ59)
					BT829Out(pDevInfo, BT829_59MHZ_REG[i], BT829_59MHZ[i][j], FE_Number);
				else
					BT829Out(pDevInfo, BT829_58MHZ_REG[i], BT829_58MHZ[i][j], FE_Number);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
BOOL BT829SetFeVideoFormat(PDEVICE_INFO pDevInfo, WORD wFeVideoFormat, DWORD FE_Number)
{
	BYTE bMode;
	PHWINFO pHw;

	pHw = VC_GetHWInfo(pDevInfo);

	switch (wFeVideoFormat)
	{
		case MICO_NTSC_SQUARE:
			bMode = 0;
			break;

		case MICO_PAL_SQUARE:
			bMode = 1;
			break;

		case MICO_NTSC_CCIR:
			bMode = 2;
			break;

		case MICO_PAL_CCIR:
			bMode = 3;
			break;
		default:
			return FALSE;
	}

	if (pHw->mico_id == MHZ59)
	{
		BT829Out(pDevInfo, BT829_IFORM,     BT829_59MHZ[BT829_IFORM][bMode], FE_Number);
		BT829Out(pDevInfo, BT829_CROP,      BT829_59MHZ[BT829_CROP][bMode], FE_Number);
		BT829Out(pDevInfo, BT829_VACTIVE_LO,BT829_59MHZ[BT829_VACTIVE_LO][bMode], FE_Number);
		BT829Out(pDevInfo, BT829_HDELAY_LO, BT829_59MHZ[BT829_HDELAY_LO][bMode], FE_Number);
		BT829Out(pDevInfo, BT829_HACTIVE_LO,BT829_59MHZ[BT829_HACTIVE_LO][bMode], FE_Number);
		BT829Out(pDevInfo, BT829_ADELAY,    BT829_59MHZ[BT829_ADELAY][bMode], FE_Number);
		BT829Out(pDevInfo, BT829_BDELAY,    BT829_59MHZ[BT829_BDELAY][bMode], FE_Number);
		BT829Out(pDevInfo, BT829_HSCALE_LO, BT829_59MHZ[BT829_HSCALE_LO][bMode], FE_Number);
		BT829Out(pDevInfo, BT829_HSCALE_HI, BT829_59MHZ[BT829_HSCALE_HI][bMode], FE_Number);
		BT829Out(pDevInfo, BT829_VSCALE_LO, BT829_59MHZ[BT829_VSCALE_LO][bMode], FE_Number);
		BT829Out(pDevInfo, BT829_VSCALE_HI, BT829_59MHZ[BT829_VSCALE_HI][bMode], FE_Number);
	}
	else
	{
		BT829Out(pDevInfo, BT829_IFORM,     BT829_58MHZ[BT829_IFORM][bMode], FE_Number);
		BT829Out(pDevInfo, BT829_CROP,      BT829_58MHZ[BT829_CROP][bMode], FE_Number);
		BT829Out(pDevInfo, BT829_VACTIVE_LO,BT829_58MHZ[BT829_VACTIVE_LO][bMode], FE_Number);
		BT829Out(pDevInfo, BT829_HDELAY_LO, BT829_58MHZ[BT829_HDELAY_LO][bMode], FE_Number);
		BT829Out(pDevInfo, BT829_HACTIVE_LO,BT829_58MHZ[BT829_HACTIVE_LO][bMode], FE_Number);
		BT829Out(pDevInfo, BT829_ADELAY,    BT829_58MHZ[BT829_ADELAY][bMode], FE_Number);
		BT829Out(pDevInfo, BT829_BDELAY,    BT829_58MHZ[BT829_BDELAY][bMode], FE_Number);
		BT829Out(pDevInfo, BT829_HSCALE_LO, BT829_58MHZ[BT829_HSCALE_LO][bMode], FE_Number);
		BT829Out(pDevInfo, BT829_HSCALE_HI, BT829_58MHZ[BT829_HSCALE_HI][bMode], FE_Number);
		BT829Out(pDevInfo, BT829_VSCALE_LO, BT829_58MHZ[BT829_VSCALE_LO][bMode], FE_Number);
		BT829Out(pDevInfo, BT829_VSCALE_HI, BT829_58MHZ[BT829_VSCALE_HI][bMode], FE_Number);
	}

	pHw->wBTFeVideoFormat = wFeVideoFormat;
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
WORD BT829GetFeVideoFormat(PDEVICE_INFO pDevInfo)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);
	return pHw->wBTFeVideoFormat;
}

///////////////////////////////////////////////////////////////////////////////
BOOL BT829SetFeSourceType(PDEVICE_INFO pDevInfo, WORD wFeSourceType, WORD wFeFilter, DWORD FE_Number)
{
	BYTE bControl;
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	bControl = BT829In(pDevInfo, BT829_CONTROL, FE_Number);

	switch (wFeSourceType)
	{
		case MICO_FBAS:
			bControl = (BYTE)CLRBIT(bControl, CONTROL_COMP_BIT);
			bControl = (BYTE)SETBIT(bControl, CONTROL_LDEC_BIT);
			if ((wFeFilter == MICO_NOTCH_FILTER_AUTO) || (wFeFilter == MICO_NOTCH_FILTER_ON))
				bControl = (BYTE)CLRBIT(bControl, CONTROL_LNOTCH_BIT);
			else
				bControl = (BYTE)SETBIT(bControl, CONTROL_LNOTCH_BIT);
			break;

		case MICO_SVHS:
			bControl = (BYTE)SETBIT(bControl, CONTROL_COMP_BIT);
			bControl = (BYTE)SETBIT(bControl, CONTROL_LDEC_BIT);
			if ((wFeFilter == MICO_NOTCH_FILTER_AUTO) || (wFeFilter == MICO_NOTCH_FILTER_OFF))
				bControl = (BYTE)SETBIT(bControl, CONTROL_LNOTCH_BIT);
			else
				bControl = (BYTE)CLRBIT(bControl, CONTROL_LNOTCH_BIT);
			break;

		default:
			return FALSE;
	}

	BT829Out(pDevInfo, BT829_CONTROL, bControl, FE_Number);
	pHw->wFeSourceType = wFeSourceType;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
WORD BT829GetFeSourceType(PDEVICE_INFO pDevInfo)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	return pHw->wFeSourceType;
}

///////////////////////////////////////////////////////////////////////////////
BOOL BT829SetFeBrightness(PDEVICE_INFO pDevInfo, WORD wFeBrightness, DWORD FE_Number)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	BT829Out(pDevInfo, BT829_BRIGHT, (BYTE)((short int)wFeBrightness - 128), FE_Number);
	pHw->wFeBrightness = wFeBrightness;
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
WORD BT829GetFeBrightness(PDEVICE_INFO pDevInfo)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);
	return pHw->wFeBrightness;
}

///////////////////////////////////////////////////////////////////////////////
BOOL BT829SetFeContrast(PDEVICE_INFO pDevInfo, WORD wFeContrast, DWORD FE_Number)
{
	BYTE bControl;
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	bControl = BT829In(pDevInfo, BT829_CONTROL, FE_Number);
	BT829Out(pDevInfo, BT829_CONTRAST_LO, (BYTE)wFeContrast, FE_Number);

	if (wFeContrast > 0xff)
		bControl = (BYTE)SETBIT(bControl, CONTROL_CON_MSB_BIT);
	else
		bControl = (BYTE)CLRBIT(bControl, CONTROL_CON_MSB_BIT);

	BT829Out(pDevInfo, BT829_CONTROL, bControl, FE_Number);
	pHw->wFeContrast = wFeContrast;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
WORD BT829GetFeContrast(PDEVICE_INFO pDevInfo)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);
	return pHw->wFeContrast;
}

///////////////////////////////////////////////////////////////////////////////
BOOL BT829SetFeSaturation(PDEVICE_INFO pDevInfo, WORD wFeSaturation, DWORD FE_Number)
{
	BYTE  bControl;
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);


	bControl = BT829In(pDevInfo, BT829_CONTROL, FE_Number);

	BT829Out(pDevInfo, BT829_SAT_U_LO, (BYTE)wFeSaturation, FE_Number);
	BT829Out(pDevInfo, BT829_SAT_V_LO, (BYTE)wFeSaturation, FE_Number);

	if (wFeSaturation > 0xff)
	{
		bControl = (BYTE)SETBIT(bControl, CONTROL_SAT_V_MSB_BIT);
		bControl = (BYTE)SETBIT(bControl, CONTROL_SAT_U_MSB_BIT);
	}
	else
	{
		bControl = (BYTE)CLRBIT(bControl, CONTROL_SAT_V_MSB_BIT);
		bControl = (BYTE)CLRBIT(bControl, CONTROL_SAT_U_MSB_BIT);
	}

	BT829Out(pDevInfo, BT829_CONTROL, bControl, FE_Number);
	pHw->wFeSaturation = wFeSaturation;

	return TRUE;
}

//
// *JW* Modified to include HLoc
//
///////////////////////////////////////////////////////////////////////////////
BOOL BT829IsVideoPresent(PDEVICE_INFO pDevInfo, DWORD FE_Number)
{
	DWORD Pres = 0;
	DWORD Hloc = 0;
	int i;
	//
	// OLD Code
	//
	// Pres = (DWORD)TSTBIT(BT829In(pDevInfo, BT829_STATUS, FE_Number), STATUS_PRES_BIT);
	// if (Pres != 0)
	//    return TRUE;
	// else
	//    return FALSE;
	// 	  
	i = 100;
	while (i > 0)
	{
		Pres = (DWORD)TSTBIT(BT829In(pDevInfo, BT829_STATUS, FE_Number), STATUS_PRES_BIT);
		Hloc = (DWORD)TSTBIT(BT829In(pDevInfo, BT829_STATUS, FE_Number), STATUS_HLOC_BIT);

		if ((Pres > 0) && (Hloc > 0))
			return TRUE;
		else
		{
			VC_Stall(5);
			i--;
		}
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
WORD BT829GetFeSaturation(PDEVICE_INFO pDevInfo)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);
	return pHw->wFeSaturation;
}

///////////////////////////////////////////////////////////////////////////////
BOOL BT829SetFeHue(PDEVICE_INFO pDevInfo, WORD wFeHue, DWORD FE_Number)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	BT829Out(pDevInfo, BT829_HUE, (BYTE)((short int)wFeHue - 128), FE_Number);
	pHw->wFeHue = wFeHue;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
WORD BT829GetFeHue(PDEVICE_INFO pDevInfo)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	return pHw->wFeHue;
}

///////////////////////////////////////////////////////////////////////////////
WORD BT829SetFeHScale(PDEVICE_INFO pDevInfo, WORD wFeHScale, DWORD FE_Number)
{
	WORD wOldFeHScale = BT829GetFeHScale(pDevInfo);
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	BT829Out(pDevInfo, BT829_HSCALE_LO, LOBYTE(wFeHScale), FE_Number);
	BT829Out(pDevInfo, BT829_HSCALE_HI, HIBYTE(wFeHScale), FE_Number);
	pHw->wFeHScale = wFeHScale;

	return wOldFeHScale;
}

///////////////////////////////////////////////////////////////////////////////
WORD BT829GetFeHScale(PDEVICE_INFO pDevInfo)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	return pHw->wFeHScale;
}

///////////////////////////////////////////////////////////////////////////////
WORD BT829SetFeVScale(PDEVICE_INFO pDevInfo, WORD wFeVScale, DWORD FE_Number)
{
	WORD wOldFeVScale = BT829GetFeVScale(pDevInfo);
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	BT829Out(pDevInfo, BT829_VSCALE_LO, LOBYTE(wFeVScale), FE_Number);
	BT829Out(pDevInfo, BT829_VSCALE_HI, HIBYTE(wFeVScale), FE_Number);
	pHw->wFeVScale = wFeVScale;

	return wOldFeVScale;
}

///////////////////////////////////////////////////////////////////////////////
WORD BT829GetFeVScale(PDEVICE_INFO pDevInfo)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	return pHw->wFeVScale;
}

///////////////////////////////////////////////////////////////////////////////
WORD BT829SetFeFilter(PDEVICE_INFO pDevInfo, WORD wFeFilter, DWORD FE_Number)
{
	WORD wOldFeFilter = wFeFilter;
	BYTE bControl;
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	bControl = BT829In(pDevInfo, BT829_CONTROL, FE_Number);

	if (wFeFilter & MICO_LNOTCH)
		bControl = (BYTE)SETBIT(bControl, CONTROL_LNOTCH_BIT);
	else
		bControl = (BYTE)CLRBIT(bControl, CONTROL_LNOTCH_BIT);

	if (wFeFilter & MICO_LDEC)
		bControl = (BYTE)SETBIT(bControl, CONTROL_LDEC_BIT);
	else
		bControl = (BYTE)CLRBIT(bControl, CONTROL_LDEC_BIT);

	BT829Out(pDevInfo, BT829_CONTROL, bControl, FE_Number);
	pHw->wFeFilter = wFeFilter;

	return wOldFeFilter;
}

///////////////////////////////////////////////////////////////////////////////
WORD BT829GetFeFilter(PDEVICE_INFO pDevInfo)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	return pHw->wFeFilter;
}

//-----------------------------------------------------------------------------
// Zoran Routines
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
BOOL ZRXReset(PDEVICE_INFO pDevInfo)
{
	BYTE bReset;

	bReset = HW_ReadFromLatch(pDevInfo,RESET_OFFSET,0);
	bReset = (BYTE)CLRBIT(bReset, PARAM_RESET_ZR);
	HW_WriteToLatch(pDevInfo, RESET_OFFSET, bReset,0);
	VC_Stall(100000);

	bReset = (BYTE)SETBIT(bReset, PARAM_RESET_ZR);
	HW_WriteToLatch(pDevInfo, RESET_OFFSET, bReset,0);
	VC_Stall(100000);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
BOOL ZRXInit(PDEVICE_INFO pDevInfo)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	pHw->nEncoderState    = MICO_ENCODER_OFF;
	pHw->nDecoderState    = MICO_DECODER_OFF;
	pHw->wFormat = MICO_ENCODER_HIGH;
	pHw->wFramerate = 12;
	pHw->dwBPF    = 20000;
	pHw->wYBackgnd = 0x7ff;
	pHw->wUVBackgnd = 0x7ff;
	pHw->wZRFeVideoFormat = MICO_PAL_CCIR;

	pHw->wFeHStart[MICO_NTSC_SQUARE] = 0;
	pHw->wFeHEnd[MICO_NTSC_SQUARE] = 0;
	pHw->wFeHTotal[MICO_NTSC_SQUARE] = 0;
	pHw->wFeHSStart[MICO_NTSC_SQUARE] = 0;
	pHw->wFeHSEnd[MICO_NTSC_SQUARE] = 0;
	pHw->wFeBlankStart[MICO_NTSC_SQUARE] = 0;
	pHw->wFeVStart[MICO_NTSC_SQUARE] = 0;
	pHw->wFeVEnd[MICO_NTSC_SQUARE] = 0;
	pHw->wFeVTotal[MICO_NTSC_SQUARE] = 0;

	pHw->wFeHStart[MICO_PAL_SQUARE] = 0;
	pHw->wFeHEnd[MICO_PAL_SQUARE] = 0;
	pHw->wFeHTotal[MICO_PAL_SQUARE] = 0;
	pHw->wFeHSStart[MICO_PAL_SQUARE] = 0;
	pHw->wFeHSEnd[MICO_PAL_SQUARE] = 0;
	pHw->wFeBlankStart[MICO_PAL_SQUARE] = 0;
	pHw->wFeVStart[MICO_PAL_SQUARE] = 0;
	pHw->wFeVEnd[MICO_PAL_SQUARE] = 0;
	pHw->wFeVTotal[MICO_PAL_SQUARE] = 0;
	pHw->wFeNMCU[MICO_PAL_SQUARE] = 0;

	pHw->wFeHStart[MICO_NTSC_CCIR] = 0;
	pHw->wFeHEnd[MICO_NTSC_CCIR] = 0;
	pHw->wFeHTotal[MICO_NTSC_CCIR] = 0;
	pHw->wFeHSStart[MICO_NTSC_CCIR] = 0;
	pHw->wFeHSEnd[MICO_NTSC_CCIR] = 0;
	pHw->wFeBlankStart[MICO_NTSC_CCIR]    = 0;
	pHw->wFeVStart[MICO_NTSC_CCIR] = 0;
	pHw->wFeVEnd[MICO_NTSC_CCIR] = 0;
	pHw->wFeVTotal[MICO_NTSC_CCIR] = 0;
	pHw->wFeNMCU[MICO_NTSC_CCIR] = 0;

	pHw->wFeHStart[MICO_PAL_CCIR] = 0;
	pHw->wFeHEnd[MICO_PAL_CCIR] = 0;
	pHw->wFeHTotal[MICO_PAL_CCIR] = 0;
	pHw->wFeHSStart[MICO_PAL_CCIR] = 0;
	pHw->wFeHSEnd[MICO_PAL_CCIR] = 0;
	pHw->wFeBlankStart[MICO_PAL_CCIR] = 0;
	pHw->wFeVStart[MICO_PAL_CCIR] = 0;
	pHw->wFeVEnd[MICO_PAL_CCIR] = 0;
	pHw->wFeVTotal[MICO_PAL_CCIR] = 0;
	pHw->wFeNMCU[MICO_PAL_CCIR] = 0;

	//
	//  Make Backend and Frontend config equal
	//
	pHw->wBeVideoFormat= pHw->wZRFeVideoFormat;
	pHw->wBeHStart = pHw->wFeHStart[pHw->wBeVideoFormat];
	pHw->wBeHEnd = pHw->wFeHEnd[pHw->wBeVideoFormat];
	pHw->wBeHTotal = pHw->wFeHTotal[pHw->wBeVideoFormat];
	pHw->wBeHSStart = pHw->wFeHSStart[pHw->wBeVideoFormat];
	pHw->wBeHSEnd = pHw->wFeHSEnd[pHw->wBeVideoFormat];
	pHw->wBeBlankStart = pHw->wFeBlankStart[pHw->wBeVideoFormat];
	pHw->wBeVStart = pHw->wFeVStart[pHw->wBeVideoFormat];
	pHw->wBeVEnd = pHw->wFeVEnd[pHw->wBeVideoFormat];
	pHw->wBeVTotal = pHw->wFeVTotal[pHw->wBeVideoFormat];
	pHw->wBeNMCU = pHw->wFeNMCU[pHw->wBeVideoFormat];

	pHw->dwTCVData = 20000*8;
	pHw->dwTCVNet = JPEG_BUFFER_SIZE*8-15;
	pHw->wSF = 1*256;

	pHw->wFeHStart[MICO_NTSC_CCIR] = (WORD) VC_ReadProfile(pDevInfo,PARAM_T58NC_FeHStart,8);
	pHw->wFeHEnd[MICO_NTSC_CCIR] = (WORD) VC_ReadProfile(pDevInfo,PARAM_T58NC_FeHEnd,712);
	pHw->wFeHTotal[MICO_NTSC_CCIR] = (WORD) VC_ReadProfile(pDevInfo,PARAM_T58NC_FeHTotal,858);
	pHw->wFeHSStart[MICO_NTSC_CCIR] = (WORD) VC_ReadProfile(pDevInfo,PARAM_T58NC_FeHSStart,732);
	pHw->wFeHSEnd[MICO_NTSC_CCIR] = (WORD) VC_ReadProfile(pDevInfo,PARAM_T58NC_FeHSEnd ,784);
	pHw->wFeBlankStart[MICO_NTSC_CCIR] = (WORD) VC_ReadProfile(pDevInfo,PARAM_T58NC_FeBlankStart,720);
	pHw->wFeVStart[MICO_NTSC_CCIR] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T58NC_FeVStart,14);
	pHw->wFeVEnd[MICO_NTSC_CCIR] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T58NC_FeVEnd,254);
	pHw->wFeVTotal[MICO_NTSC_CCIR] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T58NC_FeVTotal,525);
	pHw->wFeNMCU[MICO_NTSC_CCIR] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T58NC_FeNMCU,43);

	pHw->wFeHStart[MICO_PAL_CCIR] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T58PC_FeHStart, 8);
	pHw->wFeHEnd[MICO_PAL_CCIR] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T58PC_FeHEnd, 712);
	pHw->wFeHTotal[MICO_PAL_CCIR] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T58PC_FeHTotal, 864);
	pHw->wFeHSStart[MICO_PAL_CCIR] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T58PC_FeHSStart, 732);
	pHw->wFeHSEnd[MICO_PAL_CCIR] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T58PC_FeHSEnd, 784);
	pHw->wFeBlankStart[MICO_PAL_CCIR] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T58PC_FeBlankStart, 720);
	pHw->wFeVStart[MICO_PAL_CCIR] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T58PC_FeVStart, 16);
	pHw->wFeVEnd[MICO_PAL_CCIR] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T58PC_FeVEnd, 304);
	pHw->wFeVTotal[MICO_PAL_CCIR] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T58PC_FeVTotal, 625);
	pHw->wFeNMCU[MICO_PAL_CCIR]= (WORD) VC_ReadProfile(pDevInfo, PARAM_T58PC_FeNMCU, 43);

	//
	//  58 MHz Quarz
	//
	if (pHw->mico_id == 0x22)
	{
		pHw->wFeHStart[MICO_NTSC_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T58NC_FeHStart, 0);
		pHw->wFeHEnd[MICO_NTSC_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T58NC_FeHEnd, 640);
		pHw->wFeHTotal[MICO_NTSC_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T58NC_FeHTotal, 780);
		pHw->wFeHSStart[MICO_NTSC_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T58NC_FeHSStart, 657);
		pHw->wFeHSEnd[MICO_NTSC_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T58NC_FeHSEnd, 697);
		pHw->wFeBlankStart[MICO_NTSC_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T58NC_FeBlankStart, 640);
		pHw->wFeVStart[MICO_NTSC_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T58NC_FeVStart, 14);
		pHw->wFeVEnd[MICO_NTSC_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T58NC_FeVEnd, 254);
		pHw->wFeVTotal[MICO_NTSC_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T58NC_FeVTotal, 525);

		pHw->wFeHStart[MICO_PAL_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T58PS_FeHStart, 0);
		pHw->wFeHEnd[MICO_PAL_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T58PS_FeHEnd, 768);
		pHw->wFeHTotal[MICO_PAL_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T58PS_FeHTotal, 944);
		pHw->wFeHSStart[MICO_PAL_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T58PS_FeHSStart, 791);
		pHw->wFeHSEnd[MICO_PAL_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T58PS_FeHSEnd, 837);
		pHw->wFeBlankStart[MICO_PAL_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T58PS_FeBlankStart, 768);
		pHw->wFeVStart[MICO_PAL_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T58PS_FeVStart, 18);
		pHw->wFeVEnd[MICO_PAL_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T58PS_FeVEnd, 306);
		pHw->wFeVTotal[MICO_PAL_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T58PS_FeVTotal, 625);
		pHw->wFeNMCU[MICO_PAL_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T58PS_FeNMCU, 47);
	}
	//
	// 59 MHz Quarz
	//
	else if (pHw->mico_id == 0x30)
	{
		pHw->wFeHStart[MICO_NTSC_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T59NS_FeHStart, 0);
		pHw->wFeHEnd[MICO_NTSC_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T59NS_FeHEnd, 640);
		pHw->wFeHTotal[MICO_NTSC_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T59NS_FeHTotal, 780);
		pHw->wFeHSStart[MICO_NTSC_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T59NS_FeHSStart, 657);
		pHw->wFeHSEnd[MICO_NTSC_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T59NS_FeHSEnd, 697);
		pHw->wFeBlankStart[MICO_NTSC_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T59NS_FeBlankStart, 640);
		pHw->wFeVStart[MICO_NTSC_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T59NS_FeVStart, 14);
		pHw->wFeVEnd[MICO_NTSC_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T59NS_FeVEnd, 254);
		pHw->wFeVTotal[MICO_NTSC_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T59NS_FeVTotal, 525);

		pHw->wFeHStart[MICO_PAL_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T59PS_FeHStart, 0);
		pHw->wFeHEnd[MICO_PAL_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T59PS_FeHEnd, 768);
		pHw->wFeHTotal[MICO_PAL_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T59PS_FeHTotal, 944);
		pHw->wFeHSStart[MICO_PAL_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T59PS_FeHSStart, 791);
		pHw->wFeHSEnd[MICO_PAL_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T59PS_FeHSEnd, 837);
		pHw->wFeBlankStart[MICO_PAL_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T59PS_FeBlankStart, 768);
		pHw->wFeVStart[MICO_PAL_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T59PS_FeVStart, 18);
		pHw->wFeVEnd[MICO_PAL_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T59PS_FeVEnd, 306);
		pHw->wFeVTotal[MICO_PAL_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T59PS_FeVTotal, 625);
		pHw->wFeNMCU[MICO_PAL_SQUARE] = (WORD) VC_ReadProfile(pDevInfo, PARAM_T59PS_FeNMCU, 47);
	}

	//
	//  Make backend and Frontend config the same
	//
	pHw->wBeVideoFormat= pHw->wZRFeVideoFormat;
	pHw->wBeHStart = pHw->wFeHStart[pHw->wBeVideoFormat];
	pHw->wBeHEnd = pHw->wFeHEnd[pHw->wBeVideoFormat];
	pHw->wBeHTotal = pHw->wFeHTotal[pHw->wBeVideoFormat];
	pHw->wBeHSStart = pHw->wFeHSStart[pHw->wBeVideoFormat];
	pHw->wBeHSEnd = pHw->wFeHSEnd[pHw->wBeVideoFormat];
	pHw->wBeBlankStart = pHw->wFeBlankStart[pHw->wBeVideoFormat];
	pHw->wBeVStart = pHw->wFeVStart[pHw->wBeVideoFormat];
	pHw->wBeVEnd = pHw->wFeVEnd[pHw->wBeVideoFormat];
	pHw->wBeVTotal = pHw->wFeVTotal[pHw->wBeVideoFormat];
	pHw->wBeNMCU = pHw->wFeNMCU[pHw->wBeVideoFormat];
	//
	//  Zoran reset
	//
	ZRXReset(pDevInfo);

	dprintf1(("IRQ Register (0x18) = 0x%x",HW_RdByte(pDevInfo,0x18,0)));

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void ZRSetInputSource(PDEVICE_INFO pDevInfo, WORD wFrontEnd, WORD wExtCard, WORD wSource, BYTE byYPort, BYTE byCPort, DWORD dwProzessID)
{
	BYTE	byCSVDIN;
	BYTE	byCSVIAN;
	BOOL	bChanges = FALSE;
	WORD	wCnt = 0;
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	// Gewünschte Videoquelle auswählen
	byCSVIAN = ZRChipSelectVideoInAnalogIn(pDevInfo, wFrontEnd, wExtCard) & 0xc0;
	byCSVIAN = byCSVIAN | byYPort << 3 | byCPort;		
	ZRChipSelectVideoInAnalogOut(pDevInfo, wFrontEnd, wExtCard, byCSVIAN);

	// Auf gewünschten CoVi schalten
	byCSVIAN = ZRChipSelectVideoInAnalogIn(pDevInfo, wFrontEnd, MICO_EXTCARD0) & 0x3f;
	byCSVIAN = byCSVIAN | wExtCard << 6; 					
	ZRChipSelectVideoInAnalogOut(pDevInfo, wFrontEnd, MICO_EXTCARD0, byCSVIAN);

	// Das Frontend wählen
	byCSVDIN = ZRChipSelectVideoInDigitalIn(pDevInfo);
	if (wFrontEnd == MICO_FE0)
		byCSVDIN = CLRBIT(byCSVDIN, SELECT_VD_BIT);
	else if (wFrontEnd == MICO_FE1)
		byCSVDIN = SETBIT(byCSVDIN, SELECT_VD_BIT);

	if (ZRGetEncoderState(pDevInfo) == MICO_ENCODER_ON)
	{
		// Synchron auf das jeweils andere Frontend umschalten
		pHw->dwReqProzessID	= dwProzessID;	// Prozeßzuordnung
		pHw->wReqSource		= wSource;		// Gewünschter Videoeingang
		pHw->wReqExtCard	= wExtCard;		// Gewünschtes Board
		pHw->wCSVDIN		= byCSVDIN;		// ChipSelectVideoInDigital
		pHw->bSwitch		= TRUE;			// Und Umschalten.

		// Warte bis der MiCo.vxd die Quelle umgeschaltet hat.
		while(pHw->bSwitch)
		{
			if ((wCnt++) > 2000)
			{
				ZRChipSelectVideoInDigitalOut(pDevInfo, byCSVDIN);
				pHw->dwProzessID	= dwProzessID;
				pHw->wSource		= wSource;
				pHw->wExtCard		= wExtCard;
				pHw->bSwitch		= FALSE;
				return;
			}
			VC_Stall(500);
		}
	}
	else
	{
		// Es wird nicht im Interrupt umgeschaltet -> Globale Parameter
		// müssen deshalb an dieser Stelle manuell gesetzt werden.
		pHw->dwReqProzessID	= dwProzessID;
		pHw->wReqSource		= wSource;
		pHw->wReqExtCard	= wExtCard;
		pHw->dwProzessID	= dwProzessID;
		pHw->wSource		= wSource;
		pHw->wExtCard		= wExtCard;

		// Dieser Aufruf bewirkt das manuelle digitale Umschalten, wenn
		// der Encoder nicht läuft.
		ZRChipSelectVideoInDigitalOut(pDevInfo, byCSVDIN);
	}
}

///////////////////////////////////////////////////////////////////////////////
VOID ZRSetFeVideoFormat(PDEVICE_INFO pDevInfo, WORD wFormat)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	switch (wFormat)
	{
		case MICO_PAL_SQUARE:
			break;

		case MICO_NTSC_SQUARE:
			break;

		case MICO_PAL_CCIR:
			break;
		
		case MICO_NTSC_CCIR:
			break;

		default:
			dprintf1(("ZRSetFeVideoFormat - invalid format"));
			return;
	}

	pHw->wZRFeVideoFormat = wFormat;

	if (ZRGetEncoderState(pDevInfo) == MICO_ENCODER_ON)
	{
		ZRRestartEncoding(pDevInfo);
	}
}

///////////////////////////////////////////////////////////////////////////////
VOID ZRSetBeVideoFormat(PDEVICE_INFO pDevInfo, WORD wFormat)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	switch (wFormat)
	{
		case MICO_PAL_SQUARE:
			break;

		case MICO_NTSC_SQUARE:
			break;

		case MICO_PAL_CCIR:
			break;

		case MICO_NTSC_CCIR:
			break;

		default:
			dprintf1(("ZRSetBeVideoFormat - invalid format"));
			return;
	}

	pHw->wBeHTotal	= pHw->wFeHTotal[wFormat];
	pHw->wBeVTotal	= pHw->wFeVTotal[wFormat];
	pHw->wBeBlankStart= pHw->wFeBlankStart[wFormat];
	pHw->wBeHStart	= pHw->wFeHStart[wFormat];
	pHw->wBeHEnd		= pHw->wFeHEnd[wFormat];
	pHw->wBeHSStart	= pHw->wFeHSStart[wFormat];
	pHw->wBeHSEnd		= pHw->wFeHSEnd[wFormat];
	pHw->wBeVStart	= pHw->wFeVStart[wFormat];
	pHw->wBeVEnd		= pHw->wFeVEnd[wFormat];

	pHw->wBeVideoFormat = wFormat;
	pHw->wBeNMCU = (pHw->wBeHEnd - pHw->wBeHStart) / 16 - 1;
}

///////////////////////////////////////////////////////////////////////////////
VOID ZRSetFormat(PDEVICE_INFO pDevInfo, WORD wFormat)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	if (wFormat == MICO_ENCODER_HIGH)
	{
		dprintf1(("SetFormat High"));
	}
	else if (wFormat == MICO_ENCODER_LOW)
	{
		dprintf1(("SetFormat Low"));
	}
	else
	{
		dprintf1(("SetFormat - invalid format"));
		return;
	}

	// Ändert sich das Bildformat?
	if (wFormat == pHw->wFormat)
		return;

	pHw->wFormat = wFormat;

	if (ZRGetEncoderState(pDevInfo) == MICO_ENCODER_ON)
	{
		ZRRestartEncoding(pDevInfo);
	}
}

///////////////////////////////////////////////////////////////////////////////
VOID ZRSetFramerate(PDEVICE_INFO pDevInfo, WORD wFramerate)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	pHw->wFramerate = wFramerate;
	if (wFramerate == 0)
	{
		dprintf1(("Error: Set Frame Rate to 0"));
		return;
	}

	pHw->wFps = (WORD)(25 / wFramerate);
}

///////////////////////////////////////////////////////////////////////////////
VOID ZRSetBPF(PDEVICE_INFO pDevInfo, DWORD dwBPF)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);
	pHw->dwBPF		= dwBPF;
	pHw->dwTCVData	= 8*dwBPF;
}

// This is the original Zoran Routines.modified to run under Windows NT in kernel mode.
///////////////////////////////////////////////////////////////////////////////
DWORD ZRMakeSOFMarker(PDEVICE_INFO pDevInfo, LP_Z050_CODER_CONFIG Cfg)
{
	WORD Org = SOF_INDX;
	DWORD Mlen;
	int j;

	ZRWrite050(pDevInfo, (WORD)Org, (BYTE)0xFF);
	//
	//  SOF0
	//
	ZRWrite050(pDevInfo, (WORD)(Org+0x1), (BYTE)SOF0_MARKER);
	//
	//  Len < 256
	//
	ZRWrite050(pDevInfo, (WORD)(Org+0x2), 0);
	ZRWrite050(pDevInfo, (WORD)(Org+0x3), (BYTE)(0x8 + (NO_OF_COMPONENTS * 3)));
	Mlen = (DWORD)(0x8 + (NO_OF_COMPONENTS * 3) +2);
	//
	//  P[8]
	//
	ZRWrite050(pDevInfo, (WORD)(Org+0x4), BASELINE_PRECISION);
	//
	//  Y(h) Y(l) X(h) X(l)
	//
	ZRWrite050(pDevInfo, (WORD)(Org+0x5), (BYTE)(Cfg->Height / 0x100));
	ZRWrite050(pDevInfo, (WORD)(Org+0x6), (BYTE)(Cfg->Height % 0x100));
	ZRWrite050(pDevInfo, (WORD)(Org+0x7), (BYTE)(Cfg->Width / 0x100));
	ZRWrite050(pDevInfo, (WORD)(Org+0x8), (BYTE)(Cfg->Width % 0x100));
	//
	//  YUV - 3 components
	//
	ZRWrite050(pDevInfo, (WORD)(Org+9), NO_OF_COMPONENTS);

	for (j=0; j<NO_OF_COMPONENTS; j++)
	{
		//
		//  C1 - id 0
		//
		ZRWrite050(pDevInfo, (WORD)(Org+0xA+(j*3)), (BYTE)j);
		//
		//  H, V
		//
		ZRWrite050(pDevInfo, (WORD)(Org+0xB+(j*3)),(BYTE)((Cfg->HSampRatio[j] * 0x10)+Cfg->VSampRatio[j]));
		//
		//  QTable
		ZRWrite050(pDevInfo, (WORD)(Org+0xC+(j*3)),(BYTE)QTid[j]);
	}
	return(Mlen);
};

///////////////////////////////////////////////////////////////////////////////
DWORD ZRMakeSOS1Marker(PDEVICE_INFO pDevInfo)
{
	WORD Org = SOS1_INDX;
	DWORD Mlen;
	BYTE Len;
	int j;

	ZRWrite050(pDevInfo, Org, 0xFF);
	//
	//  SOS
	//
	ZRWrite050(pDevInfo, (WORD)(Org+0x1), SOS_MARKER);
	//
	//  Len < 256
	//
	ZRWrite050(pDevInfo, (WORD)(Org+0x2), 0);
	Len = (BYTE)(0x3 + (NO_OF_SCAN1_COMPONENTS * 2) + 3);
	ZRWrite050(pDevInfo, (WORD)(Org+0x3), Len);
	Mlen = (DWORD)(Len + 2);
	//
	//  YUV - 3 components
	//
	ZRWrite050(pDevInfo, (WORD)(Org+4), NO_OF_SCAN1_COMPONENTS);

	for (j=0; j<NO_OF_SCAN1_COMPONENTS; j++)
	{
		//
		//  C1 - id 0
		//
		ZRWrite050(pDevInfo, (WORD)(Org+0x5+(j*2)), (BYTE)j);
		//
		//  AC,DC Tables
		//
		ZRWrite050(pDevInfo, (WORD)(Org+0x6+(j*2)), (BYTE)((SCAN1_HDCid[j] * 0x10)+SCAN1_HACid[j]));
	}

	ZRWrite050(pDevInfo, (WORD)(Org+ (WORD)Len - 1), 0x00);
	ZRWrite050(pDevInfo, (WORD)(Org+ (WORD)Len), 0x3F);
	ZRWrite050(pDevInfo, (WORD)(Org+ (WORD)Len + 1), 0x00);

	return(Mlen);
};

///////////////////////////////////////////////////////////////////////////////
DWORD ZRMakeDRIMarker(PDEVICE_INFO pDevInfo, LP_Z050_CODER_CONFIG Cfg)
{
	WORD Org = DRI_INDX;

	ZRWrite050(pDevInfo, Org, 0xFF);
	//
	//  DRI
	//
	ZRWrite050(pDevInfo, (WORD)(Org+0x1), DRI_MARKER);
	//
	//  Len < 256
	//
	ZRWrite050(pDevInfo, (WORD)(Org+0x2), 0);
	//
	//  Fixed Length
	//
	ZRWrite050(pDevInfo, (WORD)(Org+0x3), 4);
	//
	//  RI(h) RI(l)
	//
	ZRWrite050(pDevInfo, (WORD)(Org+0x4), (BYTE)(Cfg->DRI / 0x100));
	ZRWrite050(pDevInfo, (WORD)(Org+0x5), (BYTE)(Cfg->DRI % 0x100));
	//
	//  Fixed Length
	//
	return(6);
};


DWORD ZRMakeQTable(PDEVICE_INFO pDevInfo, char FAR* QTable)
{
	WORD Org = DQT_INDX;
	WORD Len;
	DWORD Mlen;
	int i,j;
	BYTE tmp;

	ZRWrite050(pDevInfo, Org, 0xFF);
	//
	//  DQT
	//
	ZRWrite050(pDevInfo, (WORD)(Org+0x1), DQT_MARKER);

	Len = ((QTABLE_SIZE + 1) * 2) + 2;
	Mlen = (DWORD)(Len + 2);

	ZRWrite050(pDevInfo, (WORD)(Org+0x2), (BYTE)(Len / 0x100));
	ZRWrite050(pDevInfo, (WORD)(Org+0x3), (BYTE)(Len % 0x100));

	for (j=0; j<(int)((QTABLE_SIZE + 1) * 2); j++)
	{
		ZRWrite050(pDevInfo, (WORD)(Org+0x4+j), (BYTE)QTable[j]);
	}

	i = 0;
	for (j=0; j<(int)((QTABLE_SIZE + 1) * 2); j++)
	{
		tmp = ZRRead050(pDevInfo, (WORD)(Org+0x4+j));
		if (tmp != (BYTE)QTable[j])
		i++;
	}
	if (i > 0)
	{
		dprintf1(("Errors in QTable Write %d entries of %d entries are wrong",i,j));
	}

	return(Mlen);
};

///////////////////////////////////////////////////////////////////////////////
DWORD ZRMakeHuffTable(PDEVICE_INFO pDevInfo, char FAR* HuffTable)
{
	WORD Org = DHT_INDX;
	WORD Len;
	DWORD Mlen;
	int i,j;
	BYTE tmp;

	ZRWrite050(pDevInfo, Org, 0xFF);
	//
	//  DHT
	//
	ZRWrite050(pDevInfo, (WORD)(Org+0x1), DHT_MARKER);

	Len = ((HUFF_AC_SIZE + 1) * 2) + ((HUFF_DC_SIZE + 1) * 2) + 2;
	Mlen = (DWORD)(Len + 2);

	ZRWrite050(pDevInfo, (WORD)(Org+0x2), (BYTE)(Len / 0x100));
	ZRWrite050(pDevInfo, (WORD)(Org+0x3), (BYTE)(Len % 0x100));

	for (j=0; j<(int)(((HUFF_AC_SIZE + 1) + (HUFF_DC_SIZE + 1)) * 2); j++)
	{
		ZRWrite050(pDevInfo, (WORD)(Org+0x4+j), (BYTE)HuffTable[j]);
	}

	i = 0;
	for (j=0; j<(int)(((HUFF_AC_SIZE + 1) + (HUFF_DC_SIZE + 1)) * 2); j++)
	{
		tmp = ZRRead050(pDevInfo, (WORD)(Org+0x4+j));
		if (tmp != (BYTE)HuffTable[j])
		i++;
	}

	if (i > 0)
	{
		dprintf1(("Errors in HuffTable Write %d entries of %d entries are wrong",i,j));
	}

	return(Mlen);
};


///////////////////////////////////////////////////////////////////////////////
DWORD ZRMakeAPPString(PDEVICE_INFO pDevInfo, char FAR* APPString)
{
	WORD Org = APP_INDX;
	DWORD Mlen;
	int i,j;
	BYTE tmp;

	if (APPString == NULL)
		return(0);

	ZRWrite050(pDevInfo, Org, 0xFF);
	//
	//  APP
	//
	ZRWrite050(pDevInfo, (WORD)(Org+0x1), APP_MARKER);

	ZRWrite050(pDevInfo, (WORD)(Org+0x2), 0x0);
	ZRWrite050(pDevInfo, (WORD)(Org+0x3), (BYTE)(APP_SIZE + 2));
	Mlen = (DWORD)(APP_SIZE + 4);

	for (j=0; j<APP_SIZE; j++)
	{
		ZRWrite050(pDevInfo, (WORD)(Org+0x4+j), (BYTE)APPString[j]);
	}

	i = 0;
	for (j=0; j<APP_SIZE; j++)
	{
		tmp = ZRRead050(pDevInfo, (WORD)(Org+0x4+j));
		if (tmp != (BYTE)APPString[j])
		i++;
	}

	if (i > 0)
	{
		dprintf1(("Errors in APPString Write %d entries of %d entries are wrong",i,j));
	}

	return(Mlen);
};

///////////////////////////////////////////////////////////////////////////////
DWORD ZRMakeCOMString(PDEVICE_INFO pDevInfo, char FAR* COMString)
{
	WORD Org = COM_INDX;
	DWORD Mlen;
	int i,j;
	BYTE tmp;

	if (COMString == NULL)
		return(0);

	ZRWrite050(pDevInfo, Org, 0xFF);
	//
	//  COM
	//
	ZRWrite050(pDevInfo, (WORD)(Org+0x1), COM_MARKER);

	ZRWrite050(pDevInfo, (WORD)(Org+0x2), 0x0);
	ZRWrite050(pDevInfo, (WORD)(Org+0x3), (BYTE)(COM_SIZE + 2));
	Mlen = (DWORD)(COM_SIZE + 4);

	for (j=0; j<COM_SIZE; j++)
	{
		ZRWrite050(pDevInfo, (WORD)(Org+0x4+j), (BYTE)COMString[j]);
	}

	i = 0;
	for (j=0; j<COM_SIZE; j++)
	{
		tmp = ZRRead050(pDevInfo, (WORD)(Org+0x4+j));
		if (tmp != (BYTE)COMString[j])
		i++;
	}

	if (i > 0)
	{
		dprintf1(("Errors in COMString Write %d entries of %d entries are wrong",i,j));
	}

	return(Mlen);
};

///////////////////////////////////////////////////////////////////////////////
void ZRMakeMarkerSegments(PDEVICE_INFO pDevInfo, LP_Z050_CODER_CONFIG Cfg)
{
	SOFMarkerSize = ZRMakeSOFMarker(pDevInfo, Cfg);
	SOSMarkerSize = ZRMakeSOS1Marker(pDevInfo);
	DRIMarkerSize = ZRMakeDRIMarker(pDevInfo, Cfg);
	DQTMarkerSize = ZRMakeQTable(pDevInfo, Cfg->QTable);
	DHTMarkerSize = ZRMakeHuffTable(pDevInfo, Cfg->HuffTable);
	APPMarkerSize = ZRMakeAPPString(pDevInfo, Cfg->APPString);
	COMMarkerSize = ZRMakeCOMString(pDevInfo, Cfg->COMString);
	DHTIMarkerSize = 0;
	DQTIMarkerSize = 0;
	DNLMarkerSize = 0;
};


///////////////////////////////////////////////////////////////////////////////
void ZRCoderStart(PDEVICE_INFO pDevInfo)
{
	ZRWrite050(pDevInfo, GO, 0);
};

///////////////////////////////////////////////////////////////////////////////
int  ZRCoderConfigureCompression(PDEVICE_INFO pDevInfo, LP_Z050_CODER_CONFIG Cfg)
{
	BYTE Tmp;
	WORD WTmp;
	DWORD DTmp,TotalMarkers,Dly = 0;

	delay_op = FALSE;

	Tmp = MSTR | NO_DMA | CFIS_1_CLK | NO_BELE;
	ZRWrite050(pDevInfo, HARDWARE, Tmp);

	Tmp = COMP | NO_ATP | NO_PASS2 | TLM | NO_DCONLY | NO_BRC;
	ZRWrite050(pDevInfo, MODE, Tmp);

	Tmp = NSCN_1 | NO_OVF;
	ZRWrite050(pDevInfo, OPTIONS, Tmp);

	Tmp = (BYTE) (Cfg->MaxBlockCodeVolume / 2);
	ZRWrite050(pDevInfo, MBCV_050, Tmp);

	//
	//  No Interrupts on Marker
	//
	Tmp = 0;
	ZRWrite050(pDevInfo, INT_REQ_0, Tmp);
	//
	//  The two LSB's must be 1 for no interrupts
	//
	Tmp = 3;
	ZRWrite050(pDevInfo, INT_REQ_1, Tmp);

	WTmp = Cfg->ScaleFactor;
	ZRWrite050(pDevInfo, SF_HI, (BYTE)(WTmp / 0x100));
	ZRWrite050(pDevInfo, SF_LO, (BYTE)(WTmp % 0x100));

	ZRWrite050(pDevInfo, AF_HI, 0xFF);
	ZRWrite050(pDevInfo, AF_M, 0xFF);
	ZRWrite050(pDevInfo, AF_LO, 0xFF);
	//
	//  Sets the MarkerSize Variables
	//
	ZRMakeMarkerSegments(pDevInfo, Cfg);

	Tmp = APP_OFF | COM_OFF | DRI_OFF | DQT_OFF | DHT_OFF | DNL_OFF | DQTI_OFF | DHTI_ON;
	ZRWrite050(pDevInfo, MARKERS_EN, Tmp);
	//
	//  Start the coder for Huff table preload
	//
	ZRCoderStart(pDevInfo);

	//
	//  Wait for it to finish (Max 3 sec)
	//
	while (!(ZRCheck050End(pDevInfo)))
	{
		if ((Dly++) > 3000)
		{
			dprintf1(("Zoran TableLoadMode (TLM) did not end - timeout"));
			return (0);
		}
		VC_Stall(1000);
	}
	dprintf1(("Zoran TableLoad (TLM) done..."));

	//
	//  HERE the 050 is in IDLE mode like after a RESET. Waits for GO.
	//
	Tmp = COMP | NO_ATP | PASS2 | NO_TLM | NO_DCONLY | BRC;
	ZRWrite050(pDevInfo, MODE, Tmp);

	//
	//  Not used here.
	//
	//  Tmp = APP_ON | COM_OFF | DRI_OFF  | DQT_ON | DHT_OFF | DNL_OFF | DQTI_OFF | DHTI_OFF;
	//
	Tmp = APP_ON | COM_OFF | DRI_OFF  | DQT_ON | DHT_ON | DNL_OFF | DQTI_OFF | DHTI_OFF;
	ZRWrite050(pDevInfo, MARKERS_EN, Tmp);

	TotalMarkers = SOFMarkerSize + SOSMarkerSize + 4;
	//
	//  SOF, SOS, SOI, EOI always exist
	//		     
	if (Tmp & APP_MASK) TotalMarkers += APPMarkerSize;
	if (Tmp & COM_MASK) TotalMarkers += COMMarkerSize;
	if (Tmp & DRI_MASK) TotalMarkers += DRIMarkerSize;
	if (Tmp & DQT_MASK) TotalMarkers += DQTMarkerSize;
	if (Tmp & DHT_MASK) TotalMarkers += DHTMarkerSize;
	if (Tmp & DNL_MASK) TotalMarkers += DNLMarkerSize;
	if (Tmp & DQTI_MASK) TotalMarkers += DQTIMarkerSize;
	if (Tmp & DHTI_MASK) TotalMarkers += DHTIMarkerSize;
	//
	//  in Bytes
	//
	DTmp = Cfg->TotalCodeVolume - TotalMarkers;
	//
	//  and bits
	//
	DTmp *= 8;

	WTmp = (WORD)(DTmp / 0x10000);
	ZRWrite050(pDevInfo, TCV_NET_HI, (BYTE)(WTmp / 0x100));
	ZRWrite050(pDevInfo, TCV_NET_MH, (BYTE)(WTmp % 0x100));

	WTmp = (WORD)(DTmp % 0x10000);
	ZRWrite050(pDevInfo, TCV_NET_ML, (BYTE)(WTmp / 0x100));
	ZRWrite050(pDevInfo, TCV_NET_LO, (BYTE)(WTmp % 0x100));

	//
	//  minus STUFFING minus EOB
	//
	DTmp -= (DTmp / 128);
	DTmp -= ((DTmp * 5) / 64);

	WTmp = (WORD)(DTmp / 0x10000);
	ZRWrite050(pDevInfo, TCV_DATA_HI, (BYTE)(WTmp / 0x100));
	ZRWrite050(pDevInfo, TCV_DATA_MH, (BYTE)(WTmp % 0x100));

	WTmp = (WORD)(DTmp % 0x10000);
	ZRWrite050(pDevInfo, TCV_DATA_ML, (BYTE)(WTmp / 0x100));
	ZRWrite050(pDevInfo, TCV_DATA_LO, (BYTE)(WTmp % 0x100));

	return 1;
};

///////////////////////////////////////////////////////////////////////////////
int ZRCoderConfigureExpansion(PDEVICE_INFO pDevInfo, LP_Z050_CODER_CONFIG Cfg)
{
	BYTE Tmp;
	DWORD Dly = 0;

	Tmp = NO_BSWD | MSTR | NO_DMA | CFIS_2_CLK | NO_BELE;
	ZRWrite050(pDevInfo, HARDWARE, Tmp);
	//
	//  Table Load Mode
	//
	Tmp = NO_COMP | TLM;
	ZRWrite050(pDevInfo, MODE, Tmp);
	//
	//  No Interrupts on Marker
	//
	Tmp = 0;
	ZRWrite050(pDevInfo, INT_REQ_0, Tmp);
	//
	//  The two LSB's must be 1
	//
	Tmp = 3;
	ZRWrite050(pDevInfo, INT_REQ_1, Tmp);
	//
	//  Only DHT is relevant here
	//
	ZRMakeHuffTable(pDevInfo, Cfg->HuffTable);

	Tmp = DHTI_ON;
	ZRWrite050(pDevInfo, MARKERS_EN, Tmp);
	//
	//  Start the Coder for Huffman Table preload
	//
	ZRCoderStart(pDevInfo);

	//
	//  Wait for it to finish (Max 1 sec)
	//
	while (!(ZRCheck050End(pDevInfo)))
	{
		if ((Dly++) > 2000)
		{
			dprintf1(("Zoran TableLoadMode (TLM) did not end - timeout"));
			return (0);
		}
		VC_Stall(500);
	}
	//
	//  HERE the 050 is in IDLE mode like after a RESET. Waits for GO.
	//
	Tmp = NO_COMP;
	//
	//  Expansion Mode
	//
	ZRWrite050(pDevInfo, MODE, Tmp);
	//
	//  Don't accept APP,COM into marker Segment
	//
	Tmp = APP_OFF | COM_OFF;
	ZRWrite050(pDevInfo, MARKERS_EN, Tmp);

	return 1;
};

///////////////////////////////////////////////////////////////////////////////
WORD  ZRCoderGetLastScaleFactor(PDEVICE_INFO pDevInfo)
{
	BYTE LowSF  = ZRRead050(pDevInfo,SF_LO);
	BYTE HighSF = ZRRead050(pDevInfo,SF_HI);

	return (((WORD)HighSF * 0x100) + LowSF);
}

///////////////////////////////////////////////////////////////////////////////
BYTE ZRRead050(PDEVICE_INFO pDevInfo, WORD wAddress)
{	      
	BYTE tmp;

	HW_WrShort(pDevInfo, ZR_ADDRESS_OFFSET, wAddress,0);
	if (delay_op == TRUE)
		VC_Stall(1000);

	tmp = HW_RdByte(pDevInfo, ZR_DATA_OFFSET,0);
	if (delay_op == TRUE)
		VC_Stall(1000);

	return tmp;
}

///////////////////////////////////////////////////////////////////////////////
void ZRWrite050(PDEVICE_INFO pDevInfo, WORD wAdr, BYTE bData)
{
	HW_WrShort(pDevInfo, ZR_ADDRESS_OFFSET, wAdr,0);
	if (delay_op == TRUE)
		VC_Stall(1000);

	HW_WrByte(pDevInfo, ZR_DATA_OFFSET, bData,0);
	if (delay_op == TRUE)
		VC_Stall(1000);
}

///////////////////////////////////////////////////////////////////////////////
BOOL ZRCheck050End(PDEVICE_INFO pDevInfo)
{
	return (ZRRead050(pDevInfo, 0x002f) & 0x04);
}

///////////////////////////////////////////////////////////////////////////////
BOOL ZRStartEncoding(PDEVICE_INFO pDevInfo)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);
	BYTE sts,tmp,bCom0,bCom1,bCom2;
	char strSpace[64];
	Z050_CODER_CONFIG Cfg;
	WORD wSamplesPerLine,wNumberOfLines,wMBCV;
	int i;

	//
	//  Set Fast Coder Mode
	//
	Step_D = TRUE;
	bCom0 = 0;
	bCom1 = 0;
	bCom2 = 0;
	//
	//  055 Init Sequence Cont. Compression
	//
	bCom0 = (BYTE) CLRBIT(bCom0, COM0_GRESET_BIT);
	HW_WrByte(pDevInfo,ZR_COM0_OFFSET, bCom0,0);

	bCom1 = (BYTE) SETBIT(bCom1, COM1_ACTIVE_BIT);
	bCom1 = (BYTE) CLRBIT(bCom1, COM1_CLK_EN_BIT);
	HW_WrByte(pDevInfo,ZR_COM1_OFFSET, bCom1,0);

	bCom0 = (BYTE) CLRBIT(bCom0, COM0_EXP_BIT);

	if ((pHw->wZRFeVideoFormat == MICO_PAL_SQUARE) || (pHw->wZRFeVideoFormat == MICO_PAL_CCIR))
		bCom1 = (BYTE) CLRBIT(bCom1, COM1_VID_FMT_BIT);
	else if ((pHw->wZRFeVideoFormat == MICO_NTSC_SQUARE) || (pHw->wZRFeVideoFormat == MICO_NTSC_CCIR))
		bCom1 = (BYTE) SETBIT(bCom1, COM1_VID_FMT_BIT);

	bCom0 = (BYTE) SETBIT(bCom0, COM0_PCG0_BIT);
	bCom0 = (BYTE) SETBIT(bCom0, COM0_PCG1_BIT);

	bCom1 = (BYTE) CLRBIT(bCom1, COM1_VER_DEC_BIT);

	if (pHw->wFormat == MICO_ENCODER_HIGH)
		bCom1 = (BYTE) CLRBIT(bCom1, COM1_HOR_DEC_BIT);
	else if (pHw->wFormat == MICO_ENCODER_LOW)
		bCom1 = (BYTE) SETBIT(bCom1, COM1_HOR_DEC_BIT);

	bCom2 = (BYTE) CLRBIT(bCom2, COM2_VID_FILT_BIT);
	//
	//  This can only be set if it is a STEP_D chip !!!
	//
	if (Step_D == TRUE)
		bCom0 = (BYTE) SETBIT(bCom0, COM0_FAST_BIT);
	else
		bCom0 = (BYTE) CLRBIT(bCom0, COM0_FAST_BIT);

	bCom0 = (BYTE) SETBIT(bCom0, COM0_VID_422_BIT);
	bCom0 = (BYTE) CLRBIT(bCom0, COM0_STILL_BIT);
	bCom1 = (BYTE) CLRBIT(bCom1, COM1_INT_EN_BIT);
	bCom1 = (BYTE) SETBIT(bCom1, COM1_CB_SIZE_BIT);
	bCom1 = (BYTE) SETBIT(bCom1, COM1_ACTIVE_BIT);
	bCom2 = (BYTE) SETBIT(bCom2, COM2_STRP_SIZE_BIT);

	HW_WrByte(pDevInfo,ZR_COM0_OFFSET, bCom0,0);
	HW_WrByte(pDevInfo,ZR_COM1_OFFSET, bCom1,0);
	HW_WrByte(pDevInfo,ZR_COM2_OFFSET, bCom2,0);
	//
	//  Set HSTART, HEND, HTOTAL, VSTART, VEND, VTOTAL and NMCU
	//
	HW_WrShort(pDevInfo,ZR_SIZE_OFFSET,(WORD)(0x0000 | pHw->wFeHStart[pHw->wZRFeVideoFormat]),0);
	HW_WrShort(pDevInfo,ZR_SIZE_OFFSET,(WORD)(0x1000 | pHw->wFeHEnd[pHw->wZRFeVideoFormat]-1),0);
	HW_WrShort(pDevInfo,ZR_SIZE_OFFSET,(WORD)(0x2000 | pHw->wFeHTotal[pHw->wZRFeVideoFormat]),0);
	HW_WrShort(pDevInfo,ZR_SIZE_OFFSET,(WORD)(0x8000 | pHw->wFeVStart[pHw->wZRFeVideoFormat]),0);
	HW_WrShort(pDevInfo,ZR_SIZE_OFFSET,(WORD)(0x9000 | pHw->wFeVEnd[pHw->wZRFeVideoFormat]-1),0);
	HW_WrShort(pDevInfo,ZR_SIZE_OFFSET,(WORD)(0xa000 | pHw->wFeVTotal[pHw->wZRFeVideoFormat]),0);

	HW_WrShort(pDevInfo,ZR_SIZE_OFFSET,(WORD)(0x3000 | pHw->wFeHSStart[pHw->wZRFeVideoFormat]),0);
	HW_WrShort(pDevInfo,ZR_SIZE_OFFSET,(WORD)(0x4000 | pHw->wFeHSEnd[pHw->wZRFeVideoFormat]),0);
	HW_WrShort(pDevInfo,ZR_SIZE_OFFSET,(WORD)(0xc000 | pHw->wFeNMCU[pHw->wZRFeVideoFormat]),0);
	HW_WrShort(pDevInfo,ZR_SIZE_OFFSET,(WORD)(0x5000 | pHw->wFeBlankStart[pHw->wZRFeVideoFormat]),0);
	HW_WrShort(pDevInfo,ZR_SIZE_OFFSET,(WORD)(0xe000 | pHw->wYBackgnd),0);
	HW_WrShort(pDevInfo,ZR_SIZE_OFFSET,(WORD)(0xe800 | pHw->wUVBackgnd),0);
	//
	//  Enable Clock
	//
	bCom1 = (BYTE) SETBIT(bCom1, COM1_CLK_EN_BIT);
	HW_WrByte(pDevInfo,ZR_COM1_OFFSET, bCom1,0);
	//
	//  Wait Min .5 ms for PLL to stabilize
	//
	VC_Stall(1000);
	//
	//  Deactivate GRESET (COM0 = 0xf3)
	//
	bCom0 = HW_RdByte(pDevInfo,ZR_COM0_OFFSET,0);
	bCom0 = (BYTE) SETBIT(bCom0, COM0_GRESET_BIT);
	HW_WrByte(pDevInfo,ZR_COM0_OFFSET, bCom0,0);

	wSamplesPerLine = pHw->wFeHEnd[pHw->wZRFeVideoFormat] -
	pHw->wFeHStart[pHw->wZRFeVideoFormat];
	wNumberOfLines  = pHw->wFeVEnd[pHw->wZRFeVideoFormat] -
	pHw->wFeVStart[pHw->wZRFeVideoFormat];

	if ((wSamplesPerLine * wNumberOfLines) == 0)
	{
		dprintf1(("ZRStartEncoding: BUG - Division by zero"));
		return FALSE;
	}
	
	//	wMBCV	= 8L * 64L * (DWORD)wBPF / ((DWORD)wSamplesPerLine * (DWORD)wNumberOfLines); 
	wMBCV = 510; 

	//
	//  Turn on horizontal decimating for ENCODER_LOW setting
	//
	if (pHw->wFormat == MICO_ENCODER_LOW)
		wSamplesPerLine /= 2;

	Cfg.Width  = wSamplesPerLine;
	Cfg.Height = wNumberOfLines;
	Cfg.HSampRatio[0] = 2;
	Cfg.VSampRatio[0] = 1;
	Cfg.HSampRatio[1] = 1;
	Cfg.VSampRatio[1] = 1;
	Cfg.HSampRatio[2] = 1;
	Cfg.VSampRatio[2] = 1;
	Cfg.TotalCodeVolume = pHw->dwBPF;
	Cfg.MaxBlockCodeVolume = wMBCV;
	Cfg.ScaleFactor = 1*0x100;
	Cfg.DRI = 1;

	Cfg.QTable =    (char FAR *)&DefaultQTables[0];
	Cfg.HuffTable = (char FAR *)&DefaultHuffTables[0];

	for (i=0; i<64; i++)
		strSpace[i] = 0x20;

	Cfg.APPString = (char FAR *)&strSpace;
	Cfg.COMString = (char FAR *)&strSpace;

	tmp = ZRCoderConfigureCompression(pDevInfo, (LP_Z050_CODER_CONFIG)&Cfg);

	while (tmp = 0)
	{
		dprintf1(("Re-init ZR36050 Table preload timeout..."));
		tmp = ZRCoderConfigureCompression(pDevInfo, (LP_Z050_CODER_CONFIG)&Cfg);
	}

	ZRRead050(pDevInfo,GO);
	//
	//  Get Status
	//
	tmp = HW_RdByte(pDevInfo,ZR_FCNT_OFFSET,0);
	dprintf1(("FCNT = %d",tmp));
	sts = HW_RdByte(pDevInfo,ZR_STATUS0_OFFSET,0);
	dprintf1(("Status0 = %d",sts));
	tmp = HW_RdByte(pDevInfo,ZR_LEN0_OFFSET,0);
	dprintf1(("Len0 = %d",tmp));
	tmp = HW_RdByte(pDevInfo,ZR_LEN1_OFFSET,0);
	dprintf1(("Len1 = %d",tmp));

	bCom1 = HW_RdByte(pDevInfo,ZR_COM1_OFFSET,0);
	bCom1 = (BYTE) SETBIT(bCom1, COM1_INT_EN_BIT);
	bCom1 = (BYTE) CLRBIT(bCom1, COM1_ACTIVE_BIT);
	HW_WrByte(pDevInfo,ZR_COM1_OFFSET, bCom1,0);

	pHw->nEncoderState = MICO_ENCODER_ON;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
BOOL ZRRestartEncoding(PDEVICE_INFO pDevInfo)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);
	BYTE flag;

	ZRStopEncoding(pDevInfo);
	flag = FALSE;
	while (!flag)
	{
		ZRStartEncoding(pDevInfo);
		//
		//  Wait 80ms for some to happen
		//
		VC_Stall(80000);
		if (pHw->dwZRInterruptCount != 0)
		{
			dprintf1(("ZR Iack Cnt = 0x%x",pHw->dwZRInterruptCount));
			flag = TRUE;
		}
		else
		{
			dprintf1(("Retrying encoder start"));
			ZRStopEncoding(pDevInfo);
		}
	}
	return TRUE;

}

///////////////////////////////////////////////////////////////////////////////
BOOL ZRStopEncoding(PDEVICE_INFO pDevInfo)
{
	BYTE bCom1;
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	//
	//  Set Commandregister 1
	//
	bCom1 = HW_RdByte(pDevInfo,ZR_COM1_OFFSET,0);
	bCom1 = (BYTE) SETBIT(bCom1, COM1_ACTIVE_BIT);
	bCom1 = (BYTE) CLRBIT(bCom1, COM1_INT_EN_BIT);
	HW_WrByte(pDevInfo,ZR_COM1_OFFSET, bCom1,0);

	pHw->nEncoderState = MICO_ENCODER_OFF;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
BYTE ZRGetEncoderState(PDEVICE_INFO pDevInfo)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	return pHw->nEncoderState;
}

///////////////////////////////////////////////////////////////////////////////
VOID ZRChipSelectVideoInAnalogOut(PDEVICE_INFO pDevInfo, WORD wFrontEnd, WORD wExtCard, BYTE bData)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	if (wExtCard == MICO_EXTCARD0)
	{
		if (wFrontEnd == MICO_FE0)
			HW_WriteToLatch(pDevInfo, CSVINAN1_OFFSET, bData,0);
		else if (wFrontEnd == MICO_FE1)
			HW_WriteToLatch(pDevInfo, CSVINAN2_OFFSET, bData,0);
	}
	else
	{
		HW_WriteToLatch(pDevInfo, CSVINAN_COVI_OFFSET, bData,wExtCard);
	}
}

///////////////////////////////////////////////////////////////////////////////
BYTE ZRChipSelectVideoInAnalogIn(PDEVICE_INFO pDevInfo, WORD wFrontEnd, WORD wExtCard)
{
	BYTE tmp;
	PHWINFO pHw;

	pHw = VC_GetHWInfo(pDevInfo);

	if (wExtCard == MICO_EXTCARD0)
	{
		if (wFrontEnd == MICO_FE0)
			return HW_ReadFromLatch(pDevInfo, CSVINAN1_OFFSET,0);
		else if (wFrontEnd == MICO_FE1)
			return HW_ReadFromLatch(pDevInfo, CSVINAN2_OFFSET,0);
		else
			return 0;
	}
	else
	{
		tmp = HW_ReadFromLatch(pDevInfo, CSVINAN_COVI_OFFSET, wExtCard);
		return tmp;
	}
}

///////////////////////////////////////////////////////////////////////////////
VOID ZRChipSelectVideoInDigitalOut(PDEVICE_INFO pDevInfo, BYTE bData)
{
	HW_WriteToLatch(pDevInfo, CSVINDN_OFFSET, bData,0);
}

///////////////////////////////////////////////////////////////////////////////
BYTE ZRChipSelectVideoInDigitalIn(PDEVICE_INFO pDevInfo)
{
	return HW_ReadFromLatch(pDevInfo, CSVINDN_OFFSET,0);
}

//-----------------------------------------------------------------------------
// Relais Routines
//-----------------------------------------------------------------------------


///////////////////////////////////////////////////////////////////////////////
void RELSetRelay(PDEVICE_INFO pDevInfo, BYTE bRelais)
{
	BYTE bReg;
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	if (pHw->wRELExtCard == MICO_EXTCARD0)
	{
		if (pHw->byIgnoreRelay4)
		{
			bReg = HW_RdByte(pDevInfo,RELAY_OFFSET,0);
			//          bReg &= 0xf8;
			bReg |= bRelais;
			HW_WrByte(pDevInfo, RELAY_OFFSET, bReg,0);
		}
		else
		{
			bReg = HW_RdByte(pDevInfo,RELAY_OFFSET,0);
			//          bReg &= 0xf0;
			bReg |= bRelais;
			HW_WrByte(pDevInfo, RELAY_OFFSET, bReg,0);
		}
	}
	else
	{
		bReg = HW_RdByte(pDevInfo, RELAY_COVI_OFFSET, pHw->wRELExtCard);
		//       bReg &= 0xf0;
		bReg |= bRelais;
		HW_WrByte(pDevInfo, RELAY_COVI_OFFSET, bReg, pHw->wRELExtCard);
	}
}

///////////////////////////////////////////////////////////////////////////////
void RELClrRelay(PDEVICE_INFO pDevInfo, BYTE bRelais)
{
	BYTE bReg,tmp;
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	if (pHw->wRELExtCard == MICO_EXTCARD0)
	{
		if (pHw->byIgnoreRelay4)
		{
			bReg = HW_RdByte(pDevInfo,RELAY_OFFSET,0);
			bReg &= (bRelais & 0x07);
			HW_WrByte(pDevInfo, RELAY_OFFSET, bReg,0);
		}
		else
		{
			bReg = HW_RdByte(pDevInfo,RELAY_OFFSET,0);
			bReg &= bRelais;
			HW_WrByte(pDevInfo, RELAY_OFFSET, bReg,0);
		}
	}
	else
	{
		bReg = HW_RdByte(pDevInfo, RELAY_COVI_OFFSET,pHw->wRELExtCard);
		bReg &= bRelais;
		HW_WrByte(pDevInfo, RELAY_COVI_OFFSET, bReg,pHw->wRELExtCard);
	}
}

///////////////////////////////////////////////////////////////////////////////
BYTE RELGetRelay(PDEVICE_INFO pDevInfo)
{
	BYTE bReg;
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	if (pHw->wRELExtCard == MICO_EXTCARD0)
	{
		bReg = HW_RdByte(pDevInfo, RELAY_OFFSET,0);
		return (bReg & 0x0f);
	}
	else
	{
		bReg = HW_RdByte(pDevInfo, RELAY_COVI_OFFSET,pHw->wRELExtCard);
		return (bReg & 0x0f);
	}
}


//-----------------------------------------------------------------------------
// Alarm Access Routines
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
BYTE ALARMGetCurrentState(PDEVICE_INFO pDevInfo)
{
	BYTE tmp;
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	if (pHw->wALRExtCard == MICO_EXTCARD0)
		return HW_RdByte(pDevInfo,ALARM_CUR_STATUS_OFFSET,0);
	else
	{
		tmp = HW_RdByte(pDevInfo,ALARM_CUR_STATUS_COVI_OFFSET,pHw->wALRExtCard);
		return tmp;
	}
}

///////////////////////////////////////////////////////////////////////////////
BYTE ALARMGetState(PDEVICE_INFO pDevInfo)
{
	BYTE tmp;
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	if (pHw->wALRExtCard == MICO_EXTCARD0)
		return HW_RdByte(pDevInfo,ALARM_STATUS_OFFSET,0);
	else
	{
		tmp = HW_RdByte(pDevInfo,ALARM_STATUS_COVI_OFFSET,pHw->wALRExtCard);
		return tmp;
	}
}

///////////////////////////////////////////////////////////////////////////////
void ALARMSetEdge(PDEVICE_INFO pDevInfo, BYTE byEdge)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	if (pHw->wALRExtCard == MICO_EXTCARD0)
		HW_WrByte(pDevInfo,ALARM_EDGE_SEL_OFFSET, byEdge,0);
	else
	{
		HW_WrByte(pDevInfo,ALARM_EDGE_SEL_COVI_OFFSET, byEdge,pHw->wALRExtCard);
	}

	pHw->byEdge = byEdge;
}

///////////////////////////////////////////////////////////////////////////////
void ALARMSetAck(PDEVICE_INFO pDevInfo, BYTE byAck)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	if (pHw->wALRExtCard == MICO_EXTCARD0)
		HW_WrByte(pDevInfo,ALARM_ACK_OFFSET, byAck,0);
	else
	{
		HW_WrByte(pDevInfo,ALARM_ACK_COVI_OFFSET, byAck, pHw->wALRExtCard);
	}

	pHw->byAck = byAck;
}

///////////////////////////////////////////////////////////////////////////////
BYTE ALARMGetAck(PDEVICE_INFO pDevInfo)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	return pHw->byAck;
}

///////////////////////////////////////////////////////////////////////////////
BYTE ALARMGetEdge(PDEVICE_INFO pDevInfo)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	return pHw->byEdge;
}


//-----------------------------------------------------------------------------
// Mico Higher Level Routines
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
BOOLEAN MICOParameter(PDEVICE_INFO pDevInfo)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);
	DWORD sts;
	//
	//  Set "Card present Bits" from Registry.
	//
	HW_DisableInts(pDevInfo,pHw);    
	pHw->dwHardwareState = SETBIT(pHw->dwHardwareState,0);
	sts = VC_ReadProfile(pDevInfo, PARAM_EXC1, 0);
	if (sts != 0)
	{
		dprintf1(("Setting Extcard 1")); 
		pHw->dwHardwareState = SETBIT(pHw->dwHardwareState,1);
		HW_MapPort(pDevInfo,(WORD)sts,0x1f);
		pHw->dwExtCardIO[1] = (DWORD) pDevInfo->PortBase;
	}
	else
	{
		dprintf1(("Clear Extcard 1")); 
		pHw->dwHardwareState = CLRBIT(pHw->dwHardwareState,1);
		pHw->dwExtCardIO[1] = 0;
	}

	sts = VC_ReadProfile(pDevInfo, PARAM_EXC2, 0);
	if (sts != 0)
	{
		pHw->dwHardwareState = SETBIT(pHw->dwHardwareState,2);
		HW_MapPort(pDevInfo,(WORD)sts,0x1f);
		pHw->dwExtCardIO[2] = (DWORD) pDevInfo->PortBase;
	}
	else
	{
		pHw->dwHardwareState = CLRBIT(pHw->dwHardwareState,2);
		pHw->dwExtCardIO[2] = 0;
	}

	sts = VC_ReadProfile(pDevInfo, PARAM_EXC3, 0);
	if (sts != 0)
	{
		pHw->dwHardwareState = SETBIT(pHw->dwHardwareState,3);
		HW_MapPort(pDevInfo,(WORD)sts,0x1f);
		pHw->dwExtCardIO[3] = (DWORD) pDevInfo->PortBase;
	}
	else
	{
		pHw->dwHardwareState = CLRBIT(pHw->dwHardwareState,3);
		pHw->dwExtCardIO[3] = 0;
	}

	pDevInfo->PortBase = (PUCHAR)pHw->dwExtCardIO[0];
	HW_EnableInts(pDevInfo,pHw);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
WORD MICOGetVersion(PDEVICE_INFO pDevInfo)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);
	return (WORD)pHw->mico_id;
}

///////////////////////////////////////////////////////////////////////////////
BOOL MICOIsVideoPresent(PDEVICE_INFO pDevInfo)
{
	BOOL bRet = 0;
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	//  This looks odd, but MICOSetInputSource switched the frontends on exit - so we
	//  can only look at the 'other' frontend to check for 'VideoPresent'
	if (pHw->wFrontEnd == MICO_FE0)
		bRet = BT829IsVideoPresent(pDevInfo,(DWORD)MICO_FE1);
	else
		bRet = BT829IsVideoPresent(pDevInfo,(DWORD)MICO_FE0);

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
DWORD MICOSetInputSource(PDEVICE_INFO pDevInfo, WORD wExtCard, WORD wSource, WORD wFormat, DWORD dwBPF, DWORD dwProzessID)
{
	DWORD	dwOldSource		= 0;
	BYTE	byPortY			= 0;
	BYTE	byPortC			= 0;
	DWORD	dwVidSel		= 0;
	PHWINFO pHw				= VC_GetHWInfo(pDevInfo);

	dwOldSource		= MAKELONG(pHw->wSource, pHw->wExtCard);

	if (!MICOCheckExtCard(pDevInfo, wExtCard))
	{
		return MICO_ERROR;
	}

	if(!MICOCheckSource(pDevInfo, wSource))
	{
		return MICO_ERROR;
	}
	
	// Bildgröße und Format setzen
	MICOEncoderSetBPF(pDevInfo,		dwBPF);
	MICOEncoderSetFormat(pDevInfo,	wFormat);

	// Das Frontend auf den Sourcetype der gewüschten Kamera schalten
	BT829SetFeSourceType(pDevInfo, pHw->MiCoISA.ExtCard[wExtCard].Source[wSource].wFeSourceType,
								   pHw->MiCoISA.ExtCard[wExtCard].Source[wSource].wFeFilter,
								   (DWORD)pHw->wFrontEnd);

	// Kamera umschalten
	byPortY  = (BYTE)pHw->MiCoISA.ExtCard[wExtCard].Source[wSource].nAVPortY;
	byPortC |= (BYTE)pHw->MiCoISA.ExtCard[wExtCard].Source[wSource].nAVPortC;
	ZRSetInputSource(pDevInfo, pHw->wFrontEnd, wExtCard, wSource, byPortY, byPortC, dwProzessID);

	// Die Frontends werden im Wechsel angesprochen
	if (pHw->wFrontEnd == MICO_FE0)
		pHw->wFrontEnd = MICO_FE1;
	else if (pHw->wFrontEnd == MICO_FE1)
		pHw->wFrontEnd = MICO_FE0;

	return dwOldSource;
}

///////////////////////////////////////////////////////////////////////////////
void MICOGetInputSource(PDEVICE_INFO pDevInfo, WORD wExtCard, WORD wSource)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	wSource         = pHw->wSource;
	wExtCard        = pHw->wExtCard;
}

///////////////////////////////////////////////////////////////////////////////
DWORD MICOGetHardwareState(PDEVICE_INFO pDevInfo)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	return pHw->dwHardwareState;
}

///////////////////////////////////////////////////////////////////////////////
WORD MICOSetFeSourceType(PDEVICE_INFO pDevInfo, WORD wSourceType)
{
	WORD wOldSourceType = MICOGetFeSourceType(pDevInfo);
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	if (MICOCheckSourceType(pDevInfo,wSourceType))
	{
		BT829SetFeSourceType(pDevInfo, wSourceType,
							pHw->MiCoISA.ExtCard[pHw->wExtCard].Source[pHw->wSource].wFeFilter,
							(DWORD)MICO_FE0);
		BT829SetFeSourceType(pDevInfo, wSourceType,
							pHw->MiCoISA.ExtCard[pHw->wExtCard].Source[pHw->wSource].wFeFilter,
							(DWORD)MICO_FE1);
		pHw->MiCoISA.ExtCard[pHw->wExtCard].Source[pHw->wSource].wFeSourceType = wSourceType;
	}
	return wOldSourceType;
}

///////////////////////////////////////////////////////////////////////////////
WORD MICOGetFeSourceType(PDEVICE_INFO pDevInfo)
{
	return BT829GetFeSourceType(pDevInfo);
}

///////////////////////////////////////////////////////////////////////////////
WORD MICOSetFeVideoFormat(PDEVICE_INFO pDevInfo, WORD wFeVideoFormat)
{
	WORD wOldFeVideoFormat = MICOGetFeVideoFormat(pDevInfo);
	BYTE bParam;

	if (wOldFeVideoFormat == wFeVideoFormat)
		return wOldFeVideoFormat;

	if (MICOCheckFeVideoFormat(pDevInfo,wFeVideoFormat))
	{
		bParam = HW_ReadFromLatch(pDevInfo, PARAM_OFFSET, 0);
		switch (wFeVideoFormat)
		{
			case MICO_PAL_SQUARE:
			case MICO_NTSC_SQUARE:
				bParam = (BYTE) SETBIT(bParam, PARAM_SQUARE_BIT);
				bParam = (BYTE) SETBIT(bParam, PARAM_SQUARE2_BIT);
				break;
			case MICO_PAL_CCIR:
			case MICO_NTSC_CCIR:
				bParam = (BYTE) CLRBIT(bParam, PARAM_SQUARE_BIT);
				bParam = (BYTE) CLRBIT(bParam, PARAM_SQUARE2_BIT);
				break;
		}
		HW_WriteToLatch(pDevInfo,PARAM_OFFSET, bParam,0);

		BT829SetFeVideoFormat(pDevInfo, wFeVideoFormat, (DWORD)MICO_FE0);
		BT829SetFeVideoFormat(pDevInfo, wFeVideoFormat, (DWORD)MICO_FE1);
		ZRSetFeVideoFormat(pDevInfo,wFeVideoFormat);
	}

	return wOldFeVideoFormat;
}

///////////////////////////////////////////////////////////////////////////////
WORD MICOGetFeVideoFormat(PDEVICE_INFO pDevInfo)
{
	return BT829GetFeVideoFormat(pDevInfo);
}

///////////////////////////////////////////////////////////////////////////////
WORD MICOSetBeVideoFormat(PDEVICE_INFO pDevInfo, WORD wBeVideoFormat)
{
	WORD wOldBeVideoFormat = MICOGetBeVideoFormat(pDevInfo);

	if (wOldBeVideoFormat == wBeVideoFormat)
		return wOldBeVideoFormat;

	if(MICOCheckBeVideoFormat(pDevInfo,wBeVideoFormat))
	{
		BT856SetBeVideoFormat(pDevInfo,wBeVideoFormat);
		ZRSetBeVideoFormat(pDevInfo,wBeVideoFormat);
	}
	return wOldBeVideoFormat;
}

///////////////////////////////////////////////////////////////////////////////
WORD MICOGetBeVideoFormat(PDEVICE_INFO pDevInfo)
{
	return  BT856GetBeVideoFormat(pDevInfo);
}

///////////////////////////////////////////////////////////////////////////////
WORD MICOSetBeVideoType(PDEVICE_INFO pDevInfo, WORD wBeVideoType)
{
	WORD wOldBeVideoType = MICOGetBeVideoType(pDevInfo);
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);
	BYTE bVal;

	if (wOldBeVideoType == wBeVideoType)
		return wOldBeVideoType;

	if(MICOCheckBeVideoType(pDevInfo,wBeVideoType))
	{
		bVal = HW_ReadFromLatch(pDevInfo, RELAY_OFFSET, 0);

		if (wBeVideoType == MICO_COMPOSITE)
			bVal = (BYTE) CLRBIT(bVal, RGBOUT_BIT);
		else
			bVal = (BYTE) SETBIT(bVal, RGBOUT_BIT);

		HW_WriteToLatch(pDevInfo, RELAY_OFFSET, bVal,0);

		pHw->wBeVideoType = wBeVideoType;
	}

	return wOldBeVideoType;
}

///////////////////////////////////////////////////////////////////////////////
WORD MICOGetBeVideoType(PDEVICE_INFO pDevInfo)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	return  pHw->wBeVideoType;
}

///////////////////////////////////////////////////////////////////////////////
WORD MICOSetFeBrightness(PDEVICE_INFO pDevInfo, WORD wBrightness)
{
	WORD wOldBrightness = MICOGetFeBrightness(pDevInfo);

	if (wBrightness == wOldBrightness)
		return wOldBrightness;

	BT829SetFeBrightness(pDevInfo, wBrightness, (DWORD)MICO_FE0);
	BT829SetFeBrightness(pDevInfo, wBrightness, (DWORD)MICO_FE1);

	return wOldBrightness;
}

///////////////////////////////////////////////////////////////////////////////
WORD MICOGetFeBrightness(PDEVICE_INFO pDevInfo)
{
	return BT829GetFeBrightness(pDevInfo);
}

///////////////////////////////////////////////////////////////////////////////
WORD MICOSetFeContrast(PDEVICE_INFO pDevInfo, WORD wContrast)
{
	WORD wOldContrast = MICOGetFeContrast(pDevInfo);

	if (wContrast == wOldContrast)
		return wOldContrast;

	BT829SetFeContrast(pDevInfo, wContrast, (DWORD)MICO_FE0);
	BT829SetFeContrast(pDevInfo, wContrast, (DWORD)MICO_FE1);

	return wOldContrast;
}

///////////////////////////////////////////////////////////////////////////////
WORD MICOGetFeContrast(PDEVICE_INFO pDevInfo)
{
	return BT829GetFeContrast(pDevInfo);
}

///////////////////////////////////////////////////////////////////////////////
WORD MICOSetFeSaturation(PDEVICE_INFO pDevInfo, WORD wSaturation)
{
	WORD wOldContrast = MICOGetFeSaturation(pDevInfo);

	if (wSaturation == wOldContrast)
		return wOldContrast;

	BT829SetFeSaturation(pDevInfo, wSaturation, (DWORD)MICO_FE0);
	BT829SetFeSaturation(pDevInfo, wSaturation, (DWORD)MICO_FE1);

	return wOldContrast;
}

///////////////////////////////////////////////////////////////////////////////
WORD MICOGetFeSaturation(PDEVICE_INFO pDevInfo)
{
	return BT829GetFeSaturation(pDevInfo);
}

///////////////////////////////////////////////////////////////////////////////
WORD MICOSetFeHue(PDEVICE_INFO pDevInfo, WORD wHue)
{
	WORD wOldHue = BT829GetFeHue(pDevInfo);

	if (wHue == wOldHue)
		return wOldHue;

	BT829SetFeHue(pDevInfo, wHue, (DWORD)MICO_FE0);
	BT829SetFeHue(pDevInfo, wHue, (DWORD)MICO_FE1);

	return wOldHue;
}

///////////////////////////////////////////////////////////////////////////////
WORD MICOGetFeHue(PDEVICE_INFO pDevInfo)
{
	return BT829GetFeHue(pDevInfo);
}

///////////////////////////////////////////////////////////////////////////////
WORD MICOSetFeHScale(PDEVICE_INFO pDevInfo, WORD wHScale)
{
	WORD wOldHScale = MICOGetFeHScale(pDevInfo);

	if (wHScale == wOldHScale)
		return wOldHScale;

	BT829SetFeHScale(pDevInfo, wHScale, (DWORD)MICO_FE0);
	BT829SetFeHScale(pDevInfo, wHScale, (DWORD)MICO_FE1);

	return wOldHScale;
}

///////////////////////////////////////////////////////////////////////////////
WORD MICOGetFeHScale(PDEVICE_INFO pDevInfo)
{
	return BT829GetFeHScale(pDevInfo);
}

///////////////////////////////////////////////////////////////////////////////
WORD MICOSetFeVScale(PDEVICE_INFO pDevInfo, WORD wVScale)
{
	WORD wOldVScale = BT829GetFeVScale(pDevInfo);

	if (wVScale == wOldVScale)
		return wOldVScale;

	BT829SetFeVScale(pDevInfo, wVScale, (DWORD)MICO_FE0);
	BT829SetFeVScale(pDevInfo, wVScale, (DWORD)MICO_FE1);

	return wOldVScale;
}

///////////////////////////////////////////////////////////////////////////////
WORD MICOGetFeVScale(PDEVICE_INFO pDevInfo)
{
    return  BT829GetFeVScale(pDevInfo);
}

///////////////////////////////////////////////////////////////////////////////
WORD MICOSetFeFilter(PDEVICE_INFO pDevInfo, WORD wFilter)
{
	WORD wOldFilter = BT829GetFeFilter(pDevInfo);

	if (wFilter == wOldFilter)
		return wOldFilter;

	if(MICOCheckFeFilter(pDevInfo,wFilter))
	{
		BT829SetFeFilter(pDevInfo, wFilter, (DWORD)MICO_FE0);
		BT829SetFeFilter(pDevInfo, wFilter, (DWORD)MICO_FE1);
	}

	return wOldFilter;
}

///////////////////////////////////////////////////////////////////////////////
WORD MICOGetFeFilter(PDEVICE_INFO pDevInfo)
{
	return BT829GetFeFilter(pDevInfo);
}

///////////////////////////////////////////////////////////////////////////////
BYTE MICOGetRelais(PDEVICE_INFO pDevInfo, WORD wExtCard)
{
	BYTE bRet = 0;
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	if (MICOCheckExtCard(pDevInfo, pHw->wExtCard))
		bRet = RELGetRelay(pDevInfo);

	return bRet;
}

///////////////////////////////////////////////////////////////////////////////
BYTE MICOSetRelais(PDEVICE_INFO pDevInfo, WORD wExtCard, BYTE bRelais)
{
	BYTE bOldRelais = 0;
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);
							 
	if (MICOCheckExtCard(pDevInfo, wExtCard))
	{
		bOldRelais = MICOGetRelais(pDevInfo, wExtCard);
		RELSetRelay(pDevInfo, bRelais);
	}

	return bOldRelais;
}

///////////////////////////////////////////////////////////////////////////////
BOOL MICOEncoderStart(PDEVICE_INFO pDevInfo)
{
	return ZRStartEncoding(pDevInfo);
}

///////////////////////////////////////////////////////////////////////////////
BOOL MICOEncoderStop(PDEVICE_INFO pDevInfo)
{
	return ZRStopEncoding(pDevInfo);
}

///////////////////////////////////////////////////////////////////////////////
WORD MICOEncoderSetFormat(PDEVICE_INFO pDevInfo, WORD wFormat)
{
	WORD wOldFormat = MICO_ERROR;
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	if (MICOCheckEncoderFormat(pDevInfo, wFormat))
	{
		wOldFormat = MICOEncoderGetFormat(pDevInfo);
		ZRSetFormat(pDevInfo, wFormat);
		// pHw->wFormat = wFormat;
	}
	return wOldFormat;
}

///////////////////////////////////////////////////////////////////////////////
WORD MICOEncoderGetFormat(PDEVICE_INFO pDevInfo)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	return  pHw->wFormat;
}

///////////////////////////////////////////////////////////////////////////////
WORD MICOEncoderSetFramerate(PDEVICE_INFO pDevInfo, WORD wFramerate)
{
	WORD wOldFramerate = MICO_ERROR;
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	if (MICOCheckEncoderFramerate(pDevInfo,wFramerate))
	{
		wOldFramerate = MICOEncoderGetFramerate(pDevInfo);
		ZRSetFramerate(pDevInfo,wFramerate);
		//		pHw->wFramerate = wFramerate;
	}

	return wOldFramerate;
}

///////////////////////////////////////////////////////////////////////////////
WORD MICOEncoderGetFramerate(PDEVICE_INFO pDevInfo)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	return pHw->wFramerate;
}

///////////////////////////////////////////////////////////////////////////////
DWORD MICOEncoderSetBPF(PDEVICE_INFO pDevInfo, DWORD dwBPF)
{
	DWORD	dwOldBPF	= MICO_ERROR;
	PHWINFO pHw			= VC_GetHWInfo(pDevInfo);


	if (MICOCheckEncoderBPF(pDevInfo,dwBPF))
	{
		dwOldBPF = MICOEncoderGetBPF(pDevInfo);
		ZRSetBPF(pDevInfo, dwBPF);
		//		pHw->dwBPF = dwBPF;
	}
	return dwOldBPF;
}

///////////////////////////////////////////////////////////////////////////////
DWORD MICOEncoderGetBPF(PDEVICE_INFO pDevInfo)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	return pHw->dwBPF;
}

///////////////////////////////////////////////////////////////////////////////
BOOL MICOEncoderSetMode(PDEVICE_INFO pDevInfo, WORD wFormat, WORD wFramerate, DWORD dwBPF)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	if (!MICOCheckEncoderFormat(pDevInfo,wFormat))
		return MICO_ERROR;

	if (!MICOCheckEncoderFramerate(pDevInfo,wFramerate))
		return MICO_ERROR;

	if (!MICOCheckEncoderBPF(pDevInfo,dwBPF))
		return MICO_ERROR;

	ZRSetFormat(pDevInfo,wFormat);
	ZRSetFramerate(pDevInfo,wFramerate);
	ZRSetBPF(pDevInfo,dwBPF);

//	pHw->wFormat = wFormat;
//	pHw->wFramerate = wFramerate;
//	pHw->dwBPF    = dwBPF;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
int MICOGetEncoderState(PDEVICE_INFO pDevInfo)
{
	return ZRGetEncoderState(pDevInfo);
}

///////////////////////////////////////////////////////////////////////////////
BOOL MICOCheckSource(PDEVICE_INFO pDevInfo, WORD wSource)
{
	switch(wSource)
	{
		case MICO_SOURCE0:
		case MICO_SOURCE1:
		case MICO_SOURCE2:
		case MICO_SOURCE3:
		case MICO_SOURCE4:
		case MICO_SOURCE5:
		case MICO_SOURCE6:
		case MICO_SOURCE7:
			return TRUE;
		default:
			return FALSE;
	}
}

///////////////////////////////////////////////////////////////////////////////
BOOL MICOCheckFeID(PDEVICE_INFO pDevInfo, WORD wFeID)
{
	switch(wFeID)
	{
		case MICO_FE0:
		case MICO_FE1:
			return TRUE;
		default:
			return FALSE;
	}
}

///////////////////////////////////////////////////////////////////////////////
BOOL MICOCheckExtCard(PDEVICE_INFO pDevInfo, WORD wExtCard)
{
	switch(wExtCard)
	{
		case MICO_EXTCARD0:
		case MICO_EXTCARD1:
		case MICO_EXTCARD2:
		case MICO_EXTCARD3:
			return TRUE;
		default:
			return FALSE;
	}
}

///////////////////////////////////////////////////////////////////////////////
BOOL MICOCheckSourceType(PDEVICE_INFO pDevInfo, WORD wSourceType)
{
	switch(wSourceType)
	{
		case MICO_FBAS:
		case MICO_SVHS:
			return TRUE;
		default:
			return FALSE;
	}
}

///////////////////////////////////////////////////////////////////////////////
BOOL MICOCheckFeVideoFormat(PDEVICE_INFO pDevInfo, WORD wFeVideoFormat)
{
	switch(wFeVideoFormat)
	{
		case MICO_PAL_SQUARE:
		case MICO_NTSC_SQUARE:
		case MICO_PAL_CCIR:
		case MICO_NTSC_CCIR:
			return TRUE;
		default:
			return FALSE;
	}
}

///////////////////////////////////////////////////////////////////////////////
BOOL MICOCheckBeVideoFormat(PDEVICE_INFO pDevInfo, WORD wBeVideoFormat)
{
	switch(wBeVideoFormat)
	{
		case MICO_PAL_SQUARE:
		case MICO_NTSC_SQUARE:
		case MICO_PAL_CCIR:
		case MICO_NTSC_CCIR:
			return (TRUE);
		default:
			return FALSE;
	}
}

///////////////////////////////////////////////////////////////////////////////
BOOL MICOCheckBeVideoType(PDEVICE_INFO pDevInfo, WORD wBeVideoType)
{
	switch(wBeVideoType)
	{
		case MICO_COMPOSITE:
		case MICO_RGB:
			return TRUE;
		default:
			return FALSE;
	}
}

///////////////////////////////////////////////////////////////////////////////
BOOL MICOCheckFeFilter(PDEVICE_INFO pDevInfo, WORD wFilter)
{
	if (wFilter <= (MICO_LNOTCH | MICO_LDEC))
		return TRUE;
	else
		return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
BOOL MICOCheckEncoderFormat(PDEVICE_INFO pDevInfo, WORD wFormat)
{
	switch(wFormat)
	{
		case MICO_ENCODER_HIGH:
		case MICO_ENCODER_LOW:
			return (TRUE);
		default:
			return FALSE;
	}
}

///////////////////////////////////////////////////////////////////////////////
BOOL MICOCheckEncoderFramerate(PDEVICE_INFO pDevInfo, WORD wFramerate)
{
	if ((wFramerate >= MICO_MIN_FPS) && (wFramerate <= MICO_MAX_FPS))
		return TRUE;
	else
		return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
BOOL MICOCheckEncoderBPF(PDEVICE_INFO pDevInfo, DWORD dwBPF)
{
	if ((dwBPF >= MICO_MIN_BPF) && (dwBPF <= MICO_MAX_BPF))
		return (TRUE);
	else
		return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
VOID MICOSaveRegistry(PDEVICE_INFO pDevInfo)
{
	VC_WriteProfile(pDevInfo, PARAM_TMO, 1000);
	VC_WriteProfile(pDevInfo, PARAM_EXC1, 0x240);
	VC_WriteProfile(pDevInfo, PARAM_EXC2, 0);
	VC_WriteProfile(pDevInfo, PARAM_EXC3, 0);
	VC_WriteProfile(pDevInfo, PARAM_BPF, 20000);
	VC_WriteProfile(pDevInfo, PARAM_FPS, 25);
	VC_WriteProfile(pDevInfo, PARAM_IGNREL4, 0);
	VC_WriteProfile(pDevInfo, PARAM_RES, 1);

	VC_WriteProfile(pDevInfo, PARAM_CAM_TYPE, 0);
	VC_WriteProfile(pDevInfo, PARAM_STORE_FIELD, EVEN_FIELD);

// Folgende Einstellungen werden zur Laufzeit in Abhängigkeit des Kameratyps gesetzt
//	VC_WriteProfile(pDevInfo, PARAM_SKP, 2);
//	VC_WriteProfile(pDevInfo, PARAM_HRD_SKP, 1);
//	VC_WriteProfile(pDevInfo, PARAM_SWITCH_DELAY, 1);
//	VC_WriteProfile(pDevInfo, PARAM_MIN_SIZE, 80);
//	VC_WriteProfile(pDevInfo, PARAM_MAX_SIZE, 140);

	VC_WriteProfile(pDevInfo, PARAM_BE_VF, MICO_PAL_CCIR);
	VC_WriteProfile(pDevInfo, PARAM_BE_VT, MICO_COMPOSITE);

	VC_WriteProfile(pDevInfo, PARAM_FE_VF, MICO_PAL_CCIR);
	VC_WriteProfile(pDevInfo, PARAM_FE_BR, 137);
	VC_WriteProfile(pDevInfo, PARAM_FE_SAT, 290);
	VC_WriteProfile(pDevInfo, PARAM_FE_CONT, 285);
	VC_WriteProfile(pDevInfo, PARAM_FE_HUE, 128);

	VC_WriteProfile(pDevInfo, PARAM_S00_AVPY, 0);
	VC_WriteProfile(pDevInfo, PARAM_S00_AVPC, 0);
	VC_WriteProfile(pDevInfo, PARAM_S00_ST, MICO_FBAS);
	VC_WriteProfile(pDevInfo, PARAM_S00_NF, MICO_NOTCH_FILTER_AUTO);

	VC_WriteProfile(pDevInfo, PARAM_S01_AVPY, 1);
	VC_WriteProfile(pDevInfo, PARAM_S01_AVPC, 1);
	VC_WriteProfile(pDevInfo, PARAM_S01_ST, MICO_FBAS);
	VC_WriteProfile(pDevInfo, PARAM_S01_NF, MICO_NOTCH_FILTER_AUTO);

	VC_WriteProfile(pDevInfo, PARAM_S02_AVPY, 0);
	VC_WriteProfile(pDevInfo, PARAM_S02_AVPC, 0);
	VC_WriteProfile(pDevInfo, PARAM_S02_ST, MICO_FBAS);
	VC_WriteProfile(pDevInfo, PARAM_S02_NF, MICO_NOTCH_FILTER_AUTO);

	VC_WriteProfile(pDevInfo, PARAM_S03_AVPY, 1);
	VC_WriteProfile(pDevInfo, PARAM_S03_AVPC, 1);
	VC_WriteProfile(pDevInfo, PARAM_S03_ST, MICO_FBAS);
	VC_WriteProfile(pDevInfo, PARAM_S03_NF, MICO_NOTCH_FILTER_AUTO);

	VC_WriteProfile(pDevInfo, PARAM_S04_AVPY, 0);
	VC_WriteProfile(pDevInfo, PARAM_S04_AVPC, 0);
	VC_WriteProfile(pDevInfo, PARAM_S04_ST, MICO_FBAS);
	VC_WriteProfile(pDevInfo, PARAM_S04_NF, MICO_NOTCH_FILTER_AUTO);

	VC_WriteProfile(pDevInfo, PARAM_S05_AVPY, 1);
	VC_WriteProfile(pDevInfo, PARAM_S05_AVPC, 1);
	VC_WriteProfile(pDevInfo, PARAM_S05_ST, MICO_FBAS);
	VC_WriteProfile(pDevInfo, PARAM_S05_NF, MICO_NOTCH_FILTER_AUTO);

	VC_WriteProfile(pDevInfo, PARAM_S06_AVPY, 0);
	VC_WriteProfile(pDevInfo, PARAM_S06_AVPC, 0);
	VC_WriteProfile(pDevInfo, PARAM_S06_ST, MICO_FBAS);
	VC_WriteProfile(pDevInfo, PARAM_S06_NF, MICO_NOTCH_FILTER_AUTO);

	VC_WriteProfile(pDevInfo, PARAM_S07_AVPY, 1);
	VC_WriteProfile(pDevInfo, PARAM_S07_AVPC, 1);
	VC_WriteProfile(pDevInfo, PARAM_S07_ST, MICO_FBAS);
	VC_WriteProfile(pDevInfo, PARAM_S07_NF, MICO_NOTCH_FILTER_AUTO);

	VC_WriteProfile(pDevInfo, PARAM_S10_AVPY, 0);
	VC_WriteProfile(pDevInfo, PARAM_S10_AVPC, 0);
	VC_WriteProfile(pDevInfo, PARAM_S10_ST, MICO_FBAS);
	VC_WriteProfile(pDevInfo, PARAM_S10_NF, MICO_NOTCH_FILTER_AUTO);

	VC_WriteProfile(pDevInfo, PARAM_S11_AVPY, 1);
	VC_WriteProfile(pDevInfo, PARAM_S11_AVPC, 1);
	VC_WriteProfile(pDevInfo, PARAM_S11_ST, MICO_FBAS);
	VC_WriteProfile(pDevInfo, PARAM_S11_NF, MICO_NOTCH_FILTER_AUTO);

	VC_WriteProfile(pDevInfo, PARAM_S12_AVPY, 0);
	VC_WriteProfile(pDevInfo, PARAM_S12_AVPC, 0);
	VC_WriteProfile(pDevInfo, PARAM_S12_ST, MICO_FBAS);
	VC_WriteProfile(pDevInfo, PARAM_S12_NF, MICO_NOTCH_FILTER_AUTO);

	VC_WriteProfile(pDevInfo, PARAM_S13_AVPY, 1);
	VC_WriteProfile(pDevInfo, PARAM_S13_AVPC, 1);
	VC_WriteProfile(pDevInfo, PARAM_S13_ST, MICO_FBAS);
	VC_WriteProfile(pDevInfo, PARAM_S13_NF, MICO_NOTCH_FILTER_AUTO);

	VC_WriteProfile(pDevInfo, PARAM_S14_AVPY, 0);
	VC_WriteProfile(pDevInfo, PARAM_S14_AVPC, 0);
	VC_WriteProfile(pDevInfo, PARAM_S14_ST, MICO_FBAS);
	VC_WriteProfile(pDevInfo, PARAM_S14_NF, MICO_NOTCH_FILTER_AUTO);

	VC_WriteProfile(pDevInfo, PARAM_S15_AVPY, 1 );
	VC_WriteProfile(pDevInfo, PARAM_S15_AVPC, 1);
	VC_WriteProfile(pDevInfo, PARAM_S15_ST, MICO_FBAS);
	VC_WriteProfile(pDevInfo, PARAM_S15_NF, MICO_NOTCH_FILTER_AUTO);

	VC_WriteProfile(pDevInfo, PARAM_S16_AVPY, 0);
	VC_WriteProfile(pDevInfo, PARAM_S16_AVPC, 0);
	VC_WriteProfile(pDevInfo, PARAM_S16_ST, MICO_FBAS);
	VC_WriteProfile(pDevInfo, PARAM_S16_NF, MICO_NOTCH_FILTER_AUTO);

	VC_WriteProfile(pDevInfo, PARAM_S17_AVPY, 1);
	VC_WriteProfile(pDevInfo, PARAM_S17_AVPC, 1);
	VC_WriteProfile(pDevInfo, PARAM_S17_ST, MICO_FBAS);
	VC_WriteProfile(pDevInfo, PARAM_S17_NF, MICO_NOTCH_FILTER_AUTO);

	VC_WriteProfile(pDevInfo, PARAM_S20_AVPY, 0);
	VC_WriteProfile(pDevInfo, PARAM_S20_AVPC, 0);
	VC_WriteProfile(pDevInfo, PARAM_S20_ST, MICO_FBAS);
	VC_WriteProfile(pDevInfo, PARAM_S20_NF, MICO_NOTCH_FILTER_AUTO);

	VC_WriteProfile(pDevInfo, PARAM_S21_AVPY, 1);
	VC_WriteProfile(pDevInfo, PARAM_S21_AVPC, 1);
	VC_WriteProfile(pDevInfo, PARAM_S21_ST, MICO_FBAS);
	VC_WriteProfile(pDevInfo, PARAM_S21_NF, MICO_NOTCH_FILTER_AUTO);

	VC_WriteProfile(pDevInfo, PARAM_S22_AVPY, 0);
	VC_WriteProfile(pDevInfo, PARAM_S22_AVPC, 0);
	VC_WriteProfile(pDevInfo, PARAM_S22_ST, MICO_FBAS);
	VC_WriteProfile(pDevInfo, PARAM_S22_NF, MICO_NOTCH_FILTER_AUTO);

	VC_WriteProfile(pDevInfo, PARAM_S23_AVPY, 1);
	VC_WriteProfile(pDevInfo, PARAM_S23_AVPC, 1);
	VC_WriteProfile(pDevInfo, PARAM_S23_ST, MICO_FBAS);
	VC_WriteProfile(pDevInfo, PARAM_S23_NF, MICO_NOTCH_FILTER_AUTO);

	VC_WriteProfile(pDevInfo, PARAM_S24_AVPY, 0);
	VC_WriteProfile(pDevInfo, PARAM_S24_AVPC, 0);
	VC_WriteProfile(pDevInfo, PARAM_S24_ST, MICO_FBAS);
	VC_WriteProfile(pDevInfo, PARAM_S24_NF, MICO_NOTCH_FILTER_AUTO);

	VC_WriteProfile(pDevInfo, PARAM_S25_AVPY, 1 );
	VC_WriteProfile(pDevInfo, PARAM_S25_AVPC, 1);
	VC_WriteProfile(pDevInfo, PARAM_S25_ST, MICO_FBAS);
	VC_WriteProfile(pDevInfo, PARAM_S25_NF, MICO_NOTCH_FILTER_AUTO);

	VC_WriteProfile(pDevInfo, PARAM_S26_AVPY, 0);
	VC_WriteProfile(pDevInfo, PARAM_S26_AVPC, 0);
	VC_WriteProfile(pDevInfo, PARAM_S26_ST, MICO_FBAS);
	VC_WriteProfile(pDevInfo, PARAM_S26_NF, MICO_NOTCH_FILTER_AUTO);

	VC_WriteProfile(pDevInfo, PARAM_S27_AVPY, 1);
	VC_WriteProfile(pDevInfo, PARAM_S27_AVPC, 1);
	VC_WriteProfile(pDevInfo, PARAM_S27_ST, MICO_FBAS);
	VC_WriteProfile(pDevInfo, PARAM_S27_NF, MICO_NOTCH_FILTER_AUTO);

	VC_WriteProfile(pDevInfo, PARAM_S30_AVPY, 0);
	VC_WriteProfile(pDevInfo, PARAM_S30_AVPC, 0);
	VC_WriteProfile(pDevInfo, PARAM_S30_ST, MICO_FBAS);
	VC_WriteProfile(pDevInfo, PARAM_S30_NF, MICO_NOTCH_FILTER_AUTO);

	VC_WriteProfile(pDevInfo, PARAM_S31_AVPY, 1);
	VC_WriteProfile(pDevInfo, PARAM_S31_AVPC, 1);
	VC_WriteProfile(pDevInfo, PARAM_S31_ST, MICO_FBAS);
	VC_WriteProfile(pDevInfo, PARAM_S31_NF, MICO_NOTCH_FILTER_AUTO);

	VC_WriteProfile(pDevInfo, PARAM_S32_AVPY, 0);
	VC_WriteProfile(pDevInfo, PARAM_S32_AVPC, 0);
	VC_WriteProfile(pDevInfo, PARAM_S32_ST, MICO_FBAS);
	VC_WriteProfile(pDevInfo, PARAM_S32_NF, MICO_NOTCH_FILTER_AUTO);

	VC_WriteProfile(pDevInfo, PARAM_S33_AVPY, 1);
	VC_WriteProfile(pDevInfo, PARAM_S33_AVPC, 1);
	VC_WriteProfile(pDevInfo, PARAM_S33_ST, MICO_FBAS);
	VC_WriteProfile(pDevInfo, PARAM_S33_NF, MICO_NOTCH_FILTER_AUTO);

	VC_WriteProfile(pDevInfo, PARAM_S34_AVPY, 0);
	VC_WriteProfile(pDevInfo, PARAM_S34_AVPC, 0);
	VC_WriteProfile(pDevInfo, PARAM_S34_ST, MICO_FBAS);
	VC_WriteProfile(pDevInfo, PARAM_S34_NF, MICO_NOTCH_FILTER_AUTO);

	VC_WriteProfile(pDevInfo, PARAM_S35_AVPY, 1 );
	VC_WriteProfile(pDevInfo, PARAM_S35_AVPC, 1);
	VC_WriteProfile(pDevInfo, PARAM_S35_ST, MICO_FBAS);
	VC_WriteProfile(pDevInfo, PARAM_S35_NF, MICO_NOTCH_FILTER_AUTO);

	VC_WriteProfile(pDevInfo, PARAM_S36_AVPY, 0);
	VC_WriteProfile(pDevInfo, PARAM_S36_AVPC, 0);
	VC_WriteProfile(pDevInfo, PARAM_S36_ST, MICO_FBAS);
	VC_WriteProfile(pDevInfo, PARAM_S36_NF, MICO_NOTCH_FILTER_AUTO);

	VC_WriteProfile(pDevInfo, PARAM_S37_AVPY, 1);
	VC_WriteProfile(pDevInfo, PARAM_S37_AVPC, 1);
	VC_WriteProfile(pDevInfo, PARAM_S37_ST, MICO_FBAS);
	VC_WriteProfile(pDevInfo, PARAM_S37_NF, MICO_NOTCH_FILTER_AUTO);

	VC_WriteProfile(pDevInfo, PARAM_T58PC_FeHStart, 8);
	VC_WriteProfile(pDevInfo, PARAM_T58PC_FeHEnd, 712);
	VC_WriteProfile(pDevInfo, PARAM_T58PC_FeHTotal, 864);
	VC_WriteProfile(pDevInfo, PARAM_T58PC_FeHSStart, 732);
	VC_WriteProfile(pDevInfo, PARAM_T58PC_FeHSEnd, 784);
	VC_WriteProfile(pDevInfo, PARAM_T58PC_FeBlankStart, 720);
	VC_WriteProfile(pDevInfo, PARAM_T58PC_FeVStart, 17);
	VC_WriteProfile(pDevInfo, PARAM_T58PC_FeVEnd, 297);
	VC_WriteProfile(pDevInfo, PARAM_T58PC_FeVTotal, 625);
	VC_WriteProfile(pDevInfo, PARAM_T58PC_FeNMCU, 43); 

	VC_WriteProfile(pDevInfo, PARAM_T58PS_FeHStart, 8);
	VC_WriteProfile(pDevInfo, PARAM_T58PS_FeHEnd, 744);
	VC_WriteProfile(pDevInfo, PARAM_T58PS_FeHTotal, 928);
	VC_WriteProfile(pDevInfo, PARAM_T58PS_FeHSStart, 766);
	VC_WriteProfile(pDevInfo, PARAM_T58PS_FeHSEnd, 820);
	VC_WriteProfile(pDevInfo, PARAM_T58PS_FeBlankStart, 752);
	VC_WriteProfile(pDevInfo, PARAM_T58PS_FeVStart, 16);
	VC_WriteProfile(pDevInfo, PARAM_T58PS_FeVEnd, 304);
	VC_WriteProfile(pDevInfo, PARAM_T58PS_FeVTotal, 625);
	VC_WriteProfile(pDevInfo, PARAM_T58PS_FeNMCU, 45);

	VC_WriteProfile(pDevInfo, PARAM_T58NC_FeHStart, 8);
	VC_WriteProfile(pDevInfo, PARAM_T58NC_FeHEnd, 712);
	VC_WriteProfile(pDevInfo, PARAM_T58NC_FeHTotal, 858);
	VC_WriteProfile(pDevInfo, PARAM_T58NC_FeHSStart, 736);
	VC_WriteProfile(pDevInfo, PARAM_T58NC_FeHSEnd, 784);
	VC_WriteProfile(pDevInfo, PARAM_T58NC_FeBlankStart, 720);
	VC_WriteProfile(pDevInfo, PARAM_T58NC_FeVStart, 14);
	VC_WriteProfile(pDevInfo, PARAM_T58NC_FeVEnd, 254);
	VC_WriteProfile(pDevInfo, PARAM_T58NC_FeVTotal, 525);
	VC_WriteProfile(pDevInfo, PARAM_T58NC_FeNMCU, 43);

	VC_WriteProfile(pDevInfo, PARAM_T58NS_FeHStart, 0);
	VC_WriteProfile(pDevInfo, PARAM_T58NS_FeHEnd, 640);
	VC_WriteProfile(pDevInfo, PARAM_T58NS_FeHTotal, 780);
	VC_WriteProfile(pDevInfo, PARAM_T58NS_FeHSStart, 658);
	VC_WriteProfile(pDevInfo, PARAM_T58NS_FeHSEnd, 698);
	VC_WriteProfile(pDevInfo, PARAM_T58NS_FeBlankStart, 640);
	VC_WriteProfile(pDevInfo, PARAM_T58NS_FeVStart, 14);
	VC_WriteProfile(pDevInfo, PARAM_T58NS_FeVEnd, 254);
	VC_WriteProfile(pDevInfo, PARAM_T58NS_FeVTotal, 525);
	VC_WriteProfile(pDevInfo, PARAM_T58NS_FeNMCU, 39);

	VC_WriteProfile(pDevInfo, PARAM_T59PS_FeHStart, 0);
	VC_WriteProfile(pDevInfo, PARAM_T59PS_FeHEnd, 768);
	VC_WriteProfile(pDevInfo, PARAM_T59PS_FeHTotal, 944);
	VC_WriteProfile(pDevInfo, PARAM_T59PS_FeHSStart, 782);
	VC_WriteProfile(pDevInfo, PARAM_T59PS_FeHSEnd, 838);
	VC_WriteProfile(pDevInfo, PARAM_T59PS_FeBlankStart, 768);
	VC_WriteProfile(pDevInfo, PARAM_T59PS_FeVStart, 16);
	VC_WriteProfile(pDevInfo, PARAM_T59PS_FeVEnd, 304);
	VC_WriteProfile(pDevInfo, PARAM_T59PS_FeVTotal, 625);
	VC_WriteProfile(pDevInfo, PARAM_T59PS_FeNMCU, 47);

	VC_WriteProfile(pDevInfo, PARAM_T59NS_FeHStart, 0);
	VC_WriteProfile(pDevInfo, PARAM_T59NS_FeHEnd, 640);
	VC_WriteProfile(pDevInfo, PARAM_T59NS_FeHTotal, 780);
	VC_WriteProfile(pDevInfo, PARAM_T59NS_FeHSStart, 658);
	VC_WriteProfile(pDevInfo, PARAM_T59NS_FeHSEnd, 698);
	VC_WriteProfile(pDevInfo, PARAM_T59NS_FeBlankStart, 640);
	VC_WriteProfile(pDevInfo, PARAM_T59NS_FeVStart, 14);
	VC_WriteProfile(pDevInfo, PARAM_T59NS_FeVEnd, 254);
	VC_WriteProfile(pDevInfo, PARAM_T59NS_FeVTotal, 525);
	VC_WriteProfile(pDevInfo, PARAM_T59NS_FeNMCU, 39);
}

///////////////////////////////////////////////////////////////////////////////
VOID MICOLoadSources(PDEVICE_INFO pDevInfo)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	dprintf1(("Source0-0"));
	pHw->MiCoISA.ExtCard[0].Source[0].nAVPortY = VC_ReadProfile(pDevInfo, PARAM_S00_AVPY, 0);
	pHw->MiCoISA.ExtCard[0].Source[0].nAVPortC = VC_ReadProfile(pDevInfo, PARAM_S00_AVPC, 0);
	pHw->MiCoISA.ExtCard[0].Source[0].wFeSourceType = (WORD) VC_ReadProfile(pDevInfo, PARAM_S00_ST, MICO_FBAS);
	pHw->MiCoISA.ExtCard[0].Source[0].wFeFilter = (WORD) VC_ReadProfile(pDevInfo, PARAM_S00_NF, MICO_NOTCH_FILTER_AUTO);
	dprintf1(("Source0-1"));
	pHw->MiCoISA.ExtCard[0].Source[1].nAVPortY = VC_ReadProfile(pDevInfo, PARAM_S01_AVPY, 0);
	pHw->MiCoISA.ExtCard[0].Source[1].nAVPortC = VC_ReadProfile(pDevInfo, PARAM_S01_AVPC, 0);
	pHw->MiCoISA.ExtCard[0].Source[1].wFeSourceType = (WORD) VC_ReadProfile(pDevInfo, PARAM_S01_ST, MICO_FBAS);
	pHw->MiCoISA.ExtCard[0].Source[1].wFeFilter = (WORD) VC_ReadProfile(pDevInfo, PARAM_S01_NF, MICO_NOTCH_FILTER_AUTO);
	dprintf1(("Source0-2"));
	pHw->MiCoISA.ExtCard[0].Source[2].nAVPortY = VC_ReadProfile(pDevInfo, PARAM_S02_AVPY, 0);
	pHw->MiCoISA.ExtCard[0].Source[2].nAVPortC = VC_ReadProfile(pDevInfo, PARAM_S02_AVPC, 0);
	pHw->MiCoISA.ExtCard[0].Source[2].wFeSourceType = (WORD) VC_ReadProfile(pDevInfo, PARAM_S02_ST, MICO_FBAS);
	pHw->MiCoISA.ExtCard[0].Source[2].wFeFilter = (WORD) VC_ReadProfile(pDevInfo, PARAM_S02_NF, MICO_NOTCH_FILTER_AUTO);
	dprintf1(("Source0-3"));
	pHw->MiCoISA.ExtCard[0].Source[3].nAVPortY = VC_ReadProfile(pDevInfo, PARAM_S03_AVPY, 0);
	pHw->MiCoISA.ExtCard[0].Source[3].nAVPortC = VC_ReadProfile(pDevInfo, PARAM_S03_AVPC, 0);
	pHw->MiCoISA.ExtCard[0].Source[3].wFeSourceType = (WORD) VC_ReadProfile(pDevInfo, PARAM_S03_ST, MICO_FBAS);
	pHw->MiCoISA.ExtCard[0].Source[3].wFeFilter = (WORD) VC_ReadProfile(pDevInfo, PARAM_S03_NF, MICO_NOTCH_FILTER_AUTO);
	dprintf1(("Source0-4"));
	pHw->MiCoISA.ExtCard[0].Source[4].nAVPortY = VC_ReadProfile(pDevInfo, PARAM_S04_AVPY, 0);
	pHw->MiCoISA.ExtCard[0].Source[4].nAVPortC = VC_ReadProfile(pDevInfo, PARAM_S04_AVPC, 0);
	pHw->MiCoISA.ExtCard[0].Source[4].wFeSourceType = (WORD) VC_ReadProfile(pDevInfo, PARAM_S04_ST, MICO_FBAS);
	pHw->MiCoISA.ExtCard[0].Source[4].wFeFilter = (WORD) VC_ReadProfile(pDevInfo, PARAM_S04_NF, MICO_NOTCH_FILTER_AUTO);
	dprintf1(("Source0-5"));
	pHw->MiCoISA.ExtCard[0].Source[5].nAVPortY = VC_ReadProfile(pDevInfo, PARAM_S05_AVPY, 0);
	pHw->MiCoISA.ExtCard[0].Source[5].nAVPortC = VC_ReadProfile(pDevInfo, PARAM_S05_AVPC, 0);
	pHw->MiCoISA.ExtCard[0].Source[5].wFeSourceType = (WORD) VC_ReadProfile(pDevInfo, PARAM_S05_ST, MICO_FBAS);
	pHw->MiCoISA.ExtCard[0].Source[5].wFeFilter = (WORD) VC_ReadProfile(pDevInfo, PARAM_S05_NF, MICO_NOTCH_FILTER_AUTO);
	dprintf1(("Source0-6"));
	pHw->MiCoISA.ExtCard[0].Source[6].nAVPortY = VC_ReadProfile(pDevInfo, PARAM_S06_AVPY, 0);
	pHw->MiCoISA.ExtCard[0].Source[6].nAVPortC = VC_ReadProfile(pDevInfo, PARAM_S06_AVPC, 0);
	pHw->MiCoISA.ExtCard[0].Source[6].wFeSourceType = (WORD) VC_ReadProfile(pDevInfo, PARAM_S06_ST, MICO_FBAS);
	pHw->MiCoISA.ExtCard[0].Source[6].wFeFilter = (WORD) VC_ReadProfile(pDevInfo, PARAM_S06_NF, MICO_NOTCH_FILTER_AUTO);
	dprintf1(("Source0-7"));
	pHw->MiCoISA.ExtCard[0].Source[7].nAVPortY = VC_ReadProfile(pDevInfo, PARAM_S07_AVPY, 0);
	pHw->MiCoISA.ExtCard[0].Source[7].nAVPortC = VC_ReadProfile(pDevInfo, PARAM_S07_AVPC, 0);
	pHw->MiCoISA.ExtCard[0].Source[7].wFeSourceType = (WORD) VC_ReadProfile(pDevInfo, PARAM_S07_ST, MICO_FBAS);
	pHw->MiCoISA.ExtCard[0].Source[7].wFeFilter = (WORD) VC_ReadProfile(pDevInfo, PARAM_S07_NF, MICO_NOTCH_FILTER_AUTO);


	if (TSTBIT(MICOGetHardwareState(pDevInfo), 1))
	{
		dprintf1(("Source1-0"));
		pHw->MiCoISA.ExtCard[1].Source[0].nAVPortY = VC_ReadProfile(pDevInfo, PARAM_S10_AVPY, 0);
		pHw->MiCoISA.ExtCard[1].Source[0].nAVPortC = VC_ReadProfile(pDevInfo, PARAM_S10_AVPC, 0);
		pHw->MiCoISA.ExtCard[1].Source[0].wFeSourceType = (WORD) VC_ReadProfile(pDevInfo, PARAM_S10_ST, MICO_FBAS);
		pHw->MiCoISA.ExtCard[1].Source[0].wFeFilter = (WORD) VC_ReadProfile(pDevInfo, PARAM_S10_NF, MICO_NOTCH_FILTER_AUTO);
		dprintf1(("Source1-1"));
		pHw->MiCoISA.ExtCard[1].Source[1].nAVPortY = VC_ReadProfile(pDevInfo, PARAM_S11_AVPY, 0);
		pHw->MiCoISA.ExtCard[1].Source[1].nAVPortC = VC_ReadProfile(pDevInfo, PARAM_S11_AVPC, 0);
		pHw->MiCoISA.ExtCard[1].Source[1].wFeSourceType = (WORD) VC_ReadProfile(pDevInfo, PARAM_S11_ST, MICO_FBAS);
		pHw->MiCoISA.ExtCard[1].Source[1].wFeFilter = (WORD) VC_ReadProfile(pDevInfo, PARAM_S11_NF, MICO_NOTCH_FILTER_AUTO);
		dprintf1(("Source1-2"));
		pHw->MiCoISA.ExtCard[1].Source[2].nAVPortY = VC_ReadProfile(pDevInfo, PARAM_S12_AVPY, 0);
		pHw->MiCoISA.ExtCard[1].Source[2].nAVPortC = VC_ReadProfile(pDevInfo, PARAM_S12_AVPC, 0);
		pHw->MiCoISA.ExtCard[1].Source[2].wFeSourceType = (WORD) VC_ReadProfile(pDevInfo, PARAM_S12_ST, MICO_FBAS);
		pHw->MiCoISA.ExtCard[1].Source[2].wFeFilter = (WORD) VC_ReadProfile(pDevInfo, PARAM_S12_NF, MICO_NOTCH_FILTER_AUTO);
		dprintf1(("Source1-3"));
		pHw->MiCoISA.ExtCard[1].Source[3].nAVPortY = VC_ReadProfile(pDevInfo, PARAM_S13_AVPY, 0);
		pHw->MiCoISA.ExtCard[1].Source[3].nAVPortC = VC_ReadProfile(pDevInfo, PARAM_S13_AVPC, 0);
		pHw->MiCoISA.ExtCard[1].Source[3].wFeSourceType = (WORD) VC_ReadProfile(pDevInfo, PARAM_S13_ST, MICO_FBAS);
		pHw->MiCoISA.ExtCard[1].Source[3].wFeFilter = (WORD) VC_ReadProfile(pDevInfo, PARAM_S13_NF, MICO_NOTCH_FILTER_AUTO);
		dprintf1(("Source1-4"));
		pHw->MiCoISA.ExtCard[1].Source[4].nAVPortY = VC_ReadProfile(pDevInfo, PARAM_S14_AVPY, 0);
		pHw->MiCoISA.ExtCard[1].Source[4].nAVPortC = VC_ReadProfile(pDevInfo, PARAM_S14_AVPC, 0);
		pHw->MiCoISA.ExtCard[1].Source[4].wFeSourceType = (WORD) VC_ReadProfile(pDevInfo, PARAM_S14_ST, MICO_FBAS);
		pHw->MiCoISA.ExtCard[1].Source[4].wFeFilter = (WORD) VC_ReadProfile(pDevInfo, PARAM_S14_NF, MICO_NOTCH_FILTER_AUTO);
		dprintf1(("Source1-5"));
		pHw->MiCoISA.ExtCard[1].Source[5].nAVPortY = VC_ReadProfile(pDevInfo, PARAM_S15_AVPY, 0);
		pHw->MiCoISA.ExtCard[1].Source[5].nAVPortC = VC_ReadProfile(pDevInfo, PARAM_S15_AVPC, 0);
		pHw->MiCoISA.ExtCard[1].Source[5].wFeSourceType = (WORD) VC_ReadProfile(pDevInfo, PARAM_S15_ST, MICO_FBAS);
		pHw->MiCoISA.ExtCard[1].Source[5].wFeFilter = (WORD) VC_ReadProfile(pDevInfo, PARAM_S15_NF, MICO_NOTCH_FILTER_AUTO);
		dprintf1(("Source1-6"));
		pHw->MiCoISA.ExtCard[1].Source[6].nAVPortY = VC_ReadProfile(pDevInfo, PARAM_S16_AVPY, 0);
		pHw->MiCoISA.ExtCard[1].Source[6].nAVPortC = VC_ReadProfile(pDevInfo, PARAM_S16_AVPC, 0);
		pHw->MiCoISA.ExtCard[1].Source[6].wFeSourceType = (WORD) VC_ReadProfile(pDevInfo, PARAM_S16_ST, MICO_FBAS);
		pHw->MiCoISA.ExtCard[1].Source[6].wFeFilter = (WORD) VC_ReadProfile(pDevInfo, PARAM_S16_NF, MICO_NOTCH_FILTER_AUTO);
		dprintf1(("Source1-7"));
		pHw->MiCoISA.ExtCard[1].Source[7].nAVPortY = VC_ReadProfile(pDevInfo, PARAM_S17_AVPY, 0);
		pHw->MiCoISA.ExtCard[1].Source[7].nAVPortC = VC_ReadProfile(pDevInfo, PARAM_S17_AVPC, 0);
		pHw->MiCoISA.ExtCard[1].Source[7].wFeSourceType = (WORD) VC_ReadProfile(pDevInfo, PARAM_S17_ST, MICO_FBAS);
		pHw->MiCoISA.ExtCard[1].Source[7].wFeFilter = (WORD) VC_ReadProfile(pDevInfo, PARAM_S17_NF, MICO_NOTCH_FILTER_AUTO);
	}

	if (TSTBIT(MICOGetHardwareState(pDevInfo), 2))
	{
		dprintf1(("Source2-0"));
		pHw->MiCoISA.ExtCard[2].Source[0].nAVPortY = VC_ReadProfile(pDevInfo, PARAM_S20_AVPY, 0);
		pHw->MiCoISA.ExtCard[2].Source[0].nAVPortC = VC_ReadProfile(pDevInfo, PARAM_S20_AVPC, 0);
		pHw->MiCoISA.ExtCard[2].Source[0].wFeSourceType = (WORD) VC_ReadProfile(pDevInfo, PARAM_S20_ST, MICO_FBAS);
		pHw->MiCoISA.ExtCard[2].Source[0].wFeFilter = (WORD) VC_ReadProfile(pDevInfo, PARAM_S20_NF, MICO_NOTCH_FILTER_AUTO);
		dprintf1(("Source2-1"));
		pHw->MiCoISA.ExtCard[2].Source[1].nAVPortY = VC_ReadProfile(pDevInfo, PARAM_S21_AVPY, 0);
		pHw->MiCoISA.ExtCard[2].Source[1].nAVPortC = VC_ReadProfile(pDevInfo, PARAM_S21_AVPC, 0);
		pHw->MiCoISA.ExtCard[2].Source[1].wFeSourceType = (WORD) VC_ReadProfile(pDevInfo, PARAM_S21_ST, MICO_FBAS);
		pHw->MiCoISA.ExtCard[2].Source[1].wFeFilter = (WORD) VC_ReadProfile(pDevInfo, PARAM_S21_NF, MICO_NOTCH_FILTER_AUTO);
		dprintf1(("Source2-2"));
		pHw->MiCoISA.ExtCard[2].Source[2].nAVPortY = VC_ReadProfile(pDevInfo, PARAM_S22_AVPY, 0);
		pHw->MiCoISA.ExtCard[2].Source[2].nAVPortC = VC_ReadProfile(pDevInfo, PARAM_S22_AVPC, 0);
		pHw->MiCoISA.ExtCard[2].Source[2].wFeSourceType = (WORD) VC_ReadProfile(pDevInfo, PARAM_S22_ST, MICO_FBAS);
		pHw->MiCoISA.ExtCard[2].Source[2].wFeFilter = (WORD) VC_ReadProfile(pDevInfo, PARAM_S22_NF, MICO_NOTCH_FILTER_AUTO);
		dprintf1(("Source2-3"));
		pHw->MiCoISA.ExtCard[2].Source[3].nAVPortY = VC_ReadProfile(pDevInfo, PARAM_S23_AVPY, 0);
		pHw->MiCoISA.ExtCard[2].Source[3].nAVPortC = VC_ReadProfile(pDevInfo, PARAM_S23_AVPC, 0);
		pHw->MiCoISA.ExtCard[2].Source[3].wFeSourceType = (WORD) VC_ReadProfile(pDevInfo, PARAM_S23_ST, MICO_FBAS);
		pHw->MiCoISA.ExtCard[2].Source[3].wFeFilter = (WORD) VC_ReadProfile(pDevInfo, PARAM_S23_NF, MICO_NOTCH_FILTER_AUTO);
		dprintf1(("Source2-4"));
		pHw->MiCoISA.ExtCard[2].Source[4].nAVPortY = VC_ReadProfile(pDevInfo, PARAM_S24_AVPY, 0);
		pHw->MiCoISA.ExtCard[2].Source[4].nAVPortC = VC_ReadProfile(pDevInfo, PARAM_S24_AVPC, 0);
		pHw->MiCoISA.ExtCard[2].Source[4].wFeSourceType = (WORD) VC_ReadProfile(pDevInfo, PARAM_S24_ST, MICO_FBAS);
		pHw->MiCoISA.ExtCard[2].Source[4].wFeFilter = (WORD) VC_ReadProfile(pDevInfo, PARAM_S24_NF, MICO_NOTCH_FILTER_AUTO);
		dprintf1(("Source2-5"));
		pHw->MiCoISA.ExtCard[2].Source[5].nAVPortY = VC_ReadProfile(pDevInfo, PARAM_S25_AVPY, 0);
		pHw->MiCoISA.ExtCard[2].Source[5].nAVPortC = VC_ReadProfile(pDevInfo, PARAM_S25_AVPC, 0);
		pHw->MiCoISA.ExtCard[2].Source[5].wFeSourceType = (WORD) VC_ReadProfile(pDevInfo, PARAM_S25_ST, MICO_FBAS);
		pHw->MiCoISA.ExtCard[2].Source[5].wFeFilter = (WORD) VC_ReadProfile(pDevInfo, PARAM_S25_NF, MICO_NOTCH_FILTER_AUTO);
		dprintf1(("Source2-6"));
		pHw->MiCoISA.ExtCard[2].Source[6].nAVPortY = VC_ReadProfile(pDevInfo, PARAM_S26_AVPY, 0);
		pHw->MiCoISA.ExtCard[2].Source[6].nAVPortC = VC_ReadProfile(pDevInfo, PARAM_S26_AVPC, 0);
		pHw->MiCoISA.ExtCard[2].Source[6].wFeSourceType = (WORD) VC_ReadProfile(pDevInfo, PARAM_S26_ST, MICO_FBAS);
		pHw->MiCoISA.ExtCard[2].Source[6].wFeFilter = (WORD) VC_ReadProfile(pDevInfo, PARAM_S26_NF, MICO_NOTCH_FILTER_AUTO);
		dprintf1(("Source2-7"));
		pHw->MiCoISA.ExtCard[2].Source[7].nAVPortY = VC_ReadProfile(pDevInfo, PARAM_S27_AVPY, 0);
		pHw->MiCoISA.ExtCard[2].Source[7].nAVPortC = VC_ReadProfile(pDevInfo, PARAM_S27_AVPC, 0);
		pHw->MiCoISA.ExtCard[2].Source[7].wFeSourceType = (WORD) VC_ReadProfile(pDevInfo, PARAM_S27_ST, MICO_FBAS);
		pHw->MiCoISA.ExtCard[2].Source[7].wFeFilter = (WORD) VC_ReadProfile(pDevInfo, PARAM_S27_NF, MICO_NOTCH_FILTER_AUTO);
	}

	if (TSTBIT(MICOGetHardwareState(pDevInfo), 3))
	{
		dprintf1(("Source3-0"));
		pHw->MiCoISA.ExtCard[3].Source[0].nAVPortY = VC_ReadProfile(pDevInfo, PARAM_S30_AVPY, 0);
		pHw->MiCoISA.ExtCard[3].Source[0].nAVPortC = VC_ReadProfile(pDevInfo, PARAM_S30_AVPC, 0);
		pHw->MiCoISA.ExtCard[3].Source[0].wFeSourceType = (WORD) VC_ReadProfile(pDevInfo, PARAM_S30_ST, MICO_FBAS);
		pHw->MiCoISA.ExtCard[3].Source[0].wFeFilter = (WORD) VC_ReadProfile(pDevInfo, PARAM_S30_NF, MICO_NOTCH_FILTER_AUTO);
		dprintf1(("Source3-1"));
		pHw->MiCoISA.ExtCard[3].Source[1].nAVPortY = VC_ReadProfile(pDevInfo, PARAM_S31_AVPY, 0);
		pHw->MiCoISA.ExtCard[3].Source[1].nAVPortC = VC_ReadProfile(pDevInfo, PARAM_S31_AVPC, 0);
		pHw->MiCoISA.ExtCard[3].Source[1].wFeSourceType = (WORD) VC_ReadProfile(pDevInfo, PARAM_S31_ST, MICO_FBAS);
		pHw->MiCoISA.ExtCard[3].Source[1].wFeFilter = (WORD) VC_ReadProfile(pDevInfo, PARAM_S31_NF, MICO_NOTCH_FILTER_AUTO);
		dprintf1(("Source3-2"));
		pHw->MiCoISA.ExtCard[3].Source[2].nAVPortY = VC_ReadProfile(pDevInfo, PARAM_S32_AVPY, 0);
		pHw->MiCoISA.ExtCard[3].Source[2].nAVPortC = VC_ReadProfile(pDevInfo, PARAM_S32_AVPC, 0);
		pHw->MiCoISA.ExtCard[3].Source[2].wFeSourceType = (WORD) VC_ReadProfile(pDevInfo, PARAM_S32_ST, MICO_FBAS);
		pHw->MiCoISA.ExtCard[3].Source[2].wFeFilter = (WORD) VC_ReadProfile(pDevInfo, PARAM_S32_NF, MICO_NOTCH_FILTER_AUTO);
		dprintf1(("Source3-3"));
		pHw->MiCoISA.ExtCard[3].Source[3].nAVPortY = VC_ReadProfile(pDevInfo, PARAM_S33_AVPY, 0);
		pHw->MiCoISA.ExtCard[3].Source[3].nAVPortC = VC_ReadProfile(pDevInfo, PARAM_S33_AVPC, 0);
		pHw->MiCoISA.ExtCard[3].Source[3].wFeSourceType = (WORD) VC_ReadProfile(pDevInfo, PARAM_S33_ST, MICO_FBAS);
		pHw->MiCoISA.ExtCard[3].Source[3].wFeFilter = (WORD) VC_ReadProfile(pDevInfo, PARAM_S33_NF, MICO_NOTCH_FILTER_AUTO);
		dprintf1(("Source3-4"));
		pHw->MiCoISA.ExtCard[3].Source[4].nAVPortY = VC_ReadProfile(pDevInfo, PARAM_S34_AVPY, 0);
		pHw->MiCoISA.ExtCard[3].Source[4].nAVPortC = VC_ReadProfile(pDevInfo, PARAM_S34_AVPC, 0);
		pHw->MiCoISA.ExtCard[3].Source[4].wFeSourceType = (WORD) VC_ReadProfile(pDevInfo, PARAM_S34_ST, MICO_FBAS);
		pHw->MiCoISA.ExtCard[3].Source[4].wFeFilter = (WORD) VC_ReadProfile(pDevInfo, PARAM_S34_NF, MICO_NOTCH_FILTER_AUTO);
		dprintf1(("Source3-5"));
		pHw->MiCoISA.ExtCard[3].Source[5].nAVPortY = VC_ReadProfile(pDevInfo, PARAM_S35_AVPY, 0);
		pHw->MiCoISA.ExtCard[3].Source[5].nAVPortC = VC_ReadProfile(pDevInfo, PARAM_S35_AVPC, 0);
		pHw->MiCoISA.ExtCard[3].Source[5].wFeSourceType = (WORD) VC_ReadProfile(pDevInfo, PARAM_S35_ST, MICO_FBAS);
		pHw->MiCoISA.ExtCard[3].Source[5].wFeFilter = (WORD) VC_ReadProfile(pDevInfo, PARAM_S35_NF, MICO_NOTCH_FILTER_AUTO);
		dprintf1(("Source3-6"));
		pHw->MiCoISA.ExtCard[3].Source[6].nAVPortY = VC_ReadProfile(pDevInfo, PARAM_S36_AVPY, 0);
		pHw->MiCoISA.ExtCard[3].Source[6].nAVPortC = VC_ReadProfile(pDevInfo, PARAM_S36_AVPC, 0);
		pHw->MiCoISA.ExtCard[3].Source[6].wFeSourceType = (WORD) VC_ReadProfile(pDevInfo, PARAM_S36_ST, MICO_FBAS);
		pHw->MiCoISA.ExtCard[3].Source[6].wFeFilter = (WORD) VC_ReadProfile(pDevInfo, PARAM_S36_NF, MICO_NOTCH_FILTER_AUTO);
		dprintf1(("Source3-7"));
		pHw->MiCoISA.ExtCard[3].Source[7].nAVPortY = VC_ReadProfile(pDevInfo, PARAM_S37_AVPY, 0);
		pHw->MiCoISA.ExtCard[3].Source[7].nAVPortC = VC_ReadProfile(pDevInfo, PARAM_S37_AVPC, 0);
		pHw->MiCoISA.ExtCard[3].Source[7].wFeSourceType = (WORD) VC_ReadProfile(pDevInfo, PARAM_S37_ST, MICO_FBAS);
		pHw->MiCoISA.ExtCard[3].Source[7].wFeFilter = (WORD) VC_ReadProfile(pDevInfo, PARAM_S37_NF, MICO_NOTCH_FILTER_AUTO);
	}
}

//-----------------------------------------------------------------------------
// Higher level routines and combinations
//-----------------------------------------------------------------------------

//
// Called on device close. We need to free up memory allocated here
///////////////////////////////////////////////////////////////////////////////
BOOLEAN HW_Close(PDEVICE_INFO pDevInfo)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	dprintf1(("HW_Close called"));
	pHw->Format = FmtInvalid;
	HW_DisableInts(pDevInfo,pHw);
	ZRStopEncoding(pDevInfo);

	return(TRUE);
}

//
// Called on device cleanup
//
// Different to device close- we shut down the card, which will only
// be woken up b the HW_Init function next time the driver is loaded.
///////////////////////////////////////////////////////////////////////////////
BOOLEAN HW_Cleanup(PDEVICE_INFO pDevInfo)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	dprintf1(("HW_Cleanup called"));
	HW_DisableInts(pDevInfo,pHw);
	ZRStopEncoding(pDevInfo);
	HW_Close(pDevInfo);
	return(TRUE);
}

//  Open Device
///////////////////////////////////////////////////////////////////////////////
BOOLEAN HW_DeviceOpen(PDEVICE_INFO pDevInfo)
{
	BOOL bVideoIn;
	DWORD sts;

	PHWINFO pHw = VC_GetHWInfo(pDevInfo);
	PVC_CALLBACK pCallback;

	dprintf1(("HW_DeviceOpen called"));

	//  Disable overlay callbacks - vckernel will then reject these
	//  requests as not-supported.
	pCallback = VC_GetCallbackTable(pDevInfo);

	pCallback->GetOverlayModeFunc = HW_WatchDog;
	pCallback->SetKeyRGBFunc = MICOParameter;
	pCallback->SetKeyPalIdxFunc = NULL;
	pCallback->GetKeyColourFunc = NULL;
	pCallback->SetOverlayRectsFunc = HW_Alarm;
	pCallback->SetOverlayOffsetFunc = HW_Relais;
	pCallback->OverlayFunc = NULL;
	pCallback->DrawFrameFunc = NULL;

	//  Init some fields
	pHw->wSource = MICO_SOURCE_NO;
	pHw->wExtCard = MICO_EXTCARD_NO;
	pHw->wFrontEnd = MICO_FE0;
	pHw->wFormat = 0;
	pHw->wFramerate = 12;
	pHw->wBeVideoType = MICO_PAL_CCIR;
	pHw->dwBPF = 20000;

	// Zusätzliche Parameter
	pHw->wSkipFields	= 1;
	pHw->bHardwareSkip	= FALSE;
	pHw->bSwitchDelay	= TRUE;
	pHw->wField			= EVEN_FIELD;
	pHw->wMinLen		= 80;		// 80%
	pHw->wMaxLen		= 140;		// 140%
	pHw->dwReqProzessID = 0;
	pHw->dwProzessID	= 0;

	// Standardwerte für sychrone und asynchrone Kameras.
	if ((WORD)VC_ReadProfile(pDevInfo, PARAM_CAM_TYPE, -1) == 0)
	{
		dprintf1(("CameraType = Async"));

		// Anzahl der Halbbilder die nach der Umschaltung mindestens verworfen werden 
		pHw->wSkipFields	= 1;	// Mindestens ein Halbbild verwerfen   

		// Hardwareskip ein/aus
		pHw->bHardwareSkip	= FALSE;	// Hardwareskip aus	

		// Zusätlicher Delay von 20ms zwischen analoger und digitaler Umschaltung
		pHw->bSwitchDelay	= TRUE;	// Umschaltverzögerung ein
	}
	else if ((WORD)VC_ReadProfile(pDevInfo, PARAM_CAM_TYPE, -1) == 1)
	{
		dprintf1(("CameraType = Sync"));

		// Anzahl der Halbbilder die nach der Umschaltung mindestens verworfen werden 
//		pHw->wSkipFields	= 1;		// Mindestens ein Halbbild verwerfen
		pHw->wSkipFields	= 0;		// Kein Halbbild verwerfen

		// Hardwareskip ein/aus
		pHw->bHardwareSkip	= FALSE;	// Hardwareskip aus	

		// Zusätlicher Delay von 20ms zwischen analoger und digitaler Umschaltung
//		pHw->bSwitchDelay	= FALSE;	// Umschaltverzögerung aus
		pHw->bSwitchDelay	= TRUE;		// Umschaltverzögerung ein
	}
	else
	{
		dprintf1(("No Cameratyp specified\n"));
	}

	// Anzahl der Halbbilder die nach der Umschaltung mindestens verworfen werden 
	if (VC_ReadProfile(pDevInfo, PARAM_SKP, -1) != -1)
		pHw->wSkipFields   = (WORD)VC_ReadProfile(pDevInfo, PARAM_SKP, 0);

	// Hardwareskip ein/aus
	if (VC_ReadProfile(pDevInfo, PARAM_HRD_SKP, -1) != -1)
		pHw->bHardwareSkip = (BOOL)VC_ReadProfile(pDevInfo, PARAM_HRD_SKP, 0);

	// Zusätlicher Delay von 20ms zwischen analoger und digitaler Umschaltung
	if (VC_ReadProfile(pDevInfo, PARAM_SWITCH_DELAY, -1) != -1)
		pHw->bSwitchDelay = (BOOL)VC_ReadProfile(pDevInfo, PARAM_SWITCH_DELAY, 0);

	// Legt fest welches Field gespeichert werden soll
	if (VC_ReadProfile(pDevInfo, PARAM_STORE_FIELD, -1) != -1)
		pHw->wField = VC_ReadProfile(pDevInfo, PARAM_STORE_FIELD, EVEN_FIELD);


	// Filtereckwerte
	pHw->wMinLen = VC_ReadProfile(pDevInfo, PARAM_MIN_SIZE, 80);	// 80%
	pHw->wMaxLen = VC_ReadProfile(pDevInfo, PARAM_MAX_SIZE, 140);	// 140%

	// Bytes per Frame
	MICOEncoderSetBPF(pDevInfo,VC_ReadProfile(pDevInfo, PARAM_BPF, 20000));

	// FPS
	MICOEncoderSetFramerate(pDevInfo, (WORD) VC_ReadProfile(pDevInfo, PARAM_FPS, 25));

	// Resolution
	MICOEncoderSetFormat(pDevInfo, (WORD) VC_ReadProfile(pDevInfo, PARAM_RES, MICO_ENCODER_HIGH));

	// Video Format
	MICOSetFeVideoFormat(pDevInfo,(WORD) VC_ReadProfile(pDevInfo, PARAM_FE_VF, MICO_PAL_CCIR));
                                
	//  Front End Defaults for startup
	MICOSetFeBrightness(pDevInfo, (WORD) VC_ReadProfile(pDevInfo, PARAM_FE_BR, (MICO_MAX_BRIGHTNESS - MICO_MIN_BRIGHTNESS)/2));
	MICOSetFeContrast(pDevInfo,   (WORD) VC_ReadProfile(pDevInfo, PARAM_FE_CONT, (MICO_MAX_CONTRAST - MICO_MIN_CONTRAST)/2));
	MICOSetFeSaturation(pDevInfo, (WORD) VC_ReadProfile(pDevInfo, PARAM_BPF, (MICO_MAX_SATURATION - MICO_MIN_SATURATION)/2));
	MICOSetFeHue(pDevInfo,		  (WORD) VC_ReadProfile(pDevInfo, PARAM_FE_HUE, (MICO_MAX_HUE - MICO_MIN_HUE)/2));

	//  Backend Defaults for startup
	MICOSetBeVideoFormat(pDevInfo,(WORD) VC_ReadProfile(pDevInfo, PARAM_BE_VF, MICO_PAL_CCIR));
	MICOSetBeVideoType(pDevInfo,  (WORD) VC_ReadProfile(pDevInfo, PARAM_BE_VT, MICO_COMPOSITE));

	MICOLoadSources(pDevInfo);

	//  Default to Card0/Source0
	dprintf(("Setting Card0/Source0 Format = 0x%x, BPF = 0x%x",pHw->wFormat,pHw->dwBPF));
	dprintf(("Current FE = 0x%x",pHw->wFrontEnd));

	//  Set Input Source (Need's Interrupts)
	pHw->bVideoIn = TRUE;
	HW_EnableInts(pDevInfo,pHw);

	sts = MICOSetInputSource(pDevInfo, MICO_EXTCARD0, MICO_SOURCE0, pHw->wFormat, pHw->dwBPF, 0);

	if (sts == MICO_ERROR)
	{
		dprintf(("Setting input source failed."));
		return FALSE;
	}

	//  Test Video Present
	bVideoIn = MICOIsVideoPresent(pDevInfo);

	if (bVideoIn == TRUE)
	{
		dprintf1(("Found Video Signal on Card0/Source0"));
	}
	else
	{
		dprintf1(("No Video Signal detected on Card0/Source0"));
	}

	//  Start and init MJPEG compression
	pHw->bVideoAtOpen = bVideoIn;
	if (pHw->bVideoAtOpen == TRUE)
		ZRRestartEncoding(pDevInfo);

	return(TRUE);
}

///////////////////////////////////////////////////////////////////////////////
BOOLEAN HW_WatchDog(PDEVICE_INFO pDevInfo, PCONFIG_INFO pGeneric)
{
	PCONFIG_WDOG pConfig;
	PHWINFO pHw;
	BYTE tmp;
	BOOL done;

	dprintf1(("HW_Watchdog called"));
	if (pGeneric->ulSize != sizeof(CONFIG_WDOG))
	{
		dprintf(("HW_Watchdog bad config struct"));
		return(FALSE);
	}

	pConfig = (PCONFIG_WDOG) pGeneric;
	pHw = (PHWINFO) VC_GetHWInfo(pDevInfo);

	dprintf1(("HW_WatchDog Fnc=0x%x Param=0x%x",pConfig->ubFunction, pConfig->ulParameter));

	switch (pConfig->ubFunction)
	{
		case WDOG_GO:
		{
			tmp = (BYTE)HW_RdByte(pDevInfo,WATCHDOG_OFFSET,0);
			tmp = (BYTE)SETBIT(tmp,WATCHDOG_ENABLE_BIT);
			HW_WrByte(pDevInfo,WATCHDOG_OFFSET,tmp,0);                
			done = TRUE;
			break;
		}
		case WDOG_RESET:
		{
			tmp = (BYTE)HW_RdByte(pDevInfo,WATCHDOG_OFFSET,0);
			tmp = (BYTE)CLRBIT(tmp,WATCHDOG_ENABLE_BIT);
			HW_WrByte(pDevInfo,WATCHDOG_OFFSET,tmp,0);
			done = TRUE;
			break;
		}
		case WDOG_WRITE:
		{
			tmp = (BYTE)pConfig->ulParameter;
			HW_WrByte(pDevInfo,PTC_COUNT0_OFFSET,tmp,0);
			tmp = (BYTE)(pConfig->ulParameter >> 8);
			HW_WrByte(pDevInfo,PTC_COUNT1_OFFSET,tmp,0);
			tmp = (BYTE)(pConfig->ulParameter >> 16);
			HW_WrByte(pDevInfo,PTC_COUNT2_OFFSET,tmp,0);
			tmp = (BYTE)(pConfig->ulParameter >> 24);
			HW_WrByte(pDevInfo,PTC_CONTROL_OFFSET,tmp,0);
			done = TRUE;
			break;
		}
	}

	if (done)
		return (TRUE);
	else
		return (FALSE);        

}

///////////////////////////////////////////////////////////////////////////////
DWORD HW_Relais(PDEVICE_INFO pDevInfo, PCONFIG_INFO pGeneric)
{
	PCONFIG_RELAIS pConfig;
	PHWINFO pHw;
	BOOL done;
	DWORD tmp;

	done = FALSE;
	tmp = 0;
	dprintf1(("HW_Relais called"));
	if (pGeneric->ulSize != sizeof(CONFIG_RELAIS))
	{
		dprintf(("HW_Relais bad config struct"));
		return(0xffff);
	}								 

	pConfig = (PCONFIG_RELAIS) pGeneric;
	pHw = (PHWINFO) VC_GetHWInfo(pDevInfo);

	dprintf1(("HW_Relais Fnc=0x%x ExC=0x%x St=0x%x",
	pConfig->ubFunction, pConfig->ulExtCard, pConfig->ulState));

	pHw->wRELExtCard = (WORD) pConfig->ulExtCard;

	if (!MICOCheckExtCard(pDevInfo,(WORD)pConfig->ulExtCard))
		return 0xffff;

	switch (pConfig->ubFunction)
	{
		case WR_RELAIS_1:
		{
			if (pConfig->ulState == 1)
				MICOSetRelais(pDevInfo, (WORD)pConfig->ulExtCard, (BYTE)1);
			else
				RELClrRelay(pDevInfo, (BYTE)0xfe);
			done = TRUE;
			break;
		}
		case WR_RELAIS_2:
		{
			if (pConfig->ulState == 1)
				MICOSetRelais(pDevInfo, (WORD)pConfig->ulExtCard, (BYTE)2);
			else
				RELClrRelay(pDevInfo, (BYTE)0xfd);
			done = TRUE;
			break;
		}
		case WR_RELAIS_3:
		{
			if (pConfig->ulState == 1)
				MICOSetRelais(pDevInfo, (WORD)pConfig->ulExtCard, (BYTE)4);
			else
				RELClrRelay(pDevInfo, (BYTE)0xfb);
			done = TRUE;
			break;
		}
		case WR_RELAIS_4:
		{
			if (pConfig->ulState == 1)
				MICOSetRelais(pDevInfo, (WORD)pConfig->ulExtCard, (BYTE)8);
			else
				RELClrRelay(pDevInfo, (BYTE)0xf7);
			done = TRUE;
			break;
		}
		case RD_RELAIS_1:
		{
			tmp = (DWORD) MICOGetRelais(pDevInfo, (WORD)pConfig->ulExtCard);
			if ((tmp & 0x1) != 0)
				tmp = 1;
			else
				tmp = 0;
			done = TRUE;
			break;
		}
		case RD_RELAIS_2:
		{
			tmp = (DWORD) MICOGetRelais(pDevInfo, (WORD)pConfig->ulExtCard);
			if ((tmp & 0x2) != 0)
				tmp = 1;
			else
				tmp = 0;
			done = TRUE;
			break;
		}
		case RD_RELAIS_3:
		{
			tmp = (DWORD) MICOGetRelais(pDevInfo, (WORD)pConfig->ulExtCard);
			if ((tmp & 0x4) != 0)
				tmp = 1;
			else
				tmp = 0;
			done = TRUE;
			break;
		}
		case RD_RELAIS_4:
		{
			tmp = (DWORD) MICOGetRelais(pDevInfo, (WORD)pConfig->ulExtCard);
			if ((tmp & 0x8) != 0)
				tmp = 1;
			else
				tmp = 0;
			done = TRUE;
			break;
		}
	}
	if (done)
		return tmp;
	else
		return 0xffff;
}

///////////////////////////////////////////////////////////////////////////////
DWORD HW_Alarm(PDEVICE_INFO pDevInfo, PCONFIG_INFO pGeneric)
{
	PCONFIG_ALARM pConfig;
	PHWINFO pHw;
	BOOL done;

	done = FALSE;
	dprintf1(("HW_Alarm called"));
	if (pGeneric->ulSize != sizeof(CONFIG_ALARM))
	{
		dprintf(("HW_Alarm bad config struct"));
		return(0xffff);
	}

	pConfig = (PCONFIG_ALARM) pGeneric;
	pHw = (PHWINFO) VC_GetHWInfo(pDevInfo);

	dprintf1(("HW_Alarm: Fnc=0x%x ExC=0x%x St=0x%x StT=0x%x",
	pConfig->ubFunction, pConfig->ulExtCard,
	pConfig->ulState, pConfig->ulStateType));

	if (!MICOCheckExtCard(pDevInfo,(WORD)pConfig->ulExtCard))
		return 0xffff;

	pHw->wALRExtCard = (WORD)pConfig->ulExtCard;

	switch (pConfig->ubFunction)
	{
		case WR_ALARM:
		{
			if (pConfig->ulStateType == ALARM_EDGE)
				ALARMSetEdge(pDevInfo, (BYTE)pConfig->ulState);
			else if (pConfig->ulStateType == ALARM_ACK)
				ALARMSetAck(pDevInfo, (BYTE)pConfig->ulState);
			done = TRUE;
			break;
		}
		case RD_ALARM:
		{
			if (pConfig->ulStateType == ALARM_STATE)
				pConfig->ulState = (DWORD)ALARMGetState(pDevInfo);
			else if (pConfig->ulStateType == ALARM_CUR_STATE)
				pConfig->ulState = (DWORD)ALARMGetCurrentState(pDevInfo);
			else if (pConfig->ulStateType == ALARM_EDGE)
				pConfig->ulState = (DWORD)ALARMGetEdge(pDevInfo);
			else if (pConfig->ulStateType == ALARM_ACK)
				pConfig->ulState = (DWORD)ALARMGetAck(pDevInfo);
			done = TRUE;
			break;
		}
	}

	if (done)
		return pConfig->ulState;
	else
		return 0xffff;
}

//  Set Video Source, Video Standard, Hue and Filter
///////////////////////////////////////////////////////////////////////////////
BOOLEAN HW_ConfigSource(PDEVICE_INFO pDevInfo, PCONFIG_INFO pGeneric)
{
	PCONFIG_SOURCE pConfig;
	PHWINFO pHw;

	dprintf1(("HW_ConfigSource called"));
	if (pGeneric->ulSize != sizeof(CONFIG_SOURCE))
	{
		dprintf(("HW_ConfigSource bad config struct"));
		return(FALSE);
	}

	pConfig = (PCONFIG_SOURCE) pGeneric;
	pHw = (PHWINFO) VC_GetHWInfo(pDevInfo);

	MICOSetInputSource(pDevInfo, (WORD)pConfig->ulExtCard, (WORD)pConfig->ulInpSource, pHw->wFormat, pHw->dwBPF, pConfig->ulProzessID);

	// Helligkeitseinstellung geändert?
	if ((WORD)pConfig->ulBright != pHw->wFeBrightness)
		MICOSetFeBrightness(pDevInfo,(WORD)pConfig->ulBright);

	// Kontrasteinstellung geändert?
	if ((WORD)pConfig->ulContrast != pHw->wFeContrast)
		MICOSetFeContrast(pDevInfo,(WORD)pConfig->ulContrast);

	// Farbsättigung verändert?
	if ((WORD)pConfig->ulSat != pHw->wFeSaturation)
		MICOSetFeSaturation(pDevInfo,(WORD)pConfig->ulSat);

	// Hue geändert?
	if ((WORD)pConfig->ulHue != pHw->wFeHue)
		MICOSetFeHue(pDevInfo,(WORD)pConfig->ulHue);

/*
	//
	// *JW* Modified Return FALSE if no Video Signal is seen
	//
	//  Test Video Present Signal on current source.
	//  If Video is present retrun TRUE else return FALSE.
	//
	if (MICOIsVideoPresent(pDevInfo))
		return TRUE;
	else
		return FALSE;
*/
	return TRUE;
}

//
// Configure the destination format - the size, bitdepth of the
// destination dib, and set any necessary translation table to
// translate from YUV to the specified format.
///////////////////////////////////////////////////////////////////////////////
BOOLEAN HW_ConfigFormat(PDEVICE_INFO pDevInfo, PCONFIG_INFO pGeneric)
{
	PCONFIG_FORMAT pConfig;
	PHWINFO pHw;

	dprintf1(("HW_ConfigFormat called"));
	pConfig = (PCONFIG_FORMAT) pGeneric;

	if (pConfig->ulSize != sizeof(CONFIG_FORMAT))
	{
		dprintf(("HW_ConfigFormat bad config struct"));
		return(FALSE);
	}

	pHw = (PHWINFO) VC_GetHWInfo(pDevInfo);

	//  Encoder, Frontends and Backend
	MICOEncoderSetBPF(pDevInfo,(WORD)pConfig->ulBPF);
	MICOEncoderSetFramerate(pDevInfo, (WORD)pConfig->ulFPS);
	MICOEncoderSetFormat(pDevInfo, (WORD)pConfig->ulResolution);
	MICOSetFeVideoFormat(pDevInfo,(WORD)pConfig->ulFeVideoFormat);
	MICOSetBeVideoFormat(pDevInfo, (WORD)pConfig->ulBeVideoFormat );
	MICOSetBeVideoType(pDevInfo,(WORD)pConfig->ulBeVideoType);

	dprintf1(("HW_ConfigFormat: BPF=0x%x, FPS=0x%x",pHw->dwBPF, pHw->wFramerate));

	//  Report our expected image size. VCKernel will then reject
	//  buffers of less than this at request time (rather than us doing it
	//  at interrupt time...)
	VC_SetImageSize(pDevInfo, pConfig->ulBPF);

	return(TRUE);
}

///////////////////////////////////////////////////////////////////////////////////
BOOLEAN HW_Capture(PDEVICE_INFO pDevInfo, BOOL bCapture)
{
	dprintf1(("HW_Capture called"));
	return(TRUE);
}

///////////////////////////////////////////////////////////////////////////////////
BOOLEAN HW_StreamInit(PDEVICE_INFO pDevInfo, DWORD microsecperframe)
{
	PHWINFO pHw;

	pHw = VC_GetHWInfo(pDevInfo);

	pHw->dwTimePerFrame = microsecperframe / 100;
	dprintf(("HW_StreamInit: Requested 100-uSecs units per Frame =     %d",pHw->dwTimePerFrame));
	//
	// *JW* Modified: Test for Video Signal
	//
	//  Test Video Present Signal on current source.
	//  If Video is present start Encoding else return FALSE.
	//
	if (MICOIsVideoPresent(pDevInfo))
	{
		ZRRestartEncoding(pDevInfo);
		pHw->bVideoAtOpen = TRUE;
		return TRUE;
	}
	else
		return FALSE;

}

///////////////////////////////////////////////////////////////////////////////////
BOOLEAN HW_StreamFini(PDEVICE_INFO pDevInfo)
{
	return (TRUE);
}		     

///////////////////////////////////////////////////////////////////////////////////
BOOLEAN HW_StreamStart(PDEVICE_INFO pDevInfo)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);
	WORD Ctrl, Cnt1, Cnt2;

	// Convert from usec to 100 usec units
	if (pHw->VideoStd == PAL)
		pHw->dwTimePerInterrupt = PAL_MICROSPERFIELD / 100;
	else
		pHw->dwTimePerInterrupt = NTSC_MICROSPERFIELD / 100;

	dprintf(("HW_StreamStart: Expected 100-uSecs units per Interrupt = %d",pHw->dwTimePerInterrupt));

	pHw->dwInterruptCount = 0;
	pHw->dwVideoTime = 0;
	pHw->dwNextFrameTime = 0;
	pHw->bVideoIn = TRUE;

	return(TRUE);
}

///////////////////////////////////////////////////////////////////////////////////
BOOLEAN HW_StreamStop(PDEVICE_INFO pDevInfo)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);
	WORD Ctrl;

	return(TRUE);
}

///////////////////////////////////////////////////////////////////////////////////
DWORD HW_StreamGetPosition(PDEVICE_INFO pDevInfo)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	dprintf1(("HW_StreamGetPosition called"));
	return ((DWORD) pHw->dwVideoTime / 10);
}

/////////////////////////////////////////////////////////////////////////////////////
// Interrupt Service Routine (ISR)
BOOLEAN HW_InterruptAck(PDEVICE_INFO pDevInfo)
{
	BYTE	byIRR;
	BYTE	byIER;
	WORD	wIOBase;
	WORD	wField	= EVEN_FIELD;
	BOOL	bDoDPC	= FALSE;
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	pHw->dwInterruptCount++;

	// Interrupt Request lesen
	byIRR = HW_RdByte(pDevInfo, MICO_IRR_OFFSET, 0);
	byIER = HW_RdByte(pDevInfo, MICO_IER_OFFSET, 0); 

	// Fieldinformation holen.
	if (TSTBIT(byIRR, VSYNC_BIT))
		wField = (TSTBIT(byIRR, FIELD_BIT)) ? ODD_FIELD : EVEN_FIELD;
	else
		wField = (TSTBIT(byIRR, FIELD_BIT)) ? EVEN_FIELD : ODD_FIELD;

	// Zoran Interrupt? 
	if ((TSTBIT(byIER, ZR_IE_BIT)) && (TSTBIT(byIRR, ZR_IR_BIT)))
	{ 
		if (HW_ZRIAck(pDevInfo, wField, &bDoDPC))
		{
			// Soll die Videoquelle gewechselt werden ?
			if (pHw->bSwitch)
				ServiceSourceSelection(pDevInfo);
		}
	}
	else
	{
		// Interrupt quitieren (Test gegen das 'Stehenbleiben' des Encoders)
		HW_RdByte(pDevInfo, ZR_STATUS0_OFFSET, 0);
		HW_RdByte(pDevInfo, ZR_FCNT_OFFSET, 0);
		HW_RdByte(pDevInfo, ZR_LEN0_OFFSET, 0);
		HW_RdByte(pDevInfo, ZR_LEN1_OFFSET, 0);
	}

	//  Advance Video Position b one VSync
	pHw->dwVideoTime += pHw->dwTimePerInterrupt;

	return bDoDPC;
}


/////////////////////////////////////////////////////////////////////////////////////
// Behandlung der Zoran-Interrupts
BOOLEAN HW_ZRIAck(PDEVICE_INFO pDevInfo, WORD wField, BOOL *bDoDPC)
{
	static BYTE byPrevFCNT  = 0;
	BYTE  byStatus0;
	BYTE  byFCNT;
	BYTE  byLen0;
	BYTE  byLen1;
	BYTE  byCom0;
	BYTE  byBufferPage;
	WORD  wLen;
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

    pHw->wJpegLen = (WORD) -1;
    pHw->dwZRInterruptCount++;

	// Interrupt quitieren
	byStatus0= HW_RdByte(pDevInfo, ZR_STATUS0_OFFSET, 0);
	byFCNT	 = HW_RdByte(pDevInfo, ZR_FCNT_OFFSET, 0);
	byLen0	 = HW_RdByte(pDevInfo, ZR_LEN0_OFFSET, 0);
	byLen1	 = HW_RdByte(pDevInfo, ZR_LEN1_OFFSET, 0);
	wLen	 = MAKEWORD(byLen0, byLen1);

	// Pagenummer aus dem Status0-Register selektieren
	byBufferPage = byStatus0 & 0x03;

	// Eine neue Pagenummer in COM0 selektieren.
	byCom0 = HW_RdByte(pDevInfo, ZR_COM0_OFFSET, 0) & 0xfc;
	byCom0 |= byBufferPage;
	HW_WrByte(pDevInfo, ZR_COM0_OFFSET, byCom0, 0);
	 
	// Lost Field ?
	if ((BYTE)(byPrevFCNT + 1) != byFCNT)
	{
		dprintf1(("MICO_VXD: WM_MICO_VXD_LOST_FIELD"));
	}

	// Für den nächsten Durchgang sichern
	byPrevFCNT = byFCNT;

	// Soll die Videoquelle gewechselt werden? dann wollen wir keine alten
	// Bilder mehr haben.
	if (pHw->bSwitch)
	{
		*bDoDPC = FALSE; 
		return TRUE;
	}

	// Nach einer Umschaltung sollen Bilder verworfen werden.
	if (wGlobalSkipFieldCnt != 0)
	{
		wGlobalSkipFieldCnt--;
		*bDoDPC = FALSE; 
		return FALSE;
	}	  

	// Es wird nur ein Halbbild verwertet
	if ((pHw->wField == ODD_FIELD) && (wField == EVEN_FIELD))
	{
		*bDoDPC = FALSE; 
		return TRUE;
	}
	if ((pHw->wField == EVEN_FIELD) && (wField == ODD_FIELD))
	{
		*bDoDPC = FALSE; 
		return TRUE;
	}

	// Invalid Data
	if (TSTBIT(byStatus0, STATUS0_FNV_BIT))
	{
		dprintf1(("Unvalid field"));
		*bDoDPC = FALSE; 
 		return TRUE;
	}

	// Länge soll gerade sein
	wLen = ((wLen + 1) & 0xfffe);	

    // This section prevents reads of lengthes ending with 511 or 512 chip bug.
	if (((wLen & 0xff) == 0xfe) || ((wLen & 0xff) == 0xfd))
		wLen += 8;
							   
	// Filtert zu kleine und zu große Bilder herraus.
	if ((pHw->wMinLen != 0) && (wLen < ((pHw->dwBPF * pHw->wMinLen) / 100L)))
	{
		dprintf1(("Jpegbild zu klein")); 
		*bDoDPC = FALSE; 
		return TRUE;
	}
	if ((pHw->wMaxLen != 0) && (wLen > ((pHw->dwBPF * pHw->wMaxLen) / 100L)))
	{
		dprintf1(("Jpegbild zu groß")); 
		*bDoDPC = FALSE; 
		return TRUE;
	}

	*bDoDPC = TRUE; 
	pHw->wJpegLen = wLen;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////
// Grabt die Daten vom Zoran
DWORD HW_CaptureService(PDEVICE_INFO pDevInfo,PUCHAR pBuffer,PDWORD pTimeStamp,DWORD BufferLength)
{
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);
	WORD	wIOAddr;
	WORD	wLen;
	WORD	wI;

	// Save Image Length before we continue
	wLen = pHw->wJpegLen;

//	HW_EnableZRInts(pDevInfo, pHw);
	if (pBuffer == NULL)
	{
		// Skip frame - no buffer available
		return 0;
	}
	else
	{
		// This is burst mode readout - if this is too fast we need for/to loops
		dprintf1(("DPC - Called, Ln=0x%x",wLen));

		// This IOAddr stuff is to work around a compiler Bug in VC++ V4.2
		wIOAddr = (WORD)pDevInfo->PortBase;
		wIOAddr += ZR_CB_DATA_OFFSET;
		READ_PORT_BUFFER_USHORT((PWORD)wIOAddr,(PWORD)pBuffer,(wLen/2));

		// ProzeßID und Videoquelle mit in den Buffer schreiben.
		*((DWORD*)&pBuffer[wLen+0]) = pHw->dwProzessID;
		*((WORD*) &pBuffer[wLen+4]) = pHw->wSource;
		*((WORD*) &pBuffer[wLen+6]) = pHw->wExtCard;


		return (DWORD)wLen+8;	// +8 Bytes Zusatzinformationen
	}
}

/////////////////////////////////////////////////////////////////////////////////////
// Behandlung Videoquellenumschaltung
void ServiceSourceSelection(PDEVICE_INFO pDevInfo)
{
	BYTE	byCSVDIN;
	PHWINFO pHw = VC_GetHWInfo(pDevInfo);

	if (bGlobalSwitchAnalogVideo == FALSE)
	{
		// Analoge Umschaltung ist erfolgt.
		bGlobalSwitchAnalogVideo = TRUE;

		if (pHw->bHardwareSkip)
		{
			// Setzen des Bits 'SKPNVS'	-> Nächstes VSYNC sperren
			byCSVDIN = ZRChipSelectVideoInDigitalIn(pDevInfo);
			byCSVDIN	= SETBIT(byCSVDIN, SELECT_SKPNVS_BIT);
			ZRChipSelectVideoInDigitalOut(pDevInfo, byCSVDIN);
		}

		// Soll ein Interrupt gewartet werden, bevor digital umgeschaltet wird?
		if (pHw->bSwitchDelay)
			return;
	}

	// Sollbildgröße setzen
	SetBPF(pDevInfo, pHw->dwBPF);	

	// Auf neue Videoquelle schalten und Vsyncs wieder freigeben.			
	byCSVDIN = (BYTE)pHw->wCSVDIN;
	byCSVDIN = CLRBIT(byCSVDIN, SELECT_SKPNVS_BIT);
	ZRChipSelectVideoInDigitalOut(pDevInfo, byCSVDIN);

	// Digitale Umschaltung ist erfolgt
	bGlobalSwitchAnalogVideo	= FALSE;

	// Zahl der Halbbilder, die nach einem Umschaltvorgang verworfen werden
	wGlobalSkipFieldCnt = pHw->wSkipFields;

	// Neue Videoquelle merken.
	pHw->wSource	= pHw->wReqSource;
	pHw->wExtCard	= pHw->wReqExtCard;

	// Aktuelle ProzeßID merken
	pHw->dwProzessID = pHw->dwReqProzessID;

	// Umschaltung komplett
	pHw->bSwitch	= FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////
void  SetBPF(PDEVICE_INFO pDevInfo, DWORD dwBPF)
{
	DWORD dwTemp = 8L * dwBPF;

	HW_WrShort(pDevInfo, ZR_ADDRESS_OFFSET, 0x09, 0);						// Write Adress
	HW_WrShort(pDevInfo, ZR_DATA_OFFSET,	(dwTemp & 0xff000000)>>24, 0);	// Write Data
	HW_WrShort(pDevInfo, ZR_ADDRESS_OFFSET, 0x0a, 0);						// Write Adress
	HW_WrShort(pDevInfo, ZR_DATA_OFFSET,	(dwTemp & 0x00ff0000)>>16, 0);	// Write Data
	HW_WrShort(pDevInfo, ZR_ADDRESS_OFFSET, 0x0b, 0);						// Write Adress
	HW_WrShort(pDevInfo, ZR_DATA_OFFSET,	(dwTemp & 0x0000ff00)>>8, 0);	// Write Data
	HW_WrShort(pDevInfo, ZR_ADDRESS_OFFSET, 0x0c, 0);						// Write Adress
	HW_WrShort(pDevInfo, ZR_DATA_OFFSET,	(dwTemp & 0x000000ff), 0);		// Write Data
	HW_WrShort(pDevInfo, ZR_ADDRESS_OFFSET, 0x00, 0);						// Go

	dwTemp -= (dwTemp / 128L);		              // minus STUFFING (SHMUEL)
	dwTemp -= ((dwTemp * 5L) / 64L);              // minus EOB (SHMUEL)

	HW_WrShort(pDevInfo, ZR_ADDRESS_OFFSET, 0x0d, 0);						// Write Adress
	HW_WrShort(pDevInfo, ZR_DATA_OFFSET,	(dwTemp & 0xff000000)>>24, 0);	// Write Data
	HW_WrShort(pDevInfo, ZR_ADDRESS_OFFSET, 0x0e, 0);						// Write Adress
	HW_WrShort(pDevInfo, ZR_DATA_OFFSET,	(dwTemp & 0x00ff0000)>>16, 0);	// Write Data
	HW_WrShort(pDevInfo, ZR_ADDRESS_OFFSET, 0x0f, 0);						// Write Adress
	HW_WrShort(pDevInfo, ZR_DATA_OFFSET,	(dwTemp & 0x0000ff00)>>8, 0);	// Write Data
	HW_WrShort(pDevInfo, ZR_ADDRESS_OFFSET, 0x10, 0);						// Write Adress
	HW_WrShort(pDevInfo, ZR_DATA_OFFSET,	(dwTemp & 0x000000ff), 0);		// Write Data
	HW_WrShort(pDevInfo, ZR_ADDRESS_OFFSET, 0x00, 0);						// Go
}
