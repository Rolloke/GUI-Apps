/********************************************************************/
/* LED Treiber										    			*/
/********************************************************************/
/* Autor: Christian Lohmeier										*/
/* Datum: 18.06.2003												*/
/* Firma: Videte IT AG												*/
/********************************************************************/
#include "led.h"

//--------------------------------------------------------------------------//
// Funktion: led															//
//																			//
// Beschreibung: LED 0-6 setzen 											//
//				 led(0x3F) Setzt alle 6 LEDs, led(0) löscht alle LEDs		//
//																			//
//--------------------------------------------------------------------------//
void led(BYTE byLed)	
{
	*pFlashA_PortB_Dir = 0x3F;	    // LED Port als Ausgang setzen
	*pFlashA_PortB_Data = byLed;	// LED setzen oder rücksetzen
	return;				            // Zurück zum Aufruf
}

//--------------------------------------------------------------------------//
// Funktion: pftest					                                        //
// Beschreibung: Püft ob Tatser gedrückt 			                        //	
//--------------------------------------------------------------------------//
bool pftest(BYTE2 usChoose)
{
	BYTE2 usTest;
	*pFIO_INEN = 0x0f00;		//workaround: schreiben in FIO_DIR 
	*pFIO_POLAR = 0x0000;   	//            verfälscht andere PF's
	*pFIO_EDGE = 0x0000;		//----------------------------------
	*pFIO_DIR = *pFIO_DIR & 0xF0FF;	
	usTest = *pFIO_FLAG_D;
	if(usChoose == (usTest & usChoose)) return(true);
	return(false);
}

