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
#include "definitionen.h"
#include "..\Include\helper.h"
#include "..\Include\TMMessages.h"
#include "..\..\TashaUnit\TashaUnitDefs.h"


//--------------------------------------------------------------------------//
// Structures																//
//--------------------------------------------------------------------------//
// Bufferhandling
typedef struct  
{
	BYTE byInBuff_A[FRAME];
	BYTE byInBuff_B[FRAME];
}BUFFER;

//--------------------------------------------------------------------------//
// Symbolic constants														//
//--------------------------------------------------------------------------//
// addresses for Port B in Flash A
#define pFlashA_PortA_Dir	(volatile unsigned char *)0x20270006
#define pFlashA_PortA_Data	(volatile unsigned char *)0x20270004
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

extern bool bFullA,bFullB,bLed,bHBild,bSTrans,bAlarm,bRx,bPush,bLongRx;
extern BYTE byBlockC,byLineC,byIC,byMax,byCrossDis,byCounter,byHelper,byRank;
extern BYTE byThresholdMD,byThresholdMask,byDelayMask,byIncrementMask,byChannel;
extern BYTE4 nFrameC,nSaveFrameC,nChecksum;
extern BYTE2 *pHeader2;
extern BYTE *pFrameBuff_A,*pFrameBuff_B,*pFrameBuff_C,*pHeader;
extern BYTE *pInBuffA,*pInBuffB;

extern BUFFER* pBuffer;
extern DATA_PACKET* pPacket;
extern SPIPacketStruct *pSPIReceivePacket, *pSPITransmitPacket;
extern MASK_STRUCT *pSPIReceiveMask;

extern volatile BYTE2 usDescriptor1[];
extern volatile BYTE2 usDescriptor2[];
extern volatile BYTE2 usSPIDescriptor1[];
extern volatile BYTE2 usSPIDescriptor2[];
extern volatile BYTE2 usSPIDescriptor3[];

//--------------------------------------------------------------------------//
// Prototypes																//
//--------------------------------------------------------------------------//
// in file i2c_function.c
void i2c_ini(void);
void i2c_start(void);
void i2c_stop(void);

void i2c_putbyte(BYTE byWort);
BYTE i2c_getbyte(void);

void i2c_nack();
void i2c_ack();
bool i2c_gack();

bool wait();

bool i2c_write(BYTE bySlAdr, BYTE bySuAdr, BYTE byDataW);
bool i2c_Read(BYTE bySlAdr, BYTE bySuAdr, BYTE *pbyDataR);

// in file led.c
void setled(BYTE byLed);
void clearled(BYTE byLed);	
bool pftest(BYTE2 usChoose);

// in file initialisation.c
void sram_ini();
void ppi_ini();
void isr_ini();
void sport_ini();
void timer_ini();
void adc_ini();
void clock_set();

// detection.asm
void framec();
void clear();
void diff(BYTE *pFrameBuff_C);
void clk();
void max_search(BYTE cOffset);
void adaptiv_mask();

// submain.c
void blocktransfer_end();
void blocktransfer_start();
void abbruch();
void sport_en();
void CreateImageHeader(BYTE2* pBuffer, int nHSize, int nVSize, int nChannel);
bool DoTrace(char *pString);

// spi.c
void RxSPI();
void TxSPI();
void SPI_en();
void SPI_dis();
void DoConfirm();
bool CheckIncommingData();
void make_Descriptor(BYTE2 wLength);
void Ini_Descriptor();
bool DoMessage(BYTE4 nMessageID, DWORD dwParam1, DWORD dwParam2, BYTE byPriority);

// Message Handling
bool RequestSetBrightness(DWORD dwParam1);
bool RequestSetContrast(DWORD dwParam1);
bool RequestSetSaturation(DWORD dwParam1);

bool RequestGetBrightness();
bool RequestGetContrast();
bool RequestGetSaturation();

bool RequestPing(DWORD dwParam1);

bool RequestSetThresholdMd(DWORD dwParam1);
bool RequestSetThresholdMask(DWORD dwParam1);
bool RequestSetIncrementMask(DWORD dwParam1);
bool RequestSetDelayMask(DWORD dwParam1);

bool RequestGetThresholdMd();
bool RequestGetThresholdMask();
bool RequestGetIncrementMask();
bool RequestGetDelayMask();
bool RequestSetPermanentMask();
bool RequestChangePermanentMask(DWORD dwParam1,DWORD dwParam2);
bool RequestInvertPermanentMask();
bool RequestClearPermanentMask();
bool RequestSetChannel(DWORD dwParam1);

// in file ISR.c
EX_INTERRUPT_HANDLER(PPI_ISR);
EX_INTERRUPT_HANDLER(BLOCKA_ISR);
EX_INTERRUPT_HANDLER(BLOCKB_ISR);
EX_INTERRUPT_HANDLER(SPORT_ISR);
EX_INTERRUPT_HANDLER(SPI_ISR);
EX_INTERRUPT_HANDLER(ERROR_ISR);
EX_INTERRUPT_HANDLER(TIMERERROR_ISR);

#endif __MAIN_DEFINED
