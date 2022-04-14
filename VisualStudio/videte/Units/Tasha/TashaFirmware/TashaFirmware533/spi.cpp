//////////////////////////////////////////////////////////
// SPI Funktions  BF533						 11.11.03   //
// Christian Lohmeier 									//
// videte IT											//
//////////////////////////////////////////////////////////

#include "main.h"
#include "spi.h"
#include "i2c.h"
#include "codec.h"
#include "sport.h"

extern BOOL g_bEnableNR;

extern tMPEG4VideoEncParam lVideoEncParam0, lVideoEncParam1;
SPIPacketStruct *pSPIReceivePacket, *pSPITransmitPacket;
MASK_STRUCT *pSPIReceiveMask;

volatile BYTE2 usSPIDescriptor1[SPI_DESCRIPTOR_SIZE];
volatile BYTE2 usSPIDescriptor2[SPI_DESCRIPTOR_SIZE];
volatile BYTE2 usSPIDescriptor3[SPI_DESCRIPTOR_SIZE];

/////////////////////////////////////////////////////////////////////////////
EX_INTERRUPT_HANDLER(SPI_ISR)
{	

	bool bFine = false;
	BYTE i = 0;	
	static bool bCopyMask = false;
	BYTE* pMask_B = byMask_B;			

	DoConfirm();
  	if (bRx)
	{
		SPI_dis();
		if (bCopyMask)
		{
			DWORD dwSize = pSPIReceiveMask->nDimH * pSPIReceiveMask->nDimV;
			if (dwSize != 1620)
			{
				dwSize = 1620;
			}
			memcpy (pMask_B, pSPIReceiveMask->byMask,dwSize); 
			DoMessage(TMM_CONFIRM_SET_PERMANENT_MASK,0,0,21);
		}
		if (DoMessage(TMM_SLAVE_ASLEEP, pSPIReceivePacket->dwParam1,pSPIReceivePacket->dwParam2,254)) 
			CheckIncommingData();
		if(!bLongRx)
		{	
			*pSPI_CTL = SPITX;
			*pDMA5_CURR_DESC_PTR = usSPIDescriptor2;
	    	*pDMA5_CONFIG = SIZE_16|IEN|0x4500;
			bRx = false;
		}
	}
	else 
	{	
		while(!bFine || (i < 100))
		{
			if(!(*pSPI_STAT & TXS))
			{
				WaitMicro(10);
				if(!(*pSPI_STAT & TXS))
					bFine = true;
			}
			i++;
		}
		SPI_dis();
		*pSPI_CTL = SPIRX;
		*pDMA5_CURR_DESC_PTR = usSPIDescriptor1;
	    *pDMA5_CONFIG = SIZE_16|IEN|MEMWR|0x4500;
		bRx = true;
		byRank = 0xFF;		
	}
	bCopyMask = bLongRx;
	bLongRx = false;
	SPI_en();
	*pDMA5_IRQ_STATUS = 0x0003;
}
/////////////////////////////////////////////////////////////////////////////

void Ini_Descriptor()			// Initialisiert Sende und Empfangs descriptor
{	
	
static	SPIPacketStruct bySPI_A;
static 	SPIPacketStruct bySPI_B;
	pSPIReceiveMask		= &byMask;	
	pSPIReceivePacket  	= &bySPI_A;
    pSPITransmitPacket 	= &bySPI_B;			// SPI Kommunikation mit Bf535
	*pSPI_CTL = SPIRX;						// "0" am Ende, 16 Bit, Empfangen 
	*pDMA5_PERIPHERAL_MAP = 0x5000;			// direction SPI
	*pDMA5_CURR_DESC_PTR = usSPIDescriptor1;//Start Descriptorblock
	// 16 Bit, Interrupt am Ende, In Speicher schreiben,  Liste mit 5 Einträgen
	*pDMA5_CONFIG = SIZE_16|IEN|MEMWR|0x4500;
    
	usSPIDescriptor1[0] = (BYTE2)((BYTE4) pSPIReceivePacket & 0xFFFF); 	// Startaddress RecivePacket
	usSPIDescriptor1[1] = (BYTE2)((BYTE4) pSPIReceivePacket >> 16);
	usSPIDescriptor1[2] = SIZE_16|DEN|IEN|MEMWR;						// 16 Bit, Interrupt am Ende, In Speicher schreiben	
	usSPIDescriptor1[3] = (sizeof(SPIPacketStruct)/2);					// Länge bei 16 Bit Transfer 
	usSPIDescriptor1[4] = 2;											// x Modify (16 Bit)
	
	usSPIDescriptor2[0] = (BYTE2)((BYTE4) pSPITransmitPacket & 0xFFFF); // Startaddress RecivePacket
	usSPIDescriptor2[1] = (BYTE2)((BYTE4) pSPITransmitPacket >> 16);		
	usSPIDescriptor2[2] = SIZE_16|DEN|IEN;								// 16 Bit, Interrupt am Ende, Aus Speicher lesen
	usSPIDescriptor2[3] = (sizeof(SPIPacketStruct)/2);					// Länge bei 16 Bit Transfer 
	usSPIDescriptor2[4] = 2;// x Modify (16 Bit)	
	bRx = true;
	return;
}
/////////////////////////////////////////////////////////////////////////////

void make_Descriptor(BYTE2 wLength)			// initiert einen Discriptorempfang mit wLength
{
	*pSPI_CTL = SPIRX;	// "0" am Ende, 16 Bit, Empfangen 
	*pDMA5_PERIPHERAL_MAP = 0x5000;			// direction SPI
	*pDMA5_CURR_DESC_PTR = usSPIDescriptor3;// aktueller Descriptorblock
	// 16 Bit, Interrupt am Ende, In Speicher schreiben,  Liste mit 5 Einträgen
	*pDMA5_CONFIG = SIZE_16|IEN|MEMWR|0x4500;
    
	usSPIDescriptor3[0] = (BYTE2)((BYTE4) pSPIReceiveMask & 0xFFFF); 	// Startaddress ReciveMask
	usSPIDescriptor3[1] = (BYTE2)((BYTE4) pSPIReceiveMask >> 16);
	usSPIDescriptor3[2] = SIZE_16|DEN|IEN|MEMWR;				  		// 16 Bit, Interrupt am Ende, In Speicher schreiben	
	usSPIDescriptor3[3] = wLength>>1;									// Länge bei 16 Bit Transfer 
	usSPIDescriptor3[4] = 2;											// x Modify (16 Bit)
	return;
}
/////////////////////////////////////////////////////////////////////////////
void SPI_en()				// Start Kommunikation
{
	*pDMA5_CONFIG |= DEN;
	*pSPI_CTL |= SPE;
	return;
}

/////////////////////////////////////////////////////////////////////////////
void SPI_dis()				// Stopp Kommunikation
{
	*pDMA5_CONFIG &= ~DEN;
	*pSPI_CTL &= ~SPE;
	return;
}

/////////////////////////////////////////////////////////////////////////////
bool CheckIncommingData()	// Prüft eingehende Masseges
{	
	bool	bResult = false;
	int nMessageID 	= pSPIReceivePacket->nMessageID;
	DWORD dwUserData= pSPIReceivePacket->dwUserData;
	DWORD dwParam1	= pSPIReceivePacket->dwParam1;
	DWORD dwParam2	= pSPIReceivePacket->dwParam2;
	DWORD dwCheck 	= pSPIReceivePacket->dwCheckSum;
	if(GetSPICheckSum(*pSPIReceivePacket) != dwCheck)
	{	
//		sprintf(pPacket->Debug.sText, "Slave: %lu Invalid SPI Checksum: (H)%lu != (S)%lu Message No: %lu \n",pPacket->wSource, dwCheck, GetSPICheckSum(*pSPIReceivePacket), nMessageID);
//		DoTrace(pPacket->Debug.sText);
		return false;
	}
 	switch (nMessageID)
	{
		case TMM_REQUEST_SET_BRIGHTNESS:
			bResult = RequestSetBrightness(dwParam1);
			break;		
		case TMM_REQUEST_SET_CONTRAST:
			bResult = RequestSetContrast(dwParam1);
			break;
		case TMM_REQUEST_SET_SATURATION:
			bResult = RequestSetSaturation(dwParam1);
			break; 
		case TMM_REQUEST_GET_BRIGHTNESS:
			bResult = RequestGetBrightness();
			break;		
		case TMM_REQUEST_GET_CONTRAST:
			bResult = RequestGetContrast();
			break;
		case TMM_REQUEST_GET_SATURATION:
			bResult = RequestGetSaturation();
			break;
		case TMM_REQUEST_BF533_PING:
			bResult = RequestPing(dwParam1);
			break;
		case TMM_REQUEST_SET_THRESHOLD_MD:
			bResult = RequestSetThresholdMd(dwParam1);
			break;               
		case TMM_REQUEST_SET_THRESHOLD_MASK:
			bResult = RequestSetThresholdMask(dwParam1);
			break;               
		case TMM_REQUEST_SET_INCREMENT_MASK:
			bResult = RequestSetIncrementMask(dwParam1);
			break;
		case TMM_REQUEST_SET_DELAY_MASK:
			bResult = RequestSetDelayMask(dwParam1);
			break;
		case TMM_REQUEST_GET_THRESHOLD_MD:
			bResult = RequestGetThresholdMd();
			break;               
		case TMM_REQUEST_GET_THRESHOLD_MASK:
			bResult = RequestGetThresholdMask();
			break;               
		case TMM_REQUEST_GET_INCREMENT_MASK:
			bResult = RequestGetIncrementMask();
			break;
		case TMM_REQUEST_GET_DELAY_MASK:
			bResult = RequestGetDelayMask();
			break;
		case TMM_REQUEST_SET_PERMANENT_MASK:
			bResult = RequestSetPermanentMask();
			break; 
		case TMM_REQUEST_CHANGE_PERMANENT_MASK:
			bResult = RequestChangePermanentMask(dwParam1,dwParam2);
			break;
		case TMM_REQUEST_INVERT_PERMANENT_MASK:
			bResult = RequestInvertPermanentMask();
			break;        
		case TMM_REQUEST_CLEAR_PERMANENT_MASK:
			bResult = RequestClearPermanentMask();
			break;                                                                                                                                                                          
		case TMM_REQUEST_SET_CHANNEL:
			bResult = RequestSetChannel(dwParam1);
			break;          		
		case TMM_REQUEST_SET_ENCODER_PARAMETER:
			bResult = RequestSetEncoderParam(dwUserData,dwParam1,dwParam2);
			break;        
		case TMM_REQUEST_GET_ENCODER_PARAMETER:
			bResult = RequestGetEncoderParam(dwParam1);
			break;     
		case TMM_REQUEST_NEW_FRAME:
			bResult = RequestNewFrame();
			break;     
		case TMM_REQUEST_SET_NR:
			bResult = RequestSetNR((BOOL)dwParam1);
			break;		
		case TMM_REQUEST_GET_NR:
			bResult = RequestGetNR();
			break;		
							  		
	}
	pSPITransmitPacket->dwCheckSum = GetSPICheckSum(*pSPITransmitPacket);
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
void DoConfirm()
{

	pSPITransmitPacket->nPending		=	pSPIReceivePacket->nPending;
	pSPITransmitPacket->dwUserData 		= 	pSPIReceivePacket->dwUserData;
	pSPITransmitPacket->wSlave	  		= 	SLAVE;
	return;
}
/////////////////////////////////////////////////////////////////////////////

bool DoMessage(BYTE4 nMessageID, DWORD dwParam1, DWORD dwParam2, BYTE byPriority)
{
	if (byPriority<byRank)
	{
		byRank = byPriority;
		pSPITransmitPacket->nMessageID 	= nMessageID;
		pSPITransmitPacket->dwParam1   	= dwParam1;
		pSPITransmitPacket->dwParam2 	= dwParam2;
		pSPITransmitPacket->wSlave	  	= SLAVE;
		pSPITransmitPacket->dwCheckSum = GetSPICheckSum(*pSPITransmitPacket); 
		return true;
	}
	else 
	{
		pSPITransmitPacket->dwParam1   	= 0;
		pSPITransmitPacket->dwParam2 	= 0;
		pSPITransmitPacket->dwCheckSum = GetSPICheckSum(*pSPITransmitPacket); 
		return false; 
	}
}
/////////////////////////////////////////////////////////////////////////////

bool RequestSetBrightness(DWORD dwParam1)
{
	BYTE byHelper = (BYTE)((float)((float)dwParam1*255)/100+0.5);
	if(i2c_write(SLAVE_ADDRESS, FE_BRIGHT, byHelper)) 
	{
		pSPITransmitPacket->nMessageID = TMM_CONFIRM_SET_BRIGHTNESS;
		return 	true;
	}
	else
		return false;
	
}
/////////////////////////////////////////////////////////////////////////////

bool RequestSetContrast(DWORD dwParam1)
{
	BYTE byHelper = (BYTE)((float)((float)dwParam1*127)/100+0.5);
	if(i2c_write(SLAVE_ADDRESS, FE_CONTRAST, byHelper)) 
	{
		pSPITransmitPacket->nMessageID = TMM_CONFIRM_SET_CONTRAST;
		return 	true;
	}
	else
		return false;
	
}
/////////////////////////////////////////////////////////////////////////////

bool RequestSetSaturation(DWORD dwParam1)
{
	BYTE byHelper = (BYTE)((float)((float)dwParam1*127)/100+0.5);
	if(i2c_write(SLAVE_ADDRESS, FE_SATURATION, byHelper)) 
	{
		pSPITransmitPacket->nMessageID = TMM_CONFIRM_SET_SATURATION;
		return 	true;
	}
	else
		return false;
	
}
/////////////////////////////////////////////////////////////////////////////

bool RequestGetBrightness()
{
	BYTE byHelper;
	if (i2c_read(SLAVE_ADDRESS, FE_BRIGHT, &byHelper))
	{
		pSPITransmitPacket->dwParam1 = (DWORD)((float)((float)byHelper*100)/255+0.5);
		pSPITransmitPacket->nMessageID = TMM_CONFIRM_GET_BRIGHTNESS;		
		return 	true;
	}
	else 
		return false;
	
}
/////////////////////////////////////////////////////////////////////////////

bool RequestGetContrast()
{
	BYTE byHelper;
	bool bResult;
	if (i2c_read(SLAVE_ADDRESS, FE_CONTRAST, &byHelper))
	{
		pSPITransmitPacket->dwParam1 = (DWORD)((float)((float)byHelper*100)/127+0.5);
		pSPITransmitPacket->nMessageID = TMM_CONFIRM_GET_CONTRAST;
		return 	true;
	}
	else 
		return false;
		
}
/////////////////////////////////////////////////////////////////////////////

bool RequestGetSaturation()
{
	BYTE byHelper;
	bool bResult;
	if (i2c_read(SLAVE_ADDRESS, FE_SATURATION, &byHelper))
	{
		pSPITransmitPacket->dwParam1 = (DWORD)((float)((float)byHelper*100)/127+0.5);
		pSPITransmitPacket->nMessageID = TMM_CONFIRM_GET_SATURATION;
		return 	true;
	}
	else 
		return false;
		
}
/////////////////////////////////////////////////////////////////////////////

bool RequestPing(DWORD dwParam1)
{
	pSPITransmitPacket->nMessageID = TMM_CONFIRM_BF533_PING;	
	return true;
}
/////////////////////////////////////////////////////////////////////////////

bool RequestSetThresholdMd(DWORD dwParam1)
{
	byThresholdMD = (BYTE)((float)((float)dwParam1*255)/100+0.5);;
	pSPITransmitPacket->nMessageID = TMM_CONFIRM_SET_THRESHOLD_MD;	
	return true;
}
/////////////////////////////////////////////////////////////////////////////

bool RequestSetThresholdMask(DWORD dwParam1)
{
	byThresholdMask = (BYTE)((float)((float)dwParam1*255)/100+0.5);
	pSPITransmitPacket->nMessageID = TMM_CONFIRM_SET_THRESHOLD_MASK;	
	return true;
}
/////////////////////////////////////////////////////////////////////////////

bool RequestSetIncrementMask(DWORD dwParam1)
{
	byIncrementMask = (BYTE) dwParam1;
	pSPITransmitPacket->nMessageID = TMM_CONFIRM_SET_INCREMENT_MASK;	
	return true;
}
/////////////////////////////////////////////////////////////////////////////

bool RequestSetDelayMask(DWORD dwParam1)
{
	byDelayMask = (BYTE) dwParam1;
	pSPITransmitPacket->nMessageID = TMM_CONFIRM_SET_DELAY_MASK;	
	return true;
}
/////////////////////////////////////////////////////////////////////////////

bool RequestGetThresholdMd()
{
	pSPITransmitPacket->dwParam1 = (DWORD)((float)((float)byThresholdMD*100)/255+0.5);
	
	pSPITransmitPacket->nMessageID = TMM_CONFIRM_GET_THRESHOLD_MD;
	return 	true;
}
/////////////////////////////////////////////////////////////////////////////

bool RequestGetThresholdMask()
{
	pSPITransmitPacket->dwParam1 = (DWORD)((float)((float)byThresholdMask*100)/255+0.5);
	pSPITransmitPacket->nMessageID = TMM_CONFIRM_GET_THRESHOLD_MASK;
	return 	true;
}
/////////////////////////////////////////////////////////////////////////////

bool RequestGetIncrementMask()
{
	pSPITransmitPacket->dwParam1 = (DWORD) (byIncrementMask);
	pSPITransmitPacket->nMessageID = TMM_CONFIRM_GET_INCREMENT_MASK;
	return 	true;

}
/////////////////////////////////////////////////////////////////////////////

bool RequestGetDelayMask()
{
	pSPITransmitPacket->dwParam1 = (DWORD) (byDelayMask);
	pSPITransmitPacket->nMessageID = TMM_CONFIRM_GET_DELAY_MASK;
	return 	true;

}
/////////////////////////////////////////////////////////////////////////////

bool RequestSetPermanentMask()
{
	//pSPIReceiveMask  = (MASK_STRUCT*) malloc(sizeof(MASK_STRUCT));
//	pPacket->PermanentMask.bHasChanged = (TRUE);  		// optimiert nicht speichern wenn falsch übertragen
	make_Descriptor(sizeof(MASK_STRUCT));
	bRx = true;
	bLongRx = true;
	return 	true;
}
/////////////////////////////////////////////////////////////////////////////

bool RequestChangePermanentMask(DWORD dwParam1,DWORD dwParam2)
{
	BYTE* pMask = byMask_B;
	BYTE byX,byY;
	byX = (BYTE) HI(dwParam1);
	byY = (BYTE) LO(dwParam1);
	 *(pMask+(byY*(COL/BLOCK/2)+byX)) = (BYTE) dwParam2;
	pPacket->PermanentMask.bHasChanged = (TRUE);
	pSPITransmitPacket->nMessageID	= TMM_CONFIRM_CHANGE_PERMANENT_MASK;
	return 	true;
}
/////////////////////////////////////////////////////////////////////////////

bool RequestInvertPermanentMask()
{
	BYTE *pMask = byMask_B;
	BYTE2 uslength;
	for (uslength = 0; uslength < 45*36; uslength++)
		*(pMask+uslength) = 0xFF - *(pMask+uslength);
	pSPITransmitPacket->nMessageID = TMM_CONFIRM_INVERT_PERMANENT_MASK;
	pPacket->PermanentMask.bHasChanged = (TRUE);
	nSaveFrameC = nFrameC;
	return true;	
}
/////////////////////////////////////////////////////////////////////////////

bool RequestClearPermanentMask()
{
	BYTE *pMask = byMask_B;
	BYTE2 uslength;
	for (uslength = 0; uslength < 45*36; uslength++)
		*(pMask+uslength) = 0xFF;
	pSPITransmitPacket->nMessageID = TMM_CONFIRM_CLEAR_PERMANENT_MASK;
	pPacket->PermanentMask.bHasChanged = (TRUE);
	nSaveFrameC = nFrameC;
	return true;	
}
/////////////////////////////////////////////////////////////////////////////

bool RequestSetChannel(DWORD dwParam1)
{
	byChannel = (BYTE) dwParam1;
	pSPITransmitPacket->nMessageID = TMM_CONFIRM_SET_CHANNEL;
	return true;	
}
/////////////////////////////////////////////////////////////////////////////

bool RequestSetEncoderParam(DWORD dwUserData,DWORD dwParam1,DWORD dwParam2)
{
	DWORD dwStreamNr = (DWORD) ((dwParam1>>18) & 0x03);
	switch (dwStreamNr)
	{
		case 0:
			eResulution[0] 	= (ImageRes)((dwParam1>>0) & 0x07);
			// Bildweite 
			sprintf(pPacket->Debug.sText, "Slave: %lu hat folgende Aufloesung erhalten: %lu Stream 1\n",pPacket->wSource,eResulution[0]);
			DoTrace(pPacket->Debug.sText);
			lVideoEncParam0.VOPRate 		= (float)	((dwParam1>>3) & 0x1f);
			lVideoEncParam0.GOVLength		= (DWORD)	((dwParam1>>12) & 0x3f);
			lVideoEncParam0.BitRate 		= (DWORD)1024*((dwParam1>>20) & 0xfff);
			pPacket->dwStreamID  			= dwStreamNr;
			pPacket->dwProzessID 			= dwUserData;
			lVideoEncParam0.TimerResolution = (DWORD)lVideoEncParam0.VOPRate;
			
			lVideoEncParam0.VOPWidth  = wNormH[bySetNorm][eResulution[0]];		// Bildweite 
			lVideoEncParam0.VOPHeight = wNormV[bySetNorm][eResulution[0]];
		
			bStreamChange0 = true;	
			break;
		case 1:
			eResulution[1] 	= (ImageRes)((dwParam1>>0) & 0x07);
			// Bildweite 
			lVideoEncParam1.VOPRate 		= (float)	((dwParam1>>3) & 0x1f);
			lVideoEncParam1.GOVLength		= (DWORD)	((dwParam1>>12) & 0x3f);
			lVideoEncParam1.BitRate 		= (DWORD)1024*((dwParam1>>20) & 0xfff);
			pPacket1->dwStreamID  			= dwStreamNr;
			pPacket1->dwProzessID 			= dwUserData;
			
			lVideoEncParam1.VOPWidth  = wNormH[bySetNorm][eResulution[1]];		// Bildweite 
			lVideoEncParam1.VOPHeight = wNormV[bySetNorm][eResulution[1]];

			bStreamChange1 = true;
			break;
		case 2:
			i2c_write(SLAVE_ADDRESS, (BYTE)((dwParam1>>12) & 0x3f), (BYTE)((dwParam1>>20) & 0xff));
			break;
		case 3:
			bCapture = bCapture ^ true; //Toggle bCapture
			break;
	}
	pSPITransmitPacket->nMessageID = TMM_CONFIRM_SET_ENCODER_PARAMETER;	
	return true;
}
//                           20	   18		         12           8	             3        0
// F  F  F  F  F  F  F  F  F  F  E  E  D  D  D  D  D  D  C  C  C  C  B  B  B  B  B  A  A  A
// F = Bitrate (64...4000KBits)
// E = StreamID (0,1)
// D = I-Frame Intervall (1...100)
// C = Compressiontype (eTypeMpeg4, eTypeYUV422...)
// B = Frames per second (1...25)
// A = Resolution (eImageResHigh, eImageResMid, eImageResLow)

/////////////////////////////////////////////////////////////////////////////

bool RequestGetEncoderParam(DWORD dwParam1)
{
	DWORD dwVal,dwBitRate;
	switch (dwParam1)
	{
		case 0:
			dwBitRate = lVideoEncParam0.BitRate/1024;
			dwVal = dwBitRate<<20|pPacket->dwStreamID<<18|lVideoEncParam0.GOVLength<<12|pPacket->eType<<8|(BYTE)lVideoEncParam0.VOPRate<<3|eResulution[0];
			break;
		case 1:
			dwBitRate = lVideoEncParam1.BitRate/1024;
			dwVal = dwBitRate<<20|pPacket1->dwStreamID<<18|lVideoEncParam1.GOVLength<<12|pPacket1->eType<<8|(BYTE)lVideoEncParam1.VOPRate<<3|eResulution[1];
			break;
	}
	pSPITransmitPacket->dwParam1 = dwVal;
	pSPITransmitPacket->nMessageID = TMM_CONFIRM_GET_ENCODER_PARAMETER;	
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool RequestNewFrame()
{
	dwRequestCount++;	
	pSPITransmitPacket->nMessageID = TMM_CONFIRM_NEW_FRAME;	
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool RequestSetNR(BOOL bEnable)
{
	g_bEnableNR = bEnable;
	pSPITransmitPacket->nMessageID = TMM_CONFIRM_SET_NR;	
	return true;	
}

/////////////////////////////////////////////////////////////////////////////
bool RequestGetNR()
{
	pSPITransmitPacket->dwParam1 = (DWORD)g_bEnableNR;
	pSPITransmitPacket->nMessageID = TMM_CONFIRM_GET_NR;		
	return 	true;	
}

/////////////////////////////////////////////////////////////////////////////

DWORD GetSPICheckSum(SPIPacketStruct SPIPacket)
{	
	DWORD dwCheckSum = 0;
	BYTE* pData;
	int nI;
	SPIPacket.dwCheckSum = 0;
	SPIPacket.nPending = 0;	
	pData  	= (BYTE*)&SPIPacket;

	// Message an den Slave verschicken und Antwort auswerten..
	for (nI = 0; nI < sizeof(SPIPacketStruct)/sizeof(BYTE); nI++)
		dwCheckSum += nI*pData[nI];
	return dwCheckSum;		
}
/////////////////////////////////////////////////////////////////////////////




