/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaFirmware535
// FILE:		$Workfile: SharedMemoryDef.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/TashaFirmware535/SharedMemoryDef.h $
// CHECKIN:		$Date: 7.04.04 16:31 $
// VERSION:		$Revision: 19 $
// LAST CHANGE:	$Modtime: 7.04.04 15:19 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////
 
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
section ("PermMasks") 	   		volatile MASK_STRUCT PermMasks[CHANNEL_COUNT];

#else
#define DSP2PCMESSAGEBUFFER	(0x080+dlProgMem)
#define PC2DSPMESSAGEBUFFER	(0x098+dlProgMem)
#define PERMMASKS (0x718+dlProgMem)
#endif
#endif
