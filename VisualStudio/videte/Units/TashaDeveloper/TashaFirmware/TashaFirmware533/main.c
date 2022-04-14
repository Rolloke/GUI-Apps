/********************************************************************/
/* Main Routine													    */
/********************************************************************/
/* Autor: Christian Lohmeier										*/
/* Datum: 18.08.2003												*/
/* Firma: Videte IT	AG												*/
/********************************************************************/
#include "main.h"

//--------------------------------------------------------------------------//
// Funktion: Main															//
//																			//
// Beschreibung: Initialisiert, SDRAM, I2C, ADC, SPORT,	TIMER, PPI, DMA, ISR//
//				 wartet auf Interrupt										//
//																			//
//--------------------------------------------------------------------------//
BYTE4 nCount,nCount2;

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
    
    CreateImageHeader(pHeader2,H_SIZE ,V_SIZE , CHANNEL);
    
	Ini_Descriptor();
    sdram_ini();		// SDRAM initialisieren 
    clock_set();		// Setzt CCLK auf 540MHZ und volle Spannung
    i2c_ini();     		// I2C Bus initialisieren 
    adc_ini(); 		    // Konfigurieren des AD-Wandlers
   	sport_ini();		// Initialisieren des SPORT
	timer_ini();		// SPORT CLOCK setzen
    ppi_ini();	     	// PPI programmieren
    dma_ini();	     	// DMA konfigurieren
    isr_ini();			// Interrupt Service Routine registrieren
    dma_en();	      	// PPI DMA starten
	nCount = 0;
	SPI_en();
    clearled(0x2F);		       	// LED Anzeige löschen
    *pFIO_DIR &= ~PF8;
    *pFIO_INEN |= PF8;
    *pFIO_MASKA_S = PF8;
    *pFIO_MASKA_D = PF8;
    *pTIMER_ENABLE = 0x0001; 
    DoMessage(TMM_NOTIFY_SLAVE_INIT_READY,0,0,0);
    
	while(1)										// warten auf Interrupt 
	{
        if (bFullA && bFullB) blocktransfer_end();	// Wenn 2*8 Zeilen im L1 dann Transferend     
    }
}

