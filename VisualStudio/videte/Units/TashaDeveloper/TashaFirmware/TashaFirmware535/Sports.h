#ifndef __SPORT_H__
#define __SPORT_H__

// Bitdefinitionen des SPORTx_RX_CONFIG
#define RSPEN  				0x0001  // RX enable 
#define ICLK   				0x0002  // Internal Clock
#define DTYPE_RJZ			0x0000	// Data Format Right Justify Zero fill
#define DTYPE_RJI			0x0004	// Data Format Right Justify sign extend
#define DTYPE_YLAW			0x0008	// Data Format y-Law
#define DTYPE_ALAW			0x000C	// Data Format A-LAw
#define SENDN				0x0010
#define SLEN3				0x0040	// 3 Bit
#define SLEN4				0x0060	// 4 Bit
#define SLEN5				0x0080	// 5 Bit
#define SLEN6				0x00a0	// 6 Bit
#define SLEN7				0x00c0	// 7 Bit
#define SLEN8				0x00e0	// 8 Bit
#define SLEN9				0x0100	// 9 Bit
#define SLEN10				0x0120	// 10Bit
#define SLEN11				0x0140	// 11Bit
#define SLEN12				0x0160	// 12Bit
#define SLEN13				0x0180	// 13Bit
#define SLEN14				0x01a0	// 14Bit
#define SLEN15				0x01c0	// 15Bit
#define SLEN16				0x01e0	// 16Bit
#define IRFS				0x0200	// Internal Receive Frame Sync Select
#define RFSR				0x0400	// Receive Frame Sync Required Select
#define LRFS				0x1000	// Low receive Frame Sync Select
#define LARFS				0x2000	// Late Receive Frame Sync
#define CKFE				0x4000	// Clock Drive Sample Edge Select
 
// Bitdefinitionen des SPORTx_CONFIG_DMA_RX-Registers
#define	DEN				0x0001
#define TRAN			0x0002
#define DCOME			0x0004
#define SIZE8			0x1008	// Bit 12 und Bit 3 gesetzt
#define SIZE16			0x0000	// Bit 12 und Bit 3 gelöscht
#define SIZE32			0x0008	// Bit 3 gesetzt
#define DAUTO			0x0010
#define FLSH			0x0080
#define DERE			0x0100	// Interrupt on Error

// Bitdefinitionen SPORTx_MC1
#define MCM						0x0001

// Bitdefinitionen SPORTx_MCM2
#define MCCRM_BYPASS			0x0000
#define MCCRM_RECOVER_2_4MHZ	0x0002
#define MCCRM_RECOVER_8_16MHZ	0x0003
#define MCDTXPE					0x0004
#define MCDRXPE					0x0008
#define MCOM					0x0010
#define MCFF_2CHANNELS			0x0000
#define MCFF_4CHANNELS			0x0020
#define MCFF_8CHANNELS			0x0040
#define MCFF_16CHANNELS			0x0060
#define FSDR					0x0080

#define SPORT0					0
#define SPORT1					1

// Mögliche SPORT-Frequenzen
// 66.5 MHz, 33.25 MHz, 22.17 MHz, 16.625 MHz, 13.3 MHz, 11.08 MHz 9.5 MHz
// SPORT0 Einstellungen
#define SPORT0_FREQUENCY			66000000								// 64MHz
#define SPORT0_CHANNELS				(CHANNEL_COUNT/2)						// Anzahl der SPORT0 Kanäle
#define SPORT0_TIME_SLICE_SIZE		4										// Breite eines Kanals in BYTES (1...4)
#define SPORT0_DMA_BUFFER_ADDR		0xff800000								// BufferA im L1
#define SPORT0_DMA_BUFFER_SIZE		(SPORT0_CHANNELS*1024)					// SPORT0 Eingangsbuffer
#define SPORT0_L1_DST_BUFFER_ADDR	0xff900000								// SPORT0 Demultiplex Ausgangsbuffer

// SPORT1 Einstellungen
#define SPORT1_FREQUENCY			66000000								// 64MHz
#define SPORT1_CHANNELS				(CHANNEL_COUNT/2)						// Anzahl der SPORT1 Kanäle
#define SPORT1_TIME_SLICE_SIZE		4										// Breite eines Kanals in BYTES (1...4)
#define SPORT1_DMA_BUFFER_ADDR		(0xff800000+2*SPORT0_DMA_BUFFER_SIZE)	// BufferA im L1
#define SPORT1_DMA_BUFFER_SIZE		(SPORT1_CHANNELS*1024)					// SPORT1 Eingangsbuffer
#define SPORT1_L1_DST_BUFFER_ADDR	(0xff900000+2*SPORT0_DMA_BUFFER_SIZE)		// SPORT1 Demultiplex Ausgangsbuffer

// Framebuffer für Codec 1...8
// Jeder SPORT besitzt zwei 256KByte SDRAM-Buffer im PC.
// Während der MemoryDMA die aktuellen Daten in Buffer A schreibt, muß die
// TashaUnit die Daten aus Buffer B lesen.
#define SDRAM_DMA_BUFFER0_SIZE	(SPORT0_CHANNELS*CHANNEL_SIZE)				// 4*64K=256 KByte
#define SDRAM_DMA_BUFFER1_SIZE	(SPORT1_CHANNELS*CHANNEL_SIZE)				// 4*64K=256 KByte

//######## SPORT ########
void InitSPORTsISR();
void CloseSPORTs();

void EnableSPORTs();
void DisableSPORTs();
void EnableSPORTsDMA();
void DisableSPORTsDMA();

EX_INTERRUPT_HANDLER(SPORTS_RX_ISR);
BOOL HandleSport0RxIRQ(DWORD dwOffset);
BOOL HandleSport1RxIRQ(DWORD dwOffset);

#endif // __SPORT_H__

