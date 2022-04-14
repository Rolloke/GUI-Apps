#include <gpiotest.h>
#include <cdefblackfin.h>
#include <CdefBF535.h>
#include <sys/exception.h>
#include <stdio.h>
#include <string.h> 

#include "..\Include\Helper.h"
#include "..\Include\TMMessages.h"
#include "..\Include\SpiDefs.h"
#include "Altera.h"
#include "Sports.h"
#include "TashaFirmware535.h"

//==========================================================================
// Schreib- und Lesezugriffe auf den Altera
//==========================================================================
section ("DescriptorSPI") volatile WORD DescriptorSPI[MAX_SPI_DESCRIPTOR_LIST_SIZE][6];

/////////////////////////////////////////////////////////////////////////
void AlteraInit()
{
	*(pEBIU_AMGCTL) = 0x00F6;
	return;
}

/////////////////////////////////////////////////////////////////////////
void writeAlteraReg(BYTE byReg, BYTE byDataW)
{
	*(pALTERA_REG+byReg) = byDataW;
	return;
}

/////////////////////////////////////////////////////////////////////////
void readAlteraReg(BYTE byReg, BYTE *pbyDataR)
{
	*pbyDataR = *(pALTERA_REG+byReg);
	return;
}


//==========================================================================
// Programmierung des Alteras
//==========================================================================

/////////////////////////////////////////////////////////////////////////
BOOL progAlteraInit()
{
	*pSPI1_CONFIG &= ~DEN;									// DMA Ausschalten
	*pSPI1_CONFIG &= ~DCOME;								// No Interrupts
	*pSPI1_CTL &= ~SPE;										// SPI Ausschalten
	
	*pFIO_DIR 	 = *pFIO_DIR | NCONFIG;						// NCONFIG Ausgang
	*pFIO_DIR 	 = *pFIO_DIR &~ CONF_DONE;					// CONFIG_DONE Eingang
	*pFIO_FLAG_C = NCONFIG;									// NCONFIG auf '0'
	*pFIO_POLAR	 = *pFIO_POLAR &~ CONF_DONE; 				// Active high
	*pFIO_EDGE	 = *pFIO_EDGE  &~ CONF_DONE;				// Level sensetive
			
	// Baudraten der beiden SPIs einstellen	
	*pSPI1_BAUD = SYSTEM_CLK/(2*SPI1_ALTERA_BAUDRATE);		// 1 MHz		
	
	// DisableSPI, Master, 8Bit, Disable MISO, SlaveSelect disable, SendZero, DMA Transmit
	*pSPI1_CTL = MSTR | LSBF | SZ | 0x02;
	
	WaitMicro(40);	// 40ys
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////
BOOL progAlteraStart()
{
	*pFIO_FLAG_S = NCONFIG;									// NCONFIG   auf '1'

	return TRUE;									
}

/////////////////////////////////////////////////////////////////////////
BOOL progAlteraTX(BYTE* pBuffer, DWORD dwLen)
{
	BOOL bResult = FALSE;

	*pSPI1_CONFIG &= ~DEN;									// SPI Ausschalten
	*pSPI1_CONFIG &= ~DAUTO;								// Disable Autobuffer mode
	
	// Descriptorliste aufbauen
	if (CreateDescriptorList(pBuffer, dwLen))
	{	
		WaitMicro(20); // 20ys

		*pSPI1_CTL |= SPE;									// SPI Enable	
		*pSPI1_CONFIG |= DEN;								// DMA Enable

		// Warte 1000ms bis Transfer zum Altera abgeschlossen ist
		bResult = WaitUntilAlteraTransferCompeted(1000);

		WaitMicro(15);	// 15ys									// warten
	
		*pSPI1_CONFIG &= ~DEN;								// DMA Ausschalten
		*pSPI1_CTL &= ~SPE;									// SPI Ausschalten
	}
	
	return bResult;	
}

///////////////////////////////////////////////////////////////////////////////////
BOOL WaitUntilAlteraTransferCompeted(int nTimeOut)
{
	int nCounter = 0;
	nTimeOut *= 100;		// Es wird jeweils 10ys gewartet.
	while(((*pFIO_FLAG_S & CONF_DONE) != CONF_DONE) && (nCounter++ < nTimeOut))
		WaitMicro(10);

	return (*pFIO_FLAG_S & CONF_DONE); 
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CreateDescriptorList(BYTE* pBuffer, DWORD dwLen)
{
	BOOL	bResult = FALSE;
	
	DWORD dwTransfers = dwLen / DMA_SPI1_TRANSFER_SIZE;
	DWORD dwRestLen   = dwLen % DMA_SPI1_TRANSFER_SIZE;
		
	if (dwTransfers < MAX_SPI_DESCRIPTOR_LIST_SIZE)
	{	
		if (dwTransfers > 0)
		{
			for (int nI = 0; nI < dwTransfers; nI++)
			{
				// Source Descriptor
				DescriptorSPI[nI][0] = SPI_CFG_DEN | SPI_CFG_SIZE8 | SPI_CFG_DOWN;
				DescriptorSPI[nI][1] = DMA_SPI1_TRANSFER_SIZE/sizeof(BYTE);
				DescriptorSPI[nI][2] = LOWORD((DWORD)pBuffer);
				DescriptorSPI[nI][3] = HIWORD((DWORD)pBuffer);
				DescriptorSPI[nI][4] = LOWORD((DWORD)&DescriptorSPI[nI+1][0]);
									
				pBuffer += DMA_SPI1_TRANSFER_SIZE;			
			}
		}
		
		if (dwRestLen != 0)
		{
			// Source Descriptor
			DescriptorSPI[dwTransfers][0] = SPI_CFG_DEN | SPI_CFG_SIZE8 | SPI_CFG_DOWN;
			DescriptorSPI[dwTransfers][1] = dwRestLen/sizeof(BYTE);
			DescriptorSPI[dwTransfers][2] = LOWORD((DWORD)pBuffer);
			DescriptorSPI[dwTransfers][3] = HIWORD((DWORD)pBuffer);
			DescriptorSPI[dwTransfers][4] = LOWORD((DWORD)&DescriptorSPI[dwTransfers][5]);
			DescriptorSPI[dwTransfers][5] = 0x0001;
		}
		else
		{
			DescriptorSPI[dwTransfers-1][4] = LOWORD((DWORD)&DescriptorSPI[dwTransfers-1][5]);
			DescriptorSPI[dwTransfers-1][5] = 0x0001;
		}
		
		// DMA_Basepointer setzen
		*pDMA_DBP = HIWORD((DWORD)&DescriptorSPI[0][0]);
		
		// SPI Next Descriptor Pointer
		*pSPI1_NEXT_DESCR = LOWORD((DWORD)DescriptorSPI);
		
		bResult = TRUE;
	}
	else
	{
		printf("CreateSingleChannelDescriptorList: Too many descriptors (%d>=%d)\n", dwTransfers, MAX_SPI_DESCRIPTOR_LIST_SIZE);
		DoNotifyInformation(INFO_TOO_MANY_DESCRIPTORS, dwTransfers);
	}
	
	
	return bResult;
}

