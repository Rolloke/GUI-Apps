/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaSys
// FILE:		$Workfile: TashaCommon.c $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaSys/TashaCommon.c $
// CHECKIN:		$Date: 15.06.04 15:14 $
// VERSION:		$Revision: 10 $
// LAST CHANGE:	$Modtime: 15.06.04 12:34 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

// Common-part of Tasha Driver
/////////////////////////////////////////////////////////////////////////////////////

#include "Driver.h"

extern BOOL			g_bHasSFence;
extern BOOL			g_bHasMMX;
extern MEMSTRUCT	g_FrameBuffer[4];

/////////////////////////////////////////////////////////////////////////////////////
NTSTATUS IOCtlDispatch(int nControlCode, IOCtlStruct IOCtl)
{
	NTSTATUS Status = STATUS_NOT_IMPLEMENTED;

	switch (nControlCode)
	{
		case TASHA_GET_DRIVERVERSION:
			Status = IOCtlGetVersion(IOCtl);
			break;
		case TASHA_WRITE_PORT:
			Status = IOCtlWritePort(IOCtl);
			break;
		case TASHA_READ_PORT:
			Status = IOCtlReadPort(IOCtl);
			break;
		case TASHA_WRITE_MEMORY:
			Status = IOCtlWriteMemory(IOCtl);
			break;
		case TASHA_READ_MEMORY:
			Status = IOCtlReadMemory(IOCtl);
			break;
		case TASHA_GET_FRAME_BUFFER:
			Status = IOCtlGetFrameBuffer(IOCtl);
			break;
		case TASHA_GET_TRANSFER_BUFFER:
			Status = IOCtlGetTransferBuffer(IOCtl);
			break;
		default:
			Out_Debug_String("Tasha Driver: Unknown Controlcode\n\r");
			break;
	}

	return Status;
}

///////////////////////////////////////////////////////////////////////////////////
NTSTATUS IOCtlGetVersion(IOCtlStruct IOCtl)
{
	DWORD	*pIO	= NULL;	

    Out_Debug_String("Tasha Driver: IOCtlGetVersion\n");

	// Size of buffer containing data from application 
 	if (IOCtl.dwInBufferSize != 0)
	{
		Out_Debug_String(("Tasha Driver: ERROR IOCtlGetVersion InBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

    // Size of buffer for data to be sent to application 
	if (IOCtl.dwOutBufferSize != sizeof(DWORD))
	{
		Out_Debug_String(("Tasha Driver: ERROR IOCtlGetVersion OutBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

	// Pointer to Outputbuffer
	if (!IOCtl.pOutBuffer)
	{
        Out_Debug_String("Tasha Driver: ERROR IOCtlGetVersion pOutputBuffer = NULL\n\r");
		return STATUS_INVALID_PARAMETER;
	}

	// Versionsnummer zurückliefern
 	*(DWORD*)IOCtl.pOutBuffer	= TASHA_UNIT_VERSION;

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
		Out_Debug_String(("Tasha Driver: ERROR IOCtlWritePort InBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

    // Size of buffer for data to be sent to application 
	if (IOCtl.dwOutBufferSize != 0)
	{
		Out_Debug_String(("Tasha Driver: ERROR IOCtlWritePort OutBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}
  
	pIoIn = (IOSTRUCT*)IOCtl.pInBuffer;
	if (!pIoIn)
	{
		Out_Debug_String("Tasha Driver: ERROR IOCtlWritePort pInputBuffer = NULL!\n");
		return STATUS_INVALID_PARAMETER;
	}

	switch (pIoIn->wIOSize)
	{
		case IO_SIZE8:
			ISAOutput8(pIoIn->dwAddr, pIoIn->byVal);
			break;
		case IO_SIZE16:
			ISAOutput16(pIoIn->dwAddr, pIoIn->wVal);
			break;
		case IO_SIZE32:
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
		Out_Debug_String(("Tasha Driver: ERROR IOCtlReadPort InBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

    // Size of buffer for data to be sent to application 
 	if (IOCtl.dwOutBufferSize != sizeof(IOSTRUCT))
	{
		Out_Debug_String(("Tasha Driver: ERROR IOCtlReadPort OutBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}
    
	pIoIn = (IOSTRUCT*)IOCtl.pInBuffer;
	if (!pIoIn)
	{
		Out_Debug_String("Tasha Driver: ERROR IOCtlReadPort pInputBuffer = NULL!\n");
		return STATUS_INVALID_PARAMETER;
	}

	// Pointer to Outputbuffer
	if (!IOCtl.pOutBuffer)
	{
        Out_Debug_String("Tasha Driver: ERROR IOCtlReadPort pOutputBuffer = NULL\n\r");
		return STATUS_INVALID_PARAMETER;
	}

	switch (pIoIn->wIOSize)
	{
		case IO_SIZE8:
			pIoIn->byVal = ISAInput8(pIoIn->dwAddr);
			break;
		case IO_SIZE16:
			pIoIn->wVal = ISAInput16(pIoIn->dwAddr);
			break;
		case IO_SIZE32:
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
		Out_Debug_String(("Tasha Driver: ERROR IOCtlWriteMemory InBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

    // Size of buffer for data to be sent to application 
	if (IOCtl.dwOutBufferSize != 0)
	{
		Out_Debug_String(("Tasha Driver: ERROR IOCtlWriteMemory OutBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}
  
	pIoIn = (IOSTRUCT*)IOCtl.pInBuffer;
	if (!pIoIn)
	{
		Out_Debug_String("Tasha Driver: ERROR IOCtlWriteMemory pInputBuffer = NULL!\n");
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
		Out_Debug_String(("Tasha Driver: ERROR IOCtlReadMemory InBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

    // Size of buffer for data to be sent to application 
 	if (IOCtl.dwOutBufferSize != sizeof(IOSTRUCT))
	{
		Out_Debug_String(("Tasha Driver: ERROR IOCtlReadMemory OutBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}
    
	pIoIn = (IOSTRUCT*)IOCtl.pInBuffer;
	if (!pIoIn)
	{
		Out_Debug_String("Tasha Driver: ERROR IOCtlReadMemory pInputBuffer = NULL!\n");
		return STATUS_INVALID_PARAMETER;
	}

	// Pointer to Outputbuffer
	if (!IOCtl.pOutBuffer)
	{
        Out_Debug_String("Tasha Driver: ERROR IOCtlReadMemory pOutputBuffer = NULL\n\r");
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
NTSTATUS IOCtlGetFrameBuffer(IOCtlStruct IOCtl)
{
	FRAMEBUFFERSTRUCT	*pMem	= NULL;	
	int			nCard	= 0;

	// Size of buffer containing data from application 
 	if (IOCtl.dwInBufferSize != sizeof(FRAMEBUFFERSTRUCT))
	{
		Out_Debug_String(("Tasha Driver: ERROR IOCtlGetFrameBuffer InBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

    // Size of buffer for data to be sent to application 
	if (IOCtl.dwOutBufferSize != sizeof(FRAMEBUFFERSTRUCT))
	{
		Out_Debug_String(("Tasha Driver: ERROR IOCtlGetFrameBuffer OutBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

	pMem = (FRAMEBUFFERSTRUCT*)IOCtl.pInBuffer;
	if (!pMem)
	{
		Out_Debug_String(("Tasha Driver: ERROR IOCtlGetFrameBuffer pInputBuffer = NULL\n"));
		return STATUS_INVALID_PARAMETER;
	}

	// Pointer to Outputbuffer
	if (!IOCtl.pOutBuffer)
	{
        Out_Debug_String("Tasha Driver: ERROR IOCtlGetFrameBuffer pOutputBuffer = NULL\n\r");
		return STATUS_INVALID_PARAMETER;
	}
	
	// Über pMem->dwLen wird der Kartenindex nach unten gereicht.
	if ((pMem->nCardIndex < 0) || (pMem->nCardIndex > MAX_TASHA_BOARDS))
	{
        Out_Debug_String("Tasha Driver: ERROR nCard is unvalid\n\r");
		return STATUS_INVALID_PARAMETER;
	}

	pMem->Buffer.pPhysAddr	= g_FrameBuffer[pMem->nCardIndex].pPhysAddr;
	pMem->Buffer.pLinAddr	= g_FrameBuffer[pMem->nCardIndex].pLinAddr;
	pMem->Buffer.dwLen		= g_FrameBuffer[pMem->nCardIndex].dwLen;

	if (!pMem || pMem->Buffer.pPhysAddr == NULL || pMem->Buffer.pLinAddr == NULL)
	{
        Out_Debug_String("Tasha Driver: ERROR PageAllocate failed\n\r");
		return STATUS_INVALID_PARAMETER;
	}

	// Bytes to returned
    *IOCtl.pBytesToReturn = sizeof(FRAMEBUFFERSTRUCT);

	return STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
NTSTATUS IOCtlGetTransferBuffer(IOCtlStruct IOCtl)
{
	FRAMEBUFFERSTRUCT	*pMem	= NULL;	
	int			nCard	= 0;

	// Size of buffer containing data from application 
 	if (IOCtl.dwInBufferSize != sizeof(FRAMEBUFFERSTRUCT))
	{
		Out_Debug_String(("Tasha Driver: ERROR IOCtlGetTransferBuffer InBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

    // Size of buffer for data to be sent to application 
	if (IOCtl.dwOutBufferSize != sizeof(FRAMEBUFFERSTRUCT))
	{
		Out_Debug_String(("Tasha Driver: ERROR IOCtlGetTransferBuffer OutBuffersize wrong\n"));
		return STATUS_INVALID_PARAMETER;
	}

	pMem = (FRAMEBUFFERSTRUCT*)IOCtl.pInBuffer;
	if (!pMem)
	{
		Out_Debug_String(("Tasha Driver: ERROR IOCtlGetTransferBuffer pInputBuffer = NULL\n"));
		return STATUS_INVALID_PARAMETER;
	}

	// Pointer to Outputbuffer
	if (!IOCtl.pOutBuffer)
	{
        Out_Debug_String("Tasha Driver: ERROR IOCtlGetTransferBuffer pOutputBuffer = NULL\n\r");
		return STATUS_INVALID_PARAMETER;
	}
	
	// Über pMem->dwLen wird der Kartenindex nach unten gereicht.
	if ((pMem->nCardIndex < 0) || (pMem->nCardIndex > MAX_TASHA_BOARDS))
	{
        Out_Debug_String("Tasha Driver: ERROR nCard is unvalid\n\r");
		return STATUS_INVALID_PARAMETER;
	}

	pMem->Buffer.pPhysAddr	= g_FrameBuffer[pMem->nCardIndex].pPhysAddr;
	pMem->Buffer.pLinAddr	= g_FrameBuffer[pMem->nCardIndex].pLinAddr;
	pMem->Buffer.dwLen		= g_FrameBuffer[pMem->nCardIndex].dwLen;

	if (!pMem || pMem->Buffer.pPhysAddr == NULL || pMem->Buffer.pLinAddr == NULL)
	{
        Out_Debug_String("Tasha Driver: ERROR PageAllocate failed\n\r");
		return STATUS_INVALID_PARAMETER;
	}

	// Bytes to returned
    *IOCtl.pBytesToReturn = sizeof(FRAMEBUFFERSTRUCT);

	return STATUS_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////
// Helper function...
/////////////////////////////////////////////////////////////////////////////////////

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


///////////////////////////////////////////////////////////////////////////////////////////////////////////
BYTE PCIInput8(DWORD dwPort)
{
	return *(BYTE*)(dwPort);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
WORD PCIInput16(DWORD dwPort)
{
	return *(WORD*)(dwPort);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
DWORD PCIInput32(DWORD dwPort)
{
	return *(DWORD*)(dwPort);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void PCIInputX(void* pDestBuffer, DWORD dwPort, DWORD dwLen)
{
	FastMemCopy(pDestBuffer, (void*)(dwPort), dwLen);
//	RtlCopyMemory(pDestBuffer, (void*)(dwPort), dwLen);
	RtlZeroMemory((void*)(dwPort), dwLen); // Löschen, damit Daten nicht ein zweites mal gefunden werden.
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void PCIOutput8(DWORD dwPort, BYTE byData)
{
	*(BYTE*)(dwPort) = byData;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void PCIOutput16(DWORD dwPort, WORD wData) 
{
	*(WORD*)(dwPort) = wData;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void PCIOutput32(DWORD dwPort, DWORD dwData) 
{
	*(DWORD*)(dwPort) = dwData;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void PCIOutputX(void* pSrcBuffer, DWORD dwPort, DWORD dwLen)
{
	FastMemCopy((void*)(dwPort), pSrcBuffer, dwLen);
//	RtlCopyMemory((void*)(dwPort), pSrcBuffer, dwLen);
}

/////////////////////////////////////////////////////////////////////////////////////
void ISAOutput8(DWORD dwAddr, BYTE byVal)
{
	WRITE_PORT_UCHAR((PUCHAR)dwAddr, byVal);
}

/////////////////////////////////////////////////////////////////////////////////////
void ISAOutput16(DWORD dwAddr, WORD wVal)
{
	WRITE_PORT_USHORT((PUSHORT)dwAddr, wVal);
}

/////////////////////////////////////////////////////////////////////////////////////
void ISAOutput32(DWORD dwAddr, DWORD dwVal)
{
	WRITE_PORT_ULONG((PULONG)dwAddr, dwVal);
}

/////////////////////////////////////////////////////////////////////////////////////
BYTE ISAInput8(DWORD dwAddr)
{
    return READ_PORT_UCHAR((PUCHAR)dwAddr);
}

/////////////////////////////////////////////////////////////////////////////////////
WORD ISAInput16(DWORD dwAddr)
{
    return READ_PORT_USHORT((PUSHORT)dwAddr);
}

/////////////////////////////////////////////////////////////////////////////////////
DWORD ISAInput32(DWORD dwAddr)
{
    return READ_PORT_ULONG((PULONG)dwAddr);
}


