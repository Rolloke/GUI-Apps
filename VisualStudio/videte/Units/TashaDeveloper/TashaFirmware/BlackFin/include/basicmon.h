/*******************************************************************************
 *
 * $Source$
 * $Revision: 1 $
 *
 * Hawk-35 Monitor API functions, header file.
 *
 * $Log: /Project/Units/Tasha(Developer)/TashaFirmware/BlackFin/include/basicmon.h $
 * 
 * 1     5.01.04 9:53 Martin.lueck
 * 
 * 1     31.07.03 14:32 Martin.lueck
 * Wichtige Header und asm files für den BF535
 *
 ******************************************************************************/

#ifndef _BASICMON_H_
#define _BASICMON_H_

#include "m21535.h"
#include "zet.h"
#include "pci_win.h"

// Change ILAT_INTERRUPT to 1 if it becomes writable in future revision of DSP.

#define ILAT_INTERRUPT 0

#if (ILAT_INTERRUPT == 1)
#define bIVG14 14
#define kIVG14_ILAT (1<<14)
#else
#define kPF14  ZET(bPF14,1)
#define bIVG12 12
#define kIVG12 ZET(bIVG12,1)
#endif

// Isr Control word, 1 this is an acknowledge of an interrupt first issued by
//                     by the DSP
//                   2 this interrupt is issed by the host so call DSP
//                     callback function

#define bMonClearInterrupt 0
#define bMonUserInterrupt  1

#define kMonClearInterrupt ZET(bMonClearInterrupt,1)
#define kMonUserInterrupt  ZET(bMonUserInterrupt,1)

//..............................................................................

#define USERDSPTOPCINT   (0x4+dlProgMem)
#define OKUSERDSPTOPCINT (0x6+dlProgMem)

#define ISR_CTRL_WRD    (0x8+dlProgMem)
#define MUTEX1          (0xa+dlProgMem)
#define MUTEX0          (0xc+dlProgMem)

#endif
