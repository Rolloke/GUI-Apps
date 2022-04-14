/*******************************************************************************
 *
 * $Source$
 * $Revision$
 *
 * This program blinks the LEDs in a circle using a delay loop.
 *
 * $Log$
 *
 ******************************************************************************/

#include <def21535.h>

//------------------------------------------------------------------------------

	.section data1;

	.global	_main;

//------------------------------------------------------------------------------

#if defined(_EAGLE_35_) || defined(_HAWK_35_)  // [ _EAGLE_35_ || _HAWK_35

	.section program;
	.align 2;

_main:

    	P0.L = FIO_DIR & 0xFFFF;
	P0.H = FIO_DIR >> 16;

	R0 = W[P0];  		

#if defined(_EAGLE_35_)
	R1.L = 0xff00;
#else      
	R1.L = 0x0f00;       
#endif

	W[P0] = R1;			
    	ssync;
	P1.L = FIO_FLAG_S & 0xFFFF;
	P1.H = FIO_FLAG_S >> 16;
	P2.L = FIO_FLAG_C & 0xFFFF;
	P2.H = FIO_FLAG_C >> 16;
	
	P5.H=0x0100;
	P5.L=0x4600;

#if defined(_EAGLE_35_)
	R2.L = 0xff00;
#else
	R2.L = 0x0f00;       
#endif

LIGHT:
	W[P2] = R2;			// clear all
	SSYNC;
	R1.L = 0x0100;
	W[P1] = R1;
	SSYNC;

	CALL DELAY_LOOP;

	W[P2] = R2;			// clear all
	SSYNC;
	R1.L = 0x0200;
	W[P1] = R1;
	SSYNC;

	CALL DELAY_LOOP;

	W[P2] = R2;			// clear all
	SSYNC;
	R1.L = 0x0400;
	W[P1] = R1;
	SSYNC;
	CALL DELAY_LOOP;

	W[P2] = R2;			// clear all
	SSYNC;
	R1.L = 0x0800;
	W[P1] = R1;
	SSYNC;
	CALL DELAY_LOOP;

#ifdef _EAGLE_35_			// [ _EAGLE_35_

	W[P2] = R2;			// clear all
	SSYNC;
	R1.L = 0x8000;
	W[P1] = R1;
	SSYNC;
	CALL DELAY_LOOP;

	W[P2] = R2;			// clear all
	SSYNC;
	R1.L = 0x4000;
	W[P1] = R1;
	SSYNC;
	CALL DELAY_LOOP;

	W[P2] = R2;			// clear all
	SSYNC;
	R1.L = 0x2000;
	W[P1] = R1;
	SSYNC;
	CALL DELAY_LOOP;

	W[P2] = R2;			// clear all
	SSYNC;
	R1.L = 0x1000;
	W[P1] = R1;
	SSYNC;
	CALL DELAY_LOOP;

#endif					// ] _EAGLE_35_

	JUMP LIGHT;

DELAY_LOOP:        
	LSETUP(L_BEGIN, L_END) LC0 = P5;
L_BEGIN:
L_END: 	NOP;    
	RTS;              

#else                               // ] [ !(_EAGLE_35_ || _HAWK_35)

#error "Board not defined"

#endif                              // ] _EAGLE_35_ || _HAWK_35