/********************************************************************/
/* Unterprogramme 													*/
/********************************************************************/
/* Autor: Christian Lohmeier										*/
/* Datum: 26.08.2004												*/
/* Firma: Videte IT	AG												*/
/********************************************************************/
#include "main.h"
#include "sport.h"

//--------------------------------------------------------------------------//
// Funktion: blocktransfer_start				                            //
// Beschreibung: Startet den DMA für Videobuffer A und B	                //
//--------------------------------------------------------------------------//
void blocktransfer_start()
{
	bPush = true;
	*pMDMA_S0_CONFIG |= DEN;	
	*pMDMA_D0_CONFIG |= DEN;
	return;
}
/////////////////////////////////////////////////////////////////////////////

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
     
    if (byLineC == (dwFrameSize[bySetNorm]/(2*COL))/BLOCK) 	// Wenn Differenzbild vollständig (nach 36 Zeilen)
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
			for(BYTE byClear=0;byClear<5;byClear++)
			{
				pPacket->Point[byClear].nValue 	= 0;
				pPacket->Point[byClear].cx 		= 0;	
	    		pPacket->Point[byClear].cy 		= 0;
			}
    	}		 
    	bLed = false;						// Alarm löschen					
    	byLineC = 0;						// Zeilenzähler auf Null zurücksetzen
    	pInBuffA = pBuffer->byInBuff_A+1;	// Zeiger auf Eingangsbuffer A
    	pInBuffB = pBuffer->byInBuff_B+1;	// Zeiger auf Eingangsbuffer B
    	pFrameBuff_C = byFrameBuff_C;      	// Zeiger auf Differenzbildspeicher 
    	bPush = false;
    	IniMemDMA(); 
    	bMd = false;		
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
/////////////////////////////////////////////////////////////////////////////

void copy(BYTE2 *pDest, BYTE2 *pSource, BYTE2 nLength)	// Kopierfunktion
{
	BYTE2 nmax;
	for (nmax = 0; nmax < nLength/2; nmax++)
	{
		*(pDest+nmax) = *(pSource+nmax);
	}
}
/////////////////////////////////////////////////////////////////////////////

void copyMask()
{
 	pPacket->AdaptiveMask.bValid = (TRUE);										// adaptive Maske vorhanden
	copy((BYTE2*)(pPacket->AdaptiveMask.byMask), (BYTE2*)(byMask_A), 45*36);	// in Header copieren
	pPacket->PermanentMask.bValid = (TRUE);										// permanente Maske vorhanden
	copy((BYTE2*)(pPacket->PermanentMask.byMask), (BYTE2*)(byMask_B), 45*36);	// in Header copieren
	pPacket->Diff.bValid = (TRUE);												// Differenzbild vorhanden
	copy((BYTE2*)(pPacket->Diff.byMask), (BYTE2*)(byFrameBuff_C), 45*36);		// in Header copieren
}
/////////////////////////////////////////////////////////////////////////////

void PermanentMaskFlagReset()
{
	if (nFrameC > nSaveFrameC)
	{	
		pPacket->PermanentMask.bHasChanged = FALSE; 
		nSaveFrameC = 0;
	}
}
/////////////////////////////////////////////////////////////////////////////

