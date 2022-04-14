/********************************************************************/
/* Main Routine													    */
/********************************************************************/
/* Autor: Christian Lohmeier										*/
/* Datum: 18.08.2003												*/
/* Firma: Videte IT	AG												*/
/********************************************************************/
#include <math.h>
#include "main.h"

//--------------------------------------------------------------------------//
// Funktion: Main															//
//																			//
// Beschreibung: Initialisiert, SDRAM, I2C, ADC, SPORT,	TIMER, PPI, DMA, ISR//
//				 wartet auf Interrupt										//
//																			//
//--------------------------------------------------------------------------//
#define H_SIZ 	720
#define V_SIZ	288

BYTE4 nCount,nCount2,i;
BYTE byValue;
double y;
void CreateTestImage(YUV* pBuffer, int nHSize, int nVSize);

void submain()
{	
	bPush = false; 	
	byIC 			= 0;		// Löschen der Variablen
 	byMax 			= 0;
 	byCrossDis 		= 0;
 	byThresholdMD 	= THRESHOLD;
 	byThresholdMask = THRESHOLD1;
 	byIncrementMask = MOD;
 	byDelayMask 	= MASKDELAY;
 	byCounter		= 0;
 	nFrameC = 0;
 	byLineC = 0;
	bFullA = false;
	bFullB = false;
    bLed = false;
    bHBild = false;
    bSTrans = false;	
    bAlarm = true;
    bRx = true;
    bLongRx = false;
    byRank = 0xFF;
    byChannel = CHANNEL;
    	
	pHeader = byHeader;
    pFrameBuff_A = byFrameBuff_A;  //Pointer auf Buffer zeigen lassen
    pFrameBuff_B = byFrameBuff_B;  //Pointer auf Buffer zeigen lassen 		
    pFrameBuff_C = byFrameBuff_C;  //Pointer auf Buffer zeigen lassen
    
    pHeader2           = (BYTE2*) malloc(sizeof(DATA_PACKET));
    pSPIReceivePacket  = (SPIPacketStruct*) malloc(sizeof(SPIPacketStruct));
    pSPITransmitPacket = (SPIPacketStruct*) malloc(sizeof(SPIPacketStruct));
    
    CreateImageHeader(pHeader2,H_SIZE ,V_SIZE , SLAVE);
    
	Ini_Descriptor();
    sdram_ini();		// SDRAM initialisieren 
    clock_set();		// Setzt CCLK auf 540MHZ und volle Spannung
    
  	i2c_ini();     		// I2C Bus initialisieren 
 	adc_ini(); 		    // Konfigurieren des AD-Wandlers
   	sport_ini();		// Initialisieren des SPORT
	timer_ini();		// SPORT CLOCK setzen
//    ppi_ini();	     	// PPI programmieren
    dma_ini();	     	// DMA konfigurieren
    isr_ini();			// Interrupt Service Routine registrieren
//    dma_en();	      	// PPI DMA starten
	nCount = 0;
	nCount2 = 0;
	SPI_en();
//   clearled(0x2F);		       	// LED Anzeige löschen
/*    *pFIO_DIR &= ~PF8;
    *pFIO_INEN |= PF8;
    *pFIO_MASKA_S = PF8;
    *pFIO_MASKA_D = PF8;
    *pTIMER_ENABLE = 0x0001; 
*/    DoMessage(TMM_NOTIFY_SLAVE_INIT_READY,0,0,0);

	CreateTestImage((YUV*)pBuffer->byInBuff_A, H_SIZE, V_SIZE);
	CreateTestImage((YUV*)pBuffer->byInBuff_B, H_SIZE, V_SIZE);
	
	*pDMA4_CONFIG = SIZE_08|0x4500|DEN; 	
	*pSPORT0_TCR1	= (*pSPORT0_TCR1 | DEN);	// SPORT Port starten
	*pSPORT0_RCR1	= (*pSPORT0_RCR1 | DEN);
	 
	DoTrace("Ich bin ein kleiner BF533, mir geht es gut und wie geht es Dir? \n");
   byValue = 10;
	while(1)										// warten auf Interrupt 
	{
		nCount2++;

		y = (sin(0.0314159 * nCount2)+1)/2;
		*pFIO_DIR |= PF5;
		for(nCount= 0; nCount < 20000; nCount++)
		{
		if (bFullA && bFullB) blocktransfer_end();	// Wenn 2*8 Zeilen im L1 dann Transferend     
		if (nCount < ((BYTE4) (20000 * y)))
		*pFIO_FLAG_C = PF5;
		else
		*pFIO_FLAG_S = PF5;
		}
        
    }

	
	 
}
/*
void CreateTestImage(YUV* pBuffer, int nHSize, int nVSize)
{
	DATA_PACKET* pPacket = (DATA_PACKET*)pBuffer;
	YUV* pTmpBuff		 = 0;
	int  nX, nY, nI;
	
	pTmpBuff = pBuffer;
	

	for (nY = 0; nY < nVSize; nY++)
	{
		for (nX = 0; nX < nHSize; nX++)
		{
			pTmpBuff->UV   	= 255;
			pTmpBuff->Y  	= 255;
			pTmpBuff++;;
		}
	}
	
}
*/
void CreateTestImage(YUV* pBuffer, int nHSize, int nVSize)
{
	DATA_PACKET* pPacket = (DATA_PACKET*)pBuffer;
	YUV* pTmpBuff		 = 0;
	int  nX, nY, nI;
	float fX1 = 0.0;
	float fX2 = 0.0;
	float dX  = 0.0;
	
//	pBuffer = (YUV*)&pPacket->pImageData;
	
	pTmpBuff = pBuffer;
	
	dX = (float)256/(float)nHSize;
	// Graukeil 
	for (nY = 0; nY < nVSize; nY++)
	{
		fX1 = 0;
		for (nX = 0; nX < nHSize; nX++)
		{
			pTmpBuff->UV   = (int)fX1;
			pTmpBuff->Y  = 0x80;
			pTmpBuff++;;
			fX1 += dX;
		}
	}
	
	dX = (float)nHSize/(float)nVSize;
	fX1 = 0;
	fX2 = nHSize;
	nY = 0;
	
	// Diagonalen 
	for (nI = 0; nI < nVSize; nI++)
	{
		fX1 += dX;
		fX2 -= dX;
		nY  += 1;
		
		pTmpBuff = (YUV*)pBuffer + ((int)fX1+nY*nHSize);
		pTmpBuff->UV	= 16*nI;
		pTmpBuff->Y  = 0x80;

		pTmpBuff = (YUV*)pBuffer + ((int)fX2+nY*nHSize);
		pTmpBuff->UV	= 16*nI;
		pTmpBuff->Y  = 0x80;
	}
	
	nX = 0;
	nY = nVSize/2;
	
	// Wagerechte Linie
	for (nI = 0; nI < nHSize; nI++)
	{
		pTmpBuff = (YUV*)pBuffer + (nX+nY*nHSize);
		pTmpBuff->UV	= 16*nI;
		pTmpBuff->Y  = 0x80;
		nX+=1;
	}

	nX = nHSize/2;
	nY = 0;
	
	// Sebkrechte Linie
	for (nI = 0; nI < nVSize; nI++)
	{
		pTmpBuff = (YUV*)pBuffer + (nX+nY*nHSize);
		pTmpBuff->UV	= 16*nI;
		pTmpBuff->Y  = 0x80;
		nY+=1;
	}

}

