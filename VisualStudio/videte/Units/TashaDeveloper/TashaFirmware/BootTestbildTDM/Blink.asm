/******************************************************************************

(C) Copyright 2003 - Analog Devices, Inc.  All rights reserved.

File Name:		Blink.asm

Date Modified:	4/11/03		Rev 1.0
				
Purpose:	   	This blink example is being used to explain how to create a
				loader file.  See the Readme file (LoaderFile.txt) for help.	
				
*******************************************************************************/

#include <defBF533.h>

#define flashA_csio 	0x20270000	// base addr
#define portA_data_out 	0x04		// offset
#define portB_data_out 	0x05		// offset
#define portA_data_dir 	0x06		// offset
#define portB_data_dir 	0x07		// offset

#define delay 			0x1ffffff	// loop delay
.extern _submain;
.section program;

START:

// read AMS global control register
P0.h = hi( EBIU_AMGCTL );
P0.l = lo( EBIU_AMGCTL );
R0.l = w[P0];

// enable all AMS banks
R1 = 0xE;
R0 = R0 | R1;
w[p0] = R0.l;

// set all flags as outputs
P1.h = hi( flashA_csio );
P1.l = lo( flashA_csio ) + portB_data_dir;
R0 = 0xFF;
b[P1] = R0;

// initialize loop counter
p2.l = lo(delay);
p2.h = hi(delay);
/*
clk:

	p0.h = hi(PLL_CTL);	// Laden der oberen 16 Bit des PPL Registers
	p0.l = lo(PLL_CTL);	// Laden der unteren 16 Bit des PPL Registers

	CLI R2; 			// Interrupts aus, IMASK in r2 kopieren
	R1.L=0x2800; 		// VCO Faktor auf 21x 27MHz
	W[P0]=R1;			//  durch schreiben in PLL_CTL Register
	IDLE; 				// Wartet bis die Pipeline frei ist und PPL Wake Up eintrifft
	STI R2;				// Stellt die alte IMASK wieder her und Interrupts werden enabled
clk.end:
*/	

// turn on LEDs
P1.h = hi( flashA_csio );
P1.l = lo( flashA_csio ) + portB_data_out;
R0 = 0x55;

blink_loop:
call _submain;
jump blink_loop;

// end label
START.END:
