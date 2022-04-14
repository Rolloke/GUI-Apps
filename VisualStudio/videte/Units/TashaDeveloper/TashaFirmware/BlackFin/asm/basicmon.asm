/*******************************************************************************
 *
 * $Source$
 * $Revision$
 *
 * Hawk-35 Monitor API functions.
 *
 * $Log$
 *
 ******************************************************************************/

#include <defBF535.h>
#include "m21535.h"
#include "basicmon.h"

//..............................................................................

#if defined(_HAWK_35_) // [ _HAWK_35_ 

//------------------------------------------------------------------------------

	.section abs_monitor_data;	

	// Exported symbol names

	.global _InitBasicMon;
	.global _InterruptPC;
	.global _SetInterruptCallback;


// Pending flag, 1 if interrupt DSP->host is pending, 0 if not

userDspToPcInt:   .byte2 = 0;

// Enable flag, 1 if interrupt DSP->host is allowed, 0 if not

okUserDspToPcInt: .byte2 = 0;

isrCtrlWrd: .byte2 = 0;

// Mutex1 Used in handshaking between DSP-host

mutex1:     .byte2 = 0;

// Mutex0 Used in handshaking between DSP-host

mutex0:     .byte2 = 0;

//------------------------------------------------------------------------------

	.section program;
	.align 4;

userCallbackAddress: .byte4 = NullCallback;

//------------------------------------------------------------------------------
//
// InitBasicMon:
//
//   Uses r0,r1,p0.
//
//------------------------------------------------------------------------------

_InitBasicMon:

	// Disable user callbacks.

	r0.l = NullCallback;
	r0.h = NullCallback;
	p0.l = userCallbackAddress;
	p0.h = userCallbackAddress;
	// store callback address
	[p0] = r0;	

	// Enable host->DSP interrupt. 

#if (ILAT_INTERRUPT == 1)
	p0.l = EVT14 & 0xffff;
	p0.h = EVT14 >> 16;
#else
	p0.l = EVT12 & 0xffff;
	p0.h = EVT12 >> 16;
#endif
	p1.l = MonitorIsr;
	p1.h = MonitorIsr;
	[p0] = p1;

#if (ILAT_INTERRUPT == 1)
	cli r0;#if (ILAT
	bitset(r0,bIVG14); 	// enable core interrupt 14
	sti r0;
#else
   	p0.l = FIO_DIR & 0xffff;
	p0.h = FIO_DIR >> 16;
	r0 = w[p0](z);
	r1 = kPF14(z);     // Set PF14 to be an output.      
                       // I can only have level-triggered interrupts
                       // using output pins and I want an edge-triggered.
		               // Tried to use edge-triggered input pin but for some
					   // reason it doesn't work?
	r0 = r0 | r1;
	w[p0] = r0.l;			
   	ssync;

	// Make pf14 level-sensitive.
	// Have tried to use edge-triggered interrupt with loopback (pf14->pf13) 
	// but then I can't make pf go from high to low for some reason only from 
	// low to high for the input pin?
   	p0.l = FIO_EDGE & 0xffff;
	p0.h = FIO_EDGE >> 16;
	r0 = w[p0](z);
	r1 = (~kPF14 & 0xffff)(z); // pf14 level-sensitive       
	r0 = r0 & r1;
	w[p0] = r0.l; 
  	ssync;

	// before enabling the interrupt make sure the flag is 0
	// otherwise I could have an old interrupt showing up
	P0.L = FIO_FLAG_C & 0xFFFF;
	P0.H = FIO_FLAG_C >> 16;

	r0.l = kPF14;
	w[p0] = r0.l;	// clear flag
	ssync;

   	p0.l = FIO_MASKA_S & 0xffff;
	p0.h = FIO_MASKA_S >> 16;
	r1 = kPF14(z);
	w[p0] = r1.l;     // enable pf14 flag A interrupt
  	ssync;

	// enable system interrupt
   	p0.l = SIC_IMASK & 0xffff;
	p0.h = SIC_IMASK >> 16;
	r0 = [p0];
  	ssync;

	r1.l = SIC_MASK17 & 0xffff;
	r1.h = SIC_MASK17 >> 16;
	r0 = r0 | r1;     // "or in" pf interrupt a
	[p0] = r0;
  	ssync;

	// pf interrupt a will show up on IVG12
	cli r0;
	bitset(r0,bIVG12); 	   // enable core interrupt 12
	sti r0;

#endif

	// Enable INTA.

	p0.l = PCI_ICTL & 0xffff;
	p0.h = PCI_ICTL >> 16;	
	r0 = [p0];
	r1 = kPCI_INTAEn;
	r0 = r0 | r1;
	[p0] = r0;
	ssync;

	rts;
_InitBasicMon.end:

//------------------------------------------------------------------------------
//
// HandShake:
//
//	 DSP-host Synchronization Routine
//
//   Uses: r0,p0,p1.
//
//
//------------------------------------------------------------------------------

HandShake:

	p0.l = mutex0;
	p0.h = mutex0;
	p1.l = mutex1;
	p1.h = mutex1;

waitSet:					// wait for host to set mutex0 
	r0 = w[p0](z);
	cc = r0;
	if !cc jump waitSet;

	r0.l = 1;
	w[p1] = r0.l;           // set mutex1

waitClr:
	r0 = w[p0](z);
	cc = r0;
	if cc jump waitClr;

	r0.l = 0;
	w[p1] = r0.l;           // clear mutex1

	rts;

//------------------------------------------------------------------------------

MonitorIsr:

	// context save
 	[--sp] = r0;
 	[--sp] = r1;
    [--sp] = p0;
    [--sp] = p1;
	[--sp] = RETS;		// nested subroutines might be in use
	[--sp] = astat;

    // first handshake
	call HandShake;

#if (ILAT_INTERRUPT != 1)

	// deassert level-triggered PC->DSP interrupt so we won't get a 2nd one 	
	// after leaving here

	p0.l = FIO_FLAG_C & 0xffff;
	p0.h = FIO_FLAG_C >> 16;

	r0.l = kPF14;
	w[p0] = r0.l;	// clear flag interrupt

#endif

	p0.l = isrCtrlWrd;	
	p0.h = isrCtrlWrd;	
	r0 = w[p0](z); 
	cc = bittst(r0,bMonClearInterrupt);		
	if !cc jump passedClr; 	

	// Deassert DSP->host interrupt.

	p1.l = PCI_CTL & 0xffff;
	p1.h = PCI_CTL >> 16;

//	NOTE: I PROBABLY NEED A LOCK HERE FOR THE PCI_CTL register since PC will want to
//		  check who's issuing the interrupt by querying this register.

	r0 = [p1];
	r1 = ~kPCI_IntAToPCI;
	r0 = r0 & r1; // deassert interrupt
	[p1] = r0;
	ssync; 

passedClr:

	r0 = w[p0](z); 
	cc = bittst(r0,bMonUserInterrupt);		
	if !cc jump passedUser; 		

	// call user callback

	p0.l = userCallbackAddress;
	p0.h = userCallbackAddress;
	r0 = [p0];
	p0 = r0;
	call (p0); 

passedUser:

	// second handshake
	call HandShake;

	// context restore
	astat = [sp++];
	RETS = [sp++];
	p1 = [sp++];
	p0 = [sp++];
	r1 = [sp++];
	r0 = [sp++];

	rti;

//------------------------------------------------------------------------------
//
// InterruptPC:
//
//   Function that interrupts PC.
//
//   Uses: r0,r1,p0.
//
//   Output: r0 1: successful
//              0: failed
//
//------------------------------------------------------------------------------

_InterruptPC:

  	// Check that host->DSP interrupt is enabled. This needs to be ensured because
  	// host will acknowledge this interrupt with another interrupt.

   	p0.l = IMASK & 0xffff;
	p0.h = IMASK >> 16;

	r0 =  [p0];  	
#if (ILAT_INTERRUPT == 1)
	cc = bittst(r0,bIVG14);
#else
	cc = bittst(r0,bIVG12); // the check should be extended to check SIC_IMASK as well? 
#endif
	if !cc jump failedInt; // if not enabled exit

	// Check that there is no interrupt pending.

	p0.l = userDspToPcInt;	
	p0.h = userDspToPcInt;
 	r0 = w[p0](z);
	cc = r0;
	if cc jump failedInt;
	
	// Check that DSP->host interrupt is enabled.

	p0.l = okUserDspToPcInt;	
	p0.h = okUserDspToPcInt;
 	r0 = w[p0](z);
	cc = r0;
	if !cc jump failedInt;

	// Set interrupt to be pending.

	p0.l = userDspToPcInt;	
	p0.h = userDspToPcInt;
	r0.l = 1;
	w[p0] = r0.l;

	// Assert DSP->host interrupt.

	p0.l = PCI_CTL & 0xffff;
	p0.h = PCI_CTL >> 16;

//	NOTE: I PROBABLY NEED A LOCK HERE FOR THE PCI_CTL register since PC will want to
//		  check who's issuing the interrupt by querying this register.

	r0 = [p0];
	r1 = kPCI_IntAToPCI;
	r0 = r0 | r1; // generate interrupt
	[p0] = r0;
	ssync;

	r0 = 1(z);

	rts;

failedInt:
	r0 = 0(z);
	rts;

_InterruptPC.end:

//------------------------------------------------------------------------------
//
// SetInterruptCallback:
//
//   Function that installs callback function.
//
//   Uses: r0,p0.
//      
//   Input: r0: 0 callbacks are turned off
//	        r0: !0 address of callback
//
//------------------------------------------------------------------------------

_SetInterruptCallback:

	cc = r0;
	if cc jump addressOk;
	r0.l = NullCallback;
	r0.h = NullCallback;

addressOk:

	p0.l = userCallbackAddress;
	p0.h = userCallbackAddress;
	// store callback address
	[p0] = r0;

	rts;

_SetInterruptCallback.end:
//------------------------------------------------------------------------------

// Dummy callback function

NullCallback:

	rts;

//..............................................................................

#else              // ] [ !_HAWK_35_ 
#error "Board not defined"
#endif             // ] _HAWK_35_ 