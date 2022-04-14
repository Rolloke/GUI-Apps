/******************************************************************************

(C) Copyright 2003 - Analog Devices, Inc.  All rights reserved.

File Name:		Blink.asm

Date Modified:	4/11/03		Rev 1.0
				
Purpose:	   	This blink example is being used to explain how to create a
				loader file.  See the Readme file (LoaderFile.txt) for help.	
				
*******************************************************************************/

#include <defBF533.h>

.extern _submain;
.section start;

_start:

/*	p0.h = hi(PLL_CTL);	// Laden der oberen 16 Bit des PPL Registers
	p0.l = lo(PLL_CTL);	// Laden der unteren 16 Bit des PPL Registers
	
	p1.h = hi(VR_CTL);	// Laden der oberen 16 Bit des Volrage Registers
	p1.l = lo(VR_CTL);	// Laden der unteren 16 Bit des Voltage Registers
	
	p2.h = hi(PLL_DIV);	// Laden der oberen 16 Bit des PPL_DIV Registers
	p2.l = lo(PLL_DIV);	// Laden der unteren 16 Bit des PPL_DIV Registers

	CLI R2; 			// Interrupts aus, IMASK in r2 kopieren
	R0.L=0x0004;		// DIV auf /4 stellen
	W[P2]=R0;			// schreiben in PLL_DIV Register
	R1.L=0x00CB;		// auf ca. 1.2V
	W[P1]=R1;			// schreiben in VR_CTL Register
	R3.L=0x2C00; 		// VCO Faktor auf 21x 27MHz
	W[P0]=R3;			// durch schreiben in PLL_CTL Register
	IDLE; 				// Wartet bis die Pipeline frei ist und PPL Wake Up eintrifft
	STI R2;				// Stellt die alte IMASK wieder her und Interrupts werden enabled
*/
blink_loop:
call _submain;
jump blink_loop;

// end label
_start.end:




