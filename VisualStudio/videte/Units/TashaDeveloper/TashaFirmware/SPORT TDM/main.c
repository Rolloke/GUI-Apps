/********************************************************************/
/* Main Routine													    */
/********************************************************************/
/* Autor: Christian Lohmeier										*/
/* Datum: 18.08.2003												*/
/* Firma: Videte IT	AG												*/
/********************************************************************/
#include "motion.h"
#include "main.h"

#define CHANNEL 0
#define H_SIZE 	256
#define V_SIZE	128

//--------------------------------------------------------------------------//
// Funktion: Main															//
//																			//
// Beschreibung: Initialisiert, SDRAM, I2C, ADC, SPORT,	TIMER, PPI, DMA, ISR//
//				 wartet auf Interrupt										//
//																			//
//--------------------------------------------------------------------------//
#include "main.h"

void CreateTestImage(YUV* pBuffer, int nHSize, int nVSize, int nChannel);

void main()
{	
	BYTE4 nCount;			// Zähler zum Löschen
	int   nX = 0;
	int   nY = 0;
	int	  nI = 0;
	DWORD dwIndex = 0;
	
/*	*pPLL_DIV = 0x0004; 	// SCLKvisor: CCLK / 4 = SCLK
	clk();					// CCLK = 540 MHz
	*pVR_CTL = 0x00dB;     	// Kernspannung auf 1.2 V (Full Power Mode)
*/
	
	pInBuffA = 0;			// Eingangsbuffer A

	*pEBIU_SDRRC = 0x0817;	// SDRAM Schnittstelle programmieren: RDIV = 1866 Cycles
	*pEBIU_SDBCTL = 0x0013;	// Bank 0 enabled, 32 MB, address width 9 Bit
	*pEBIU_SDGCTL = 0x0091998D;// 16 Bit Datenbus, kein externes Buffering,  = 2 SCLK,

	CreateTestImage((YUV*)pInBuffA+sizeof(DATA_PACKET), H_SIZE, V_SIZE, CHANNEL);
		 
////////////////////////////////////////////////////////////////////////////////////////////// 	 	
	
	// 16-bit data
	*pSPORT1_RCR1 = RFSR;
	*pSPORT1_RCR2 = SLEN_16;
	
	// Sport1 transmit configuration
	// External CLK, External Frame sync, MSB first
	// 16-bit data
	*pSPORT1_TCR1 = TFSR;
	*pSPORT1_TCR2 = SLEN_16;

	// Enable MCM 4 transmit & receive channels
	*pSPORT1_MTCS0 = 0x00000003;	// Channel 1&2 (2x16Bit=32Bit)	
	*pSPORT1_MRCS0 = 0x00000003;		
	
	// Set MCM configuration register and enable MCM mode
	*pSPORT1_MCMC1 = 0x0000;	//WSIZE=8Channels
	*pSPORT1_MCMC2 = 0x001c;	//MC enable, MC-DMA Transmit Packing

	//////////////////////////////////////////////////////////////////////////////////////////////	
#if (0)	
	*pDMA4_PERIPHERAL_MAP = 0x4000;		
	// 16-bit transfers,2D DMA, Autobuffer mode
	*pDMA4_CONFIG = SIZE_16|FLOW_A|DMA2;
	// Start address of data buffer
	*pDMA4_START_ADDR = pInBuffA;
	// DMA inner loop count
	*pDMA4_X_COUNT = H_SIZE;
	*pDMA4_Y_COUNT = V_SIZE;
	*pDMA4_X_MODIFY = 2;		
	*pDMA4_Y_MODIFY = 2;		
#else
	*pDMA4_PERIPHERAL_MAP = 0x4000;		
	// 16-bit transfers,1D DMA, Autobuffer mode
	*pDMA4_CONFIG = SIZE_16|FLOW_A;
	// Start address of data buffer
	*pDMA4_START_ADDR = pInBuffA;
	// DMA inner loop count
	*pDMA4_X_COUNT = H_SIZE*V_SIZE; //32768;

	*pDMA4_X_MODIFY = 2;		

#endif	
	
//////////////////////////////////////////////////////////////////////////////////////////////	
	*pDMA4_CONFIG	= (*pDMA4_CONFIG | DEN);	// DMA anschalten
	*pSPORT1_TCR1	= (*pSPORT1_TCR1 | DEN);	// SPORT Port starten
	*pSPORT1_RCR1	= (*pSPORT1_RCR1 | DEN);	// SPORT Port starten

	while(1)
	{
		asm("nop;nop;nop;");	
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////	
void CreateTestImage(YUV* pBuffer, int nHSize, int nVSize, int nChannel)
{
	DATA_PACKET* pPacket = (DATA_PACKET*)pBuffer;
	YUV* pTmpBuff		 = 0;
	int  nX, nY, nI;
		
	pPacket->bValid				= TRUE;
	pPacket->dwMarker			= 0x12345678;
	pPacket->wSize				= sizeof(DATA_PACKET);
	pPacket->eType				= eTypeYUV422;
	pPacket->eSubType			= eSubTypeUnvalid;
	pPacket->wSource			= nChannel;
	pPacket->wSizeH				= 240;
	pPacket->wSizeV				= 136;
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
	pPacket->dwImageDataLen		= pPacket->wSizeH*pPacket->wSizeV*pPacket->wBytesPerPixel;
	
	pBuffer += sizeof(DATA_PACKET);
	
	pTmpBuff = pBuffer;
	
	// Graukeil 
	for (nY = 0; nY < nVSize; nY++)
	{
		for (nX = 0; nX < nHSize; nX++)
		{
			pTmpBuff->Y 	= nX;
			pTmpBuff->UV  = 0x80;
			pTmpBuff++;
		}
	}
	nX = 0;
	nY = 0;
	pTmpBuff = pBuffer;
	
	// Diagonale (linksoben -> rechts unten)
	for (nI = 0; nI < nVSize; nI++)
	{
		pTmpBuff->Y	= 16*nI;
		pTmpBuff->UV  = 0x80;
		nX+=2;
		nY+=1;
		pTmpBuff = (YUV*)pBuffer + (nX+nY*nHSize);
	}
	nX = nHSize;
	nY = 0;
	pTmpBuff = pBuffer;
	
	// Diagonale (rechts oben -> links unten)
	for (nI = 0; nI < nVSize; nI++)
	{
		pTmpBuff->Y	= 16*nI;
		pTmpBuff->UV  = 0x80;
		nX-=2;
		nY+=1;
		pTmpBuff = (YUV*)pBuffer + (nX+nY*nHSize);
	}

	nX = 0;
	nY = nVSize/2;
	pTmpBuff = pBuffer;
	
	// Wagerechte Linie
	for (nI = 0; nI < nHSize; nI++)
	{
		pTmpBuff->Y	= 16*nI;
		pTmpBuff->UV  = 0x80;
		nX+=1;
		pTmpBuff = (YUV*)pBuffer + (nX+nY*nHSize);
	}

	nX = nHSize/2;
	nY = 0;
	pTmpBuff = pBuffer;
	
	// Sebkrechte Linie
	for (nI = 0; nI < nVSize; nI++)
	{
		pTmpBuff->Y	= 16*nI;
		pTmpBuff->UV  = 0x80;
		nY+=1;
		pTmpBuff = (YUV*)pBuffer + (nX+nY*nHSize);
	}
}
