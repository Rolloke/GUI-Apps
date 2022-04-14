/////////////////////////////////////////////////////////////////////////////
// PROJECT:		HAcc.sys
// FILE:		$Workfile: HAccCommon.c $
// ARCHIVE:		$Archive: /Project/Units/Tasha(Developer)/haccsys/HAccCommon.c $
// CHECKIN:		$Date: 5.01.04 9:52 $
// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 5.01.04 9:25 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

// Common-part of HAcc Driver
/////////////////////////////////////////////////////////////////////////////////////

#include "Driver.h"

extern CONFIG*		g_pBackupConfig[4];
extern BOOL			g_bHasSFence;
extern BOOL			g_bHasMMX;
extern MEMSTRUCT	g_FrameBuffer;

/////////////////////////////////////////////////////////////////////////////////////
NTSTATUS IOCtlDispatch(int nControlCode, IOCtlStruct IOCtl)
{
	NTSTATUS Status = STATUS_NOT_IMPLEMENTED;

	switch (nControlCode)
	{
		case HACC_OPEN:
			Status = IOCtlOpen(IOCtl);
			break;
		case HACC_CLOSE:
			Status = IOCtlClose(IOCtl);
			break;
		case HACC_GET_DRIVERVERSION:
			Status = IOCtlGetVersion(IOCtl);
			break;
		case HACC_MAP_PHYS_TO_LIN:
			Status = IOCtlMapPhysToLin(IOCtl);
			break;
		case HACC_UNMAP_PHYS_TO_LIN:
			Status = IOCtlUnmapPhysToLin(IOCtl);
			break;
		case HACC_MAP_LIN_TO_PHYS:
			Status = IOCtlMapLinToPhys(IOCtl);
			break;
		case HACC_READ_EVENT_PACKET:
			Status = IOCtlReadEventPacket(IOCtl);
			break;	
		case HACC_WRITE_PORT:
			Status = IOCtlWritePort(IOCtl);
			break;
		case HACC_READ_PORT:
			Status = IOCtlReadPort(IOCtl);
			break;
		case HACC_WRITE_MEMORY:
			Status = IOCtlWriteMemory(IOCtl);
			break;
		case HACC_READ_MEMORY:
			Status = IOCtlReadMemory(IOCtl);
			break;
		case HACC_ALLOC_MEMORY:
			Status = IOCtlAllocateMemory(IOCtl);
			break;
		case HACC_FREE_MEMORY:
			Status = IOCtlFreeMemory(IOCtl);
			break;
		case HACC_GET_FRAME_BUFFER:
			Status = IOCtlGetFrameBuffer(IOCtl);
		default:
			Out_Debug_String("HAcc Driver: Unknown Controlcode\n\r");
			break;
	}

	return Status;
}

/////////////////////////////////////////////////////////////////////////////////////
NTSTATUS IOCtlOpen(IOCtlStruct IOCtl)
{
	MEMSTRUCT	Mem;
	CONFIG		*pConfig	= NULL;	
	SETTINGS	*pSettings	= NULL;

	Out_Debug_String("HAcc Driver: Initialisation...\n\r");

	// Size of buffer containing data from application 
	if (IOCtl.dwInBufferSize != sizeof(SETTINGS))
	{
		Out_Debug_String(("HAcc Driver: ERROR IOCtlOpen InBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

    // Size of buffer for data to be sent to application 
	if (IOCtl.dwOutBufferSize != sizeof(HDEVICE))
	{
		Out_Debug_String(("HAcc Driver: ERROR IOCtlOpen OutBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

	// Adresse der Settingsstruktur
	pSettings = (SETTINGS*)IOCtl.pInBuffer;
	if (!pSettings)
	{
        Out_Debug_String("HAcc Driver: ERROR IOCtlOpen pInputBuffer = NULL\n\r");
		return STATUS_INVALID_PARAMETER;
	}

	// Pointer to Outputbuffer
	if (!IOCtl.pOutBuffer)
	{
        Out_Debug_String("HAcc Driver: ERROR IOCtlOpen pOutputBuffer = NULL\n\r");
		return STATUS_INVALID_PARAMETER;
	}

	// Configurationsspeicher anlegen.
	if (!PageAllocate(&Mem, sizeof(CONFIG), FALSE))
	{
		Out_Debug_String("HAcc Driver: ERROR IOCtlOpen can't allocate Configurationspace\n\r");
		return STATUS_INVALID_PARAMETER;
	}

	pConfig = (CONFIG*)Mem.pLinAddr;	
	if (!pConfig)
	{
		Out_Debug_String("HAcc Driver: ERROR IOCtlOpen pConfig = NULL\n\r");
		return STATUS_INVALID_PARAMETER;
	}

	// Sichere diese Adresse, um in DriverUnload den Treiber noch schließen
	// zu können, wenn die HAccUnit dies nicht gemacht hat.
	if (!g_pBackupConfig[0])
		g_pBackupConfig[0] = pConfig;
	else if (!g_pBackupConfig[1])
		g_pBackupConfig[1] = pConfig;
	else if (!g_pBackupConfig[2])
		g_pBackupConfig[2] = pConfig;
	else if (!g_pBackupConfig[3])
		g_pBackupConfig[3] = pConfig;
	else
		Out_Debug_String("HAcc Driver: ERROR IOCtlOpen Bereits 4 Instanzen gestartet\n");



	// Configurationsbereich mit den Initialisierungsdaten konfigurieren
	pConfig->wBusType			= pSettings->wBusType;
	pConfig->dwIOBase			= pSettings->dwIOBase;
	pConfig->wIRQ				= pSettings->wIRQ;
	pConfig->hDebugEvent		= CreateRing0Event(pSettings->hDebugEvent);
	pConfig->dwCPUClk			= pSettings->dwCPUClk;


	// Interruptroutine einbinden
	if (!InstallISR(pConfig))
	{
        Out_Debug_String("HAcc Driver: ERROR ISR konnte nicht installiert werden!\n\r");
		return STATUS_SHARED_IRQ_BUSY;
	}


	// Erfolgsmeldung an die HAccUnit verschicken
	SendDebugMessageToUnit0(pConfig, EVENT_INFO, "Open successfully\n");

	// DeviceID zurückliefern
 	*(HDEVICE*)IOCtl.pOutBuffer = (HDEVICE)pConfig;

	// Bytes to returned
	*IOCtl.pBytesToReturn		= sizeof(HDEVICE);

	return STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
NTSTATUS IOCtlClose(IOCtlStruct IOCtl)
{
	CONFIG	*pConfig	= NULL;	
	HDEVICE	*pDevice	= NULL;

	// Size of buffer containing data from application 
 	if (IOCtl.dwInBufferSize != sizeof(HDEVICE))
	{
		Out_Debug_String(("HAcc Driver: ERROR IOCtlClose InBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

    // Size of buffer for data to be sent to application 
	if (IOCtl.dwOutBufferSize != 0)
	{
		Out_Debug_String(("HAcc Driver: ERROR IOCtlClose OutBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}
    
	// Adresse der Config-Struktur
	pDevice = (HDEVICE*)IOCtl.pInBuffer;
	if (!pDevice)
	{
        Out_Debug_String("HAcc Driver: IOCtlClose pInputBuffer = NULL!\n");
		return STATUS_INVALID_PARAMETER; 
	}
	
	pConfig = (CONFIG*)*pDevice;
	if (!pConfig)
	{
        Out_Debug_String("HAcc Driver: IOCtlClose pConf = NULL!\n");
		return STATUS_INVALID_PARAMETER; 
	}

	// Aufräumen
	CleanUp(pConfig);

	return STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
NTSTATUS IOCtlGetVersion(IOCtlStruct IOCtl)
{
	DWORD	*pIO	= NULL;	

    Out_Debug_String("HAcc Driver: IOCtlGetVersion\n");

	// Size of buffer containing data from application 
 	if (IOCtl.dwInBufferSize != 0)
	{
		Out_Debug_String(("HAcc Driver: ERROR IOCtlGetVersion InBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

    // Size of buffer for data to be sent to application 
	if (IOCtl.dwOutBufferSize != sizeof(DWORD))
	{
		Out_Debug_String(("HAcc Driver: ERROR IOCtlGetVersion OutBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

	// Pointer to Outputbuffer
	if (!IOCtl.pOutBuffer)
	{
        Out_Debug_String("HAcc Driver: ERROR IOCtlGetVersion pOutputBuffer = NULL\n\r");
		return STATUS_INVALID_PARAMETER;
	}

	// Versionsnummer zurückliefern
 	*(DWORD*)IOCtl.pOutBuffer	= HACC_UNIT_VERSION;

	// Bytes to returned
	*IOCtl.pBytesToReturn		= sizeof(DWORD);
	
    return STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
NTSTATUS IOCtlWritePort(IOCtlStruct IOCtl)
{
	IOSTRUCT	*pIoIn	= NULL;	

	*IOCtl.pBytesToReturn = 0;

	// Size of buffer containing data from application 
 	if (IOCtl.dwInBufferSize != sizeof(IOSTRUCT))
	{
		Out_Debug_String(("HAcc Driver: ERROR IOCtlWritePort InBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

    // Size of buffer for data to be sent to application 
	if (IOCtl.dwOutBufferSize != 0)
	{
		Out_Debug_String(("HAcc Driver: ERROR IOCtlWritePort OutBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}
  
	pIoIn = (IOSTRUCT*)IOCtl.pInBuffer;
	if (!pIoIn)
	{
		Out_Debug_String("HAcc Driver: ERROR IOCtlWritePort pInputBuffer = NULL!\n");
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

    return STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
NTSTATUS IOCtlReadPort(IOCtlStruct IOCtl)
{
	IOSTRUCT	*pIoIn	= NULL;	

	// Size of buffer containing data from application 
 	if (IOCtl.dwInBufferSize != sizeof(IOSTRUCT))
	{
		Out_Debug_String(("HAcc Driver: ERROR IOCtlReadPort InBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

    // Size of buffer for data to be sent to application 
 	if (IOCtl.dwOutBufferSize != sizeof(IOSTRUCT))
	{
		Out_Debug_String(("HAcc Driver: ERROR IOCtlReadPort OutBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}
    
	pIoIn = (IOSTRUCT*)IOCtl.pInBuffer;
	if (!pIoIn)
	{
		Out_Debug_String("HAcc Driver: ERROR IOCtlReadPort pInputBuffer = NULL!\n");
		return STATUS_INVALID_PARAMETER;
	}

	// Pointer to Outputbuffer
	if (!IOCtl.pOutBuffer)
	{
        Out_Debug_String("HAcc Driver: ERROR IOCtlReadPort pOutputBuffer = NULL\n\r");
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

	// Bytes to returned
	*IOCtl.pBytesToReturn = sizeof(IOSTRUCT);

	return STATUS_SUCCESS;
}



///////////////////////////////////////////////////////////////////////////////////
NTSTATUS IOCtlWriteMemory(IOCtlStruct IOCtl)
{
	IOSTRUCT	*pIoIn	= NULL;	

	*IOCtl.pBytesToReturn = 0;

	// Size of buffer containing data from application 
 	if (IOCtl.dwInBufferSize != sizeof(IOSTRUCT))
	{
		Out_Debug_String(("HAcc Driver: ERROR IOCtlWriteMemory InBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

    // Size of buffer for data to be sent to application 
	if (IOCtl.dwOutBufferSize != 0)
	{
		Out_Debug_String(("HAcc Driver: ERROR IOCtlWriteMemory OutBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}
  
	pIoIn = (IOSTRUCT*)IOCtl.pInBuffer;
	if (!pIoIn)
	{
		Out_Debug_String("HAcc Driver: ERROR IOCtlWriteMemory pInputBuffer = NULL!\n");
		return STATUS_INVALID_PARAMETER;
	}

	switch (pIoIn->wIOSize)
	{
		case IO_SIZE8:
			PCIOutput8(pIoIn->dwAddr, pIoIn->byVal);
			break;
		case IO_SIZE16:
			PCIOutput16(pIoIn->dwAddr, pIoIn->wVal);
			break;
		case IO_SIZE32:
			PCIOutput32(pIoIn->dwAddr, pIoIn->dwVal);
			break;
		case IO_SIZEX:
			PCIOutputX(pIoIn->pBuffer, pIoIn->dwAddr, pIoIn->dwLen);
			break;
	}

    return STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
NTSTATUS IOCtlReadMemory(IOCtlStruct IOCtl)
{
	IOSTRUCT	*pIoIn	= NULL;	

	// Size of buffer containing data from application 
 	if (IOCtl.dwInBufferSize != sizeof(IOSTRUCT))
	{
		Out_Debug_String(("HAcc Driver: ERROR IOCtlReadMemory InBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

    // Size of buffer for data to be sent to application 
 	if (IOCtl.dwOutBufferSize != sizeof(IOSTRUCT))
	{
		Out_Debug_String(("HAcc Driver: ERROR IOCtlReadMemory OutBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}
    
	pIoIn = (IOSTRUCT*)IOCtl.pInBuffer;
	if (!pIoIn)
	{
		Out_Debug_String("HAcc Driver: ERROR IOCtlReadMemory pInputBuffer = NULL!\n");
		return STATUS_INVALID_PARAMETER;
	}

	// Pointer to Outputbuffer
	if (!IOCtl.pOutBuffer)
	{
        Out_Debug_String("HAcc Driver: ERROR IOCtlReadMemory pOutputBuffer = NULL\n\r");
		return STATUS_INVALID_PARAMETER;
	}

	switch (pIoIn->wIOSize)
	{
		case IO_SIZE8:
			pIoIn->byVal = PCIInput8(pIoIn->dwAddr);
			break;
		case IO_SIZE16:
			pIoIn->wVal  = PCIInput16(pIoIn->dwAddr);
			break;
		case IO_SIZE32:
			pIoIn->dwVal = PCIInput32(pIoIn->dwAddr);
			break;
		case IO_SIZEX:
			PCIInputX(pIoIn->pBuffer, pIoIn->dwAddr, pIoIn->dwLen);
			break;
	}

	// Bytes to returned
	*IOCtl.pBytesToReturn = sizeof(IOSTRUCT);

	return STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
NTSTATUS IOCtlReadEventPacket(IOCtlStruct IOCtl)
{
	CONFIG*			pConfig		= NULL;
	HDEVICE* 		pDevice		= NULL;
	EVENT_PACKET*	pEventPacket= NULL;
	WORD			wRdTP		= 0;

	// Size of buffer containing data from application 
 	if (IOCtl.dwInBufferSize != sizeof(EVENT_PACKET))
	{
		Out_Debug_String(("HAcc Driver: ERROR IOCtlReadEventPacket InBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

    // Size of buffer for data to be sent to application 
	if (IOCtl.dwOutBufferSize != sizeof(EVENT_PACKET))
	{
		Out_Debug_String(("HAcc Driver: ERROR IOCtlReadEventPacket OutBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

	// Adresse des Eventpaketes
	pEventPacket = (EVENT_PACKET*)IOCtl.pInBuffer;
	if (!pEventPacket)
	{
        Out_Debug_String("HAcc Driver: ERROR IOCtlReadEventPacket pInputBuffer = NULL!\n");
		return STATUS_INVALID_PARAMETER; 
	}
	
	// Pointer to Outputbuffer
	if (!IOCtl.pOutBuffer)
	{
        Out_Debug_String("HAcc Driver: ERROR IOCtlReadEventPacket pOutputBuffer = NULL\n\r");
		return STATUS_INVALID_PARAMETER;
	}

	// Adresse der Config-Struktur...
	pConfig = (CONFIG*)pEventPacket->hDevice;
	if (!pConfig)
	{
        Out_Debug_String("HAcc Driver: ERROR IOCtlReadEventPacket pConfig = NULL!\n");
		return STATUS_INVALID_PARAMETER; 
	}

	wRdTP = pConfig->wRdTP;
	
	// ReadIndex ok?
	if (wRdTP >= MAX_EVENTS)
	{
        Out_Debug_String("HAcc Driver: ERROR IOCtlReadEventPacket wRdTP too large!\n\r");
		return STATUS_INVALID_PARAMETER; 
	}
  	
	// Liegt eine Message vor.
	if (!pConfig->TracePacket[wRdTP].bValid)
		return STATUS_SUCCESS;

	// Message kopieren
	MemCopy(pEventPacket, &pConfig->TracePacket[wRdTP], sizeof(EVENT_PACKET));

	// Und Messagebuffer freigeben
	pConfig->TracePacket[wRdTP].bValid = FALSE;

	// TracePacketbuffer zyklisch weiterzählen.
	wRdTP++;
	if (wRdTP == MAX_EVENTS)
		wRdTP = 0;
	pConfig->wRdTP = wRdTP;

	// Bytes to returned
    *IOCtl.pBytesToReturn = sizeof(EVENT_PACKET);

    return STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
NTSTATUS IOCtlMapPhysToLin(IOCtlStruct IOCtl)
{
	MEMSTRUCT			*pMem	= NULL;	
  
	// Size of buffer containing data from application 
 	if (IOCtl.dwInBufferSize != sizeof(MEMSTRUCT))
	{
		Out_Debug_String(("HAcc Driver: ERROR IOCtlMapPhysToLin InBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

    // Size of buffer for data to be sent to application 
	if (IOCtl.dwOutBufferSize != sizeof(MEMSTRUCT))
	{
		Out_Debug_String(("HAcc Driver: ERROR IOCtlMapPhysToLin OutBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

	pMem = (MEMSTRUCT*)IOCtl.pInBuffer;
	if (!pMem)
	{
		Out_Debug_String(("HAcc Driver: ERROR IOCtlMapPhysToLin pInputBuffer = NULL\n"));
		return STATUS_INVALID_PARAMETER;
	}

	// Pointer to Outputbuffer
	if (!IOCtl.pOutBuffer)
	{
        Out_Debug_String("HAcc Driver: ERROR IOCtlMapPhysToLin pOutputBuffer = NULL\n\r");
		return STATUS_INVALID_PARAMETER;
	}

	if (!MapPhysToLin(pMem))
		return STATUS_INVALID_PARAMETER;

	// Bytes to returned
    *IOCtl.pBytesToReturn = sizeof(MEMSTRUCT);

	return STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
NTSTATUS IOCtlUnmapPhysToLin(IOCtlStruct IOCtl)
{
	MEMSTRUCT			*pMem	= NULL;	
  
	// Size of buffer containing data from application 
 	if (IOCtl.dwInBufferSize != sizeof(MEMSTRUCT))
	{
		Out_Debug_String(("HAcc Driver: ERROR IOCtlUnmapPhysToLin InBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

    // Size of buffer for data to be sent to application 
	if (IOCtl.dwOutBufferSize != sizeof(MEMSTRUCT))
	{
		Out_Debug_String(("HAcc Driver: ERROR IOCtlUnmapPhysToLin OutBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

	pMem = (MEMSTRUCT*)IOCtl.pInBuffer;
	if (!pMem)
	{
		Out_Debug_String(("HAcc Driver: ERROR IOCtlUnmapPhysToLin pInputBuffer = NULL\n"));
		return STATUS_INVALID_PARAMETER;
	}

	// Pointer to Outputbuffer
	if (!IOCtl.pOutBuffer)
	{
        Out_Debug_String("HAcc Driver: ERROR IOCtlUnmapPhysToLin pOutputBuffer = NULL\n\r");
		return STATUS_INVALID_PARAMETER;
	}

	if (!UnmapPhysToLin(pMem))
		return STATUS_INVALID_PARAMETER;

	// Bytes to returned
    *IOCtl.pBytesToReturn = sizeof(MEMSTRUCT);

	return STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
NTSTATUS IOCtlMapLinToPhys(IOCtlStruct IOCtl)
{
	MEMSTRUCT			*pMem	= NULL;	
  
	// Size of buffer containing data from application 
 	if (IOCtl.dwInBufferSize != sizeof(MEMSTRUCT))
	{
		Out_Debug_String(("HAcc Driver: ERROR IOCtlMapLinToPhys InBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

    // Size of buffer for data to be sent to application 
	if (IOCtl.dwOutBufferSize != sizeof(MEMSTRUCT))
	{
		Out_Debug_String(("HAcc Driver: ERROR IOCtlMapLinToPhys OutBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

	pMem = (MEMSTRUCT*)IOCtl.pInBuffer;
	if (!pMem)
	{
		Out_Debug_String(("HAcc Driver: ERROR IOCtlMapLinToPhys pInputBuffer = NULL\n"));
		return STATUS_INVALID_PARAMETER;
	}

	// Pointer to Outputbuffer
	if (!IOCtl.pOutBuffer)
	{
        Out_Debug_String("HAcc Driver: ERROR IOCtlMapLinToPhys pOutputBuffer = NULL\n\r");
		return STATUS_INVALID_PARAMETER;
	}

	if (!MapLinToPhys(pMem))
		return STATUS_INVALID_PARAMETER;

	// Bytes to returned
    *IOCtl.pBytesToReturn = sizeof(MEMSTRUCT);

	return STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
NTSTATUS IOCtlAllocateMemory(IOCtlStruct IOCtl)
{
	MEMSTRUCT			*pMem	= NULL;	
  
	// Size of buffer containing data from application 
 	if (IOCtl.dwInBufferSize != sizeof(MEMSTRUCT))
	{
		Out_Debug_String(("HAcc Driver: ERROR IOCtlMapPhysToLin InBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

    // Size of buffer for data to be sent to application 
	if (IOCtl.dwOutBufferSize != sizeof(MEMSTRUCT))
	{
		Out_Debug_String(("HAcc Driver: ERROR IOCtlMapPhysToLin OutBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

	pMem = (MEMSTRUCT*)IOCtl.pInBuffer;
	if (!pMem)
	{
		Out_Debug_String(("HAcc Driver: ERROR IOCtlMapPhysToLin pInputBuffer = NULL\n"));
		return STATUS_INVALID_PARAMETER;
	}

	// Pointer to Outputbuffer
	if (!IOCtl.pOutBuffer)
	{
        Out_Debug_String("HAcc Driver: ERROR IOCtlMapPhysToLin pOutputBuffer = NULL\n\r");
		return STATUS_INVALID_PARAMETER;
	}

	if (!PageAllocate(pMem, pMem->dwLen, TRUE))
	{
        Out_Debug_String("HAcc Driver: ERROR PageAllocate failed\n\r");
		return STATUS_INVALID_PARAMETER;
	}

	// Bytes to returned
    *IOCtl.pBytesToReturn = sizeof(MEMSTRUCT);

	return STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
NTSTATUS IOCtlFreeMemory(IOCtlStruct IOCtl)
{
	MEMSTRUCT			*pMem	= NULL;	
  
	// Size of buffer containing data from application 
 	if (IOCtl.dwInBufferSize != sizeof(MEMSTRUCT))
	{
		Out_Debug_String(("HAcc Driver: ERROR IOCtlMapPhysToLin InBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

    // Size of buffer for data to be sent to application 
	if (IOCtl.dwOutBufferSize != sizeof(MEMSTRUCT))
	{
		Out_Debug_String(("HAcc Driver: ERROR IOCtlMapPhysToLin OutBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

	pMem = (MEMSTRUCT*)IOCtl.pInBuffer;
	if (!pMem)
	{
		Out_Debug_String(("HAcc Driver: ERROR IOCtlMapPhysToLin pInputBuffer = NULL\n"));
		return STATUS_INVALID_PARAMETER;
	}

	// Pointer to Outputbuffer
	if (!IOCtl.pOutBuffer)
	{
        Out_Debug_String("HAcc Driver: ERROR IOCtlMapPhysToLin pOutputBuffer = NULL\n\r");
		return STATUS_INVALID_PARAMETER;
	}

	if (!PageFree(pMem,  TRUE))
	{
        Out_Debug_String("HAcc Driver: ERROR PageFree failed\n\r");
		return STATUS_INVALID_PARAMETER;
	}

	// Bytes to returned
    *IOCtl.pBytesToReturn = sizeof(MEMSTRUCT);

	return STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
NTSTATUS IOCtlGetFrameBuffer(IOCtlStruct IOCtl)
{
	MEMSTRUCT			*pMem	= NULL;	
  
	// Size of buffer containing data from application 
 	if (IOCtl.dwInBufferSize != sizeof(MEMSTRUCT))
	{
		Out_Debug_String(("HAcc Driver: ERROR IOCtlMapPhysToLin InBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

    // Size of buffer for data to be sent to application 
	if (IOCtl.dwOutBufferSize != sizeof(MEMSTRUCT))
	{
		Out_Debug_String(("HAcc Driver: ERROR IOCtlMapPhysToLin OutBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

	pMem = (MEMSTRUCT*)IOCtl.pInBuffer;
	if (!pMem)
	{
		Out_Debug_String(("HAcc Driver: ERROR IOCtlMapPhysToLin pInputBuffer = NULL\n"));
		return STATUS_INVALID_PARAMETER;
	}

	// Pointer to Outputbuffer
	if (!IOCtl.pOutBuffer)
	{
        Out_Debug_String("HAcc Driver: ERROR IOCtlMapPhysToLin pOutputBuffer = NULL\n\r");
		return STATUS_INVALID_PARAMETER;
	}

	pMem->pPhysAddr = g_FrameBuffer.pPhysAddr;
	pMem->pLinAddr  = g_FrameBuffer.pLinAddr;
	pMem->dwLen		= g_FrameBuffer.dwLen;

	if (!pMem || pMem->pPhysAddr ==NULL || pMem->pLinAddr == NULL)
	{
        Out_Debug_String("HAcc Driver: ERROR PageAllocate failed\n\r");
		return STATUS_INVALID_PARAMETER;
	}

	// Bytes to returned
    *IOCtl.pBytesToReturn = sizeof(MEMSTRUCT);

	return STATUS_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////
// Kopiert auf einem AMD 1200MHz mit SDRAM 100 etwa 395 MByte/Sekunde
// (Ist die Bufferlänge ein Vielfaches von CACHEBLOCK, so ist das Kopieren am schnellsten)
// Quelle: http://www.sgi.com/developers/technology/irix/resources/asc_cpu.html
// MMX memcpy with prefetch, non-temporal stores, and streaming reads/writes
void FastMemCopy(void* pDestination, void* pSource, DWORD dwLength)
{
	BYTE fpBuffer[256];
	int  nI = 0;
	if (!pDestination || !pSource || !dwLength)
		return;

	// Wird MMX sowie die SFENCE-Instruktion unterstützt?
	if (!g_bHasMMX || !g_bHasSFence)
		RtlCopyMemory(pDestination, pSource, dwLength);
	else if ((dwLength % 64) == 0)
	{
		// Suche die Prefetch-größe, bei dem dwLen ein vielfaches der Prefetch-größe ist
		// (Es werden nur 2er Potenzen untersucht, ab 64Byte Größe (Also 64, 128, 256, 512...)
		DWORD dwPrefetchSize = 0;
		for (nI = 6; nI<32;nI++)
		{
			if (TSTBIT(dwLength, nI))
			{
				dwPrefetchSize = SETBIT(dwPrefetchSize, nI);
				break;
			}
		}

		if (dwPrefetchSize)
		{
			dwPrefetchSize /= 8;			// Numbers of QWORDS
			dwLength /= 8;					// Numbers of QWORDS
			__asm
			{
				fsave fpBuffer	
				fwait

				push eax
				push ebx
				push ecx
				push edi
				push esi

				mov esi, pSource			// source array
				mov edi, pDestination		// destination array
				mov ecx, dwLength			// number of QWORDS (8bytes) (assumes len / CACHEBLOCK is an integer)
				lea esi, [esi+ecx*8]		// end of source
				lea edi, [edi+ecx*8]		// end of destination		
				neg ecx						// use anegative offset as a combo pointer-and-loop-counter

				mainloop:
					mov eax, dwPrefetchSize			// note: prefetch loop is unrolled 2X
					shr eax, 4						// Buffersize / 16
					add ecx, dwPrefetchSize			// move up to end of block

					prefetchloop:
						mov ebx, [esi+ecx*8-64]		// read one address in this cache line...
						mov ebx, [esi+ecx*8-128]	// ... and one in the previous line
						sub ecx, 16					// 16 QWORDS = 2 64-byte cache lines
						dec eax
						jnz prefetchloop

					mov eax, dwPrefetchSize
					shr eax, 3						// Buffersize / 8

					writeloop:
						movq mm0, qword ptr [esi+ecx*8+0]		// reads from cache!
						movq mm1, qword ptr [esi+ecx*8+8]		// reads from cache!
						movq mm2, qword ptr [esi+ecx*8+16]		// reads from cache!
						movq mm3, qword ptr [esi+ecx*8+24]		// reads from cache!
						movq mm4, qword ptr [esi+ecx*8+32]		// reads from cache!
						movq mm5, qword ptr [esi+ecx*8+40]		// reads from cache!
						movq mm6, qword ptr [esi+ecx*8+48]		// reads from cache!
						movq mm7, qword ptr [esi+ecx*8+56]		// reads from cache!

						movntq qword ptr [edi+ecx*8+0], mm0		// streaming store
						movntq qword ptr [edi+ecx*8+8], mm1		// streaming store
						movntq qword ptr [edi+ecx*8+16], mm2	// streaming store
						movntq qword ptr [edi+ecx*8+24], mm3	// streaming store
						movntq qword ptr [edi+ecx*8+32], mm4	// streaming store
						movntq qword ptr [edi+ecx*8+40], mm5	// streaming store
						movntq qword ptr [edi+ecx*8+48], mm6	// streaming store
						movntq qword ptr [edi+ecx*8+56], mm7	// streaming store

						add ecx, 8
						dec eax
						jnz writeloop

					or ecx, ecx 	// assumes integer number of cacheblocks
					jnz mainloop
				
				sfence

				pop esi
				pop edi
				pop ecx
				pop ebx
				pop eax
				
				emms
				frstor	fpBuffer
				fwait
			}
		}
		else
		{
			// Dies ist die dritt schnellste Kopierfunktion
			_asm
			{
				push ecx
				push edi
				push esi

				mov     esi, pSource
				mov     edi, pDestination
				mov		ecx, dwLength
				cld			 
				rep		movsb

				pop esi
				pop edi
				pop ecx
			}
		}
	}
	else
		RtlCopyMemory(pDestination, pSource, dwLength);
}

/////////////////////////////////////////////////////////////////////////////////////
void MemCopy(void* pDestination, void* pSource, DWORD dwLength)
{
	DWORD	*pDest		= (DWORD*)pDestination;
	DWORD	*pSrc		= (DWORD*)pSource;
	DWORD	dwLen		= dwLength;

	if (!pDest || !pSrc || !dwLen)
		return;

	_asm{
  		  mov		ecx, dwLen
		  mov       esi, pSrc
		  mov       edi, pDest
		  cld			 
		  rep		movs
	}
}

/////////////////////////////////////////////////////////////////////////////////////
BOOL SendDebugMessageToUnit0(CONFIG* pConfig, WORD wEventType, char* pMsg)
{
	return SendDebugMessageToUnit(pConfig, wEventType, pMsg, 0, 0, 0, 0, 0);
}

/////////////////////////////////////////////////////////////////////////////////////
BOOL SendDebugMessageToUnit1(CONFIG* pConfig, WORD wEventType, char* pMsg, DWORD dwP1)
{
	return SendDebugMessageToUnit(pConfig, wEventType, pMsg, 1, dwP1, 0, 0, 0);
}

/////////////////////////////////////////////////////////////////////////////////////
BOOL SendDebugMessageToUnit2(CONFIG* pConfig, WORD wEventType, char* pMsg, DWORD dwP1, DWORD dwP2)
{
	return SendDebugMessageToUnit(pConfig, wEventType, pMsg, 2, dwP1, dwP2, 0, 0);
}

/////////////////////////////////////////////////////////////////////////////////////
BOOL SendDebugMessageToUnit3(CONFIG* pConfig, WORD wEventType, char* pMsg, DWORD dwP1, DWORD dwP2, DWORD dwP3)
{
	return SendDebugMessageToUnit(pConfig, wEventType, pMsg, 3, dwP1, dwP2, dwP3, 0);
}

/////////////////////////////////////////////////////////////////////////////////////
BOOL SendDebugMessageToUnit4(CONFIG* pConfig, WORD wEventType, char* pMsg, DWORD dwP1, DWORD dwP2, DWORD dwP3, DWORD dwP4)
{
	return SendDebugMessageToUnit(pConfig, wEventType, pMsg, 4, dwP1, dwP2, dwP3, dwP4);
}

/////////////////////////////////////////////////////////////////////////////////////
BOOL SendDebugMessageToUnit(CONFIG* pConfig, WORD wEventType, char* pMsg, DWORD dwNP, DWORD dwP1, DWORD dwP2, DWORD dwP3, DWORD dwP4)
{
	DWORD   dwLen	= 0;
	DWORD	dwI;
	WORD	wWrTP = pConfig->wWrTP;

	// Ein wenig die Parameter überprüfen
	if ((pConfig == NULL) ||
		(pMsg == NULL) ||
		(pConfig->hDebugEvent == 0) ||
		(dwNP > 4))
		return FALSE;

	// Länge des String ermitteln.
	for (dwI = 0; dwI < MAX_MSG_LEN; dwI++)
	{
		if (pMsg[dwI] == '\0')
			break;
	}

	// Länge auf Maximallänge kürzen.
	dwLen = MIN(dwI+1, MAX_MSG_LEN);

	// Länge Ok?
	if (dwLen == 0)
		return FALSE;

	// Vorheriges Packet schon angekommen?
	if (pConfig->TracePacket[wWrTP].bValid)
		return TRUE;

	// DebugPacket füllen...
	for (dwI = 0; dwI < dwLen;dwI++)
		pConfig->TracePacket[wWrTP].Message[dwI] = pMsg[dwI];

	pConfig->TracePacket[wWrTP].wEventType  = wEventType;
	pConfig->TracePacket[wWrTP].dwNumParam  = dwNP;
	pConfig->TracePacket[wWrTP].dwParam1	= dwP1;
	pConfig->TracePacket[wWrTP].dwParam2	= dwP2;
	pConfig->TracePacket[wWrTP].dwParam3	= dwP3;
	pConfig->TracePacket[wWrTP].dwParam4	= dwP4;
	pConfig->TracePacket[wWrTP].bValid		= TRUE;

	// TracePacketbuffer zyklisch weiterzählen.
	wWrTP++;
	if (wWrTP == MAX_EVENTS)
		wWrTP = 0;
	pConfig->wWrTP = wWrTP;
		
	// ...und abschicken.
	SetDebugEvent(pConfig);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////
BOOL CleanUp(CONFIG *pConfig)
{
    MEMSTRUCT	Mem;
	BOOL		bStatus = FALSE;
	int			nI		= 0;

	Out_Debug_String("HAcc Driver: CleanUp\n\r");

	if (!pConfig)
		return FALSE;

	// Interrupt Service Routine deinstallieren
	RemoveISR(pConfig);

	// IO-Bereich 'unmappen'
	if (pConfig->wBusType == BUSTYPE_PCI) 
	{
		UnmapIOSpace(pConfig);
	}

	// Alle Events schließen
	CloseAllEventHandles(pConfig);

	// Configurationsspeicher freigeben
	Mem.pLinAddr = pConfig;
	Mem.dwLen	 = sizeof(CONFIG);
	PageFree(&Mem, FALSE);

	// Backupadresse löschen
	if (g_pBackupConfig[0] == pConfig)
		g_pBackupConfig[0] = NULL;
	if (g_pBackupConfig[1] == pConfig)
		g_pBackupConfig[1] = NULL;
	if (g_pBackupConfig[2] == pConfig)
		g_pBackupConfig[2] = NULL;
	if (g_pBackupConfig[3] == pConfig)
		g_pBackupConfig[3] = NULL;

	return(bStatus);
}

/////////////////////////////////////////////////////////////////////////////
void WaitMicroSecs(CONFIG *pConfig, DWORD dwMicros)
{
	LARGE_INTEGER Start64;
	LARGE_INTEGER Stop64;
	DWORD dwTimeDiff; 

	if (!pConfig)
		return;

	Start64 = ReadRDTSC();
	Stop64.QuadPart  = Start64.QuadPart +  (pConfig->dwCPUClk / 1000000) * dwMicros;
	do
	{
		Start64 = ReadRDTSC();
	}while (Start64.QuadPart < Stop64.QuadPart);

}

/////////////////////////////////////////////////////////////////////////////
LARGE_INTEGER ReadRDTSC()
{
	LARGE_INTEGER lRDTSC;
	_asm {             
		_asm push eax  
		_asm push edx  
		_asm _emit 0Fh 
		_asm _emit 31h 
		_asm mov lRDTSC.LowPart, eax 
		_asm mov lRDTSC.HighPart, edx
		_asm pop edx            
		_asm pop eax            
	} 
	return lRDTSC;
}

//////////////////////////////////////////////////////////////////////
BOOL HasMMX()
{
	BOOL bResult = TRUE;

	_asm
	{
		mov	EAX, 1					; request for feature flags
		CPUID						; 0Fh, 0A2h   CPUID Instruction
		test	EDX, 00800000h		; is MMX technology Bit(bit 23)in feature
									; flags equal to 1
		jnz	Found
		mov bResult, 0
		jmp End
	Found:
		mov bResult, 1
	End:
	}
	return bResult;
}

//////////////////////////////////////////////////////////////////////
// Überprüft, ob der SFENCE-Befehl unterstützt wird.
// Dies ist z.B. bei den VIA C3 Prozessoren nicht der Fall.
BOOL SupportCheckForSFence()
{
	_try
	{
		__asm sfence	// Löst dieser Befehl eine Exception aus?
	}
	_except(EXCEPTION_EXECUTE_HANDLER)
	{
		if (_exception_code()==STATUS_ILLEGAL_INSTRUCTION)
			return FALSE;	// Streaming SIMD Extensions not supported
	}

	// Streaming SIMD Extensions are supported by OS
	return TRUE;
}
