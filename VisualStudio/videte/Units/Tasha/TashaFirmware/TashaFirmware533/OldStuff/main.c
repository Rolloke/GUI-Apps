/********************************************************************/
/* Main Routine													    */
/********************************************************************/
/* Autor: Christian Lohmeier										*/
/* Datum: 18.08.2003												*/
/* Firma: Videte IT	AG												*/
/********************************************************************/
#include <math.h>
#include "main.h"

#include <cplb.h>
#include <cplbtab.h>

// Cache Attributes according to use:
#define ALL_ACCESS	CPLB_SUPV_WR | CPLB_USER_RD | CPLB_USER_WR
#define I_PAGE_MGMT	CPLB_LOCK | CPLB_WT | CPLB_VALID
#define D_PAGE_MGMT	CPLB_LOCK | CPLB_SUPV_WR | CPLB_WT | CPLB_VALID
#define D_HEAP		CPLB_L1_CHBL | ALL_ACCESS | CPLB_WT | CPLB_VALID
#define D_STACK		CPLB_L1_CHBL | ALL_ACCESS | CPLB_WT | CPLB_VALID
#define D_ARGV		CPLB_L1_CHBL | ALL_ACCESS | CPLB_WT | CPLB_VALID
#define D_PROGRAM	CPLB_L1_CHBL | ALL_ACCESS | CPLB_WT | CPLB_VALID
#define D_PCI		CPLB_SUPV_WR | CPLB_WT | CPLB_VALID
#define DGENERIC	CPLB_L1_CHBL | CPLB_WT | ALL_ACCESS | CPLB_VALID

//--------------------------------------------------------------------------//
// Funktion: Main															//
//																			//
// Beschreibung: Initialisiert, SDRAM, I2C, ADC, SPORT,	TIMER, PPI, DMA, ISR//
//				 wartet auf Interrupt										//
//																			//
//--------------------------------------------------------------------------//
#include "export_hdr\exp_SP_MPEG4_Enc.h"
BYTE4 i;
bool bSuccess = false;
tMPEG4VideoEncParam lVideoEncParam;
tBaseVideoEncoder*   lBaseVideoEnc; 

uint32  lNumBytes;
uint32  lVOPType, lOutputBufferSize;
uint32 lNumPkts,*lPacketSizes; 
void main()
{
	
	*pFIO_DIR |= PF5|PF1;			// LED ausschalten
	*pFIO_FLAG_S = PF5;
   	*pFIO_FLAG_T = PF1;
		 
	bPush = false; 				// Löschen der Variablen
	byIC 			= 0;	
 	byMax 			= 0;
 	byCrossDis 		= 0;
 	byThresholdMD 	= THRESHOLD;
 	byThresholdMask = THRESHOLD1;
 	byIncrementMask = MOD;
 	byDelayMask 	= MASKDELAY;
 	byCounter		= 0;
 	nFrameC = 0;
 	nFCSave = 0;
 	byLineC = 0;
	bFullA = false;
	bFullB = false;
    bLed = false;
    bHBild = false;
    bSTrans = false;	
    bAlarm = true;
    bRx = true;
    bLongRx = false;
    bMd = false;
    byRank = 0xFF;
    byChannel = CHANNEL;
   	bCapture = false;
    
	pHeader 	 = byHeader;		// Pointer auf Header
	pHeader2 	 = byHeader1;
    pFrameBuff_A = byFrameBuff_A;  	// Pointer auf Buffer zeigen lassen
    pFrameBuff_B = byFrameBuff_B;  	// Pointer auf Buffer zeigen lassen 		
    pFrameBuff_C = byFrameBuff_C;  	// Pointer auf Buffer zeigen lassen
    
/*    pHeader2           = (BYTE2*) malloc(sizeof(DATA_PACKET));
    pSPIReceivePacket  = (SPIPacketStruct*) malloc(sizeof(SPIPacketStruct));
    pSPITransmitPacket = (SPIPacketStruct*) malloc(sizeof(SPIPacketStruct));
  */  
  	pSPIReceivePacket  	= &bySPI_A;
    pSPITransmitPacket 	= &bySPI_B;
    pSPIReceiveMask		= &byMask;
    
   CreateImageHeader(pHeader2,H_SIZE ,V_SIZE , SLAVE);	// Data Packet Struktur ausfüllen
        

    sdram_ini();		// SDRAM initialisieren   	
    i2c_ini();     		// I2C Bus initialisieren 
	adc_ini(); 		    // Konfigurieren des AD-Wandlers
	sport_ini();		// Initialisieren des SPORT
	timer_ini();		// SPORT CLOCK setzen
    ppi_ini();	     	// PPI programmieren
    dma_ini();	     	// DMA konfigurieren
    isr_ini();			// Interrupt Service Routine registrieren
	
    Ini_Descriptor();	// Kommunikations DMA vorbereiten (SPI)
//	*pTIMER_ENABLE = 0x0001; 
    DoMessage(TMM_NOTIFY_SLAVE_INIT_READY,0,0,0); 	// Booten unt Ini erledigt -> Host
    SPI_en();			// SPI einschalten
    switch (codec())	// Mpeg4 Encoder istantiesiern 
	{
		case E_SUCCESS:	
   			DoTrace("Codec Instance: Success!\n");	
			break;
		case E_OUT_OF_MEMORY:
   			DoTrace("Codec Instance: Out of Memory!\n");
			break;
		case E_INVALID_PARAMS:
   			DoTrace("Codec Instance: Invalid Parameter!\n");
			break;
		case E_INVALID_SIZE:
   			DoTrace("Codec Instance: Invalid Size!\n");
			break;
	}
	
	gEzdMP4SPEnc_GetParam(lBaseVideoEnc, GET_MAX_PACKETS, &lNumPkts);	// Anzahl der Pakete ermitten
	lPacketSizes = (uint32 *) malloc(lNumPkts * sizeof(int32));			// Buffer reserviern
	InitCache();
    dma_en();	      	// PPI DMA starten

	while(1)										// warten auf Interrupt 
	{		 
		*pFIO_INEN = PF10;
		pPacket->bVidPresent = TRUE; 
		if(!(*pFIO_FLAG_D & PF10))
		{
				pPacket->bVidPresent = FALSE;
				bMd= false;
		}
		if(bFullB)
		{	
			IniMemDMA();
			blocktransfer_end();	// Wenn 2*8 Zeilen im L1 dann Transferend 	
		}
		if(!bCapture && !bMd)
		{
			bCapture = true;
			bMd = true;
			IniMemDMA();
			blocktransfer_start();	// 8 Zeilen des Frames und des Letzten in L1 verschieben
			DoCoding();
		  		
			*pDMA4_CONFIG &= ~DEN;
			*pSPORT0_TCR1 &= ~DEN;	// SPORT Port ausschalten
			*pSPORT0_RCR1 &= ~DEN;
		
			*pDMA4_CURR_DESC_PTR = usDescriptor1;
			*pDMA4_CONFIG = SIZE_16|0x4500;
/*				{
				sprintf(pPacket->Debug.sText, "Slave: %lu Frame ID %lu -> EncodeNo: %lu \n",pPacket->wSource, nFrameC, nFCSave);
				DoTrace(pPacket->Debug.sText);
			}			
*/				usDescriptor1[8] = (lNumBytes+byMod+8)>>1;
			if (nFrameC > nSaveFrameC)
			{	
				pPacket->PermanentMask.bHasChanged = FALSE; 
				nSaveFrameC = 0;
			}
			*pDMA4_CONFIG |= DEN;
			*pSPORT0_TCR1 |= DEN;	// SPORT Port starten
			*pSPORT0_RCR1 |= DEN;
			
//			bCapture = false;
		
		}
			

    }	 
}

//--------------------------------------------------------------------------//
// Function: codec	Instantisiert den Encoder gibt Handel zurück //
//--------------------------------------------------------------------------//
int32 codec()
{	
	asm volatile( ".align 4;" );
	
	lVideoEncParam.VOPWidth = WIDTH;		// Bildweite
	lVideoEncParam.VOPHeight = HEIGHT;		// Bildhöhe
	lVideoEncParam.BitRate = 4096000;		// Datenrate
	lVideoEncParam.VOPRate = 25;			// Bildrate
	lVideoEncParam.Level = 5;				// Level 	
	lVideoEncParam.DeInterlace = 0;			// DeInterlace Aus
	lVideoEncParam.RCModel = VENC_UVBR;		// Ungezwungen variable Bitrate
	lVideoEncParam.SVHMode = 0;				// short Video Header
	lVideoEncParam.PacketMode = 0;			// Packet Mode
	lVideoEncParam.PacketSize = 8192;		// Packet Größe
	lVideoEncParam.AIR = 0;					// Adaptiv Intra Refresh	
	lVideoEncParam.GOVLength = 5;			// I-Frame Rate
	lVideoEncParam.SearchRange = 1;		// Suchweite der Motionestamtion
	lVideoEncParam.EncoderPerformance = 1;	// Komplexität
	lVideoEncParam.TimerResolution = 25;	// TimeStamp Auflösung >= VOPRate
	
	return (gEzdMP4SPEnc_Create(&lBaseVideoEnc,&lVideoEncParam));	// Encoder generieren
}

//--------------------------------------------------------------------------//
// Function: Init_Data_Cache //
//--------------------------------------------------------------------------//
void InitCache()
{

	// disable DCPLBs
	*pDMEM_CONTROL = 0x00000001;
	csync();

	// configure DCPLBs
	*pDCPLB_ADDR0 = (void *)0xff800000;			// Data Bank A SRAM
	*pDCPLB_DATA0 = PAGE_SIZE_1MB | CPLB_WT | CPLB_VALID | CPLB_SUPV_WR | CPLB_LOCK ;
	*pDCPLB_ADDR1 = (void *)0xff900000;			// Data Bank B SRAM
	*pDCPLB_DATA1 = PAGE_SIZE_1MB | CPLB_WT | CPLB_VALID | CPLB_SUPV_WR | CPLB_LOCK ;
	*pDCPLB_ADDR2 = (void *)0x00000000;			// SDRAM 0MB - 4MB
	*pDCPLB_DATA2 = PAGE_SIZE_4MB | CPLB_WT | CPLB_VALID | CPLB_SUPV_WR | CPLB_LOCK | CPLB_USER_RD | CPLB_USER_WR | CPLB_L1_CHBL;
	*pDCPLB_ADDR3 = (void *)0x00400000;			// SDRAM 0MB - 4MB
	*pDCPLB_DATA3 = PAGE_SIZE_4MB | CPLB_WT | CPLB_VALID | CPLB_SUPV_WR | CPLB_LOCK | CPLB_USER_RD | CPLB_USER_WR | CPLB_L1_CHBL;
	// enable DCPLBs
	*pDMEM_CONTROL = 0x0000100b;
	csync();	

/*

	dcplbs_table[0].addr 	= 0xFF800000;
	dcplbs_table[0].flags 	= (PAGE_SIZE_1MB | D_PAGE_MGMT | CPLB_L1SRAM);	// L1 Data A
	dcplbs_table[1].addr 	= 0xFF900000;
	dcplbs_table[1].flags 	= (PAGE_SIZE_1MB | D_PAGE_MGMT | CPLB_L1SRAM);	// L1 Data B
	dcplbs_table[2].addr 	= 0x200000;
	dcplbs_table[2].flags 	= (PAGE_SIZE_1MB | DGENERIC);					// SDRAM Block 16MB,  Page 4MB
	dcplbs_table[3].addr 	= 0x300000;
	dcplbs_table[3].flags 	= (PAGE_SIZE_1MB | DGENERIC);					// SDRAM Block 16MB,  Page 4MB
	dcplbs_table[4].addr 	= 0x400000;
	dcplbs_table[4].flags 	= (PAGE_SIZE_1MB | DGENERIC);					// SDRAM Block 16MB,  Page 4MB
	dcplbs_table[5].addr 	= 0x500000;
	dcplbs_table[5].flags 	= (PAGE_SIZE_1MB | DGENERIC);					// SDRAM Block 16MB,  Page 4MB
	dcplbs_table[6].addr 	= 0x600000;
	dcplbs_table[6].flags 	= (PAGE_SIZE_1MB | DGENERIC);					// SDRAM Block 16MB,  Page 4MB

	cplb_init(CPLB_ENABLE_DCACHE|CPLB_ENABLE_CPLBS);
*/
}

bool MemTest(DWORD dwStart, DWORD dwEnd)
{
	DWORD* pData = NULL;
	bool  bErr  = false;
	DWORD dwI;
	dwStart = dwStart 	>> 2;
	dwEnd 	= dwEnd 	>> 2;
	for (dwI = dwStart; dwI < dwEnd; dwI++)
		pData[dwI] = dwI;

	for (dwI = dwStart; (dwI < dwEnd); dwI++)
	{
		if (pData[dwI] != dwI)
			bErr = true;
	}
	
	return !bErr;		
}	
