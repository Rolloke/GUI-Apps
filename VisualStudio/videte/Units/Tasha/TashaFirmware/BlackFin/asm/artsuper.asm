/*******************************************************************************
 *
 * $Source$
 * $Revision$
 *
 * Boot code for assembly language applications running in supervisor mode.
 *
 * $Log$
 *
 ******************************************************************************/


#include <defBF535.h>
#include <DefBlackfin.h>

#define IVBh (EVT0 >> 16)
#define IVBl (EVT0 & 0xFFFF)

//..............................................................................

	.section program;
	.align 2;

	.global start_super;
	.extern _main;
	.extern pllInit;
	.extern ldf_stack_end;
	.extern ldf_sysstack_end;
start_super:
// Zap loop counters to zero, to make sure that
	// hw loops are disabled - it could be really baffling
	// if the counters and bottom regs are set, and we happen
	// to run into them.
	R7 = 0;
	LC0 = R7;
	LC1 = R7;

	// Clear the DAG Length regs too, so that it's safe to
	// use I-regs without them wrapping around.
	L0 = R7;
	L1 = R7;
	L2 = R7;
	L3 = R7;

//..............................................................................

#ifdef DIRECT_FLASH_BOOT

	// If flash bootstrap code is bypassed and it executes directly from
	// flash memory, move code to internal memory before progressing further.

// Note this address is chosen because elfloader inserts 4 bytes of beginning
// of .hex file, which needs to be cleared out (inserting two nops there).
// It is easier to just clear these bytes than to readjust the entire .hex
// file. However, if a script does this or a new version of elfloader comes out
// which supports removing these bytes the address below should be changed to 
// 0x20000000.

#define FLASH_BASE 0x20000000

	// The code is a bit lame, but does the work and is also likely to 
 	// change so...

	P0.H = (FLASH_BASE >> 16)&0xFFFF;
	P0.L = (FLASH_BASE & 0xFFFF);

	P1.H = (L2_BASE >> 16)&0xFFFF;
	P1.L = (L2_BASE & 0xFFFF);

	P2.H = startCodeToMove;
	P2.L = startCodeToMove;

	P5 = 2000;	// move 2000 16-bit words

	LOOP moveCode LC0 = P5;

LOOP_BEGIN moveCode;

	R0 = W[ P0 ++ ](Z);
	W[ P1 ++ ] = R0;

LOOP_END moveCode;

	JUMP (P2);     // jump to moved startCodeMove

startCodeToMove:

#endif

//..............................................................................

	P0.H = IVBh;
	P0.L = IVBl;
	P0 += 2*4;		// Skip Emulation and Reset

	R0 = _NHANDLER (Z);
	R0.H = _NHANDLER;	// NMI Handler (Int2)
	[ P0 ++ ] = R0;

	R0.L = _XHANDLER;
	R0.H = _XHANDLER;	// Exception Handler (Int3)
	[ P0 ++ ] = R0;
    
	[ P0 ++ ] = R0;		// IVT4 isn't used

	R0 = _HWHANDLER (Z);
	R0.H = _HWHANDLER;	// HW Error Handler (Int5)
	[ P0 ++ ] = R0;
    
	R0 = _THANDLER (Z);
	R0.H = _THANDLER;	// Timer Handler (Int6)
	[ P0 ++ ] = R0;
    
	R0 = _RTCHANDLER (Z);
	R0.H = _RTCHANDLER;	// IVG7 Handler
	[ P0 ++ ] = R0;

	R0 = _I8HANDLER (Z);
	R0.H = _I8HANDLER;	// IVG8 Handler
	[ P0 ++ ] = R0;

	R0 = _I9HANDLER (Z);
	R0.H = _I9HANDLER;	// IVG9 Handler
	[ P0 ++ ] = R0;

	R0 = _I10HANDLER (Z);
	R0.H = _I10HANDLER;	// IVG10 Handler
	[ P0 ++ ] = R0;

	R0 = _I11HANDLER (Z);
	R0.H = _I11HANDLER;	// IVG11 Handler
	[ P0 ++ ] = R0;

	R0 = _I12HANDLER (Z);
	R0.H = _I12HANDLER;	// IVG12 Handler
	[ P0 ++ ] = R0;

	R0 = _I13HANDLER (Z);
	R0.H = _I13HANDLER;	// IVG13 Handler
	[ P0 ++ ] = R0;

	R0 = _I14HANDLER (Z);
	R0.H = _I14HANDLER;	// IVG14 Handler
	[ P0 ++ ] = R0;

	R0 = _I15HANDLER (Z);
	R0.H = _I15HANDLER;	// IVG15 Handler
	[ P0 ++ ] = R0;

#if defined(__ADSP21535REV1__) || defined(__ADSP21532__)
	// EVT_OVERRIDE fixed at 0x0000. Not available for use.
#else
	// EVT_OVERRIDE comes up with random value on reset.
	// zero before allowing interrupts to occur.
	P0.H = (EVT_OVERRIDE >> 16);
	P0.L = (EVT_OVERRIDE & 0xFFFF);
	R0 = 0;
	[P0] = R0;
#endif	/* __ADSP21535REV1__ || __ADSP21532__ */

	R0 = SYSCFG;		// Enable the Cycle counter
	BITSET(R0,1);
	SYSCFG = R0;

	// Initialise the stacks. First the user-mode stack:

	FP.L=ldf_stack_end;
	FP.H=ldf_stack_end;
	SP.L=ldf_stack_end;
	SP.H=ldf_stack_end;
	usp = sp;

	// Now the supervisor stack.
	SP.L=ldf_sysstack_end - 16;
	SP.H=ldf_sysstack_end - 16;

	//  Enable interrupts
	R0 = 0x400;	
	R0 <<= 5;		// enable interrupt 15; 4-0 not settable
	STI R0;

	// Stay in Supervisor Mode when coming out of reset

	call pllInit;		// PLL erratum workaround
	
#ifdef SPI_BOOT			// [ SPI_BOOT

	// remain in reset if this is the SPI boot

	jump _main;

#else

	p0.l=EVT15&0xFFFF;
	p0.h=(EVT15 >> 16)&0xFFFF;
	p1.l=ivt15_isr;
	p1.h=ivt15_isr;
	[p0]=p1;
	raise 15;
	p0.l=wait_here;
	p0.h=wait_here;

	reti=p0;
	rti;

//------------------------------------------------------------------------------

wait_here:
	jump wait_here;

ivt15_isr:
	[--sp] = RETI; // enable interrupts
	jump _main;
	rti;

#endif				// ] SPI_BOOT

start.end:
//------------------------------------------------------------------------------

// Interrupt vectors

_NHANDLER:	      // NMI Handler 2
    	RTN;

_XHANDLER:	      // Exception Handler 3
    	RTX;

_HWHANDLER:	      // HW Error Handler 5
    	RTI;

_THANDLER:	      // Timer Handler 6
    	RTI;

_RTCHANDLER:	       // IVG 7 Handler	 (RTC)
    	RTI;

_I8HANDLER:	      // IVG 8 Handler
    	RTI;

_I9HANDLER:	      // IVG 9 Handler
    	RTI;

_I10HANDLER:	      // IVG 10 Handler
    	RTI;

_I11HANDLER:	      // IVG 11 Handler
    	RTI;

_I12HANDLER:	      // IVG 12 Handler
    	RTI;

_I13HANDLER:	    // IVG 13 Handler
    	RTI;

_I14HANDLER:
    	RTI;

_I15HANDLER:
    	RTI;

//------------------------------------------------------------------------------




