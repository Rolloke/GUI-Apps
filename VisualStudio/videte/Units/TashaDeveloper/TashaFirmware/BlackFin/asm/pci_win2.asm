/*******************************************************************************
 *
 * $Source$
 * $Revision$
 *
 * Hawk-35 default setup program to communicate with Windows driver.
 *
 * This program configures ADSP-21535, so it can participate on the PCI bus as 
 * a device. 
 *
 * $Log$
 *
 ******************************************************************************/

// Nur zum Test eingebunden !!!
#include <def21535.h>
#include "m21535.h"
#include "zet.h"
#include "gpio.h"
#include "pci_win.h"
#include "mem_tst.h"

//..............................................................................

	.section    data1;

	.global	initPCI;

//..............................................................................

#if defined(_HAWK_35_) || defined(_PUB_)  // [ _HAWK_35_ || _PUB_

#define bMajor 16
#define bMinor 8
#define bWee   0

// Firmware version 1.0.0

#define	kFW_VERSION ( \
	ZET(bMajor,1)+ \
	ZET(bMinor,0)+ \
	ZET(bWee,0))	

// PCI_CTL

#define	kPCICTL	\
	   ZET(bPCI_HostDevice,0)+       /* Device operation */                \
	   ZET(bPCI_PCIEn,0)+            /* PCI disabled on the PCI side */    \
	   ZET(bPCI_FastBackToBackEn,0)+ /* No fast back-to-back transfers */  \
	   ZET(bPCI_IntAToPCIEn,0)+      /* Disable PCI_INTA pad as output */  \
	   ZET(bPCI_IntAToPCI,0)+        /* Do not generate interrupt to PCI   
					    (no action) */                     \
	   ZET(bPCI_RSTToPCIEn,0)+       /* Disable PCI_RST pad as an output */\
	   ZET(bPCI_RSTToPCI,0)          /* Do not generate a reset to PCI */    

#define	kPCICTL_Reset	\
	   ZET(bPCI_HostDevice,0)+       /* Device operation */                \
	   ZET(bPCI_PCIEn,0)+            /* PCI disabled on the PCI side */    \
	   ZET(bPCI_FastBackToBackEn,0)+ /* No fast back-to-back transfers */  \
	   ZET(bPCI_IntAToPCIEn,0)+      /* Disable PCI_INTA pad as output */  \
	   ZET(bPCI_IntAToPCI,0)+        /* Do not generate interrupt to PCI   
					    (no action) */                     \
	   ZET(bPCI_RSTToPCIEn,0)+       /* Disable PCI_RST pad as an output */\
	   ZET(bPCI_RSTToPCI,1)          /* Generate a reset to PCI */    

#define	kPCICTL_Enable	\
	   ZET(bPCI_HostDevice,0)+       /* Device operation */                \
	   ZET(bPCI_PCIEn,1)+            /* PCI enabled on the PCI side */     \
	   ZET(bPCI_FastBackToBackEn,0)+ /* No fast back-to-back transfers */  \
	   ZET(bPCI_IntAToPCIEn,1)+      /* Enable PCI_INTA pad as output */  \
	   ZET(bPCI_IntAToPCI,0)+        /* Do not generate interrupt to PCI   
					    (no action) */                     \
	   ZET(bPCI_RSTToPCIEn,0)+       /* Disable PCI_RST pad as an output */\
	   ZET(bPCI_RSTToPCI,0)          /* Do not generate a reset to PCI */         

#define	kPCIICTL	\
	   ZET(bPCI_INTAEn,0)+               /*  */\
	   ZET(bPCI_INTBEn,0)+               /*  */\
	   ZET(bPCI_INTCEn,0)+               /*  */\
	   ZET(bPCI_INTDEn,0)+               /*  */\
	   ZET(bPCI_ParityErrorEn,1)+        /*  */\
	   ZET(bPCI_FatalErrorEn,1)+         /*  */\
	   ZET(bPCI_ResetEn,1)+              /*  */\
	   ZET(bPCI_MasterTxFifoFullEn,1)+   /*  */\
	   ZET(bPCI_MemoryWriteInvalEn,1)+   /*  */\
	   ZET(bPCI_ErrorOnInboundRdEn,1)+   /*  */\
	   ZET(bPCI_ErrorOnInboundWrEn,1)+   /*  */\
	   ZET(bPCI_UnsuppEABAccessEn,1)+    /*  */\
	   ZET(bPCI_SerrEn,1)+               /*  */\
	   ZET(bPCI_SlaveRxFifoGotDataEn,1)+ /*  */\
	   ZET(bPCI_SlaveTxFifoGotDataEn,1)  /* */  

#define	kPCICFGCMD	\
	   ZET(bPCI_CFG_IOSpaceEn,1)+        /*  */\
	   ZET(bPCI_CFG_MemorySpaceEn,1)+    /*  */\
	   ZET(bPCI_CFG_BusMasterEn,1)+      /*  */\
	   ZET(bPCI_CFG_ParityErrorEn,1)+    /*  */\
	   ZET(bPCI_CFG_SerrEn,1)+           /*  */\
	   ZET(bPCI_CFG_FastBackToBackEn,0)  /* Disable '-outbound-' fast btb 
						transactions */

//------------------------------------------------------------------------------

	.section program;
	.align 2;

initExtL2SDRAM:

  	// set up SDRAM registers

  	p0.l = EBIU_SDRRC & 0xffff;
  	p0.h = EBIU_SDRRC >> 16;
	r0.l = kSDRRC & 0xffff;
	w[p0] = r0.l;
	ssync;
 	p0.l = EBIU_SDBCTL & 0xffff;
  	p0.h = EBIU_SDBCTL >> 16;
	r0.l = kSDBCTL & 0xffff;
	r0.h = kSDBCTL >> 16;
	[p0] = r0;
	ssync;
 	p0.l = EBIU_SDGCTL & 0xffff;
  	p0.h = EBIU_SDGCTL >> 16;
	r0.l = kSDGCTL & 0xffff;
	r0.h = kSDGCTL >> 16;
	[p0] = r0;
	ssync;

	rts;

//..............................................................................

initPCI:

// set up PCI registers

    // disable PCI before configuring it
   	p0.l = PCI_CTL & 0xffff;
	p0.h = PCI_CTL >> 16;
	r0.l = kPCICTL;
	r0.h = 0;
	[p0] = r0;	
  	ssync;

    // clear sticky status bits
	p0.l = PCI_STAT & 0xffff;
	p0.h = PCI_STAT >> 16;
	r0.l = 0xffff;
	r0.h = 0;
	[p0] = r0;	
	ssync;

	p0.l = PCI_CFG_HT & 0xffff;
	p0.h = PCI_CFG_HT >> 16;
	r0 = 0;
	[p0] = r0;	
	ssync;

#define PCI_CLASS_SP_OTHER		0x8000
#define PCI_BASE_CLASS_SIGNAL_PROCESSING 0x11

	p0.l = PCI_CFG_CC & 0xffff;
	p0.h = PCI_CFG_CC >> 16;
	r0.l = PCI_CLASS_SP_OTHER;
	r0.h = PCI_BASE_CLASS_SIGNAL_PROCESSING;
	[p0] = r0;	
	ssync;

#define TBUBP_PCI_REVISION_ID	0x0	/* board rev */

	p0.l = PCI_CFG_RID & 0xffff;
	p0.h = PCI_CFG_RID >> 16;
	r0 = 0;
	[p0] = r0;	
	ssync;

#define	DEVICE_ID			0x1535

	p0.l = PCI_CFG_DIC & 0xffff;
	p0.h = PCI_CFG_DIC >> 16;
//	r0 = 0;
	r0 = DEVICE_ID;
	[p0] = r0;	
	ssync;
	
#define	VENDOR_ID			0x11D4

	p0.l = PCI_CFG_VIC & 0xffff;
	p0.h = PCI_CFG_VIC >> 16;
//	r0 = 0;
	r0 = VENDOR_ID;
	[p0] = r0;	
	ssync;

#define SUBSYSTEM_ID		0x1701

	p0.l = PCI_CFG_SID & 0xffff;
	p0.h = PCI_CFG_SID >> 16;
//	r0 = 0;
	r0 = SUBSYSTEM_ID;
	[p0] = r0;	
	ssync;

#define	SUBSYSTEM_VENDOR_ID			0x0000

	p0.l = PCI_CFG_SVID & 0xffff;
	p0.h = PCI_CFG_SVID >> 16;
//	r0 = 0;
	r0 = VENDOR_ID;
	[p0] = r0;	
	ssync;

#define TBUBP_PCI_MIN_GRANT	0x1	/* = 0.25 usecs for 8 DWord FIFO */

	p0.l = PCI_CFG_MING & 0xffff;
	p0.h = PCI_CFG_MING >> 16;
	r0 = TBUBP_PCI_MIN_GRANT;
	[p0] = r0;	
	ssync;

#define TBUBP_PCI_MAX_LATENCY	0x2	/* = 0.50 usecs for 32-byte PCI read (16 PCI clks) */

	p0.l = PCI_CFG_MAXL & 0xffff;
	p0.h = PCI_CFG_MAXL >> 16;
	r0 = TBUBP_PCI_MAX_LATENCY;
	[p0] = r0;	
	ssync;

  	// set how much memory is allowed to be seen on the PCI side 					      
	p0.l = PCI_DMBARM & 0xffff;
	p0.h = PCI_DMBARM >> 16;
	r0.l = kPCI_DMBARM & 0xffff;
	r0.h = kPCI_DMBARM >> 16;
	[p0] = r0;		
	ssync;

    // set base address of ADSP_21535 memory allowed to be seen on the PCI side 
	p0.l = PCI_TMBAP & 0xffff;
	p0.h = PCI_TMBAP >> 16;
	p4.l = dlProgMem & 0xffff;
	p4.h = dlProgMem >> 16;
	[p0] = p4;		
	ssync;

  	// set how much IO memory is allowed to be seen on the PCI side 	
	p0.l = PCI_DIBARM & 0xffff;
	p0.h = PCI_DIBARM >> 16; 
	r0.l = kPCI_DIBARM & 0xffff;
	r0.h = kPCI_DIBARM >> 16; 
	[p0] = r0;
	ssync;

    // set base address of ADSP_21535 IO memory allowed to be seen on the PCI side 
	p0.l = PCI_TIBAP & 0xffff;
	p0.h = PCI_TIBAP >> 16; 
	r0.l = PCI_CTL & 0xffff;	
	r0.h = PCI_CTL >> 16; 
	[p0] = r0; 
	ssync;

// Test{
#define MEM_IO_BASE 0xD8000008

    // set base address of ADSP_21535 IO memory allowed to be seen on the PCI side 
	p0.l = PCI_CFG_MBAR & 0xffff;
	p0.h = PCI_CFG_MBAR >> 16; 
	r0.l = MEM_IO_BASE & 0xffff;	
	r0.h = MEM_IO_BASE >> 16; 
	[p0] = r0; 
	ssync;
// }Test	
	
	p0.l = PCI_CFG_IP & 0xffff;
	p0.h = PCI_CFG_IP >> 16;
#ifdef _PUB_
	r0 = 0x2; // Int B is wired on PUB
#else
	r0 = 0x1; // Int A or B may be selected depending on which resistor is used 
	          // A selected here
#endif
	[p0] = r0;	
	ssync;

//..............................................................................

	p0.l = PCI_ICTL & 0xffff;
	p0.h = PCI_ICTL >> 16;
	r0.l = kPCIICTL & 0xffff; 
	r0.h = kPCIICTL >> 16;
	[p0] = r0;
	ssync;

	p0.l = PCI_CTL & 0xffff;
	p0.h = PCI_CTL >> 16;
	r0 = kPCICTL_Enable;
	[p0] = r0;
	ssync;

	rts;

//..............................................................................    

	.align 4;
	.byte4	versionData[4] =

// ^ operator doesn't work, use XOR(x,y) = NOT(x)*y+x*NOT(y)
	
	kFW_VERSION, ~kFW_VERSION, ((~kFW_VERSION)&0x123489AB)|(kFW_VERSION&(~0x123489AB)), ((~kFW_VERSION)&0x55555555)|(kFW_VERSION&(~0x55555555));

//..............................................................................    

//..............................................................................

	.section moved_bootcode;

// create a label for where to move code

movedWaitForDownload:

//..............................................................................

	.section fw_version;

// create a label for where to move firmware version data

version:

//..............................................................................

#else              // ] [ !(_HAWK_35_ || _PUB_) 
#error "Board not defined"
#endif             // ] _HAWK_35_ || _PUB_