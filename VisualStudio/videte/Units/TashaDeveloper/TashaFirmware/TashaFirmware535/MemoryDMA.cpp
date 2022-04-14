#include <gpiotest.h>
#include <CdefBF535.h>
#include <sys/exception.h>
#include <stdio.h>
#include <string.h> 
#include "..\Include\Helper.h"
#include "..\Include\TMMessages.h"
#include "TashaFirmware535.h"
#include "MemoryDMA.h"

section ("DescriptorSrc") volatile WORD DescriptorSrc[MAX_DESCIPTOR_LIST_SIZE][6];
section ("DescriptorDst") volatile WORD DescriptorDst[MAX_DESCIPTOR_LIST_SIZE][6];

volatile BOOL g_bDMAComplete = FALSE;

///////////////////////////////////////////////////////////////////////////////////
BOOL MemCopyDma(BYTE* pDst, BYTE* pSrc, DWORD dwLen, BOOL bWait/*=TRUE*/)
{
	BOOL bResult = FALSE;
	
	// Warte bis ein evtl. noch laufender MemoryDMA abgeschlossen ist.
	WaitUntilMemCopyDmaCompleted();

	// Descriptorliste erzeugen
	if (CreateSingleChannelDescriptorList(pDst, pSrc, dwLen))
	{
		EnableDma();
		
		if (bWait)
		{
			while(!IsMemCopyDmaReadyEx()){};
			bResult = !TSTBIT(*pMDD_DCFG, 14); // DCS-Bit im StatusRegister
		}
		else
			bResult = TRUE;
	}
	
	return bResult;	
}

///////////////////////////////////////////////////////////////////////////////////
void WaitUntilMemCopyDmaCompleted()
{
	while(!IsMemCopyDmaReadyEx()){};
}

///////////////////////////////////////////////////////////////////////////////////
// Ausführungszeit ohne DMA-Transfer: 530 Cycles (1,7ys bei 300 MHz)
BOOL MultiChannelCopy(BYTE* pDst, DWORD dwDstSize, BYTE* pSrc, DWORD dwSrcSize, int nChannels, BOOL bWait/*TRUE*/)
{
	BOOL bResult = FALSE;

	// Warte bis ein evtl. noch laufender MemoryDMA abgeschlossen ist.
	WaitUntilMemCopyDmaCompleted();
	
	// Descriptorliste erzeugen
	if (CreateMultiChannelDescriptorList(pDst, dwDstSize, pSrc, dwSrcSize, nChannels))
	{
		EnableDma();
		
		if (bWait)
		{
			while(!IsMemCopyDmaReadyEx()){};
			bResult = !TSTBIT(*pMDD_DCFG, 14); // DCS-Bit im StatusRegister
		}
		else
			bResult = TRUE;
	}
	
	return bResult;	
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CreateSingleChannelDescriptorList(BYTE* pDst, BYTE* pSrc, DWORD dwLen)
{
	BOOL	bResult = FALSE;
	
	DWORD dwTransfers = dwLen / DMA_MEMORY_TRANSFER_SIZE;
	DWORD dwRestLen   = dwLen % DMA_MEMORY_TRANSFER_SIZE;
		
	if (dwTransfers < MAX_DESCIPTOR_LIST_SIZE)
	{
		DisableDma();	
		
		if (dwTransfers > 0)
		{
			for (int nI = 0; nI < dwTransfers; nI++)
			{
				// Source Descriptor
				DescriptorSrc[nI][0] = DCFG_DMA_EN | DCFG_SIZE32 | DCFG_DBO;
				DescriptorSrc[nI][1] = DMA_MEMORY_TRANSFER_SIZE/sizeof(DWORD);
				DescriptorSrc[nI][2] = LOWORD((DWORD)pSrc);
				DescriptorSrc[nI][3] = HIWORD((DWORD)pSrc);
				DescriptorSrc[nI][4] = LOWORD((DWORD)&DescriptorSrc[nI+1][0]);
						
				// Destination Descriptor
				DescriptorDst[nI][0] = DCFG_DMA_EN | DCFG_SIZE32 | DCFG_DBO;
				DescriptorDst[nI][1] = DMA_MEMORY_TRANSFER_SIZE/sizeof(DWORD);
				DescriptorDst[nI][2] = LOWORD((DWORD)pDst);
				DescriptorDst[nI][3] = HIWORD((DWORD)pDst);
				DescriptorDst[nI][4] = LOWORD((DWORD)&DescriptorDst[nI+1][0]);
				
				pDst += DMA_MEMORY_TRANSFER_SIZE;
				pSrc += DMA_MEMORY_TRANSFER_SIZE;			
			}
		}
		
		if (dwRestLen != 0)
		{
			// Source Descriptor
			DescriptorSrc[dwTransfers][0] = DCFG_DMA_EN | DCFG_SIZE32 | DCFG_DBO;
			DescriptorSrc[dwTransfers][1] = dwRestLen/sizeof(DWORD);
			DescriptorSrc[dwTransfers][2] = LOWORD((DWORD)pSrc);
			DescriptorSrc[dwTransfers][3] = HIWORD((DWORD)pSrc);
			DescriptorSrc[dwTransfers][4] = LOWORD((DWORD)&DescriptorSrc[dwTransfers][5]);
			DescriptorSrc[dwTransfers][5] = 0x0001;
			
			// Destination Descriptor
			DescriptorDst[dwTransfers][0] = DCFG_DMA_EN | DCFG_SIZE32 | DCFG_DBO;
			DescriptorDst[dwTransfers][1] = dwRestLen/sizeof(DWORD);
			DescriptorDst[dwTransfers][2] = LOWORD((DWORD)pDst);
			DescriptorDst[dwTransfers][3] = HIWORD((DWORD)pDst);
			DescriptorDst[dwTransfers][4] = LOWORD((DWORD)&DescriptorDst[dwTransfers][5]);
			DescriptorDst[dwTransfers][5] = 0x0001;
		}
		else
		{
			DescriptorSrc[dwTransfers-1][4] = LOWORD((DWORD)&DescriptorSrc[dwTransfers-1][5]);
			DescriptorSrc[dwTransfers-1][5] = 0x0001;
			DescriptorDst[dwTransfers-1][0] = DCFG_DMA_EN | DCFG_SIZE32 | DCFG_DBO;	
			DescriptorDst[dwTransfers-1][4] = LOWORD((DWORD)&DescriptorDst[dwTransfers-1][5]);
			DescriptorDst[dwTransfers-1][5] = 0x0001;
		}
		
		// DMA_Basepointer setzen
		*pDMA_DBP = HIWORD((DWORD)&DescriptorSrc[0][0]);
		
		// Destination Memory DMA Next Descriptor Pointer
		*pMDD_DND = LOWORD((DWORD)DescriptorDst);	
	
		// Source Memory DMA Next Descriptor Pointer
		*pMDS_DND = LOWORD((DWORD)DescriptorSrc);
				
		// Destinationbuffer löschen
		*pMDD_DCFG = *pMDD_DCFG | DCFG_CLEAR_BUFF;
		
		// Sourcebuffer löschen
		*pMDS_DCFG = *pMDD_DCFG | DCFG_CLEAR_BUFF;

		bResult = TRUE;
	}
	else
	{
		DoNotifyInformation(INFO_TOO_MANY_DESCRIPTORS, dwTransfers);	
		printf("CreateSingleChannelDescriptorList: Too many descriptors (%d>=%d)\n", dwTransfers, MAX_DESCIPTOR_LIST_SIZE);
	}
	
	
	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CreateMultiChannelDescriptorList(BYTE* pDst, DWORD dwDstSize, BYTE* pSrc, DWORD dwSrcSize, int nChannels)
{
	BOOL  bResult = FALSE;

	int nI = 0;

	if (nChannels >=1 && nChannels <= 8)
	{	
		DisableDma();	

		for (nI = 0; nI < nChannels-1; nI++)
		{
			// Source Descriptor
			DescriptorSrc[nI][0] = DCFG_DMA_EN | DCFG_SIZE32  | DCFG_DBO;
			DescriptorSrc[nI][1] = dwSrcSize/sizeof(DWORD);
			DescriptorSrc[nI][2] = LOWORD((DWORD)pSrc);
			DescriptorSrc[nI][3] = HIWORD((DWORD)pSrc);
			DescriptorSrc[nI][4] = LOWORD((DWORD)&DescriptorSrc[nI+1][0]);
					
			// Destination Descriptor
			DescriptorDst[nI][0] = DCFG_DMA_EN | DCFG_SIZE32 | DCFG_DBO;
			DescriptorDst[nI][1] = dwSrcSize/sizeof(DWORD);
			DescriptorDst[nI][2] = LOWORD((DWORD)pDst);
			DescriptorDst[nI][3] = HIWORD((DWORD)pDst);
			DescriptorDst[nI][4] = LOWORD((DWORD)&DescriptorDst[nI+1][0]);
			
			pDst += dwDstSize;
			pSrc += dwSrcSize;			
		}
		
		// Source Descriptor
		DescriptorSrc[nI][0] = DCFG_DMA_EN | DCFG_SIZE32 | DCFG_DBO;
		DescriptorSrc[nI][1] = dwSrcSize/sizeof(DWORD);
		DescriptorSrc[nI][2] = LOWORD((DWORD)pSrc);
		DescriptorSrc[nI][3] = HIWORD((DWORD)pSrc);
		DescriptorSrc[nI][4] = LOWORD((DWORD)&DescriptorSrc[nI][5]);
		DescriptorSrc[nI][5] = 0x0001;
				
		// Destination Descriptor
		DescriptorDst[nI][0] = DCFG_DMA_EN | DCFG_SIZE32 | DCFG_DBO;
		DescriptorDst[nI][1] = dwSrcSize/sizeof(DWORD);
		DescriptorDst[nI][2] = LOWORD((DWORD)pDst);
		DescriptorDst[nI][3] = HIWORD((DWORD)pDst);
		DescriptorDst[nI][4] = LOWORD((DWORD)&DescriptorDst[nI][5]);
		DescriptorDst[nI][5] = 0x0001;
	
		// DMA_Basepointer setzen
		*pDMA_DBP = HIWORD((DWORD)&DescriptorSrc[0][0]);
		
		// Destination Memory DMA Next Descriptor Pointer
		*pMDD_DND = LOWORD((DWORD)DescriptorDst);	
	
		// Source Memory DMA Next Descriptor Pointer
		*pMDS_DND = LOWORD((DWORD)DescriptorSrc);

		// Destinationbuffer löschen
		*pMDD_DCFG = *pMDD_DCFG | DCFG_CLEAR_BUFF;
		
		// Sourcebuffer löschen
		*pMDS_DCFG = *pMDD_DCFG | DCFG_CLEAR_BUFF;

		bResult = TRUE;
	}
	else
	{
		DoNotifyInformation(INFO_TOO_MANY_CHANNELS, nChannels);	
		printf("CreateMultiChannelDescriptorList: too many channels (%d)\n", nChannels);
	}
	
	
	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
void EnableDma()
{
	// Enable Destination Memory DMA
	*pMDD_DCFG = *pMDD_DCFG	| DCFG_DMA_EN;	

	// Enable Source Memory DMA
	*pMDS_DCFG = *pMDS_DCFG	| DCFG_DMA_EN;
}


///////////////////////////////////////////////////////////////////////////////////
void DisableDma()
{
	// Disable Source Memory DMA
	*pMDS_DCFG = *pMDS_DCFG	& ~DCFG_DMA_EN;

	// Disable Destination Memory DMA
	*pMDD_DCFG = *pMDD_DCFG	& ~DCFG_DMA_EN;		
}

