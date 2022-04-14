/********************************************************************/
/* Main Routine													    */
/********************************************************************/
/* Autor: Christian Lohmeier										*/
/* Datum: 18.08.2003												*/
/* Firma: Videte IT	AG												*/
/********************************************************************/
#include "motion.h"
#include "main.h"

//--------------------------------------------------------------------------//
// Funktion: Main															//
//																			//
// Beschreibung: Initialisiert, SDRAM, I2C, ADC, SPORT,	TIMER, PPI, DMA, ISR//
//				 wartet auf Interrupt										//
//																			//
//--------------------------------------------------------------------------//
#include "main.h"

#define H_SIZE 	352
#define V_SIZE	288
#define CHANNEL 0 

void CreateTestImage(YUV* pBuffer, int nHSize, int nVSize, int nChannel);

void submain()
{	
	BYTE4 nCount;			// Zähler zum Löschen
	bool bAN;
	int   nX = 0;
	int   nY = 0;
	int	  nI = 0;
	DWORD dwIndex = 0;
	
	*pPLL_DIV = 0x0004; 	// SCLKvisor: CCLK / 4 = SCLK
	*pVR_CTL = 0x00dB;     	// Kernspannung auf 1.2 V (Full Power Mode)

	bAN = false;
	pInBuffA = 0;			// Eingangsbuffer A
	
	*pEBIU_AMGCTL = 0x00FE;
	*pEBIU_SDRRC = 0x0817;	// SDRAM Schnittstelle programmieren: RDIV = 1866 Cycles
	*pEBIU_SDBCTL = 0x0013;	// Bank 0 enabled, 32 MB, address width 9 Bit
	*pEBIU_SDGCTL = 0x0091998D;// 16 Bit Datenbus, kein externes Buffering,  = 2 SCLK,

	CreateTestImage((YUV*)pInBuffA, H_SIZE, V_SIZE, CHANNEL);
		 
////////////////////////////////////////////////////////////////////////////////////////////// 	 	
	
	*pDMA4_CONFIG	= (*pDMA4_CONFIG & ~DEN);	// DMA anschalten
	*pSPORT0_TCR1	= (*pSPORT1_TCR1 & ~DEN);	// SPORT Port starten
	*pSPORT0_RCR1	= (*pSPORT1_RCR1 & ~DEN);	// SPORT Port starten

	// 16-bit data
	*pSPORT0_RCR1 = RFSR;//|IRCLK|IRFS;
	*pSPORT0_RCR2 = SLEN_16;
	
	// Sport1 transmit configuration
	// External CLK, External Frame sync, MSB first
	// 16-bit data
	*pSPORT0_TCR1 = TFSR;//|ITCLK|ITFS;
	*pSPORT0_TCR2 = SLEN_16;

	// Enable MCM 4 transmit & receive channels
	*pSPORT0_MTCS0 = 0x00000003<<(2*CHANNEL);	// Channel 1&2 (2x16Bit=32Bit)	
	*pSPORT0_MRCS0 = 0x00000003<<(2*CHANNEL);		
	
	// Set MCM configuration register and enable MCM mode
	*pSPORT0_MCMC1 = 0x0000;	//WSIZE=8Channels
	*pSPORT0_MCMC2 = 0x001c;	//MC enable, MC-DMA Transmit Packing

	//////////////////////////////////////////////////////////////////////////////////////////////	
	*pDMA4_PERIPHERAL_MAP = 0x2000;		
	// 16-bit transfers,2D DMA, Autobuffer mode
	*pDMA4_CONFIG = SIZE_16|FLOW_A|DMA2;
	// Start address of data buffer
	*pDMA4_START_ADDR = pInBuffA;
	// DMA inner loop count
	*pDMA4_X_COUNT = H_SIZE;
	*pDMA4_Y_COUNT = V_SIZE+2;
	*pDMA4_X_MODIFY = 2;		
	*pDMA4_Y_MODIFY = 2;		
	
	//////////////////////////////////////////////////////////////////////////////////////////////	
	*pDMA4_CONFIG	= (*pDMA4_CONFIG | DEN);	// DMA anschalten
	*pSPORT0_TCR1	= (*pSPORT0_TCR1 | DEN);	// SPORT Port starten
	*pSPORT0_RCR1	= (*pSPORT0_RCR1 | DEN);	// SPORT Port starten
	
	nCount = 0;
	led(1);
	while(1)
	{
		nCount++;
		if((600000 == nCount) && !bAN)
		{
			bAN = true; 
			led(1<<CHANNEL);
			nCount = 0;
		}
		if((600000 == nCount) && bAN)
		{
			bAN = false; 
			led(0);
			nCount = 0;
		}	
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////	
void CreateTestImage(YUV* pBuffer, int nHSize, int nVSize, int nChannel)
{
	DATA_PACKET* pPacket = (DATA_PACKET*)pBuffer;
	YUV* pTmpBuff		 = 0;
	int  nX, nY, nI;
	float fX1 = 0.0;
	float fX2 = 0.0;
	float dX  = 0.0;
	
	pPacket->bValid				= TRUE;
	pPacket->dwMarker			= MAGIC_MARKER;
	pPacket->wSize				= sizeof(DATA_PACKET);
	pPacket->eType				= eTypeYUV422;
	pPacket->eSubType			= eSubTypeUnvalid;
	pPacket->wSource			= nChannel;
	pPacket->wSizeH				= nHSize;
	pPacket->wSizeV				= nVSize;
	pPacket->wBytesPerPixel		= 2;
	pPacket->wField				= 1;
	pPacket->bVidPresent		= TRUE;
	pPacket->bMotion			= FALSE;
	pPacket->Point[0].cx		= 10;
	pPacket->Point[0].cy		= 15;
	pPacket->Point[0].nValue	= 100;
	pPacket->Point[1].cx		= (WORD)-1;
	pPacket->Point[1].cy		= (WORD)-1;
	pPacket->Point[1].nValue	= 200;
	pPacket->Point[2].cx		= (WORD)-1;
	pPacket->Point[2].cy		= (WORD)-1;
	pPacket->Point[2].nValue	= 300;
	pPacket->Point[3].cx		= (WORD)-1;
	pPacket->Point[3].cy		= (WORD)-1;
	pPacket->Point[3].nValue	= 400;
	pPacket->Point[4].cx		= (WORD)-1;
	pPacket->Point[4].cy		= (WORD)-1;
	pPacket->Point[4].nValue	= 500;
	pPacket->TimeStamp.bValid 	= FALSE;
	pPacket->TimeStamp.wYear	= 2003;
	pPacket->TimeStamp.wMonth	= 8;
	pPacket->TimeStamp.wDay		= 4;
	pPacket->TimeStamp.wHour	= 13;
	pPacket->TimeStamp.wMinute	= 3;
	pPacket->TimeStamp.wSecond	= 34;
	pPacket->TimeStamp.wMSecond	= 333;
	pPacket->dwFieldID			= 123;
	pPacket->dwProzessID		= 0;
	pPacket->dwImageDataLen		= nHSize*nVSize*pPacket->wBytesPerPixel;

	pBuffer = (YUV*)&pPacket->pImageData;
	
	pTmpBuff = pBuffer;
	
	dX = (float)256/(float)nHSize;

	// Graukeil 
	for (nY = 0; nY < nVSize; nY++)
	{
		fX1 = 0;
		for (nX = 0; nX < nHSize; nX++)
		{
			pTmpBuff->Y   = (int)fX1;
			pTmpBuff->UV  = 0x80;
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
		pTmpBuff->Y	= 16*nI;
		pTmpBuff->UV  = 0x80;

		pTmpBuff = (YUV*)pBuffer + ((int)fX2+nY*nHSize);
		pTmpBuff->Y	= 16*nI;
		pTmpBuff->UV  = 0x80;
	}
	
	nX = 0;
	nY = nVSize/2;
	
	// Wagerechte Linie
	for (nI = 0; nI < nHSize; nI++)
	{
		pTmpBuff = (YUV*)pBuffer + (nX+nY*nHSize);
		pTmpBuff->Y	= 16*nI;
		pTmpBuff->UV  = 0x80;
		nX+=1;
	}

	nX = nHSize/2;
	nY = 0;
	
	// Sebkrechte Linie
	for (nI = 0; nI < nVSize; nI++)
	{
		pTmpBuff = (YUV*)pBuffer + (nX+nY*nHSize);
		pTmpBuff->Y	= 16*nI;
		pTmpBuff->UV  = 0x80;
		nY+=1;
	}
}

