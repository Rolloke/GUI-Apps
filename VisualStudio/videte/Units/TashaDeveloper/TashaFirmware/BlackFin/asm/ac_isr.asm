/*******************************************************************************
 *
 * $Source$
 * $Revision$
 *
 * Audio codec test program. Interrupt service routine.
 *
 * $Log$
 *
 ******************************************************************************/

#include    <def21535.h>
#include    "zet.h"
#include    "m21535.h"
#include    "ac_init.h"
#ifdef	UV_METER
#include    "gpio.h"
#endif

//..............................................................................

	.section data1;

	.global  setUpIVG8Handler;
	.extern  rxStatus;

#ifdef GEN_SINE

	.extern  _sineCalc;
	.global  initLookUp;

	.align 4;
	.byte4   ptrArray = sineLookUp;
	.byte2	 sineLookUp[sampleRate];
endSineLookUp:

#endif

#ifdef	UV_METER

#define CMP_SET_LED_START(thres,count) \
	r7 = thres(z);\
	cc = r1 <= r7;\
	if cc jump led##count;\
	r0 = kLEDSet##count##(z);

#define CMP_SET_LED(thres,count) \
	r7 = thres(z);\
	cc = r1 <= r7;\
	if cc jump led##count;\
	r7 = kLEDSet##count;\
	r0 = r7|r0;

#define	arrayLength 8

	.align 2;
	.byte2	circArr[arrayLength];
	.byte4	circArrPtr = circArr;

#endif

	.byte2	leftSmpl;
	.byte2	rightSmpl;

//------------------------------------------------------------------------------

	.section program;
	.align 2;

#ifdef	GEN_SINE

initLookUp:

	[--sp] = RETS;		// will use nested subroutines so store RETS
 
loopStart:

	call	_sineCalc;	// return value in r0

	p4.l = ptrArray;
	p4.h = ptrArray;

	p2 = [p4];
	w[p2] = r0.l;
	p2 += 2;
	[p4] = p2;

	p5.l = endSineLookUp;
	p5.h = endSineLookUp;

	CC = p2 == p5;	
	if !cc jump loopStart;	

//..............................................................................

	p2.l = sineLookUp;
	p2.h = sineLookUp;
	[p4] = p2;		// restore ptr

	RETS = [sp++];

	rts;

#endif

//..............................................................................

setUpIVG8Handler:

	// Setup IVG8 (SPORT0/1 interrupt vector)

	R0 = _I8HANDLER (Z);
	R0.H = _I8HANDLER;	// IVG8 Handler
	p0.l = EVT0 & 0xffff;
	p0.h = EVT0 >> 16;
	P0 += 8*4;		// go to IVG 8
	[ P0 ] = R0;
	rts;

//..............................................................................

_I8HANDLER:	      // IVG 8 Handler

    	[--SP] = (r7:0);
    	[--SP] = (p5:0);

//..............................................................................

	// Don't know exactly what's the purpose of this check? 
	// If it is to ensure that it was the DMA receive that generated the
	// interrupt wouldn't it be better to check its status bit in 
	// SPORT0_IRQSTAT_RX?

	R0 = LENGTH(rx_buf);
	p0.l = SPORT0_COUNT_RX & 0xFFFF;
	p0.h = (SPORT0_COUNT_RX >> 16) & 0xFFFF;
	R1 = W[p0];
	R0 = R0 - R1;
	CC = AZ;
	IF CC jump rollOver;

//..............................................................................
	
	// check to see that background code has reported we are now receiving
	// audio data

	r0 = 0(z);
	bitset(r0,bAudioData);

	p0.l = rxStatus;
	p0.h = rxStatus;
	r1 = w[p0];		
	R0 = R0 & R1;
	CC = AZ;
	IF CC jump rollOver;
	call procAudioSamples;

//..............................................................................

rollOver:
	R0 = kIRQSTAT_RX;    		// Clear RX Interrupts 
					// (cleared by writing 1s to them)
	P0.L = SPORT0_IRQSTAT_RX & 0xFFFF;
	P0.H = (SPORT0_IRQSTAT_RX >> 16) & 0xFFFF;
	W[p0] = R0;

	(p5:0) = [SP++];	    	// restore regs
	(r7:0) = [SP++];
	RTI;

//------------------------------------------------------------------------------

procAudioSamples:

	r7.l = kVldFrm;
	p0.l = tx_buf;
	p0.h = tx_buf;
	p0 += oTagPhase;	// Clear all AC97 link Audio Output Frame slots 
	nop;
	nop;
	nop;
	W[P0] = R7;		// and set Valid Frame bit in SLOT '0' TAG phase  
	R7 = 0x0000;
	p0 += -oTagPhase;
	p0 += oCmdAddrSlot;
	nop;
	nop;
	nop;
	W[P0] = R7;
	p0 += -oCmdAddrSlot;
	p0 += oCmdDataSlot;
	nop;
	nop;
	nop;
	W[P0] = R7;
	p0 += -oCmdDataSlot;
	p0 += oLeft;
	nop;
	nop;
	nop;
	W[P0] = R7;
	p0 += -oLeft;
	p0 += oRght;
	nop;
	nop;
	nop;
	w[P0] = R7;

//..............................................................................

	// check ADCs for valid data

	p0.l = rx_buf;
	p0.h = rx_buf;
	p0 += oTagPhase;	// Get ADC valid bits from tag phase slot
	nop;
	nop;
	nop;
	R5 = W[P0];
	bitset(r1,bLeftVld);
	bitset(r1,bRghtVld);
	R7 = R5 & R1;		// Mask other bits in tag slot

	p0.l = tx_buf;
	p0.h = tx_buf;
	p0 += oTagPhase;	// Frame/Addr/Data valid bits 
	nop;
	nop;
	nop;
	R1 = W[P0];
	R1 = R1 | R7;		//  Set TX valid bits based on Recieve TAG info 
	W[P0] = R1;

//..............................................................................

	// Check Left ADC valid bit 

	CC = BITTST (R5, bLeftVld);		
	IF !CC JUMP checkADCRight; 	// If valid data then save sample 
	p0.l = rx_buf;
	p0.h = rx_buf;
	p0 += oLeft;		// Get AD1885 Left channel input sample 
	nop;
	nop;
	nop;
	R1 = W[P0];
	P1.l = leftSmpl;	
	p1.h = leftSmpl;
	nop;
	nop;
	nop;
	W[P1] = R1;		// Save to data holder for processing 

checkADCRight:					    
	CC = BITTST (R5, bRghtVld);  
	IF !CC JUMP isrExit;	// If valid data then save ADC sample 
				
	// Note: this code does not take care of case where e.g. 
	// left input sample is valid but right is not, since in
	// that case it would exit without outputting any new samples.

	// ...or right input sample is valid but left not, since then it
	// would output both left (old) and right samples. 

	p0.l = rx_buf;
	p0.h = rx_buf;
	p0 += oRght;		// Get AD1885 Right channel input sample 
	nop;
	nop;
	nop;
	R1 = W[P0];
	P0.l = rightSmpl;	// Save to data holder for processing 
	p0.h = rightSmpl;
	nop;
	nop;
	nop;
	W[P0] = R1;

	P1.l = leftSmpl;	
	p1.h = leftSmpl;

//------------------------------------------------------------------------------
// Insert DSP Algorithms Here 
//------------------------------------------------------------------------------

	// GEN_SINE may be used together with target ac_tst

#ifdef	GEN_SINE

	p5.l = endSineLookUp;
	p5.h = endSineLookUp;

	p4.l = ptrArray;
	p4.h = ptrArray;

	p2 = [p4];

	CC = p2 == p5;	
	IF !CC jump dontWrap; 

	p2.l = sineLookUp;
	p2.h = sineLookUp;

dontWrap:

	r7 = w[p2];
	w[p1] = r7.l;		// store to leftSmpl
	w[p0] = r7.l;		// store to rightSmpl
	p2+=2;
	[p4] = p2;

#endif

//------------------------------------------------------------------------------

	// UV_METER may be used together with target flashdemo

#ifdef	UV_METER

	[--sp] = l0;
	[--sp] = b0;
	[--sp] = i0;
	[--sp] = lc0;
	[--sp] = lb0;
	[--sp] = lt0;

	b0.l = circArr; 
	b0.h = circArr; 
	l0 = arrayLength(z);
	p5.l = circArrPtr;
	p5.h = circArrPtr;
	r7 = [p5];
	i0 = r7;

	// store new sample in circular array
	// takes value on left input channel
	// would (left+right)/2 be a better choice?

	r7.l = w[p1];
	w[i0++] = r7.l; // read leftSmpl
	p2 = i0;
	[p5] = p2;	// restore circular array pointer (possibly wrapped)

	// calculate absolute max value of circular array

	p5 = arrayLength(z);
	p2 = b0;	
	r1 = 0(z);

	loop calcAbsMax lc0 = p5;

loop_begin calcAbsMax;

	r7 = w[p2++](x);
	r7 = abs r7;
	r1 = max(r1,r7);

loop_end calcAbsMax;	

	// set up LEDs, r1 contains absolute max value of array

	// a "counting leading zeros" instruction would be of
	// great help calculating this but there isn't any
	// for this architecture

	p5.l = FIO_FLAG_C & 0xFFFF;	    
	p5.h = (FIO_FLAG_C >> 16) & 0xFFFF;
    	r7.l = kLEDSet;	
	w[p5] = r7.l; ssync;

	p5.l = FIO_FLAG_S & 0xFFFF;		
	p5.h = (FIO_FLAG_S >> 16) & 0xFFFF;

	r0 = 0(z);

	// these macros aren't that pretty

#ifdef _EZKITLITE_ 		// [ _EZKITLITE_


CMP_SET_LED_START(0x0100,0)
led0:
CMP_SET_LED(0x2000,1)
led1:
CMP_SET_LED(0x4000,2)
led2:
CMP_SET_LED(0x6000,3)
led3:

#else				// ] [ !_EZKITLITE_
#ifdef _EAGLE_35_  		// [ _EAGLE_35_

CMP_SET_LED_START(0x0100,0)
led0:
CMP_SET_LED(0x2000,1)
led1:
CMP_SET_LED(0x3000,2)
led2:
CMP_SET_LED(0x4000,3)
led3:
CMP_SET_LED(0x5000,4)
led4:
CMP_SET_LED(0x6000,5)
led5:
CMP_SET_LED(0x7000,6)
led6:

#else				// ] [ !_EAGLE_35_
#error "Board not defined"
#endif				// ] _EAGLE_35_
#endif				// ] _EZKITLITE_

	w[p5] = r0.l; ssync;	// set UV level

//..............................................................................
		
	lt0 = [sp++];
	lb0 = [sp++];
	lc0 = [sp++];
	i0  = [sp++];
	b0  = [sp++];
	l0  = [sp++];

#endif

//------------------------------------------------------------------------------
// End of DSP Algorithms 
//------------------------------------------------------------------------------

	R7 = W[P1];		// Fetch Left Channel 
	p2.l = tx_buf;
	p2.h = tx_buf;
	p2 += oLeft;		
	nop;
	nop;
	nop;
	W[P2] = R7;		// ...output Left data to Slot 3 
	R7 = W[P0];		// Fetch Right Channel 
	p2 += -oLeft;
	p2 += oRght;	    	
	nop;
	nop;
	nop;
	W[P2] = R7;		// ...output Right data to Slot 4 

isrExit:
	RTS;
