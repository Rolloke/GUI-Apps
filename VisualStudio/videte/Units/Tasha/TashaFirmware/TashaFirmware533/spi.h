/********************************************************************/
/* Autor: Christian Lohmeier										*/
/* Datum: 26.08.2004												*/
/* Firma: Videte IT	AG												*/
/********************************************************************/
#ifndef  __SPI_DEFINED
	#define __SPI_DEFINED

///////////////////////////////////////////////////////////////////////////////	
// Definitionen 
///////////////////////////////////////////////////////////////////////////////	
#define SPIDMARX 	0x0002
#define SPIDMATX    0x0003
#define SPISIZE_16  0x0100
#define SPISIZE_8	0x0000
#define SPITX 	SZ|SPISIZE_16|SPIDMATX|EMISO;
#define SPIRX 	SZ|SPISIZE_16|SPIDMARX;
#define SPI_DESCRIPTOR_SIZE 5
 
///////////////////////////////////////////////////////////////////////////////	
// Prototypes
///////////////////////////////////////////////////////////////////////////////
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

bool RequestSetEncoderParam(DWORD dwUserData,DWORD dwParam1,DWORD dwParam2);
bool RequestGetEncoderParam(DWORD dwParam1);
bool RequestNewFrame();

bool RequestSetNR(BOOL bEnable);
bool RequestGetNR();

DWORD GetSPICheckSum(SPIPacketStruct SPIPacket);
///////////////////////////////////////////////////////////////////////////////

#endif //__SPI_DEFINED
