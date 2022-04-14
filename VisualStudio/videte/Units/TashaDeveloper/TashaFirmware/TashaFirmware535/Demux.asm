#include <defBF535.h>
#include <asm_sprt.h>

.section program;

.global _Demux4Channels16Bit;
.global _Demux4Channels32Bit;

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
	
	// Es werden 8 Byte pro Schleifendurchlauf kopiert.
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


