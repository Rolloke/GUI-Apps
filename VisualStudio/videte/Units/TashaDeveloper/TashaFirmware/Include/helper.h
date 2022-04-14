/////////////////////////////////////////////////////////////////////////////
// PROJECT:		JaCobDll
// FILE:		$Workfile: helper.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha(Developer)/TashaFirmware/Include/helper.h $
// CHECKIN:		$Date: 5.01.04 9:54 $
// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 5.01.04 9:25 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __HELPER_H__
#define __HELPER_H__

// delete macros, that check for NULL AND reset to NULL

////////////////////////////////////////////////////////////////
 
#ifndef WIN32

extern "C" void startc();
extern "C" int  stopc();
extern "C" int  getCyclesLo();
extern "C" int  getCyclesHi();


typedef unsigned long	DWORD;
typedef unsigned short	WORD;
typedef unsigned char	BYTE;
typedef const char*		LPCSTR;
typedef int				BOOL;
typedef long			LONG;
typedef unsigned short	BYTE2;
typedef unsigned long	BYTE4;

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
// #define NULL					0

#define	B0	0xFF000000
#define	B1	0x00FF0000
#define	B2	0x0000FF00
#define	B3	0x000000FF

#define	w0	0xFF00
#define	w1	0x00FF

#define SwapEndian(x) 	((((x)<<8)&B0)|(((x)<<8)&B2)|(((x)>>8)&B1)|(((x)>>8)&B3))
#define SwapEndianSmal(x) 	((((x)<<8)&w0)|(((x)>>8)&w1))

typedef struct
{
	BYTE Y;
	BYTE UV;
}YUV;

// Struktir zur Kommunikation des BF535 mit den BF533 über SPI1
typedef struct
{
	int		nPending;
	int 	nMessageID;
	DWORD	dwUserData;
	WORD	wSlave;
	DWORD	dwParam1;
	DWORD	dwParam2;
}SPIPacketStruct;

#define SETBIT(w,b)    ((DWORD)((DWORD)(w) | (1L << (b))))
#define CLRBIT(w,b)    ((DWORD)((DWORD)(w) & ~(1L << (b))))
#define TSTBIT(w,b)    ((BOOL)((DWORD)(w) & (1L << (b)) ? TRUE : FALSE))
#define TOGGLEBIT(w,b) (TSTBIT(w,b) ? CLRBIT(w,b) : SETBIT(w,b))

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

#endif //__HELPER_H__
