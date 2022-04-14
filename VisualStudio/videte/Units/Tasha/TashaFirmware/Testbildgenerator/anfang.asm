/******************************************************************************

(C) Copyright 2003 - Analog Devices, Inc.  All rights reserved.

File Name:		Blink.asm

Date Modified:	4/11/03		Rev 1.0
				
Purpose:	   	This blink example is being used to explain how to create a
				loader file.  See the Readme file (LoaderFile.txt) for help.	
				
*******************************************************************************/

#include <defBF533.h>

.extern _submain;
.section program;

_start:

	p0.h = hi(PLL_CTL);	// Laden der oberen 16 Bit des PPL Registers
	p0.l = lo(PLL_CTL);	// Laden der unteren 16 Bit des PPL Registers

	CLI R2; 			// Interrupts aus, IMASK in r2 kopieren
	R1.L=0x2800; 		// VCO Faktor auf 21x 27MHz
	W[P0]=R1;			//  durch schreiben in PLL_CTL Register
	IDLE; 				// Wartet bis die Pipeline frei ist und PPL Wake Up eintrifft
	STI R2;				// Stellt die alte IMASK wieder her und Interrupts werden enabled

blink_loop:
call _submain;
jump blink_loop;

// end label
_start.end:




