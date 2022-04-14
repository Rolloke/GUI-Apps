/*******************************************************************************
 *
 * $Source$
 * $Revision: 2 $
 *
 * GPIO functionality, header file.
 *
 * $Log: /Project/Units/Tasha/TashaFirmware/BlackFin/include/gpiotest.h $
 * 
 * 2     27.08.04 13:17 Martin.lueck
 * 
 * 1     31.07.03 14:32 Martin.lueck
 * Wichtige Header und asm files für den BF535
 *
 ******************************************************************************/

#ifndef _GPIOTEST_H_
#define _GPIOTEST_H_

//..............................................................................

#include "stdtyp.h"
// #include <def21535.h>
#include <defBF535.h> // Ab VC-DSP3.5

//..............................................................................

// Inline functions

#ifdef _PUB_                        // [ _PUB_

#define kVal 0x8000
#define kMix 0x0000 // doesn't make sense here but define it anyway

#endif                              // ] _PUB_

#ifdef _EZKITLITE_ // [ _EZKITLITE_

#define kVal 0x000f
#define kMix 0x000a

#endif             // ] _EZKITLITE_

#ifdef _HAWK_35_  // [ _HAWK_35_

#define kVal 0x0f00
#define kMix 0x0a00

#endif             // ] _HAWK_35_

#ifdef _EAGLE_35_  // [ _EAGLE_35_

#define kVal 0xff00
#define kMix 0xaa00

#endif             // ] _EAGLE_35_

#if defined(_EAGLE_35_) || defined(_HAWK_35_)

static inline uint16 getPushBtns(void) {
  uint16 val = *(volatile uint16 *)FIO_FLAG_S;
  asm("ssync;");
  return val;
}

static inline void outputLEDs(uint16 val) {
  *(volatile uint16 *)FIO_FLAG_S = val;
  *(volatile uint16 *)FIO_FLAG_C = ~val;
  asm("ssync;");
}

#endif             

#if defined(_EZKITLITE_) || defined(_EAGLE_35_) || defined(_HAWK_35_) || defined(_PUB_)

static inline void initLEDs(void) {
  *(volatile uint16 *)FIO_DIR = (uint16)kVal;
  asm("ssync;");
}

static inline void setLEDs(void) {
  *(volatile uint16 *)FIO_FLAG_S = (uint16)kVal;
  asm("ssync;");
}

static inline void mixLEDs(void) {
  *(volatile uint16 *)FIO_FLAG_C = (uint16)kVal;
  *(volatile uint16 *)FIO_FLAG_S = (uint16)kMix;
  asm("ssync;");
}

static inline void clearLEDs(void) {
  *(volatile uint16 *)FIO_FLAG_C = (uint16)kVal;
  asm("ssync;");
} 

#else 

#error "Board not defined"

#endif

#endif
