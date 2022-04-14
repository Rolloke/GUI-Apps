//////////////////////////////////////////////////////////
// SPI Funktions  BF533						 11.11.03   //
// Christian Lohmeier 									//
// videte IT											//
//////////////////////////////////////////////////////////

#include "main.h"

/////////////////////////////////////////////////////////////////////////////
void Ini_Descriptor()						// Initialisiert Sende und Empfangs descriptor
{											// SPI Kommunikation mit Bf535
	*pSPI_CTL = SZ | SPISIZE_16 | SPIDMARX;	// "0" am Ende, 16 Bit, Empfangen 
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
	usSPIDescriptor2[4] = 2;											// x Modify (16 Bit)	
	return;
}

/////////////////////////////////////////////////////////////////////////////
void make_Descriptor(BYTE2 wLength)			// initiert einen Discriptorempfang mit wLength
{
	*pSPI_CTL = SZ | SPISIZE_16 | SPIDMARX;	// "0" am Ende, 16 Bit, Empfangen 
	*pDMA5_PERIPHERAL_MAP = 0x5000;			// direction SPI
	*pDMA5_CURR_DESC_PTR = usSPIDescriptor3;// aktueller Descriptorblock
	// 16 Bit, Interrupt am Ende, In Speicher schreiben,  Liste mit 5 Einträgen
	*pDMA5_CONFIG = SIZE_16|IEN|MEMWR|0x4500;
    
	usSPIDescriptor3[0] = (BYTE2)((BYTE4) pSPIReceiveMask & 0xFFFF); 	// Startaddress ReciveMask
	usSPIDescriptor3[1] = (BYTE2)((BYTE4) pSPIReceiveMask >> 16);
	usSPIDescriptor3[2] = SIZE_16|DEN|IEN|MEMWR;				  		// 16 Bit, Interrupt am Ende, In Speicher schreiben	
	usSPIDescriptor3[3] = wLength/2;									// Länge bei 16 Bit Transfer 
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
	}
	
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
	return true;
	}
	else 
	return false; 
}
/////////////////////////////////////////////////////////////////////////////

bool RequestSetBrightness(DWORD dwParam1)
{
	char byHelper = (char) (((dwParam1*255)/100)-128);
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
	char byHelper = (char) ((dwParam1*255)/100);
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
	char byHelper = (char) ((dwParam1*255)/100);
	if(i2c_write(SLAVE_ADDRESS, FE_SATURATION, byHelper)) 
	{
		pSPITransmitPacket->nMessageID = TMM_CONFIRM_SET_SATURATION;
		stopc();
    	pPacket->dwProzessTime = (getCyclesLo()/(CCLK/1000000));
		return 	true;
	}
	else
		return false;
	
}

/////////////////////////////////////////////////////////////////////////////
bool RequestGetBrightness()
{
	char byHelper;
	if (i2c_read(SLAVE_ADDRESS, FE_BRIGHT, &byHelper))
	{
		pSPITransmitPacket->dwParam1 = (DWORD) ((100*(byHelper+128))/255);
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
		pSPITransmitPacket->dwParam1 = (DWORD) ((100*byHelper)/255);
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
		pSPITransmitPacket->dwParam1 = (DWORD) ((100*byHelper)/255);
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
	byThresholdMD = (BYTE) ((dwParam1*255)/100);
	pSPITransmitPacket->nMessageID = TMM_CONFIRM_SET_THRESHOLD_MD;	
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool RequestSetThresholdMask(DWORD dwParam1)
{
	byThresholdMask = (BYTE) ((dwParam1*255)/100);
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
	pSPITransmitPacket->dwParam1 = (DWORD) ((byThresholdMD*100)/255);
	pSPITransmitPacket->nMessageID = TMM_CONFIRM_GET_THRESHOLD_MD;
	return 	true;
}

/////////////////////////////////////////////////////////////////////////////
bool RequestGetThresholdMask()
{
	pSPITransmitPacket->dwParam1 = (DWORD) ((byThresholdMask*100)/255);
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
	pSPIReceiveMask  = (MASK_STRUCT*) malloc(sizeof(MASK_STRUCT));
	make_Descriptor((BYTE2)sizeof(MASK_STRUCT));
	bRx = true;
	bLongRx = true;
	pSPIReceivePacket->nMessageID 	= TMM_SLAVE_ASLEEP;
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


