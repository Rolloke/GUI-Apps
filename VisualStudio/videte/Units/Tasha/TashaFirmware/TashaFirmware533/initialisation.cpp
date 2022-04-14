/********************************************************************/
/* INITIALISIERUNG 													*/
/********************************************************************/
/* Autor: Christian Lohmeier										*/
/* Datum: 08.07.2004												*/
/* Firma: Videte IT	AG												*/
/********************************************************************/

#include "main.h"
#include "i2c.h"

//-----------------------------------------------------------------------------------------------------//
// Funktion: sdram_ini							     												   //
// Beschreibung: Konfiguriert das SDRAM und löscht den Videobuffer	       							   //
//-----------------------------------------------------------------------------------------------------//
void sdram_ini(void)
{
	BYTE *pClear;				// Zeiger zum Löschen der Buffer
	BYTE4 nCount;				// Zähler zum Löschen
	pBuffer = (BUFFER*) FRAMEADDRESS;
	pInBuffA = pBuffer->byInBuff_A;		// Eingangsbuffer A
	pInBuffB = pBuffer->byInBuff_B;		// Eingangsbuffer B 
	*pEBIU_SDGCTL = 0x00000000;
	*pEBIU_SDRRC = 0x0817;		// SDRAM Schnittstelle programmieren: RDIV = 1866 Cycles
	*pEBIU_SDBCTL = 0x0011;		// Bank 0 enabled, 32 MB, address width 9 Bit
	*pEBIU_SDGCTL = 0x04911909; // 16 Bit Datenbus, kein externes Buffering,  = 2 SCLK,
	//  = 3 SCLK,  = 3 SCLK,  = 6 SCLK, prefetch disabled, CAS latency = 3 SCLK, SCLK1 disabled     
	// Buffer löschen 2 Halbbilder 
	pClear = pInBuffA;
	   	for (nCount=0;nCount<(5*FRAME);nCount++)				// Löscht die Halbbildspeicher
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
	return;
}
/////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------------------------------//
// Funktion:	Init_Interrupts						      											   //
// Beschreibung:	Initialise Interrupt PPI, MDMA, SPORT			         						   //
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

	*pSIC_IAR0 =0;// P1_IVG(14) | P2_IVG(14) | P3_IVG(14) | P4_IVG(14);
	*pSIC_IAR1 = P8_IVG(8) | P12_IVG(9) | P13_IVG(13);
	*pSIC_IAR2 = P21_IVG(10) | P22_IVG(11) | P16_IVG(14);// | P19_IVG(14) 

	// Registriert  ISRs auf Interruptvektoren
	register_handler(ik_ivg8, PPI_ISR);
   	register_handler(ik_ivg11, BLOCKA_ISR);
   	register_handler(ik_ivg10, BLOCKB_ISR);
    register_handler(ik_ivg9, SPORT_ISR);
    register_handler(ik_ivg13, SPI_ISR);
    register_handler(ik_ivg14, TIMERERROR_ISR);  // TODU Feheler in der ERROR Behandlung
	// enable interrupt
	*pSIC_IMASK = SIC_MASK(8)|SIC_MASK(12)|SIC_MASK(13)|SIC_MASK(16)|SIC_MASK(21)|SIC_MASK(22);//SIC_MASK(1)|SIC_MASK(2)|SIC_MASK(3)|SIC_MASK(4)|SIC_MASK(8)|SIC_MASK(12)|SIC_MASK(13)|SIC_MASK(16)|SIC_MASK(21)|SIC_MASK(22)|SIC_MASK(19);
	return;
}
/////////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------------------------------------//
// Funktion: adc_ini							         											   //
// Beschreibung: Initialisiert den ADC und DAC mittels I2C			         						   //
//-----------------------------------------------------------------------------------------------------//
void adc_ini(void)						// Philips FRONTEND
{
BYTE subaddress,wr_address; 			// Zählervariable
	wr_address = SLAVE_ADDRESS;
	//Register Hex02
	i2c_write(wr_address,0x01,0x08);
	i2c_write(wr_address,0x02,0xC0);
	//Register Hex03
	i2c_write(wr_address,0x03,0x00);
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
	i2c_write(wr_address,0x09,0x01);  //alt 0x01 Schärfefilter optimiert
//	i2c_write(wr_address,0x09,0x43);  // TEST ML
//	i2c_write(wr_address,0x09,0x03);  // TEST ML
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
	i2c_write(wr_address,0x12,0x44);
	//Register Hex13
	i2c_write(wr_address,0x13,0x00);  //0x01 für Analogen Ausgang
	//Register Hex15
	i2c_write(wr_address,0x15,0x00);
	//Register Hex16
	i2c_write(wr_address,0x16,0x00);
	//Register Hex17
	i2c_write(wr_address,0x17,0x00);
	//Register Hex40
	i2c_write(wr_address,0x40,0x02);

	//Register Hex41-57
	for(subaddress=0x41; subaddress<=0x57;subaddress++)
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
/////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------//
// Function: Timer															//
//																			//
// Beschreibung: Für Interrupt TIMERERROR_ISR 						    	//
//--------------------------------------------------------------------------//
void timer_ini(void)
{
	*pTIMER0_CONFIG		= 0x0019;
	*pTIMER0_PERIOD		= SCLK/TIMER0FREQ;
	*pTIMER0_WIDTH		= 0x00000001;

}
/////////////////////////////////////////////////////////////////////////////

// Memorytransfer für die Bewegungserkennung
void IniMemDMA()
{
	*pMDMA_S0_CONFIG &= ~DEN;	
	*pMDMA_D0_CONFIG &= ~DEN;

	*pMDMA_S0_PERIPHERAL_MAP = 0x0040;					// Memory DMA
	*pMDMA_S0_CURR_DESC_PTR = usMEMDescriptor3;			// Start Descriptorblock
	// 8 Bit transfer, Descriptorarray, 5 rows
	*pMDMA_S0_CONFIG = SIZE_08|0x4500;

	usMEMDescriptor3[0] = (BYTE2)((BYTE4)(pBuffer->byInBuff_A+1) & 0xFFFF); // Startaddress 
	usMEMDescriptor3[1] = (BYTE2)((BYTE4)(pBuffer->byInBuff_A+1) >> 16);
	usMEMDescriptor3[2] = SIZE_08|DEN|0x4500;			// next 7 rows, Enable
	usMEMDescriptor3[3] = (COL/2)*BLOCK;				// x Cout
	usMEMDescriptor3[4] = 4;							// x Modify
	
	usMEMDescriptor3[5] = (BYTE2)((BYTE4)(pBuffer->byInBuff_B+1) & 0xFFFF); // Startaddress 
	usMEMDescriptor3[6] = (BYTE2)((BYTE4)(pBuffer->byInBuff_B+1) >> 16);
	usMEMDescriptor3[7] = SIZE_08|DEN|IEN;				// Interrupt, Enable
	usMEMDescriptor3[8] = (COL/2)*BLOCK;				// x Cout
	usMEMDescriptor3[9] = 4;							// x Modify
    
	*pMDMA_D0_PERIPHERAL_MAP = 0x0040;					// direction SPORT
	*pMDMA_D0_CURR_DESC_PTR = usMEMDescriptor4;			// Start Descriptorblock
	// 8 Bit transfer, Descriptorarray, 5 rows
	*pMDMA_D0_CONFIG = MEMWR|SIZE_08|0x4500;
    
	usMEMDescriptor4[0] = (BYTE2)((BYTE4)(byFrameBuff_A) & 0xFFFF); // Zieladdress 
	usMEMDescriptor4[1] = (BYTE2)((BYTE4)(byFrameBuff_A) >> 16);
	usMEMDescriptor4[2] = MEMWR|SIZE_08|DEN|0x4500;		// next 5 rows, Enable
	usMEMDescriptor4[3] = (COL/2)*BLOCK; 				// x Cout
	usMEMDescriptor4[4] = 1;							// x Modify
		
	usMEMDescriptor4[5] = (BYTE2)((BYTE4)(byFrameBuff_B) & 0xFFFF); // Zieladdress 
	usMEMDescriptor4[6] = (BYTE2)((BYTE4)(byFrameBuff_B) >> 16);
	usMEMDescriptor4[7] = MEMWR|SIZE_08|DEN|IEN;		// Interrupt, Enable
	usMEMDescriptor4[8] = (COL/2)*BLOCK; 				// x Cout
	usMEMDescriptor4[9] = 1;							// x Modify
    
}

/////////////////////////////////////////////////////////////////////////////
