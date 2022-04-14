/********************************************************************/
/* Unterprogramme 													*/
/********************************************************************/
/* Autor: Christian Lohmeier										*/
/* Datum: 18.06.2003												*/
/* Firma: Videte IT	AG												*/
/********************************************************************/
#include "main.h"
#include "export_hdr\exp_SP_MPEG4_Enc.h"
extern	tMPEG4VideoEncParam lVideoEncParam;
extern  tBaseVideoEncoder*   lBaseVideoEnc;  
extern  uint32 lNumPkts,*lPacketSizes; 
extern  uint32  lNumBytes;
extern  uint32  lVOPType, lOutputBufferSize;
//--------------------------------------------------------------------------//
// Funktion: blocktransfer_start				                            //
// Beschreibung: Startet den DMA für Videobuffer A und B	                //
//--------------------------------------------------------------------------//
void blocktransfer_start()
{   
	*pMDMA_S1_CONFIG	= (*pMDMA_S1_CONFIG | DEN);	
	*pMDMA_D1_CONFIG	= (*pMDMA_D1_CONFIG | DEN);
	return;
}

//----------------------------------------------------------------------------------------------------------//
// Funktion: blocktransfer_end							                                                    //
// Beschreibung: 	Nach der Beendigung wird das Differenzbild aus   			                            //
//		Videobuffer A und B (8 Zeilen CIF nur Y) 			 	                                            //
//		wenn das Differenzbild fertig ist, wird der Maximalwert gesucht	                                    //
//----------------------------------------------------------------------------------------------------------//
void blocktransfer_end()
{			
	pFrameBuff_A = byFrameBuff_A;	// Zeiger auf Zwischenspeicher im L1 für je 8 Zeilen
	pFrameBuff_B = byFrameBuff_B;	// Zeiger auf Zwischenspeicher im L1 für je 8 Zeilen
    if (nFrameC >= 2)			// Wenn beide Bilder im SDRAM liegen 
    {
    	diff(pFrameBuff_C); 	// Differenzbild bilden über 8 Zeilen in Blöcken
    }
	byLineC++;				        // nach jeden 8 Zeilen erhöhen
    pInBuffA += 8*2*COL;		// Zeiger auf Eingangsbuffer A um 8 Zeilen erhöhen
    pInBuffB += 8*2*COL;  		// Zeiger auf Eingangsbuffer B um 8 Zeilen erhöhen
	pFrameBuff_C += (COL/2)/BLOCK;	// Zeiger auf Differenzwertbuffer um 45 Blöcke erhöhen
	bFullA = false;				    // Zwischenspeicher A frei 
	bFullB = false;				// Zwischenspeicher B frei
     
    if (byLineC == (ROW/2)/BLOCK) 	// Wenn Differenzbild vollständig (nach 36 Zeilen)
    	{
    	byCounter++;
    	if (byCounter > byDelayMask)
    	{		  	    	
    		adaptiv_mask();	       	// adaptive Maske erstellen
    		byCounter = 0;
    	}
    	
    	max_search(OFF);        // 1. Maximum suchen und Koordinaten in den Header 
	   	if(bLed)
	   		pPacket->Point[0].nValue 		= ((BYTE4)byMax);
    	byMax = 0;			    // Maximalwert löschen
	   	max_search(OFF+2);	    // 2. Maximum suchen und Koordinaten in den Header
	   	if(bLed)
	   		pPacket->Point[1].nValue 		= ((BYTE4)byMax);
	   	byMax = 0;			    // Maximalwert löschen
	   	max_search(OFF+4);		// 3. Maximum suchen und Koordinaten in den Header
	   	if(bLed)
	   		pPacket->Point[2].nValue 		= ((BYTE4)byMax);
	   	byMax = 0;			    // Maximalwert löschen
	   	max_search(OFF+6);		// 4. Maximum suchen und Koordinaten in den Header
	   	if(bLed)
	   		pPacket->Point[3].nValue 		= ((BYTE4)byMax);
	   	byMax = 0;			    // Maximalwert löschen
	   	max_search(OFF+8);		// 5. Maximum suchen und Koordinaten in den Header
	   	if(bLed)
	   		pPacket->Point[4].nValue 		= ((BYTE4)byMax);
	   	byMax = 0;			    // Maximalwert löschen
    
    	if (bLed) 				// Bewegungsalarm
    	{	
    		byCrossDis = 0;	
    		pPacket->bMotion			= (TRUE);
    		pPacket->Point[0].cx 		= ((*(pHeader+OFF+0))*1000/45);	
    		pPacket->Point[0].cy 		= ((*(pHeader+OFF+1))*1000/36);	
    		pPacket->Point[1].cx 		= ((*(pHeader+OFF+2))*1000/45);	
    		pPacket->Point[1].cy 		= ((*(pHeader+OFF+3))*1000/36);	
    		pPacket->Point[2].cx 		= ((*(pHeader+OFF+4))*1000/45);	
    		pPacket->Point[2].cy 		= ((*(pHeader+OFF+5))*1000/36);	
    		pPacket->Point[3].cx 		= ((*(pHeader+OFF+6))*1000/45);	
    		pPacket->Point[3].cy 		= ((*(pHeader+OFF+7))*1000/36);	
    		pPacket->Point[4].cx 		= ((*(pHeader+OFF+8))*1000/45);	
    		pPacket->Point[4].cy 		= ((*(pHeader+OFF+9))*1000/36);	
    	} 	
    	if (!bLed)				// kein Bewegungsalarm
    	{
			pPacket->bMotion			= (FALSE);
			pPacket->Point[0].nValue 	= 0;
			pPacket->Point[1].nValue 	= 0;
			pPacket->Point[2].nValue 	= 0;
			pPacket->Point[3].nValue 	= 0;
			pPacket->Point[4].nValue 	= 0;
    	}
 		pPacket->AdaptiveMask.bValid = (TRUE);										// adaptive Maske vorhanden
 		copy((BYTE2*)(pPacket->AdaptiveMask.byMask), (BYTE2*)(byMask_A), 45*36);	// in Header copieren
 		pPacket->PermanentMask.bValid = (TRUE);										// permanente Maske vorhanden
 		copy((BYTE2*)(pPacket->PermanentMask.byMask), (BYTE2*)(byMask_B), 45*36);	// in Header copieren
		pPacket->Diff.bValid = (TRUE);												// Differenzbild vorhanden
		copy((BYTE2*)(pPacket->Diff.byMask), (BYTE2*)(byFrameBuff_C), 45*36);		// in Header copieren
	
			 
    	bLed = false;						// Alarm löschen					
    	byLineC = 0;						// Zeilenzähler auf Null zurücksetzen
    	pInBuffA = 0;						// Zeiger auf Eingangsbuffer A
    	pInBuffB = 0;						// Zeiger auf Eingangsbuffer B
    	pInBuffA += 1;						// Auf den ersten Y Wert InBuffA
    	pInBuffB += (FRAME+1);				// Auf den ersten Y Wert InBuffB
    	pFrameBuff_C = byFrameBuff_C;      	// Zeiger auf Differenzbildspeicher 
    	IniMemDMA();      				
    	return;  			  				// zurück zum Aufruf
   	}
   	usMEMDescriptor3[0] = (BYTE2)((BYTE4)(pInBuffA) & 0xFFFF); //Startaddress byHeader
	usMEMDescriptor3[1] = (BYTE2)((BYTE4)(pInBuffA) >> 16);
	usMEMDescriptor3[5] = (BYTE2)((BYTE4)(pInBuffB) & 0xFFFF); //Startaddress byHeader
	usMEMDescriptor3[6] = (BYTE2)((BYTE4)(pInBuffB) >> 16);
	usMEMDescriptor4[0] = (BYTE2)((BYTE4)(pFrameBuff_A) & 0xFFFF); //Startaddress byHeader
	usMEMDescriptor4[1] = (BYTE2)((BYTE4)(pFrameBuff_A) >> 16);
	usMEMDescriptor4[5] = (BYTE2)((BYTE4)(pFrameBuff_B) & 0xFFFF); //Startaddress byHeader
	usMEMDescriptor4[6] = (BYTE2)((BYTE4)(pFrameBuff_B) >> 16);
	blocktransfer_start();				  	// DMA starten 
	return;						          	// zurück zum Aufruf
}

//--------------------------------------------------------------------------//
// Funktion: CreateImageHeader					                            //
// Beschreibung: Instantisiert den Header                        			//	
//--------------------------------------------------------------------------//
void CreateImageHeader(BYTE2* pBuffer, int nHSize, int nVSize, int nChannel)
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
	pPacket->bVidPresent		= (TRUE);
	pPacket->dwFieldID			= (123);
	pPacket->dwProzessID		= (0);
	pPacket->dwImageDataLen		= (nHSize*nVSize*(pPacket->wBytesPerPixel));	
	pPacket->bMotion			= (FALSE);
	pPacket->Point[0].cx		= (WORD)(10);
	pPacket->Point[0].cy		= (WORD)(15);
	pPacket->Point[0].nValue	= (100);
	pPacket->Point[1].cx		= (WORD)(0);
	pPacket->Point[1].cy		= (WORD)(0);
	pPacket->Point[1].nValue	= (200);
	pPacket->Point[2].cx		= (WORD)(0);
	pPacket->Point[2].cy		= (WORD)(0);
	pPacket->Point[2].nValue	= (300);
	pPacket->Point[3].cx		= (WORD)(0);
	pPacket->Point[3].cy		= (WORD)(0);
	pPacket->Point[3].nValue	= (400);
	pPacket->Point[4].cx		= (WORD)(0);
	pPacket->Point[4].cy		= (WORD)(0);
	pPacket->Point[4].nValue	= (500);
	pPacket->TimeStamp.bValid 	= (FALSE);
	pPacket->TimeStamp.wYear	= (WORD)(2003);
	pPacket->TimeStamp.wMonth	= (8);
	pPacket->TimeStamp.wDay		= (4);
	pPacket->TimeStamp.wHour	= (13);
	pPacket->TimeStamp.wMinute	= (3);
	pPacket->TimeStamp.wSecond	= (34);
	pPacket->TimeStamp.wMSecond	= (333);
	pPacket->AdaptiveMask.bHasChanged = FALSE;
	pPacket->AdaptiveMask.nDimH	= (45);
	pPacket->AdaptiveMask.nDimV	= (36);
	pPacket->PermanentMask.bHasChanged = (FALSE);
	pPacket->PermanentMask.nDimH= (45);
	pPacket->PermanentMask.nDimV= (36);
	pPacket->Diff.nDimH			= (45);
	pPacket->Diff.nDimV			= (36);
	pPacket->dwEndMarker		= (MAGIC_MARKER_END);

	

	return;
}

void copy(BYTE2 *pDest, BYTE2 *pSource, BYTE2 nLength)	// Kopierfunktion
{
	BYTE2 nmax;
	for (nmax = 0; nmax < nLength/2; nmax++)
	{
		*(pDest+nmax) = *(pSource+nmax);
	}
}

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

bool makePlanar()
{
	BYTE4 i=0;
/*	BYTE4 iX, iY, iMody;
	for(iY = 0; iY < HEIGHT; iY++)		// sort Y 
	{
		for(iX = 0;iX < WIDTH; iX++)
		{
			iMody = (iX*2*2+1)+(iY*1440);
		//	iMody += (iMody/4096)*4096;
			*(pBuffer->byYUV_In+iX+(iY*WIDTH)) =	*(pBuffer->byInBuff_A+iMody);
		}		
	}
	
	for(iY = 0; iY < HEIGHT/2; iY++)		// sort Cb
	{
		for(iX = 0;iX < WIDTH/2; iX++)
		{	
			iMody = (iX*4*2+2)+(iY*2880);
		//	iMody += (iMody/4096)*4096;
			*(pBuffer->byYUV_In+iX+HEIGHT*WIDTH+(iY*WIDTH/2)) =	*(pBuffer->byInBuff_A+iMody);
		}		
	}
	
	for(iY = 0; iY < HEIGHT/2; iY++)		// sort Cr
	{
		for(iX = 0;iX < WIDTH/2; iX++)
		{
			iMody = (iX*4*2)+(iY*2880);
		//	iMody += (iMody/4096)*4096;	
			*(pBuffer->byYUV_In+iX+(WIDTH*HEIGHT*5/4)+(iY*WIDTH/2)) =	*(pBuffer->byInBuff_A+iMody);
		}		
	}
*/
	*pMDMA_S0_CONFIG &= ~DEN;	
	*pMDMA_D0_CONFIG &= ~DEN;

	*pMDMA_S0_PERIPHERAL_MAP = 0x0040;				//Memory DMA
	*pMDMA_S0_CURR_DESC_PTR = usMEMDescriptor1;		//Start Descriptorblock
	// 8 Bit transfer, Descriptorarray, 5 rows
	*pMDMA_S0_CONFIG = SIZE_08|0x4700;
	
	
    if (DMA0_CURR_ADDR >= (BYTE4) pBuffer->byInBuff_B) 
    {
		usMEMDescriptor1[0] = (BYTE2)((BYTE4)(pBuffer->byInBuff_A+17) & 0xFFFF); //Startaddress byHeader
		usMEMDescriptor1[1] = (BYTE2)((BYTE4)(pBuffer->byInBuff_A+17) >> 16);
		usMEMDescriptor1[2] = SIZE_08|DEN|DMA2|0x4700;			//next 7 rows, Enable
		usMEMDescriptor1[3] = COL-16;						//x Cout
		usMEMDescriptor1[4] = 2;							//x Modify
		usMEMDescriptor1[5] = ROW/2;						//y Cout
		usMEMDescriptor1[6] = 34;							//y Modify
		
		usMEMDescriptor1[7] = (BYTE2)((BYTE4)(pBuffer->byInBuff_A+16) & 0xFFFF); //Startaddress byHeader
		usMEMDescriptor1[8] = (BYTE2)((BYTE4)(pBuffer->byInBuff_A+16) >> 16);
		usMEMDescriptor1[9] = SIZE_08|DEN|DMA2|0x4700;		//next 7 rows, Enable
		usMEMDescriptor1[10] = (COL-16)/2;						//x Cout
		usMEMDescriptor1[11] = 4;							//x Modify
		usMEMDescriptor1[12] = ROW/4;
		usMEMDescriptor1[13] = 1476;
		
		usMEMDescriptor1[14] = (BYTE2)((BYTE4)(pBuffer->byInBuff_A+18) & 0xFFFF); //Startaddress byHeader
		usMEMDescriptor1[15] = (BYTE2)((BYTE4)(pBuffer->byInBuff_A+18) >> 16);
		usMEMDescriptor1[16] = SIZE_08|DEN|DMA2;			//next 7 rows, Enable
		usMEMDescriptor1[17] = (COL-16)/2;						//x Cout
		usMEMDescriptor1[18] = 4;							//x Modify
		usMEMDescriptor1[19] = ROW/4;
		usMEMDescriptor1[20] = 1476;
    }
     else
    {
		usMEMDescriptor1[0] = (BYTE2)((BYTE4)(pBuffer->byInBuff_B+17) & 0xFFFF); //Startaddress byHeader
		usMEMDescriptor1[1] = (BYTE2)((BYTE4)(pBuffer->byInBuff_B+17) >> 16);
		usMEMDescriptor1[2] = SIZE_08|DEN|DMA2|0x4700;			//next 7 rows, Enable
		usMEMDescriptor1[3] = COL-16;						//x Cout
		usMEMDescriptor1[4] = 2;							//x Modify
		usMEMDescriptor1[5] = ROW/2;						//y Cout
		usMEMDescriptor1[6] = 34;							//y Modify
		
		usMEMDescriptor1[7] = (BYTE2)((BYTE4)(pBuffer->byInBuff_B+16) & 0xFFFF); //Startaddress byHeader
		usMEMDescriptor1[8] = (BYTE2)((BYTE4)(pBuffer->byInBuff_B+16) >> 16);
		usMEMDescriptor1[9] = SIZE_08|DEN|DMA2|0x4700;		//next 7 rows, Enable
		usMEMDescriptor1[10] = (COL-16)/2;						//x Cout
		usMEMDescriptor1[11] = 4;							//x Modify
		usMEMDescriptor1[12] = ROW/4;
		usMEMDescriptor1[13] = 1474;
		
		usMEMDescriptor1[14] = (BYTE2)((BYTE4)(pBuffer->byInBuff_B+18) & 0xFFFF); //Startaddress byHeader
		usMEMDescriptor1[15] = (BYTE2)((BYTE4)(pBuffer->byInBuff_B+18) >> 16);
		usMEMDescriptor1[16] = SIZE_08|DEN|DMA2;			//next 7 rows, Enable
		usMEMDescriptor1[17] = (COL-16)/2;						//x Cout
		usMEMDescriptor1[18] = 4;							//x Modify
		usMEMDescriptor1[19] = ROW/4;
		usMEMDescriptor1[20] = 1474;
    }
    
	*pMDMA_D0_PERIPHERAL_MAP = 0x0040;					//direction SPORT
	*pMDMA_D0_CURR_DESC_PTR = usMEMDescriptor2;			//Start Descriptorblock
	// 8 Bit transfer, Descriptorarray, 5 rows
	*pMDMA_D0_CONFIG = SIZE_08|0x4700;
    
	usMEMDescriptor2[0] = (BYTE2)((BYTE4)(pBuffer->byYUV_In) & 0xFFFF); //Startaddress byHeader
	usMEMDescriptor2[1] = (BYTE2)((BYTE4)(pBuffer->byYUV_In) >> 16);
	usMEMDescriptor2[2] = SIZE_08|MEMWR|DEN|DMA2|0x4500;		//next 7 rows, Enable
	usMEMDescriptor2[3] = (FRAME-9216)/8;						//x Cout
	usMEMDescriptor2[4] = 1;							//x Modify
	usMEMDescriptor2[5] = 4;							//y Cout
	usMEMDescriptor2[6] = 1;							//y Modify
		
	usMEMDescriptor2[7] = (BYTE2)((BYTE4)(pBuffer->byYUV_In+(FRAME-9216)/2) & 0xFFFF); //Startaddress byHeader
	usMEMDescriptor2[8] = (BYTE2)((BYTE4)(pBuffer->byYUV_In+(FRAME-9216)/2) >> 16);
	usMEMDescriptor2[9] = SIZE_08|MEMWR|DEN|0x4500;			//next 7 rows, Enable
	usMEMDescriptor2[10] = (FRAME-9216)/8;						//x Cout
	usMEMDescriptor2[11] = 1;							//x Modify

	
	usMEMDescriptor2[12] = (BYTE2)((BYTE4)(pBuffer->byYUV_In+5*(FRAME-9216)/8) & 0xFFFF); //Startaddress byHeader
	usMEMDescriptor2[13] = (BYTE2)((BYTE4)(pBuffer->byYUV_In+5*(FRAME-9216)/8) >> 16);
	usMEMDescriptor2[14] = SIZE_08|MEMWR|DEN;					//next 7 rows, Enable
	usMEMDescriptor2[15] = (FRAME-9216)/8;						//x Cout
	usMEMDescriptor2[16] = 1;							//x Modify

    
	*pMDMA_S0_CONFIG |= DEN;	
	*pMDMA_D0_CONFIG |= DEN;
	asm("ssync;");	
	
	while((*pMDMA_D0_IRQ_STATUS & 0x8) || (i == 1000)) 
	{
		wait(1);
		i++;
	}
	
//	wait(1000);
	*pMDMA_D0_CONFIG = (*pMDMA_D0_CONFIG & 0xFFFE);	// Disable DMA
	*pMDMA_S0_CONFIG = (*pMDMA_S0_CONFIG & 0xFFFE);	// Disable DMA	

	return(true);

}

bool DoCoding()
{
	bool bresult = false;
	asm volatile( ".align 4;" );
	startc();
	gEzdMP4SPEnc_GetParam(lBaseVideoEnc,GET_MAX_FRAME_SIZE,&lOutputBufferSize) ;
	lNumBytes = lOutputBufferSize;
	byMod = 0;
	if(bMod)
		byMod = 28;
	nChecksum = 0;	
	switch (gEzdMP4SPEnc_EncodeVideoFrame(lBaseVideoEnc ,pBuffer->byYUV_In,
		   (pBuffer->byYUV_Out+byMod),&lNumBytes,&lVOPType,lPacketSizes,
			nFrameC))
	{
		case E_SUCCESS:
			bMod = true;
			bresult = true;		
//			sprintf(pPacket->Debug.sText, "Slave: %lu Codec Encode: Success! No: %lu Laenge: %lu  FrameType %lu Time(us): %lu \n",pPacket->wSource,nFrameC, lNumBytes, lVOPType, (getCyclesLo()/(CCLK/1000000)));
//			DoTrace(pPacket->Debug.sText);
			break;
		case E_INVALID_ARGS:
			DoTrace("Codec Encode: Invalid Args!\n");
			bresult= false;
			break;
		case E_OUTBUF_OVERFLOW:
			DoTrace("Codec Encode: Outbuf overflow!\n");
			bresult= false;
			break;
	}
	switch (lVOPType)
	{
		case VENC_IVOP: 		
			usDescriptor1[5] = (BYTE2)((BYTE4)pBuffer->byYUV_Out & 0xFFFF);
			usDescriptor1[6] = (BYTE2)((BYTE4)pBuffer->byYUV_Out >> 16);
			pPacket->dwImageDataLen	= lNumBytes+byMod;
			docheck((DWORD) pBuffer->byYUV_Out,(uint32) lNumBytes+byMod);
			stopc();
			break;
		case VENC_PVOP:
			usDescriptor1[5] = (BYTE2)((BYTE4)(pBuffer->byYUV_Out+byMod) & 0xFFFF);
			usDescriptor1[6] = (BYTE2)((BYTE4)(pBuffer->byYUV_Out+byMod) >> 16);
			pPacket->dwImageDataLen	= lNumBytes;
			docheck((DWORD) (pBuffer->byYUV_Out+byMod),(uint32) lNumBytes);
			stopc();
			break;
	}
	pPacket->eSubType 	= (DataSubType) lVOPType;
	pPacket->dwCheckSum = nChecksum;
	pPacket->dwFieldID 	= nFrameC++; 				// Framezahl im Header hinterlegen
	return(bresult);
}


