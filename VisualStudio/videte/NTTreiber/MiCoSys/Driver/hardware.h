//
// hardware.h
//
// 32-bit Video Capture driver chip register definitions for Mico
//
// AUTHOR:
//       Julian Wolff
//  DATE:
//       09-Feb-1989
//
// REVISIONS:
//
//	X01-00	 JW0799
//		 Initial Version
//
	 
//-----------------------------------------------------------------------------
// Misc Constants
//-----------------------------------------------------------------------------

   
#define DRVR_VERSION    0x1
#define MHZ59           0x30
#define MHZ58           0x22


// VideoQuellen (Müssen von 0 an aufsteigend nummeriert sein!!)
#define	MICO_SOURCE0				0
#define	MICO_SOURCE1				1
#define	MICO_SOURCE2				2
#define	MICO_SOURCE3				3
#define	MICO_SOURCE4				4
#define	MICO_SOURCE5				5
#define	MICO_SOURCE6				6
#define	MICO_SOURCE7				7
#define MICO_MAX_SOURCE				MICO_SOURCE7
#define MICO_SOURCE_NO				255

// Erweiterungskarten  (Müssen von 0 an aufsteigend nummeriert sein!!)
#define MICO_EXTCARD0				0
#define MICO_EXTCARD1				1
#define MICO_EXTCARD2				2
#define MICO_EXTCARD3				3
#define MICO_EXTCARD_MEGRA			4
#define MICO_MAX_EXTCARD			MICO_EXTCARD_MEGRA
#define MICO_EXTCARD_NO				255

#define MICO_MAX_CAM	MICO_MAX_EXTCARD * (MICO_MAX_SOURCE + 1)

//
// Expected interval between vsync interrupts, in microseconds
//
#define PAL_MICROSPERFIELD 		(1000000L/50)	// 20000 us or 20 ms
#define NTSC_MICROSPERFIELD		(1000000L/60)   // 16666 us or 16 ms

//
// Mico Board REgister definitions
//


#define SETBIT(w,b)    ((WORD)((WORD)(w) | (1L << (b))))
#define CLRBIT(w,b)    ((WORD)((WORD)(w) & ~(1L << (b))))
#define TSTBIT(w,b)    ((BOOL)((WORD)(w) & (1L << (b)) ? TRUE : FALSE))

// Definitionen des I2C-Bus-Kontrollers PCF8584
#define	I2C_OWN_ADDRESS_REG		0x00			// Own address is only accessible right
#define	I2C_CONTROL_REG			0x01			// Control Register offset
#define	I2C_STATUS_REG			0x01			// Status Register
#define	I2C_CLOCK_REG			0x02			// I2C Clock Control register
#define	I2C_INT_REG				0x03			// Interrupt Vector Register
#define	I2C_DATA_REG			0x00			// Data	Register usable	after init

#define	I2C_SERIAL_CLOCK_FIELD	0x03			// 2 bit field for serial clock
#define	I2C_SERIAL_CLOCK_90KHZ	0x00			// 90kHz serial	clock
#define	I2C_SERIAL_CLOCK_45KHZ	0x01			// 45kHz Serial	clock
#define	I2C_SERIAL_CLOCK_11KHZ	0x02			// 11kHz Serial	Clock
#define	I2C_SERIAL_CLOCK_1500HZ	0x03			// 1.5kHz Serial Clock

#define	I2C_CLOCK_FIELD			0x1A			// PCD8584 Input Clock
#define	I2C_CLOCK_3MHZ			0x00			// 3MHz	clock attached
#define	I2C_CLOCK_4_43MHZ		0x10			// 4.43MHz clock attached
#define	I2C_CLOCK_6MHZ			0x14			// 6MHz	clock attached
#define	I2C_CLOCK_8MHZ			0x18			// 8MHZ	clock attached
#define	I2C_CLOCK_12MHZ			0x1C			// 12MHZ clock attached

#define	I2C_ESO_BIT				0x40			// Enable Serial Output	bit
#define	I2C_SERIAL_BUS_ON		0x40			// Serial Bus ON
#define	I2C_SERIAL_BUS_OFF		0x00			// SERIAL BUS OFF
#define	I2C_ES1_BIT				0x20			// Extended select bit 1
#define	I2C_ES2_BIT				0x10			// Extended select bit 2
#define	I2C_INT_ENB_BIT			0x08			// Interrupt Enable bit
#define	I2C_START_BIT			0x04			// Start Bit
#define	I2C_STOP_BIT			0x02			// Stop	Bit
#define	I2C_ACK_BIT				0x01			// Int Ack Control Bit

#define	I2C_STS_BIT				0x20			// Slave Stop Detected
#define	I2C_BER_BIT				0x10			// Bus Error Detected
#define	I2C_LRB_BIT				0x08			// Last	Received Bit
#define	I2C_AAS_BIT				0x04			// Addressed as	Slave Bit
#define	I2C_LAB_BIT				0x02			// Lost	Arbitration Bit
#define	I2C_BB_N				0x01			// Bus Busy Not	Bit 
												// 1 = Not Busy
												// 0 = Busy
#define	I2C_PIN_BIT				0x80			// Pending Interrupt Not
												// 1 = No Pending Interrupt
												// 0 = No Interrupt Pending, i.e
												//     the command is completed.
#define	I2C_START_CMD			0x04			// Start Command
#define	I2C_REPEAT_START_CMD	0x04			// Repeat Start	Command
#define	I2C_STOP_CMD			0x02			// Stop	Command
#define	I2C_CHAIN_CMD			0x01			// Data	Chaining Command
#define	I2C_NOP_CMD				0x00			// No Op command

// Bitdefinitionen des BT829 Control-Registers
#define SAT_V_MSB_BIT			0x00
#define SAT_U_MSB_BIT			0x01
#define CON_MSB_BIT				0x02
#define INTERP_BIT				0x03
#define CBSENSE_BIT				0x04
#define LDEC_BIT				0x05
#define COMP_BIT				0x06
#define LNOTCH_BIT				0x07

// Bitdefinitionen des BT829 ADC Interface-Registers
#define C_ADC_BIT				0x01
#define Y_ADC_BIT				0x02

// Slaveadressen des Videodecoders und Videoencoders
#define BT829_SLAVE_ADR			0x8A
#define BT856_SLAVE_ADR			0x88

// Index zur Registerauswahl für die 4 Videomodi
#define NTSC_MODE				0x00
#define PAL_MODE				0x01
#define NTSC_CIF_MODE			0x02
#define PAL_CIF_MODE			0x03

// Bitdefinitionen des BT829 Control-Registers
#define CONTROL_SAT_V_MSB_BIT	0x00
#define CONTROL_SAT_U_MSB_BIT	0x01
#define CONTROL_CON_MSB_BIT		0x02
#define CONTROL_INTERP_BIT		0x03
#define CONTROL_CBSENSE_BIT		0x04
#define CONTROL_LDEC_BIT		0x05
#define CONTROL_COMP_BIT		0x06
#define CONTROL_LNOTCH_BIT		0x07

// Bitdefinitionen des BT829 Status-Registers
#define	STATUS_COF_BIT			0x00
#define	STATUS_LOF_BIT			0x01
#define	STATUS_CCVALID_BIT		0x02
#define	STATUS_CSEL_BIT			0x03
#define	STATUS_NUML_BIT			0x04
#define	STATUS_FIELD_BIT		0x05
#define	STATUS_HLOC_BIT			0x06
#define STATUS_PRES_BIT			0x07

// Bitdefinitionen des BT829 ADC Interface-Registers
#define ADC_C_SLEEP_BIT			0x01
#define ADC_Y_SLEEP_BIT			0x02

// Registerdiffinition des BT829
#define BT829_STATUS			0x00
#define BT829_IFORM				0x01
#define BT829_TDEC				0x02
#define BT829_CROP				0x03
#define BT829_VDELAY_LO			0x04
#define BT829_VACTIVE_LO		0x05
#define BT829_HDELAY_LO			0x06
#define BT829_HACTIVE_LO		0x07
#define BT829_HSCALE_HI			0x08
#define BT829_HSCALE_LO			0x09
#define BT829_BRIGHT			0x0a
#define BT829_CONTROL			0x0b
#define BT829_CONTRAST_LO		0x0c
#define BT829_SAT_U_LO			0x0d
#define BT829_SAT_V_LO			0x0e
#define BT829_HUE				0x0f
#define BT829_OFORM				0x12
#define BT829_VSCALE_HI			0x13
#define BT829_VSCALE_LO			0x14
#define BT829_TEST				0x15
#define BT829_VPOLE				0x16
#define BT829_IDCODE			0x17
#define BT829_ADELAY			0x18
#define BT829_BDELAY			0x19
#define BT829_ADC				0x1a
#define BT829_SRESET			0x1f

// Registerdefinition des BT856
#define BT856_GETID				0x00 // Dieses Register gibt es nicht, es wird nur zu internem Zweck verwendet
#define BT856_RESERVED			0xc0 // 1. Register des BT856
#define BT856_CCDA				0xce
#define BT856_CCDB				0xd0
#define BT856_CCDC				0xd2
#define BT856_CCDD				0xd4
#define BT856_HSYNC_BEG			0xd6
#define BT856_HSYNC_END			0xd8
#define BT856_HSYNC_TIMING		0xda
#define BT856_MODE_CONTROL		0xdc
#define BT856_CAP_MODE_CONTROL	0xde


#define ZR_CB_DATA_OFFSET			0x00
#define ZR_ADDRESS_OFFSET			0x02
#define ZR_DATA_OFFSET				0x04
#define ZR_STATUS0_OFFSET			0x05
#define ZR_LEN0_OFFSET				0x06
#define ZR_LEN1_OFFSET				0x07
#define ZR_COM2_OFFSET				0x08
#define ZR_COM0_OFFSET				0x09
#define ZR_COM1_OFFSET				0x0a
#define ZR_FCNT_OFFSET				0x0b
#define ZR_SB_DATA_OFFSET			0x0c
#define ZR_SIZE_OFFSET				0x0e

// Bitdefinitionen des Status0-Registers
#define STATUS0_PG0_BIT				0x00
#define STATUS0_PG1_BIT				0x01
#define STATUS0_RDY_BIT				0x03
#define STATUS0_SBA_BIT				0x04
#define STATUS0_FNV_BIT				0x05

// Bitdefinitionen des COM1-Registers
#define COM0_PCG0_BIT				0x00
#define COM0_PCG1_BIT				0x01
#define COM0_STILL_BIT				0x02
#define COM0_EXP_BIT				0x03
#define COM0_VID_422_BIT			0x04
#define COM0_GRESET_BIT				0x05
#define COM0_FFI_BIT				0x06
#define COM0_FAST_BIT				0x07


// Bitdefinitionen des COM1-Registers
#define COM1_ACTIVE_BIT				0x00
#define COM1_CB_SIZE_BIT			0x01
#define COM1_VER_DEC_BIT			0x02
#define COM1_HOR_DEC_BIT			0x03
#define COM1_SNC_SRC_BIT			0x04
#define COM1_CLK_EN_BIT				0x05
#define COM1_INT_EN_BIT				0x06
#define COM1_VID_FMT_BIT			0x07

// Bitdefinitionen des COM2-Registers
#define COM2_ID0_BIT				0x00
#define COM2_ID1_BIT				0x01
#define COM2_ID2_BIT				0x02
#define COM2_ID3_BIT				0x03
#define COM2_ID4_BIT				0x04
#define COM2_ID5_BIT				0x05
#define COM2_VID_FILT_BIT			0x06
#define COM2_STRP_SIZE_BIT			0x07


#define RESET_OFFSET				0x10
#define PARAM_OFFSET				0x10
#define CSVINDN_OFFSET				0x11
#define CSVINAN1_OFFSET				0x12
#define CSVINAN2_OFFSET				0x13
#define WATCHDOG_OFFSET				0x14
#define RELAY_OFFSET				0x14
#define ALARM_CUR_STATUS_OFFSET		0x15
#define ALARM_EDGE_SEL_OFFSET		0x15
#define ALARM_STATUS_OFFSET			0x16
#define ALARM_ACK_OFFSET			0x16
#define MICO_ID_OFFSET				0x17
#define MICO_IER_OFFSET				0x18
#define MICO_IRR_OFFSET				0x19
#define MICO_IRQ_EMU_OFFSET			0x19
#define PCF8584_OFFSET	  			0x1a
#define PTC_COUNT0_OFFSET			0x1c
#define PTC_COUNT1_OFFSET			0x1d
#define PTC_COUNT2_OFFSET			0x1e
#define PTC_CONTROL_OFFSET			0x1f

// Bitdefinitionen des CSVDIN-Latches (0x11)
#define SELECT_VD_BIT			0x00
#define SELECT_I2C_BIT			0x01
#define SELECT_MEGRA_BIT		0x02
#define SELECT_SPARE2_BIT		0x03
#define SELECT_SKPNVS_BIT		0x04
#define SELECT_SKPSVS_BIT		0x05
#define SELECT_SPARE3_BIT		0x06
#define SELECT_SPARE4_BIT		0x07

// Bitdefinitionen des Reset und Parameter Register (0x10)
#define PARAM_RESET_BT829_BIT	0x00
#define PARAM_RESET_VIF_BIT		0x01
#define PARAM_RESET_BT856_BIT	0x02
#define PARAM_RESET_PCF8584_BIT 0x03
#define PARAM_RESET_ZR			0x04
#define PARAM_SQUARE_BIT		0x05
#define PARAM_SQUARE2_BIT		0x06
#define PARAM_SPARE1_BIT		0x07

// Bitdefininitionen des Spare/Relay/Watchdog-Registers	(0x14)
#define RELAY0_BIT				0x00
#define RELAY1_BIT				0x01
#define RELAY2_BIT				0x02
#define RELAY3_BIT				0x03
#define WATCHDOG_ENABLE_BIT		        0x04
#define RGBOUT_BIT				0x05
#define SPARE5_BIT				0x06
#define SPARE6_BIT				0x07

/********* INTERRUPT ENABLE REGISTER BITS *******************************/
#define ZR_IE_BIT			        0x03
#define VI_IE_BIT		                0x04
#define VID_AES_BIT				0x05
#define PULLDPWN_BIT			        0x06

/********* INTERRUPT REQUEST REGISTER BITS ******************************/
#define ZR_IR_BIT				0x00
#define ZR_EMU_BIT				0x00
#define VI_IR_BIT				0x01
#define VI_EMU_BIT				0x01
#define FIELD_BIT			        0x02
#define VSYNC_BIT				0x03

// Definitionen des CoVi's
#define ALARM_CUR_STATUS_COVI_OFFSET            0x00
#define ALARM_EDGE_SEL_COVI_OFFSET	        0x00
#define ALARM_STATUS_COVI_OFFSET	        0x01
#define ALARM_ACK_COVI_OFFSET		        0x01
#define CSVINAN_COVI_OFFSET			0x02
#define RELAY_COVI_OFFSET		        0x03


//-----------------------------------------------------------------------------
// Init Ports and size for Toshiba ASIC
// Init Ports are used to program the 'real' I/O Address on the Aviator
//-----------------------------------------------------------------------------
//
// We've got an Address ie. 0x320 to 0x328. Each port 1 byte makes 8 Ports
// to map at programmed I/O Address.
//
#define NUMBER_OF_PORTS		0x1f
				   
//-----------------------------------------------------------------------------
// Memory mapped frame buffer
//-----------------------------------------------------------------------------
#define FRAME_WINDOW_SIZE 	0x100000


//-----------------------------------------------------------------------------
// Specific structures
//-----------------------------------------------------------------------------
typedef struct
{
    WORD	wSource;			// Videoquelle zu den die Bilddaten gehören
    WORD	wExtCard;		        // CoVi zu den die Bilddaten gehören
    DWORD	dwLen;				// Länge der Bilddaten.
} JPEG;

typedef struct
{
	WORD wFeSourceType;
	WORD wFeFilter;
	int nAVPortY;
	int nAVPortC;
} SOURCESTRUCT;

typedef struct
{
	SOURCESTRUCT Source[MICO_MAX_SOURCE+1];
} EXTCARDSTRUCT;

typedef struct
{
	EXTCARDSTRUCT ExtCard[MICO_MAX_EXTCARD+1];
} MICOISASTRUCT;

//-----------------------------------------------------------------------------
// Hardware-specific data structure - stored in device extension and
// accessed by VC_GetHWInfo(pDevInfo)
//-----------------------------------------------------------------------------

typedef struct _HWINFO {

    DWORD   mico_id;
 
    BOOL    bSwitch;					// Switch Camera
    WORD    wCSVDIN;					// Video FE 
    DWORD	dwReqProzessID;				// ProzeßID des Jobs, auf den umgeschaltet werden soll
	WORD    wReqSource;					// Videoeingang	auf den umgeschaltet werden soll
    WORD    wReqExtCard;		        // ExtCard auf die umgeschaltet werden soll
    DWORD	dwProzessID;				// ProzeßID des aktuellen Job
    WORD    wSource;					// Current Video Source
    WORD    wExtCard;					// Current ExtCard
    WORD    wFps;						// wFps=50/fps
    WORD    wSkipFields;		        // Pict's to skip.
	BOOL	bHardwareSkip;				// TRUE -> Hardwareskip ein 
	BOOL	bSwitchDelay;				// TRUE -> 20ms Delay ein
	WORD	wField;						// Legt fest welches Field genommen wird
	WORD	wMinLen;					// Mindestgröße eines Fields in Bytes 
	WORD	wMaxLen;					// Maximalgröße eines Fields in Bytes
    WORD 	wJpegLen; 					// Größe des aktuellen Fields

    DWORD   dwExtCardIO[4];           // ExtCard IO Address
    int     nEncoderState;			// Encoderstatus (On/Off)
    int     nDecoderState;			// Decoderstatus (On/Off)
    WORD	wFormat;					// Bildformat (hoch/niedrig)
    WORD	wFramerate;				// Framerate
    DWORD	dwBPF;					// Sollgröße eines Jpeg Fields in Byte

    WORD	wZRFeVideoFormat;
    WORD	wFeHStart[4];
    WORD	wFeHEnd[4];
    WORD	wFeHTotal[4];
    WORD	wFeVStart[4];
    WORD	wFeVEnd[4];
    WORD	wFeHSStart[4];
    WORD	wFeHSEnd[4];
    WORD	wFeVTotal[4];
    WORD	wFeBlankStart[4];
    WORD	wFeNMCU[4];
    
    WORD	wBeVideoFormat;
    WORD	wBeHStart;
    WORD	wBeHEnd;
    WORD	wBeHTotal;
    WORD	wBeVStart;
    WORD	wBeVEnd;
    WORD	wBeHSStart;
    WORD	wBeHSEnd;
    WORD	wBeVTotal;
    WORD	wBeNMCU;
    WORD	wBeBlankStart;
    
    WORD	wYBackgnd;
    WORD	wUVBackgnd;
    
    DWORD	dwTCVData;
    DWORD	dwTCVNet;
    WORD	wSF;
    
    WORD	wBTFeVideoFormat;
    WORD	wFeSourceType;
    WORD	wFeBrightness;
    WORD	wFeContrast;
    WORD	wFeSaturation;
    WORD	wFeHue;
    WORD	wFeHScale;
    WORD	wFeVScale;
    WORD	wFeFilter;
    
    WORD	wFrontEnd;
    WORD	wRELExtCard;
    WORD	wALRExtCard;
    
    MICOISASTRUCT MiCoISA;
    
    WORD	wBeVideoType;
    DWORD	dwHardwareState;
    BYTE	byIgnoreRelay4;

    BYTE	byEdge;
    BYTE	byAck;


    VIDSTD	VideoStd;		    // current pal/ntsc setting
    VIDEOSOURCE VideoSrc;	    // which connector (Input A,B,C)
    volatile BOOL bVideoIn;	    // are we actually doing video in ?
    DWORD	dwInterruptCount;   // count of ints since stream began
    DWORD	dwZRInterruptCount; // count of ints since stream began
    DWORD	dwWidth, dwHeight;  // size of destination DIB
    CAPTUREFORMAT Format;	    // format of destination DIB
    PVOID	pXlate;				// format-dependent translation table
    DWORD	dwSizeXlate;		// size of Xlate table in bytes
    DWORD	dwVideoTime;		// time (based on frame syncs) since start
    DWORD	dwTimePerInterrupt;	//  - interrupt rate based on NTSC/PAL
    DWORD	dwTimePerFrame;	    // 100-microsecs per frame desired
    DWORD	dwNextFrameTime;
    BOOL	bVideoAtOpen;		// Was Video Present at open? If not
							    // start encoding at first StreamInit
//
// End of Unused
//

} HWINFO, *PHWINFO;

//-----------------------------------------------------------------------------
// Forward Function declarations
//-----------------------------------------------------------------------------
NTSTATUS	VC_GetBus(INTERFACE_TYPE BusType, PDWORD pBusNumber);
BOOLEAN	HW_Close(PDEVICE_INFO pDevInfo);
BOOLEAN	HW_Cleanup(PDEVICE_INFO pDevInfo);
BOOLEAN	HW_DeviceOpen(PDEVICE_INFO pDevInfo);
BOOLEAN	HW_Init(PDEVICE_INFO pDevInfo);
VOID	HW_WrByte(PDEVICE_INFO pDevInfo, WORD addr, UCHAR data, WORD Card);	 
VOID	HW_WrShort(PDEVICE_INFO pDevInfo, WORD addr, WORD data, WORD Card);
UCHAR	HW_RdByte(PDEVICE_INFO pDevInfo, WORD addr, WORD Card);
WORD	HW_RdShort(PDEVICE_INFO pDevInfo, WORD addr, WORD Card);
BYTE	HW_ReadFromLatch(PDEVICE_INFO pDevInfo, WORD port, WORD Card);
BYTE	HW_WriteToLatch(PDEVICE_INFO pDevInfo, WORD port, BYTE data, WORD Card);
BOOLEAN	HW_WriteToI2C(PDEVICE_INFO pDevInfo, BYTE bSubAddress, BYTE bData, BYTE bSlaveAddress);
BYTE	HW_ReadFromI2C(PDEVICE_INFO pDevInfo, BYTE bSubAddress, BYTE bSlaveAddress);

VOID	PCD8584Reset(PDEVICE_INFO pDevInfo);
BOOLEAN	PCD8584Init(PDEVICE_INFO pDevInfo);
BYTE	PCD8584In8(PDEVICE_INFO pDevInfo, BYTE bRegister);
VOID	PCD8584Out8(PDEVICE_INFO pDevInfo, BYTE bRegister, BYTE bData);
WORD	PCD8584WaitUntilPinFree(PDEVICE_INFO pDevInfo);
WORD	PCD8584WaitUntilBusReady(PDEVICE_INFO pDevInfo);
BOOLEAN	PCD8584TestAck(PDEVICE_INFO pDevInfo);
VOID	PCD8584Wait(int nCount);

VOID	BT856Reset(PDEVICE_INFO pDevInfo);
BOOL	BT856Init(PDEVICE_INFO pDevInfo);
BOOL	BT856Out(PDEVICE_INFO pDevInfo, BYTE bSubAddress, BYTE bData);
BYTE	BT856In(PDEVICE_INFO pDevInfo, BYTE bSubAddress);
WORD	BT856SetBeVideoFormat(PDEVICE_INFO pDevInfo, WORD wBeVideoFormat);
WORD	BT856GetBeVideoFormat(PDEVICE_INFO pDevInfo);

BOOL	BT829Out(PDEVICE_INFO pDevInfo, BYTE bSubAddress, BYTE bData, DWORD FE_Number);
BYTE	BT829In(PDEVICE_INFO pDevInfo, BYTE bSubAddress, DWORD FE_Number);
VOID	BT829Reset(PDEVICE_INFO pDevInfo);
VOID	BT829Init(PDEVICE_INFO pDevInfo, DWORD FE_Number);
BOOL	BT829SetFeVideoFormat(PDEVICE_INFO pDevInfo, WORD wFeVideoFormat, DWORD FE_Number);
WORD	BT829GetFeVideoFormat(PDEVICE_INFO pDevInfo);
BOOL	BT829SetFeSourceType(PDEVICE_INFO pDevInfo, WORD wFeSourceType, WORD wFeFilter, DWORD FE_Number);
WORD	BT829GetFeSourceType(PDEVICE_INFO pDevInfo);
BOOL	BT829SetFeBrightness(PDEVICE_INFO pDevInfo, WORD wFeBrightness, DWORD FE_Number);
WORD	BT829GetFeBrightness(PDEVICE_INFO pDevInfo);
BOOL	BT829SetFeContrast(PDEVICE_INFO pDevInfo, WORD wFeContrast, DWORD FE_Number);
WORD	BT829GetFeContrast(PDEVICE_INFO pDevInfo);
BOOL	BT829SetFeSaturation(PDEVICE_INFO pDevInfo, WORD wFeSaturation, DWORD FE_Number);
BOOL	BT829IsVideoPresent(PDEVICE_INFO pDevInfo, DWORD FE_Number);
WORD	BT829GetFeSaturation(PDEVICE_INFO pDevInfo);
BOOL	BT829SetFeHue(PDEVICE_INFO pDevInfo, WORD wFeHue, DWORD FE_Number);
WORD	BT829GetFeHue(PDEVICE_INFO pDevInfo);
WORD	BT829SetFeHScale(PDEVICE_INFO pDevInfo, WORD wFeHScale, DWORD FE_Number);
WORD	BT829GetFeHScale(PDEVICE_INFO pDevInfo);
WORD	BT829SetFeVScale(PDEVICE_INFO pDevInfo, WORD wFeVScale, DWORD FE_Number);
WORD	BT829GetFeVScale(PDEVICE_INFO pDevInfo);
WORD	BT829SetFeFilter(PDEVICE_INFO pDevInfo, WORD wFeFilter, DWORD FE_Number);
WORD	BT829GetFeFilter(PDEVICE_INFO pDevInfo);

BOOL	ZRXReset(PDEVICE_INFO pDevInfo);
BOOL	ZRXInit(PDEVICE_INFO pDevInfo);
void	ZRSetInputSource(PDEVICE_INFO pDevInfo, WORD wFrontEnd, WORD wExtCard, WORD wSource, BYTE byYPort, BYTE byCPort, DWORD dwProzessID);
VOID	ZRSetFeVideoFormat(PDEVICE_INFO pDevInfo, WORD wFormat);
VOID	ZRSetBeVideoFormat(PDEVICE_INFO pDevInfo, WORD wFormat);
VOID	ZRSetFormat(PDEVICE_INFO pDevInfo, WORD wFormat);
VOID	ZRSetFramerate(PDEVICE_INFO pDevInfo, WORD wFramerate);
VOID	ZRSetBPF(PDEVICE_INFO pDevInfo, DWORD dwBPF);
BOOL	ZRStartEncoding(PDEVICE_INFO pDevInfo);
BOOL	ZRRestartEncoding(PDEVICE_INFO pDevInfo);
BOOL	ZRStopEncoding(PDEVICE_INFO pDevInfo);
BYTE	ZRGetEncoderState(PDEVICE_INFO pDevInfo);
DWORD	ZRWriteData(PDEVICE_INFO pDevInfo, LPCSTR pData, DWORD dwDataLen);
DWORD	ZRMakeSOFMarker(PDEVICE_INFO pDevInfo, LP_Z050_CODER_CONFIG Cfg);
DWORD	ZRMakeSOS1Marker(PDEVICE_INFO pDevInfo);
DWORD	ZRMakeDRIMarker(PDEVICE_INFO pDevInfo, LP_Z050_CODER_CONFIG Cfg);
DWORD	ZRMakeQTable(PDEVICE_INFO pDevInfo, char FAR* QTable);
DWORD	ZRMakeHuffTable(PDEVICE_INFO pDevInfo, char FAR* HuffTable);
DWORD	ZRMakeAPPString(PDEVICE_INFO pDevInfo, char FAR* APPString);
DWORD	ZRMakeCOMString(PDEVICE_INFO pDevInfo, char FAR* COMString);
void	ZRMakeMarkerSegments(PDEVICE_INFO pDevInfo, LP_Z050_CODER_CONFIG Cfg);
void	ZRCoderStart(PDEVICE_INFO pDevInfo);
int		ZRCoderConfigureCompression(PDEVICE_INFO pDevInfo, LP_Z050_CODER_CONFIG Cfg);
int		ZRCoderConfigureExpansion(PDEVICE_INFO pDevInfo, LP_Z050_CODER_CONFIG Cfg);
WORD	ZRCoderGetLastScaleFactor(PDEVICE_INFO pDevInfo);
BYTE	ZRRead050(PDEVICE_INFO pDevInfo, WORD wAddress);
void	ZRWrite050(PDEVICE_INFO pDevInfo, WORD wAdr, BYTE bData);
BOOL	ZRCheck050End(PDEVICE_INFO pDevInfo);
VOID	ZRChipSelectVideoInAnalogOut(PDEVICE_INFO pDevInfo, WORD wFrontEnd, WORD wExtCard, BYTE bData);
BYTE	ZRChipSelectVideoInAnalogIn(PDEVICE_INFO pDevInfo, WORD wFrontEnd, WORD wExtCard);
VOID	ZRChipSelectVideoInDigitalOut(PDEVICE_INFO pDevInfo, BYTE bData);
BYTE	ZRChipSelectVideoInDigitalIn(PDEVICE_INFO pDevInfo);

void	RELSetRelay(PDEVICE_INFO pDevInfo, BYTE bRelais);
void	RELClrRelay(PDEVICE_INFO pDevInfo, BYTE bRelais);
BYTE	RELGetRelay(PDEVICE_INFO pDevInfo);

BYTE	ALARMGetCurrentState(PDEVICE_INFO pDevInfo);
BYTE	ALARMGetState(PDEVICE_INFO pDevInfo);
void	ALARMSetEdge(PDEVICE_INFO pDevInfo, BYTE byEdge);
void	ALARMSetAck(PDEVICE_INFO pDevInfo, BYTE byAck);
BYTE	ALARMGetAck(PDEVICE_INFO pDevInfo);
BYTE	ALARMGetEdge(PDEVICE_INFO pDevInfo);


WORD	MICOGetVersion(PDEVICE_INFO pDevInfo);
BOOL	MICOIsVideoPresent(PDEVICE_INFO pDevInfo);
DWORD   MICOSetInputSource(PDEVICE_INFO pDevInfo, WORD wExtCard, WORD wSource, WORD wFormat, DWORD dwBPF, DWORD dwProzessID);
void	MICOGetInputSource(PDEVICE_INFO pDevInfo, WORD wExtCard, WORD wSource);
DWORD	MICOGetHardwareState(PDEVICE_INFO pDevInfo);
WORD	MICOSetFeSourceType(PDEVICE_INFO pDevInfo, WORD wSourceType);
WORD	MICOGetFeSourceType(PDEVICE_INFO pDevInfo);
WORD	MICOSetFeVideoFormat(PDEVICE_INFO pDevInfo, WORD wFeVideoFormat);
WORD	MICOGetFeVideoFormat(PDEVICE_INFO pDevInfo);
WORD	MICOSetBeVideoFormat(PDEVICE_INFO pDevInfo, WORD wBeVideoFormat);
WORD	MICOGetBeVideoFormat(PDEVICE_INFO pDevInfo);
WORD	MICOSetBeVideoType(PDEVICE_INFO pDevInfo, WORD wBeVideoType);
WORD	MICOGetBeVideoType(PDEVICE_INFO pDevInfo);
WORD	MICOSetFeBrightness(PDEVICE_INFO pDevInfo, WORD wBrightness);
WORD	MICOGetFeBrightness(PDEVICE_INFO pDevInfo);
WORD	MICOSetFeContrast(PDEVICE_INFO pDevInfo, WORD wContrast);
WORD	MICOGetFeContrast(PDEVICE_INFO pDevInfo);
WORD	MICOSetFeSaturation(PDEVICE_INFO pDevInfo, WORD wSaturation);
WORD	MICOGetFeSaturation(PDEVICE_INFO pDevInfo);
WORD	MICOSetFeHue(PDEVICE_INFO pDevInfo, WORD wHue);
WORD	MICOGetFeHue(PDEVICE_INFO pDevInfo);
WORD	MICOSetFeHScale(PDEVICE_INFO pDevInfo, WORD wHScale);
WORD	MICOGetFeHScale(PDEVICE_INFO pDevInfo);
WORD	MICOSetFeVScale(PDEVICE_INFO pDevInfo, WORD wVScale);
WORD	MICOGetFeVScale(PDEVICE_INFO pDevInfo);
WORD	MICOSetFeFilter(PDEVICE_INFO pDevInfo, WORD wFilter);
WORD	MICOGetFeFilter(PDEVICE_INFO pDevInfo);
BOOL	MICOEncoderStart(PDEVICE_INFO pDevInfo);
BOOL	MICOEncoderStop(PDEVICE_INFO pDevInfo);
WORD	MICOEncoderSetFormat(PDEVICE_INFO pDevInfo, WORD wFormat);
WORD	MICOEncoderGetFormat(PDEVICE_INFO pDevInfo);
WORD	MICOEncoderSetFramerate(PDEVICE_INFO pDevInfo, WORD wFramerate);
WORD	MICOEncoderGetFramerate(PDEVICE_INFO pDevInfo);
DWORD	MICOEncoderSetBPF(PDEVICE_INFO pDevInfo, DWORD dwBPF);
DWORD	MICOEncoderGetBPF(PDEVICE_INFO pDevInfo);
BOOL	MICOEncoderSetMode(PDEVICE_INFO pDevInfo, WORD wFormat, WORD wFramerate, DWORD dwBPF);
int		MICOGetEncoderState(PDEVICE_INFO pDevInfo);
BOOL	MICOCheckSource(PDEVICE_INFO pDevInfo, WORD wSource);
BOOL	MICOCheckFeID(PDEVICE_INFO pDevInfo, WORD wFeID);
BOOL	MICOCheckExtCard(PDEVICE_INFO pDevInfo, WORD wExtCard);
BOOL	MICOCheckSourceType(PDEVICE_INFO pDevInfo, WORD wSourceType);
BOOL	MICOCheckFeVideoFormat(PDEVICE_INFO pDevInfo, WORD wFeVideoFormat);
BOOL	MICOCheckBeVideoFormat(PDEVICE_INFO pDevInfo, WORD wBeVideoFormat);
BOOL	MICOCheckBeVideoType(PDEVICE_INFO pDevInfo, WORD wBeVideoType);
BOOL	MICOCheckFeFilter(PDEVICE_INFO pDevInfo, WORD wFilter);
BOOL	MICOCheckEncoderFormat(PDEVICE_INFO pDevInfo, WORD wFormat);
BOOL	MICOCheckEncoderFramerate(PDEVICE_INFO pDevInfo, WORD wFramerate);
BOOL	MICOCheckEncoderBPF(PDEVICE_INFO pDevInfo, DWORD dwBPF);
BYTE	MICOSetRelais(PDEVICE_INFO pDevInfo, WORD wExtCard, BYTE bRelais);
BYTE	MICOGetRelais(PDEVICE_INFO pDevInfo, WORD wExtCard);
VOID	MICOLoadSources(PDEVICE_INFO pDevInfo);
VOID	MICOSaveRegistry(PDEVICE_INFO pDevInfo);
BOOLEAN MICOParameter(PDEVICE_INFO);

BOOLEAN HW_MapPort(PDEVICE_INFO pDevInfo, WORD PortBase, DWORD NrOfPorts);
BOOLEAN HW_TestInterrupts(PDEVICE_INFO pDevInfo, PHWINFO pHw);
VOID	HW_SetIRQ(PDEVICE_INFO pDevInfo, PHWINFO pHw, DWORD Irq);
VOID	HW_EnableInts(PDEVICE_INFO pDevInfo, PHWINFO pHw);
VOID	HW_DisableInts(PDEVICE_INFO pDevInfo, PHWINFO pHw);
VOID	HW_EnableZRInts(PDEVICE_INFO pDevInfo, PHWINFO pHw);
VOID	HW_DisableZRInts(PDEVICE_INFO pDevInfo, PHWINFO pHw);
BOOLEAN HW_WatchDog(PDEVICE_INFO pDevInfo, PCONFIG_INFO pGeneric);
DWORD	HW_Relais(PDEVICE_INFO pDevInfo, PCONFIG_INFO pGeneric);
DWORD	HW_Alarm(PDEVICE_INFO pDevInfo, PCONFIG_INFO pGeneric);

BOOLEAN HW_InterruptAck(PDEVICE_INFO pDevInfo);
BOOLEAN HW_ZRIAck(PDEVICE_INFO pDevInfo, WORD wField, BOOL *bDoDPC);
BOOLEAN HW_ConfigSource(PDEVICE_INFO pDevInfo, PCONFIG_INFO pGeneric);
BOOLEAN HW_ConfigFormat(PDEVICE_INFO pDevInfo, PCONFIG_INFO pGeneric);
BOOLEAN HW_Capture(PDEVICE_INFO pDevInfo, BOOL bCapture);
BOOLEAN HW_StreamInit(PDEVICE_INFO pDevInfo, DWORD microsecperframe);
BOOLEAN HW_StreamFini(PDEVICE_INFO pDevInfo);
BOOLEAN HW_StreamStart(PDEVICE_INFO pDevInfo);
BOOLEAN HW_StreamStop(PDEVICE_INFO pDevInfo);
DWORD	HW_StreamGetPosition(PDEVICE_INFO pDevInfo);
DWORD	HW_CaptureService(PDEVICE_INFO pDevInfo,PUCHAR pBuffer,PDWORD pTimeStamp,DWORD BufferLength);

void	ServiceSourceSelection(PDEVICE_INFO pDevInfo);
void	SetBPF(PDEVICE_INFO pDevInfo, DWORD dwBPF);


