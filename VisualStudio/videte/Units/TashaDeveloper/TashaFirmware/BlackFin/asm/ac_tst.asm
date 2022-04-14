/*******************************************************************************
 *
 * $Source$
 * $Revision$
 *
 * Audio codec test program.
 *
 * $Log$
 *
 ******************************************************************************/

#include	<def21535.h>
#include	"ac_init.h"

//..............................................................................

	.section    data1;

	.global	_main;
	.extern setUpIVG8Handler;
	.extern codecReset;
	.extern	initSPORT0;
	.extern	initSPORT0DMA;
	.extern	initCodec;
	.extern	rxStatus;
#ifdef	GEN_SINE
	.extern initLookUp;
#endif

//------------------------------------------------------------------------------

	.section program;
	.align 2;

_main:


#ifdef	GEN_SINE
	call initLookUp;	// create sine look-up table	
#endif

	call setUpIVG8Handler;

   	P0.L = IMASK & 0xffff;
	P0.H = IMASK >> 16;

	// enable IVG8 in IMASK

	R0 =  [P0];  	
	BITSET(R0,8);
	W[ P0 ] = R0;   

	call codecReset;	
	call initSPORT0;	// Initialize SPORT0 for codec communications 
	call initSPORT0DMA;	// Start Serial Port 0 tx and rx DMA Transfers 
	call initCodec;		// Initialize & program AD1885 

	// set rxStatus audio data flag indicating incoming RX data is now audio 
	// data and can be processed accordingly 

	p0.l = rxStatus;
	p0.h = rxStatus;
	r0 = 0x0000 (z);
	bitset(r0,bAudioData);
	w[p0] = r0.l;

waitForever:
    	jump waitForever;







