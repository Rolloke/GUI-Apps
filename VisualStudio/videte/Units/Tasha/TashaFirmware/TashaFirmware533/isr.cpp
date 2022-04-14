/********************************************************************/
/* ISR Interrupt Service Routine									*/
/********************************************************************/
/* Autor: Christian Lohmeier										*/
/* Datum: 16.07.2003												*/
/* Firma: Videte IT	AG												*/
/********************************************************************/
#include "main.h"
#include "codec.h"
#include "ppi.h"
#include "i2c.h"

bool bChange;

extern tMPEG4VideoEncParam lVideoEncParam0, lVideoEncParam1;
extern volatile BYTE2 usDescriptor1[];
extern bool bVideoAktive;
BYTE4 nFieldCSave=0;
BYTE4 nFieldCSave1=0;
float fFrameInterval;

//------------------------------------------------------------------------------//
// BLOCK_ISR						                                            //
// Beschreibung: Wenn YUV planar fertig						                    //
//------------------------------------------------------------------------------//
EX_INTERRUPT_HANDLER(BLOCKA_ISR)
{
	static float fRest = 0;
	*pMDMA_D1_CONFIG &= ~DEN;	// Disable DMA
	*pMDMA_S1_CONFIG &= ~DEN;	// Disable DMA	
	bFullA = true; 
	BYTE byFRate = 25;
	if (bySetNorm == 1)
		byFRate = 30;
	fFrameInterval = (byFRate/lVideoEncParam0.VOPRate);

	if((nFieldC-nFieldCSave)>=(BYTE) fFrameInterval)
	{
	
		nFieldCSave=nFieldC;
		if(lVideoEncParam0.VOPRate < 12)
		{
			fRest += fFrameInterval - (BYTE)fFrameInterval;
			if (fRest >= 1)
			{
				nFieldCSave++;
				fRest = 0;
			}
		}
		bStreamCode0 = true;
	}
	
	fFrameInterval = (byFRate/lVideoEncParam1.VOPRate);
	if((nFieldC-nFieldCSave1)>=(BYTE) fFrameInterval)
	{
		if(dwRequestCount > 0)
		{
			bStreamCode1 = true;
			dwRequestCount--;
		}
		nFieldCSave1=nFieldC;
		
	}

#if (0) // ML	
	if (!bPush)
	{
		IniMemDMA();
		blocktransfer_start();	// 8 Zeilen des Frames und des Letzten in L1 verschieben
	}
#endif
	nFieldC++;
	*pMDMA_S1_IRQ_STATUS = 0x0001;				// Löscht Interrupt
	*pMDMA_D1_IRQ_STATUS = 0x0001;				// Löscht Interrupt
}
/////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------//
// BLOCK_ISR						                                            //
// Beschreibung: wenn je 8 Zeilen im L1 für Differenzbild		                //
//------------------------------------------------------------------------------//
EX_INTERRUPT_HANDLER(BLOCKB_ISR)
{
	*pMDMA_D0_CONFIG &= ~DEN;	// Disable DMA
	*pMDMA_S0_CONFIG &= ~DEN;	// Disable DMA	
	bFullB = true;
	IniMemDMA();
	blocktransfer_end();		// Wenn 2*8 Zeilen im L1 dann Transferend 
	*pMDMA_S0_IRQ_STATUS = 0x0001;	// Löscht Interrupt
	*pMDMA_D0_IRQ_STATUS = 0x0001;	// Löscht Interrupt
}

/////////////////////////////////////////////////////////////////////////////
#if (1) // ML (Liefert auch zu Beginn den richtigen Status)
// Prüft auf voranden Kameras und PAL/NTSC
EX_INTERRUPT_HANDLER(TIMERERROR_ISR)
{
static BYTE byFeStatus;
	*pFIO_INEN = PF10;
	if(!(*pFIO_FLAG_D & PF10))
	{		
		usDescriptor1[7] = SIZE_16|DEN|IEN;
		usDescriptor1[8] = 	(pPacket->dwImageDataLen+8)>>1;
		pPacket->bVidPresent = FALSE;
		pPacket->dwImageDataLen = 0;
		pPacket->dwCheckSum = 0;
		pPacket->dwFieldID = nFrameC;
		pPacket->eSubType 	= eSubIFrame;
		pPacket->bValid = TRUE;
		bSportStart = true;	
		bVideoAktive = false;		
		bMd= false;
	}
	else
	{
		bVideoAktive = true;
	}
	
	if (i2c_read(SLAVE_ADDRESS, FE_STATUS, &byHelper))
	{
		if((byHelper & STAT_NTSC) != (byFeStatus & STAT_NTSC))	
		{	
			byFeStatus = byHelper;		
			bySetNorm = 0;
			if (byHelper & STAT_NTSC)	
				bySetNorm = 1;
			bStreamChange0 = true;
			bStreamChange1 = true;
			lVideoEncParam0.VOPWidth  = wNormH[bySetNorm][eResulution[0]];		// Bildweite 
			lVideoEncParam0.VOPHeight = wNormV[bySetNorm][eResulution[0]];	
			lVideoEncParam1.VOPWidth  = wNormH[bySetNorm][eResulution[1]];		// Bildweite 
			lVideoEncParam1.VOPHeight = wNormV[bySetNorm][eResulution[1]];
		}
	}	
	
	*pTIMER_STATUS = *pTIMER_STATUS ;	
}
#else
// Prüft auf voranden Kameras und PAL/NTSC
EX_INTERRUPT_HANDLER(TIMERERROR_ISR)
{
static BYTE byFeStatus;
	bVideoAktive = true;
	*pFIO_INEN = PF10;
	if(!(*pFIO_FLAG_D & PF10)&&bInSwitch)
	{		
		usDescriptor1[7] = SIZE_16|DEN|IEN;
		usDescriptor1[8] = 	(pPacket->dwImageDataLen+8)>>1;
		pPacket->bVidPresent = FALSE;
		pPacket->dwImageDataLen = 0;
		pPacket->dwCheckSum = 0;
		pPacket->dwFieldID = nFrameC;
		pPacket->eSubType 	= eSubIFrame;
		pPacket->bValid = TRUE;
		bSportStart = true;	
		bVideoAktive = false;		
		bMd= false;
	
	}
	
	if (i2c_read(SLAVE_ADDRESS, FE_STATUS, &byHelper))
	{
		if((byHelper & STAT_NTSC) != (byFeStatus & STAT_NTSC))	
		{	
			byFeStatus = byHelper;		
			bySetNorm = 0;
			if (byHelper & STAT_NTSC)	
				bySetNorm = 1;
			bStreamChange0 = true;
			bStreamChange1 = true;
			lVideoEncParam0.VOPWidth  = wNormH[bySetNorm][eResulution[0]];		// Bildweite 
			lVideoEncParam0.VOPHeight = wNormV[bySetNorm][eResulution[0]];	
			lVideoEncParam1.VOPWidth  = wNormH[bySetNorm][eResulution[1]];		// Bildweite 
			lVideoEncParam1.VOPHeight = wNormV[bySetNorm][eResulution[1]];
		}
	}	
	
	*pTIMER_STATUS = *pTIMER_STATUS ;	
}
#endif
/////////////////////////////////////////////////////////////////////////////
