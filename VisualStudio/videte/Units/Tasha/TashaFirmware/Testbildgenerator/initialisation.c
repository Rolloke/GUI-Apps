/********************************************************************/
/* INITIALISIERUNG 													*/
/********************************************************************/
/* Autor: Christian Lohmeier										*/
/* Datum: 18.06.2003												*/
/* Firma: Videte IT	AG												*/
/********************************************************************/
#include "motion.h"
#include "main.h"

//-----------------------------------------------------------------------------------------------------//
// Funktion: sdram_ini							         //
// Beschreibung: Konfiguriert das SDRAM und löscht den Videobuffer	         //
//-----------------------------------------------------------------------------------------------------//
void sdram_ini(void)
{
	BYTE *pClear;		// Zeiger zum Löschen der Buffer
	BYTE4 nCount;		// Zähler zum Löschen
	pBuffer = (BUFFER*) FRAMEADDRESS;
	pInBuffA = pBuffer->byInBuff_A;		// Eingangsbuffer A
	pInBuffB = pBuffer->byInBuff_B;		// Eingangsbuffer B 
	pYUV_A = pBuffer->byYUV_A;			// YUV 4:2:0 Planar A 

	*pEBIU_AMGCTL = 0x00FE;
	*pEBIU_SDRRC = 0x0817;	// SDRAM Schnittstelle programmieren: RDIV = 1866 Cycles
	*pEBIU_SDBCTL = 0x0003;	// Bank 0 enabled, 32 MB, address width 9 Bit
	*pEBIU_SDGCTL = 0x0091998D;// 16 Bit Datenbus, kein externes Buffering,  = 2 SCLK,
 //  = 3 SCLK,  = 3 SCLK,  = 6 SCLK, prefetch disabled, CAS latency = 3 SCLK, SCLK1 disabled     
	// Buffer löschen 2 Halbbilder 
	pClear = pInBuffA;
   	for (nCount=0;nCount<(3*FRAME);nCount++) // Löscht die Halbbildspeicher
   	{
    		*(pClear++)=0; 	
  	}
  	pClear = byMask_A;
   	for (nCount=0;nCount<(((COL/2)/8)*((ROW/2)/8));nCount++)	// Löscht die adaptive Maske
   	{
    		*(pClear++)=255; 	
   	}     
	pClear = byMask_B;
   	for (nCount=0;nCount<(((COL/2)/8)*((ROW/2)/8));nCount++)	// Löscht die adaptive Maske
   	{
    		*(pClear++)=255; 	
   	}
//   	setled(16);     
	return;
}

//-----------------------------------------------------------------------------------------------------//
// Funktion: ppi_ini			     				         //
// Beschreibung: PPI-Port wird initialisiert: ITU-656 Format,   		         //
//	              aktive Field1, Packen auf 4*8 Bit, 32 Bit DMA		         //
//-----------------------------------------------------------------------------------------------------//
void ppi_ini(void)
{
	*pPPI_CONTROL = PACK_EN | DMA32;  // Packen, 32 Bit Transfer, nur aktives Bild
	*pPPI_FRAME = 625;  // Zeilen pro Vollbild	
	return;
}

//-----------------------------------------------------------------------------------------------------//
// Function: sport_ini				  			         //
// Beschreibung: I2S, 8 Bit 			 			         //
//-----------------------------------------------------------------------------------------------------//
void sport_ini(void)
{ 
		// Sport0 receive configuration
	// External CLK, External Frame sync, MSB first
	// 32-bit data
	*pSPORT0_RCR1 = RFSR;
	*pSPORT0_RCR2 = SLEN_16;
	
	// Sport0 transmit configuration
	// External CLK, External Frame sync, MSB first
	// 32-bit data
	*pSPORT0_TCR1 = TFSR;
	*pSPORT0_TCR2 = SLEN_16;
	
	// Enable MCM 4 transmit & receive channels
	*pSPORT0_MTCS0 = 0x00000003<<(2*CHANNEL);	// Channel 1&2 (2x16Bit=32Bit)	
	*pSPORT0_MRCS0 = 0x00000000;
	
	// Set MCM configuration register and enable MCM mode
	*pSPORT0_MCMC1 = 0x0000;
	*pSPORT0_MCMC2 = 0x101C;
	
	return;
}

//-----------------------------------------------------------------------------------------------------//
// Funktion: dma_ini							         //
// Beschreibung: Setzen der DMA Kanäle 0,2 sowie der MDMA S0,S1,D0,D1	         //
//-----------------------------------------------------------------------------------------------------//
void dma_ini(void)
{
//-----------------------------------------------------------------------------------------------------//		
// Set up DMA0 to receive from PPI
//-----------------------------------------------------------------------------------------------------//
	*pDMA0_PERIPHERAL_MAP = 0x0000;		// lesen vom PPI
	// 32-bit transfers,2D DMA, Interrupt every FRAME/2, Autobuffer mode
	*pDMA0_CONFIG = MEMWR|SIZE_32|DMA2|IEN|I2D|FLOW_A|BUFFC;
	// Start address of data buffer
	*pDMA0_START_ADDR = pInBuffA;
	// DMA inner loop count
	*pDMA0_X_COUNT = FRAME * 2 / 16; 	// 1440 * 288 * 2 / (4 * 4)

	*pDMA0_X_MODIFY = 4;		// 32 Bit transfer => 4 *  1 Byte
	*pDMA0_Y_COUNT = 4; 		// 4 Buffer
	*pDMA0_Y_MODIFY = 4;		// 32 Bit am Ende des Buffers zuaddieren

//-----------------------------------------------------------------------------------------------------//	
// Set up DMA2 to transmit by SPORT
//-----------------------------------------------------------------------------------------------------//		
	*pDMA4_PERIPHERAL_MAP = 0x2000;			//direction SPORT
	*pDMA4_CURR_DESC_PTR = usDescriptor1;		//Start Descriptorblock
	// 8 Bit transfer, Descriptorarray, 5 rows
	*pDMA4_CONFIG = SIZE_16|0x4500;
    
	usDescriptor1[0] = (BYTE2)((BYTE4)(pHeader2) & 0xFFFF); //Startaddress byHeader
	usDescriptor1[1] = (BYTE2)((BYTE4)(pHeader2) >> 16);
	usDescriptor1[2] = SIZE_16|DEN|0x4500;			//next 7 rows, Enable
	usDescriptor1[3] = (sizeof(DATA_PACKET)-4)/2;			//x Cout
	usDescriptor1[4] = 2;					//x Modify
	
	usDescriptor1[5] = (BYTE2)((BYTE4)pInBuffA & 0xFFFF); //Startaddress byHeader
	usDescriptor1[6] = (BYTE2)((BYTE4)pInBuffA >> 16);
	usDescriptor1[7] = SIZE_16|DEN|0x4500;	//	|DMA2	//next 7 rows, Enable
	usDescriptor1[8] = 0x1000/2;			//x Cout
	usDescriptor1[9] = 2;					//x Modify /3
	
	usDescriptor1[10] = (BYTE2)((BYTE4)(pInBuffB+0x1000) & 0xFFFF); //Startaddress byHeader
	usDescriptor1[11] = (BYTE2)((BYTE4)(pInBuffB+0x1000) >> 16);
	usDescriptor1[12] = SIZE_16|DEN|0x4500;	//	|DMA2	//next 7 rows, Enable
	usDescriptor1[13] = 0x1000/2;			//x Cout
	usDescriptor1[14] = 2;					//x Modify /3
	
	usDescriptor1[15] = (BYTE2)((BYTE4)(pInBuffA+0x2000) & 0xFFFF); //Startaddress byHeader
	usDescriptor1[16] = (BYTE2)((BYTE4)(pInBuffA+0x2000) >> 16);
	usDescriptor1[17] = SIZE_16|DEN|0x4500;	//	|DMA2	//next 7 rows, Enable
	usDescriptor1[18] = 0x1000/2;			//x Cout
	usDescriptor1[19] = 2;					//x Modify /3
	
	usDescriptor1[20] = (BYTE2)((BYTE4)(pInBuffB+0x3000) & 0xFFFF); //Startaddress byHeader
	usDescriptor1[21] = (BYTE2)((BYTE4)(pInBuffB+0x3000) >> 16);
	usDescriptor1[22] = SIZE_16|DEN|0x4500;	//	|DMA2	//next 7 rows, Enable
	usDescriptor1[23] = 0x1000/2;			//x Cout
	usDescriptor1[24] = 2;					//x Modify /3
	
	usDescriptor1[25] = (BYTE2)((BYTE4)(pInBuffA+0x4000) & 0xFFFF); //Startaddress byHeader
	usDescriptor1[26] = (BYTE2)((BYTE4)(pInBuffA+0x4000) >> 16);
	usDescriptor1[27] = SIZE_16|DEN|0x4500;	//	|DMA2	//next 7 rows, Enable
	usDescriptor1[28] = 4096/2;			//x Cout
	usDescriptor1[29] = 2;					//x Modify /3
	
	usDescriptor1[30] = (BYTE2)((BYTE4)(pInBuffB+0x5000) & 0xFFFF); //Startaddress byHeader
	usDescriptor1[31] = (BYTE2)((BYTE4)(pInBuffB+0x5000) >> 16);
	usDescriptor1[32] = SIZE_16|DEN|0x4500;	//	|DMA2	//next 7 rows, Enable
	usDescriptor1[33] = 4096/2;			//x Cout
	usDescriptor1[34] = 2;					//x Modify /3
	
	usDescriptor1[35] = (BYTE2)((BYTE4)(pInBuffA+0x6000) & 0xFFFF); //Startaddress byHeader
	usDescriptor1[36] = (BYTE2)((BYTE4)(pInBuffA+0x6000) >> 16);
	usDescriptor1[37] = SIZE_16|DEN|0x4500;	//	|DMA2	//next 7 rows, Enable
	usDescriptor1[38] = 4096/2;			//x Cout
	usDescriptor1[39] = 2;					//x Modify /3
	
	usDescriptor1[40] = (BYTE2)((BYTE4)(pInBuffB+0x7000) & 0xFFFF); //Startaddress byHeader
	usDescriptor1[41] = (BYTE2)((BYTE4)(pInBuffB+0x7000) >> 16);
	usDescriptor1[42] = SIZE_16|DEN|0x4500;	//	|DMA2	//next 7 rows, Enable
	usDescriptor1[43] = 4096/2;			//x Cout
	usDescriptor1[44] = 2;					//x Modify /3
	
	usDescriptor1[45] = (BYTE2)((BYTE4)(pInBuffA+0x8000) & 0xFFFF); //Startaddress byHeader
	usDescriptor1[46] = (BYTE2)((BYTE4)(pInBuffA+0x8000) >> 16);
	usDescriptor1[47] = SIZE_16|DEN|0x4500;	//	|DMA2	//next 7 rows, Enable
	usDescriptor1[48] = 4096/2;			//x Cout
	usDescriptor1[49] = 2;					//x Modify /3
	
	usDescriptor1[50] = (BYTE2)((BYTE4)(pInBuffB+0x9000) & 0xFFFF); //Startaddress byHeader
	usDescriptor1[51] = (BYTE2)((BYTE4)(pInBuffB+0x9000) >> 16);
	usDescriptor1[52] = SIZE_16|DEN|0x4500;	//	|DMA2	//next 7 rows, Enable
	usDescriptor1[53] = 4096/2;			//x Cout
	usDescriptor1[54] = 2;					//x Modify /3
	
	usDescriptor1[55] = (BYTE2)((BYTE4)(pInBuffA+0xa000) & 0xFFFF); //Startaddress byHeader
	usDescriptor1[56] = (BYTE2)((BYTE4)(pInBuffA+0xa000) >> 16);
	usDescriptor1[57] = SIZE_16|DEN|0x4500;	//	|DMA2	//next 7 rows, Enable
	usDescriptor1[58] = 4096/2;			//x Cout
	usDescriptor1[59] = 2;					//x Modify /3
	
	usDescriptor1[60] = (BYTE2)((BYTE4)(pInBuffB+0xb000) & 0xFFFF); //Startaddress byHeader
	usDescriptor1[61] = (BYTE2)((BYTE4)(pInBuffB+0xb000) >> 16);
	usDescriptor1[62] = SIZE_16|DEN|0x4500;	//	|DMA2	//next 7 rows, Enable
	usDescriptor1[63] = 4096/2;			//x Cout
	usDescriptor1[64] = 2;					//x Modify /3
	
	usDescriptor1[65] = (BYTE2)((BYTE4)(pInBuffA+0xc000) & 0xFFFF); //Startaddress byHeader
	usDescriptor1[66] = (BYTE2)((BYTE4)(pInBuffA+0xc000) >> 16);
	usDescriptor1[67] = SIZE_16|DEN|0x4500;	//	|DMA2	//next 7 rows, Enable
	usDescriptor1[68] = 4096/2;			//x Cout
	usDescriptor1[69] = 2;					//x Modify /3
	
	usDescriptor1[70] = (BYTE2)((BYTE4)(pInBuffB+0xd000) & 0xFFFF); //Startaddress byHeader
	usDescriptor1[71] = (BYTE2)((BYTE4)(pInBuffB+0xd000) >> 16);
	usDescriptor1[72] = SIZE_16|DEN|0x4500;	//	|DMA2	//next 7 rows, Enable
	usDescriptor1[73] = 4096/2;			//x Cout
	usDescriptor1[74] = 2;					//x Modify /3|
	
	usDescriptor1[75] = (BYTE2)((BYTE4)(pInBuffA+0xe000) & 0xFFFF); //Startaddress byHeader
	usDescriptor1[76] = (BYTE2)((BYTE4)(pInBuffA+0xe000) >> 16);
	usDescriptor1[77] = SIZE_16|DEN|0x4500;	//	|DMA2	//next 7 rows, Enable
	usDescriptor1[78] = 4096/2;			//x Cout
	usDescriptor1[79] = 2;					//x Modify /3
	
	usDescriptor1[80] = (BYTE2)((BYTE4)(pInBuffB+0xf000) & 0xFFFF); //Startaddress byHeader
	usDescriptor1[81] = (BYTE2)((BYTE4)(pInBuffB+0xf000) >> 16);
	usDescriptor1[82] = SIZE_16|DEN|IEN;	//	|DMA2	//next 7 rows, Enable
	usDescriptor1[83] = 4096/2;			//x Cout
	usDescriptor1[84] = 2;					//x Modify /3|


	
/*	   
	usDescriptor2[0] = (BYTE2)((BYTE4)(pHeader2) & 0xFFFF); //Startaddress byHeader
	usDescriptor2[1] = (BYTE2)((BYTE4)(pHeader2) >> 16);
	usDescriptor2[2] = SIZE_08|DEN|0x4700;			//next 7 rows, Enable
	usDescriptor2[3] = (sizeof(DATA_PACKET)-4);			//x Cout
	usDescriptor2[4] = 1;					//x Modify
	
	usDescriptor2[5] = (BYTE2)((BYTE4)pInBuffB & 0xFFFF); //Startaddress byHeader
	usDescriptor2[6] = (BYTE2)((BYTE4)pInBuffB >> 16);
	usDescriptor2[7] = SIZE_08|DEN|DMA2|IEN|BUFFC;			//next 7 rows, Enable
	usDescriptor2[8] = 480;			//x Cout
	usDescriptor2[9] = 3;					//x Modify
	usDescriptor2[10] = 136;
	usDescriptor2[11] = 1443;
*/	
	pInBuffA += 1;		// Zeiger auf Y - value
	pInBuffB += 1;		// Zeiger auf Y - value
//-----------------------------------------------------------------------------------------------------//	
// Set up DMA S0 and D0 for Videobuffer_A
//-----------------------------------------------------------------------------------------------------//
	*pMDMA_S0_PERIPHERAL_MAP = 0x0040;	
	// 8-bit transfers, Interrupt am Ende, Autobuffer Mode
	*pMDMA_S0_CONFIG = SIZE_08|IEN|FLOW_A;
	// Startadresse des Eingangsbuffers 
	*pMDMA_S0_START_ADDR = pInBuffA;
	// DMA innere Schleife
	*pMDMA_S0_X_COUNT = (COL/2)*BLOCK; 	 // 8 Zeilen
	// Innere Schleife Modifikator
	*pMDMA_S0_X_MODIFY = 4;
		
	*pMDMA_D0_PERIPHERAL_MAP = 0x0040;
	// Configure DMA_D0
	// 8-bit transfers, Interrupt am Ende, Autobuffer mode
	*pMDMA_D0_CONFIG = MEMWR|SIZE_08|IEN|FLOW_A;
	// Startadresse des Eingangsbuffers
	*pMDMA_D0_START_ADDR = byFrameBuff_A;
	// DMA innere Schleife
	*pMDMA_D0_X_COUNT = (COL/2)*BLOCK; 
	// Innere Schleife Modifikator
	*pMDMA_D0_X_MODIFY = 1;

//-----------------------------------------------------------------------------------------------------//		
// Set up DMA S1 and D1 for Videobuffer_B
//-----------------------------------------------------------------------------------------------------//
	*pMDMA_S1_PERIPHERAL_MAP = 0x0040;	
	// 8-bit transfers, Interrupt am Ende, Autobuffer Mode
	*pMDMA_S1_CONFIG = SIZE_08|IEN|FLOW_A;
	// Startadresse des Eingangsbuffers 
	*pMDMA_S1_START_ADDR = pInBuffA;
	// DMA innere Schleife
	*pMDMA_S1_X_COUNT = (COL/2)*BLOCK; 	 // 8 Zeilen
	// Innere Schleife Modifikator
	*pMDMA_S1_X_MODIFY = 4;
		
	*pMDMA_D1_PERIPHERAL_MAP = 0x0040;
	// Configure DMA_D0
	// 8-bit transfers, Interrupt am Ende, Autobuffer mode
	*pMDMA_D1_CONFIG = MEMWR|SIZE_08|IEN|FLOW_A;
	// Startadresse des Eingangsbuffers
	*pMDMA_D1_START_ADDR = byFrameBuff_A;
	// DMA innere Schleife
	*pMDMA_D1_X_COUNT = (COL/2)*BLOCK; 
	// Innere Schleife Modifikator
	*pMDMA_D1_X_MODIFY = 1;
	return;
}

//-----------------------------------------------------------------------------------------------------//
// Funktion:	dma_en							         //
// Beschreibung:	Enable DMA0 and PPI					         //
//-----------------------------------------------------------------------------------------------------//
void dma_en (void)
{
	*pDMA0_CONFIG	= (*pDMA0_CONFIG | DEN); 	// Enable PPI DMA

	*pPPI_CONTROL 	= (*pPPI_CONTROL | DEN);	// Enable PPI  Port	
	return;
}


//-----------------------------------------------------------------------------------------------------//
// Funktion:	Init_Interrupts						         //
// Beschreibung:	Initialise Interrupt PPI, MDMA, SPORT			         //
//-----------------------------------------------------------------------------------------------------//
void isr_ini(void)
{
	*pSIC_IMASK = SIC_UNMASK_ALL;
	// PPI RX	(DMA0) 		interrupt priority auf 2 = IVG9
	// MDMA0 	(DMA_S/D0) 	interrupt priority auf 3 = IVG10
	// MDMA1 	(DMA_S/D1) 	interrupt priority auf 4 = IVG11
	// SPORT TX (DMA4)	 	interrupt priority auf 5 = IVG12 
	// ERROR 	(PPI ERROR)	interrupt priority auf 6 = IVG13
	// SPI 		(DMA5)		interrupt priority auf 7 = IVG14
	// TIMER0 	(TIMER0)	interrupt priority auf 8 = IVG15

	*pSIC_IAR0 = P1_IVG(14) | P2_IVG(14) | P3_IVG(14) | P4_IVG(14);
	*pSIC_IAR1 = P8_IVG(9) | P12_IVG(12) | P13_IVG(13);
	*pSIC_IAR2 = P21_IVG(10) | P22_IVG(11) | P19_IVG(14) | P16_IVG(14);

	// Registriert  ISRs auf Interruptvektoren
	register_handler(ik_ivg9, PPI_ISR);
   	register_handler(ik_ivg10, BLOCKA_ISR);
   	register_handler(ik_ivg11, BLOCKB_ISR);
    register_handler(ik_ivg12, SPORT_ISR);
    register_handler(ik_ivg13, SPI_ISR);
//    register_handler(ik_ivg14, TIMERERROR_ISR);  // TODU Feheler in der ERROR Behandlung
	// enable interrupt
	*pSIC_IMASK = SIC_MASK(1)|SIC_MASK(2)|SIC_MASK(3)|SIC_MASK(4)|SIC_MASK(8)|SIC_MASK(12)|SIC_MASK(13)|SIC_MASK(16)|SIC_MASK(21)|SIC_MASK(22)|SIC_MASK(19);
	return;
}


//-----------------------------------------------------------------------------------------------------//
// Funktion: adc_ini							         //
// Beschreibung: Initialisiert den ADC und DAC mittels I2C			         //
//-----------------------------------------------------------------------------------------------------//
/*
void adc_ini(void)					// AD FRONTEND
{
	BYTE byI; 						// Zählervariable
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
		for (byI=0;byI<=25;byI++)	// Schleife zur Initialisierung des DA-Wandlers (nur zur Kontrolle) 
		
return;
}
*/
void adc_ini(void)						// Philips FRONTEND
{
BYTE subaddress,wr_address; 			// Zählervariable
	wr_address = 0x4A;
	//Register Hex02
	i2c_write(wr_address,0x01,0x08);
	i2c_write(wr_address,0x02,0xC0);
	//Register Hex03
	i2c_write(wr_address,0x03,0x33);
	//Register Hex04
	i2c_write(wr_address,0x04,0x00);
	//Register Hex05
	i2c_write(wr_address,0x05,0x00);
	//Register Hex06
	i2c_write(wr_address,0x06,0xE9);
	//Register Hex07
	i2c_write(wr_address,0x07,0x0D);
	//Register Hex08
	i2c_write(wr_address,0x08,0x98);
	//Register Hex09
	i2c_write(wr_address,0x09,0x01);
	//Register Hex0A
	i2c_write(wr_address,0x0A,0x80);
	//Register Hex0B
	i2c_write(wr_address,0x0B,0x47);
	//Register Hex0C
	i2c_write(wr_address,0x0C,0x40);
	//Register Hex0D
	i2c_write(wr_address,0x0D,0x00);
	//Register Hex0E
	i2c_write(wr_address,0x0E,0x01);
	//Register Hex0F
	i2c_write(wr_address,0x0F,0x2A);
	//Register Hex10
	i2c_write(wr_address,0x10,0x00);
	//Register Hex11
	i2c_write(wr_address,0x11,0x0C);
	//Register Hex12
	i2c_write(wr_address,0x12,0x01);
	//Register Hex13
	i2c_write(wr_address,0x13,0x01);  //0x00
	//Register Hex15
	i2c_write(wr_address,0x15,0x00);
	//Register Hex16
	i2c_write(wr_address,0x16,0x00);
	//Register Hex17
	i2c_write(wr_address,0x17,0x00);
	//Register Hex40
	i2c_write(wr_address,0x40,0x02);

	//Register Hex41-57
	for(subaddress=0x41; subaddress<=57;subaddress++)
	    i2c_write(wr_address,subaddress,0xFF);
	//Register Hex58
	i2c_write(wr_address,0x58,0x00);		
	//Register Hex59
	i2c_write(wr_address,0x59,0x54);
	//Register Hex5A
	i2c_write(wr_address,0x5A,0x07);
	//Register Hex5B
	i2c_write(wr_address,0x5B,0x83);
	//Register Hex5E
	i2c_write(wr_address,0x5E,0x00);
	
}

//------------------------------------------------------------------------------------------------------------------------------//
// Funktion: clock_set()							         		              //
// Beschreibung:	setzt die Core-Spannung auf 1.2V, CCLK 540 MHz, SCLK 135 MHZ			//
//------------------------------------------------------------------------------------------------------------------------------//
void clock_set()
{
	*pPLL_DIV = 0x0004; 	// SCLKvisor: CCLK / 4 = SCLK
	*pVR_CTL = 0x00dB;    	// Kernspannung auf 1.2 V (Full Power Mode)
	return;
}

//--------------------------------------------------------------------------//
// Function: Timer															//
//																			//
// Beschreibung: Setzt den SPORT-Takt auf SCLOCK/2 							//
//--------------------------------------------------------------------------//
void timer_ini(void)
{
	*pTIMER0_CONFIG		= 0x0019;
	*pTIMER0_PERIOD		= SCLK/(2*TIMER0FREQ);
	*pTIMER0_WIDTH		= 0x00000001;

}

