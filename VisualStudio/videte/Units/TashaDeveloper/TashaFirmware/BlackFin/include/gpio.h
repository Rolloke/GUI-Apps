/*******************************************************************************
 *
 * $Source$
 * $Revision: 1 $
 *
 * GPIO functionality, header file.
 *
 * $Log: /Project/Units/Tasha(Developer)/TashaFirmware/BlackFin/include/gpio.h $
 * 
 * 1     5.01.04 9:53 Martin.lueck
 * 
 * 1     31.07.03 14:32 Martin.lueck
 * Wichtige Header und asm files für den BF535
 *
 ******************************************************************************/

#ifndef _GPIO_H_
#define _GPIO_H_

#include "m21535.h"

//..............................................................................

#ifdef _EZKITLITE_                  // [ _EZKITLITE_

// GPIO output pins

#define kLEDReset \
		ZET(bPF0,0)+\
		ZET(bPF1,0)+\
		ZET(bPF2,0)+\
		ZET(bPF3,0)

#define kLEDMix \
		ZET(bPF0,0)+\
		ZET(bPF1,1)+\
		ZET(bPF2,0)+\
		ZET(bPF3,1)

#define kLEDMix3 \
		ZET(bPF0,0)+\
		ZET(bPF1,1)+\
		ZET(bPF2,1)+\
		ZET(bPF3,1)

#define kLEDSet	ZET(bPF0,1)+\
		ZET(bPF1,1)+\
		ZET(bPF2,1)+\
		ZET(bPF3,1)

#define kLEDSet0 \
		ZET(bPF0,1)+\
		ZET(bPF1,0)+\
		ZET(bPF2,0)+\
		ZET(bPF3,0)

#define kLEDSet1 \
		ZET(bPF0,0)+\
		ZET(bPF1,1)+\
		ZET(bPF2,0)+\
		ZET(bPF3,0)

#define kLEDSet2 \
		ZET(bPF0,0)+\
		ZET(bPF1,0)+\
		ZET(bPF2,1)+\
		ZET(bPF3,0)

#define kLEDSet3 \
		ZET(bPF0,0)+\
		ZET(bPF1,0)+\
		ZET(bPF2,0)+\
		ZET(bPF3,1)

#else                               // ] [ !_EZKITLITE_

#ifdef _EAGLE_35_                   // [ _EAGLE_35_

// PF15 reserved for Audio Codec reset line
// However an LED is attached to this output pin as well

// GPIO output pins

#define kLEDReset \
		ZET(bPF8,0)+\
		ZET(bPF9,0)+\
		ZET(bPF10,0)+\
		ZET(bPF11,0)+\
		ZET(bPF12,0)+\
		ZET(bPF13,0)+\
		ZET(bPF14,0)

#define kLEDMix \
		ZET(bPF8,0)+\
		ZET(bPF9,1)+\
		ZET(bPF10,0)+\
		ZET(bPF11,1)+\
		ZET(bPF12,0)+\
		ZET(bPF13,1)+\
		ZET(bPF14,0)

#define kLEDMix5 \
		ZET(bPF8,0)+\
		ZET(bPF9,0)+\
		ZET(bPF10,1)+\
		ZET(bPF11,1)+\
		ZET(bPF12,1)+\
		ZET(bPF13,1)+\
		ZET(bPF14,1)

#define kLEDSet	\
		ZET(bPF8,1)+\
		ZET(bPF9,1)+\
		ZET(bPF10,1)+\
		ZET(bPF11,1)+\
		ZET(bPF12,1)+\
		ZET(bPF13,1)+\
		ZET(bPF14,1)

#define kLEDSet0 \
		ZET(bPF8,1)+\
		ZET(bPF9,0)+\
		ZET(bPF10,0)+\
		ZET(bPF11,0)+\
		ZET(bPF12,0)+\
		ZET(bPF13,0)+\
		ZET(bPF14,0)

#define kLEDSet1 \
		ZET(bPF8,0)+\
		ZET(bPF9,1)+\
		ZET(bPF10,0)+\
		ZET(bPF11,0)+\
		ZET(bPF12,0)+\
		ZET(bPF13,0)+\
		ZET(bPF14,0)

#define kLEDSet2 \
		ZET(bPF8,0)+\
		ZET(bPF9,0)+\
		ZET(bPF10,1)+\
		ZET(bPF11,0)+\
		ZET(bPF12,0)+\
		ZET(bPF13,0)+\
		ZET(bPF14,0)

#define kLEDSet3 \
		ZET(bPF8,0)+\
		ZET(bPF9,0)+\
		ZET(bPF10,0)+\
		ZET(bPF11,1)+\
		ZET(bPF12,0)+\
		ZET(bPF13,0)+\
		ZET(bPF14,0)

#define kLEDSet4 \
		ZET(bPF8,0)+\
		ZET(bPF9,0)+\
		ZET(bPF10,0)+\
		ZET(bPF11,0)+\
		ZET(bPF12,1)+\
		ZET(bPF13,0)+\
		ZET(bPF14,0)

#define kLEDSet5 \
		ZET(bPF8,0)+\
		ZET(bPF9,0)+\
		ZET(bPF10,0)+\
		ZET(bPF11,0)+\
		ZET(bPF12,0)+\
		ZET(bPF13,1)+\
		ZET(bPF14,0)

#define kLEDSet6 \
		ZET(bPF8,0)+\
		ZET(bPF9,0)+\
		ZET(bPF10,0)+\
		ZET(bPF11,0)+\
		ZET(bPF12,0)+\
		ZET(bPF13,0)+\
		ZET(bPF14,1)

#else                               // ] [ !_EAGLE_35_

#ifdef _HAWK_35_                    // [ _HAWK_35_

// PF15 reserved for Audio Codec reset line

// GPIO output pins

#define kLEDReset \
		ZET(bPF8,0)+\
		ZET(bPF9,0)+\
		ZET(bPF10,0)+\
		ZET(bPF11,0)

#define kLEDSet	\
		ZET(bPF8,1)+\
		ZET(bPF9,1)+\
		ZET(bPF10,1)+\
		ZET(bPF11,1)

#define kLEDMix \
		ZET(bPF8,0)+\
		ZET(bPF9,1)+\
		ZET(bPF10,0)+\
		ZET(bPF11,1)

#define kLEDMix3 \
		ZET(bPF8,0)+\
		ZET(bPF9,1)+\
		ZET(bPF10,1)+\
		ZET(bPF11,1)

#else                               // ] [ !_HAWK_35_

#ifdef _PUB_                        // [ _PUB_

// PF15 goes to LED D6

// GPIO output pins

#define kLEDReset 0x0000

#define kLEDSet	  0x8000

#else                               // ] [ !_PUB_

#error "Board not defined"

#endif                              // ] _PUB_

#endif                              // ] _HAWK_35_

#endif                              // ] _EAGLE_35_

#endif                              // ] _EZKITLITE_

//..............................................................................

#endif
