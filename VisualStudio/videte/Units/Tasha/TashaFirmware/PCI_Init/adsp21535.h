/****************************************************************

  $Header: /Project/Units/Tasha2/TashaFirmware/PCI_Init/adsp21535.h 1     4.11.03 14:47 Martin.lueck $

  Copyright (c) 2001 Analog Devices Inc.  
  All rights reserved.

  MODULE:	adsp21535.h

  DESCRIPTION:	Definitions for ADSP-21535 BlackFin processor
  		including System MMRs.

  REVISION HISTORY:

  $Log: /Project/Units/Tasha2/TashaFirmware/PCI_Init/adsp21535.h $
 * 
 * 1     4.11.03 14:47 Martin.lueck
 * Boot programm des BF535
  Revision 1.5  2001/10/10 21:47:18  rmishra
  Defined Event handlers's base address

  Revision 1.4  2001/10/05 17:37:59  rmishra
  Memory map definitions

  Revision 1.3  2001/09/25 23:23:26  rmishra
  Core memory-mapped registers

  Revision 1.1.1.1  2001/09/09 00:06:38  rmishra
  BlackFin ADSP-21535 hardware debug


*************************************************************/

#ifndef _ADSP_21535_H_
#define _ADSP_21535_H_

/*
 * Memory Map
 */

/* Core MMRs */
#define ADSP21535_COREMMR_BASE		0xFFE00000	/* System MMRs */
#define ADSP21535_COREMMR_SIZE		0x200000	/* 2 MB */

/* System MMRs */
#define ADSP21535_SYSMMR_BASE		0xFFC00000	/* System MMRs */
#define ADSP21535_SYSMMR_SIZE		0x200000	/* 2 MB */

/* L1 cache/SRAM internal memory */
#define ADSP21535_L1_DATA_A		0xFF800000	/* L1 Data Bank A */
#define ADSP21535_L1_DATA_B		0xFF900000	/* L1 Data Bank B */
#define ADSP21535_L1_DATA_SIZE		0x4000		/*  16K */
#define ADSP21535_L1_CODE		0xFFA03CDC	/* L1 Code SRAM */
#define ADSP21535_L1_CODE_SIZE		0x4000		/*  16K */
#define ADSP21535_L1_SCRATCH		0xFFB00000	/* L1 Scratch SRAM */
#define ADSP21535_L1_SCRATCH_SIZE	0x1000		/*  4K */

/* L2 SRAM external memory */
#define ADSP21535_L2_BASE		0xF0000000	/* L2 SRAM */
#define ADSP21535_L2_SIZE		0x40000		/*  256K */

/* PCI Spaces */
#define ADSP21535_PCI_CONFIG_SPACE_PORT	0xEEFFFFFC	/* PCI config space reg */
#define ADSP21535_PCI_CONFIG_BASE	0xEEFFFF00	/* PCI config region */
#define ADSP21535_PCI_CONFIG_SIZE	0x10000		/*  64K */
#define ADSP21535_PCI_IO_BASE		0xEEFE0000	/* PCI I/O space */
#define ADSP21535_PCI_IO_SIZE		0x10000		/*  64K */
#define ADSP21535_PCI_MEM_BASE		0xE0000000	/* PCI Mem space */
#define ADSP21535_PCI_MEM_SIZE		0x8000000	/*  64K */

/* Async Memory Banks */
#define ADSP21535_ASYNC_BANK3_BASE	0x2C000000	/* Async Bank 3 */
#define ADSP21535_ASYNC_BANK3_SIZE	0x4000000	/*  64 MB */
#define ADSP21535_ASYNC_BANK2_BASE	0x28000000	/* Async Bank 2 */
#define ADSP21535_ASYNC_BANK2_SIZE	0x4000000	/*  64 MB */
#define ADSP21535_ASYNC_BANK1_BASE	0x24000000	/* Async Bank 1 */
#define ADSP21535_ASYNC_BANK1_SIZE	0x4000000	/*  64 MB */
#define ADSP21535_ASYNC_BANK0_BASE	0x20000000	/* Async Bank 0 */
#define ADSP21535_ASYNC_BANK0_SIZE	0x4000000	/*  64 MB */

/* Sync DRAM Banks */
#define ADSP21535_SDRAM_BANK3_BASE	0x18000000	/* Sync Bank 3 */
#define ADSP21535_SDRAM_BANK2_BASE	0x10000000	/* Sync Bank 2 */
#define ADSP21535_SDRAM_BANK1_BASE	0x08000000	/* Sync Bank 1 */
#define ADSP21535_SDRAM_BANK0_BASE	0x00000000	/* Sync Bank 0 */

/*
 * System Memory-Mapped Registers
 */

/* Clock and System Control Regs */
#define ADSP21535_SYSMMR_PLLCTL		0xFFC00400	/* PLL control */
#define ADSP21535_SYSMMR_PLLSTAT	0xFFC00404	/* PLL Status */
#define ADSP21535_SYSMMR_LOCKCNT	0xFFC00406	/* PLL Lock Count */
#define ADSP21535_SYSMMR_IOCKR		0xFFC00408	/* PCLK enable */
#define ADSP21535_SYSMMR_SWRST		0xFFC00410	/* Software Reset */
#define ADSP21535_SYSMMR_SYSCR		0xFFC00414	/* System Reset Config */

/* Chip ID Reg */
#define ADSP21535_SYSMMR_CHIPID		0xFFC048C0	/* Chip ID */

/* System Interrupt Controller Regs */
#define ADSP21535_SYSMMR_SIC_RVECT 	0xFFC00C00	/* Reset Vector Address */
#define ADSP21535_SYSMMR_SIC_IAR0  	0xFFC00C04	/* Interrupt Assignment 0 */
#define ADSP21535_SYSMMR_SIC_IAR1  	0xFFC00C08	/* Interrupt Assignment 1 */
#define ADSP21535_SYSMMR_SIC_IAR2  	0xFFC00C0C	/* Interrupt Assignment 2 */
#define ADSP21535_SYSMMR_SIC_IMASK 	0xFFC00C10	/* Interrupt Mask */
#define ADSP21535_SYSMMR_SIC_ISR   	0xFFC00C14	/* Interrupt Status */
#define ADSP21535_SYSMMR_SIC_IWR       	0xFFC00C18	/* Interrupt Wake-up Enable */

/* Watchdog Timer Regs */
#define ADSP21535_SYSMMR_WDOGCTL	0xFFC01000	/* Watchdog Control */
#define ADSP21535_SYSMMR_WDOGCNT	0xFFC01004	/* Watchdog Count */
#define ADSP21535_SYSMMR_WDOGSTAT	0xFFC01008	/* Watchdog Status */

/* Real-time Clock Regs */
#define ADSP21535_SYSMMR_RTCSTAT	0xFFC01400	/* RTC Status */
#define ADSP21535_SYSMMR_RTCICTL	0xFFC01404	/* RTC Interrupt Control */
#define ADSP21535_SYSMMR_RTCISTAT	0xFFC01408	/* RTC Interrupt Status */
#define ADSP21535_SYSMMR_RTCSWCNT	0xFFC0140C	/* RTC Stopwatch Count */
#define ADSP21535_SYSMMR_RTCALARM	0xFFC01410	/* RTC Alarm */
#define ADSP21535_SYSMMR_RTCFAST	0xFFC01414	/* RTC Enable */

/* UART 0 Controller */
#define ADSP21535_SYSMMR_UART0_THR 	0xFFC01800	/* UART0 Transmit Holding */
#define ADSP21535_SYSMMR_UART0_RBR 	0xFFC01800	/* UART0 Receive Buffer */
#define ADSP21535_SYSMMR_UART0_DLL 	0xFFC01800	/* UART0 Divisor Latch Low-Byte */
#define ADSP21535_SYSMMR_UART0_IER 	0xFFC01802	/* UART0 Interrupt Enable */
#define ADSP21535_SYSMMR_UART0_DLH 	0xFFC01802	/* UART0 Divisor Latch High-Byte */
#define ADSP21535_SYSMMR_UART0_IIR 	0xFFC01804	/* UART0 Interrupt ID */
#define ADSP21535_SYSMMR_UART0_LCR 	0xFFC01806	/* UART0 Line Control */
#define ADSP21535_SYSMMR_UART0_MCR 	0xFFC01808	/* UART0 Module Control */
#define ADSP21535_SYSMMR_UART0_LSR 	0xFFC0180A	/* UART0 Line Status */
#define ADSP21535_SYSMMR_UART0_MSR 	0xFFC0180C	/* UART0 Modem Status */
#define ADSP21535_SYSMMR_UART0_SCR 	0xFFC0180E	/* UART0 Scratch */
#define ADSP21535_SYSMMR_UART0_IRCR 	0xFFC01810	/* UART0 Infrared Control */
#define ADSP21535_SYSMMR_UART0_CURR_PTR_RX 	0xFFC01A00	/* UART0 Receive DMA Current Descriptor Pointer */
#define ADSP21535_SYSMMR_UART0_CONFIG_RX	0xFFC01A02	/* UART0 Receive DMA Config */
#define ADSP21535_SYSMMR_UART0_START_ADDR_HI_RX	0xFFC01A04	/* UART0 Receive DMA Start Addr High */
#define ADSP21535_SYSMMR_UART0_START_ADDR_LO_RX	0xFFC01A06	/* UART0 Receive DMA Start Addr Low */
#define ADSP21535_SYSMMR_UART0_COUNT_RX		0xFFC01A08	/* UART0 Receive DMA Count */
#define ADSP21535_SYSMMR_UART0_NEXT_DESCR_RX	0xFFC01A0A	/* UART0 Receive DMA Next Descriptor Pointer */
#define ADSP21535_SYSMMR_UART0_DESCR_RDY_RX	0xFFC01A0C	/* UART0 Receive DMA Descriptor Ready */
#define ADSP21535_SYSMMR_UART0_IRQSTAT_RX	0xFFC01A0E	/* UART0 Receive DMA IRQ Status */
#define ADSP21535_SYSMMR_UART0_CURR_PTR_TX 	0xFFC01B00	/* UART0 Transmit DMA Current Descriptor Pointer */
#define ADSP21535_SYSMMR_UART0_CONFIG_TX	0xFFC01B02	/* UART0 Transmit DMA Config */
#define ADSP21535_SYSMMR_UART0_START_ADDR_HI_TX	0xFFC01B04	/* UART0 Transmit DMA Start Addr High */
#define ADSP21535_SYSMMR_UART0_START_ADDR_LO_TX	0xFFC01B06	/* UART0 Transmit DMA Start Addr Low */
#define ADSP21535_SYSMMR_UART0_COUNT_TX		0xFFC01B08	/* UART0 Transmit DMA Count */
#define ADSP21535_SYSMMR_UART0_NEXT_DESCR_TX	0xFFC01B0A	/* UART0 Transmit DMA Next Descriptor Pointer */
#define ADSP21535_SYSMMR_UART0_DESCR_RDY_TX	0xFFC01B0C	/* UART0 Transmit DMA Descriptor Ready */
#define ADSP21535_SYSMMR_UART0_IRQSTAT_TX	0xFFC01B0E	/* UART0 Transmit DMA IRQ Status */

/* UART 1 Controller */
#define ADSP21535_SYSMMR_UART1_THR 	0xFFC01C00	/* UART1 Transmit Holding */
#define ADSP21535_SYSMMR_UART1_RBR 	0xFFC01C00	/* UART1 Receive Buffer */
#define ADSP21535_SYSMMR_UART1_DLL 	0xFFC01C00	/* UART1 Divisor Latch Low-Byte */
#define ADSP21535_SYSMMR_UART1_IER 	0xFFC01C02	/* UART1 Interrupt Enable */
#define ADSP21535_SYSMMR_UART1_DLH 	0xFFC01C02	/* UART1 Divisor Latch High-Byte */
#define ADSP21535_SYSMMR_UART1_IIR 	0xFFC01C04	/* UART1 Interrupt ID */
#define ADSP21535_SYSMMR_UART1_LCR 	0xFFC01C06	/* UART1 Line Control */
#define ADSP21535_SYSMMR_UART1_MCR 	0xFFC01C08	/* UART1 Module Control */
#define ADSP21535_SYSMMR_UART1_LSR 	0xFFC01C0A	/* UART1 Line Status */
#define ADSP21535_SYSMMR_UART1_MSR 	0xFFC01C0C	/* UART1 Modem Status */
#define ADSP21535_SYSMMR_UART1_SCR 	0xFFC01C0E	/* UART1 Scratch */

#define ADSP21535_SYSMMR_UART1_CURR_PTR_RX 	0xFFC01E00	/* UART1 Receive DMA Current Descriptor Pointer */
#define ADSP21535_SYSMMR_UART1_CONFIG_RX	0xFFC01E02	/* UART1 Receive DMA Config */
#define ADSP21535_SYSMMR_UART1_START_ADDR_HI_RX	0xFFC01E04	/* UART1 Receive DMA Start Addr High */
#define ADSP21535_SYSMMR_UART1_START_ADDR_LO_RX	0xFFC01E06	/* UART1 Receive DMA Start Addr Low */
#define ADSP21535_SYSMMR_UART1_COUNT_RX		0xFFC01E08	/* UART1 Receive DMA Count */
#define ADSP21535_SYSMMR_UART1_NEXT_DESCR_RX	0xFFC01E0A	/* UART1 Receive DMA Next Descriptor Pointer */
#define ADSP21535_SYSMMR_UART1_DESCR_RDY_RX	0xFFC01E0C	/* UART1 Receive DMA Descriptor Ready */
#define ADSP21535_SYSMMR_UART1_IRQSTAT_RX	0xFFC01E0E	/* UART1 Receive DMA IRQ Status */
#define ADSP21535_SYSMMR_UART1_CURR_PTR_TX 	0xFFC01F00	/* UART1 Transmit DMA Current Descriptor Pointer */
#define ADSP21535_SYSMMR_UART1_CONFIG_TX	0xFFC01F02	/* UART1 Transmit DMA Config */
#define ADSP21535_SYSMMR_UART1_START_ADDR_HI_TX	0xFFC01F04	/* UART1 Transmit DMA Start Addr High */
#define ADSP21535_SYSMMR_UART1_START_ADDR_LO_TX	0xFFC01F06	/* UART1 Transmit DMA Start Addr Low */
#define ADSP21535_SYSMMR_UART1_COUNT_TX		0xFFC01F08	/* UART1 Transmit DMA Count */
#define ADSP21535_SYSMMR_UART1_NEXT_DESCR_TX	0xFFC01F0A	/* UART1 Transmit DMA Next Descriptor Pointer */
#define ADSP21535_SYSMMR_UART1_DESCR_RDY_TX	0xFFC01F0C	/* UART1 Transmit DMA Descriptor Ready */
#define ADSP21535_SYSMMR_UART1_IRQSTAT_TX	0xFFC01F0E	/* UART1 Transmit DMA IRQ Status */


/* Timer Regs */
#define ADSP21535_SYSMMR_TIMER0_STATUS	0xFFC02000	/* Timer 0 Global Status and Sticky */
#define ADSP21535_SYSMMR_TIMER0_CONFIG	0xFFC02002	/* Timer 0 Config */
#define ADSP21535_SYSMMR_TIMER0_COUNTER_LO	0xFFC02004	/* Timer 0 Counter (Low Word) */
#define ADSP21535_SYSMMR_TIMER0_COUNTER_HI	0xFFC02006	/* Timer 0 Counter (High Word) */
#define ADSP21535_SYSMMR_TIMER0_PERIOD_LO	0xFFC02008	/* Timer 0 Period (Low Word) */
#define ADSP21535_SYSMMR_TIMER0_PERIOD_HI	0xFFC0200A	/* Timer 0 Period (High Word) */
#define ADSP21535_SYSMMR_TIMER0_WIDTH_LO	0xFFC0200C	/* Timer 0 Width (Low Word) */
#define ADSP21535_SYSMMR_TIMER0_WIDTH_HI	0xFFC0200E	/* Timer 0 Width (High Word) */
#define ADSP21535_SYSMMR_TIMER1_STATUS	0xFFC02010	/* Timer 1 Global Status and Sticky */
#define ADSP21535_SYSMMR_TIMER1_CONFIG	0xFFC02012	/* Timer 1 Config */
#define ADSP21535_SYSMMR_TIMER1_COUNTER_LO	0xFFC02014	/* Timer 1 Counter (Low Word) */
#define ADSP21535_SYSMMR_TIMER1_COUNTER_HI	0xFFC02016	/* Timer 1 Counter (High Word) */
#define ADSP21535_SYSMMR_TIMER1_PERIOD_LO	0xFFC02018	/* Timer 1 Period (Low Word) */
#define ADSP21535_SYSMMR_TIMER1_PERIOD_HI	0xFFC0201A	/* Timer 1 Period (High Word) */
#define ADSP21535_SYSMMR_TIMER1_WIDTH_LO	0xFFC0201C	/* Timer 1 Width (Low Word) */
#define ADSP21535_SYSMMR_TIMER1_WIDTH_HI	0xFFC0201E	/* Timer 1 Width (High Word) */
#define ADSP21535_SYSMMR_TIMER2_STATUS	0xFFC02020	/* Timer 2 Global Status and Sticky */
#define ADSP21535_SYSMMR_TIMER2_CONFIG	0xFFC02022	/* Timer 2 Config */
#define ADSP21535_SYSMMR_TIMER2_COUNTER_LO	0xFFC02024	/* Timer 2 Counter (Low Word) */
#define ADSP21535_SYSMMR_TIMER2_COUNTER_HI	0xFFC02026	/* Timer 2 Counter (High Word) */
#define ADSP21535_SYSMMR_TIMER2_PERIOD_LO	0xFFC02028	/* Timer 2 Period (Low Word) */
#define ADSP21535_SYSMMR_TIMER2_PERIOD_HI	0xFFC0202A	/* Timer 2 Period (High Word) */
#define ADSP21535_SYSMMR_TIMER2_WIDTH_LO	0xFFC0202C	/* Timer 2 Width (Low Word) */
#define ADSP21535_SYSMMR_TIMER2_WIDTH_HI	0xFFC0202E	/* Timer 2 Width (High Word) */

/* Programmable Flag Regs */
#define ADSP21535_SYSMMR_FIO_DIR	0xFFC02400	/* Flag Direction */
#define ADSP21535_SYSMMR_FIO_FLAG_C	0xFFC02404	/* Flag Clear */
#define ADSP21535_SYSMMR_FIO_FLAG_S	0xFFC02406	/* Flag Set */
#define ADSP21535_SYSMMR_FIO_MASKA_C	0xFFC02408	/* Flag Int A Mask Clear */
#define ADSP21535_SYSMMR_FIO_MASKA_S	0xFFC0240A	/* Flag Int A Mask Set */
#define ADSP21535_SYSMMR_FIO_MASKB_C	0xFFC0240C	/* Flag Int B Mask Clear */
#define ADSP21535_SYSMMR_FIO_MASKB_S	0xFFC0240E	/* Flag Int B Mask Set */
#define ADSP21535_SYSMMR_FIO_POLAR	0xFFC02410	/* Flag Polarity */
#define ADSP21535_SYSMMR_FIO_EDGE	0xFFC02414	/* Flag Interrupt Sensitivity */
#define ADSP21535_SYSMMR_FIO_BOTH	0xFFC02418	/* Flag Set on Both Edges */

/* SPORT0 Controller Regs */
#define ADSP21535_SYSMMR_SPORT0_TX_CONFIG	0xFFC02800	/* SPORT0 Transmit Config */
#define ADSP21535_SYSMMR_SPORT0_RX_CONFIG	0xFFC02802	/* SPORT0 Receive Config */
#define ADSP21535_SYSMMR_SPORT0_TX	0xFFC02804	/* SPORT0 TX Transmit */
#define ADSP21535_SYSMMR_SPORT0_RX	0xFFC02806	/* SPORT0 RX Receive */
#define ADSP21535_SYSMMR_SPORT0_TSCLKDIV	0xFFC02808	/* SPORT0 Transmit Serial Clock Divider */
#define ADSP21535_SYSMMR_SPORT0_RSCLKDIV	0xFFC0280A	/* SPORT0 Receive Serial Clock Divider */
#define ADSP21535_SYSMMR_SPORT0_TFSDIV	0xFFC0280C	/* SPORT0 Transmit Frame Sync Divider */
#define ADSP21535_SYSMMR_SPORT0_RFSDIV	0xFFC0280E	/* SPORT0 Receive Frame Sync Divider */
#define ADSP21535_SYSMMR_SPORT0_STAT	0xFFC02810	/* SPORT0 Status */
#define ADSP21535_SYSMMR_SPORT0_MTCS0	0xFFC02812	/* SPORT0 Multi-channel Transmit Select */
#define ADSP21535_SYSMMR_SPORT0_MTCS1	0xFFC02814	/* SPORT0 Multi-channel Transmit Select */
#define ADSP21535_SYSMMR_SPORT0_MTCS2	0xFFC02816	/* SPORT0 Multi-channel Transmit Select */
#define ADSP21535_SYSMMR_SPORT0_MTCS3	0xFFC02818	/* SPORT0 Multi-channel Transmit Select */
#define ADSP21535_SYSMMR_SPORT0_MTCS4	0xFFC0281A	/* SPORT0 Multi-channel Transmit Select */
#define ADSP21535_SYSMMR_SPORT0_MTCS5	0xFFC0281C	/* SPORT0 Multi-channel Transmit Select */
#define ADSP21535_SYSMMR_SPORT0_MTCS6	0xFFC0281E	/* SPORT0 Multi-channel Transmit Select */
#define ADSP21535_SYSMMR_SPORT0_MTCS7	0xFFC02820	/* SPORT0 Multi-channel Transmit Select */
#define ADSP21535_SYSMMR_SPORT0_MRCS0	0xFFC02822	/* SPORT0 Multi-channel Receive Select */
#define ADSP21535_SYSMMR_SPORT0_MRCS1	0xFFC02824	/* SPORT0 Multi-channel Receive Select */
#define ADSP21535_SYSMMR_SPORT0_MRCS2	0xFFC02826	/* SPORT0 Multi-channel Receive Select */
#define ADSP21535_SYSMMR_SPORT0_MRCS3	0xFFC02828	/* SPORT0 Multi-channel Receive Select */
#define ADSP21535_SYSMMR_SPORT0_MRCS4	0xFFC0282A	/* SPORT0 Multi-channel Receive Select */
#define ADSP21535_SYSMMR_SPORT0_MRCS5	0xFFC0282C	/* SPORT0 Multi-channel Receive Select */
#define ADSP21535_SYSMMR_SPORT0_MRCS6	0xFFC0282E	/* SPORT0 Multi-channel Receive Select */
#define ADSP21535_SYSMMR_SPORT0_MRCS7	0xFFC02830	/* SPORT0 Multi-channel Receive Select */
#define ADSP21535_SYSMMR_SPORT0_MCMC1	0xFFC02832	/* SPORT0 Multi-channel Config */
#define ADSP21535_SYSMMR_SPORT0_MCM2	0xFFC02834	/* SPORT0 Multi-channel Config */
#define ADSP21535_SYSMMR_SPORT0_CURR_PTR_RX	0xFFC02A00	/* SPORT0 Receive DMA Current Descriptor Pointer */
#define ADSP21535_SYSMMR_SPORT0_CONFIG_DMA_RX	0xFFC02A02	/* SPORT0 Receive DMA Config */
#define ADSP21535_SYSMMR_SPORT0_START_ADDR_HI_RX 0xFFC02A04	/* SPORT0 Receive DMA Start Address HI */
#define ADSP21535_SYSMMR_SPORT0_START_ADDR_LO_RX 0xFFC02A06	/* SPORT0 Receive DMA Start Address LO */
#define ADSP21535_SYSMMR_SPORT0_COUNT_RX	0xFFC02A08	/* SPORT0 Receive DMA Count */
#define ADSP21535_SYSMMR_SPORT0_NEXT_DESCR_RX	0xFFC02A0A	/* SPORT0 Receive DMA Next Descriptor Pointer */
#define ADSP21535_SYSMMR_SPORT0_DESCR_RDY_RX	0xFFC02A0C	/* SPORT0 Receive DMA Descriptor Ready */
#define ADSP21535_SYSMMR_SPORT0_IRQSTAT_RX	0xFFC02A0E	/* SPORT0 Receive DMA IRQ */
#define ADSP21535_SYSMMR_SPORT0_CURR_PTR_TX	0xFFC02B00	/* SPORT0 Transmit DMA Current Descriptor Pointer */
#define ADSP21535_SYSMMR_SPORT0_CONFIG_DMA_TX	0xFFC02B02	/* SPORT0 Transmit DMA Config */
#define ADSP21535_SYSMMR_SPORT0_START_ADDR_HI_TX 0xFFC02B04	/* SPORT0 Transmit DMA Start Address HI */
#define ADSP21535_SYSMMR_SPORT0_START_ADDR_LO_TX 0xFFC02B06	/* SPORT0 Transmit DMA Start Address LO */
#define ADSP21535_SYSMMR_SPORT0_COUNT_TX	0xFFC02B08	/* SPORT0 Transmit DMA Count */
#define ADSP21535_SYSMMR_SPORT0_NEXT_DESCR_TX	0xFFC02B0A	/* SPORT0 Transmit DMA Next Descriptor Pointer */
#define ADSP21535_SYSMMR_SPORT0_DESCR_RDY_TX	0xFFC02B0C	/* SPORT0 Transmit DMA Descriptor Ready */
#define ADSP21535_SYSMMR_SPORT0_IRQSTAT_TX	0xFFC02B0E	/* SPORT0 Transmit DMA IRQ */

/* SPORT1 Controller Regs */
#define ADSP21535_SYSMMR_SPORT1_TX_CONFIG	0xFFC02C00	/* SPORT1 Transmit Config */
#define ADSP21535_SYSMMR_SPORT1_RX_CONFIG	0xFFC02C02	/* SPORT1 Receive Config */
#define ADSP21535_SYSMMR_SPORT1_TX	0xFFC02C04	/* SPORT1 TX Transmit */
#define ADSP21535_SYSMMR_SPORT1_RX	0xFFC02C06	/* SPORT1 RX Receive */
#define ADSP21535_SYSMMR_SPORT1_TSCLKDIV	0xFFC02C08	/* SPORT1 Transmit Serial Clock Divider */
#define ADSP21535_SYSMMR_SPORT1_RSCLKDIV	0xFFC02C0A	/* SPORT1 Receive Serial Clock Divider */
#define ADSP21535_SYSMMR_SPORT1_TFSDIV	0xFFC02C0C	/* SPORT1 Transmit Frame Sync Divider */
#define ADSP21535_SYSMMR_SPORT1_RFSDIV	0xFFC02C0E	/* SPORT1 Receive Frame Sync Divider */
#define ADSP21535_SYSMMR_SPORT1_STAT	0xFFC02C10	/* SPORT1 Status */
#define ADSP21535_SYSMMR_SPORT1_MTCS0	0xFFC02C12	/* SPORT1 Multi-channel Transmit Select */
#define ADSP21535_SYSMMR_SPORT1_MTCS1	0xFFC02C14	/* SPORT1 Multi-channel Transmit Select */
#define ADSP21535_SYSMMR_SPORT1_MTCS2	0xFFC02C16	/* SPORT1 Multi-channel Transmit Select */
#define ADSP21535_SYSMMR_SPORT1_MTCS3	0xFFC02C18	/* SPORT1 Multi-channel Transmit Select */
#define ADSP21535_SYSMMR_SPORT1_MTCS4	0xFFC02C1A	/* SPORT1 Multi-channel Transmit Select */
#define ADSP21535_SYSMMR_SPORT1_MTCS5	0xFFC02C1C	/* SPORT1 Multi-channel Transmit Select */
#define ADSP21535_SYSMMR_SPORT1_MTCS6	0xFFC02C1E	/* SPORT1 Multi-channel Transmit Select */
#define ADSP21535_SYSMMR_SPORT1_MTCS7	0xFFC02C20	/* SPORT1 Multi-channel Transmit Select */
#define ADSP21535_SYSMMR_SPORT1_MRCS0	0xFFC02C22	/* SPORT1 Multi-channel Receive Select */
#define ADSP21535_SYSMMR_SPORT1_MRCS1	0xFFC02C24	/* SPORT1 Multi-channel Receive Select */
#define ADSP21535_SYSMMR_SPORT1_MRCS2	0xFFC02C26	/* SPORT1 Multi-channel Receive Select */
#define ADSP21535_SYSMMR_SPORT1_MRCS3	0xFFC02C28	/* SPORT1 Multi-channel Receive Select */
#define ADSP21535_SYSMMR_SPORT1_MRCS4	0xFFC02C2A	/* SPORT1 Multi-channel Receive Select */
#define ADSP21535_SYSMMR_SPORT1_MRCS5	0xFFC02C2C	/* SPORT1 Multi-channel Receive Select */
#define ADSP21535_SYSMMR_SPORT1_MRCS6	0xFFC02C2E	/* SPORT1 Multi-channel Receive Select */
#define ADSP21535_SYSMMR_SPORT1_MRCS7	0xFFC02C30	/* SPORT1 Multi-channel Receive Select */
#define ADSP21535_SYSMMR_SPORT1_MCMC1	0xFFC02C32	/* SPORT1 Multi-channel Config */
#define ADSP21535_SYSMMR_SPORT1_MCM2	0xFFC02C34	/* SPORT1 Multi-channel Config */
#define ADSP21535_SYSMMR_SPORT1_CURR_PTR_RX	0xFFC02E00	/* SPORT1 Receive DMA Current Descriptor Pointer */
#define ADSP21535_SYSMMR_SPORT1_CONFIG_DMA_RX	0xFFC02E02	/* SPORT1 Receive DMA Config */
#define ADSP21535_SYSMMR_SPORT1_START_ADDR_HI_RX 0xFFC02E04	/* SPORT1 Receive DMA Start Address HI */
#define ADSP21535_SYSMMR_SPORT1_START_ADDR_LO_RX 0xFFC02E06	/* SPORT1 Receive DMA Start Address LO */
#define ADSP21535_SYSMMR_SPORT1_COUNT_RX	0xFFC02E08	/* SPORT1 Receive DMA Count */
#define ADSP21535_SYSMMR_SPORT1_NEXT_DESCR_RX	0xFFC02E0A	/* SPORT1 Receive DMA Next Descriptor Pointer */
#define ADSP21535_SYSMMR_SPORT1_DESCR_RDY_RX	0xFFC02E0C	/* SPORT1 Receive DMA Descriptor Ready */
#define ADSP21535_SYSMMR_SPORT1_IRQSTAT_RX	0xFFC02E0E	/* SPORT1 Receive DMA IRQ */
#define ADSP21535_SYSMMR_SPORT1_CURR_PTR_TX	0xFFC02F00	/* SPORT1 Transmit DMA Current Descriptor Pointer */
#define ADSP21535_SYSMMR_SPORT1_CONFIG_DMA_TX	0xFFC02F02	/* SPORT1 Transmit DMA Config */
#define ADSP21535_SYSMMR_SPORT1_START_ADDR_HI_TX 0xFFC02F04	/* SPORT1 Transmit DMA Start Address HI */
#define ADSP21535_SYSMMR_SPORT1_START_ADDR_LO_TX 0xFFC02F06	/* SPORT1 Transmit DMA Start Address LO */
#define ADSP21535_SYSMMR_SPORT1_COUNT_TX	0xFFC02F08	/* SPORT1 Transmit DMA Count */
#define ADSP21535_SYSMMR_SPORT1_NEXT_DESCR_TX	0xFFC02F0A	/* SPORT1 Transmit DMA Next Descriptor Pointer */
#define ADSP21535_SYSMMR_SPORT1_DESCR_RDY_TX	0xFFC02F0C	/* SPORT1 Transmit DMA Descriptor Ready */
#define ADSP21535_SYSMMR_SPORT1_IRQSTAT_TX	0xFFC02F0E	/* SPORT1 Transmit DMA IRQ */

/* SPI0 Controller Regs */
#define ADSP21535_SYSMMR_SPI0_CTL	0xFFC03000	/* SPI0 Control */
#define ADSP21535_SYSMMR_SPI0_FLG	0xFFC03002	/* SPI0 Flag */
#define ADSP21535_SYSMMR_SPI0_ST	0xFFC03004	/* SPI0 Status */
#define ADSP21535_SYSMMR_SPI0_TDBR	0xFFC03006	/* SPI0 Transmit Data Buffer */
#define ADSP21535_SYSMMR_SPI0_RDBR	0xFFC03008	/* SPI0 Receive Data Buffer */
#define ADSP21535_SYSMMR_SPI0_BAUD	0xFFC0300A	/* SPI0 Baud Rate */
#define ADSP21535_SYSMMR_SPI0_SHADOW	0xFFC0300C	/* SPI0 RDBR Shadow */
#define ADSP21535_SYSMMR_SPI0_CURR_PTR	0xFFC03200	/* SPI0 DMA Current Descriptor Pointer */
#define ADSP21535_SYSMMR_SPI0_CONFIG	0xFFC03202	/* SPI0 DMA Config */
#define ADSP21535_SYSMMR_SPI0_START_ADDR_HI	0xFFC03204	/* SPI0 DMA Start Address High */
#define ADSP21535_SYSMMR_SPI0_START_ADDR_LO	0xFFC03206	/* SPI0 DMA Start Address Low */
#define ADSP21535_SYSMMR_SPI0_COUNT	0xFFC03208	/* SPI0 DMA Count */
#define ADSP21535_SYSMMR_SPI0_NEXT_DESCR	0xFFC0320A	/* SPI0 DMA Next Descriptor Pointer */
#define ADSP21535_SYSMMR_SPI0_DESCR_RDY	0xFFC0320C	/* SPI0 DMA Descriptor Ready */
#define ADSP21535_SYSMMR_SPI0_DMA_INT	0xFFC0320E	/* SPI0 DMA Interrupt */

/* SPI1 Controller Regs */
#define ADSP21535_SYSMMR_SPI1_CTL	0xFFC03400	/* SPI1 Control */
#define ADSP21535_SYSMMR_SPI1_FLG	0xFFC03402	/* SPI1 Flag */
#define ADSP21535_SYSMMR_SPI1_ST	0xFFC03404	/* SPI1 Status */
#define ADSP21535_SYSMMR_SPI1_TDBR	0xFFC03406	/* SPI1 Transmit Data Buffer */
#define ADSP21535_SYSMMR_SPI1_RDBR	0xFFC03408	/* SPI1 Receive Data Buffer */
#define ADSP21535_SYSMMR_SPI1_BAUD	0xFFC0340A	/* SPI1 Baud Rate */
#define ADSP21535_SYSMMR_SPI1_SHADOW	0xFFC0340C	/* SPI1 RDBR Shadow */
#define ADSP21535_SYSMMR_SPI1_CURR_PTR	0xFFC03600	/* SPI1 DMA Current Descriptor Pointer */
#define ADSP21535_SYSMMR_SPI1_CONFIG	0xFFC03602	/* SPI1 DMA Config */
#define ADSP21535_SYSMMR_SPI1_START_ADDR_HI	0xFFC03604	/* SPI1 DMA Start Address High */
#define ADSP21535_SYSMMR_SPI1_START_ADDR_LO	0xFFC03606	/* SPI1 DMA Start Address Low */
#define ADSP21535_SYSMMR_SPI1_COUNT	0xFFC03608	/* SPI1 DMA Count */
#define ADSP21535_SYSMMR_SPI1_NEXT_DESCR	0xFFC0360A	/* SPI1 DMA Next Descriptor Pointer */
#define ADSP21535_SYSMMR_SPI1_DESCR_RDY	0xFFC0360C	/* SPI1 DMA Descriptor Ready */
#define ADSP21535_SYSMMR_SPI1_DMA_INT	0xFFC0360E	/* SPI1 DMA Interrupt */

/* Memory DMA Controller Regs */
#define ADSP21535_SYSMMR_MDW_DCP	0xFFC03800	/* pDMA Memory DMA Current Descriptor Pointer - Write Channel */
#define ADSP21535_SYSMMR_MDW_DCFG	0xFFC03802	/* pDMA Memory DMA Config - Write Channel */
#define ADSP21535_SYSMMR_MDW_DSAH	0xFFC03804	/* pDMA Memory DMA Start Address High - Write Channel */
#define ADSP21535_SYSMMR_MDW_DSAL	0xFFC03806	/* pDMA Memory DMA Start Address Low - Write Channel */
#define ADSP21535_SYSMMR_MDW_DCT	0xFFC03808	/* pDMA Memory DMA Count - Write Channel */
#define ADSP21535_SYSMMR_MDW_DND	0xFFC0380A	/* pDMA Memory DMA Next Descriptor Pointer - Write Channel */
#define ADSP21535_SYSMMR_MDW_DDR	0xFFC0380C	/* pDMA Memory DMA Descriptor Ready - Write Channel */
#define ADSP21535_SYSMMR_MDW_DI		0xFFC0380E	/* pDMA Memory DMA Interrupt - Write Channel */
#define ADSP21535_SYSMMR_MDR_DCP	0xFFC03800	/* sDMA Memory DMA Current Descriptor Pointer - Read Channel */
#define ADSP21535_SYSMMR_MDR_DCFG	0xFFC03802	/* sDMA Memory DMA Config - Read Channel */
#define ADSP21535_SYSMMR_MDR_DSAH	0xFFC03804	/* sDMA Memory DMA Start Address High - Read Channel */
#define ADSP21535_SYSMMR_MDR_DSAL	0xFFC03806	/* sDMA Memory DMA Start Address Low - Read Channel */
#define ADSP21535_SYSMMR_MDR_DCT	0xFFC03808	/* sDMA Memory DMA Count - Read Channel */
#define ADSP21535_SYSMMR_MDR_DND	0xFFC0380A	/* sDMA Memory DMA Next Descriptor Pointer - Read Channel */
#define ADSP21535_SYSMMR_MDR_DDR	0xFFC0380C	/* sDMA Memory DMA Descriptor Ready - Read Channel */
#define ADSP21535_SYSMMR_MDR_DI		0xFFC0380E	/* sDMA Memory DMA Interrupt - Read Channel */

/* Asynchronous Memory Controller - EBIU */
#define ADSP21535_SYSMMR_EBIU_AMGCTL	0xFFC03C00	/* Asynch Mem Global Control */
#define ADSP21535_SYSMMR_EBIU_AMBCTL0	0xFFC03C04	/* Asynch Mem Bank Control 0 */
#define ADSP21535_SYSMMR_EBIU_AMBCTL1	0xFFC03C08	/* Asynch Mem Bank Control 1 */

/* PCI Bridge PAB Regs */
#define ADSP21535_SYSMMR_PCI_CTL	0xFFC04000	/* PCI Bridge Control */
#define  ADSP21535_PCI_CTL_HOST	 	 0x01
#define  ADSP21535_PCI_CTL_ENABPCI	 0x02
#define  ADSP21535_PCI_CTL_FASTBCK2BCK	 0x04
#define  ADSP21535_PCI_CTL_ENABINTA	 0x08
#define  ADSP21535_PCI_CTL_OUTPUTINTA	 0x10
#define  ADSP21535_PCI_CTL_ENABRST	 0x20
#define  ADSP21535_PCI_CTL_OUTPUTRST	 0x40

#define ADSP21535_SYSMMR_PCI_STAT	0xFFC04004	/* PCI Status */
#define   ADSP21535_PCI_STAT_INTA	 0x0001
#define   ADSP21535_PCI_STAT_INTB	 0x0002
#define   ADSP21535_PCI_STAT_INTC	 0x0004
#define   ADSP21535_PCI_STAT_INTD	 0x0008
#define   ADSP21535_PCI_STAT_PARERR	 0x0010
#define   ADSP21535_PCI_STAT_FATERR	 0x0020
#define   ADSP21535_PCI_STAT_RESET	 0x0040
#define   ADSP21535_PCI_STAT_TXEMPTY	 0x0080
#define   ADSP21535_PCI_STAT_TXFULL	 0x0100
#define   ADSP21535_PCI_STAT_QUEFULL	 0x0200
#define   ADSP21535_PCI_STAT_MEMWRINV	 0x0400
#define   ADSP21535_PCI_STAT_INRDERR	 0x0800
#define   ADSP21535_PCI_STAT_INWRERR	 0x1000
#define   ADSP21535_PCI_STAT_INVEABACC	 0x2000
#define   ADSP21535_PCI_STAT_SYSERR	 0x4000
#define ADSP21535_SYSMMR_PCI_ICTL	0xFFC04008	/* PCI Interrupt Controller */
#define   ADSP21535_PCI_ICTL_INTA	 0x0001
#define   ADSP21535_PCI_ICTL_INTB	 0x0002
#define   ADSP21535_PCI_ICTL_INTC	 0x0004
#define   ADSP21535_PCI_ICTL_INTD	 0x0008
#define   ADSP21535_PCI_ICTL_PARERR	 0x0010
#define   ADSP21535_PCI_ICTL_FATERR	 0x0020
#define   ADSP21535_PCI_ICTL_RESET	 0x0040
#define   ADSP21535_PCI_ICTL_TXFULL	 0x0080
#define   ADSP21535_PCI_ICTL_MEMWRINV	 0x0400
#define   ADSP21535_PCI_ICTL_INRDERR	 0x0800
#define   ADSP21535_PCI_ICTL_INWRERR	 0x1000
#define   ADSP21535_PCI_ICTL_INVEABACC	 0x2000
#define   ADSP21535_PCI_ICTL_SYSERR	 0x4000
#define ADSP21535_SYSMMR_PCI_MBAP	0xFFC0400C	/* PCI Outbound Memory Base Address */
#define ADSP21535_SYSMMR_PCI_IBAP	0xFFC04010	/* PCI Outbound I/O Base Address */
#define ADSP21535_SYSMMR_PCI_CBAP	0xFFC04014	/* PCI Outbound Config Base Address */
#define ADSP21535_SYSMMR_PCI_TMBAP	0xFFC04018	/* PCI Inbound Memory Base Address */
#define ADSP21535_SYSMMR_PCI_TIBAP	0xFFC0401C	/* PCI Inbound I/O Base Address */

/* PCI Bridge External Access Bus Regs */
#define ADSP21535_SYSMMR_PCI_DMBARM	0xEEFFFF00	/* PCI Device Memory BAR Mask */
#define ADSP21535_SYSMMR_PCI_DIBARM	0xEEFFFF04	/* PCI Device I/O BAR Mask */
#define ADSP21535_SYSMMR_PCI_CFG_DIC	0xEEFFFF08	/* PCI Config Device ID */
#define ADSP21535_SYSMMR_PCI_CFG_VIC	0xEEFFFF0C	/* PCI Config Vendor ID */
#define ADSP21535_SYSMMR_PCI_CFG_STAT	0xEEFFFF10	/* PCI Config Status */
#define ADSP21535_SYSMMR_PCI_CFG_CMD	0xEEFFFF14	/* PCI Config Command */
#define ADSP21535_SYSMMR_PCI_CFG_CC	0xEEFFFF18	/* PCI Config Class Code */
#define ADSP21535_SYSMMR_PCI_CFG_RID	0xEEFFFF1C	/* PCI Config Revision ID */
#define ADSP21535_SYSMMR_PCI_CFG_BIST	0xEEFFFF20	/* PCI Config Built-in Self Test */
#define ADSP21535_SYSMMR_PCI_CFG_HT	0xEEFFFF24	/* PCI Config Header Type */
#define ADSP21535_SYSMMR_PCI_CFG_MLT	0xEEFFFF28	/* PCI Config Memory Latency Timer */
#define ADSP21535_SYSMMR_PCI_CFG_CLS	0xEEFFFF2C	/* PCI Config Cache Line Size */
#define ADSP21535_SYSMMR_PCI_CFG_MBAR	0xEEFFFF30	/* PCI Config Memory Base Address */
#define ADSP21535_SYSMMR_PCI_CFG_IBAR	0xEEFFFF34	/* PCI Config I/O Base Address */
#define ADSP21535_SYSMMR_PCI_CFG_SID	0xEEFFFF38	/* PCI Config Subsystem ID */
#define ADSP21535_SYSMMR_PCI_CFG_SVID	0xEEFFFF3C	/* PCI Config Subsystem Vendor ID */
#define ADSP21535_SYSMMR_PCI_CFG_MAXL	0xEEFFFF40	/* PCI Config Max Latency */
#define ADSP21535_SYSMMR_PCI_CFG_MING	0xEEFFFF44	/* PCI Config Min Grant */
#define ADSP21535_SYSMMR_PCI_CFG_IP	0xEEFFFF48	/* PCI Config Interrupt Pin */
#define ADSP21535_SYSMMR_PCI_CFG_IL	0xEEFFFF4C	/* PCI Config Interrupt Line */
#define ADSP21535_SYSMMR_PCI_HMCTL	0xEEFFFF50	/* PCI Host Memory Control */
#define  ADSP21535_PCI_HMCTL_SYSMMRENAB	 0x1
#define  ADSP21535_PCI_HMCTL_L2ENAB	 0x2
#define  ADSP21535_PCI_HMCTL_ASYNCENAB	 0x4
#define  ADSP21535_PCI_HMCTL_ASYNCSIZE	 0x18		/* 00 - 64 MB, 01 - 128 MB, 10 - 192 MB, 11 - 256 MB */
#define  ADSP21535_PCI_HMCTL_SDRAMENAB	 0x20
#define  ADSP21535_PCI_HMCTL_SDRAMSIZE	 0x7C0		/* 0 - 32 MB, 1 - 64 MB, 2 - 96 MB, 128 MB, 160 MB */

/* System Bus Interface Unit (SBIU) Regs */
#define ADSP21535_SYSMMR_L1SBAR		0xFFC04840	/* L1 SRAM Base Address */
#define ADSP21535_SYSMMR_L1CSR		0xFFC04844	/* L1 SRAM Control Initialization */
#define ADSP21535_SYSMMR_DB_NDBP	0xFFC04880	/* DMA Next Descriptor Base Pointer */
#define ADSP21535_SYSMMR_DB_ACOMP	0xFFC04884	/* DMA Bus Address Comparator */
#define ADSP21535_SYSMMR_DB_CCOMP	0xFFC04888	/* DMA Bus Control Comparator */

/* SDRAM Controller External Bus Interface Unit (EBIU) Regs */
#define ADSP21535_SYSMMR_EBIU_SDGCTL	0xFFC04C00	/* SDRAM Global Control */
#define ADSP21535_SYSMMR_EBIU_SDBCTL	0xFFC04C04	/* SDRAM Bank Control */
#define ADSP21535_SYSMMR_EBIU_SDRRC	0xFFC04C0A	/* SDRAM Refresh Control */
#define ADSP21535_SYSMMR_EBIU_SDSTAT	0xFFC04C0E	/* SDRAM Control Status */

/*
 * Core Memory-Mapped Registers
 */

/* Event handlers */
#define ADSP21535_EVENT_BASE		0xFFE02000

/* Core Interrupt Controller Registers */
#define ADSP21535_COREMMR_IMASK		0xFFE02104
#define ADSP21535_COREMMR_IPEND		0xFFE02108
#define ADSP21535_COREMMR_ILAT		0xFFE0210C

/*
 * Interrupt Mask Bit Assigments
 */
#define ADSP21535_IMASK_IVHW	0x0020		/* Hardware Exception */
#define ADSP21535_IMASK_IVTMR	0x0040		/* Timer */
#define ADSP21535_IMASK_IVG7	0x0080		/* User-Interrupt 7 */
#define ADSP21535_IMASK_IVG8	0x0100		/* User-Interrupt 8 */
#define ADSP21535_IMASK_IVG9	0x0200		/* User-Interrupt 9 */
#define ADSP21535_IMASK_IVG10	0x0400		/* User-Interrupt 10 */
#define ADSP21535_IMASK_IVG11	0x0800		/* User-Interrupt 11 */
#define ADSP21535_IMASK_IVG12	0x1000		/* User-Interrupt 12 */
#define ADSP21535_IMASK_IVG13	0x2000		/* User-Interrupt 13 */
#define ADSP21535_IMASK_IVG14	0x4000		/* User-Interrupt 14 */
#define ADSP21535_IMASK_IVG15	0x8000		/* User-Interrupt 15 */

/*
 * Event Table Offsets
 */
#define ADSP21535_EVENTOFFSET_EMUN	0x00	/* Emulation */
#define ADSP21535_EVENTOFFSET_IRST	0x04	/* Reset */
#define ADSP21535_EVENTOFFSET_NMI	0x08	/* Non-maskable Interrupt */
#define ADSP21535_EVENTOFFSET_EVSW	0x0C	/* Software Exception */
#define ADSP21535_EVENTOFFSET_IVHW	0x14	/* Hardware Exception */
#define ADSP21535_EVENTOFFSET_TMR	0x18	/* Timer */
#define ADSP21535_EVENTOFFSET_IVG7	0x1C	/* User-defined */
#define ADSP21535_EVENTOFFSET_IVG8	0x20	/* User-defined */
#define ADSP21535_EVENTOFFSET_IVG9	0x24	/* User-defined */
#define ADSP21535_EVENTOFFSET_IVG10	0x28	/* User-defined */
#define ADSP21535_EVENTOFFSET_IVG11	0x2C	/* User-defined */
#define ADSP21535_EVENTOFFSET_IVG12	0x30	/* User-defined */
#define ADSP21535_EVENTOFFSET_IVG13	0x34	/* User-defined */
#define ADSP21535_EVENTOFFSET_IVG14	0x38	/* User-defined */
#define ADSP21535_EVENTOFFSET_IVG15	0x3C	/* User-defined */

#endif	/* _ADSP_21535_H_ */

