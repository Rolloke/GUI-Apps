/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaFirmware535
// FILE:		$Workfile: MemoryDMA.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/TashaFirmware535/MemoryDMA.h $
// CHECKIN:		$Date: 4.03.04 11:48 $
// VERSION:		$Revision: 6 $
// LAST CHANGE:	$Modtime: 4.03.04 11:26 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __MEMORY_DMA_H__
#define __MEMORY_DMA_H__

// 'g_bDMAComplete' wird aus der ISR herraus gesetzt und signalisiert, die Beendigung
// des DMA-Transfers
extern volatile BOOL  g_bDMAComplete;

// Kopiert per MemoryDMA Dateien aus einem Quellbuffer in einen Zielbuffer
// pDest = Pointer auf den Beginn des Quellbuffers
// pSrc  = Pointer auf den Beginn des Zielbuffers
// dwLen = Anzahl der zu übertragenen Bytes.
// bWait   TRUE:  Die Funktion kehrt erst zurück, wenn der Transfer abgeschlossen ist.
// 		   FALSE: Die Funktion kehrt sofort zurück. Die Beendigung des MemoryDMA-Transfers
//                kann mit 'sMemCopyDmaReadyEx()' jederzeit von außen geprüft werden.
// Note: - Es kann jeweils nur ein MemoryDma-Transfer zur Zeit gestartet werden.
//		 - pDest, pSrc, sowie dwLen müssen 32Bit alligned sein.
//	     - dwLen darf maximal 32Mbyte sein, da ansonsten die Desciptorliste mehr als 128 Einträge
//         benötigt.
BOOL MemCopyDma(BYTE* pDest, BYTE* pSrc, DWORD dwLen, BOOL bWait=TRUE);

// Kopiert Daten aus mehreren Quellbuffern in mehrere Zielbuffer
// pDest 	= Pointer auf den Beginn des Quellbuffers
// pSrc  	= Pointer auf den Beginn des Zielbuffers
// dwDstSize= Größe eines Quellbuffers. 
// dwSrcSize= Größe eines Zielbuffers. 
// nChannels= Anzahl der Quell/Zielbuffer
//
// Note: - Es kann jeweils nur ein MemoryDma-Transfer zur Zeit gestartet werden.
//		 - pDest, pSrc, dwDstSize sowie dwSrcSize müssen 32Bit alligned sein.
//	     - dwDstSize und  dwSrcSize darf maximal 256Kbyte groß sein
//		 - dwDestSize muß ein ganzzahlig vielfaches von dwSrcSize sein.
//       - nChannels muß zwischen 1 und 8 liegen.
BOOL MultiChannelCopy(BYTE* pDst, DWORD dwDstSize, BYTE* pSrc, DWORD dwSrcSize, int nChannels, BOOL bWait=TRUE);

// Bindet die ISR ein, die beim Beenden des MemoryDMA-Transfers aufgerufen werden soll.
void InitDmaISR();

// Legt die DescriptorListe für den MemCopyDma-MemoryDMA-Transfer an.
// Parameter siehe bei 'MemCopyDma'
BOOL CreateSingleChannelDescriptorList(BYTE* pDst, BYTE* pSrc, DWORD dwLen);

// Legt die DescriptorListe für den MultiChannelCopy-MemoryDMA-Transfer an.
// Parameter siehe bei 'MultiChannelCopy'
BOOL CreateMultiChannelDescriptorList(BYTE* pDst, DWORD dwDstSize, BYTE* pSrc, DWORD dwSrcSize, int nChannels);

// Aktiviert den MemoryDMA-Transfer
void EnableDma();

// Deaktiviert den MemoryDMA-Transfer
void DisableDma();

// Ist das CompleteBit innerhalb der ISR gesetzt worden?
// Dies funktioniert aber nur, wenn der Interrupt durchkommt. Sollte
// der DMA-Transfer aus einer ISR mit höherer Priorität heraus gestartet
// worden sein, so wird die ISR des MemDMA NICHT aufgerufen! In einem
// solchen Fall muß die 'IsMemCopyDmaReadyEx' verwendet werden, die überprüft
// ob die DMA engine noch akltive ist.
inline BOOL IsMemCopyDmaReady(){return g_bDMAComplete;};

// Ownerchip DMA/CPU?
inline BOOL IsMemCopyDmaReadyEx(){return !TSTBIT(*pMDD_DCFG, 15);};

// Kehr erst zurück wenn kein MemoryDma mehr aktiv ost.
//void WaitUntilMemCopyDmaCompleted();
BOOL WaitUntilMemCopyDmaCompleted(int nTimeOut);

// Iterrupt ausgelöst?
//inline BOOL IsMemCopyDmaReadyEx(){return !TSTBIT(*pMDD_DI, 0);};

// ISR. Diese Routine wird nach erfolgtem MemoryDMA-Transfer aufgerufen, sofern
// dies nicht durch einen Interrupt höherer Priorität verhindert wird.
EX_INTERRUPT_HANDLER(MemoryDmaISR);

#define DCFG_DMA_EN			0x0001	// DMA enable/disable
#define DCFG_DIR			0x0002	// 0=Read, 1=Write
#define DCFG_IOC			0x0004	// Interrupt on Completion enable/disable
#define DCFG_SIZE8			0x1008	// Bit 12 und Bit 3 gesetzt
#define DCFG_SIZE16			0x0000	// Bit 12 und Bit 3 gelöscht
#define DCFG_SIZE32			0x0008	// Bit 3 gesetzt
#define DCFG_CLEAR_BUFF		0x0080
#define DCFG_DCS			0x4000
#define DCFG_DBO			0x8000

// Maximale Anzahl von Descriptoren.
#define MAX_DESCIPTOR_LIST_SIZE 	64

// Anzahl der Bytes, die in einem Rutsch kopiert werden sollen
// (Wird in 'CreateSingleChannelDescriptorList' verwendet)
#define DMA_MEMORY_TRANSFER_SIZE	(0xffff*sizeof(DWORD))
//#define DMA_MEMORY_TRANSFER_SIZE	(1024)


#endif // __MEMORY_DMA_H__
