/****************************************************************

  $Header: /Project/Units/Tasha2/TashaFirmware/PCI_Init/tbubp.h 1     4.11.03 14:47 Martin.lueck $

  Copyright (c) 2001 Analog Devices Inc.  
  All rights reserved.

  MODULE:	tbubp.h

  DESCRIPTION:	Definitions for Tahoe Bring-up-Board PCI (TBuBp).

  REVISION HISTORY:

  $Log: /Project/Units/Tasha2/TashaFirmware/PCI_Init/tbubp.h $
 * 
 * 1     4.11.03 14:47 Martin.lueck
 * Boot programm des BF535
  Revision 1.3  2001/10/19 21:32:35  rmishra
  PCI window definitions

  Revision 1.2  2001/10/05 17:43:32  rmishra
  SDRAM definitions fixed.
  Removed fixed PCI MEM and I/O spaces (done dynamically).

  Revision 1.1.1.1  2001/09/09 00:06:39  rmishra
  BlackFin ADSP-21535 hardware debug


*************************************************************/

#ifndef _TBUBP_H_
#define _TBUBP_H_

#include "pci_ids.h"

#define TBUBP_SDRAM_NUM_BANKS	1		/* only 1 bank populated */
#define TBUBP_SDRAM_SIZE	0x1000000	/* 16 MB */

/*
 * PCI target definitions
 */
#define TBUBP_PCI_VENDOR_ID	PCI_VENDOR_ID_ADI
#define TBUBP_PCI_DEVICE_ID	PCI_DEVICE_ID_ADSP21535
#define TBUBP_PCI_CLASS		PCI_CLASS_SP_OTHER	/* other signal proc */
#define TBUBP_PCI_REVISION_ID	0x0	/* board rev */
#define TBUBP_PCI_MIN_GRANT	0x1	/* = 0.25 usecs for 8 DWord FIFO */
#define TBUBP_PCI_MAX_LATENCY	0x2	/* = 0.50 usecs for 32-byte PCI read (16 PCI clks) */

#define TBUBP_PCI_IO_SIZE	256	/* max window size to match host */
#define TBUBP_PCI_IO_BARMASK	(~(TBUBP_PCI_IO_SIZE-1))
#define TBUBP_PCI_MEM_SIZE	0x40000	/* 256 KB to match host */
#define TBUBP_PCI_MEM_BARMASK	0xFFFC0000	/* (~(TBUBP_PCI_MEM_SIZE-1)) */

#endif	/* _TBUBP_H_ */
