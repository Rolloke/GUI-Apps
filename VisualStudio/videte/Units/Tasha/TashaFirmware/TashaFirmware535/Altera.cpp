/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaFirmware535
// FILE:		$Workfile: Altera.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/TashaFirmware535/Altera.cpp $
// CHECKIN:		$Date: 11.05.04 10:48 $
// VERSION:		$Revision: 27 $
// LAST CHANGE:	$Modtime: 11.05.04 8:21 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

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
#include "TashaFirmware535.h"

BOOL bAlteraReady = FALSE;

//==========================================================================
// Schreib- und Lesezugriffe auf den Altera
//==========================================================================
section ("DescriptorSPI") volatile WORD DescriptorSPI[MAX_SPI_DESCRIPTOR_LIST_SIZE][6];

/////////////////////////////////////////////////////////////////////////
BOOL IsAlteraValid()
{
	return bAlteraReady;
}

/////////////////////////////////////////////////////////////////////////
void AlteraInit()
{
	*pEBIU_AMBCTL0= 0xfffEfffE;
	*pEBIU_AMGCTL = 0x00F2;
	
	// Clock für den Altera einstellen
	InitAlteraClockTimer(25);	
	StartAlteraClockTimer();
	
	return;
}

/////////////////////////////////////////////////////////////////////////
BOOL writeAlteraReg(BYTE byReg, WORD wVal)
{
	if (!bAlteraReady)
		return FALSE;
		
	*(pALTERA_REG+byReg) = wVal;
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////
BOOL readAlteraReg(BYTE byReg, WORD& wVal)
{
	if (!bAlteraReady)
		return FALSE;

	wVal = *(pALTERA_REG+byReg);
	
	return TRUE;
}


//==========================================================================
// Programmierung des Alteras
//==========================================================================

/////////////////////////////////////////////////////////////////////////
BOOL progAlteraInit()
{
	*pSPI1_CONFIG &= ~SPI_CFG_DEN;							// DMA Ausschalten
	*pSPI1_CONFIG &= ~SPI_CFG_DCOME;						// No Interrupts
	*pSPI1_CTL &= ~SPE;										// SPI Ausschalten
	
	*pFIO_DIR 	 = *pFIO_DIR | NCONFIG;						// NCONFIG auf Ausgang
	*pFIO_DIR 	 = *pFIO_DIR &~ CONF_DONE;					// CONFIG_DONE auf Eingang
	*pFIO_DIR 	 = *pFIO_DIR &~ NSTATUS;					// NSTATUS auf Eingang
	
	*pFIO_FLAG_C = NCONFIG;									// NCONFIG auf '0'
	*pFIO_POLAR	 = *pFIO_POLAR &~ CONF_DONE; 				// Active high
	*pFIO_EDGE	 = *pFIO_EDGE  &~ CONF_DONE;				// Level sensitive
			
	// Baudraten der beiden SPIs einstellen	
	*pSPI1_BAUD = SYSTEM_CLK/(2*SPI1_ALTERA_BAUDRATE);		// 1 MHz		
	
	// DisableSPI, Master, 8Bit, Disable MISO, SlaveSelect disable, SendZero, DMA Transmit
	*pSPI1_CTL = MSTR | LSBF | SZ | 0x02;
	
	WaitMicro(50);	// 40ys
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
	bAlteraReady = FALSE;
	
	*pSPI1_CONFIG &= ~SPI_CFG_DEN;							// SPI Ausschalten
	*pSPI1_CONFIG &= ~SPI_CFG_DAUTO;						// Disable Autobuffer mode
	
	// Descriptorliste aufbauen
	if (CreateDescriptorList(pBuffer, dwLen))
	{	
		WaitMicro(20); // 20ys

		*pSPI1_CTL |= SPE;									// SPI Enable	
		*pSPI1_CONFIG |= SPI_CFG_DEN;						// DMA Enable

		// Warte 1000ms bis Transfer zum Altera abgeschlossen ist
		bResult = WaitUntilAlteraTransferCompleted(1000);

		WaitMicro(15);	// 15ys								// warten
	
		*pSPI1_CONFIG &= ~SPI_CFG_DEN;						// DMA Ausschalten
		*pSPI1_CTL &= ~SPE;									// SPI Ausschalten
	}
	
	bAlteraReady = bResult;
	return bResult;	
}

///////////////////////////////////////////////////////////////////////////////////
BOOL WaitUntilAlteraTransferCompleted(int nTimeOut)
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

///////////////////////////////////////////////////////////////////////////////////
BOOL InitAlteraClockTimer(int nFreq)
{
	int nPeriod 		= SYSTEM_CLK/nFreq;

	*pTIMER2_CONFIG = 0;
    *pTIMER2_CONFIG = *pTIMER2_CONFIG | PERIOD_CNT | PWM_OUT;

    *pTIMER2_PERIOD_HI = HIWORD(nPeriod);
    *pTIMER2_PERIOD_LO = LOWORD(nPeriod);
    
    *pTIMER2_WIDTH_HI = HIWORD(nPeriod/2);
    *pTIMER2_WIDTH_LO = LOWORD(nPeriod/2);
  	
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////
void StartAlteraClockTimer()
{
	*pTIMER2_STATUS = IRQ2;		
	*pTIMER2_STATUS = TIMEN2;
}

///////////////////////////////////////////////////////////////////////////////////
void StopAlteraClockTimer()
{
	*pTIMER2_STATUS = TIMDIS2;
}

