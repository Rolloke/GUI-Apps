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

#define H_SIZE 	240
#define V_SIZE	136
#define CHANNEL 2

BYTE4 ntest;
void CreateTestImage(YUV* pBuffer, int nHSize, int nVSize, int nChannel);
//EX_INTERRUPT_HANDLER(SPORT_ISR);


void submain()
{	
	BYTE4 nCount;			// Zähler zum Löschen
	bool bAN;
	int   nX = 0;
	int   nY = 0;
	int	  nI = 0;
	DWORD dwIndex = 0;
	
	ntest = SwapEndian(0x34127856);
	*pPLL_DIV = 0x0004; 	// SCLKvisor: CCLK / 4 = SCLK
	*pVR_CTL = 0x00dB;     	// Kernspannung auf 1.2 V (Full Power Mode)

	bAN = false;
	pInBuffA = 0;			// Eingangsbuffer A
	
	*pEBIU_AMGCTL = 0x00FE;
	*pEBIU_SDRRC = 0x0817;	// SDRAM Schnittstelle programmieren: RDIV = 1866 Cycles
	*pEBIU_SDBCTL = 0x0013;	// Bank 0 enabled, 32 MB, address width 9 Bit
	*pEBIU_SDGCTL = 0x0091998D;// 16 Bit Datenbus, kein externes Buffering,  = 2 SCLK,
		
		i2c_ini();
		i2c_write(0x88,0x00,0x80); 	// FBAS Eingang, PAL BGH
		i2c_write(0x88,0x01,0x80); 	// default
   		i2c_write(0x88,0x02,0x04); 	// kein Filter
    	i2c_write(0x88,0x03,0x0C); 	// ITU 656 Format 8 Bit 4:2:2 
    	i2c_write(0x88,0x04,0x84); 	// ITU 601 Kompatibel 	
    	i2c_write(0x88,0x05,0x00);	// GPIO Register nicht gesetzt 
    	i2c_write(0x88,0x07,0x04); 	// FIFO Kontrolle: Normale Operation
    	i2c_write(0x88,0x08,0x80); 	// Kontrasteinstellung
    	i2c_write(0x88,0x09,0x80); 	// Farbeinstellung 0dB
    	i2c_write(0x88,0x0A,0x00); 	// Helligkeitseinstellung 0dB
    	i2c_write(0x88,0x0B,0x00); 	// Farbwinkel (NTSC) 0°
    	i2c_write(0x88,0x0C,0x10); 	// default Y Wert 16
    	i2c_write(0x88,0x0D,0x88); 	// default C Wert 128 
    	i2c_write(0x88,0x0E,0x00); 	// keine Dezimation
    	i2c_write(0x88,0x0F,0x00); 	// kein Powermanagment
    	i2c_write(0x88,0x13,0x45); 	// H-SYNC, V-SYNC, Field Ausgang ist abhängig von !OE
    	i2c_write(0x88,0x14,0x18); 	// Analog-Klemmung Register
    	i2c_write(0x88,0x15,0x60); 	// Digital-Klemmung-Register
    	i2c_write(0x88,0x17,0x01); 	// Auto Schärfungsfilter
    	i2c_write(0x88,0x19,0x10); 	// Adaptiver Farbfilter
    	i2c_write(0x88,0x23,0xE0); 	// Auto Farbträgerfrequenz
    	i2c_write(0x88,0x27,0x58); 	// Kein Pixeldelay
    	i2c_write(0x88,0x28,0x3F);  // Taktausgang gesteuert durch das Videosignal  
		i2c_write(0x88,0x2C,0xCE); 	// AGC (Auto Gain Control)  
    	i2c_write(0x88,0x2D,0xF0);  // Choma Gain Control
    	i2c_write(0x88,0x2F,0xF0); 	// Luma Gain Control  
    	i2c_write(0x88,0x31,0x70);  // Kein Gain Shadow
    	i2c_write(0x88,0x33,0xE3); 	// default
    	i2c_write(0x88,0x34,0x0F); 	// H-SYNC Position
    	i2c_write(0x88,0x35,0x01); 	// H-SYNC Anfang
    	i2c_write(0x88,0x36,0x00); 	// H-SYNC Ende
    	i2c_write(0x88,0x37,0x00); 	// Daten "High" aktiv
    	i2c_write(0x88,0x44,0x41); 	// Kompatibel mit ADV7170	
    	i2c_write(0x88,0x45,0xBB);	// default
    	i2c_write(0x88,0xF1,0xEF); 	// default
    	i2c_write(0x88,0xF2,0x80);	// default
    	
	//////////////////////////////////////////////////////////////////////////////////////////////
	*pPPI_CONTROL = PACK_EN | DMA32;  // Packen, 32 Bit Transfer, nur aktives Bild
	*pPPI_FRAME = 576;  // Zeilen pro Halbbild	

	//-----------------------------------------------------------------------------------------------------//		
// Set up DMA0 to receive from PPI
//-----------------------------------------------------------------------------------------------------//
	*pDMA0_PERIPHERAL_MAP = 0x0000;		// lesen vom PPI
	// 32-bit transfers,2D DMA, Interrupt every FRAME/2, Autobuffer mode
	*pDMA0_CONFIG = MEMWR|SIZE_32|DMA2|FLOW_A;
	// Start address of data buffer
	*pDMA0_START_ADDR = pInBuffA;
	// DMA inner loop count
	*pDMA0_X_COUNT = FRAME /16;	// 1440 * 288 * 2 / (4 * 4)

	*pDMA0_X_MODIFY = 4;		// 32 Bit transfer => 4 *  1 Byte
	*pDMA0_Y_COUNT = 4; 		// 2 Buffer
	*pDMA0_Y_MODIFY = 4;		// 32 Bit am Ende des Buffers zuaddieren
	
		
	CreateTestImage((YUV*)(pInBuffA+FRAME),H_SIZE ,V_SIZE , CHANNEL);
//	sort_h();	 
////////////////////////////////////////////////////////////////////////////////////////////// 	 	
/*	*pSPORT0_RFSDIV = 4*32-1; // nach 4*32 SClock Framewechsel 
	*pSPORT1_RFSDIV = 4*32-1;// nach 4*32 SClock Framewechsel 
	*pSPORT0_TFSDIV = *pSPORT0_RFSDIV;
	*pSPORT1_TFSDIV = *pSPORT1_RFSDIV;
*/
	
	*pDMA4_CONFIG	= (*pDMA4_CONFIG & ~DEN);	// DMA anschalten
	*pSPORT0_TCR1	= (*pSPORT1_TCR1 & ~DEN);	// SPORT Port starten
	*pSPORT0_RCR1	= (*pSPORT1_RCR1 & ~DEN);	// SPORT Port starten

	// 16-bit data
	*pSPORT0_RCR1 = RFSR;//|IRCLK|IRFS;
	*pSPORT0_RCR2 = SLEN_8;
	
	// Sport1 transmit configuration
	// External CLK, External Frame sync, MSB first
	// 16-bit data
	*pSPORT0_TCR1 = TFSR;//|ITCLK|ITFS;
	*pSPORT0_TCR2 = SLEN_8;

	// Enable MCM 4 transmit & receive channels
	*pSPORT0_MTCS0 = 0x0000000F<<(4*CHANNEL);	// Channel 1&2 (2x16Bit=32Bit)	
	*pSPORT0_MRCS0 = 0x0000000F<<(4*CHANNEL);		
	
	// Set MCM configuration register and enable MCM mode
	*pSPORT0_MCMC1 = 0x1000;	//WSIZE=8Channels
	*pSPORT0_MCMC2 = 0x001c;	//MC enable, MC-DMA Transmit Packing

	//////////////////////////////////////////////////////////////////////////////////////////////	

#if (1)
	*pDMA4_PERIPHERAL_MAP = 0x2000;			//direction SPORT
	*pDMA4_CURR_DESC_PTR = usDescriptor1;		//Start Descriptorblock
	// 8 Bit transfer, Descriptorarray, 5 rows
	*pDMA4_CONFIG = SIZE_08|0x4500;

	usDescriptor1[0] = (BYTE2)((BYTE4)(pInBuffA+FRAME) & 0xFFFF); //Startaddress byHeader
	usDescriptor1[1] = (BYTE2)((BYTE4)(pInBuffA+FRAME) >> 16);
	usDescriptor1[2] = SIZE_08|DEN|0x4700;			//next 7 rows, Enable
	usDescriptor1[3] = (sizeof(DATA_PACKET)-4);			//x Cout
	usDescriptor1[4] = 1;					//x Modify

	
	usDescriptor1[5] = (BYTE2)((BYTE4)pInBuffA & 0xFFFF); //Startaddress byHeader
	usDescriptor1[6] = (BYTE2)((BYTE4)pInBuffA >> 16);
	usDescriptor1[7] = SIZE_08|DEN|DMA2|IEN;//|0x4500;			//next 7 rows, Enable
	usDescriptor1[8] = 480;			//x Cout
	usDescriptor1[9] = 3;					//x Modify
	usDescriptor1[10] = 136;
	usDescriptor1[11] = 1443;
	
/*	usDescriptor1[12] = (BYTE2)((BYTE4)pInBuffA & 0xFFFF); //Startaddress byHeader
	usDescriptor1[13] = (BYTE2)((BYTE4)pInBuffA >> 16);
	usDescriptor1[14] = SIZE_08|IEN|DEN;			//next 7 rows, Enable
	usDescriptor1[15] = 256-(sizeof(DATA_PACKET)-4);			//x Cout
	usDescriptor1[16] = 1;					//x Modify

	*pDMA4_PERIPHERAL_MAP = 0x4000;			//direction SPORT
	*pDMA4_CURR_DESC_PTR = usDescriptor1;		//Start Descriptorblock
	// 8 Bit transfer, Descriptorarray, 5 rows
	*pDMA4_CONFIG = SIZE_16|0x4500;
    
	usDescriptor1[0] = (BYTE2)((BYTE4)pInBuffA & 0xFFFF); //Startaddress byHeader
	usDescriptor1[1] = (BYTE2)((BYTE4)pInBuffA >> 16);
	usDescriptor1[2] = SIZE_16|DEN|0x4500;			//next 5 rows, Enable
	usDescriptor1[3] = H_SIZE*V_SIZE;			//x Cout
	usDescriptor1[4] = 2;					//x Modify

	
	usDescriptor1[5] = (BYTE2)((BYTE4)pInBuffA & 0xFFFF); //Startaddress byHeader
	usDescriptor1[6] = (BYTE2)((BYTE4)pInBuffA >> 16);
	usDescriptor1[7] = SIZE_16|IEN|DEN;			//next 7 rows, Enable
	usDescriptor1[8] = 128;			//x Cout
	usDescriptor1[9] = 2;					//x Modify	
*/
#else
	*pDMA4_PERIPHERAL_MAP = 0x2000;		
	// 16-bit transfers,1D DMA, Autobuffer mode
	*pDMA4_CONFIG = SIZE_16|FLOW_A;
	// Start address of data buffer
	*pDMA4_START_ADDR = pInBuffA;
	// DMA inner loop count
	*pDMA4_X_COUNT = H_SIZE*V_SIZE+128; //32768;

	*pDMA4_X_MODIFY = 2;		

#endif	
	
	*pSIC_IAR0 = 0xffffffff;
	*pSIC_IAR1 = 0xfff5ffff;
	*pSIC_IAR2 = 0xffffffff;

	// Registriert  ISRs auf Interruptvektoren

    register_handler(ik_ivg12, SPORT_ISR);
	// enable interrupt
	*pSIC_IMASK = 0x00001000;
	
//////////////////////////////////////////////////////////////////////////////////////////////	

	*pDMA0_CONFIG	= (*pDMA0_CONFIG | DEN); 	// Enable PPI DMA
	*pPPI_CONTROL 	= (*pPPI_CONTROL | DEN);	// Enable PPI  Port	
	
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
/*		
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
*/		
	pPacket->bValid				= SwapEndianSmal(TRUE);
	pPacket->dwMarker			= SwapEndian(0x12345678);
	pPacket->wSize				= SwapEndianSmal(sizeof(DATA_PACKET));
	pPacket->eType				= (DataType)SwapEndian(eTypeYUV422);
	pPacket->eSubType			= (DataSubType)SwapEndian(eSubTypeUnvalid);
	pPacket->wSource			= SwapEndianSmal(nChannel);
	pPacket->wSizeH				= SwapEndianSmal(nHSize);
	pPacket->wSizeV				= SwapEndianSmal(nVSize);
	pPacket->wBytesPerPixel		= SwapEndianSmal(2);
	pPacket->wField				= SwapEndianSmal(1);
	pPacket->bVidPresent		= SwapEndianSmal(TRUE);
	pPacket->bMotion			= SwapEndianSmal(FALSE);
	pPacket->Point[0].cx		= (WORD)SwapEndianSmal(10);
	pPacket->Point[0].cy		= (WORD)SwapEndianSmal(15);
	pPacket->Point[0].nValue	= SwapEndianSmal(100);
	pPacket->Point[1].cx		= (WORD)SwapEndianSmal((WORD)-1);
	pPacket->Point[1].cy		= (WORD)SwapEndianSmal((WORD)-1);
	pPacket->Point[1].nValue	= SwapEndianSmal(200);
	pPacket->Point[2].cx		= (WORD)SwapEndianSmal((WORD)-1);
	pPacket->Point[2].cy		= (WORD)SwapEndianSmal((WORD)-1);
	pPacket->Point[2].nValue	= SwapEndianSmal(300);
	pPacket->Point[3].cx		= (WORD)SwapEndianSmal((WORD)-1);
	pPacket->Point[3].cy		= (WORD)SwapEndianSmal((WORD)-1);
	pPacket->Point[3].nValue	= SwapEndianSmal(400);
	pPacket->Point[4].cx		= (WORD)SwapEndianSmal((WORD)-1);
	pPacket->Point[4].cy		= (WORD)SwapEndianSmal((WORD)-1);
	pPacket->Point[4].nValue	= SwapEndianSmal(500);
	pPacket->TimeStamp.bValid 	= SwapEndianSmal(FALSE);
	pPacket->TimeStamp.wYear	= (WORD)SwapEndianSmal(2003);
	pPacket->TimeStamp.wMonth	= SwapEndianSmal(8);
	pPacket->TimeStamp.wDay		= SwapEndianSmal(4);
	pPacket->TimeStamp.wHour	= SwapEndianSmal(13);
	pPacket->TimeStamp.wMinute	= SwapEndianSmal(3);
	pPacket->TimeStamp.wSecond	= SwapEndianSmal(34);
	pPacket->TimeStamp.wMSecond	= SwapEndianSmal(333);
	pPacket->dwFieldID			= SwapEndian(123);
	pPacket->dwProzessID		= SwapEndian(0);
	pPacket->dwImageDataLen		= SwapEndian(nHSize*nVSize*SwapEndianSmal(pPacket->wBytesPerPixel));
}

EX_INTERRUPT_HANDLER(SPORT_ISR)
{
	*pDMA4_IRQ_STATUS = 0x0001;	// Löscht Interrupt
        
    *pDMA4_CURR_DESC_PTR = usDescriptor1;
    *pDMA4_CONFIG = SIZE_08|0x4500|DEN;
 
}

