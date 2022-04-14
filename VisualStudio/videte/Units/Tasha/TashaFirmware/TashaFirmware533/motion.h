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

DWORD dwFrameSize[2] = {FRAME,FRAMENTSC};
		
WORD wNormH[2][5]={{0,176,352,704,704},
				   {0,176,352,704,704}};
WORD wNormV[2][5]={{0,144,288,288,576},
				   {0,120,240,240,480}};

/*section ("Videobuffer_A")*/ BYTE byFrameBuff_A[BLOCK*(COL/2)];
/*section ("Videobuffer_B")*/ BYTE byFrameBuff_B[BLOCK*(COL/2)]; 
/*section ("Videobuffer_C")*/ BYTE byFrameBuff_C[((COL/2)/BLOCK)*((ROW/2)/BLOCK)]; 
/*section ("Videobuffer_C")*/ BYTE byMask_A[((COL/2)/BLOCK)*((ROW/2)/BLOCK)]; 
BYTE byMask_B[((COL/2)/BLOCK)*((ROW/2)/BLOCK)];

BYTE *pFrameBuff_A,*pFrameBuff_B,*pFrameBuff_C,*pHeader;
BYTE *pInBuffA,*pInBuffB,*pYUV_In0,*pYUV_Out0;

BYTE byLineC,byThresholdMD,byThresholdMask,byDelayMask,byIncrementMask,byChannel;
BYTE byIC,byMax,byCrossDis,byCounter,byHelper,byRank;
BYTE4 nFrameC,nFrameC1,nFieldC,nSaveFrameC,nChecksum,dwRequestCount;
BYTE2 *pHeader2;
bool bFullA,bFullB,bLed,bSportStart,bRx,bPush,bLongRx;
bool bMod,bMod1, bCapture,bMd,bInSwitch,bStreamReady0,bStreamReady1,bStreamChange0,bStreamChange1;
bool bStreamCode0, bStreamCode1;
BYTE byMod,byMod1,bySetNorm;

ImageRes eResulution[2] = {eImageResUnvalid, eImageResUnvalid};



volatile BYTE2 usMEMDescriptor1[DESCRIPTOR_SIZE];
volatile BYTE2 usMEMDescriptor2[DESCRIPTOR_SIZE];
volatile BYTE2 usMEMDescriptor3[DESCRIPTOR_SIZE];
volatile BYTE2 usMEMDescriptor4[DESCRIPTOR_SIZE];


MASK_STRUCT byMask;

BYTE2 byHeader1[sizeof(DATA_PACKET)/2];

BUFFER* pBuffer;
DATA_PACKET *pPacket;
DATA_PACKET_HELPER *pPacket1;

#endif //__MOTION_DEFINED	
