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
//------------------------------------------------------------------------------//
// PPI_ISR						                                                //
// Beschreibung: Wird zyclisch 2 mal pro Halbbild aufgerufen                    //
//------------------------------------------------------------------------------//
EX_INTERRUPT_HANDLER(PPI_ISR)
{		 
	*pDMA0_IRQ_STATUS = 0x0001;	// Löscht Interrupt
	byIC++;
	if (byIC == 2) 				// Bei jedem Halbbild		
	{
	
		bHBild = true;			// Halbbild in InBuff = wahr			
		byIC = 0;			    // Halbbildzähler zurücksetzen
		pPacket->Debug.bValid = SwapEndian(FALSE);
		if (nFrameC > nSaveFrameC)
		{
			pPacket->PermanentMask.bHasChanged = SwapEndian(FALSE); 
			nSaveFrameC = 0;
		}
		nFrameC++;			    // Framezähler erhöhen	
		
		nChecksum = 0;
		if (bChange) docheck((DWORD) pBuffer->byInBuff_A);
		else docheck((DWORD) pBuffer->byInBuff_B);
		blocktransfer_start();	// 8 Zeilen des Frames und des Letzten in L1 verschieben	  		
	}
	else 
	{
	    *pDMA4_IRQ_STATUS = 0x0003;	// Löscht Interrupt
		*pDMA4_CONFIG &= ~DEN;
	    *pSPORT0_TCR1 &= ~DEN;	// SPORT Port ausschalten
		*pSPORT0_RCR1 &= ~DEN;
		pPacket->dwCheckSum = SwapEndian(nChecksum);
	    if (*pDMA0_CURR_Y_COUNT < 2) 				// wechselt zwischen Buffer A und B beim Senden über SPORT 
	    {
	    	*pDMA4_CURR_DESC_PTR = usDescriptor1;
	    	*pDMA4_CONFIG = SIZE_08|0x4500|DEN;
	    	bChange = false;
	    }
		if (*pDMA0_CURR_Y_COUNT > 2)
		{
	    	*pDMA4_CURR_DESC_PTR = usDescriptor2;
	    	*pDMA4_CONFIG = SIZE_08|0x4500|DEN;
	    	bChange = true;
	   	}
		bHBild = false;		// erst ein halbes Halbbild empfangen
		*pSPORT0_TCR1	= (*pSPORT0_TCR1 | DEN);	// SPORT Port starten
		*pSPORT0_RCR1	= (*pSPORT0_RCR1 | DEN);
	}

}

//------------------------------------------------------------------------------//
// BLOCK_ISR						                                            //
// Beschreibung: setzt bFullA auf 1 wenn acht Zeilen im L1 sind                 //
//------------------------------------------------------------------------------//
EX_INTERRUPT_HANDLER(BLOCKA_ISR)
{
	*pMDMA_S0_IRQ_STATUS = 0x0001;				// Löscht Interrupt
	*pMDMA_D0_IRQ_STATUS = 0x0001;				// Löscht Interrupt
	
	*pMDMA_D0_CONFIG = (*pMDMA_D0_CONFIG & 0xFFFE);	// Disable DMA
	*pMDMA_S0_CONFIG = (*pMDMA_S0_CONFIG & 0xFFFE);	// Disable DMA	
	
	bFullA = true;
}

//------------------------------------------------------------------------------//
// BLOCK_ISR						                                            //
// Beschreibung: setzt bFullB auf 1 wenn acht Zeilen im L1 sind                 //
//------------------------------------------------------------------------------//
EX_INTERRUPT_HANDLER(BLOCKB_ISR)
{
	*pMDMA_S1_IRQ_STATUS = 0x0001;				// Löscht Interrupt
	*pMDMA_D1_IRQ_STATUS = 0x0001;				// Löscht Interrupt
	
	*pMDMA_D1_CONFIG = (*pMDMA_D1_CONFIG & 0xFFFE);	//Disable DMA
	*pMDMA_S1_CONFIG = (*pMDMA_S1_CONFIG & 0xFFFE);	//Disable DMA

	bFullB = true;
}


//------------------------------------------------------------------------------//
// SPORT_ISR						                                            //
// Beschreibung: Wird zyclisch nach jedem Halbbild aufgerufen                   //
//------------------------------------------------------------------------------//
EX_INTERRUPT_HANDLER(SPORT_ISR)
{
	*pDMA4_IRQ_STATUS = 0x0001;	// Löscht Interrupt
	while (!(*pSPORT0_STAT & TXHRE))
		asm("nop;"); 
	*pDMA4_CONFIG &= ~DEN;
	*pSPORT0_TCR1 &= ~DEN;	// SPORT Port ausschalten
	*pSPORT0_RCR1 &= ~DEN;
	byChannel++;
	if (byChannel >3) byChannel = 0;
	// Enable MCM 4 transmit & receive channels
	*pSPORT0_MTCS0 = 0x0000000f<<(byChannel<<2);	// Channel 1&2 (2x16Bit=32Bit)	
	*pSPORT0_MRCS0 = 0x00000000;
	pPacket->wSource = SwapEndianSmal((BYTE4) byChannel+4);	
}

EX_INTERRUPT_HANDLER(SPI_ISR)
{
	static bool bCopyMask = false;
	BYTE* pMask_B = byMask_B;
 	startc();				// Cycle Counter starten
	DoConfirm();
  	if (bRx)
	{	
		SPI_dis();
		if (bCopyMask)
		{
			memcpy (pMask_B, pSPIReceiveMask->byMask, pSPIReceiveMask->nDimH * pSPIReceiveMask->nDimV); 
			free(pSPIReceiveMask);
			DoMessage(TMM_CONFIRM_SET_PERMANENT_MASK,0,0,21);
		}
		if (DoMessage(TMM_SLAVE_ASLEEP, pSPIReceivePacket->dwParam1,pSPIReceivePacket->dwParam2,254)) 
			CheckIncommingData();
		setled(1);
		if(!bLongRx)
		{
			*pSPI_CTL = SZ | SPISIZE_16 | SPIDMATX | EMISO;
			*pDMA5_CURR_DESC_PTR = usSPIDescriptor2;
	    	*pDMA5_CONFIG = SIZE_16|IEN|0x4500;
			bRx = false;
		}
	}
	else 
	{	
		wait(100);
		SPI_dis();
		clearled(1);
		*pSPI_CTL = SZ | SPISIZE_16 | SPIDMARX;
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
	if (*pTIMER_STATUS & 0x0001)
	{
		*pTIMER_STATUS = *pTIMER_STATUS ;
		if (pftest(PF9)) // Taster PF9 prüfen wenn 1 dann Abbruch
    	{
    		clearled(16);
    		DoMessage(TMM_NOTIFY_INFORMATION,INFO_SLAVE_PF_REQUEST,0,41);
    	} 
    	if (pftest(PF10))
    	{
    		DoTrace("Hallo dies ist ein TEST\n");	
    	}
		if (!bAN)
		{
			setled(1<<CHANNEL);
			bAN = true;
		}
		else
		{
			clearled(1<<CHANNEL);
			bAN = false;
		}	
	}
	else
	{
		if (*pDMA0_IRQ_STATUS & DMA_ERR)
		{
			DoTrace("DMA0 Data Error\n");
			DoMessage(TMM_NOTIFY_INFORMATION,INFO_PPI_ERROR,0,13);
			*pDMA0_IRQ_STATUS = *pDMA0_IRQ_STATUS;
		}
		if (*pPPI_STATUS & FT_ERR)
		{
			DoTrace("Frame Track Error\n");
			*pSYSCR = 0x0010;
			asm("raise 1;");
			// *pSWRST = 0x0007;
		}
		
		if (*pPPI_STATUS & OVR)
			DoTrace("FIFO Overflow\n");
		if (*pPPI_STATUS & UNDR)
			DoTrace("FIFO Underrun\n");	
		if (*pSPORT0_STAT & TOVF)
		{
			DoMessage(TMM_NOTIFY_INFORMATION,INFO_SPORT_ERROR,0,10);
			*pSPORT0_STAT  = *pSPORT0_STAT;
		}
		if (*pPPI_STATUS & ERR_DET) 
	       	DoMessage(TMM_NOTIFY_INFORMATION,INFO_PPI_PREAMBEL_ERROR,0,15);
	    if (*pPPI_STATUS & ERR_NCOR) 
 	      	DoMessage(TMM_NOTIFY_INFORMATION,INFO_PPI_PREAMBEL_ERROR_NO_CORR,0,14);

	}
}
