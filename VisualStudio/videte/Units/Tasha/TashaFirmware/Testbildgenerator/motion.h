/********************************************************************/
/* Video Header														*/
/********************************************************************/
/* Autor: Christian Lohmeier										*/
/* Datum: 18.06.2003												*/
/* Firma: Videte IT	AG												*/
/********************************************************************/
#ifndef  __MOTION_DEFINED
	#define __MOTION_DEFINED

#include "main.h"


//--------------------------------------------------------------------------//
// Buffer im Speichersgment													//
//--------------------------------------------------------------------------//


BYTE byHeader[10] = {		
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF};
	

/*section ("Videobuffer_A")*/ BYTE byFrameBuff_A[BLOCK*(COL/2)];
/*section ("Videobuffer_B")*/ BYTE byFrameBuff_B[BLOCK*(COL/2)]; 
/*section ("Videobuffer_C")*/ BYTE byFrameBuff_C[((COL/2)/BLOCK)*((ROW/2)/BLOCK)]; 
/*section ("Videobuffer_C")*/ BYTE byMask_A[((COL/2)/BLOCK)*((ROW/2)/BLOCK)]; 
BYTE byMask_B[((COL/2)/BLOCK)*((ROW/2)/BLOCK)];

BYTE *pFrameBuff_A,*pFrameBuff_B,*pFrameBuff_C,*pHeader;
BYTE *pInBuffA,*pInBuffB,*pYUV_A;

BYTE byLineC,byThresholdMD,byThresholdMask,byDelayMask,byIncrementMask,byChannel;
BYTE byIC,byMax,byCrossDis,byCounter,byHelper,byRank;
BYTE4 nFrameC,nSaveFrameC,nChecksum;
BYTE2 *pHeader2;
bool bFullA,bFullB,bLed,bAlarm,bRx,bPush,bLongRx;

volatile BYTE2 usDescriptor1[90];
volatile BYTE2 usDescriptor2[DESCRIPTOR_SIZE];

volatile BYTE2 usMEMDescriptor1[DESCRIPTOR_SIZE];
volatile BYTE2 usMEMDescriptor2[DESCRIPTOR_SIZE];

volatile BYTE2 usSPIDescriptor1[SPI_DESCRIPTOR_SIZE];
volatile BYTE2 usSPIDescriptor2[SPI_DESCRIPTOR_SIZE];
volatile BYTE2 usSPIDescriptor3[SPI_DESCRIPTOR_SIZE];

BUFFER* pBuffer;
DATA_PACKET* pPacket;
SPIPacketStruct *pSPIReceivePacket, *pSPITransmitPacket;
MASK_STRUCT *pSPIReceiveMask;
#endif //__MOTION_DEFINED	
