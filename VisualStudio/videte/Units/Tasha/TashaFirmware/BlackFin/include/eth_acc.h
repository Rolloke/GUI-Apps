/*******************************************************************************
 *
 * $Source$
 * $Revision: 1 $
 *
 * Ethernet access functions, header file.
 *
 * $Log: /Project/Units/Tasha2/BlackFin/include/eth_acc.h $
 * 
 * 1     31.07.03 14:32 Martin.lueck
 * Wichtige Header und asm files für den BF535
 *
 ******************************************************************************/

#ifndef _ETH_ACC_H_
#define _ETH_ACC_H_

#include "stdtyp.h"

//..............................................................................

// Function protypes

extern void programEeprom(uint32 *);
extern void dumpEeprom(uint32 *);
extern uint32 mdioRead(uint32 *,int);
extern uint32 mdioWrite(uint32 *,int,int);

//..............................................................................

// Control/Status Registers (CSR)

#define k82559CSRMemSize   4096    // CSR memory mapped size 

#define SCB_STATUS         0       // SCB Status Register
#define EEPROM_CTL         0x0e    // EEPROM Control Register  
#define MDI_CTL            0x10    // Management Data Interface Control Register

// SCB_STATUS

#define bSCB_StatusMDI_Int 11

#define kSCB_StatusMDI_Int ZET(bSCB_StatusMDI_Int,1)

// EEPROM_CTL 

// Delay between EEPROM clock transitions.
// This will actually work with no delay on 33Mhz PCI.  

#define eepromDelay(nanosec)

#define kEEPROM_CtlClk         1    // EEPROM shift clock
#define kEEPROM_CtlCs          2    // EEPROM chip select
#define kEEPROM_CtlDataWrite   4    // EEPROM chip data in
#define kEEPROM_CtlDataRead    8    // EEPROM chip data out
#define kEEPROM_CtlENB         (0x4800 | kEEPROM_CtlCs)

// The EEPROM commands include the always-set leading bit. 
#define kEEPROM_CtlWriteCmd		(5 << addrBitLen)
#define kEEPROM_CtlReadCmd		(6 << addrBitLen)
#define kEEPROM_CtlEraseCmd		(7 << addrBitLen)
#define kEEPROM_CtlWriteEnableCmd	0x980
#define kEEPROM_CtlWriteDisableCmd	0x800

//..............................................................................

// MDI_CTL

#define	bMDI_CtlData	   0
#define	bMDI_CtlPhyRegAddr 16
#define	bMDI_CtlPhyAddr    21
#define	bMDI_CtlOpcode	   26
#define	bMDI_CtlRdy        28
#define	bMDI_CtlIntEn      29
#define	bMDI_CtlReserved   30

#define kMDI_CtlRdy        ZET(bMDI_CtlRdy,1)

//..............................................................................

#endif
