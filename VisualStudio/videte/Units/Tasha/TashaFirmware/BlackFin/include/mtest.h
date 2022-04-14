/*******************************************************************************
 *
 * $Source$
 * $Revision: 1 $
 *
 * Memory test functionality, header file.
 *
 * $Log: /Project/Units/Tasha2/BlackFin/include/mtest.h $
 * 
 * 1     31.07.03 14:32 Martin.lueck
 * Wichtige Header und asm files für den BF535
 *
 ******************************************************************************/

#ifndef _MTEST_H_
#define _MTEST_H_

//..............................................................................

// Function prototypes

// TODO: interface should have addresses as (void *) and not as (uint32 *) to 
// allow e.g. 16-bit addresses
// dataBusWidth and addrBusWidth should rather be ints and not uint32s?

extern int runDataBusTest(uint32 dataBusWidth,uint32 *startAddr,uint32 *endAddr);
extern int runAddrBusTest(uint32 addrBusWidth,uint32 *startAddr,uint32 *endAddr);
extern int runRangeTest(uint32 dataBusWidth,uint32 *startAddr,uint32 *endAddr);

#endif
