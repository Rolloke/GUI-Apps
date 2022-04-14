/********************************************************************/
/* Unterprogramme 													*/
/********************************************************************/
/* Autor: Christian Lohmeier										*/
/* Datum: 18.06.2003												*/
/* Firma: Videte IT	AG												*/
/********************************************************************/
#include "main.h"


//--------------------------------------------------------------------------//
// Funktion: blocktransfer_start				                            //
// Beschreibung: Startet den DMA für Videobuffer A und B	                //
//--------------------------------------------------------------------------//
void blocktransfer_start()
{   
	*pMDMA_S0_CONFIG	= (*pMDMA_S0_CONFIG | DEN);	
	*pMDMA_D0_CONFIG	= (*pMDMA_D0_CONFIG | DEN);
	
	*pMDMA_S1_CONFIG	= (*pMDMA_S1_CONFIG | DEN);	
	*pMDMA_D1_CONFIG	= (*pMDMA_D1_CONFIG | DEN);
	return;
}

void copy(BYTE2 *pDest, BYTE2 *pSource, BYTE2 nLength);
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
	   		pPacket->Point[0].nValue 		= SwapEndian((BYTE4)byMax);
    	byMax = 0;			    // Maximalwert löschen
	   	max_search(OFF+2);	    // 2. Maximum suchen und Koordinaten in den Header
	   	if(bLed)
	   		pPacket->Point[1].nValue 		= SwapEndian((BYTE4)byMax);
	   	byMax = 0;			    // Maximalwert löschen
	   	max_search(OFF+4);		// 3. Maximum suchen und Koordinaten in den Header
	   	if(bLed)
	   		pPacket->Point[2].nValue 		= SwapEndian((BYTE4)byMax);
	   	byMax = 0;			    // Maximalwert löschen
	   	max_search(OFF+6);		// 4. Maximum suchen und Koordinaten in den Header
	   	if(bLed)
	   		pPacket->Point[3].nValue 		= SwapEndian((BYTE4)byMax);
	   	byMax = 0;			    // Maximalwert löschen
	   	max_search(OFF+8);		// 5. Maximum suchen und Koordinaten in den Header
	   	if(bLed)
	   		pPacket->Point[4].nValue 		= SwapEndian((BYTE4)byMax);
	   	byMax = 0;			    // Maximalwert löschen
    
    	if (bLed) 
    	{	
    		byCrossDis = 0;	
       		setled(32);		// Bei Alarm LED1 setzen
    		pPacket->bMotion			= SwapEndian(TRUE);
    		pPacket->Point[0].cx 		= SwapEndianSmal(((*(pHeader+OFF+0))*1000/44));	
    		pPacket->Point[0].cy 		= SwapEndianSmal(((*(pHeader+OFF+1))*1000/35));	
    		pPacket->Point[1].cx 		= SwapEndianSmal(((*(pHeader+OFF+2))*1000/44));	
    		pPacket->Point[1].cy 		= SwapEndianSmal(((*(pHeader+OFF+3))*1000/35));	
    		pPacket->Point[2].cx 		= SwapEndianSmal(((*(pHeader+OFF+4))*1000/44));	
    		pPacket->Point[2].cy 		= SwapEndianSmal(((*(pHeader+OFF+5))*1000/35));	
    		pPacket->Point[3].cx 		= SwapEndianSmal(((*(pHeader+OFF+6))*1000/44));	
    		pPacket->Point[3].cy 		= SwapEndianSmal(((*(pHeader+OFF+7))*1000/35));	
    		pPacket->Point[4].cx 		= SwapEndianSmal(((*(pHeader+OFF+8))*1000/44));	
    		pPacket->Point[4].cy 		= SwapEndianSmal(((*(pHeader+OFF+9))*1000/35));	
    	} 	
    	if (!bLed)
    	{
    		byCrossDis++;
    		clearled(32); 		// Wenn kein Alarm LED1 löschen   	
    		if (byCrossDis == 25)
				pPacket->Point[0].nValue 		= 0;
    		
			if (byCrossDis == 50)
    		{
    			pPacket->bMotion			= SwapEndian(FALSE);
    			pPacket->Point[0].nValue 		= 0;
    			pPacket->Point[1].nValue 		= 0;
    			pPacket->Point[2].nValue 		= 0;
    			pPacket->Point[3].nValue 		= 0;
    			pPacket->Point[4].nValue 		= 0;
    			byCrossDis = 0;	
    		}
    	}
 		pPacket->AdaptiveMask.bValid = SwapEndian(TRUE);
 		copy((BYTE2*)(pPacket->AdaptiveMask.byMask), (BYTE2*)(byMask_A), 45*36);
 		pPacket->PermanentMask.bValid = SwapEndian(TRUE);
 		copy((BYTE2*)(pPacket->PermanentMask.byMask), (BYTE2*)(byMask_B), 45*36);
		pPacket->Diff.bValid = SwapEndian(TRUE);
		copy((BYTE2*)(pPacket->Diff.byMask), (BYTE2*)(byFrameBuff_C), 45*36);
		pPacket->dwFieldID = SwapEndian(nFrameC); // Framezahl im Header hinterlegen
			 
    	bLed = false;			// Alarm löschen					
    	byLineC = 0;			// Zeilenzähler auf Null zurücksetzen
    	pInBuffA = 0;			// Zeiger auf Eingangsbuffer A
    	pInBuffB = 0;			// Zeiger auf Eingangsbuffer B
    	pInBuffA += 1;			// Auf den ersten Y Wert InBuffA
    	pInBuffB += (FRAME+1);	// Auf den ersten Y Wert InBuffB
    	pFrameBuff_C = byFrameBuff_C;      // Zeiger auf Differenzbildspeicher       
    	*pMDMA_S0_START_ADDR = pInBuffA;   // DMA neu laden
		*pMDMA_D0_START_ADDR = pFrameBuff_A;
		*pMDMA_S1_START_ADDR = pInBuffB;
		*pMDMA_D1_START_ADDR = pFrameBuff_B;  			             
    
    	return;  			  // zurück zum Aufruf
   	 }
   	*pMDMA_S0_START_ADDR = pInBuffA;	   // DMA neu Laden
	*pMDMA_D0_START_ADDR = pFrameBuff_A;
	*pMDMA_S1_START_ADDR = pInBuffB;
	*pMDMA_D1_START_ADDR = pFrameBuff_B;
	blocktransfer_start();				  // DMA starten 
	return;						          // zurück zum Aufruf
}

//-----------------------------------------------------------------------------------------------//
// Funktion: Abbruch					 		                                                 //
// Beschreibung: Schließt den DMA Transfer und terminiert das Programm 	                         //	
//-----------------------------------------------------------------------------------------------//
// diese Funktionalität entfällt (-:

//--------------------------------------------------------------------------//
// Funktion: sport_en					                                    //
// Beschreibung: CLOCK starten DMA und SPORT Enable	                        //	
//--------------------------------------------------------------------------//
void sport_en()
{
	bSTrans = true;						// SPORT Transfer hat begonnen

	*pDMA4_CONFIG	= (*pDMA4_CONFIG | DEN);	// DMA anschalten
	*pSPORT0_TCR1	= (*pSPORT0_TCR1 | DEN);	// SPORT Port starten
	*pSPORT0_RCR1	= (*pSPORT0_RCR1 | DEN);
	return;
}

void CreateImageHeader(BYTE2* pBuffer, int nHSize, int nVSize, int nChannel)
{
	pPacket = (DATA_PACKET*)pBuffer;
	

	pPacket->dwStartMarker		= SwapEndian(MAGIC_MARKER_START);
	
	pPacket->bValid				= SwapEndian(TRUE);
	pPacket->wSize				= SwapEndianSmal(sizeof(DATA_PACKET));
	pPacket->eType				= (DataType)SwapEndian(eTypeYUV422);
	pPacket->eSubType			= (DataSubType)SwapEndian(eSubTypeUnvalid);
	pPacket->wSource			= SwapEndianSmal(nChannel+4);
	pPacket->wSizeH				= SwapEndianSmal(nHSize);
	pPacket->wSizeV				= SwapEndianSmal(nVSize);
	pPacket->wBytesPerPixel		= SwapEndianSmal(2);
	pPacket->wField				= SwapEndianSmal(1);
	pPacket->bVidPresent		= SwapEndian(TRUE);
	pPacket->dwFieldID			= SwapEndian(123);
	pPacket->dwProzessID		= SwapEndian(0);
	pPacket->dwImageDataLen		= SwapEndian(nHSize*nVSize*SwapEndianSmal(pPacket->wBytesPerPixel));	
	pPacket->bMotion			= SwapEndian(FALSE);
	pPacket->Point[0].cx		= (WORD)SwapEndianSmal(10);
	pPacket->Point[0].cy		= (WORD)SwapEndianSmal(15);
	pPacket->Point[0].nValue	= SwapEndian(100);
	pPacket->Point[1].cx		= (WORD)SwapEndianSmal(0);
	pPacket->Point[1].cy		= (WORD)SwapEndianSmal(0);
	pPacket->Point[1].nValue	= SwapEndian(200);
	pPacket->Point[2].cx		= (WORD)SwapEndianSmal(0);
	pPacket->Point[2].cy		= (WORD)SwapEndianSmal(0);
	pPacket->Point[2].nValue	= SwapEndian(300);
	pPacket->Point[3].cx		= (WORD)SwapEndianSmal(0);
	pPacket->Point[3].cy		= (WORD)SwapEndianSmal(0);
	pPacket->Point[3].nValue	= SwapEndian(400);
	pPacket->Point[4].cx		= (WORD)SwapEndianSmal(0);
	pPacket->Point[4].cy		= (WORD)SwapEndianSmal(0);
	pPacket->Point[4].nValue	= SwapEndian(500);
	pPacket->TimeStamp.bValid 	= SwapEndian(FALSE);
	pPacket->TimeStamp.wYear	= (WORD)SwapEndianSmal(2003);
	pPacket->TimeStamp.wMonth	= SwapEndianSmal(8);
	pPacket->TimeStamp.wDay		= SwapEndianSmal(4);
	pPacket->TimeStamp.wHour	= SwapEndianSmal(13);
	pPacket->TimeStamp.wMinute	= SwapEndianSmal(3);
	pPacket->TimeStamp.wSecond	= SwapEndianSmal(34);
	pPacket->TimeStamp.wMSecond	= SwapEndianSmal(333);
	pPacket->AdaptiveMask.bHasChanged = SwapEndian(FALSE);
	pPacket->AdaptiveMask.nDimH	= SwapEndian(45);
	pPacket->AdaptiveMask.nDimV	= SwapEndian(36);
	pPacket->PermanentMask.bHasChanged = SwapEndian(FALSE);
	pPacket->PermanentMask.nDimH= SwapEndian(45);
	pPacket->PermanentMask.nDimV= SwapEndian(36);
	pPacket->Diff.nDimH			= SwapEndian(45);
	pPacket->Diff.nDimV			= SwapEndian(36);
	pPacket->dwEndMarker		= SwapEndian(MAGIC_MARKER_END);
	

	return;
}

void copy(BYTE2 *pDest, BYTE2 *pSource, BYTE2 nLength)	// SwapEndian Kopierfunktion
{
	BYTE2 nmax;
	for (nmax = 0; nmax < nLength/2; nmax++)
	{
		*(pDest+nmax) = (SwapEndianSmal(*(pSource+nmax)));
	}
}

bool DoTrace(char *pString)
{
	if (pPacket->Debug.bValid == SwapEndian(FALSE))
	{
		pPacket->Debug.bValid = SwapEndian(TRUE);
    	copy((BYTE2*) pPacket->Debug.sText, (BYTE2*) pString, MAX_DEBUG_LEN);
    	return true;
	}
	else
	{
		return false;
	}
}
