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

.section program;

.global _startc;
.global _stopc;
.global _resumec;
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
	csync;
	rts;
_stopc.end:

_resumec:
	r0 		= SYSCFG;	// Cycle Counter start
	bitset(r0,1);
	SYSCFG 	= r0;
	rts;
_resumec.end:

_getCyclesLo:
	r0 		= cycles;
	csync;
	rts;
_getCyclesLo.end:

_getCyclesHi:
	r0 		= cycles2;
	csync;
	rts;
_getCyclesHi.end:	 
