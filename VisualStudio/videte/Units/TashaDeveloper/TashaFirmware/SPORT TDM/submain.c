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
    	adaptiv_mask();	       	// adaptive Maske erstellen	
    	max_search(OFF);        // 1. Maximum suchen und Koordinaten in den Header 
	   	byMax = 0;			    // Maximalwert löschen
	   	max_search(OFF+2);	    // 2. Maximum suchen und Koordinaten in den Header
	   	byMax = 0;			    // Maximalwert löschen
	   	max_search(OFF+4);		// 3. Maximum suchen und Koordinaten in den Header
	   	byMax = 0;			    // Maximalwert löschen
	   	max_search(OFF+6);		// 4. Maximum suchen und Koordinaten in den Header
	   	byMax = 0;			    // Maximalwert löschen
	   	max_search(OFF+8);		// 5. Maximum suchen und Koordinaten in den Header
	   	byMax = 0;			    // Maximalwert löschen
    
    	if (bLed) led(1);		// Bei Alarm LED1 setzen
    	if (!bLed)led(0); 		// Wenn kein Alarm LED1 löschen   	
    	if (!bSTrans && bLed) sport_en(); // Beim ersten Alarm SPORT einschalten 
   	    bLed = false;			// Alarm löschen			
    	framec(nFrameC);		// Framezahl im Header hinterlegen
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
    	return;    			               // zurück zum Aufruf
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
void abbruch()
{
	*pDMA0_CONFIG	= (*pDMA0_CONFIG & 0xFFFE);	     // Alle DMAs ausschalten
	*pMDMA_D1_CONFIG = (*pMDMA_D1_CONFIG & 0xFFFE);
	*pMDMA_S1_CONFIG = (*pMDMA_S1_CONFIG & 0xFFFE);
	*pMDMA_D0_CONFIG = (*pMDMA_D1_CONFIG & 0xFFFE);
	*pMDMA_S0_CONFIG = (*pMDMA_S1_CONFIG & 0xFFFE);
	*pDMA2_CONFIG = (*pDMA2_CONFIG & 0xFFFE);			
	*pPPI_CONTROL 	= (*pPPI_CONTROL & 0xFFFE);	   // PPI ausschalten
	*pSPORT0_TCR1 = (*pSPORT0_TCR1 & 0xFFFE); 	   // SPORT ausschalten
	stopc();								       // Counter anhalten
	abort();								       // Programm terminieren
}

//--------------------------------------------------------------------------//
// Funktion: sport_en					                                    //
// Beschreibung: CLOCK starten DMA und SPORT Enable	                        //	
//--------------------------------------------------------------------------//
void sport_en()
{
	bSTrans = true;						// SPORT Transfer hat begonnen

	*pTIMER_ENABLE 		= 0x0003;		// Takt für SPORT beginnen
	
	*pDMA2_CONFIG	= (*pDMA2_CONFIG | DEN);	// DMA anschalten
	*pSPORT0_TCR1	= (*pSPORT0_TCR1 | DEN);	// SPORT Port starten

}



