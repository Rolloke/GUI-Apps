/********************************************************************/
/* CODEC											  				*/
/********************************************************************/
/* Autor: Christian Lohmeier										*/
/* Datum: 16.07.2004												*/
/* Firma: Videte IT	AG												*/
/********************************************************************/
#include "main.h"
#include "codec.h"
#include "sport.h"
#include "filter.h"

extern volatile BYTE2 usDescriptor1[];
tMPEG4VideoEncParam lVideoEncParam0,lVideoEncParam1;
tBaseVideoEncoder *lBaseVideoEnc0,*lBaseVideoEnc1;  
uint32 lNumPkts0,lNumPkts1,*lPacketSizes0,*lPacketSizes1; 
uint32  lNumBytes0,lNumBytes1;
uint32  lVOPType0,lVOPType1, lOutputBufferSize0,lOutputBufferSize1;
bool bCoding = false;
extern bool bVideoAktive;
BOOL g_bEnableNR = FALSE;

/////////////////////////////////////////////////////////////////////////////
// Wandelt die Daten Aus den Eingangsbuffern in YUV 4:2:0 planar Format
bool makePlanar(ImageRes eImageRes1, ImageRes eImageRes2, BYTE* pSorce) 
{
	*pMDMA_S1_CONFIG &= ~DEN;	
	*pMDMA_D1_CONFIG &= ~DEN;

	*pMDMA_S1_PERIPHERAL_MAP = 0x0040;				// Quelle
	*pMDMA_S1_CURR_DESC_PTR = usMEMDescriptor1;		// Start Descriptorblock
	// 8 Bit transfer, Descriptorarray, 
	*pMDMA_S1_CONFIG = SIZE_08|DMA2|0x4700;
	*pMDMA_D1_PERIPHERAL_MAP = 0x0040;				// Ziel	
	*pMDMA_D1_CURR_DESC_PTR = usMEMDescriptor2;		// Start Descriptorblock
	// 8 Bit transfer, Descriptorarray
	*pMDMA_D1_CONFIG = SIZE_08|DMA2|MEMWR|0x4700;
 	
	DWORD dwFrameSize = wNormH[bySetNorm][eImageRes1]*wNormV[bySetNorm][eImageRes1]*2; // Berechnung der Framegröße in Bytes

	usMEMDescriptor1[0] = (BYTE2)((BYTE4)(pSorce+1+16) & 0xFFFF); 	// Startaddress Frame Buff A oder B
	usMEMDescriptor1[1] = (BYTE2)((BYTE4)(pSorce+1+16) >> 16);
	usMEMDescriptor1[2] = SIZE_08|DEN|DMA2|0x4700;					
	usMEMDescriptor1[3] = wNormH[bySetNorm][eImageRes1];			// x Cout					
	usMEMDescriptor1[5] = wNormV[bySetNorm][eImageRes1];			// y Cout

	usMEMDescriptor1[7] = (BYTE2)((BYTE4)(pSorce+0+16) & 0xFFFF); 	//Startaddress 
	usMEMDescriptor1[8] = (BYTE2)((BYTE4)(pSorce+0+16) >> 16);
	usMEMDescriptor1[9] = SIZE_08|DEN|DMA2|0x4700;	
	usMEMDescriptor1[10] = wNormH[bySetNorm][eImageRes1]>>1;		// x Cout
	usMEMDescriptor1[12] = wNormV[bySetNorm][eImageRes1]>>1;	   	// y Cout

	usMEMDescriptor1[14] = (BYTE2)((BYTE4)(pSorce+2+16) & 0xFFFF); // tartaddress 
	usMEMDescriptor1[15] = (BYTE2)((BYTE4)(pSorce+2+16) >> 16);
	usMEMDescriptor1[16] = SIZE_08|DEN|IEN|DMA2;		
	usMEMDescriptor1[17] = wNormH[bySetNorm][eImageRes1]>>1;		//  Cout
	usMEMDescriptor1[19] = wNormV[bySetNorm][eImageRes1]>>1;		// y Cout
	
	switch(eImageRes1)
	{ 
		case eImageResUnvalid:
			// do nothing
			break;
		case eImageResHigh:
		
			usMEMDescriptor1[4] = 2;						// x Modify					//y Cout
			usMEMDescriptor1[6] = 34;						// y Modify
			
			usMEMDescriptor1[11] = 4;						// x Modify
			usMEMDescriptor1[13] = 1476;					// y Modify
		
			usMEMDescriptor1[18] = 4;						// x Modify;
			usMEMDescriptor1[20] = 1476;					// y Modify
			break;
		
		
		case eImageResMid:
					
			usMEMDescriptor1[4] = 4;						// x Modify
			usMEMDescriptor1[6] = 36;						// y Modify
			
			usMEMDescriptor1[11] = 8;						// x Modify
			usMEMDescriptor1[13] = 1480;					// y Modify
								
			usMEMDescriptor1[18] = 8;						// x Modify
			usMEMDescriptor1[20] = 1480;			   		// y Modify		
			break;
		
		case eImageResLow:
					
			usMEMDescriptor1[4] = 8;						// x Modify
			usMEMDescriptor1[6] = 1480;						// y Modify
			
			usMEMDescriptor1[11] = 16;						// x Modify
			usMEMDescriptor1[13] = 4368;					// y Modify
							
			usMEMDescriptor1[18] = 16;						// x Modify
			usMEMDescriptor1[20] = 4368;			 		// y Modify  			
			break;
			
	}

	usMEMDescriptor2[0] = (BYTE2)((BYTE4)pBuffer->byYUV_In0 & 0xFFFF); 	// Zieladdress 
	usMEMDescriptor2[1] = (BYTE2)((BYTE4)pBuffer->byYUV_In0 >> 16);
	usMEMDescriptor2[2] = SIZE_08|MEMWR|DEN|DMA2|0x4500;	
	usMEMDescriptor2[3] = (dwFrameSize)>>3;								// x Cout
	usMEMDescriptor2[4] = 1;											// x Modify
	usMEMDescriptor2[5] = 4;											// y Cout
	usMEMDescriptor2[6] = 1;											// y Modify
		
	usMEMDescriptor2[7] = (BYTE2)((BYTE4)(pBuffer->byYUV_In0+(dwFrameSize>>1)) & 0xFFFF);	// Zieladdress
	usMEMDescriptor2[8] = (BYTE2)((BYTE4)(pBuffer->byYUV_In0+(dwFrameSize>>1)) >> 16);
	usMEMDescriptor2[9] = SIZE_08|MEMWR|DEN|0x4500;		
	usMEMDescriptor2[10] = (dwFrameSize)>>3;						// x Cout
	usMEMDescriptor2[11] = 1;										// x Modify

	
	usMEMDescriptor2[12] = (BYTE2)((BYTE4)(pBuffer->byYUV_In0+5*(dwFrameSize>>3)) & 0xFFFF); // Zieladdress
	usMEMDescriptor2[13] = (BYTE2)((BYTE4)(pBuffer->byYUV_In0+5*(dwFrameSize>>3)) >> 16);
	usMEMDescriptor2[14] = SIZE_08|MEMWR|IEN|DEN;				
	usMEMDescriptor2[15] = (dwFrameSize)>>3;						// x Cout
	usMEMDescriptor2[16] = 1;										// x Modify


	*pMDMA_S1_CONFIG |= DEN;	// DMA starten	
	*pMDMA_D1_CONFIG |= DEN;

	return(true);

}
/////////////////////////////////////////////////////////////////////////////

// Codiert je nach Anfrage Stream1 oder Stream2
bool DoCoding()
{
	BYTE4 i = 0;
	bCoding = true;
	bool bresult0 = false;
	bool bresult1 = false;
	asm volatile( ".align 4;" );
	if(bStreamReady0 && bStreamCode0)
	{
		DoFiltering(pBuffer, 0);
		 
		bStreamCode0 = false; 	
		gEzdMP4SPEnc_GetParam(lBaseVideoEnc0,GET_MAX_FRAME_SIZE,&lOutputBufferSize0) ;
		lNumBytes0 = lOutputBufferSize0;
		byMod = 0;
		if(bMod)
			byMod = 28;
		switch (gEzdMP4SPEnc_EncodeVideoFrame(lBaseVideoEnc0 ,pBuffer->byYUV_In0,
			   (pBuffer->byYUV_Out0+byMod),&lNumBytes0,&lVOPType0,lPacketSizes0,
				nFrameC))
		{
			case E_SUCCESS:
				bMod = true;
				bresult0 = true;
				break;
			case E_INVALID_ARGS:
				DoTrace("Codec Encode1: Invalid Args!\n");
				bresult0 = false;
				break;
			case E_OUTBUF_OVERFLOW:
				DoTrace("Codec Encode1: Outbuf overflow!\n");
				bresult0 = false;
				break;
		}	
	}
	if(bStreamReady1 && bStreamCode1)
	{
		DoFiltering(pBuffer, 1);
		
		bStreamCode1 = false; 
		CopyCif();
		gEzdMP4SPEnc_GetParam(lBaseVideoEnc1,GET_MAX_FRAME_SIZE,&lOutputBufferSize1) ;
		lNumBytes1 = lOutputBufferSize1;
		byMod1 = 0;
		if(bMod1)
			byMod1 = 40;
		switch (gEzdMP4SPEnc_EncodeVideoFrame(lBaseVideoEnc1 ,pBuffer->byYUV_In1,
			   (pBuffer->byYUV_Out1+byMod1),&lNumBytes1,&lVOPType1,lPacketSizes1,
				nFrameC1))
		{
			case E_SUCCESS:
				bresult1 = true;
				break;
			case E_INVALID_ARGS:
				sprintf(pPacket->Debug.sText,"Codec Encode2: Invalid Args! Handel: 0x%x Packet: 0x%x \n", lBaseVideoEnc1, lPacketSizes1);
				DoTrace(pPacket->Debug.sText);
				bresult1 = false;
				break;
			case E_OUTBUF_OVERFLOW:
				DoTrace("Codec Encode2: Outbuf overflow!\n");
				bresult1 = false;
				break;
		}
	}
	while (!bInSwitch || (i < 100))
	{
		WaitMicro(10);
		i++;
	}
	SportDisable();
	
	pPacket->bValid = FALSE;
	pPacket1->bValid = FALSE; 
	pPacket->dwImageDataLen = 0;
	pPacket1->dwImageDataLen = 0;
	pPacket->dwCheckSum = 0;
	pPacket1->dwCheckSum = 0;	
	
	if(bresult0)
	{	
		switch (lVOPType0)
		{
			case VENC_IVOP: 
				SetSportDMASorce(pBuffer->byYUV_Out0,0);		
				pPacket->dwImageDataLen	= lNumBytes0+byMod;
				pPacket->dwCheckSum = docheck((DWORD) (pBuffer->byYUV_Out0),(uint32) lNumBytes0+byMod);
				break;
			case VENC_PVOP:
				SetSportDMASorce(pBuffer->byYUV_Out0+byMod,0);
				pPacket->dwImageDataLen	= lNumBytes0;
				pPacket->dwCheckSum = docheck((DWORD) (pBuffer->byYUV_Out0+byMod),(uint32) lNumBytes0);
				break;
		}
		
		usDescriptor1[7] = SIZE_16|DEN|IEN;
		usDescriptor1[8] = (BYTE2) (pPacket->dwImageDataLen+8)>>1;
		bSportStart = true;
		
		pPacket->bVidPresent = TRUE; 
		pPacket->bValid = TRUE;
		pPacket->eSubType 	= (DataSubType) lVOPType0;
		pPacket->dwFieldID 	= nFrameC; 		

	}
	if(bresult1)
	{
		switch (lVOPType1)
		{
			case VENC_IVOP: 
				SetSportDMASorce(pBuffer->byYUV_Out1,1);		
				pPacket1->dwImageDataLen	= lNumBytes1+byMod1;
				pPacket1->dwCheckSum = docheck((DWORD) pBuffer->byYUV_Out1,(uint32) lNumBytes1+byMod1);
						// SPORT Descriptor
				usDescriptor1[20] = (BYTE2)((BYTE4)(pBuffer->byYUV_Out1) & 0xFFFF);
				usDescriptor1[21] = (BYTE2)((BYTE4)(pBuffer->byYUV_Out1) >> 16);
				usDescriptor1[22] = SIZE_16|DEN|0x4500;
				usDescriptor1[23] = 19;
				usDescriptor1[24] = 2;
				if (!bMod1)
				{
					usDescriptor1[25] = (BYTE2)((BYTE4)(pBuffer->byYUV_Out1+38) & 0xFFFF);
					usDescriptor1[26] = (BYTE2)((BYTE4)(pBuffer->byYUV_Out1+38) >> 16);
					usDescriptor1[27] = SIZE_16|DEN|IEN;
					usDescriptor1[28] = (BYTE2) ((pPacket1->dwImageDataLen+8)>>1);
					usDescriptor1[29] = 2;
				}
				else
				{
					usDescriptor1[25] = (BYTE2)((BYTE4)(pBuffer->byYUV_Out1+40) & 0xFFFF);
					usDescriptor1[26] = (BYTE2)((BYTE4)(pBuffer->byYUV_Out1+40) >> 16);
					usDescriptor1[27] = SIZE_16|DEN|IEN;
					usDescriptor1[28] = (BYTE2) ((pPacket1->dwImageDataLen+8)>>1);
					usDescriptor1[29] = 2;
				}
				bMod1 = true;
				break;
			case VENC_PVOP:
				SetSportDMASorce(pBuffer->byYUV_Out1+byMod1,1);
				pPacket1->dwImageDataLen	= lNumBytes1;
				pPacket1->dwCheckSum = docheck((DWORD) (pBuffer->byYUV_Out1+byMod1),(uint32) lNumBytes1);
						// SPORT Descriptor	
				usDescriptor1[20] = (BYTE2)((BYTE4)(pBuffer->byYUV_Out1+40) & 0xFFFF);
				usDescriptor1[21] = (BYTE2)((BYTE4)(pBuffer->byYUV_Out1+40) >> 16);
				usDescriptor1[22] = SIZE_16|DEN|IEN;
				usDescriptor1[23] = (BYTE2) ((pPacket1->dwImageDataLen+8)>>1);
				usDescriptor1[24] = 2;
	
				break;
		}
		usDescriptor1[7] = SIZE_16|DEN|0x4500; 	
		bSportStart = true;	
		
		pPacket1->bVidPresent = TRUE;
		pPacket1->bValid = TRUE; 
		pPacket1->eSubType 	= (DataSubType) lVOPType1;
		pPacket1->dwFieldID = nFrameC1;	
	
	}
	

	bCoding = false;
	return(bresult0 || bresult1);
	/*
	MODE  |  Startmarker  |   Endmarker   |   Länge   |
	---------------------------------------------------
	UVBR  |  0x000001B0   |    0x0A31     |     28    |
	VBR	  |  0x000001B0   |  0x04241463	  |     38    |
	CBR   |  0x000001B0   |  0x04241463	  |     38    |
	*/ 
}
/////////////////////////////////////////////////////////////////////////////

// Parameter festlegen
void codec1()
{	
	asm volatile( ".align 4;" );
	
//	lVideoEncParam0.VOPWidth = 352;			// Bildweite
//	lVideoEncParam0.VOPHeight = 288;		// Bildhöhe
//	lVideoEncParam0.BitRate = 4096000;		// Datenrate
//	lVideoEncParam0.VOPRate = 25;			// Bildrate
	lVideoEncParam0.Level = 5;				// Level 	
	lVideoEncParam0.DeInterlace = 0;		// DeInterlace Aus
	lVideoEncParam0.RCModel = VENC_UVBR;	// Ungezwungen variable Bitrate
	lVideoEncParam0.SVHMode = 0;			// short Video Header
	lVideoEncParam0.PacketMode = 0;			// Packet Mode
	lVideoEncParam0.PacketSize = 8192;		// Packet Größe
	lVideoEncParam0.AIR = 0;				// Adaptiv Intra Refresh	
//	lVideoEncParam0.GOVLength = 5;			// I-Frame Rate
	lVideoEncParam0.SearchRange = 20;		// Suchweite der Motionestamtion
	lVideoEncParam0.EncoderPerformance = 0;	// Komplexität
//	lVideoEncParam0.TimerResolution = 300;	// TimeStamp Auflösung >= VOPRate
	
	return;	// Encoder generieren
}
void codec2()
{	
	asm volatile( ".align 4;" );
	
//	lVideoEncParam1.VOPWidth = 352;			// Bildweite
//	lVideoEncParam1.VOPHeight = 288;		// Bildhöhe
//	lVideoEncParam1.BitRate = 4096000;		// Datenrate
//	lVideoEncParam1.VOPRate = 25;			// Bildrate
	lVideoEncParam1.Level = 5;				// Level 	
	lVideoEncParam1.DeInterlace = 0;		// DeInterlace Aus
	lVideoEncParam1.RCModel = VENC_CBR;		// Ungezwungen constand Bitrate
	lVideoEncParam1.SVHMode = 0;			// short Video Header
	lVideoEncParam1.PacketMode = 0;			// Packet Mode
	lVideoEncParam1.PacketSize = 8192;		// Packet Größe
	lVideoEncParam1.AIR = 0;				// Adaptiv Intra Refresh	
//	lVideoEncParam1.GOVLength = 5;			// I-Frame Rate
	lVideoEncParam1.SearchRange = 20;		// Suchweite der Motionestamtion
	lVideoEncParam1.EncoderPerformance = 0;	// Komplexität
	lVideoEncParam1.TimerResolution = 300;	// TimeStamp Auflösung >= VOPRate
	return;
}
/////////////////////////////////////////////////////////////////////////////

// Speicher für Codec reservieren
void IniCodecBuffer(BYTE byNum)
{
	switch(byNum)
	{
		case 1:
			gEzdMP4SPEnc_GetParam(lBaseVideoEnc0, GET_MAX_PACKETS, &lNumPkts0);	// Anzahl der Pakete ermitten
			lPacketSizes0 = (uint32 *) malloc(lNumPkts0 * sizeof(int32));			// Buffer reserviern
			break;
		case 2:
			gEzdMP4SPEnc_GetParam(lBaseVideoEnc1, GET_MAX_PACKETS, &lNumPkts1);	// Anzahl der Pakete ermitten
			lPacketSizes1 = (uint32 *) malloc(lNumPkts1 * sizeof(int32));			// Buffer reserviern
			break;		
	}
}
/////////////////////////////////////////////////////////////////////////////

// Codec Handel anlegen
void IniCodec(BYTE byNum)
{ 
	switch (byNum)
	{
		case 1:
			codec1();
			switch (gEzdMP4SPEnc_Create(&lBaseVideoEnc0,&lVideoEncParam0))	// Mpeg4 Encoder istantiesiern 
			{
				case E_SUCCESS:	
		  			DoTrace("Codec Instance1: Success!\n");	
					break;
				case E_OUT_OF_MEMORY:
		   			DoTrace("Codec Instance1: Out of Memory!\n");
					break;
				case E_INVALID_PARAMS:
		   			DoTrace("Codec Instance1: Invalid Parameter!\n");
					break;
				case E_INVALID_SIZE:
		   			DoTrace("Codec Instance1: Invalid Size!\n");
					break;
			}
			break; 	
		case 2:	
			codec2();
			switch (gEzdMP4SPEnc_Create(&lBaseVideoEnc1,&lVideoEncParam1))	// Mpeg4 Encoder istantiesiern 
			{
				case E_SUCCESS:	
		   			DoTrace("Codec Instance2: Success!\n");	
					break;
				case E_OUT_OF_MEMORY:
		   			DoTrace("Codec Instance2: Out of Memory!\n");
					break;
				case E_INVALID_PARAMS:
		   			DoTrace("Codec Instance2: Invalid Parameter!\n");
					break;
				case E_INVALID_SIZE:
		   			DoTrace("Codec Instance2: Invalid Size!\n");
					break;
			}
			break;
	}
}
/////////////////////////////////////////////////////////////////////////////
	
// Codec Objekt Löschen
bool DeleteCodec(BYTE byNr)
{
	bool bResult = false;
	switch(byNr)
	{
		case 1:
			if(gEzdMP4SPEnc_Delete(lBaseVideoEnc0)==E_SUCCESS)
			{
				free(lPacketSizes0);
				bResult = true;	
			}
			
			break;
		
		case 2:
		 	if(gEzdMP4SPEnc_Delete(lBaseVideoEnc1)==E_SUCCESS)
			{
				free(lPacketSizes1);
				bResult = true;	
			}
			break;
	}
	return(bResult);
}
/////////////////////////////////////////////////////////////////////////////

// Kopierroutine für den 2. Stream
void CopyCif()
{
	BYTE *pSource,*pDest;
	BYTE byDelta = 1;
	pSource = pBuffer->byYUV_In0;
	pDest	= pBuffer->byYUV_In1;
	if (eResulution[0] == eImageResHigh)
		byDelta = 2;
	switch (eResulution[1])
	{
		case eImageResMid:		
		
			for (BYTE4 nmax = 0; nmax < 152064; nmax++)
			{
				*(pDest+nmax) = *(pSource+byDelta*nmax);
			}
			break;
		case eImageResLow:
			for (BYTE4 ny = 0; ny < 144; ny++)
			{
				for (BYTE4 nx = 0; nx < 176; nx++)
				{
					*(pDest+nx+ny*176) = *(pSource+2*byDelta*(nx+ny*352));
				}
			}
			pSource += byDelta*352*288;
			pDest += 176*144;
			for (BYTE4 ny = 0; ny < 144; ny++)
			{
				for (BYTE4 nx = 0; nx < 88; nx++)
				{
					*(pDest+nx+ny*88) = *(pSource+2*byDelta*(nx+ny*176));
				}
			}
			break;
	}
}
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
bool DoFiltering(BUFFER* pBuffer, int nStreamID)
{
	BYTE* pY = NULL;
	BYTE* pU = NULL;
	BYTE* pV = NULL;
	int   nBPLY 	= 0;
	int   nBPLU		= 0;
	int   nBPLV		= 0;
	
	int   nWidthY  	= 0;
	int   nWidthU 	= 0;
	int   nWidthV	= 0;
			
	int   nHeightY  = 0;
	int   nHeightU 	= 0;
	int   nHeightV	= 0;

	if (nStreamID == 0)
	{
		DWORD dwFrameSize =	lVideoEncParam0.VOPWidth*lVideoEncParam0.VOPHeight;
		pY = &pBuffer->byYUV_In0[0];
		pU = &pBuffer->byYUV_In0[dwFrameSize];
		pV = &pBuffer->byYUV_In0[dwFrameSize + dwFrameSize/4];
		
		nBPLY  	= lVideoEncParam0.VOPWidth;
		nBPLU	= lVideoEncParam0.VOPWidth/2;
		nBPLV	= lVideoEncParam0.VOPWidth/2;

		nWidthY	= lVideoEncParam0.VOPWidth;
		nWidthU	= lVideoEncParam0.VOPWidth/2;
		nWidthV	= lVideoEncParam0.VOPWidth/2;
		
		nHeightY= lVideoEncParam0.VOPHeight;
		nHeightU= lVideoEncParam0.VOPHeight/2;
		nHeightV= lVideoEncParam0.VOPHeight/2;
	}
	else if (nStreamID == 1)
	{
		DWORD dwFrameSize =	lVideoEncParam1.VOPWidth*lVideoEncParam1.VOPHeight;
		pY = &pBuffer->byYUV_In1[0];
		pU = &pBuffer->byYUV_In1[dwFrameSize];
		pV = &pBuffer->byYUV_In1[dwFrameSize + dwFrameSize/4];
		
		nBPLY = lVideoEncParam1.VOPWidth;
		nBPLU = lVideoEncParam1.VOPWidth/2;
		nBPLV = lVideoEncParam1.VOPWidth/2;

		nWidthY	= lVideoEncParam1.VOPWidth;
		nWidthU	= lVideoEncParam1.VOPWidth/2;
		nWidthV	= lVideoEncParam1.VOPWidth/2;
		
		nHeightY= lVideoEncParam1.VOPHeight;
		nHeightU= lVideoEncParam1.VOPHeight/2;
		nHeightV= lVideoEncParam1.VOPHeight/2;
	}
	
	short int Mask3x3[9] = {455, 455, 455,
							455, 456, 455,
							455, 455, 455};
					  
	corr_3x3(pU, nHeightU, nWidthU, Mask3x3, pU);

	// Rauschunterdrückung aktiviert?
	if (g_bEnableNR)
	{
		static BYTE* pLastFrame = NULL;
		if (!pLastFrame)
			pLastFrame = (BYTE*)malloc(FRAME);
		temporal(pY, nHeightY, nWidthY, pLastFrame);	
	}
	
	return true;
}
