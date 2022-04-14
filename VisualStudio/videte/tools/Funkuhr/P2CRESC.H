/*-------------------------------------------------------------------------- *
 *
 *   QuickStep P2C Translator Library                       V 2.06-TP
 *
 *   (c) Copyright 1987-90 by LAUER & WALLWITZ GmbH, D-6200 Wiesbaden
 *
 *   P2CRESC.H : system resource definitions
 *
 *-------------------------------------------------------------------------- */

#if defined(__TURBOC__)
#define TURBOC 1                /* automatic compiler recognition */
#elif defined(__TSC__)
#define TOPC   1
#elif defined(M_I86)
#define MSC    1
#else
#define ANSIC  1
#endif

/*
    Program code translated by P2C may only be compiled in the compact,large
    or huge memory models
*/
#if defined(TURBOC)
#if (__TINY__ || __SMALL__ || __MEDIUM__)
#error wrong model use compact large or huge model
#endif
#elif defined(MSC) || defined(TOPC) 
#if defined(M_I86SM) || defined(M_I86MM)
#error wrong model use compact large or huge model
#endif
#endif

typedef void          VOID  ,*PVOID;
typedef unsigned int  BOOL  ,*PBOOL;
typedef   signed char CHAR  ,*PSZ;
typedef   signed char SBYTE ,*PSBYTE;
typedef unsigned char BYTE  ,*PBYTE;
typedef   signed int  SHORT ,*PSHORT;
typedef unsigned int  USHORT,*PUSHORT;
typedef   signed long LONG  ,*PLONG;
typedef unsigned long ULONG ,*PULONG;
typedef long double   EXTENDED;

#define FALSE         0
#define TRUE          1

#undef SHRT_MAX
#undef LONG_MAX
#if defined(TURBOC)
#define SHRT_MAX      0x7FFFU
#define LONG_MAX      0x7FFFFFFFL
#else
#define SHRT_MAX      32767
#define LONG_MAX      2147483647L
#endif
#ifndef MA_PI
#define MA_PI	      3.14159265358979323846
#endif
#ifndef NULL
#define NULL          0L
#endif
