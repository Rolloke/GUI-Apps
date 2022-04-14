/*
 * $Header: /proj/ras/CVS/utils/hardware/adsp21535/tbubp/tbubp_boot.asm,v 1.2 2001/10/24 20:11:18 rmishra Exp $
 * Revision 1.6 04/22/02 jm
 * Moved init code from the base of L1 to the bottom of L1. basically, changed the value of ADSP21535_L1_CODE 
 * in the adsp21535.h file
 *
 * Revision 1.5 04/15/02 jm
 * SIC IMASK is now set based on the revision ID.
 * Also, removed some unecessary DMA initializations.
 * changed PCI_DEVICE_ID_ADSP21535 to 0x1535, the default Device ID for rev 1.0 silicon
 *
 * Revision 1.4 02/05/02 jm
 * Changed Class Code register to have a Base Class of 0x11 (Data Acquisition and Signal
 *                                                           Processing Controllers)
 *                                     a Sub-Class of 0x80 and an Interface of 0x00
 *
 *
 * Revision 1.3 01/30/02 jm 
 * fixed warm reset problem by using an interrupt based 
 *  method of recognized that a PCI reset has occurred.
 *
 * $Log: tbubp_boot.asm,v $
 * Revision 1.2  2001/10/24 20:11:18  rmishra
 * Assigned INTERRUPT_PIN config reg for enabling PCI interrupts
 * Fixed MING, MAXL to perform Byte accesses, instead of Word accesses
 *
 * Revision 1.1  2001/10/17 20:46:00  rmishra
 * Tahoe Bring-up Board boot-code for PCI operation
 *
 *
 * DESCRIPTION:	Tahoe Bring-up Board boot code for PCI operation.
 *		code taken from pci-bub.asm and sdram_init.asm
 * 
 */
#include "adsp21535.h"
#include "tbubp.h"

#if 0
#define MEM_DEBUG
#endif

#ifdef MEM_DEBUG
#define EMIT_MEM(addr,val)	p0.h = (addr >> 16);  p0.l = (addr & 0xFFFF); \
				r0.h = (val >> 16); r0.l = (val & 0xFFFF); \
				[p0] = r0
#endif
	
// #define USE_L2_BOOT

.section program;
.align 4; 
jump start;

nop;  /* used to have the isr here, keeping jump instruction to be consistent with LINUX test routine */

start:

// load exception handler
p0.h = (ADSP21535_EVENT_BASE >> 16); p0.l = (ADSP21535_EVENT_BASE & 0xFFFF);

/* 12/28/01 JM */
p1.h = PCI_RST_ISR;  p1.l = PCI_RST_ISR;  // PCI ISR
[p0 + ADSP21535_EVENTOFFSET_IVG7] = p1;	
ssync; 




//Check revision of silicon to set SIC_IMASK appropriately
P3.H = (ADSP21535_SYSMMR_SIC_IMASK >> 16);  P3.L = (ADSP21535_SYSMMR_SIC_IMASK & 0xffff);
P0.H = (ADSP21535_SYSMMR_CHIPID >> 16);     P0.L = (ADSP21535_SYSMMR_CHIPID & 0xffff);
R0 = [P0];
// R1 = 0xf000(z);
R1.H = 0XF000;
R1 = R1 & R0;
cc = bittst(r1, 28); // check if CHIP_ID is non zero
if cc jump rev_1_0; // if CHIP_ID is non-zero, a 1 in SIC_MASK enables interrupts

rev0_1_or_0_2:
//Unmask the PF Interrupt A in System Interrupt Mask Register
R6 = [p3];
BITCLR(R6,3); // change bit to desired bit position to match your application
[ P3 ] = R6;
SSYNC;
jump continue;

rev_1_0:
R6 = [p3];
BITSET(R6,3); // change bit to desired bit position to match your application
[ P3 ] = R6;
SSYNC;



/*  unmask PCI interrupt at the system level */
// p2.l = ADSP21535_SYSMMR_SIC_IMASK & 0xffff; p2.h = ADSP21535_SYSMMR_SIC_IMASK >> 16;
// R0   = [P2];
// ssync;
// BITCLR(R0,3); 
// BITSET(R0,3);
// [P2] = R0;  
				/* 	jm: unmask  PCI interrupts, in the current engineering silicon revs,
				a particular bit needs to be cleared in the sic_imask to unmask the
				corresponding interrupt.  In the production revs, the reverse will be true. */
// ssync;

continue:
/* jm unmask PCI interrupt at the core level. */
p2.l = ADSP21535_COREMMR_IMASK & 0xffff; p2.h = ADSP21535_COREMMR_IMASK >> 16;
R0.l   = W[P2];
R1.L = ADSP21535_IMASK_IVG7 & 0XFFFF;
R0 = R0 | R1;
W[P2] = R0.l;  /* Set the IVG7 mask bit to unmask the interrupt*/
ssync;

p1.h = expthand; p1.l = expthand;
[p0 + ADSP21535_EVENTOFFSET_EVSW]   = p1;	// software exception
ssync;

p1.h = hwerrhand; p1.l = hwerrhand;
[p0 + ADSP21535_EVENTOFFSET_IVHW] = p1;		// hardware exception
ssync;

SP.H = 0xF003; SP.L = 0xFF00;  /* JM: 12/26/01 setup supervisor stack */

#ifdef MEM_DEBUG
  EMIT_MEM(0xF0001000, 0x12345678); 
#endif
	
  /* set PF15 to be output for LED  */
  P0.H = ADSP21535_SYSMMR_FIO_DIR >> 16; P0.L = ADSP21535_SYSMMR_FIO_DIR & 0XFFFF;
  R0.l = w[P0];
  BITSET(R0,15);
  w[P0] = R0.l; /* set PF 15 as an output */
  ssync;




  /* Initialize SDRAM */
  call sdram_init;

  /* clear PCI STAT only once */
  p4.h = (ADSP21535_SYSMMR_PCI_STAT >> 16); p4.l = (ADSP21535_SYSMMR_PCI_STAT & 0xFFFF);		// clear PCI status before writing config space
  r0.h = 0x0; r0.l = 0x0;
  [p4] = r0;     // write PCI_CFG_STAT
  SSYNC;

  /* Initialize PCI interface for the first time */	
  call pci_cfg_init;
  nop; nop; nop; nop; nop; nop;

#ifdef MEM_DEBUG
  EMIT_MEM(0xF0001004, 0x87654321); 
#endif

#ifdef USE_L2_BOOT
  /* jump directly to polling loop (avoid using L1 to boot) */
  jump code_L1_start; 
#else
  /* copy bootstrap code and PCI initialization to L1 */
  call copy_code_L1;
  nop; nop; nop; nop; nop; nop;

#ifdef MEM_DEBUG
  EMIT_MEM(0xF0001008, 0xabcdef12); 
#endif
	
  /* jump to L1 code */
  p0.h = (ADSP21535_L1_CODE >> 16);   p0.l = (ADSP21535_L1_CODE & 0xFFFF);
  jump (p0);
#endif	/* USE_L2_BOOT */
				
/*
 * Software Exception Handler: lights up LEDs 1-5 and loops forever.
 */
expthand:
  p0.h = 0xffc0; p0.l = 0x2406;     
  r0.h = 0; r0.l = 0xf800;
  r1 = ~r0;
  r1 = r1 & r0;
  W[p0]=r1;
  csync;
  jump expthand;

/*
 * Hardware Exception Handler:	lights up LEDs 1, 3, and 5 and loops forever.
 */
hwerrhand:
  p0.h = 0xffc0; p0.l = 0x2406;     
  r0.h = 0; r0.l = 0xa800;
  r1.h = 0; r1.l = 0xf800;
  r0 = ~r0;
  r1 = r1 & r0;
  W[p0]=r1;
  csync;
  jump hwerrhand;

/*
 * Sub-routine which DMA's code from L2 to L1
 */
copy_code_L1:

  p0.h = 0xffe0; p0.l = 0x1004; // IMEM_CONTROL mmr

  r0 = [p0];	// enable L1 Instruction memory
  bitset(r0, 0); 
  [p0] = r0;

  /* Initialize Descriptor Blocks */
  p0.h = descblock_s; p0.l = descblock_s;
  p1.h = descblock_p; p1.l = descblock_p;

  r0.h = code_L1_start; r0.l = code_L1_start;
  r1.h = code_L1_end;   r1.l = code_L1_end;
  r1 = r1 - r0;   // number of bytes to transfer
  R1 = R1 >> 2;  // NUMBER OF 32-BIT WORDS 	
  w[p0+2] = r1;
  w[p1+2] = r1;  
	
  r1.h = (ADSP21535_L1_CODE >> 16);   r1.l = (ADSP21535_L1_CODE & 0xFFFF);
  w[p0+4] = r0;
  w[p1+4] = r1;

  r0 = r0 >> 16;
  r1 = r1 >> 16;
  w[p0+6] = r0;
  w[p1+6] = r1;

  r1.h = descblockstop; r1.l = descblockstop;
  w[p0+8] = r1;
  w[p1+8] = r1;

 /* jm r1 = r1 >> 16;
  w[p0+8] = r1;
  w[p1+8] = r1;  */

  p0.h = 0xffc0; p0.l = 0x4880;
  r0.h = descblock_s; r0.l = descblock_s;
  w[p0] = r0.h;	

  /* Initialize next, ready and config regs to start DMA Proc */
  p0.h = 0xffc0; p0.l = 0x3800; // write channel
  p1.h = 0xffc0; p1.l = 0x3900; // read channel
	   
  r1.h = descblock_p; r1.l = descblock_p;  // write next DB pointers
  w[p0+0xa] = r1;               
  w[p1+0xa] = r0;

  r0 = 0;
  w[p0+0xc] = r0;  // write ready reg
  w[p1+0xc] = r0;

  r0 = 1;      // enable DMA
  w[p0+2] = r0;
  w[p1+2] = r0;

  p1 = r1;
  p0.h = descblock_s; p0.l = descblock_s;
  r0.h = 0;
  r1.h = 0; r1.l = 0x8000;

poll_reader:
  r0 = w[p0];
  r0 = r0 & r1; 
  cc = r0;
  if cc jump poll_reader;  // test ownership of secondary mem DMA

poll_writer:
  r0 = w[p1];
  r0 = r0 & r1;
  cc = r0;
  if cc jump poll_writer; // test ownership of primary mem DMA
	

  rts;

/*
 * Initialize Micron 64MB MT48LC4M16A2-7E - copied from sdram_init.asm
 */
sdram_init:
  p0.h = (ADSP21535_SYSMMR_EBIU_SDRRC >> 16); p0.l = (ADSP21535_SYSMMR_EBIU_SDRRC & 0xFFFF);
  r0 = 0x0817 (z);
  W[p0] = r0;
  ssync;

  p0.h = (ADSP21535_SYSMMR_EBIU_SDBCTL >> 16); p0.l = (ADSP21535_SYSMMR_EBIU_SDBCTL & 0xFFFF);
  #ifdef BUB
      r0.l = 0x0005;  // for BUB
  #else
      r0.h = 0x0404;  r0.l = 0x0015;	// for PUB
  #endif
  
  [p0] = r0;
  ssync;

  p0.h = (ADSP21535_SYSMMR_EBIU_SDGCTL >> 16); p0.l = (ADSP21535_SYSMMR_EBIU_SDGCTL & 0xFFFF);
  r0.h = 0x0091; r0.l = 0x99fb;
  [p0] = r0;
  ssync;

  rts;

/************************** CODE TO COPY TO L1: START ****************************/

/*
 * Loop forever and examine VENDOR_ID register to see if it has
 * been cleared. If it is cleared, indicates PCI RESET has occurred.
 * If PCI RESET has occurred, PCI interface must be reinitialized.
 *
 *
 * Also examine PCI_CBAP PAB register to see if host has written
 * 0xFEEDFACE. If it has, then jump to starting address in L2.
 * This triggers execution of downloaded code.
 */
.align 4;	
code_L1_start:
 
// L1_code:

r7.h = code_L1_start; r7.l = code_L1_start;
r0.h = PCI_RST_ISR;   r0.l = PCI_RST_ISR;
R7 = R0 - R7;

r0.h = (ADSP21535_L1_CODE >> 16);  r0.l = (ADSP21535_L1_CODE & 0xFFFF);

r7 = r0 + r7;

p0.h = (ADSP21535_EVENT_BASE >> 16); p0.l = (ADSP21535_EVENT_BASE & 0xFFFF);
//p1.h = PCI_RST_ISR;  p1.l = PCI_RST_ISR;  // PCI ISR
[p0 + ADSP21535_EVENTOFFSET_IVG7] =  r7;	 
ssync;


//Check revision of silicon to set SIC_IMASK appropriately
P3.H = (ADSP21535_SYSMMR_SIC_IMASK >> 16);  P3.L = (ADSP21535_SYSMMR_SIC_IMASK & 0xffff);
P0.H = (ADSP21535_SYSMMR_CHIPID >> 16);     P0.L = (ADSP21535_SYSMMR_CHIPID & 0xffff);
R0 = [P0];
// R1 = 0xf000(z);
R1.H = 0XF000;
R1 = R1 & R0;
cc = bittst(r1, 28); // check if CHIP_ID is non zero
if cc jump rev_1_0_L1; // if CHIP_ID is non-zero, a 1 in SIC_MASK enables interrupts

rev0_1_or_0_2_L1:
//Unmask the PF Interrupt A in System Interrupt Mask Register
R6 = [p3];
BITCLR(R6,3); // change bit to desired bit position to match your application
[ P3 ] = R6;
SSYNC;
jump continue_L1;

rev_1_0_L1:
R6 = [p3];
BITSET(R6,3); // change bit to desired bit position to match your application
[ P3 ] = R6;
SSYNC;

continue_L1:
/* jm unmask PCI interrupt at the core level. */
 p2.l = ADSP21535_COREMMR_IMASK & 0xffff; p2.h = ADSP21535_COREMMR_IMASK >> 16;
 R0.l   = W[P2];
 R1.L = ADSP21535_IMASK_IVG7 & 0XFFFF;
 R0 = R0 | R1;
 W[P2] = R0.l;  /* Set the IVG7 mask bit to unmask the interrupt*/
 ssync;


#ifdef MEM_DEBUG
  EMIT_MEM(0xF000100C, 0x98765432); 
#endif

#if 0
p1.h = (ADSP21535_SYSMMR_PCI_STAT >> 16); p1.l = (ADSP21535_SYSMMR_PCI_STAT & 0xFFFF);
r4.h = 0x0; r4.l = ADSP21535_PCI_STAT_RESET; 	// PCI_RESET bit
#else
p1.h = (ADSP21535_SYSMMR_PCI_CFG_VIC >> 16); p1.l = (ADSP21535_SYSMMR_PCI_CFG_VIC & 0xFFFF);	// poll on Vendor ID != 0x0 for PCI RST
#endif
r0 = 0;
p2.h = (ADSP21535_SYSMMR_PCI_CBAP >> 16); p2.l = (ADSP21535_SYSMMR_PCI_CBAP & 0xFFFF);
[p2] = r0;
ssync;
r3.h = 0xFEED; r3.l = 0xFACE;	// poll on CBAP PCI reg for starting execution at L2 base addr

wait_for_interrupt:
 // nop;
 // jump wait_for_interrupt;
  /* poll for starting execution */
  r0 = [p2];
  ssync;
  r0 = r0 - r3;
  cc = r0;
  if cc jump wait_for_interrupt; // not really needed

  p0.h = (ADSP21535_L2_BASE >> 16); p0.l = (ADSP21535_L2_BASE & 0xFFFF); 
  jump (p0);   /* jump to L2 base and start execution of downloaded PCI application */

  /* 12/28 test PCI Reset */
/* PCI_software_reset:
P5.H = (ADSP21535_SYSMMR_PCI_CTL >> 16); P5.L = (ADSP21535_SYSMMR_PCI_CTL & 0xFFFF); 
R0 = [P5];
BITSET(R0,6);
[P5] = R0; // assert RST to PCI interrupt 
ssync; */


pci_cfg_init:

  p4.h = (ADSP21535_SYSMMR_PCI_CTL >> 16); p4.l = (ADSP21535_SYSMMR_PCI_CTL & 0xFFFF);		// disable PCI before writing config space
  r0.h = 0x0; r0.l = 0x0;
  [p4] = r0;     // write PCI_CFG_CTL
  SSYNC;

  p4.h = (ADSP21535_SYSMMR_PCI_ICTL >> 16); p4.l = (ADSP21535_SYSMMR_PCI_ICTL & 0xFFFF);		// clear PCI interrupt enables before writing config space
  r0.h = 0x0; r0.l = 0x0;
  [p4] = r0;     // write PCI_CFG_ICTL
  SSYNC;


device_id:	
  p4.h = (ADSP21535_SYSMMR_PCI_CFG_DIC >> 16); p4.l = (ADSP21535_SYSMMR_PCI_CFG_DIC & 0xFFFF);
  r0.h = 0x0; r0.l = TBUBP_PCI_DEVICE_ID;
  [p4] = r0;     // write PCI_CFG_DIC
  SSYNC;

  r1 = [p4];	// delay loop until device ID written
  SSYNC;
  r2 = r1 - r0;
  cc = r2;
  if cc jump device_id; 
	 
vendor_id:	
  p4.h = (ADSP21535_SYSMMR_PCI_CFG_VIC >> 16); p4.l = (ADSP21535_SYSMMR_PCI_CFG_VIC & 0xFFFF);
  r0.h = 0x0; r0.l = TBUBP_PCI_VENDOR_ID;
  [p4] = r0;     // write PCI_CFG_VIC
  SSYNC;

  r1 = [p4];	// delay loop until vendor ID written
  SSYNC;
  r2 = r1 - r0;
  cc = r2;
  if cc jump vendor_id; 

// Tom
command:	
  p4.h = (ADSP21535_SYSMMR_PCI_CFG_CMD >> 16); p4.l = (ADSP21535_SYSMMR_PCI_CFG_CMD & 0xFFFF);
  r0.h = 0x0; r0.l = 0x0;
  [p4] = r0;     // write PCI_CFG_CMD
  SSYNC;

  r1 = [p4];	// delay loop until command written
  SSYNC;
  r2 = r1 - r0;
  cc = r2;
  if cc jump command;

header_type:	
  p4.h = (ADSP21535_SYSMMR_PCI_CFG_HT >> 16); p4.l = (ADSP21535_SYSMMR_PCI_CFG_HT & 0xFFFF);
  r0.h = 0x0; r0.l = 0x0;
  [p4] = r0;     // write PCI_CFG_HT
  SSYNC;

  r1 = [p4];	// delay loop until Header Type written
  SSYNC;
  r2 = r1 - r0;
  cc = r2;
  if cc jump header_type; 

class_code:	
  p4.h = (ADSP21535_SYSMMR_PCI_CFG_CC >> 16); p4.l = (ADSP21535_SYSMMR_PCI_CFG_CC & 0xFFFF);
  r0.h = PCI_BASE_CLASS_SIGNAL_PROCESSING; r0.l = TBUBP_PCI_CLASS; // 2/05/02, JM: added Base class value of 0x11
  [p4] = r0;     // write PCI_CFG_CC
  SSYNC;

  r1 = [p4];	// delay loop until class code written
  SSYNC;
  r2 = r1 - r0;
  cc = r2;
  if cc jump class_code; 

revision_id:	
  p4.h = (ADSP21535_SYSMMR_PCI_CFG_RID >> 16); p4.l = (ADSP21535_SYSMMR_PCI_CFG_RID & 0xFFFF);
  r0.h = 0x0; r0.l = TBUBP_PCI_REVISION_ID;
  [p4] = r0;     // write PCI_CFG_RID
  SSYNC;

  r1 = [p4];	// delay loop until Revision ID written
  SSYNC;
  r2 = r1 - r0;
  cc = r2;
  if cc jump revision_id; 

subsystem_id:	
  p4.h = (ADSP21535_SYSMMR_PCI_CFG_SID >> 16); p4.l = (ADSP21535_SYSMMR_PCI_CFG_SID & 0xFFFF);
  r0.h = 0x0; r0.l = 0x0;
  [p4] = r0;     // write PCI_CFG_SID
  SSYNC;

  r1 = [p4];	// delay loop until sub-system ID written
  SSYNC;
  r2 = r1 - r0;
  cc = r2;
  if cc jump subsystem_id; 

subsystem_vendor_id:	
  p4.h = (ADSP21535_SYSMMR_PCI_CFG_SVID >> 16); p4.l = (ADSP21535_SYSMMR_PCI_CFG_SVID & 0xFFFF);
  r0.h = 0x0; r0.l = 0x0;
  [p4] = r0;     // write PCI_CFG_SVID
  SSYNC;

  r1 = [p4];	// delay loop until Subsystem Vendor ID written
  SSYNC;
  r2 = r1 - r0;
  cc = r2;
  if cc jump subsystem_vendor_id; 

int_pin:	
  p4.h = (ADSP21535_SYSMMR_PCI_CFG_IP >> 16); p4.l = (ADSP21535_SYSMMR_PCI_CFG_IP & 0xFFFF);
  #ifdef BUB
	  r0.h = 0x0; r0.l = 0x1;	// INTA   is wired
  #else
	  r0.h = 0x0; r0.l = 0x2;	// INTB   is wired
  #endif
  B[p4] = r0;     // write PCI_CFG_IP
  SSYNC;

  r1 = B[p4];	// delay loop until Subsystem Vendor ID written
  SSYNC;
  r2 = r1 - r0;
  cc = r2;
  if cc jump int_pin; 

min_grant:		
  p4.h = (ADSP21535_SYSMMR_PCI_CFG_MING >> 16); p4.l = (ADSP21535_SYSMMR_PCI_CFG_MING & 0xFFFF);
  r0.h = 0x0; r0.l = TBUBP_PCI_MIN_GRANT;
  B[p4] = r0;     // write PCI_CFG_MING
  SSYNC;

  r1 = B[p4];	// delay loop until Minimum Grant written
  SSYNC;
  r2 = r1 - r0;
  cc = r2;
  if cc jump min_grant; 

max_latency:		
  p4.h = (ADSP21535_SYSMMR_PCI_CFG_MAXL >> 16); p4.l = (ADSP21535_SYSMMR_PCI_CFG_MAXL & 0xFFFF);
  r0.h = 0x0; r0.l = TBUBP_PCI_MAX_LATENCY;
  B[p4] = r0;     // write PCI_CFG_MAXL
  SSYNC;

  r1 = B[p4];	// delay loop until maximum Latency written
  SSYNC;
  r2 = r1 - r0;
  cc = r2;
  if cc jump max_latency; 

dmbarm:		
  p4.h = (ADSP21535_SYSMMR_PCI_DMBARM >> 16); p4.l = (ADSP21535_SYSMMR_PCI_DMBARM & 0xFFFF);
#if 0
  r0.h = (TBUBP_PCI_MEM_BARMASK >> 16); r0.l = (TBUBP_PCI_MEM_BARMASK & 0xFFFF);
#else
  //r0.h = 0xFFFF; r0.l = 0xFF00;	/* TBUBP_PCI_MEM_BARMASK */
  r0.h = 0xFC00; r0.l = 0x0; // TOM
#endif	
  [p4] = r0;     // write PCI_CFG_DMBARM
  SSYNC;

  r1 = [p4];	// delay loop until Memory size written
  SSYNC;
  r2 = r1 - r0;
  cc = r2;
  if cc jump dmbarm; 

dibarm:	
  p4.h = (ADSP21535_SYSMMR_PCI_DIBARM >> 16); p4.l = (ADSP21535_SYSMMR_PCI_DIBARM & 0xFFFF);
  // r0.h = (TBUBP_PCI_IO_BARMASK >> 16); r0.l = (TBUBP_PCI_IO_BARMASK & 0xFFFF);
  r0.h = 0xFFFF; r0.l = 0xFF00; // TOM
  [p4] = r0;     // write PCI_CFG_DIBARM
  SSYNC;

  r1 = [p4];	// delay loop until I/O size written
  SSYNC;
  r2 = r1 - r0;
  cc = r2;
  if cc jump dibarm; 

hmctl:
  p4.h = (ADSP21535_SYSMMR_PCI_HMCTL >> 16); p4.l = (ADSP21535_SYSMMR_PCI_HMCTL & 0xFFFF);
  r0.h = 0x0; r0.l = ADSP21535_PCI_HMCTL_L2ENAB;	// enable L2 only
  [p4] = r0;		// write PCI_HMCTL 
  SSYNC;

  r1 = [p4];	// delay loop until Host Memory Control written
  SSYNC;
  r2 = r1 - r0;
  cc = r2;
  if cc jump hmctl; 

tmbap:	
  p4.h = (ADSP21535_SYSMMR_PCI_TMBAP >> 16); p4.l = (ADSP21535_SYSMMR_PCI_TMBAP & 0xFFFF);
  r0.h = (ADSP21535_L2_BASE >> 16); r0.l = (ADSP21535_L2_BASE & 0xFFFF);
  //r0.h = 0x0000; r0.l = 0x0000;  // TOM
  //r0.h = 0xFC00; r0.l = 0x0000;  // TOM
  [p4] = r0;     // write PCI_CFG_TMBAP
  SSYNC;

  r1 = [p4];	// delay loop until Memory offset written
  SSYNC;
  r2 = r1 - r0;
  cc = r2;
  if cc jump tmbap; 

tibap:		
  p4.h = (ADSP21535_SYSMMR_PCI_TIBAP >> 16); p4.l = (ADSP21535_SYSMMR_PCI_TIBAP & 0xFFFF);
  r0.h = (ADSP21535_SYSMMR_PCI_CTL >> 16); r0.l = (ADSP21535_SYSMMR_PCI_CTL & 0xFFFF);	/* 256 bytes of PCI PAB regs */
  [p4] = r0;     // write PCI_CFG_TIBAP
  SSYNC;

  r1 = [p4];	// delay loop until I/O offset written
  SSYNC;
  r2 = r1 - r0;
  cc = r2;
  if cc jump tibap; 

enab_pci:

  p4.h = (ADSP21535_SYSMMR_PCI_ICTL >> 16); p4.l = (ADSP21535_SYSMMR_PCI_ICTL & 0xFFFF);			/* enable PCI interrupts */
  r0.h = 0x0; r0.l = (ADSP21535_PCI_ICTL_RESET);
  [p4] = r0;     // write PCI_CFG_ICTL
  SSYNC;

  p4.h = (ADSP21535_SYSMMR_PCI_CTL >> 16); p4.l = (ADSP21535_SYSMMR_PCI_CTL & 0xFFFF);
  r0.h = 0x0; r0.l = (ADSP21535_PCI_CTL_ENABPCI | ADSP21535_PCI_CTL_ENABINTA);	/* enable PCI interface with interrupts */
  [p4] = r0;     // write PCI_CFG_CTL 
  SSYNC;
  
  rts;  /* Return from PCI subroutine */


/* 12/28/01 JM */
.align 4;
PCI_RST_ISR:
  p4.h = (ADSP21535_SYSMMR_PCI_CTL >> 16); p4.l = (ADSP21535_SYSMMR_PCI_CTL & 0xFFFF);		// disable PCI before writing config space
  r0.h = 0x0; r0.l = 0x0;
  [p4] = r0;     // disable PCI
  SSYNC;

  p4.h = (ADSP21535_SYSMMR_PCI_STAT >> 16); p4.l = (ADSP21535_SYSMMR_PCI_STAT & 0xFFFF);		// clear PCI status before writing config space
  r0 = [p4];
  bitset(r0,6);  /*  Write a 1 to clear PCI RESET status bit */
  [p4] = r0; 
  SSYNC;

  p0.h = ADSP21535_SYSMMR_FIO_FLAG_C >> 16; p0.l = ADSP21535_SYSMMR_FIO_FLAG_C & 0xffff;
  R0.l = w[P0];
  BITSET(R0,15);
  w[P0] = R0.l; /* clear PF 15 */
  ssync;

  p1.h = (ADSP21535_SYSMMR_PCI_CFG_VIC >> 16); p1.l = (ADSP21535_SYSMMR_PCI_CFG_VIC & 0xFFFF);	// poll on Vendor ID != 0x0 for PCI RST

still_in_reset:
  R7 = 0x11D4;
  csync;
  [P1] = R7;
  SSYNC;
  r7 = [p1];
  SSYNC;
  cc = r7; /* if set no longer in reset */
  if !cc jump still_in_reset;

  call pci_cfg_init;

  p0.h = ADSP21535_SYSMMR_FIO_FLAG_S >> 16; p0.l = ADSP21535_SYSMMR_FIO_FLAG_S & 0xffff;
  R0.l = w[P0];
  BITSET(R0,15);
  w[P0] = R0.l; /* set PF 15 */
  ssync;

rti; /* return from PCI Reset interrupt */

nop;nop;nop;nop;nop;nop;

code_L1_end:
  
/************************** CODE TO COPY TO L1: END ****************************/
.align 4;
.global descblock_s;  // read from memory
descblock_s:
.byte2 = 0x8009;   // DMA config
.byte2 = 0;        // words to be transferred
.byte2 = 0;        // buffer start addr, 16 LSB
.byte2 = 0;        // buffer start addr, 16 MSB
.byte2 = 0;        // next desc block, 16 LSB
.byte2 = 0;        // next desc block, 16 MSB

.align 4;
.global descblock_p;  // write to memory
descblock_p:
.byte2 = 0x800B;   // DMA config
.byte2 = 0;        // words to be transferred
.byte2 = 0;        // buffer start addr, 16 LSB
.byte2 = 0;        // buffer start addr, 16 MSB
.byte2 = 0;        // next desc block, 16 LSB
.byte2 = 0;        // next desc block, 16 MSB

.align 4;
descblockstop:
.byte2 = 0x0000;
//.byte2 = 0xfffe;
.byte2 = 0x8888;
.byte2 = 0x8888;
.byte2 = 0x8888;
.byte2 = 0x8888;
.byte2 = 0x8888;


