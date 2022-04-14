/*******************************************************************************
 *
 * $Source$
 * $Revision: 1 $
 *
 * PCI access functions, header file.
 *
 * $Log: /Project/Units/Tasha2/BlackFin/include/pci_acc.h $
 * 
 * 1     31.07.03 14:32 Martin.lueck
 * Wichtige Header und asm files für den BF535
 *
 ******************************************************************************/

#ifndef _PCI_ACC_H_
#define _PCI_ACC_H_

#include "stdtyp.h"

//..............................................................................

// PCI Configuration Space

// The DSP can't do config accesses that are not dword aligned.

#define PCI_VENDOR_ID		0x00
#define PCI_COMMAND		0x04
#define PCI_CLASS_REVISION	0x08
#define PCI_CACHE_LINE_SIZE	0x0c

// Base addresses specify locations in memory or I/O space. Decoded size can be 
// determined by writing a value of 0xffffffff to the register, and reading it 
// back. Only 1 bits are decoded.

#define PCI_BASE_ADDRESS_0	0x10
#define PCI_BASE_ADDRESS_1	0x14
#define PCI_BASE_ADDRESS_2	0x18
#define PCI_BASE_ADDRESS_3	0x1c
#define PCI_BASE_ADDRESS_4	0x20
#define PCI_BASE_ADDRESS_5	0x24

// Header type 0 (normal devices) 
#define PCI_CARDBUS_CIS		0x28
#define PCI_SUBSYSTEM_VENDOR_ID	0x2c
#define PCI_SUBSYSTEM_ID	0x2e  
#define PCI_ROM_ADDRESS		0x30
#define PCI_CAP_PTR             0x34	
#define PCI_LAT_GNT_INT         0x3C

// Header type 1 (PCI-to-PCI bridges) 
#define PCI_BRIDGE_PRIMARY_BUS	0x18	
#define PCI_BRIDGE_IO_BASE	0x1c	
#define PCI_BRIDGE_MEMORY_BASE	0x20	
#define PCI_IO_BASE_UPPER16     0x30

//..............................................................................

// Number of buses, devices and functions on each device accessible from PCI. 

#define END_BUS    2
#define END_DEVICE 6   // devices 0 (DSP) through 5 connected on the Eagle-35
#define END_FUNC   1

//..............................................................................

typedef struct _pciDevBusInfo_T {
  uint32 bn; // bus number
  uint32 dn; // device number
  uint32 fn; // function number
} pciDevBusInfo_T;

typedef struct {
       uint32 pciVendorId;
       uint32 pciCmd;
       uint32 pciClassRev;
       uint32 pciCacheLineSize;
       uint32 pciBaseAddr0;
       uint32 pciBaseAddr1;
       uint32 pciBaseAddr2;
       uint32 pciBaseAddr3;
       uint32 pciBaseAddr4;
       uint32 pciBaseAddr5;
       uint32 pciCardBusCISPtr;
       uint32 pciSubsystemVendorId;
       uint32 pciSubsystemId;
       uint32 pciRomAddress;
       uint32 pciCapPtr;
       uint32 pciLatGntInt;
} configSpace_T;

//..............................................................................

// Function protypes

extern int getConfigSpaceData(uint32 bn, uint32 dn, uint32 fn, 
			      configSpace_T *configSpace);
extern int scanPciForDevice(uint32,pciDevBusInfo_T *);
extern uint32 pciReadConfigDword(uint32 bn,uint32 dn,uint32 fn,uint32 reg,uint32 *val);
extern uint32 pciWriteConfigDword(uint32 bn,uint32 dn,uint32 fn,uint32 reg,uint32 val);
extern int pciCheckMemAddr(uint32 addr);

#endif
