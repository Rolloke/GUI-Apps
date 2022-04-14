/********************************************************************/
/* ISR Interrupt Service Routine									*/
/********************************************************************/
/* Autor: Christian Lohmeier										*/
/* Datum: 18.06.2003												*/
/* Firma: Videte IT	AG												*/
/********************************************************************/
#include "main.h"




//------------------------------------------------------------------------------//
// SPORT_ISR						                                            //
// Beschreibung: Wird zyclisch nach jedem Halbbild aufgerufen                   //
//------------------------------------------------------------------------------//
EX_INTERRUPT_HANDLER(SPORT_ISR)
{
	*pDMA4_IRQ_STATUS = 0x0001;	// Löscht Interrupt
    

}

