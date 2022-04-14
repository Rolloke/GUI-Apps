/********************************************************************/
/* Main Header (Definitionen)										*/
/********************************************************************/
/* Autor: Christian Lohmeier										*/
/* Datum: 18.06.2003												*/
/* Firma: Videte IT	AG												*/
/********************************************************************/
#ifndef  __MAIN_DEFINED
	#define __MAIN_DEFINED

//--------------------------------------------------------------------------//
// Header files																//
//--------------------------------------------------------------------------//
#include <sys\exception.h>
#include <cdefBF533.h>
#include <stdlib.h>
#include <string.h> 
#include <stdio.h> 
#include "definitionen.h"
//#include "..\Include\TashaTimer.h"
#include "..\Include\helper.h"
#include "..\Include\TMMessages.h"
#include "..\..\TashaUnit\TashaUnitDefs.h"

// #define pDMA_TC_PER ((volatile unsigned short *)0xFFC00B0C)
//--------------------------------------------------------------------------//
// Structures																//
//--------------------------------------------------------------------------//
// Bufferhandling
asm volatile( ".align 4;" );
typedef struct  
{
	BYTE byInBuff_A[FRAME];
	BYTE byInBuff_B[FRAME];
	BYTE byYUV_In0[FRAME];
	BYTE byYUV_In1[FRAME];
	BYTE byYUV_Out0[FRAME/2];
	BYTE byYUV_Out1[FRAME/2];
}BUFFER;

typedef struct _DATA_PACKET_HELPER
{
	DWORD			dwDummy;			// Die ersten Bytes der SPORT-Übertragung enthalten oft Bitfehler, daher die Dummyvariable
	DWORD			dwStartMarker;		// Magic Marker1 (Startmarkierung des Headers)
	DWORD			dwCheckSum;			// Prüfsumme über die Bilddaten.
	DWORD			dwProzessTime;		// Gibt die Ausführungszeit des BF533 in MikroSeconds an
	BOOL			bValid;				// TRUE: Bilddaten können abgeholt werden
	WORD			wSize;				// Größe der Struktur ('sizeof(DATA_PACKET)')
	DataType		eType;				// Gibt den Typ des Bildes an (Jpeg, Mpeg4, YUV422,...)	
	DataSubType		eSubType;			// Beschreibt den Bildtyp näher (I-Frame, P-Frame,....)
	WORD			wSource;			// Kameranummer
	WORD			wSizeH;				// Breite des Bildes
	WORD			wSizeV;				// Höhe des Bildes
	WORD			wBytesPerPixel;		// Bytes pro Pixel: (z.B. 2Bytes bei YUV422)
	WORD			wField;				// 0->Even Field 1->Odd Field
	BOOL			bVidPresent;		// TRUE->Video detektiert/FALSE->Video nicht detektiert.
	DWORD			dwFieldID;			// FieldCounter
	DWORD			dwProzessID;		// ProzessID für den Server
	DWORD			dwStreamID;			// Streamnummer für BF533 Video requests 
	DWORD			dwImageDataLen;		// Länge der eigentlichen Bilddaten
}DATA_PACKET_HELPER;


//--------------------------------------------------------------------------//
// Symbolic constants														//
//--------------------------------------------------------------------------//
#define pVR_CTL ((volatile unsigned short *)VR_CTL)

//--------------------------------------------------------------------------//
// Globale Variablen														//
//--------------------------------------------------------------------------//
extern BYTE byFrameBuff_A[];
extern BYTE byFrameBuff_B[];
extern BYTE byFrameBuff_C[];
extern BYTE byHeader[];
extern BYTE byMask_A[];
extern BYTE byMask_B[];

extern bool bFullA,bFullB,bLed,bSportStart,bRx,bPush,bLongRx;
extern BYTE byBlockC,byLineC,byIC,byMax,byCrossDis,byCounter,byHelper,byRank;
extern BYTE byThresholdMD,byThresholdMask,byDelayMask,byIncrementMask,byChannel;
extern BYTE4 nFrameC,nFrameC1,nFieldC,nSaveFrameC,nChecksum,dwRequestCount;
extern BYTE2 *pHeader2;
extern BYTE *pFrameBuff_A,*pFrameBuff_B,*pFrameBuff_C,*pHeader;
extern BYTE *pInBuffA,*pInBuffB,*pYUV_In,*pYUV_Out;

extern bool bMod,bMod1,bCapture,bMd,bInSwitch,bStreamReady0,bStreamReady1,bStreamChange0,bStreamChange1;
extern bool bStreamCode0, bStreamCode1;
extern BYTE byMod,byMod1,bySetNorm;

extern BUFFER* pBuffer;
extern DATA_PACKET *pPacket;
extern DATA_PACKET_HELPER *pPacket1;

extern ImageRes eResulution[];
extern BYTE2 byHeader1[];

extern MASK_STRUCT byMask;

extern volatile BYTE2 usPPIDescriptor1[];
extern volatile BYTE2 usMEMDescriptor1[];
extern volatile BYTE2 usMEMDescriptor2[];
extern volatile BYTE2 usMEMDescriptor3[];
extern volatile BYTE2 usMEMDescriptor4[];

extern DWORD dwFrameSize[];
extern WORD wNormH[2][5];
extern WORD wNormV[2][5];

//--------------------------------------------------------------------------//
// Prototypes																//
//--------------------------------------------------------------------------//


// in file initialisation.c
void sdram_ini();
void isr_ini();
void timer_ini();
void adc_ini();
void IniMemDMA();


// detection.asm
extern "C" void framec();
extern "C" DWORD docheck(DWORD pIn, DWORD nLenght);
extern "C" void adaptiv_mask();
extern "C" void diff(BYTE *pFrameBuff_C);
extern "C" void max_search(BYTE cOffset);
//extern "C" void makeit(BYTE* pY, BYTE* pCb, BYTE* pCr);

// submain.c
void blocktransfer_end();
void blocktransfer_start();

void InitCache();
void CacheDisable();
void CacheEnable();
void copy(BYTE2 *pDest, BYTE2 *pSource, BYTE2 nLength);
void copyMask();
void PermanentMaskFlagReset();

bool WaitMicro(BYTE4 nMicroSeconds);


// in file ISR.c
EX_INTERRUPT_HANDLER(PPI_ISR);
EX_INTERRUPT_HANDLER(BLOCKA_ISR);
EX_INTERRUPT_HANDLER(BLOCKB_ISR);
EX_INTERRUPT_HANDLER(SPORT_ISR);
EX_INTERRUPT_HANDLER(SPI_ISR);
EX_INTERRUPT_HANDLER(ERROR_ISR);
EX_INTERRUPT_HANDLER(TIMERERROR_ISR);

#endif //__MAIN_DEFINED
