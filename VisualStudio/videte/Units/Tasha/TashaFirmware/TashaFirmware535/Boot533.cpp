/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaFirmware535
// FILE:		$Workfile: Boot533.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/TashaFirmware535/Boot533.cpp $
// CHECKIN:		$Date: 10.02.05 12:03 $
// VERSION:		$Revision: 48 $
// LAST CHANGE:	$Modtime: 10.02.05 11:28 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include <CdefBF535.h>
#include <sys/exception.h>

#include "..\Include\SpiDefs.h" 
#include "..\Include\Helper.h"
#include "..\Include\TMMessages.h"
#include "..\..\TashaUnit\TashaUnitDefs.h"
#include "Boot533.h"
#include "TashaFirmware535.h"
#include "Altera.h"
#include "LED.h"

#define PF0 	0x0001
#define PF1 	0x0002
#define PF2 	0x0004
#define PF3 	0x0008
#define PF4 	0x0010
#define PF5 	0x0020
#define PF6 	0x0040
#define PF7 	0x0080
#define PF8 	0x0100
#define PF9 	0x0200
#define PF10 	0x0400
#define PF11 	0x0800
#define PF12 	0x1000
#define PF13 	0x2000
#define PF14 	0x4000
#define PF15 	0x8000

extern section ("DescriptorSPI") volatile WORD DescriptorSPI[MAX_SPI_DESCRIPTOR_LIST_SIZE][6];
extern "C" BOOL Emulate24BitEEProm(BYTE* pSrcBase, DWORD dwLength);
extern "C" BOOL FirmwareTransfer(BYTE* pSrcBase, DWORD dwLength);


/////////////////////////////////////////////////////////////////////////
// Diese Funktion Emuliert ein EEProm. Sie dient zur Übertragung des Bootloaders
BOOL SPIBootTransferBootLoader(int nBFNr, BYTE* pBuffer, DWORD dwLen)
{
	BOOL 	bValidAddress 	= FALSE;
	DWORD 	dwErrorCnt		= 0;
	DWORD 	dwAddr			= 0;
	WORD  	wVal 			= 0;
	BOOL	bResult			= FALSE;

	SPIBootInitSPI(FALSE);

	// BF533 'reseten'
	// BF533-Clock ein, BF533 Bootmethode an, VideoOPs ein, JTAG ein
	if (readAlteraReg(ALT_CTRL, wVal))
	{
		wVal = SETBIT(wVal, ALT_CTRL_24MHZ_EN_BIT);
		wVal = SETBIT(wVal, ALT_CTRL_27MHZ_A_EN_BIT);
		wVal = SETBIT(wVal, ALT_CTRL_27MHZ_B_EN_BIT);
		wVal = SETBIT(wVal, ALT_CTRL_EN_BF_BOOT_BIT);
		wVal = SETBIT(wVal, ALT_CTRL_VIDEO_ENABLE_BIT);
		wVal = CLRBIT(wVal, ALT_CTRL_JTAG_EN_BIT);
		writeAlteraReg(ALT_CTRL, wVal);
	}
	
	// 100ys warten
	WaitMicro(100);									

	// BF533 in den Reset bringen
	if (readAlteraReg(ALT_RESET_BF533, wVal))
	{
		wVal = CLRBIT(wVal, nBFNr);
		writeAlteraReg(ALT_RESET_BF533, wVal);
	}
	
	// BF533 mindestens 100ys im Reset belassen.
	WaitMicro(100);									

	// BF533 aus dem Reset holen
	if (readAlteraReg(ALT_RESET_BF533, wVal))
	{
		wVal = SETBIT(wVal, nBFNr);
		writeAlteraReg(ALT_RESET_BF533, wVal);
	}
	
	// BF533 booten. (BF535 ist Slave, BF533 ist Master)
	bResult = Emulate24BitEEProm(pBuffer, dwLen);

	// Im Fehlerfall muß der DSP wieder in den Reset gebracht werden, damit er nicht den SPI stört.
	if (!bResult)
	{
		// BF533 in den Reset bringen
		if (readAlteraReg(ALT_RESET_BF533, wVal))
		{
			wVal = CLRBIT(wVal, nBFNr);
			writeAlteraReg(ALT_RESET_BF533, wVal);
		}	
	}
	
	return bResult;
}

/////////////////////////////////////////////////////////////////////////
BOOL SPIBootTransferFirmware(int nBFNr, BYTE* pBuffer, DWORD dwLen)
{
	BOOL 	bValidAddress 	= FALSE;
	DWORD 	dwErrorCnt		= 0;
	DWORD 	dwAddr			= 0;
	WORD  	wVal 			= 0;
	BOOL	bResult			= FALSE;

	SPIBootInitSPI(TRUE);

	// BF533 booten
#ifdef USE_DMA
	bResult = SPIBootMasterDMATransfer(nBFNr, pBuffer, dwLen);
#else
	bResult = SPIBootMasterTransfer(nBFNr, pBuffer, dwLen);
#endif

	// Im Fehlerfall muß der DSP wieder in den Reset gebracht werden, damit er nicht den SPI stört.
	if (!bResult)
	{
		// BF533 in den Reset bringen
		if (readAlteraReg(ALT_RESET_BF533, wVal))
		{
			wVal = CLRBIT(wVal, nBFNr);
			writeAlteraReg(ALT_RESET_BF533, wVal);
		}	
	}
	
	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL SPIBootInitSPI(BOOL bMaster)
{
	BOOL bResult = FALSE;
	if (bMaster)
	{
#ifdef USE_DMA
		*pSPI1_CONFIG &= ~SPI_CFG_DEN;							// DMA Ausschalten
		*pSPI1_CONFIG &= ~SPI_CFG_DCOME;						// No Interrupts
		*pSPI1_CTL 	  &= ~SPE;									// SPI Ausschalten
			
		// Baudraten der beiden SPIs einstellen	
		*pSPI1_BAUD = SYSTEM_CLK/(2*SPI1_BOOTBF533_BAUDRATE);	
	
		// DisableSPI, Master, 8Bit, Disable MISO, SlaveSelect disable, SendZero, DMA Transmit
		*pSPI1_CTL = MSTR | SZ | 0x02;
#else
		// Baudraten der beiden SPIs einstellen	
		*pSPI1_BAUD = SYSTEM_CLK/(2*SPI1_BOOTBF533_BAUDRATE);	
		
		// EnableSPI, Master, 8Bit, Disable MISO, SlaveSelect disable, SendZero, Transmit
		*pSPI1_CTL = SZ | SPE | 0x01 | MSTR;
#endif
	}
	else
	{
		*pFIO_DIR = *pFIO_DIR & ~PF1;	// SPISS auf Eingang
			
		// MSB-First, Enable MISO, SendZero, SlaveSelect disable, Slave, 8Bit, EnableSPI
		*pSPI1_CTL = EMISO | SZ | SPE | 0x01;			
	}

	return TRUE;	
}

///////////////////////////////////////////////////////////////////////////////////
BOOL SPIBootMasterTransfer(int nSlave, BYTE* pBuffer, DWORD dwLen)
{
	BOOL bErr   	= FALSE;
	DWORD dwTimeOut	= 10;
	
	BYTE  byDummy	= 0;
	int	 nAddr		= 0;
		
	SPIBootSelectSlave(nSlave);
	if (SPIBootWaitUntilSlaveIsReady(nSlave, 1000))
		bErr = !FirmwareTransfer(pBuffer, dwLen);
	else
		bErr = TRUE;
		
	SPIBootSelectSlave(-1);
		
	return !bErr;	
}

///////////////////////////////////////////////////////////////////////////////////
BOOL SPIBootWaitUntilSlaveIsReady(int nSlave, int nTimeOut)
{
	BOOL bState  = FALSE;
	WORD wVal	 = 0;
	int nCounter = 0;
	nTimeOut *= 1000;		// Es wird jeweils 1ys gewartet.

	do
	{
		if (readAlteraReg(ALT_BF533_GPIO_0_X, wVal))
			bState = TSTBIT(wVal, nSlave+8);
		if (!bState)
			WaitMicro(1);
	}
	while ((nCounter++ < nTimeOut) && !bState);

	return bState;		

}

///////////////////////////////////////////////////////////////////////////////////
void SPIBootSelectSlave(int nSlave)
{
	if (nSlave == -1)
		writeAlteraReg(ALT_SLAVE_SELECT, 0);
	else
		writeAlteraReg(ALT_SLAVE_SELECT, SETBIT(0, nSlave));
}


///////////////////////////////////////////////////////////////////////////////////
BOOL SPIBootMasterDMATransfer(int nSlave, BYTE* pBuffer, DWORD dwLen)
{
	BOOL bResult   	= FALSE;
		
	*pSPI1_CONFIG &= ~SPI_CFG_DEN;							// SPI Ausschalten
	*pSPI1_CONFIG &= ~SPI_CFG_DAUTO;						// Disable Autobuffer mode

	// Descriptorliste aufbauen
	if (SPIBootCreateDescriptorList(pBuffer, dwLen))
	{		
		SPIBootSelectSlave(nSlave);
		if (SPIBootWaitUntilSlaveIsReady(nSlave, 100))
		{
			*pSPI1_CTL |= SPE;									// SPI Enable	
			*pSPI1_CONFIG |= SPI_CFG_DEN;						// DMA Enable
	
			// Warte bis zu 1000ms bis Transfer abgeschlossen ist
			bResult = SPIBootWaitUntilFirmwareTransferCompleted(1000);
			
			*pSPI1_CONFIG &= ~SPI_CFG_DEN;						// DMA Ausschalten
			*pSPI1_CTL &= ~SPE;									// SPI Ausschalten
		}
		SPIBootSelectSlave(-1);
	}
		
	return bResult;	
}

///////////////////////////////////////////////////////////////////////////////////
BOOL SPIBootWaitUntilFirmwareTransferCompleted(int nTimeOut)
{
	int nCounter = 0;
	nTimeOut *= 1000;		// Es wird jeweils 1ys gewartet.
	while(*pSPI1_CONFIG & SPI_CFG_DOWN)
		WaitMicro(1);

	return !(*pSPI1_CONFIG & SPI_CFG_DOWN); 
}

///////////////////////////////////////////////////////////////////////////////////
BOOL SPIBootCreateDescriptorList(BYTE* pBuffer, DWORD dwLen)
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
		DoNotifyInformation(INFO_TOO_MANY_DESCRIPTORS, dwTransfers);
	}
	
	
	return bResult;
}

