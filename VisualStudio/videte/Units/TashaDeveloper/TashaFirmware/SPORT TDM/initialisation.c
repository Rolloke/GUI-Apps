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
	
	pInBuffA = 0;		// Eingangsbuffer A
	pInBuffB = 0;		// Eingangsbuffer B 
	pInBuffB += FRAME;

	*pEBIU_SDRRC = 0x0817;	// SDRAM Schnittstelle programmieren: RDIV = 1866 Cycles
	*pEBIU_SDBCTL = 0x0013;	// Bank 0 enabled, 32 MB, address width 9 Bit
	*pEBIU_SDGCTL = 0x0091998D;// 16 Bit Datenbus, kein externes Buffering,  = 2 SCLK,
 //  = 3 SCLK,  = 3 SCLK,  = 6 SCLK, prefetch disabled, CAS latency = 3 SCLK, SCLK1 disabled     
	// Buffer löschen 2 Halbbilder 
	pClear = pInBuffA;
   	for (nCount=0;nCount<(2*FRAME+2*(COL*2));nCount++) // Löscht die Halbbildspeicher
   	{
    		*(pClear++)=0; 	
  	}
  	  pClear = byMask_A;
   	 for (nCount=0;nCount<(((COL/2)/8)*((ROW/2)/8));nCount++)	// Löscht die adaptive Maske
   	 {
    		*(pClear++)=255; 	
   	 }     
}

//-----------------------------------------------------------------------------------------------------//
// Funktion: ppi_ini			     				         //
// Beschreibung: PPI-Port wird initialisiert: ITU-656 Format,   		         //
//	              aktive Field1, Packen auf 4*8 Bit, 32 Bit DMA		         //
//-----------------------------------------------------------------------------------------------------//
void ppi_ini(void)
{
	*pPPI_CONTROL = PACK_EN | DMA32;  // Packen, 32 Bit Transfer, nur aktives Bild
	*pPPI_FRAME = 288;  // Zeilen pro Halbbild	
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
	*pSPORT0_RCR2 = SLEN_32;
	
	// Sport0 transmit configuration
	// External CLK, External Frame sync, MSB first
	// 32-bit data
	*pSPORT0_TCR1 = TFSR;
	*pSPORT0_TCR2 = SLEN_32;
	
	// Enable MCM 4 transmit & receive channels
	*pSPORT0_MTCS0 = 0x00000001;
	*pSPORT0_MRCS0 = 0x00000001;
	
	// Set MCM configuration register and enable MCM mode
	*pSPORT0_MCMC1 = 0x0000;
	*pSPORT0_MCMC2 = 0x0010;
	
	
/*	*pSPORT0_TCR1 = (*pSPORT0_TCR1 & 0xFFFE);	// sichergehen, dass der SPORT aus ist	*pSPORT0_TCR1 = LATFS | ITFS | TFSR;	// Late Frame, interner Frame, Frame erzeugen 	*pSPORT0_TCR2 = TSFSE | 0x7; 		// Stereo Frame, 7+1 = 8 Bit Worte
	*pSPORT0_TCR1 = LATFS | ITFS | TFSR;	// Late Frame, interner Frame, Frame erzeugen 
	*pSPORT0_TCR2 = TSFSE | SLEN_32; 		// Stereo Frame, 7+1 = 8 Bit Worte

*/	
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
	*pDMA0_CONFIG = MEMWR|SIZE_32|DMA2|IEN|I2D|FLOW_A;
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
	*pDMA2_PERIPHERAL_MAP = 0x2000;			//direction SPORT
	*pDMA2_CURR_DESC_PTR = usDescriptor1;		//Start Descriptorblock
	// 8 Bit transfer, Descriptorarray, 5 rows
	*pDMA2_CONFIG = SIZE_32|0x4700;
    
	usDescriptor1[0] = (BYTE2)((BYTE4)pInBuffA & 0xFFFF); //Startaddress byHeader
	usDescriptor1[1] = (BYTE2)((BYTE4)pInBuffA >> 16);
	usDescriptor1[2] = SIZE_32|IEN|DEN|DMA2;			//next 7 rows, Enable
	usDescriptor1[3] = 360;			//x Cout
	usDescriptor1[4] = 4;					//x Modify
	usDescriptor1[5] = 144;
	usDescriptor1[6] = 364;
	
	usDescriptor2[0] = (BYTE2)((BYTE4)pInBuffB & 0xFFFF); //Startaddress byHeader
	usDescriptor2[1] = (BYTE2)((BYTE4)pInBuffB >> 16);
	usDescriptor2[2] = SIZE_32|IEN|DEN|DMA2;			//next 7 rows, Enable
	usDescriptor2[3] = 360;			//x Cout
	usDescriptor2[4] = 4;					//x Modify
	usDescriptor2[5] = 144;
	usDescriptor2[6] = 364;
	/*
	usDescriptor1[0] = (BYTE2)((BYTE4)byHeader & 0xFFFF); //Startaddress byHeader
	usDescriptor1[1] = (BYTE2)((BYTE4)byHeader >> 16);
	usDescriptor1[2] = SIZE_08|0x4700|DEN;			//next 7 rows, Enable
	usDescriptor1[3] = sizeof(byHeader);			//x Cout
	usDescriptor1[4] = 1;					//x Modify

	usDescriptor1[5] = (BYTE2)((BYTE4)pInBuffA & 0xFFFF);//Startaddress InBuffA
	usDescriptor1[6] = (BYTE2)((BYTE4)pInBuffA >> 16);	
	usDescriptor1[7] = SIZE_08|DEN|IEN|DMA2;		//next Stop, Enable, 2D DMA, Interrupt
	usDescriptor1[8] = ((FRAME+4*COL)-sizeof(byHeader))/8;	//x Cout
	usDescriptor1[9] = 1;					//x Modify
	usDescriptor1[10] = 8;					//y Cout
	usDescriptor1[11] = 1;					//y Modify
	
	
	usDescriptor2[0] = (BYTE2)((BYTE4)byHeader & 0xFFFF); //Startaddress byHeader
	usDescriptor2[1] = (BYTE2)((BYTE4)byHeader >> 16);
	usDescriptor2[2] = SIZE_08|0x4700|DEN;			//next 7 rows, Enable
	usDescriptor2[3] = sizeof(byHeader);			//x Cout
	usDescriptor2[4] = 1;					//x Modify

	usDescriptor2[5] = (BYTE2)((BYTE4)pInBuffA & 0xFFFF);//Startaddress InBuffB
	usDescriptor2[6] = (BYTE2)((BYTE4)pInBuffA >> 16);	
	usDescriptor2[7] = SIZE_08|DEN|IEN|DMA2;		//next Stop, Enable, 2D DMA, Interrupt
	usDescriptor2[8] = ((FRAME+4*COL)-sizeof(byHeader))/8;	//x Cout
	usDescriptor2[9] = 1;					//x Modify
	usDescriptor2[10] = 8;					//y Cout
	usDescriptor2[11] = 1;					//y Modify
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
}

//-----------------------------------------------------------------------------------------------------//
// Funktion:	dma_en							         //
// Beschreibung:	Enable DMA0 and PPI					         //
//-----------------------------------------------------------------------------------------------------//
void dma_en (void)
{
	*pDMA0_CONFIG	= (*pDMA0_CONFIG | DEN); 	// Enable PPI DMA

	*pPPI_CONTROL 	= (*pPPI_CONTROL | DEN);	// Enable PPI  Port	
}


//-----------------------------------------------------------------------------------------------------//
// Funktion:	Init_Interrupts						         //
// Beschreibung:	Initialise Interrupt PPI, MDMA, SPORT			         //
//-----------------------------------------------------------------------------------------------------//
void isr_ini(void)
{
	// PPI RX	(DMA0) 	interrupt priority auf 2 = IVG9
	// MDMA1 	(DMA_S/D0) 	interrupt priority auf 3 = IVG10
	// MDMA2 	(DMA_S/D1) 	interrupt priority auf 4 = IVG11
	// SPORT TX 	(DMA2) 	interrupt priority auf 5 = IVG12 
	*pSIC_IAR0 = 0xffffffff;
	*pSIC_IAR1 = 0xfffff5f2;
	*pSIC_IAR2 = 0xf43fffff;

	// Registriert  ISRs auf Interruptvektoren
	register_handler(ik_ivg9, PPI_ISR);
   	register_handler(ik_ivg10, BLOCKA_ISR);
   	register_handler(ik_ivg11, BLOCKB_ISR);
    	register_handler(ik_ivg12, SPORT_ISR);
	// enable interrupt
	*pSIC_IMASK = 0x00600500;
}


//-----------------------------------------------------------------------------------------------------//
// Funktion: Timer							         //
// Beschreibung: Setzt den SPORT-Takt auf SCLK/2 			         //
//-----------------------------------------------------------------------------------------------------//
void timer_ini(void)
{
	*pTIMER0_CONFIG = PWM_OUT | PULSE_HI | PERIOD_CNT;	// Pulsweitenmodulation 
	*pTIMER0_PERIOD = 0x00000080;  	// Periodendauer 2 SCLk
	*pTIMER0_WIDTH =  0x00000040;	// Impulsbreite 1 SCLK
	
	*pTIMER1_CONFIG = PWM_OUT | PULSE_HI | PERIOD_CNT;	// Pulsweitenmodulation 
	*pTIMER1_PERIOD = 0x00004000;  	// Periodendauer 2 SCLk
	*pTIMER1_WIDTH =  0x00000080;	// Impulsbreite 1 SCLK
}

//-----------------------------------------------------------------------------------------------------//
// Funktion: adc_ini							         //
// Beschreibung: Initialisiert den ADC und DAC mittels I2C			         //
//-----------------------------------------------------------------------------------------------------//
void adc_ini(void)
{
BYTE byI; 			// Zählervariable
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
	{
    		i2c_write(0x54,byI,byADV7171[byI]); // aus Motion.h
    		wait();
    	}
}

//------------------------------------------------------------------------------------------------------------------------------//
// Funktion: clock_set()							         		              //
// Beschreibung:	setzt die Core-Spannung auf 1.2V, CCLK 540 MHz, SCLK 135 MHZ			//
//------------------------------------------------------------------------------------------------------------------------------//
void clock_set()
{
	*pPLL_DIV = 0x0004; 	// SCLKvisor: CCLK / 4 = SCLK
	clk();			// CCLK = 540 MHz
	*pVR_CTL = 0x00dB;     // Kernspannung auf 1.2 V (Full Power Mode)
}

