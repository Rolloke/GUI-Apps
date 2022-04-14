 /*******************************************************************************
 *
 * $Source$
 * $Revision: 1 $
 *
 * Hawk-35 DSP Interrupt Test Example, header file.
 *
 * $Log: /Project/Units/Tasha(Developer)/TashaFirmware/TashaFirmware535/SharedMemoryDef.h $
 * 
 * 1     5.01.04 9:54 Martin.lueck
 * 
 * 16    27.10.03 10:22 Martin.lueck
 * 
 * 15    15.10.03 13:03 Martin.lueck
 *
 ******************************************************************************/
 
#ifndef _DEV_INTTST_H_
#define _DEV_INTTST_H_
#include "..\..\TashaFirmware\BlackFin\include\pci_win.h"
#include "..\..\TashaUnit\TashaUnitDefs.h"

typedef struct
{
	int 	nMessageID;
	DWORD	dwParam1;
	DWORD	dwParam2;
	DWORD	dwParam3;
	DWORD	dwParam4;
	BOOL	bAck;
}MessageStruct;

#ifndef WIN32
section ("DSP2PCMessageBuffer") volatile MessageStruct DSP2PCMessageBuffer;
section ("PC2DSPMessageBuffer") volatile MessageStruct PC2DSPMessageBuffer;
section ("InitDone")       		volatile WORD wInitDone = 0;
section ("Finish") 		   		volatile WORD wFinish = 0;

#else
#define DSP2PCMESSAGEBUFFER	(0x080+dlProgMem)
#define PC2DSPMESSAGEBUFFER	(0x098+dlProgMem)
#endif
#endif
