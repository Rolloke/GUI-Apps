/*******************************************************************************
 *
 * $Source$
 * $Revision: 1 $
 *
 * Hawk-35 default setup program to communicate with Windows driver. Header
 * file.
 *
 * $Log: /Project/Units/Tasha2/BlackFin/include/pci_win.h $
 * 
 * 1     31.07.03 14:33 Martin.lueck
 * Wichtige Header und asm files für den BF535
 *
 ******************************************************************************/

#ifndef _PCI_WIN_H_
#define _PCI_WIN_H_

//..............................................................................

// PCI window settings.

// Allocate 64 MB of memory and 256 bytes of IO memory to be seen over the 
// PCI bus.

// Note: the reason 64 MB is selected here and not 32 which is the amount of
// memory on the board is to be compatible with ADI's driver that requires
// the size of 64 MB being used.

#define kPCI_MemSize   0x04000000
#define kPCI_IOMemSize 256

// Base address for downloadable program area.

#define dlProgMem (0xf0000000) 

// Enable 64 MB of ADSP-21535 memory to external host

#define kPCI_DMBARM ~(kPCI_MemSize-1)

// Enable 256 bytes of ADSP-21535 IO memory to external host

#define kPCI_DIBARM ~(kPCI_IOMemSize-1)

// Number to write to jump to new downloaded code.
#define MAGIC_CONSTANT 0xFEEDFACE

// Firmware version data resides at end of internal L2 memory.
#define FWVERSION_ADDR 0xF003FFF0

//..............................................................................

// WDOG_CTL offset from WDOG_CTL
#define WDOG_CTL_OFFSET 0x00	
// WDOG_CNT offset from WDOG_CTL
#define WDOG_CNT_OFFSET 0x04
// WDOG_STAT offset from WDOG_CTL
#define WDOG_STAT_OFFSET 0x08	

// PCI_CTL offset from PCI_CTL
#define PCI_CTL_OFFSET 0x0
// PCI_CBAP offset from PCI_CTL
#define PCI_CBAP_OFFSET 0x14
// PCI_TMBAP offset from PCI_CTL
#define PCI_TMBAP_OFFSET 0x18
// PCI_TIBAP offset from PCI_CTL
#define PCI_TIBAP_OFFSET 0x1c

#endif
