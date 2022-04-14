/*******************************************************************************
 *
 * $Source$
 * $Revision$
 *
 * RTC test program.
 *
 * $Log$
 *
 ******************************************************************************/

#include <def21535.h>
#include "m21535.h"
#include "gpio.h"

//------------------------------------------------------------------------------

	.section data1;

	.global	_main;

	.align 4;
	.byte4  rtcStatCopy;

//------------------------------------------------------------------------------

	.section program;
	.align 2;

_main:

	// Clear variable

	I0.L = rtcStatCopy;
	I0.H = rtcStatCopy;
	P0 = I0;
	R0 = 0(z);
	[P0] = R0;

//..............................................................................

	// set up event handler

	p0.l = EVT0 & 0xffff;
	p0.h = EVT0 >> 16;
	P0 += 7*4;		// go to IVG 7

	R0 = _RTCHANDLER (Z);
	r0.h = _RTCHANDLER; 	// IVG7 Handler
	[ P0 ] = R0;

//..............................................................................

   	P0.L = IMASK & 0xffff;
	P0.H = IMASK >> 16;

	// enable IVG7 (RTC handler) by enabling it in IMASK
	// P0 now points to IMASK

	R0 =  [P0];  	
	BITSET(R0,7);
	W[ P0 ] = R0;   

//..............................................................................

	// set up GPIO

    	P3.L = FIO_DIR & 0xffff;	
    	P3.H = FIO_DIR >> 16;

	r6 = kLEDSet(z);

    	W [ P3 ] = R6;
    	SSYNC;

	// clear LEDs

	P3.L = FIO_FLAG_C & 0xffff;	
	P3.H = FIO_FLAG_C >> 16;
    	W [ P3 ] = R6; 
    	SSYNC;

//..............................................................................

	// set up RTC

//Check revision of silicon 
//The polarity of SIC_IMASK changed between silicon revisions 0.1/0.2 and 1.0
	P0.L = chipId & 0xffff;    
	P0.H = chipId >> 16;
	R0 = [P0];           

	p3.l = SIC_IMASK & 0xffff;
	p3.h = SIC_IMASK >> 16;
	R6 = [p3];

	r1.h = 0xf000;
	r1.l = 0;
	R1 = R1 & R0;
	cc = az;
	if cc jump beforeRev1_0;      

	// write complete and write pending flags are not valid until the
	// second event has occurred, so check for that first

	// unmask the RTC general purpose interrupt

	BITSET(R6,0);
	jump restore;	

beforeRev1_0:
	BITCLR(R6,0);	

restore:
	[ P3 ] = R6;

	// get RTC CLK running fast for test

	p3.l = RTCFAST & 0xffff;
	p3.h = RTCFAST >> 16;
	R6.L = 0x0000;
	W [ P3 ] = R6.L;

	p0.l = RTCISTAT & 0xffff;
	p0.h = RTCISTAT >> 16;

	// wait for seconds event,
	// since until it has occurred other flags are not valid

loop_sync: 
	r0=[p0];
	cc=bittst(r0,bSecondsEvt); // loop until we get a seconds event
	if !cc jump loop_sync;

//..............................................................................

	// set current time (set everything to 0)
	p3.l = RTCSTAT & 0xffff;
	p3.h = RTCSTAT >> 16;
	R6 = 0x0000(z);
	[ P3 ] = R6;
	ssync;

	// Loop until write to RTCSTAT has been confirmed.
	// This is just done to give it a known start value since undefined 
	// after reset.
	// The real time should be set when running with a 1 Hz clock later.

	p0.l = RTCISTAT & 0xffff;
	p0.h = RTCISTAT >> 16;

loop_sync2: 
	r0=[p0];
	cc=bittst(r0,bWriteComplete); 
	if !cc jump loop_sync2;

//..............................................................................

	// clear sticky bits

	r6=(kWriteCompleteEvt|kDayAlarmEvt|k24HoursEvt|kMinutesEvt|\
kSecondsEvt|kAlarmEvt|kStopwatchEvt)(z);
	W[p0]=r6;

	// enable the seconds interrupt (used to blink LEDs and to store current 
	// time to rtcStatCopy)

	p3.l = RTCICTL & 0xffff;
	p3.h = RTCICTL >> 16;
	R6 = [p3];
	BITSET(R6,bSecondsIntEn);  
	W[ P3 ] = R6; 
	ssync;

	p3.l = RTCFAST & 0xffff;
	p3.h = RTCFAST >> 16;
	R6.L = 0x0001;		// set clock to be every 1 Hz
	W [ P3 ] = R6.L;
	ssync;

//..............................................................................

	// clear the pending reset interrupt (return to User mode) and
	// set all other return regs to a known spot (except RETE)
	nop;
	r0.h = waitHere;
	r0.l = waitHere;
	reti = r0;
	rets = r0;
	retn = r0;
	retx = r0;
	nop;
	rti;	// return from supervisor mode to user mode!!
	nop;	// never executed
	nop;	// never executed

//..............................................................................

waitHere: 
	jump waitHere;

//------------------------------------------------------------------------------

_RTCHANDLER:          // IVG 7 Handler  (RTC)

	i1.L =  RTCISTAT & 0xffff;
  	i1.H =  RTCISTAT >> 16;
	r7.l = kSecondsEvt;
    	r7.h = 0x0000;

	// clear seconds event flag
    	[i1] = r7; 
    	ssync;
	
	// store RTC_STAT to rtcStatCopy
	p4.l = RTCSTAT & 0xffff;
	p4.h = RTCSTAT >> 16;
	I0.L = rtcStatCopy;
	I0.H = rtcStatCopy;
	R2 = [ P4 ];
	[ I0 ] = R2;

	// toggle LED to indicate RTC interrupt occurred

	P2.L = FIO_FLAG_C & 0xffff;
  	p2.h = FIO_FLAG_C >> 16;

	P4.L = FIO_FLAG_S & 0xffff;
  	p4.h = FIO_FLAG_S >> 16;

	// get current setting of GPIO

	r4=W[p4];
	r2=kLEDSet(z);
	r4=r4&r2;
	cc = r4 == 0;	
	if cc jump toggle1;
	W[p2]=r2;	// clear LEDs
	jump toggle2;
toggle1:
	W[p4]=r2;	// set LEDs
toggle2:

	rti;

//------------------------------------------------------------------------------




