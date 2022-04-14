/********************************************************************/
/* ISR Interrupt Service Routine									*/
/********************************************************************/
/* Autor: Christian Lohmeier										*/
/* Datum: 18.06.2003												*/
/* Firma: Videte IT	AG												*/
/********************************************************************/
#include "main.h"
#include "led.h"
bool bHBild,bSTrans,bChange;
bool bAN = false;
BYTE4 nCount=0;
BYTE4 nCount2=0;
double y;
//------------------------------------------------------------------------------//
// PPI_ISR						                                                //
// Beschreibung: Wird zyclisch 2 mal pro Halbbild aufgerufen                    //
//------------------------------------------------------------------------------//
EX_INTERRUPT_HANDLER(PPI_ISR)
{	
	*pFIO_DIR |= PF5;
	*pFIO_FLAG_T = PF5;
	
		byIC++;
		bHBild = false;		// erst ein halbes Halbbild empfangen	
	
		if (byIC==4)
		{
			bHBild=true;
			makePlanar();
			bMd = false;
		}
		if (byIC==8)
		{
		
			*pFIO_FLAG_C = PF5;
			bHBild=true;
			byIC = 0;
		
			*pDMA0_CONFIG	&=  ~DEN; 	// Disable PPI DMA
			*pPPI_CONTROL 	&=  ~DEN;	// Disable PPI  Port
			*pPPI_CONTROL = PACK_EN;//|DMA32;  // Packen, 32 Bit Transfer, nur aktives Bild
			*pPPI_FRAME = 625;  // Zeilen pro Vollbild	
			//-----------------------------------------------------------------------------------------------------//		
			// Set up DMA0 to receive from PPI
			//-----------------------------------------------------------------------------------------------------//
			*pDMA0_PERIPHERAL_MAP = 0x0000;		// lesen vom PPI
			// 32-bit transfers,2D DMA, Interrupt every FRAME/2, Autobuffer mode
			*pDMA0_CONFIG = MEMWR|SIZE_16|DMA2|IEN|FLOW_A|I2D; 
			// Start address of data buffer
			*pDMA0_START_ADDR = pBuffer->byInBuff_A;
			// DMA inner loop count
			*pDMA0_X_COUNT = FRAME*2/16;//FRAME * 2 / 16; 	// 1440 * 288 * 2 / (4 * 4)
		
			*pDMA0_X_MODIFY = 2;		// 32 Bit transfer => 4 *  1 Byte
			*pDMA0_Y_COUNT = 8; 		// 4 Buffer
			*pDMA0_Y_MODIFY = 2;		// 32 Bit am Ende des Buffers zuaddieren
			*pDMA0_CONFIG	|=  DEN; 	// Enable PPI DMA
			*pPPI_CONTROL 	|=  DEN;	// Enable PPI  Port	
			makePlanar();
			bMd = false; 
		}
	
	*pDMA0_IRQ_STATUS = 0x0001;	// Löscht Interrupt		
	
	

}

//------------------------------------------------------------------------------//
// BLOCK_ISR						                                            //
// Beschreibung: setzt bFullA auf 1 wenn acht Zeilen im L1 sind                 //
//------------------------------------------------------------------------------//
EX_INTERRUPT_HANDLER(BLOCKA_ISR)
{
	*pMDMA_D0_CONFIG = (*pMDMA_D0_CONFIG & 0xFFFE);	// Disable DMA
	*pMDMA_S0_CONFIG = (*pMDMA_S0_CONFIG & 0xFFFE);	// Disable DMA	
	bFullA = true;
	*pMDMA_S0_IRQ_STATUS = 0x0001;				// Löscht Interrupt
	*pMDMA_D0_IRQ_STATUS = 0x0001;				// Löscht Interrupt
}

//------------------------------------------------------------------------------//
// BLOCK_ISR						                                            //
// Beschreibung: setzt bFullB auf 1 wenn acht Zeilen im L1 sind                 //
//------------------------------------------------------------------------------//
EX_INTERRUPT_HANDLER(BLOCKB_ISR)
{
	*pMDMA_D1_CONFIG = (*pMDMA_D0_CONFIG & 0xFFFE);	// Disable DMA
	*pMDMA_S1_CONFIG = (*pMDMA_S0_CONFIG & 0xFFFE);	// Disable DMA	
	bFullB = true;
	*pMDMA_S1_IRQ_STATUS = 0x0001;				// Löscht Interrupt
	*pMDMA_D1_IRQ_STATUS = 0x0001;				// Löscht Interrupt
}


//------------------------------------------------------------------------------//
// SPORT_ISR						                                            //
// Beschreibung: Wird zyclisch nach jedem Halbbild aufgerufen                   //
//------------------------------------------------------------------------------//
EX_INTERRUPT_HANDLER(SPORT_ISR)
{
	BYTE4 i=0;
	bCapture = false;
	nFCSave++;		
	pPacket->Debug.bValid = (FALSE);
 	while (!(*pSPORT0_STAT & TXHRE) || (i == 100))
 	{
 		wait(1);
 		i++;
 	}
 	*pDMA4_CONFIG &= ~DEN;
	*pSPORT0_TCR1 &= ~DEN;	// SPORT Port ausschalten
	*pSPORT0_RCR1 &= ~DEN;
	bSport = false;
	pPacket->wSource = SLAVE;
	
 	*pSPORT0_STAT = *pSPORT0_STAT;
 	
	*pDMA4_IRQ_STATUS = *pDMA4_IRQ_STATUS ;	// Löscht Interrupt
	*pFIO_FLAG_C = PF5;		
}
EX_INTERRUPT_HANDLER(SPI_ISR)
{	

	bool bFine = false;
	BYTE i = 0;	
	static bool bCopyMask = false;
	BYTE* pMask_B = byMask_B;				// Cycle Counter starten

	DoConfirm();
  	if (bRx)
	{

		SPI_dis();
		if (bCopyMask)
		{
			memcpy (pMask_B, pSPIReceiveMask->byMask, pSPIReceiveMask->nDimH * pSPIReceiveMask->nDimV); 
//			free(pSPIReceiveMask);
			DoMessage(TMM_CONFIRM_SET_PERMANENT_MASK,0,0,21);
		}
		if (DoMessage(TMM_SLAVE_ASLEEP, pSPIReceivePacket->dwParam1,pSPIReceivePacket->dwParam2,254)) 
			CheckIncommingData();
		if(!bLongRx)
		{	

			*pSPI_CTL = SPITX;
			*pDMA5_CURR_DESC_PTR = usSPIDescriptor2;
	    	*pDMA5_CONFIG = SIZE_16|IEN|0x4500;
			bRx = false;
		}
	}
	else 
	{
		
		while(!bFine || (i == 100))
		{
			if(!(*pSPI_STAT & TXS))
			{
				wait(10);
				if(!(*pSPI_STAT & TXS))
					bFine = true;
			}
			i++;
		}
		SPI_dis();
		*pSPI_CTL = SPIRX;
		*pDMA5_CURR_DESC_PTR = usSPIDescriptor1;
	    *pDMA5_CONFIG = SIZE_16|IEN|MEMWR|0x4500;
		bRx = true;
		byRank = 0xFF;		
	}
	bCopyMask = bLongRx;
	bLongRx = false;
	SPI_en();
	*pDMA5_IRQ_STATUS = 0x0003;
}

EX_INTERRUPT_HANDLER(TIMERERROR_ISR)
{
	if(bFullB)
	{	
		IniMemDMA();
		blocktransfer_end();	// Wenn 2*8 Zeilen im L1 dann Transferend 
	}
	*pTIMER_STATUS = *pTIMER_STATUS ;	
}
