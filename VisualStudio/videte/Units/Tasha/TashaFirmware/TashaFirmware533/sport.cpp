/********************************************************************/
/* SPORT											  				*/
/********************************************************************/
/* Autor: Christian Lohmeier										*/
/* Datum: 22.04.2004												*/
/* Firma: Videte IT	AG												*/
/********************************************************************/
#include "main.h"
#include "sport.h" 

extern DATA_PACKET_HELPER DataPacket2,  *pHeader3;	
volatile BYTE2 usDescriptor1[30];

//------------------------------------------------------------------------------//
// SPORT_ISR						                                            //
// Beschreibung: Wird zyclisch nach jeder Sport Übertragung aufgerufen                   //
//------------------------------------------------------------------------------//
EX_INTERRUPT_HANDLER(SPORT_ISR)
{
	BYTE4 i=0;
	if (pPacket1->bValid)	// Framecounter erhöhen
		nFrameC1++;
	if (pPacket->bValid)
		nFrameC++;	
		
	PermanentMaskFlagReset();	
	pPacket->Debug.bValid = (FALSE);
 	while (!(*pSPORT0_STAT & TXHRE) || (i < 100))
 	{
 		WaitMicro(1);
 		i++;
 	}
	
 	SportDisable();
 	bInSwitch = true;
	pPacket->wSource = SLAVE;
	*pDMA4_IRQ_STATUS = *pDMA4_IRQ_STATUS ;	// Löscht Interrupt	
}
/////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------------------------------//
// Function: sport_ini				  			         //
// Beschreibung: TDM 32 Bit pro Kanal			 			         //
//-----------------------------------------------------------------------------------------------------//
void sport_ini()
{ 
	// Sport0 receive configuration
	// External CLK, External Frame sync, MSB first
	// 16-bit data
	*pSPORT0_RCR1 = 0;//RFSR;
	*pSPORT0_RCR2 = SLEN_16;
	
	// Sport0 transmit configuration
	// External CLK, External Frame sync, MSB first
	// 16-bit data
	*pSPORT0_TCR1 = 0;//TFSR;
	*pSPORT0_TCR2 = SLEN_16;
	
	// Enable MCM 4 transmit & receive channels
	*pSPORT0_MTCS0 = 0x00000003<<(2*CHANNEL);	// Channel 1&2 (2x16Bit=32Bit)	
	*pSPORT0_MRCS0 = 0x00000000;
	
	// Set MCM configuration register and enable MCM mode
	*pSPORT0_MCMC1 = 0x0000;
	*pSPORT0_MCMC2 = 0x1014;					// Framedelay 1 Clock
	
//-----------------------------------------------------------------------------------------------------//	
// Set up DMA4 to transmit by SPORT
//-----------------------------------------------------------------------------------------------------//		
	*pDMA4_PERIPHERAL_MAP = 0x2000;			//direction SPORT
	*pDMA4_CURR_DESC_PTR = usDescriptor1;		//Start Descriptorblock
	// 8 Bit transfer, Descriptorarray, 5 rows
	*pDMA4_CONFIG = SIZE_16|0x4500;
    
	usDescriptor1[0] = (BYTE2)((BYTE4)(pPacket) & 0xFFFF); //Startaddress byHeader
	usDescriptor1[1] = (BYTE2)((BYTE4)(pPacket) >> 16);
	usDescriptor1[2] = SIZE_16|DEN|0x4500;			//next 7 rows, Enable
	usDescriptor1[3] = (sizeof(DATA_PACKET)-4)>>1;			//x Cout
	usDescriptor1[4] = 2;	
	
		// SPORT Descriptor
	
	usDescriptor1[5] = (BYTE2)((BYTE4)(&pBuffer->byYUV_Out0) & 0xFFFF);
	usDescriptor1[6] = (BYTE2)((BYTE4)(&pBuffer->byYUV_Out0) >> 16);
	usDescriptor1[7] = SIZE_16|DEN|0x4500;
	usDescriptor1[8] = 32768;
	usDescriptor1[9] = 2;

	usDescriptor1[10] = (BYTE2)((BYTE4)(pHeader3) & 0xFFFF); //Startaddress byHeader
	usDescriptor1[11] = (BYTE2)((BYTE4)(pHeader3) >> 16);
	usDescriptor1[12] = SIZE_16|DEN|0x4500;			//next 7 rows, Enable
	usDescriptor1[13] = (sizeof(DATA_PACKET_HELPER))>>1;			//x Cout
	usDescriptor1[14] = 2;	
	
	usDescriptor1[15] = (BYTE2)((BYTE4)(pHeader2+sizeof(DATA_PACKET_HELPER)/2) & 0xFFFF); //Startaddress byHeader
	usDescriptor1[16] = (BYTE2)((BYTE4)(pHeader2+sizeof(DATA_PACKET_HELPER)/2) >> 16);
	usDescriptor1[17] = SIZE_16|DEN|0x4500;			//next 7 rows, Enable
	usDescriptor1[18] = (sizeof(DATA_PACKET)-4-sizeof(DATA_PACKET_HELPER))>>1;			//x Cout
	usDescriptor1[19] = 2;	
	

}
//////////////////////////////////////////////////////////////////////////////

void SportDisable()// SPORT Port ausschalten
{
	*pDMA4_CONFIG &= ~DEN;
	*pSPORT0_TCR1 &= ~DEN;	
	*pSPORT0_RCR1 &= ~DEN;
}
//////////////////////////////////////////////////////////////////////////////

void SportEnable()// SPORT Port starten
{

	*pDMA4_CONFIG |= DEN;
	*pSPORT0_TCR1 |= DEN;	
	*pSPORT0_RCR1 |= DEN;
}
//////////////////////////////////////////////////////////////////////////////

void SetSport()	//Setzen der Sport DMA Übertragungslänge in Bytes 
{
	*pDMA4_CURR_DESC_PTR = usDescriptor1;
	*pDMA4_CONFIG = SIZE_16|0x4500;
}
//////////////////////////////////////////////////////////////////////////////

void SetSportDMASorce(BYTE* pSorce, BYTE bySorce)
{
	switch (bySorce)
	{
		case 0:	
//			usDescriptor1[7] = SIZE_16|DEN|IEN;		
			usDescriptor1[5] = (BYTE2)((BYTE4)pSorce & 0xFFFF);
			usDescriptor1[6] = (BYTE2)((BYTE4)pSorce >> 16);
			break;
		case 1:
//			usDescriptor1[7] = SIZE_16|DEN|0x4500; 			
//			usDescriptor1[25] = (BYTE2)((BYTE4)pSorce & 0xFFFF);
//			usDescriptor1[26] = (BYTE2)((BYTE4)pSorce >> 16);
			break;
	}
}
//////////////////////////////////////////////////////////////////////////////

bool DoTrace(char *pString)
{
	if (pPacket->Debug.bValid == (FALSE))
	{
		pPacket->Debug.bValid = (TRUE);
    	copy((BYTE2*) pPacket->Debug.sText, (BYTE2*) pString, MAX_DEBUG_LEN);
    	return true;
	}
	else
	{
		return false;
	}
}
/////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------//
// Funktion: CreateImageHeader					                            //
// Beschreibung: Instantisiert den Header                        			//	
//--------------------------------------------------------------------------//
void CreateImageHeader(BYTE2* pBuffer, int nHSize, int nVSize, int nChannel, bool bHelp)
{	   
	if(!bHelp)
	{
		pPacket = (DATA_PACKET*)pBuffer;
		pPacket->dwDummy			= 0x5555;
		pPacket->dwStartMarker		= (MAGIC_MARKER_START);
		pPacket->bValid				= (TRUE);
		pPacket->wSize				= (sizeof(DATA_PACKET));
		pPacket->eType				= (DataType)(eTypeMpeg4);	//(eTypeYUV422);
		pPacket->eSubType			= (DataSubType)(eSubTypeUnvalid);
		pPacket->wSource			= (nChannel);
		pPacket->wSizeH				= (nHSize);
		pPacket->wSizeV				= (nVSize);
		pPacket->wBytesPerPixel		= (2);
		pPacket->wField				= (1);
		pPacket->bVidPresent		= (FALSE); //(TRUE);
		pPacket->dwImageDataLen		= (nHSize*nVSize*(pPacket->wBytesPerPixel));
		pPacket->bMotion			= (FALSE);
		pPacket->TimeStamp.bValid 	= (FALSE);
		pPacket->AdaptiveMask.bHasChanged = FALSE;
		pPacket->AdaptiveMask.nDimH	= (45);
		pPacket->AdaptiveMask.nDimV	= (wNormV[bySetNorm][eResulution[0]]/BLOCK);
		pPacket->PermanentMask.bHasChanged = (FALSE);
		pPacket->PermanentMask.nDimH= (45);
		pPacket->PermanentMask.nDimV= (wNormV[bySetNorm][eResulution[0]]/BLOCK);
		pPacket->Diff.nDimH			= (45);
		pPacket->Diff.nDimV			= (wNormV[bySetNorm][eResulution[0]]/BLOCK);
		pPacket->dwEndMarker		= (MAGIC_MARKER_END);
	}
	else
	{
		pPacket1 = (DATA_PACKET_HELPER*)pBuffer;
		pPacket1->dwDummy			= 0x5555;
		pPacket1->dwStartMarker		= (MAGIC_MARKER_START);
		pPacket1->bValid			= (TRUE);
		pPacket1->wSize				= (sizeof(DATA_PACKET));
		pPacket1->eType				= (DataType)(eTypeMpeg4);	//(eTypeYUV422);
		pPacket1->eSubType			= (DataSubType)(eSubTypeUnvalid);
		pPacket1->wSource			= (nChannel);
		pPacket1->wSizeH			= (nHSize);
		pPacket1->wSizeV			= (nVSize);
		pPacket1->wBytesPerPixel	= (2);
		pPacket1->wField			= (1);
		pPacket1->bVidPresent		= (FALSE); //(TRUE);
		pPacket1->dwImageDataLen	= (nHSize*nVSize*(pPacket1->wBytesPerPixel));

	}
return;		
}
/////////////////////////////////////////////////////////////////////////////

