/********************************************************************/
/* Main Header BF535												*/
/********************************************************************/
/* Autor: Christian Lohmeier										*/
/* Datum: 18.06.2003												*/
/* Firma: Videte IT	AG												*/
/********************************************************************/
#ifndef  __MAIN_DEFINED
	#define __MAIN_DEFINED

//--------------------------------------------------------------------------//
// Header files																//
//--------------------------------------------------------------------------//
#include <sys\exception.h>
#include <cdefBF535.h>


//--------------------------------------------------------------------------//
// Symbolic constants														//
//--------------------------------------------------------------------------//


#define SPI1_BAUDRATE	1000000 	//1 MHz

#define pALTERA_REG ((volatile BYTE *)0x24000000)
#define pALTERA_REG_END ((volatile BYTE *)0x24000020)

#ifndef WIN32

typedef unsigned char	BYTE;
typedef unsigned short	BYTE2;
typedef unsigned int	BYTE4;
typedef unsigned long	DWORD;
typedef unsigned short	WORD;
typedef const char*		LPCSTR;
typedef int				BOOL;
typedef long			LONG;

#define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#define LOWORD(l)           ((WORD)(l & 0xFFFF))
#define HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))
#define WK_DELETE(ptr)			{if(ptr){delete ptr; ptr=NULL;}}
#define WK_DELETE_ARRAY(ptr)	{if(ptr){delete [] ptr; ptr=NULL;}}
#define MAX(a,b)				(((a)>(b))?(a):(b))
#define MIN(a,b)				(((a)<(b))?(a):(b))
#define max(a,b)				MAX(a,b)
#define min(a,b)				MIN(a,b)
#define TRUE					1
#define FALSE					0
//#define NULL					0

typedef struct
{
	BYTE Y;
	BYTE UV;
}YUV;

// SYSTEM_CLK
#define SYSTEM_CLK 				   	133000000

#define SETBIT(w,b)    ((DWORD)((DWORD)(w) | (1L << (b))))
#define CLRBIT(w,b)    ((DWORD)((DWORD)(w) & ~(1L << (b))))
#define TSTBIT(w,b)    ((BOOL)((DWORD)(w) & (1L << (b)) ? TRUE : FALSE))
#define TOGGLEBIT(w,b) (TSTBIT(w,b) ? CLRBIT(w,b) : SETBIT(w,b))


#define PF0 	0x0001
#define PF1 	0x0002
#define PF2 	0x0004
#define PF3 	0x0008
#define PF4 	0x0010
#define PF5 	0x0020
#define PF6 	0x0040
#define PF7 	0x0080
#define PF8 	0x0100
#define PF9 	0x0200
#define PF10 	0x0400
#define PF11 	0x0800
#define PF12 	0x1000
#define PF13 	0x2000
#define PF14 	0x4000
#define PF15 	0x8000

//Altera Porogrammierung

#define nCONFIG 	PF10
#define CONF_DONE	PF11
#define DCOMI 		0x00000001;


#define SAVE_USER_REGS\
		asm("[--SP] = (R7:0,P5:0);");\
		asm("[--SP] = I3;");\
		asm("[--SP] = I2;");\
		asm("[--SP] = I1;");\
		asm("[--SP] = I0;");\
		asm("[--SP] = M3;");\
		asm("[--SP] = M2;");\
		asm("[--SP] = M1;");\
		asm("[--SP] = M0;");\
		asm("[--SP] = B3;");\
		asm("[--SP] = B2;");\
		asm("[--SP] = B1;");\
		asm("[--SP] = B0;");\
		asm("[--SP] = L3;");\
		asm("[--SP] = L2;");\
		asm("[--SP] = L1;");\
		asm("[--SP] = L0;");\
		asm("[--SP] = A0.x;");\
		asm("[--SP] = A0.w;");\
		asm("[--SP] = A1.x;");\
		asm("[--SP] = A1.w;");\
		asm("[--SP] = LC1;");\
		asm("[--SP] = LC0;");\
		asm("[--SP] = LT1;");\
		asm("[--SP] = LT0;");\
		asm("[--SP] = LB1;");\
		asm("[--SP] = LB0;")

		
// Restore all user registers except USP,SP, and FP
// Can be used in either Supervisor or user mode
#define RESTORE_USER_REGS\
		asm("LB0 = [SP++];");\
		asm("LB1 = [SP++];");\
		asm("LT0 = [SP++];");\
		asm("LT1 = [SP++];");\
		asm("LC0 = [SP++];");\
		asm("LC1 = [SP++];");\
		asm("A1.w = [SP++];");\
		asm("A1.x = [SP++];");\
		asm("A0.w = [SP++];");\
		asm("A0.x = [SP++];");\
		asm("L0 = [SP++];");\
		asm("L1 = [SP++];");\
		asm("L2 = [SP++];");\
		asm("L3 = [SP++];");\
		asm("B0 = [SP++];");\
		asm("B1 = [SP++];");\
		asm("B2 = [SP++];");\
		asm("B3 = [SP++];");\
		asm("M0 = [SP++];");\
		asm("M1 = [SP++];");\
		asm("M2 = [SP++];");\
		asm("M3 = [SP++];");\
		asm("I0 = [SP++];");\
		asm("I1 = [SP++];");\
		asm("I2 = [SP++];");\
		asm("I3 = [SP++];");\
		asm("(R7:0,P5:0) = [SP++];")
		
// Save all supervisor only registers except USP,SP, and FP
// Can only be used in Supervisor mode
#define SAVE_SUPER_REGS\
		asm("[--SP] = ASTAT;");\
		asm("[--SP] = RETS;");\
		asm("[--SP] = RETI;");\
		asm("[--SP] = RETX;");\
		asm("[--SP] = RETN;");\
		asm("[--SP] = RETE;");\
		asm("[--SP] = SEQSTAT;");\
		asm("[--SP] = SYSCFG;")


// Restore all supervisor only registers except USP,SP, and FP
// Can only be used in Supervisor mode
#define RESTORE_SUPER_REGS\
		asm("SYSCFG = [SP++];");\
		asm("SEQSTAT = [SP++];");\
		asm("RETE = [SP++];");\
		asm("RETN = [SP++];");\
		asm("RETX = [SP++];");\
		asm("RETI = [SP++];");\
		asm("RETS = [SP++];");\
		asm("ASTAT = [SP++];")

#endif // WIN32
//--------------------------------------------------------------------------//
// Globale Variablen																//
//--------------------------------------------------------------------------//
extern BYTE byBuffer[];
//extern BYTE2 usAlteraFile[];


//--------------------------------------------------------------------------//
// Prototypes																//
//--------------------------------------------------------------------------//
// in main.c

void wait();
// in file ISR.c

EX_INTERRUPT_HANDLER(SPI_ISR);
#endif __MAIN_DEFINED
