//////////////////////////////////////////////////////////////////
//																//
// Public "C"													//
// gemeinsam genutzte "C"- Funktionen 							//
// 																//
//////////////////////////////////////////////////////////////////
#include "..\BootBewegungTDM\main.h"

bool wait(BYTE4 nMicroSeconds)		// Warteschleife (0us.. 7,9s)
{
    BYTE4 nExpCycle = ((CCLK/1000000)*nMicroSeconds);
	if (nMicroSeconds > 7900000)
    	return(false); 
	startc();
    while (getCyclesLo() <= nExpCycle)
    	asm("nop;");
    return(true);
}
