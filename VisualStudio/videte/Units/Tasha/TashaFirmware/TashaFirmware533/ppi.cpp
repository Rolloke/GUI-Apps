/********************************************************************/
/* PPI 												  				*/
/********************************************************************/
/* Autor: Christian Lohmeier										*/
/* Datum: 22.04.2004												*/
/* Firma: Videte IT	AG												*/
/********************************************************************/
#include "main.h"
#include "ppi.h"
#include "codec.h" 
extern bool bCoding;
volatile BYTE2 usPPIDescriptor1[20];

//------------------------------------------------------------------------------//
// PPI_ISR						                                                //
// Beschreibung: Wird zyclisch 2 mal pro Halbbild aufgerufen                    //
//------------------------------------------------------------------------------//
EX_INTERRUPT_HANDLER(PPI_ISR)
{	
		byIC++;	
		if (byIC==1)
		{	
			//if (!bCoding) //ML
				makePlanar((ImageRes) eResulution[0],(ImageRes) eResulution[1],pBuffer->byInBuff_A);
		}
		if (byIC==2)
		{	
			byIC = 0;
		    PPIDisable();	  
			PPI_ini();
			PPIdma_ini();	
			PPIEnable();
			//if (!bCoding) //ML
				makePlanar((ImageRes) eResulution[0],(ImageRes) eResulution[1],pBuffer->byInBuff_B);
			*pFIO_DIR |= PF5;			// LED ausschalten
			*pFIO_FLAG_T = PF5;
		}
	*pDMA0_IRQ_STATUS = 0x0001;			// Löscht Interrupt			
}
/////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------------------------------//
// Funktion: ppi_ini			     				         									       //
// Beschreibung: PPI-Port wird initialisiert: ITU-656 Format,   		         					   //
//	              aktive Field1, Packen auf 2*8 Bit, 16 Bit DMA		         						   //
//-----------------------------------------------------------------------------------------------------//
void PPI_ini()
{
	*pPPI_CONTROL = PACK_EN;//|DMA32;  // Packen, 32 Bit Transfer, nur aktives Bild, Filed 1	
	return;
}
/////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------------------------------//
// Funktion: dma_ini							         											   //
// Beschreibung: Setzen der DMA Kanäle 0,2 sowie der MDMA S0,S1,D0,D1	         					   //
//-----------------------------------------------------------------------------------------------------//
void PPIdma_ini()
{
	DWORD dwFrameS;
	dwFrameS =  dwFrameSize[bySetNorm];
	if (bySetNorm == 1)
	dwFrameS =  351360; 
//-----------------------------------------------------------------------------------------------------//		
// Set up DMA0 to receive from PPI																	   //
//-----------------------------------------------------------------------------------------------------//
	*pDMA0_PERIPHERAL_MAP = 0x0000;		// lesen vom PPI
	*pDMA0_CURR_DESC_PTR = usPPIDescriptor1;
	// 16-bit transfers,2D DMA, Interrupt every FRAME
	*pDMA0_CONFIG = MEMWR|SIZE_16|DMA2|IEN|0x4700; 
	// Start address of data buffer
	
	usPPIDescriptor1[0] = (BYTE2)((BYTE4)(pBuffer->byInBuff_A) & 0xFFFF); // Startaddress 
	usPPIDescriptor1[1] = (BYTE2)((BYTE4)(pBuffer->byInBuff_A) >> 16);
	usPPIDescriptor1[2] = MEMWR|SIZE_16|DMA2|0x4700|IEN|DEN;			  // next 7 rows, Enable
	usPPIDescriptor1[3] = dwFrameS >> 3;		//x Cout
	usPPIDescriptor1[4] = 2;
	usPPIDescriptor1[5] = 4;					//y Cout
	usPPIDescriptor1[6] = 2;
	usPPIDescriptor1[7] = (BYTE2)((BYTE4)(pBuffer->byInBuff_B) & 0xFFFF); // Startaddress 
	usPPIDescriptor1[8] = (BYTE2)((BYTE4)(pBuffer->byInBuff_B) >> 16);
	usPPIDescriptor1[9] = MEMWR|SIZE_16|DMA2|IEN|DEN;					  // Interrupt, Enable
	usPPIDescriptor1[10] = dwFrameS >> 3;		//x Cout
	usPPIDescriptor1[11] = 2;
	usPPIDescriptor1[12] = 4;					//y Cout
	usPPIDescriptor1[13] = 2;				
}
/////////////////////////////////////////////////////////////////////////////


	
//-----------------------------------------------------------------------------------------------------//
// Funktion:	dma_en							       												   //
// Beschreibung:	Enable DMA0 and PPI					         									   //
//-----------------------------------------------------------------------------------------------------//
void PPIEnable()
{
	*pDMA0_CONFIG	|=  DEN; 	// Enable PPI DMA
	*pPPI_CONTROL 	|=  DEN;	// Enable PPI  Port	
}
/////////////////////////////////////////////////////////////////////////////

void PPIDisable()
{
	*pDMA0_CONFIG	&=  ~DEN; 	// Disable PPI DMA
	*pPPI_CONTROL 	&=  ~DEN;	// Disable PPI  Port
}


