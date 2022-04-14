/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaFirmware535
// FILE:		$Workfile: TashaFirmware535.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/TashaFirmware535/TashaFirmware535.cpp $
// CHECKIN:		$Date: 19.01.04 11:18 $
// VERSION:		$Revision: 82 $
// LAST CHANGE:	$Modtime: 19.01.04 11:18 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include <defBF535.h>
#include <asm_sprt.h>

.section program;

.global _Demux4Channels16Bit;
.global _Demux4Channels32Bit;
.global _SetVCOMultiplier;
//.global _SetVCOMultiplier2;

#define UPPER_( x ) (((x) >> 16) & 0x0000FFFF)  
#define LOWER_( x ) ((x) & 0x0000FFFF)  
#define PLL_MSEL 		2
#define PLL_D 	 		0
#define PLL_LOCK_COUNT 	10000

.extern start;

_SetVCOMultiplier:

	P0.H = UPPER_(PLL_CTL);
	P0.L = LOWER_(PLL_CTL);
	
	R1.H = 0x0001; 	// Set BYPASS bit in PLL_CTL without modifying MSEL  
	R1.L = 0x1900;	// because we must be in BYPASS to change MSEL
	[P0] = R1;		//  
	SSYNC;

	CLI R2;			// Disable Interrupts, copy IMASK to R1
	IDLE;
	SSYNC;
	STI R2;			// Restore interrupts and IMASK
	
	R1.H = 0x0000;	// Set CLKIN to CCLK multiplier to 7x in PLL_CTL
	R1.L = 0x0F00;	// keeping BYPASS bit set as both MSEL and BYPASS
	[P0] = R1;		// cannot be changed simultaneously
	SSYNC;
		
	R1.H = 0x0000;	// Clear BYPASS bit in PLL_CTL, satisfying the requirement of
	R1.L = 0x0E00;	// breaking simultaneous changes to MSEL and BYPASS
	[P0] = R1;
	SSYNC;
	
	CLI R2;			// Disable interrupts, copy IMASK to R1
	IDLE;
	SSYNC;
	STI R2;			// Restore interrupts and IMASK
	
	idle;			// warum auch immer???
	
	jump start;
//  rts;	
/*
_SetVCOMultiplier:
	P0.H = UPPER_(PLL_CTL);
	P0.L = LOWER_(PLL_CTL);
	
	R1.H = 0x0000; 	// Set BYPASS bit in PLL_CTL without modifying MSEL  
	R1.L = 0x3F00;	// because we must be in BYPASS to change MSEL
	[P0] = R1;		//  "
	SSYNC;

	CLI R1;			// Disable Interrupts, copy IMASK to R1
	IDLE;
	SSYNC;
	STI R1;			// Restore interrupts and IMASK
	
	R1.H = 0x0000;	// Set CLKIN to CCLK multiplier to 2x in PLL_CTL
//	R1.L = 0x0500;	// keeping BYPASS bit set as both MSEL and BYPASS
	R1.L = 0x1700;	// keeping BYPASS bit set as both MSEL and BYPASS
	[P0] = R1;		// cannot be changed simultaneously
	SSYNC;
		
	R1.H = 0x0000;	// Clear BYPASS bit in PLL_CTL, satisfying the requirement of
//	R1.L = 0x0400;	// breaking simultaneous changes to MSEL and BYPASS
	R1.L = 0x1600;	// breaking simultaneous changes to MSEL and BYPASS
	[P0] = R1;
	SSYNC;
	
	CLI R1;			// Disable interrupts, copy IMASK to R1
	IDLE;
	SSYNC;
	STI R1;			// Restore interrupts and IMASK

	rts;
_SetVCOMultiplier.end:

// Reconfigure PLL_CTL Register
_SetVCOMultiplier2:
#if defined(PLL_MSEL)  || defined(PLL_D)

	#ifdef PLL_LOCK_COUNT
		p0.l = LOWER_(PLL_LOCKCNT);
		p0.h = UPPER_(PLL_LOCKCNT);		
		r0.l = PLL_LOCK_COUNT & 0xffff;
		r0.h = (PLL_LOCK_COUNT >> 16) & 0xffff;
		w[p0] = r0;	
	#endif	//PLL_LOCK_COUNT

	p0.l = LOWER_(PLL_CTL);
	p0.h = UPPER_(PLL_CTL);
	r1 = w[p0](z);
	r0 = 0(z);
		
	#ifdef PLL_D
		bitclr(r1,0);
		r0.l = (PLL_D & 0x1);
		r1 = r1 | r0;
	#else
		bitset(r1,0);
		r0.l = (PLL_D & 0x1);
		r1 = r1 | r0;
	#endif // PLL_D
	
	#ifdef PLL_MSEL
		r0.l = ~(0x3f << 9);
		r1 = r1 & r0;
		r0.l = ((PLL_MSEL & 0x3f) << 9);
		r1 = r1 | r0;
	#endif // PLL_MSEL
	
	w[p0] = r1;
	ssync;
	
	// Apply PLL_CTL changes. Because I am in the reset routine
	// there is no need to disable(cli)/enable(sti) interrupts. 
	idle;	// wait for Loop_count expired wake up (no ssync required).
	
#endif //(PLL_MSEL | PLL_D)

//-------------------------------------
	rts;
*/
_SetVCOMultiplier2.end:

// BOOL Demux4Channels16Bit(WORD* pDestBase, WORD* pSrcBase, int nLength);
// Ausführungszeit: 22523 cycle
_Demux4Channels16Bit:
	[--SP] = (P5:0);
	[--SP] = R3;
	[--SP] = I3;
	[--SP] = I2;
	[--SP] = I1;
	[--SP] = I0;
	[--SP] = lc0;
	
	I0 = R0;		// DestBufferBase		
	P0 = R1;		// SourceBufferBase
	P4 = R2;		// Länge in Bytes		
	
	R3 = R2>>2;		// Länge eines SubBuffers (Lenght/4)
	R0 = R0+R3;
	I1 = R0;		// Beginn des 2. SubBuffers (Channel2)
	R0 = R0+R3;
	I2 = R0;		// Beginn des 3. SubBuffers (Channel3)
	R0 = R0+R3;
	I3 = R0;		// Beginn des 4. SubBuffers (Channel4)
	
	P4 = P4>>1;		// Div 8 (Es werden 8 Byte pro Schleifendurchlauf kopiert.(4*16Bits))
	P4 = P4>>2;		//		"
	
	R0 = [P0++];
	lsetup(begin_loop16, end_loop16) lc0=P4;
	
	// Es werden 8 Byte pro Schleifendurchlauf kopiert.
	begin_loop16:	    																					
					W[I0++] = R0.L||R1 = [P0++];
					W[I1++] = R0.H;
					W[I2++] = R1.L||R0 = [P0++];													
	end_loop16:		W[I3++] = R1.H; 	   	

	lc0 	= [SP++];
	I0 		= [SP++];
	I1 		= [SP++];
	I2 		= [SP++];
	I3 		= [SP++];
	R3		= [SP++];
	(P5:0) 	= [SP++];
	
	R0 = 1;			// return True
	rts;
_Demux4Channels16Bit.end:

// BOOL Demux4Channels32Bit(WORD* pDestBase, WORD* pSrcBase, int nLength);
// Ausführungszeit für 4KByte: 1311 cycle (4,37ys bei 300MHz)

_Demux4Channels32Bit:
	[--SP] = (P5:0);
	[--SP] = R3;
	[--SP] = I3;
	[--SP] = I2;
	[--SP] = I1;
	[--SP] = I0;
	[--SP] = lc0;
	
	I0 = R0;		// DestBufferBase		
	P0 = R1;		// SourceBufferBase
	P4 = R2;		// Länge in Bytes		
	
	R3 = R2>>2;		// Länge eines SubBuffers (Lenght/4)
	R0 = R0+R3;
	I1 = R0;		// Beginn des 2. SubBuffers (Channel2)
	R0 = R0+R3;
	I2 = R0;		// Beginn des 3. SubBuffers (Channel3)
	R0 = R0+R3;
	I3 = R0;		// Beginn des 4. SubBuffers (Channel4)
	
	P4 = P4>>2;		// Div 16 (Es werden 16 Byte pro Schleifendurchlauf kopiert.(4*32Bits))
	P4 = P4>>2;		//		"
	
	R0 = [P0++];
	lsetup(begin_loop32, end_loop32) lc0=P4;
	
	// Es werden 16 Byte pro Schleifendurchlauf kopiert.
	begin_loop32:	    																					
					[I0++] = R0||R1 = [P0++];
					[I1++] = R1||R0 = [P0++];													
					[I2++] = R0||R1 = [P0++];													
	end_loop32:		[I3++] = R1||R0 = [P0++]; 	   	

	lc0 	= [SP++];
	I0 		= [SP++];
	I1 		= [SP++];
	I2 		= [SP++];
	I3 		= [SP++];
	R3		= [SP++];
	(P5:0) 	= [SP++];
	
	R0 = 1;			// return True
	rts;
_Demux4Channels32Bit.end:


