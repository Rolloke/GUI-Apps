/* Copyright (C) 2000 Analog Devices Inc., All Rights Reserved.
** This contains Analog Devices Background IP and Development IP as
** defined in the ADI/Intel Collaboration Agreement.
** ADI/Intel Confidential.
*/
// basic startup code which
// - installs default event handlers
// - turns the cycle counter on
// - loads up FP & SP (both supervisor and user)
// - initialises the device drivers (FIOCRT)
// - gets out of supervisor mode and into user mode
// - loads other registers with unassigned pattern 0x81818181
// - calls monstartup to set up the profiling routines (PROFCRT)
// - calls the C++ startup (CPLUSCRT)
// - initialises argc/argv (FIOCRT/normal)
// - calls _main
// - calls _exit (which calls monexit to dump accumulated prof data (PROFCRT))
// - defines dummy IO routines (!FIOCRT)

#include <DefBlackfin.h>
#define IVBh (EVT0 >> 16)
#define IVBl (EVT0 & 0xFFFF)
#define UNASSIGNED_FILL 1

	.section program;
	.align 2;
start:
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

	// Initialise the Event Vector table, to invoke a
	// default routine for most events. The routines
	// all just report the event status to standard
	// err, and exit.
	// Exceptions are, well, the exception. Their
	// handler does real, useful stuff.

	P0.H = IVBh;
	P0.L = IVBl;
	P0 += 2*4;		// Skip Emulation and Reset
	
	P1.L=_ex_def_nmi;
	P1.H=_ex_def_nmi;
	[P0++]=P1;

	P1.L=_ex_def_exception;	// or _exception_hdr
	P1.H=_ex_def_exception;
	[P0++]=P1;

	P1.L=_ex_def_glob;
	P1.H=_ex_def_glob;
	[P0++]=P1;

	P1.L=_ex_def_hw;
	P1.H=_ex_def_hw;
	[P0++]=P1;

	P1.L=_ex_def_timer;
	P1.H=_ex_def_timer;
	[P0++]=P1;

	P1.L=_ex_def_i7;
	P1.H=_ex_def_i7;
	[P0++]=P1;

	P1.L=_ex_def_i8;
	P1.H=_ex_def_i8;
	[P0++]=P1;

	P1.L=_ex_def_i9;
	P1.H=_ex_def_i9;
	[P0++]=P1;

	P1.L=_ex_def_i10;
	P1.H=_ex_def_i10;
	[P0++]=P1;

	P1.L=_ex_def_i11;
	P1.H=_ex_def_i11;
	[P0++]=P1;

	P1.L=_ex_def_i12;
	P1.H=_ex_def_i12;
	[P0++]=P1;

	P1.L=_ex_def_i13;
	P1.H=_ex_def_i13;
	[P0++]=P1;

	P1.L=_ex_def_i14;
	P1.H=_ex_def_i14;
	[P0++]=P1;

	P1.L=_ex_def_i15;
	P1.H=_ex_def_i15;
	[P0++]=P1;

	R0 = SYSCFG;		// Enable the Cycle counter
	BITSET(R0,1);
	SYSCFG = R0;

	// Initialise the stacks. First the user-mode stack:
	FP.L=ldf_stack_end;
	FP.H=ldf_stack_end;
	SP.L=ldf_stack_end;
	SP.H=ldf_stack_end;
	usp = sp;

	// Now the sypervisor stack.
	SP.L=ldf_sysstack_end - 16;
	SP.H=ldf_sysstack_end - 16;

#ifdef L1CACHE
	// Memory map configuration.
	// We assume the presence of the following areas, with the
	// following cache settings:
	// Core and System MMRs, 4MB, not cached
	// L1 SRAM Scratch pad, 1MB, not cached
	// L1 SRAM code area, 1MB, instruction cache of L2
	// L1 SRAM data area A, 1MB, data cache of L2
	// L1 SRAM data area B, 1MB, data cache of L2
	// L2 SRAM 1MB, cached
	// (all sizes give the regions of memory allowed, which may be
	// larger than the physical memory available)
	//
	// Other areas of memory are considered to be not available, and
	// accesses to them will generate exceptions.
#ifdef __ADSP21535__
#define NUM_CPLBS 6
#else
#define NUM_CPLBS 11
#endif

	// Create DCPLB_ADDR and ICPLB_ADDR entries
	I0.L = (DCPLB_ADDR0 & 0xFFFF);
	I0.H = (DCPLB_ADDR0 >> 16);
	I1.L = (ICPLB_ADDR0 & 0xFFFF);
	I1.H = (ICPLB_ADDR0 >> 16);
	I2.L = cplb_addrs;
	I2.H = cplb_addrs;
	P3=NUM_CPLBS;
	LSETUP(s_cplb_addrs, e_cplb_addrs) LC0=P3;
s_cplb_addrs:	R0 = [I2++];
		[I0++] = R0;
e_cplb_addrs:	[I1++] = R0;

	// Configure DCPLB_DATA entries
	I0.L = (DCPLB_DATA0 & 0xFFFF);
	I0.H = (DCPLB_DATA0 >> 16);
	I3.L = (ICPLB_DATA0 & 0xFFFF);
	I3.H = (ICPLB_DATA0 >> 16);
	I1.L = cplb_clr;
	I1.H = cplb_clr;
	I2.L = cplb_set;
	I2.H = cplb_set;
	LSETUP(s_cplb_bits, e_cplb_bits) LC0 = P3;
s_cplb_bits:	R0 = [I0];	// Get current DCPLB entry
		R1 = [I1++];
		R2 = [I2++];
		R0 = R0 & R1;	// mask off what we want to clear
		R0 = R0 | R2;	// then set bits we need
		[I0++] = R0;	// and store back
		R3 = [I3];	// Ditto for ICPLB
		R3 = R3 & R1;
		R3 = R3 | R2;
e_cplb_bits:	[I3++] = R3;

	// Now that the CPLBs are set up, set the control words to
	// reference them.

	// Configure L1 SRAM data banks as cache
	// - Default to DCBS==0, so LOWBIT (bit14) selects bank A or B
	//   (because that splits L2 across A and B)
	// - set DMC==11, so both A and B are cache.
	// - Set ENDCPLB==1, so DCPLBs are referenced.
	// - set ENDM==1, so L1 Data Memory is enabled.
	P0.L = (DMEM_CONTROL & 0xFFFF);
	P0.H = (DMEM_CONTROL >> 16);
	R0 = [P0];
	R1 = (ACACHE_BCACHE | ENDCPLB |ENDM);
	R0 = R0 | R1;			// set these bits
	[P0] = R0;

	// Configure L1 SRAM code bank as cache
	// - Default to ILOC==0000
	// - Set IMC==1, so cache.
	// - Set ENICPLB==1, so ICPLBs are referenced.
	// - Set ENIM==1, so Code memory is enabled.
	P0.L = (IMEM_CONTROL & 0xFFFF);
	P0.H = (IMEM_CONTROL >> 16);
	R0 = [P0];
	R1 = (IMC | ENICPLB | ENIM);
	R0 = R0 | R1;
	[P0] = R0;
#endif /* L1CACHE */

#ifdef FIOCRT
	// initialise the devices known about for stdio.
	call _init_devtab;
#endif /* FIOCRT */

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

	//  Enable interrupts
	R0 = 0x400;	
	R0 <<= 5;	// enable interrupt 15; 4-0 not settable
	STI R0;

#ifdef BUILD_USER_MODE
	// This is only good for applications which don't need to
	// access hardware and high privilege. For application like
	// RTOS we need to stay in supervisor mode for all the
	// C/C++ implementation.

	// Move the processor into user mode.
	P0.L=usermode;
	P0.H=usermode;
	RETI=P0;
	rti;		// should take us to following instruction in user mode
#else

	// Stay in Supervisor Mode when coming out of reset

	call pllInit;		// PLL erratum workaround

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
wait_here:
	jump wait_here;

ivt15_isr:
	[--sp] = RETI; // enable interrupts
	jump _main;
	rti;

#endif
usermode:

#ifdef UNASSIGNED_FILL
	P0.L=unass;
	P0.H=unass;
	R0=[P0];
	R2=R0;
	R3=R0;
	R4=R0;
	R5=R0;
	R6=R0;
	R7=R0;
	P0=R0;
	P1=R0;
	P2=R0;
	P3=R0;
	P4=R0;
	P5=R0;
	[SP]=R0;
	[SP+4]=R0;
	[FP]=R0;
	[FP+4]=R0;
#endif

#ifdef PROFCRT
        call monstartup; // initialise profiling routines
#endif  /* PROFCRT */
#ifdef PROFGUIDE
        call ___start_prof; // initialise profile guided routine
#endif  /* PROFGUIDE */
#ifdef CPLUSCRT
	R0=0;		// load up R0 and R1 and call _main()
	R1=R0;
	call ___ctorloop;
#endif  /* CPLUSCRT */
#ifdef FIOCRT
	// FILE IO provides access to real command-line arguments.
	call __getargv;
	r1.l=__Argv; 
	r1.h=__Argv;
#else
	// Default to having no arguments and a null list.
	R0=0;
	R1.L=argv;
	R1.H=argv;
#endif /* FIOCRT */

	// At long last, call the application program.
	call _main;

	call _exit;	// passing in main's return value

	nop;
	nop;
	HLT;		// In case we're still here.


	.global start;
	.type start,STT_FUNC;
	.global _main;
	.type _main,STT_FUNC;
	.global _exit;
	.type _exit,STT_FUNC;
#ifdef FIOCRT
	.global __getargv;
	.type __getargv,STT_FUNC;
	.global _init_devtab;
	.type _init_devtab,STT_FUNC;
#else
	// If File IO support isn't provided, then
	// we provide dummy versions of the device-handling
	// functions, so that the exception handlers don't rely
	// on the file IO library
	.align 2;
_dev_open:
_dev_close:
_dev_write:
_dev_read:
_dev_seek:
_dev_dup:
_fileio_lseek:
	R0 = -1;
	RTS;

	.global _dev_open;
	.type _dev_open,STT_FUNC;
	.global _dev_close;
	.type _dev_close,STT_FUNC;
	.global _dev_write;
	.type _dev_write,STT_FUNC;
	.global _dev_read;
	.type _dev_read,STT_FUNC;
	.global _dev_seek;
	.type _dev_seek,STT_FUNC;
	.global _dev_dup;
	.type _dev_dup,STT_FUNC;
	.global _fileio_lseek;
	.type _fileio_lseek,STT_FUNC;
#endif /* FIOCRT */


	.section data1;
#ifndef FIOCRT
	// With no FILE IO support, we cannot fetch arguments
	// from the command line, so we provide a null list.
	.align 4;
argv:
	.byte4=0;				// argv[0]==0
#endif /* !FIOCRT */
	.align 4;
unass:
	.byte4=0x81818181;

	.align 4;
nullp:
	.byte4=0;				// no environment pointers
	.align 4;
__Environ:
	.byte4=nullp;
	.global __Environ;
	.type __Environ,STT_OBJECT;
#ifdef L1CACHE
	// Data for use when initialising the CPLBs when setting up
	// the cache. Cleaner than loading lots of literals.

	// Addresses of the memory areas we configure.
cplb_addrs:
	.byte4 =
		0xFFC00000, 	// MMRs
		0xFFB00000,	// Scratchpad
		0xFFA00000,	// Code
		0xFF900000,	// Data B
		0xFF800000,	// Data A
#ifdef __ADSP21535__
		0xF0000000;	// L2 SRAM - just 256K
#else
		// For generic Blackfin, consider L2 to be as large
		// as possible, and plant 4MB pages in the CPLBs to
		// cover the sections defined. (if the sections expand
		// beyond 4MB each, additional CPLBs will be needed).
		0xF0000000,	// L2 SRAM program
		0xF4000000,	// data
		0xF8000000,	// data2
		0xFBFC0000,	// argv
		0xFC000000,	// heap
		0xFF400000;	// stacks (CPLB at top of this space)
#endif
	// Bits to clear in the CPLBs (these are masks)
cplb_clr:
	.byte4 =
		(~(CPLB_L2_CHBL|CPLB_L1_CHBL|CPLB_DA0ACC)), // MMRs
		(~(CPLB_L2_CHBL|CPLB_L1_CHBL|CPLB_DA0ACC)), // Scr
		(~(CPLB_L2_CHBL|CPLB_L1_CHBL|CPLB_DIRTY|CPLB_DA0ACC)), // Code
		(~(CPLB_L2_CHBL|CPLB_L1_CHBL|CPLB_DIRTY|CPLB_DA0ACC)), // D B
		(~(CPLB_L2_CHBL|CPLB_L1_CHBL|CPLB_DIRTY|CPLB_DA0ACC)), // D A
#ifdef __ADSP21535__
		(~(CPLB_L2_CHBL|CPLB_DIRTY|CPLB_DA0ACC)); // L2
#else
		(~(CPLB_L2_CHBL|CPLB_DIRTY|CPLB_DA0ACC)), // L2 program
		(~(CPLB_L2_CHBL|CPLB_DIRTY|CPLB_DA0ACC)), // L2 data
		(~(CPLB_L2_CHBL|CPLB_DIRTY|CPLB_DA0ACC)), // L2 data2
		(~(CPLB_L2_CHBL|CPLB_DIRTY|CPLB_DA0ACC)), // L2 argv
		(~(CPLB_L2_CHBL|CPLB_DIRTY|CPLB_DA0ACC)), // L2 heap
		(~(CPLB_L2_CHBL|CPLB_DIRTY|CPLB_DA0ACC)); // L2 stacks
#endif
	// Bits to set in the CPLBs
cplb_set:
#define L2_FLAGS CPLB_WT|CPLB_L1_CHBL|CPLB_L1SRAM|CPLB_SUPV_WR|CPLB_USER_WR|CPLB_USER_RD|CPLB_VALID
	.byte4 =
		(PAGE_SIZE_4MB|CPLB_DIRTY|CPLB_SUPV_WR|CPLB_VALID), // MMRs
		(PAGE_SIZE_1MB|CPLB_DIRTY|CPLB_SUPV_WR|CPLB_USER_WR|CPLB_USER_RD|CPLB_VALID), // Scr
		(PAGE_SIZE_1MB|CPLB_WT|CPLB_VALID), // Code
		(PAGE_SIZE_1MB|CPLB_WT|CPLB_VALID), // D B
		(PAGE_SIZE_1MB|CPLB_WT|CPLB_VALID), // D A
#ifdef __ADSP21535__
		(PAGE_SIZE_1MB|L2_FLAGS); // L2
#else
		(PAGE_SIZE_4MB|L2_FLAGS), // L2 program
		(PAGE_SIZE_4MB|L2_FLAGS), // L2 data
		(PAGE_SIZE_4MB|L2_FLAGS), // L2 data2
		(PAGE_SIZE_4MB|L2_FLAGS), // L2 argv
		(PAGE_SIZE_4MB|L2_FLAGS), // L2 heap
		(PAGE_SIZE_4MB|L2_FLAGS); // L2 stacks
#endif
#endif /* L1CACHE */

#ifdef CPLUSCRT
.section ctor;
	.align 4;
___ctor_table:
	.byte4=0;				
.global ___ctor_table;
.type ___ctor_table,STT_OBJECT;
#endif  /* CPLUSCRT */
