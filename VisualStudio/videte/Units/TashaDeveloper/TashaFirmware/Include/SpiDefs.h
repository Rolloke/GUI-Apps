#define DMA_SPI1_TRANSFER_SIZE   	 (0xffff*sizeof(BYTE))
#define MAX_SPI_DESCRIPTOR_LIST_SIZE 8
 

#define SPI_CFG_DEN			0x0001	// DMA enable/disable
#define SPI_CFG_TRAN		0x0002	// 0=Read, 1=Write
#define SPI_CFG_DCOME		0x0004	// Interrupt on Completion enable/disable
#define SPI_CFG_SIZE8		0x1008	// Bit 12 und Bit 3 gesetzt
#define SPI_CFG_SIZE16		0x0000	// Bit 12 und Bit 3 gelöscht
#define SPI_CFG_SIZE32		0x0008	// Bit 3 gesetzt
#define SPI_CFG_DAUTO		0x0010	// 1=Autobuffermode
#define SPI_CFG_CLEAR_BUFF	0x0080	// Buffer Clear
#define SPI_CFG_DERE		0x0100	// Interrupt on Error
#define SPI_CFG_RBSY		0x0200	// Receive Overflow Error
#define SPI_CFG_TXE			0x0400	// Transmit Underrun Error
#define SPI_CFG_MODF		0x0800	// Mode Fault Error
#define SPI_CFG_FS			0x2000	// DMA Buffer Status
#define SPI_CFG_DS			0x4000	// DMA Completion Status
#define SPI_CFG_DOWN		0x8000	// Ownership

// Bitdefinitionen des SPIxStatusRegisters (SPIx_ST)
#define SPIF		0x0001
#define MODF		0x0002
#define TXE			0x0004
#define TXS			0x0008
#define RBSY		0x0010
#define RXS			0x0020
#define TXCOL		0x0040

