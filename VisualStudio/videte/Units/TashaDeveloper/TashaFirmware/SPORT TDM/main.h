/********************************************************************/
/* Main Header (Definitionen)													*/
/********************************************************************/
/* Autor: Christian Lohmeier										*/
/* Datum: 18.06.2003												*/
/* Firma: Videte IT	AG												*/
/********************************************************************/
#ifndef  __MAIN_DEFINED
	#define __MAIN_DEFINED

//--------------------------------------------------------------------------//
// Header files																//
//--------------------------------------------------------------------------//
#include <sys\exception.h>
#include <cdefBF533.h>
#include "definitionen.h"

//--------------------------------------------------------------------------//
// Symbolic constants														//
//--------------------------------------------------------------------------//
// addresses for Port B in Flash A
#define pFlashA_PortA_Dir	(volatile unsigned char *)0x20270006
#define pFlashA_PortA_Data	(volatile unsigned char *)0x20270004
#define pVR_CTL ((volatile unsigned short *)VR_CTL)

#include "..\Include\helper.h"
#include "..\..\TashaUnit\TashaUnitDefs.h"

//--------------------------------------------------------------------------//
// Globale Variablen																//
//--------------------------------------------------------------------------//

extern BYTE *pInBuffA,*pInBuffB;


//--------------------------------------------------------------------------//
// Prototypes																//
//--------------------------------------------------------------------------//

// in file led.c
void led(BYTE byLed);
bool pftest(BYTE2 usChoose);



// counter.asm
void startc();
void stopc();



// in file ISR.c

EX_INTERRUPT_HANDLER(SPORT_ISR);
#endif __MAIN_DEFINED

