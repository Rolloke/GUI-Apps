/*******************************************************************************
 *
 * $Source$
 * $Revision: 1 $
 *
 * 21535 memory definitions file.
 *
 * $Log: /Project/Units/Tasha(Developer)/TashaFirmware/BlackFin/include/m21535.h $
 * 
 * 1     5.01.04 9:53 Martin.lueck
 * 
 * 1     31.07.03 14:32 Martin.lueck
 * Wichtige Header und asm files für den BF535
 *
 ******************************************************************************/

#ifndef _M21535_H_
#define _M21535_H_

#include "zet.h"

//------------------------------------------------------------------------------
// Flag pins 
//------------------------------------------------------------------------------

#define bPF0		0
#define bPF1		1
#define bPF2		2
#define bPF3		3
#define bPF4		4
#define bPF5		5
#define bPF6		6
#define bPF7		7
#define bPF8		8
#define bPF9		9
#define bPF10		10
#define bPF11		11
#define bPF12		12
#define bPF13		13
#define bPF14		14

//------------------------------------------------------------------------------
// System 
//------------------------------------------------------------------------------

// NOTE: CHIPID is defined to the wrong value in <def21535.h> use chipId 
//       instead

#define chipId                   0xFFC048C0      // Chip ID Register

// DSPID
#define DSPID                    0xFFE05000      // DSP Device ID Register

// SIC_IMASK

#define bSIC_IMASKPCIInt 3
#define bSPORT0RxDMAInt  4

//------------------------------------------------------------------------------
// PCI 
//------------------------------------------------------------------------------

#define PCI_CONFIG_DATA_PORT    0xEEFFFFFC
#define PCI_MEMORY_DATA_WINDOW  0xE0000000
#define PCI_IO_DATA_WINDOW      0xEEFE0000

// PCI_CTL

#define bPCI_HostDevice           0
#define bPCI_PCIEn                1
#define bPCI_FastBackToBackEn     2
#define bPCI_IntAToPCIEn          3
#define bPCI_IntAToPCI            4
#define bPCI_RSTToPCIEn           5
#define bPCI_RSTToPCI             6

#define kPCI_HostDevice           ZET(bPCI_HostDevice,1)
#define kPCI_PCIEn                ZET(bPCI_PCIEn,1)
#define kPCI_FastBackToBackEn     ZET(bPCI_FastBackToBackEn,1)
#define kPCI_IntAToPCIEn          ZET(bPCI_IntAToPCIEn,1)
#define kPCI_IntAToPCI            ZET(bPCI_IntAToPCI,1)
#define kPCI_RSTToPCIEn           ZET(bPCI_RSTToPCIEn,1)
#define kPCI_RSTToPCI             ZET(bPCI_RSTToPCI,1)

// PCI_STAT

#define bPCI_INTA                 0
#define bPCI_INTB                 1
#define bPCI_INTC                 2
#define bPCI_INTD                 3
#define bPCI_ParityError          4
#define bPCI_FatalError           5
#define bPCI_Reset                6
#define bPCI_MasterTxFifoEmpty    7
#define bPCI_MasterTxFifoFull     8
#define bPCI_MasterQueueFull      9
#define bPCI_MemoryWriteInval     10
#define bPCI_ErrorOnInboundRd     11
#define bPCI_ErrorOnInboundWr     12
#define bPCI_UnsuppEABAccess      13
#define bPCI_Serr                 14
#define bPCI_SlaveRxFifoGotData   30
#define bPCI_SlaveTxFifoGotData   31

#define kPCI_INTA                 ZET(bPCI_INTA,1)
#define kPCI_INTB                 ZET(bPCI_INTB,1)
#define kPCI_INTC                 ZET(bPCI_INTC,1)
#define kPCI_INTD                 ZET(bPCI_INTD,1)

#define kPCI_FatalError           ZET(bPCI_FatalError,1)

// PCI_ICTL

#define bPCI_INTAEn               0
#define bPCI_INTBEn               1
#define bPCI_INTCEn               2
#define bPCI_INTDEn               3
#define bPCI_ParityErrorEn        4
#define bPCI_FatalErrorEn         5
#define bPCI_ResetEn              6
#define bPCI_MasterTxFifoFullEn   7
#define bPCI_MemoryWriteInvalEn   10
#define bPCI_ErrorOnInboundRdEn   11
#define bPCI_ErrorOnInboundWrEn   12
#define bPCI_UnsuppEABAccessEn    13
#define bPCI_SerrEn               14
#define bPCI_SlaveRxFifoGotDataEn 30
#define bPCI_SlaveTxFifoGotDataEn 31

#define kPCI_INTAEn                ZET(bPCI_INTAEn,1)
#define kPCI_INTBEn                ZET(bPCI_INTBEn,1)
#define kPCI_INTCEn                ZET(bPCI_INTCEn,1)
#define kPCI_INTDEn                ZET(bPCI_INTDEn,1)

// PCI_CFG_CMD
#define bPCI_CFG_IOSpaceEn         0
#define bPCI_CFG_MemorySpaceEn     1
#define bPCI_CFG_BusMasterEn       2
#define bPCI_CFG_SpecialCyclesEn   3
#define bPCI_CFG_MemWriteInvalEn   4
#define bPCI_CFG_VGASnoopEn        5
#define bPCI_CFG_ParityErrorEn     6
#define bPCI_CFG_SteppingEn        7
#define bPCI_CFG_SerrEn            8
#define bPCI_CFG_FastBackToBackEn  9

// PCI_HMCTL

#define bPCI_HMCTL_MMREn           0
#define bPCI_HMCTL_L2En            1
#define bPCI_HMCTL_AsyncEn         2
#define bPCI_HMCTL_AsyncSize       3
#define bPCI_HMCTL_SDRAMEn         5
#define bPCI_HMCTL_SDRAMSize       6

//------------------------------------------------------------------------------
// USB 
//------------------------------------------------------------------------------

// USBD_CTRL

#define bUSBD_CtrlEna     0

#define kUSBD_CtrlEna     ZET(bUSBD_CtrlEna,1)

// USBD_EPBUF

#define bUSBD_EpbufRdy	  14
#define	bUSBD_EpbufCfg	  15

#define kUSBD_EpbufRdy    ZET(bUSBD_EpbufRdy,1)
#define kUSBD_EpbufCfg    ZET(bUSBD_EpbufCfg,1)

// USBD_GINTR

#define	bUSBD_GintrSof		0
#define	bUSBD_GintrCfg		1
#define	bUSBD_GintrMsof		2
#define	bUSBD_GintrRst		3
#define	bUSBD_GintrSusp		4
#define	bUSBD_GintrResume	5
#define	bUSBD_GintrFrmmat	6
#define	bUSBD_GintrDmairq	7
#define	bUSBD_GintrEP0Int	8
#define	bUSBD_GintrEP1Int	9
#define	bUSBD_GintrEP2Int	10
#define	bUSBD_GintrEP3Int	11
#define	bUSBD_GintrEP4Int	12
#define	bUSBD_GintrEP5Int	13
#define	bUSBD_GintrEP6Int	14
#define	bUSBD_GintrEP7Int	15

#define	kUSBD_GintrSof		ZET(bUSBD_GintrSof,1)
#define	kUSBD_GintrCfg		ZET(bUSBD_GintrCfg,1)
#define	kUSBD_GintrMsof		ZET(bUSBD_GintrMsof,1)
#define	kUSBD_GintrRst		ZET(bUSBD_GintrRst,1)
#define	kUSBD_GintrSusp		ZET(bUSBD_GintrSusp,1)
#define	kUSBD_GintrResume	ZET(bUSBD_GintrResume,1)
#define	kUSBD_GintrFrmmat	ZET(bUSBD_GintrFrmmat,1)
#define	kUSBD_GintrDmairq	ZET(bUSBD_GintrDmairq,1)
#define	kUSBD_GintrEP0Int	ZET(bUSBD_GintrEP0Int,1)
#define	kUSBD_GintrEP1Int	ZET(bUSBD_GintrEP1Int,1)
#define	kUSBD_GintrEP2Int	ZET(bUSBD_GintrEP2Int,1)
#define	kUSBD_GintrEP3Int	ZET(bUSBD_GintrEP3Int,1)
#define	kUSBD_GintrEP4Int	ZET(bUSBD_GintrEP4Int,1)
#define	kUSBD_GintrEP5Int	ZET(bUSBD_GintrEP5Int,1)
#define	kUSBD_GintrEP6Int	ZET(bUSBD_GintrEP6Int,1)
#define	kUSBD_GintrEP7Int	ZET(bUSBD_GintrEP7Int,1)

// USBD_GMASK

#define	bUSBD_GmaskSofm		0
#define	bUSBD_GmaskCfgm		1
#define	bUSBD_GmaskMsofm	2
#define	bUSBD_GmaskRstm		3
#define	bUSBD_GmaskSuspm	4
#define	bUSBD_GmaskResumem	5
#define	bUSBD_GmaskFrmatm	6
#define	bUSBD_GmaskDmairqm	7
#define	bUSBD_GmaskEP0Msk	8
#define	bUSBD_GmaskEP1Msk	9
#define	bUSBD_GmaskEP2Msk	10
#define	bUSBD_GmaskEP3Msk	11
#define	bUSBD_GmaskEP4Msk	12
#define	bUSBD_GmaskEP5Msk	13
#define	bUSBD_GmaskEP6Msk	14
#define	bUSBD_GmaskEP7Msk	15

#define	kUSBD_GmaskSofm		ZET(bUSBD_GmaskSofm,1)
#define	kUSBD_GmaskCfgm		ZET(bUSBD_GmaskCfgm,1)
#define	kUSBD_GmaskMsofm	ZET(bUSBD_GmaskMsofm,1)
#define	kUSBD_GmaskRstm		ZET(bUSBD_GmaskRstm,1)
#define	kUSBD_GmaskSuspm	ZET(bUSBD_GmaskSuspm,1)
#define	kUSBD_GmaskResumem	ZET(bUSBD_GmaskResumem,1)
#define	kUSBD_GmaskFrmatm	ZET(bUSBD_GmaskFrmatm,1)
#define	kUSBD_GmaskDmairqm	ZET(bUSBD_GmaskDmairqm,1)
#define	kUSBD_GmaskEP0Msk	ZET(bUSBD_GmaskEP0Msk,1)
#define	kUSBD_GmaskEP1Msk	ZET(bUSBD_GmaskEP1Msk,1)
#define	kUSBD_GmaskEP2Msk	ZET(bUSBD_GmaskEP2Msk,1)
#define	kUSBD_GmaskEP3Msk	ZET(bUSBD_GmaskEP3Msk,1)
#define	kUSBD_GmaskEP4Msk	ZET(bUSBD_GmaskEP4Msk,1)
#define	kUSBD_GmaskEP5Msk	ZET(bUSBD_GmaskEP5Msk,1)
#define	kUSBD_GmaskEP6Msk	ZET(bUSBD_GmaskEP6Msk,1)
#define	kUSBD_GmaskEP7Msk	ZET(bUSBD_GmaskEP7Msk,1)

// USBD_EPCFGx

#define	bUSBD_EpcfgArm	        0
#define	bUSBD_EpcfgDir	        1
#define	bUSBD_EpcfgTyp	        2
#define	bUSBD_EpcfgMax	        4

#define	kUSBD_EpcfgArm          ZET(bUSBD_EpcfgArm,1) 
#define	kUSBD_EpcfgDir          ZET(bUSBD_EpcfgDir,1) 

// USBD_INTRx

#define	bUSBD_IntrTc		0
#define	bUSBD_IntrPc		1
#define	bUSBD_IntrBcstat	2
#define	bUSBD_IntrSetup		3
#define	bUSBD_IntrMsetup	4
#define	bUSBD_IntrMerr		5

#define	kUSBD_IntrTc		ZET(bUSBD_IntrTc,1)
#define	kUSBD_IntrPc		ZET(bUSBD_IntrPc,1)
#define	kUSBD_IntrBcstat	ZET(bUSBD_IntrBcstat,1)
#define	kUSBD_IntrSetup		ZET(bUSBD_IntrSetup,1)
#define	kUSBD_IntrMsetup	ZET(bUSBD_IntrMsetup,1)
#define	kUSBD_IntrMerr		ZET(bUSBD_IntrMerr,1)

// USBD_MASKx

#define	bUSBD_MaskTc		0
#define	bUSBD_MaskPc		1
#define	bUSBD_MaskBcstat	2
#define	bUSBD_MaskSetup		3
#define	bUSBD_MaskMsetup	4
#define	bUSBD_MaskMerr		5

#define	kUSBD_MaskTc		ZET(bUSBD_MaskTc,1)
#define	kUSBD_MaskPc		ZET(bUSBD_MaskPc,1)
#define	kUSBD_MaskBcstat	ZET(bUSBD_MaskBcstat,1)
#define	kUSBD_MaskSetup		ZET(bUSBD_MaskSetup,1)
#define	kUSBD_MaskMsetup	ZET(bUSBD_MaskMsetup,1)
#define	kUSBD_MaskMerr		ZET(bUSBD_MaskMerr,1)

// USBD_DMACFG

#define	bUSBD_DmaCfg_DmaEn	0
#define	bUSBD_DmaCfg_Ioc	2
#define	bUSBD_DmaCfg_DmaBc	7
#define	bUSBD_DmaCfg_DmaBs	12

#define	kUSBD_DmaCfg_DmaEn	ZET(bUSBD_DmaCfg_DmaEn,1)
#define	kUSBD_DmaCfg_Ioc	ZET(bUSBD_DmaCfg_Ioc,1)
#define	kUSBD_DmaCfg_DmaBc	ZET(bUSBD_DmaCfg_DmaBc,1)
#define	kUSBD_DmaCfg_DmaBs	ZET(bUSBD_DmaCfg_DmaBs,1)

//------------------------------------------------------------------------------
// RTC 
//------------------------------------------------------------------------------

// RTC_ISTAT

#define bStopwatchEvt 	0
#define bAlarmEvt 	1
#define bSecondsEvt 	2
#define bMinutesEvt 	3
#define b24HoursEvt 	4
#define bDayAlarmEvt 	5
#define bWriteComplete 	15

#define kStopwatchEvt     ZET(bStopwatchEvt,1) 	   
#define kAlarmEvt 	  ZET(bAlarmEvt,1)         
#define kSecondsEvt 	  ZET(bSecondsEvt,1)       
#define kMinutesEvt 	  ZET(bMinutesEvt,1)       
#define k24HoursEvt 	  ZET(b24HoursEvt,1)       
#define kDayAlarmEvt 	  ZET(bDayAlarmEvt,1)      
#define kWriteCompleteEvt ZET(bWriteComplete,1)    

// RTCICTL

#define bSecondsIntEn 	 2

//------------------------------------------------------------------------------
// UART 
//------------------------------------------------------------------------------

// UARTx_LCR

#define bUART_WLS  0
#define bUART_STB  2
#define bUART_PEN  3
#define bUART_EPS  4
#define bUART_SP   5
#define bUART_SB   6
#define bUART_DLAB 7

#define kUART_DLAB ZET(bUART_DLAB,1)   

// UARTx_LSR

#define bUART_DR   0
#define bUART_OE   1
#define bUART_PE   2
#define bUART_FE   3
#define bUART_BI   4
#define bUART_THRE 5
#define bUART_TEMT 6

#define kUART_DR   ZET(bUART_DR,1) 
#define kUART_OE   ZET(bUART_OE,1) 
#define kUART_PE   ZET(bUART_PE,1) 
#define kUART_FE   ZET(bUART_FE,1) 
#define kUART_BI   ZET(bUART_BI,1) 
#define kUART_THRE ZET(bUART_THRE,1) 
#define kUART_TEMT ZET(bUART_TEMT,1) 

//------------------------------------------------------------------------------
// EBIU 
//------------------------------------------------------------------------------

// EBIU_AMGCTL

#define bEBIU_AMCKEN 0
#define bEBIU_AMBEN  1
#define bEBIU_B0PEN  4
#define bEBIU_B1PEN  5
#define bEBIU_B2PEN  6
#define bEBIU_B3PEN  7

// EBIU_AMBCTL0
#define bEBIU_B0RDYEN   0
#define bEBIU_B0RDYPOL  1
#define bEBIU_B0TT      2
#define bEBIU_B0ST      4
#define bEBIU_B0HT      6
#define bEBIU_B0RAT     8
#define bEBIU_B0WAT    12
#define bEBIU_B1RDYEN  16
#define bEBIU_B1RDYPOL 17
#define bEBIU_B1TT     18
#define bEBIU_B1ST     20
#define bEBIU_B1HT     22
#define bEBIU_B1RAT    24
#define bEBIU_B1WAT    28

// EBIU_AMBCTL1
#define bEBIU_B2RDYEN   0
#define bEBIU_B2RDYPOL  1
#define bEBIU_B2TT      2
#define bEBIU_B2ST      4
#define bEBIU_B2HT      6
#define bEBIU_B2RAT     8
#define bEBIU_B2WAT    12
#define bEBIU_B3RDYEN  16
#define bEBIU_B3RDYPOL 17
#define bEBIU_B3TT     18
#define bEBIU_B3ST     20
#define bEBIU_B3HT     22
#define bEBIU_B3RAT    24
#define bEBIU_B3WAT    28


// EBIU_SDRRC

#define bEBIU_RDIV 0

// EBIU_SDBCTL

#define bEBIU_EB0E   0
#define bEBIU_EB0SZ  1
#define bEBIU_EB0CAW 4
#define bEBIU_EB1E   8
#define bEBIU_EB1SZ  9
#define bEBIU_EB1CAW 12
#define bEBIU_EB2E   16
#define bEBIU_EB2SZ  17
#define bEBIU_EB2CAW 20
#define bEBIU_EB3E   24
#define bEBIU_EB3SZ  25
#define bEBIU_EB3CAW 28

// EBIU_SDGCTL

#define bEBIU_SCTLE 0
#define bEBIU_SCK1E 1
#define bEBIU_CL    2
#define bEBIU_PFE   4
#define bEBIU_PFP   5
#define bEBIU_TRAS  6
#define bEBIU_TRP   11
#define bEBIU_TRCD  15
#define bEBIU_TWR   19
#define bEBIU_PSM   22
#define bEBIU_PSSE  23
#define bEBIU_SRFS  24
#define bEBIU_EBUFE 25
#define bEBIU_X16DE 31

//------------------------------------------------------------------------------
// SPORTx 
//------------------------------------------------------------------------------

// SPORTx_TX_CONFIG 

#define	bTxTSPEN 0
#define	bTxICLK  1
#define	bTxDTYPE 2
#define	bTxSENDN 4
#define	bTxSLEN  5
#define	bTxITFS  9
#define	bTxTFSR  10
#define	bTxDITFS 11
#define	bTxLTFS  12
#define	bTxLATFS 13
#define	bTxCKFE  14
#define	bTxICLKD 15

//..............................................................................

// SPORTx_RX_CONFIG 

#define	bRxRSPEN 0
#define	bRxICLK  1
#define	bRxDTYPE 2
#define	bRxSENDN 4
#define	bRxSLEN  5
#define	bRxIRFS  9
#define	bRxRFSR  10
#define	bRxLRFS  12
#define	bRxLARFS 13
#define	bRxCKFE  14
#define	bRxICLKD 15

//..............................................................................
// SPORTx_STAT
#define bROVF 0
#define bRXS 1
#define bTXS 2
#define bTUVF 3
#define bCHNL 4

//..............................................................................
// SPORTx_IRQSTAT_RX

#define bRxCOMPL  0 
#define bRxERR    1
#define bRxBUSERR 2

// SPORTx_IRQSTAT_TX

#define bTxCOMPL  0 
#define bTxERR    1
#define bTxBUSERR 2
 
// SPORTx_MCMC1

#define bMCM   0
#define bMFD   1
#define bWSIZE 5
#define bWOFF  9

// SPORTx_MCMC2

#define bMCCRM   0
#define bMCDTXPE 2
#define bMCDRXPE 3
#define bMCOM    4
#define bMCFF    5
#define bFSDR    7

// SPORTx_CONFIG_DMA_TX

#define bTxDEN   0
#define bTxTRAN  1
#define bTxDCOME 2
#define bTxDSB0  3
#define bTxDAUTO 4
#define bTxFLSH  7
#define bTxDERE  8
#define bTxTUVF  9
#define bTxDSB1  12
#define bTxFS    12
#define bTxDS    14
#define bTxDOWN  15

#define kTxDAUTO   ZET(bTxDAUTO,1) 

// SPORTx_CONFIG_DMA_RX

#define bRxDEN   0
#define bRxTRAN  1
#define bRxDCOME 2
#define bRxDSB0  3
#define bRxDAUTO 4
#define bRxFLSH  7
#define bRxDERE  8
#define bRxROVF  9
#define bRxDSB1  12
#define bRxFS    12
#define bRxDS    14
#define bRxDOWN  15

#endif
