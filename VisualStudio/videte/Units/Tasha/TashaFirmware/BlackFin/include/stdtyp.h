/*******************************************************************************
 *
 * $Source$
 * $Revision: 1 $
 *
 * Raw type definitions file.
 *
 * $Log: /Project/Units/Tasha2/BlackFin/include/stdtyp.h $
 * 
 * 1     31.07.03 14:33 Martin.lueck
 * Wichtige Header und asm files für den BF535
 *
 ******************************************************************************/

#ifndef _STDTYP_H_
#define _STDTYP_H_

#ifdef _BLACKFIN_

typedef unsigned char uint8;
typedef signed char int8;

typedef unsigned short uint16;
typedef signed short int16;

typedef unsigned int uint32;
typedef signed int int32;

#else
#error "Blackfin architecture not defined"
#endif

#endif
