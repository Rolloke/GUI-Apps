/********************************************************************/
/* Main Header (Definitionen)													*/
/********************************************************************/
/* Autor: Christian Lohmeier										*/
/* Datum: 18.06.2003												*/
/* Firma: Videte IT	AG												*/
/********************************************************************/
#ifndef  __MAIN_DEFINED
	#define __MAIN_DEFINED

#include "..\Include\helper.h"
#include "..\..\TashaUnit\TashaUnitDefs.h"

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

//--------------------------------------------------------------------------//
// Globale Variablen																//
//--------------------------------------------------------------------------//

extern BYTE *pInBuffA,*pInBuffB;
extern BYTE2 usDescriptor1[];

//--------------------------------------------------------------------------//
// Prototypes																//
//--------------------------------------------------------------------------//

// in file led.c
void led(BYTE byLed);
bool pftest(BYTE2 usChoose);

// in file i2c_function.c
void i2c_ini(void);
void i2c_start(void);
void i2c_stop(void);

void i2c_putbyte(BYTE byWort);
BYTE i2c_getbyte(void);

void i2c_nack();
void i2c_ack();
bool i2c_gack();

void wait();

bool i2c_write(BYTE bySlAdr, BYTE bySuAdr, BYTE byDataW);
bool i2c_Read(BYTE bySlAdr, BYTE bySuAdr, BYTE *pbyDataR);

// counter.asm
//void startc();
//void stopc();



// in file ISR.c

EX_INTERRUPT_HANDLER(SPORT_ISR);
#endif __MAIN_DEFINED
