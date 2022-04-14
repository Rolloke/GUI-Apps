// PLL initialization code from Blackfin errata list, 8/7/02

#include 	<defBF535.h>
#include    "zet.h"
#include    "m21535.h"
	
//#define kPLLCTL_LO	0x1300	// MSEL = 9X, bypass on
#define kPLLCTL_LO	0x0900	// MSEL = 9X, bypass on
#define kPLLCTL_HI	0x0001	// SSEL = CCLK/2.5

	.section	program; 

	.global		_pllInit;	      
	
				
_pllInit:
	
// Begin Bypass workaround.
// PLL is in Bypass mode when external bypass pin pulled high
// Setup Watchdog Timer to generate wakeup event to bring DSP out of idle
// Set watchdog count value to allow enough time for transition and PLL lock
	r7.l = 0xfff1;
	r7.h = 0x0000;
	p0.l = WDOGCNT & 0xffff;
	p0.h = WDOGCNT >> 16;
	[p0] = r7;
	ssync;
// Program the PLLCTL register to multiply core and system clock frequency by
// setting MSEL and SSEL bits. In this example, MSEL=15x and SSEL=CCLK/2.5.
// This can be changed to another core and system frequency of choice.
	r7.l = kPLLCTL_LO; 
	r7.h = kPLLCTL_HI; 
	p0.l = PLLCTL & 0xffff;
	p0.h = PLLCTL >> 16;
	[p0] = r7;
	ssync;
//After desired MSEL and SSEL frequency ratios are programmed, turn bypass mode
// off using the following code sequence.
	p0.l = PLLCTL & 0xffff;
	p0.h = PLLCTL >> 16;
	r7 = [p0]; // Read the value in PLLCTL
	bitclr(r7,8); // Set bypass mode off
	[p0] = r7; // set the PLLCTL to: PLL not bypassed
	ssync;
// The following code sequence reloads the Watchdog counter.
	r7.l = 0x0000;
	r7.h = 0x0000;
	p0.l = WDOGSTAT & 0xffff;
	p0.h = WDOGSTAT >> 16;
	[p0] = r7;
	ssync;
// This enables the watchdog GP interrupt.
	r7.l = 0x0004;
	p0.l = WDOGCTL & 0xffff;
	p0.h = WDOGCTL >> 16;
	W[p0] = r7;
	ssync;
// Enter IDLE mode to allow PLL to changed to programmed frequency.
// The programmed watchdog wakeup event will bring the DSP out of IDLE.
	cli r7;
	idle;
	ssync;
	sti r7;
// Clear WD flag and disable WD events after DSP comes of IDLE in new frequency.
	r7.l = 0x8006;
	p0.l = WDOGCTL & 0xffff;
	p0.h = WDOGCTL >> 16;
	W[p0] = r7;
	ssync;
// End bypass workaround
	rts; 
	
_pllInit.end:	
