/********************************************************************/
/* Main Routine													    */
/********************************************************************/
/* Autor: Christian Lohmeier										*/
/* Datum: 16.07.2004												*/
/* Firma: Videte IT	AG												*/
/********************************************************************/
#include "main.h"
#include "spi.h"
#include "i2c.h"
#include "ppi.h"
#include "sport.h"
#include "codec.h"

#include <cplb.h>
#include <cplbtab.h>

//--------------------------------------------------------------------------//
// Funktion: Main															//
//																			//
// Beschreibung: Initialisiert, SDRAM, I2C, ADC, SPORT,	TIMER, PPI, DMA, ISR//
//				 wartet auf Interrupt										//
//																			//
//--------------------------------------------------------------------------//
DATA_PACKET_HELPER DataPacket2,  *pHeader3;	
BYTE4 i;
bool bVideoAktive = FALSE;
extern tMPEG4VideoEncParam lVideoEncParam0,lVideoEncParam1;
bool bSuccess = false;

/////////////////////////////////////////////////////////////////////////////
int main(void)
{	
	BYTE byPresent = 0;
	*pFIO_DIR |= PF5|PF1;			// LED ausschalten
	*pFIO_FLAG_S = PF5;
   	*pFIO_FLAG_T = PF1;
	
   	startc();	 
	bPush = false; 				// Löschen der Variablen
	byIC 			= 0;	
 	byMax 			= 0;
 	byCrossDis 		= 0;
 	byThresholdMD 	= THRESHOLD;
 	byThresholdMask = THRESHOLD1;
 	byIncrementMask = MOD;
 	byDelayMask 	= MASKDELAY;
 	byCounter		= 0;
 	nFieldC = 0;
 	nFrameC = 0;
 	nFrameC = 0;
 	byLineC = 0;
	bFullA = false;
	bFullB = false;
    bLed = false;	
    bSportStart = false;
    bRx = true;
    bLongRx = false;
    bMd = false;
    byRank = 0xFF;
   	bCapture = false;
   	bStreamReady0 = false;
   	bStreamReady1 = false;
   	bStreamCode0 = true;
   	bStreamCode1 = true;
   	bInSwitch = true;
   	bySetNorm = 0;	// PAL
   	dwRequestCount = 0;
   	
   	
   	*pDMA_TC_PER = 0x8777;			// DMA Preoritäten vergeben 
   	pHeader3	 = &DataPacket2;
 	pHeader2	 = byHeader1;
	pHeader 	 = byHeader;		// Pointer auf Header
    pFrameBuff_A = byFrameBuff_A;  	// Pointer auf Buffer zeigen lassen
    pFrameBuff_B = byFrameBuff_B;  	// Pointer auf Buffer zeigen lassen 		
    pFrameBuff_C = byFrameBuff_C;  	// Pointer auf Buffer zeigen lassen
    CreateImageHeader(pHeader2,(int) 240,(int) 704, SLAVE, false);// Data Packet Struktur ausfüllen     
	CreateImageHeader((BYTE2*)pHeader3,(int) 240,(int) 704, SLAVE, true);// Data Packet Struktur ausfüllen     
    sport_ini();		// Initialisieren des SPORT	
    sdram_ini();		// SDRAM initialisieren 
    InitCache(); 
    i2c_ini();     		// I2C Bus initialisieren 
	adc_ini(); 		    // Konfigurieren des AD-Wandlers

	timer_ini();		// Kamera ausfall Testintervall setzen
	*pTIMER_ENABLE = 0x0001; // Und starten 
    PPI_ini();	     	// PPI programmieren
    PPIdma_ini();	    // DMA konfigurieren
    isr_ini();			// Interrupt Service Routine registrieren
	PPIEnable();	    // PPI DMA starten
	Ini_Descriptor();	// Kommunikations DMA vorbereiten (SPI)
	SPI_en();			// SPI einschalten
	
	byRank = 0xFF;
    DoMessage(TMM_NOTIFY_SLAVE_INIT_READY,0,0,0); 	// Booten unt Ini erledigt -> Host			
/////////////////////////////////////////////////////////////////////////////
  
    while(1)										// warten auf Interrupt 
	{	
 
		if(!bMd)
		{	
			bMd = true;
			if (bVideoAktive)
				DoCoding();
#if (1) //ML
IniMemDMA();
blocktransfer_start();	// 8 Zeilen des Frames und des Letzten in L1 verschieben
#endif		
			if(bSportStart)
			{
				copyMask();
				SetSport();
				bInSwitch = false;
				SportEnable();
			}
			
			bSportStart = false;		
		}
		if(bStreamChange0)					// wenn neue Parameter Codec neu Ini
		{
			if (bStreamReady0)
			{
				DeleteCodec(1);
				bMod = false;
			}		
			bStreamChange0=false;
			bStreamReady0 = false;
			if(lVideoEncParam0.BitRate > 0)
			{
				IniCodec(1);	
				IniCodecBuffer(1);
				bStreamReady0 = true;
				CreateImageHeader(pHeader2,(int) lVideoEncParam0.VOPWidth,(int) lVideoEncParam0.VOPHeight, SLAVE, false);// Data Packet Struktur ausfüllen     
			}
		}
		if(bStreamChange1)					// wenn neue Parameter Codec neu Ini
		{
			if (bStreamReady1)
			{
				DeleteCodec(2);
				bMod1 = false;
			}		
			bStreamChange1=false;
			bStreamReady1 = false;
			if(lVideoEncParam1.BitRate > 0)
			{
				IniCodec(2);
				IniCodecBuffer(2);
				bStreamReady1 = true;
				CreateImageHeader((BYTE2*)pHeader3,(int) lVideoEncParam1.VOPWidth ,(int) lVideoEncParam1.VOPHeight, SLAVE, true);// Data Packet Struktur ausfüllen     
			}
		}

	}
}
/////////////////////////////////////////////////////////////////////////////
	
//--------------------------------------------------------------------------//
// Function: Init_Data_Cache //
//--------------------------------------------------------------------------//
void InitCache()
{
	// disable DCPLBs
	*pDMEM_CONTROL = 0x00000001;
	asm volatile("csync;");

	// configure DCPLBs
	*pDCPLB_ADDR0 = (void *)0xff800000;			// Data Bank A SRAM
	*pDCPLB_DATA0 = PAGE_SIZE_1MB | CPLB_WT | CPLB_VALID | CPLB_SUPV_WR | CPLB_LOCK ;
	*pDCPLB_ADDR1 = (void *)0xff900000;			// Data Bank B SRAM
	*pDCPLB_DATA1 = PAGE_SIZE_1MB | CPLB_WT | CPLB_VALID | CPLB_SUPV_WR | CPLB_LOCK ;
	*pDCPLB_ADDR2 = (void *)0x00000000;			// SDRAM 0MB - 4MB
	*pDCPLB_DATA2 = PAGE_SIZE_4MB | CPLB_WT | CPLB_VALID | CPLB_SUPV_WR | CPLB_LOCK | CPLB_USER_RD | CPLB_USER_WR | CPLB_L1_CHBL;
	*pDCPLB_ADDR3 = (void *)0x00400000;			// SDRAM 4MB - 8MB
	*pDCPLB_DATA3 = PAGE_SIZE_4MB | CPLB_WT | CPLB_VALID | CPLB_SUPV_WR | CPLB_LOCK | CPLB_USER_RD | CPLB_USER_WR | CPLB_L1_CHBL;
	// enable DCPLBs
	*pDMEM_CONTROL = 0x0000100b;
	asm volatile("csync;");	
}
/////////////////////////////////////////////////////////////////////////////

void CacheEnable()
{
	*pDMEM_CONTROL = 0x0000100b;
}
/////////////////////////////////////////////////////////////////////////////

void CacheDisable()
{
	*pDMEM_CONTROL = 0x00000001;
}
/////////////////////////////////////////////////////////////////////////////
	
bool WaitMicro(BYTE4 nMicroSeconds)		// Warteschleife (0us.. 7,9s)
{
	if (nMicroSeconds > 7900000)
    	return(false); 
	DWORD dwLo = getCyclesLo();
	DWORD dwHi = getCyclesHi();
	DWORD dwExpCycle = ((CORE_CLK/1000000)*nMicroSeconds);
	
	if((0xFFFFFFFF - dwLo) < nMicroSeconds)
	{
		dwHi++;
	}
	dwLo += dwExpCycle;
    while((getCyclesLo() <= dwLo) && (getCyclesHi() <= dwHi))
    	asm("nop;");
    return(true);
}
/////////////////////////////////////////////////////////////////////////////
