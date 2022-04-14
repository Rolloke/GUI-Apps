#ifdef DOCU
/*----------------------------------------------------+-------*/
/*                                                    |  ITK  */
/*   SYSTEM   universal                               +-------*/
/*                                                            */
/*------------------------------------------------------------*/
/*                                                            */
/*   $Workfile::   null.h                                   $ */
/*   $Revision::   1.5                                      $ */
/*       $Date::   08 Jun 1995 10:53:34                     $ */
/*     $Author::   ELLIGER                                  $ */
/*                                                            */
/*                                                            */
/*   ITK                                                      */
/*   Emil-Figge-Strasse 80                                    */
/*   D-44247 Dortmund                                         */
/*   Telefon 0231/9747-0                                      */
/*                                                            */
/*   TERMINOLOGY                                              */
/*     -                                                      */
/*                                                            */
/*   DESCRIPTION                                              */
/*     allgemeine Definitionen                                */
/*                                                            */
/*                                                            */
/*------------------------------------------------------------*/
/*                                                            */
/* $Log::   M:/ixeins/inc/null.h_v                          $ *
 * 
 *    Rev 1.5   08 Jun 1995 10:53:34   ELLIGER
 * Cisco_V1.02_and_basic-SW_V2.42_08.06.95
 * 
 *    Rev 1.4   07 Feb 1995 14:14:04   ELLIGER
 * Cisco_Zwischenversion_07.02.94
 * 
 *    Rev 1.3   18 Nov 1994 13:01:20   ELLIGER
 * Karten-Software_Version_2.31
 * 
 *    Rev 1.2   28 Jun 1994 11:44:06   ELLIGER
 * zur_Kartensoftware_Version_2.23
 *
 *    Rev 1.1   03 Feb 1994 12:49:28   ELLIGER
 * OS/2_CAPI_Version_1.03_vom_03.02.94
 *
 *    Rev 1.0   25 May 1993 11:16:18   MIPO
 * FirstVersion
 *  08.03.93   Frank Heidemann  FP_SEG und FP_OFF-Makros
 *                              geÑndert
 *  12.03.93   Frank Heidemann  SUCC and PRED Macro hinzugefÅgt
 *  05.10.93   Frank Heidemann  FP_OFF von FP_SEG getrennt
 *  07.10.93   Frank Heidemann  FAR und NEAR extra fÅr NW3_NLM
 *  18.05.94   Frank Heidemann  FP_OFF geÑndert, damit "segment lost
 *                              in Conversion" nicht mehr auftritt
 *                                                            */
/*------------------------------------------------------------*/
#endif

#ifndef NULL_DEF    /* damit nicht alles doppelt definiert wird !! */
#define NULL_DEF


/*#ifdef ANSI
  #define FAR
  #define NEAR
#else
  #if defined(NW3_NLM) || defined (NEWPORT)
    #define FAR
    #define NEAR
  #else
    #define FAR  _far
    #define NEAR _near
  #endif
#endif */


#ifndef NULL
  #define NULL  ((void *)0)
#endif


#define TRUE    1
#define FALSE   0

#define FOREVER for(;;)


/******** module handling ********************************************/
#define   PRIVATE    static
#define   PUBLIC
#define   EXTERNAL   extern


/******** ASCII-Characters *******************************************/
#define     BELL                   0x07
#define     CR                     0x0d
#define     LF                     0x0a
#define     BS                     0x08
#define     TAB                    0x09
#define     SP                     0x20
#define     DEL                    0x7f


/******** useful type definitions ************************************/

#define CHAR     char                   /* only for chars and strings */
typedef signed char             SBYTE;          /*  8 bit signed integer   */
typedef unsigned char           UBYTE;          /*  8 bit unsigned integer */
typedef signed short            SWORD;          /* 16 bit signed integer   */
typedef unsigned short          UWORD;          /* 16 bit unsigned integer */
typedef signed long             SLWORD;         /* 32 bit signed integer   */
typedef unsigned long           ULWORD;         /* 32 bit unsigned integer */
typedef float                   REAL;           /* 32 bit floating point   */
typedef double                  LREAL;          /* 64 bit floating point   */
typedef unsigned long           SYSTEM_ID;      /* 32 bit SYSTEM WIDE ID TYPE     */
typedef unsigned long           SYSPTR;         /* 32 bit pointer          */
//typedef unsigned int            BOOLEAN;
typedef char                    *STRING;
typedef char FAR                *FAR_ADDRESS;


struct  UPOINTER
	{       UWORD offset;
		UWORD segment;
	};

union address
	{       struct UPOINTER fadr;
		long            lval;
	};

#define far_adr  union address

/*---------------------------------------*/
/* some other useful typedef definitions */
/*---------------------------------------*/

typedef UBYTE  (*PFB)  (void);  /* pointer to function returning an unsigned byte    */
typedef UWORD  (*PFW)  (void);  /* pointer to function returning an unsigned word    */
typedef ULWORD (*PFL)  (void);  /* pointer to function returning an unsigned long    */
typedef void   (*PFV)  (void);  /* pointer to function returning nothing          */
typedef void   (FAR *FPFV) (void);   /* far pointer to function returning nothing */
typedef UWORD  (FAR *FPFW) (void);   /* far pointer to function returning unsigned word */

/*-------------------------------------------------------------------*/
/* macro to build a far pointer using segment number and offset      */
/*-------------------------------------------------------------------*/

#ifndef MK_FP
  #define     MK_FP(seg,ofs)         ((void FAR *)((((ULWORD)(seg))<<16)|(UWORD)ofs))
#endif


/*-------------------------------------------------------------------*/
/* macros to break  C "far" pointers into their segment and offset   */
/* components                                                        */
/*-------------------------------------------------------------------*/

#ifndef FP_SEG
  #define FP_SEG(fp)     ((UWORD) (((ULWORD) (fp)) >> 16) )
#endif

#ifndef FP_OFF
  #define FP_OFF(fp)     ((UWORD) ((ULWORD) (fp)) )
#endif



/* ------------------------------------------------------------------*/
/*  one can use D(x) to make statemets only valid for debug purposes */
/* ------------------------------------------------------------------*/

#ifdef DEV
#define D(x)    x
#else
#define D(x)
#endif



/* ------------------------------------------------------------------*/
/*  successor and predassessor to access array fields cyclicly       */
/* ------------------------------------------------------------------*/
#ifndef SUCC
  #define     SUCC( i, n )        (((i)+1)%(n))
  #define     PRED( i, n )        (((i)+(n)-1)%(n))
#endif



#endif

