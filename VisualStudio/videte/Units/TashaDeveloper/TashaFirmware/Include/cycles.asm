.section program;

.global _startc;
.global _stopc;
.global _getCyclesLo;
.global _getCyclesHi;


_startc:
	r0 = 0;				// Cycle Counter = 0
	cycles2 = r0;
	cycles 	= r0;
	r0 		= SYSCFG;	// Cycle Counter start
	bitset(r0,1);
	bitclr(r0,2);
	SYSCFG 	= r0;
	rts;
_startc.end:

_stopc:
	r0 		= SYSCFG;	// Cycle Counter stop
	bitclr (r0, 1);
	SYSCFG 	= r0;
	r0		=cycles;
	rts;
_stopc.end:

_getCyclesLo:
	r0 		= cycles;
	rts;
_getCyclesLo.end:

_getCyclesHi:
	r0 		= cycles;
	rts;
_getCyclesHi.end:	 
