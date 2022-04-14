/********************************************************************/
/* LED Header														*/
/********************************************************************/
/* Autor: Christian Lohmeier										*/
/* Datum: 18.06.2003												*/
/* Firma: Videte IT	AG												*/
/********************************************************************/
#ifndef  __LED_DEFINED
#define __LED_DEFINED

#include "main.h"
//--------------------------------------------------------------------------//
// Symbolic constants					                                    //
//--------------------------------------------------------------------------//
// addresses for Port B in Flash A   Data(0x00->0x3F) Led4->Led9
#define pFlashA_PortB_Dir	    (volatile unsigned char *)0x20270007
#define pFlashA_PortB_DataRW	(volatile unsigned char *)0x20270005
#define pFlashA_PortB_DataR	    (volatile unsigned char *)0x20270001
#endif //__#endif __LED_DEFINED

