/*******************************************************************************
 *
 * $Source$
 * $Revision: 1 $
 *
 * Audio codec test functionality, header file.
 *
 * $Log: /Project/Units/Tasha2/BlackFin/include/ac_init.h $
 * 
 * 1     31.07.03 14:32 Martin.lueck
 * Wichtige Header und asm files für den BF535
 *
 ******************************************************************************/

#ifndef _AC_INIT_H_
#define _AC_INIT_H_

#include    "m21535.h"

// note need to correspond to settings for
// codec sample rate

#define  sampleRate            8000

//..............................................................................

// Status bits in rxStatus

#define bAudioData              15

//..............................................................................

// TDM Timeslot Definitions DSP Transmit and Receive

	// slots

#define	oLeft		    	6
#define	oRght		    	8

	// bit masks

#define kVldCmd			0x4000 // (slot 0)
#define kVldAddr		0x2000 // (slot 0)

//..............................................................................

// TDM Timeslot Definitions DSP Transmit 

	// slots

#define	oTagPhase		0
#define	oCmdAddrSlot    	2
#define	oCmdDataSlot		4

	// bit masks

#define	kVldFrm			0x8000 // (slot 0)
#define	kAddrRead		0x8000 // (slot 1)

//..............................................................................

// TDM Timeslot Definitions DSP Receive

	// slots

#define	oStatusAddrSlot	        2
#define	oStatusDataSlot	        4

	// bit positions

#define	bCodecRdy		15	// (slot 0)
#define	bLeftVld		12	// (slot 0)
#define	bRghtVld		11	// (slot 0)

//..............................................................................

// SPORTx_TX_CONFIG


#define	kTX_CFG	\
		ZET(bTxTSPEN,0)+	/* Transmit disabled */		    \
		ZET(bTxICLK,0)+ 	/* External transmit clock */       \
		ZET(bTxDTYPE,0)+ 	/* Right justify and zero fill */   \
		ZET(bTxSENDN,0)+	/* Transmit most sign. bit first */ \
		ZET(bTxSLEN,15)+  	/* Serial word length 16 bits */    \
		ZET(bTxITFS,0)+  	/* External TFS used */             \
		ZET(bTxTFSR,0)+   /* Does not require TFS for every word*/  \
		ZET(bTxDITFS,0)+ 	/* Data dependent TFS used */       \
		ZET(bTxLTFS,0)+  	/* Active high TFS */               \
		ZET(bTxLATFS,0)+ 	/* Early frame syncs */             \
		ZET(bTxCKFE,0)+  	/* Rising edge on clocks used */    \
		ZET(bTxICLKD,0) 	/* TCLK enabled */

// SPORTx_RX_CONFIG

#define	kRX_CFG	\
		ZET(bRxRSPEN,0)+	/* Receive disabled */              \
		ZET(bRxICLK,0)+ 	/* External receive clock */        \
		ZET(bRxDTYPE,0)+ 	/* Right justify and zero fill */   \
		ZET(bRxSENDN,0)+ 	/* Transmit most sign. bit first */ \
		ZET(bRxSLEN,15)+  	/* Serial word length 16 bits */    \
		ZET(bRxIRFS,1)+  	/* Internal RFS used */             \
		ZET(bRxRFSR,0)+   /* Does not require RFS for every word*/  \
		ZET(bRxLRFS,0)+  	/* Active high RFS */               \
		ZET(bRxLARFS,0)+ 	/* Early frame syncs */             \
		ZET(bRxCKFE,0)+  	/* Rising edge on clocks used */    \
		ZET(bRxICLKD,0) 	/* RCLK enabled */


// SPORTx_IRQSTAT_RX

#define	kIRQSTAT_RX \
		ZET(bRxCOMPL,1)+	/* Completion IRQ status */	\
		ZET(bRxERR,1)+ 	        /* Error IRQ Status */ 		\
		ZET(bRxBUSERR,1)	/* Bus error IRQ status */

// SPORTx_IRQSTAT_TX

#define	kIRQSTAT_TX \
		ZET(bTxCOMPL,1)+	/* Completion IRQ status */	\
		ZET(bTxERR,1)+	        /* Error IRQ Status */ 		\
		ZET(bTxBUSERR,1)	/* Bus error IRQ status */

// SPORTx_MCMC1

#define	kMCMC1 \
		ZET(bMCM,1)+		/* Multichannels operations enabled */\
		ZET(bMFD,1)+   	        /* Multichannel frame delay */	      \
		ZET(bWSIZE,1)+ 	        /* Window size 16 */                  \
		ZET(bWOFF,0)		/* No offset */

// SPORTx_MCMC2

#define	kMCMC2 \
		ZET(bMCCRM,0)+		/* Bypass mode */                     \
		ZET(bMCDTXPE,1)+	/* Multich. DMA tx packing enabled */ \
		ZET(bMCDRXPE,1)+	/* Multich. DMA rx packing enabled */ \
		ZET(bMCOM,0)+		/* Ch. select offset mode disabled */ \
		ZET(bMCFF,0)+		/* Tx fifo prefetch max dist. 2 ch. */\
		ZET(bFSDR,0)		/* Frame sync. to data rela. disabled */

// SPORTx_CONFIG_DMA_TX

#define kTX_DMA_CFG \
		ZET(bTxDEN,1)+	        /* DMA enabled */                    \
		ZET(bTxTRAN,0)+  	/* Memory read */                    \
		ZET(bTxDCOME,0)+ 	/* Interrupt on completion disabled*/\
		ZET(bTxDSB0,0)+  	/* 16-bit halfword */                             \
		ZET(bTxDAUTO,1)+	/* Autobuffer mode enabled */        \
		ZET(bTxFLSH,0)+  	/* Normal buffer operation */        \
		ZET(bTxDERE,0)+  	/* Interrupt on error disabled */    \
		ZET(bTxDSB1,0)    	/* 16-bit halfword */  

// SPORTx_CONFIG_DMA_RX

#define kRX_DMA_CFG \
		ZET(bRxDEN,1)+	        /* DMA enabled */                    \
		ZET(bRxTRAN,1)+  	/* Memory write */                   \
		ZET(bRxDCOME,1)+ 	/* Interrupt on completion enabled */\
		ZET(bRxDSB0,0)+  	/* 16-bit halfword */                             \
		ZET(bRxDAUTO,1)+	/* Autobuffer mode enabled */        \
		ZET(bRxFLSH,0)+  	/* Normal buffer operation */        \
		ZET(bRxDERE,0)+  	/* Interrupt on error disabled */    \
		ZET(bRxDSB1,0)    	/* 16-bit halfword */  

//..............................................................................

#endif
