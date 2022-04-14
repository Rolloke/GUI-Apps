/********************************************************************/
/* Cycle Counter													*/
/********************************************************************/
/* Autor: Christian Lohmeier										*/
/* Datum: 18.06.2003												*/
/* Firma: Videte IT	AG												*/
/********************************************************************/
#include <asm_sprt.h>
#include <defBF533.h>


.section program;
.global _startc;
.global _stopc;
.global _clk;

_startc:
r0 = 0;			// Cycle Counter = 0
cycles2 = r0;
cycles = r0;
r2 = SYSCFG;	// Cycle Counter start
bitset(r2,1);
SYSCFG = r2;
_startc.end:
rts;

_stopc:
r2 = SYSCFG;	// Cycle Counter stop
bitclr (r2, 1);
SYSCFG = r2;
_stopc.end:
rts;

_clk:

	p0.h = hi(PLL_CTL);	// Laden der oberen 16 Bit des PPL Registers
	p0.l = lo(PLL_CTL);	// Laden der unteren 16 Bit des PPL Registers

	CLI R2; 			// Interrupts aus, IMASK in r2 kopieren
	R1.L=0x2800; 		// VCO Faktor auf 21x 27MHz
	W[P0]=R1;			//  durch schreiben in PLL_CTL Register
	IDLE; 				// Wartet bis die Pipeline frei ist und PPL Wake Up eintrifft
	STI R2;				// Stellt die alte IMASK wieder her und Interrupts werden enabled
_clk.end:
rts;					// Zurück zum aufrufenden Programm

