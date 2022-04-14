#include <gpiotest.h>
#include <CdefBF535.h>
#include <cdefblackfin.h>
#include <sys/exception.h>
#include <stdio.h>
#include <string.h> 
#include "..\Include\Helper.h"
#include "..\..\TashaUnit\TashaUnitDefs.h"
#include "Sports.h"
#include "MemoryDMA.h"
#include "TashaFirmware535.h"
#include "..\Include\TMMessages.h"
#include "Demultiplex.h"

extern "C" BOOL Demux4Channels16Bit(WORD* pDestBase, WORD* pSrcBase, int nLength);
extern "C" BOOL Demux4Channels32Bit(WORD* pDestBase, WORD* pSrcBase, int nLength);
extern BOOL  g_bCaptureOn;
extern BOOL  g_bBF533Booting;
extern DWORD g_dwPCIFrameBuffer;

//////////////////////////////////////////////////////////////////////////////////
void InitSPORTsISR()
{ 
	*pIMASK 	= *pIMASK & ~EVT_IVG8;							// Disable IVG8
	CloseSPORTs();    
	
	ex_handler_fn p =register_handler(ik_ivg8, SPORTS_RX_ISR); 	// Register ISR

    *pSIC_IAR0 	= *pSIC_IAR0 | P4_IVG8 | P6_IVG8;				// Assign SPORT0_RX and SPORT1_RX to IVG8         
	*pSIC_IMASK = *pSIC_IMASK | SIC_MASK4 | SIC_MASK6;			// enable SPORT0_RX and SPORT1_RX 

    *pIMASK 	= *pIMASK | EVT_IVG8;							// enable IVG8
}

///////////////////////////////////////////////////////////////////////////////////
void EnableSPORTs()
{
	DisableSPORTs();
	
	*pSPORT0_RX_CONFIG =  ICLK | DTYPE_RJZ | SLEN16 | IRFS;
	*pSPORT1_RX_CONFIG =  ICLK | DTYPE_RJZ | SLEN16 | IRFS;

	*pSPORT0_RFSDIV = SPORT0_CHANNELS*SPORT0_TIME_SLICE_SIZE*8-1; // nach 4*32 SClock Framewechsel 
	*pSPORT1_RFSDIV = SPORT1_CHANNELS*SPORT1_TIME_SLICE_SIZE*8-1; // nach 4*32 SClock Framewechsel 

	*pSPORT0_MRCS0 = 0x00ff; 
	*pSPORT0_MRCS1 = 0x0000;
	*pSPORT0_MRCS2 = 0x0000;
	*pSPORT0_MRCS3 = 0x0000;
	*pSPORT1_MRCS0 = 0x00ff;
	*pSPORT1_MRCS1 = 0x0000;
	*pSPORT1_MRCS2 = 0x0000;
	*pSPORT1_MRCS3 = 0x0000;
	
	// WOFF=0, WSIZE=0(8Channels), MFD=0, MCM=1
	*pSPORT0_MCMC1 = 0x0003;
	*pSPORT1_MCMC1 = 0x0003;	// Bei 3 boards muß ein Delay von 1 clk hinzu!
	
	// MCCRM=Bypass mode, MCDTXPE=0, MCDRXPE=0, MCOM=0, MCFF=0, FSDR=0
	*pSPORT0_MCMC2 = 0x0000;
	*pSPORT1_MCMC2 = 0x0000;

	// Die Clockfrequenz des SPORTx setzen
	*pSPORT0_RSCLKDIV = SYSTEM_CLK/(2*SPORT0_FREQUENCY)-1;	// SPORT0_RX Frequency
	*pSPORT1_RSCLKDIV = SYSTEM_CLK/(2*SPORT1_FREQUENCY)-1;	// SPORT1_RX Frequency

	*pSPORT0_RX_CONFIG = *pSPORT0_RX_CONFIG | RSPEN;	// Enable SPORT0_RX
	
	// Beide SPORT sollten NICHT synchron gestartet werden, da sie ansonsten
	// zeitgleich Interrupts generieren.
	Delay(10000);	
	*pSPORT1_RX_CONFIG = *pSPORT1_RX_CONFIG | RSPEN;	// Enable SPORT1_RX
}

///////////////////////////////////////////////////////////////////////////////////
void DisableSPORTs()
{
	*pSPORT0_RX_CONFIG = *pSPORT0_RX_CONFIG & ~RSPEN;	// Disable SPORT0_RX   
	*pSPORT1_RX_CONFIG = *pSPORT1_RX_CONFIG & ~RSPEN;   // Disable SPORT1_RX

	// Workaround Bug 36 (SPORT Multichannel Mode starting and stopping)
	*pSPORT0_MCMC1 = 0x0000;							// Disable MultiChannelMode
	*pSPORT1_MCMC1 = 0x0000;							// Disable MultiChannelMode
}

///////////////////////////////////////////////////////////////////////////////////
void EnableSPORTsDMA()
{
	// Setze SPORT0 DMA  auf '16Bit Autobuffer'- Mode....
	//================================================

	// Erstmal DAUTO auf '1', damit die anderen Bits beschrieben werden können
	*pSPORT0_CONFIG_DMA_RX 		= DAUTO;	
	*pSPORT0_CONFIG_DMA_RX 		= TRAN | DCOME | DAUTO | SIZE16 | DERE;

	// Setze Startadresse auf den DMA Buffer zum Empfang der SPORT0-Daten.
	*pSPORT0_START_ADDR_HI_RX 	= HIWORD((DWORD)SPORT0_DMA_BUFFER_ADDR);
	*pSPORT0_START_ADDR_LO_RX 	= LOWORD((DWORD)SPORT0_DMA_BUFFER_ADDR);
	
	// Setze die erwartete Anzahl der zu Empfangenen Daten.
	*pSPORT0_COUNT_RX			= 2*SPORT0_DMA_BUFFER_SIZE/sizeof(short);

	// Enable SPORT0 DMA
	*pSPORT0_CONFIG_DMA_RX 		= *pSPORT0_CONFIG_DMA_RX | DEN;

	
	// Setze SPORT1 DMA  auf '16Bit Autobuffer'- Mode....
	//================================================

	// Erstmal DAUTO auf '1', damit die anderen Bits beschrieben werden können
	*pSPORT1_CONFIG_DMA_RX 		= DAUTO;	
	*pSPORT1_CONFIG_DMA_RX 		= TRAN | DCOME | DAUTO | SIZE16 | DERE;

	// Setze Startadresse auf den DMA Buffer zum Empfang der SPORT1-Daten.
	*pSPORT1_START_ADDR_HI_RX 	= HIWORD((DWORD)SPORT1_DMA_BUFFER_ADDR);
	*pSPORT1_START_ADDR_LO_RX 	= LOWORD((DWORD)SPORT1_DMA_BUFFER_ADDR);
	
	// Setze die erwartete Anzahl der zu Empfangenen Daten.
	*pSPORT1_COUNT_RX			= 2*SPORT1_DMA_BUFFER_SIZE/sizeof(short);

	// Enable SPORT1 DMA
	*pSPORT1_CONFIG_DMA_RX 		= *pSPORT1_CONFIG_DMA_RX | DEN;
}

///////////////////////////////////////////////////////////////////////////////////
void DisableSPORTsDMA()
{
	// Workaround Bug 36 (SPORT Multichannel Mode starting and stopping)
	*pSPORT0_CONFIG_DMA_RX  = *pSPORT0_CONFIG_DMA_RX | FLSH;	// Set FLSH-Bit
	*pSPORT1_CONFIG_DMA_RX  = *pSPORT1_CONFIG_DMA_RX | FLSH;	// Set FLSH-Bit
	Delay(10000); // Wait 4 SPORT clock cycles
	
	*pSPORT0_CONFIG_DMA_RX  = *pSPORT0_CONFIG_DMA_RX &~ FLSH;	// Clear FLSH-Bit
	*pSPORT1_CONFIG_DMA_RX  = *pSPORT1_CONFIG_DMA_RX &~ FLSH;	// Clear FLSH-Bit

	*pSPORT0_CONFIG_DMA_RX = *pSPORT0_CONFIG_DMA_RX & ~DEN;		// Disable SPORT0_RX DMA
	*pSPORT1_CONFIG_DMA_RX = *pSPORT1_CONFIG_DMA_RX & ~DEN;		// Disable SPORT1_RX DMA
}

///////////////////////////////////////////////////////////////////////////////////
void CloseSPORTs()
{
	DisableSPORTs();
	DisableSPORTsDMA();
}

///////////////////////////////////////////////////////////////////////////////////
EX_INTERRUPT_HANDLER(SPORTS_RX_ISR)
{
	// Ist der Datentransfer abgeschlossen?
	if (TSTBIT(*pSPORT0_IRQSTAT_RX,0))
	{
		// Clear Interrupts
		*pSPORT0_IRQSTAT_RX = *pSPORT0_IRQSTAT_RX;
		// Wenn Capture aus ist, oder einer der BF533 bootet, dann sofort raus hier.
		if (g_bCaptureOn && !g_bBF533Booting)
		{
			WORD wCount = *pSPORT0_COUNT_RX;
			if (*pSPORT0_COUNT_RX < SPORT0_DMA_BUFFER_SIZE/sizeof(short))
				HandleSport0RxIRQ(0);	
			else
				HandleSport0RxIRQ(SPORT0_DMA_BUFFER_SIZE);	
		}
	
	}
	
	// Ist der Datentransfer abgeschlossen?
	if (TSTBIT(*pSPORT1_IRQSTAT_RX,0))
	{
		// Clear Interrupts
		*pSPORT1_IRQSTAT_RX = *pSPORT1_IRQSTAT_RX;
		// Wenn Capture aus ist, oder einer der BF533 bootet, dann sofort raus hier.
		if (g_bCaptureOn && !g_bBF533Booting)
		{
			WORD wCount = *pSPORT0_COUNT_RX;
			if (*pSPORT1_COUNT_RX < SPORT1_DMA_BUFFER_SIZE/sizeof(short))
				HandleSport1RxIRQ(0);	
			else
				HandleSport1RxIRQ(SPORT1_DMA_BUFFER_SIZE);	
		}
	}
} 

///////////////////////////////////////////////////////////////////////////////////
// Am SPORT0 hängen die BF533 (1...4)
BOOL HandleSport0RxIRQ(DWORD dwL1Offset)
{
	static DWORD dwIndex = 0;
	static DWORD dwSDRAMOffset = 0;
	BOOL bResult = FALSE;
	
	if (Demux4Channels32Bit((WORD*) (SPORT0_L1_DST_BUFFER_ADDR+dwL1Offset),
					        (WORD*) (SPORT0_DMA_BUFFER_ADDR+dwL1Offset),
						     SPORT0_DMA_BUFFER_SIZE))
	{	
		if (MultiChannelCopy((BYTE*)(g_dwPCIFrameBuffer+dwSDRAMOffset+dwIndex),
							         SDRAM_DMA_BUFFER0_SIZE/SPORT0_CHANNELS,
						 	 (BYTE*)(SPORT0_L1_DST_BUFFER_ADDR+dwL1Offset),
						 			 SPORT0_DMA_BUFFER_SIZE/SPORT0_CHANNELS,
						 			 SPORT0_CHANNELS, FALSE))						 
		{
			dwIndex+=SPORT0_DMA_BUFFER_SIZE/SPORT0_CHANNELS;	
	
			// Alle Daten für ein Frame von jeder Kamera zusammen?
			if (dwIndex >= SDRAM_DMA_BUFFER0_SIZE/SPORT0_CHANNELS)
			{	
				// Warte bis auch die letzeten Daten über den PCI-Bus übertragen wurden
				WaitUntilMemCopyDmaCompleted();

				// Informiere die  TashaUnit, darüber, daß sie die Daten der Kanäle 0...3 abholen kann
				DoIndicationNewData(dwSDRAMOffset, SPORT0);

				// Wechsel zwischen den beiden SDRAM-Buffern. Während die Bilddaten von der TashaUnit
				// abgeholt werden, schreibt der DMA-Prozess die Daten in einen zweiten Buffer.
				dwSDRAMOffset += SDRAM_DMA_BUFFER0_SIZE;
				if (dwSDRAMOffset >= 16*SDRAM_DMA_BUFFER0_SIZE)
					dwSDRAMOffset = 0;
				dwIndex = 0;
			}		
			
			bResult = TRUE;
		}	
	}
	
	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
// Am SPORT1 hängen die BF533 (5...8)
BOOL HandleSport1RxIRQ(DWORD dwL1Offset)
{
	static DWORD dwIndex = 0;
	static DWORD dwSDRAMOffset = 4*SDRAM_DMA_BUFFER0_SIZE;
	BOOL bResult = FALSE;

	if (Demux4Channels32Bit((WORD*) (SPORT1_L1_DST_BUFFER_ADDR+dwL1Offset),
					        (WORD*) (SPORT1_DMA_BUFFER_ADDR+dwL1Offset),
						     SPORT1_DMA_BUFFER_SIZE))
	{
		if (MultiChannelCopy((BYTE*)(g_dwPCIFrameBuffer+dwSDRAMOffset+dwIndex),
									SDRAM_DMA_BUFFER1_SIZE/SPORT1_CHANNELS,
							 (BYTE*)(SPORT1_L1_DST_BUFFER_ADDR+dwL1Offset),
							 		SPORT1_DMA_BUFFER_SIZE/SPORT1_CHANNELS,
							 		SPORT1_CHANNELS, FALSE))
		{
			dwIndex+=SPORT1_DMA_BUFFER_SIZE/SPORT1_CHANNELS;	
	
			// Alle Daten für ein Frame von jeder Kamera zusammen?
			if (dwIndex >= SDRAM_DMA_BUFFER1_SIZE/SPORT1_CHANNELS)
			{	
				// Warte bis auch die letzeten Daten über den PCI-Bus übertragen wurden
				WaitUntilMemCopyDmaCompleted();		
				
				// Informiere die TashaUnit, darüber, daß sie die Daten der Kanäle 0...3 abholen kann
				DoIndicationNewData(dwSDRAMOffset, SPORT1);
				
				// Wechsel zwischen den beiden SDRAM-Buffern. Während die Bilddaten von der TashaUnit
				// abgeholt werden, schreibt der DMA-Prozess die Daten in einen zweiten Buffer.
				dwSDRAMOffset += SDRAM_DMA_BUFFER1_SIZE;
				if (dwSDRAMOffset >= 16*(SDRAM_DMA_BUFFER0_SIZE+SDRAM_DMA_BUFFER1_SIZE))
					dwSDRAMOffset = 16*SDRAM_DMA_BUFFER0_SIZE;
				dwIndex = 0;
			}
			bResult = TRUE;
		}	
	}

	return bResult;
}


