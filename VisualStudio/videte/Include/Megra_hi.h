/******************************************************************************/
/* Projekt: MEGRA                                                             */
/* Datei:   MEGRA_HI.H                                                        */
/* Zweck:   Headerdatei des Moduls MEGRA_HI.CPP. Sie enthält u.a. sämtliche   */
/*          Registerbezeichner, sowie Prototypen                              */
/******************************************************************************/

/*** Macros zur Vereinfachung der Registeransteuerung ***/
#define LOWBYTE24(w)   ((WORD)(((DWORD)(w))&0xff))
#define MIDBYTE24(w)   ((WORD)(((DWORD)(w)>>8)&0xff))
#define HIBYTE24(w)    ((WORD)(((DWORD)(w)>>16)&0xff))
#define SETBIT(w,b)    ((WORD)((WORD)(w) | (1L << (b))))
#define CLRBIT(w,b)    ((WORD)((WORD)(w) & ~(1L << (b))))
#define TSTBIT(w,b)    ((BOOL)((WORD)(w) & (1L << (b)) ? TRUE : FALSE))
#define Range(a,b,c)   (min(b, max(a, c)))
#define InRange(a,b,c) (c==Range(a, b, c))

/****************************************************/
/* Suchindex: KACKE                                 */
/* neue I2C-Definitionen  22.11.96 Wolfgang         */
/****************************************************/
#define I2C_SCL     0
#define I2C_SDA     1
#define HIGH        1
#define LOW         0
#define I2C_SCLOUT  0x04
#define I2C_SCLHIGH 0x04
#define I2C_SDAOUT  0x08
#define I2C_SDAHIGH 0x08
#define TRISTATE    1
#define REPEAT      10
#define MSBMASK     0x80
/**********************************************************/

#define SAA7111		 0x48
#define SAA7167		 0xBE

#define VRAMWIDTH   1024         // Breite und Höhe des VRAMs
#define VRAMHEIGHT  512          //   "

/*** Mögliche Basisadressen für den VMCplus ***/
#define VMC_BASE1		0x280
#define VMC_BASE2		0x2C0
#define VMC_BASE3		0x300
#define VMC_BASE4		0x340

/*** Typparameter für AccessMemory ***/
#define LOCK			0x0001
#define UNLOCK			0x0002

typedef struct
{
		WORD w7111Reg[12 + 255];		 // 12 Register des SAA7111
		WORD w7167Reg[3  + 255];		 // 3  Register des SAA7167
}REGBUFFER;

//Struktur (auch) für den NT-Treiber
typedef struct
{ 
        WORD    wVMCBase;         // Basisadresse des VMCplus
        WORD    wCODECBase;       // Basisadresse CODEC
		WORD	wVMCId;		      // ID des VideoController.
}vdDriver; //,*PvdDriver;

/*********************************************/ 
/* Register des VMCplus SAA7195A von Philips */
/*********************************************/
typedef enum plagVMC_Reg
{// PC-control
        VMC_Command,            // 0x00 Set misc. modes
        VMC_StatusLow,          // 0x01 Poll status of misc. signals (LOW)
        VMC_StatusHigh,         // 0x02   "                          (HIGH)
        VMC_USERIOctrl,         // 0x03 I2C-Bus Direction, Exter. IO dir        
        VMC_USERIOdata,         // 0x04 I2C-Bus Data, Exter. IO data
        VMC_IrqEna,             // 0x05 Interrupt Enable Reg.
        VMC_ACQUMaskLow=0x08,   // 0x08 Video VRAM Write Mask Register Value (LOW)
        VMC_ACQUMaskMid,        // 0x09   "                                  (MID)
        VMC_ACQUMaskHigh,       // 0x0a   "                                  (HIGH)
        VMC_CPUMaskLow,         // 0x0b CPU VRAM Write Mask Register Value (LOW)
        VMC_CPUMaskMid,         // 0x0c   "                                (MID)
        VMC_CPUMaskHigh,        // 0x0d   "                                (HIGH)
// MemBaseBank 
        VMC_MemBaseBank=0x10,   // 0x10 Memory Base Address and Memory Banking
        VMC_MemIndexLow,        // 0x11 Pointer into memory for IO access (LOW)
        VMC_MemIndexMid,        // 0x12   "                               (MID)
        VMC_MemIndexHigh,       // 0x13   "                               (HIGH)
        VMC_MemIncLow,          // 0x14 Pointer increment (LOW)
        VMC_MemIncHigh,         // 0x15   "               (HIGH)
        VMC_MemData,            // 0x16 Data register for IO access
        VMC_MemCtrl,            // 0x17 Pagesize definition in UMA
        VMC_UMABase,            // 0x18 Memory base definition in UMA
        VMC_UMABank,            // 0x19 Memory bank definition for UMA paging
// Video control
        VMC_ChromaKey=0x26,     // 0x26 Chroma Key settings
        VMC_VLUTIndex,          // 0x27 Pointer to VLUT
        VMC_VLUTDataY,          // 0x28 Y Data Value for VLUT
        VMC_VLUTDataC,          // 0x29 Chroma Data Value for VLUT
        VMC_VLUT,               // 0x2a VLUT Bypass and Test modes
        VMC_Filter,             // 0x2b Filtering modes
        VMC_Matrix,             // 0x2c Matrix modes
        VMC_RFMT,               // 0x2d Reformater modes
        VMC_FMTR,               // 0x2e Formater modes
        VMC_Fifo,               // 0x2f Fifo modes    
// Display control
        VMC_VdAcqMode,          // 0x30 Modes for video acquisition
        VMC_DpMode,             // 0x31 Modes for display
        VMC_DXOffsetLow,        // 0x32 Global Horizontal Offset (LOW) 
        VMC_DXOffsetHigh,       // 0x33   "                      (HIGH)
        VMC_DYOffsetLow,        // 0x34 Global Vertical Offset (LOW)
        VMC_DYOffsetHigh,       // 0x35   "                    (HIGH)
// Keying
        VMC_CKeyMode=0x42,      // 0x42 Mode control for Colour
        VMC_HrefDel,            // 0x43 programmable delay for HREFOUT
        VMC_VidSelDel,          // 0x44 programmable delay for VIDEOSELECT
        VMC_ChromaDel,          // 0x45 programmable delay for CHROMAIN
        VMC_SclkBoost,          // 0x46 programmable delay for zooming
        VMC_CKeyMask,           // 0x47 Mask Value for Colour Keying
        VMC_CKeyMaskHi,         // 0x48 Mask Value for HiColour Keying
        VMC_CKeyComp,           // 0x49 Compare Value for Colour Keying
        VMC_CKeyCompHi,         // 0x4a Compare Value for HiColour Keying
// Odd field Acquisition
        VMC_WrStartAdrOLow=0x60,// 0x60 Start video acquisition from here (LOW)
        VMC_WrStartAdrOMid,     // 0x61   "                               (MID)
        VMC_WrStartAdrOHigh,    // 0x62   "                               (HIGH)
        VMC_WrLineIncOLow,      // 0x63 Line address incr. for video acqu.(LOW)
        VMC_WrLineIncOHigh,     // 0x64   "                               (HIGH)
        VMC_VXStartOLow,        // 0x65 X start of video window (LOW)
        VMC_VXStartOHigh,       // 0x66   "                     (HIGH)
        VMC_VXLengthOLow,       // 0x67 X length of video window (LOW)
        VMC_VXLengthOHigh,      // 0x68   "                      (HIGH)
        VMC_VXScaleOLow,        // 0x69 X scale of video window (LOW)
        VMC_VXScaleOHigh,       // 0x6a   "                     (HIGH)
        VMC_VYStartOLow,        // 0x6b Y start of video window (LOW)
        VMC_VYStartOHigh,       // 0x6c   "                     (HIGH)
        VMC_VYLengthOLow,       // 0x6d Y length of video window (LOW)
        VMC_VYLengthOHigh,      // 0x6e   "                      (HIGH)
        VMC_VYScaleOLow,        // 0x6f Y scale of video window (LOW)
        VMC_VYScaleOHigh,       // 0x70   "                     (HIGH)
// Even field acquisition
        VMC_WrStartAdrELow=0x80,// 0x80 Start video acquisition from here (LOW)
        VMC_WrStartAdrEMid,     // 0x81   "                               (MID)
        VMC_WrStartAdrEHigh,    // 0x82   "                               (HIGH)
        VMC_WrLineIncELow,      // 0x83 Line address incr. for video acqu.(LOW)
        VMC_WrLineIncEHigh,     // 0x84   "                               (HIGH)
        VMC_VXStartELow,   		// 0x85 X start of video window (LOW)
        VMC_VXStartEHigh,       // 0x86   "                     (HIGH)
        VMC_VXLengthELow,       // 0x87 X length of video window (LOW)
        VMC_VXLengthEHigh,      // 0x88   "                      (HIGH)
        VMC_VXScaleELow,        // 0x89 X scale of video window (LOW)
        VMC_VXScaleEHigh,       // 0x8a   "                     (HIGH)
        VMC_VYStartELow,        // 0x8b Y start of video window (LOW)
        VMC_VYStartEHigh,       // 0x8c   "                     (HIGH)
        VMC_VYLengthELow,       // 0x8d Y length of video window (LOW)
        VMC_VYLengthEHigh,      // 0x8e   "                      (HIGH)
        VMC_VYScaleELow,        // 0x8f Y scale of video window (LOW)
        VMC_VYScaleEHigh,       // 0x90   "                     (HIGH)              
// Display control
        VMC_RdStartAdrOLow=0xa0,// 0xa0 Start display readout from here odd (LOW)
        VMC_RdStartAdrOHigh,    // 0xa1   "                                 (HIGH)
        VMC_RdStartAdrELow,     // 0xa2 Start display readout from here eveb (LOW)
        VMC_RdStartAdrEHigh,    // 0xa3   "                                  (HIGH)
        VMC_RdLineIncLow,       // 0xa4 Line addr. incr. for display readout (LOW)
        VMC_RdLineIncHigh,      // 0xa5   "                                  (HIGH)
        VMC_DXStartLow,         // 0xa6 X start of display line (LOW)
        VMC_DXStartHigh,        // 0xa7   "                     (HIGH)
        VMC_DXLengthLow,        // 0xa8 X length of display line (LOW)
        VMC_DXLengthHigh,       // 0xa9   "                      (High)
        VMC_DXZoomLow,          // 0xaa X display zoom factor (LOW)
        VMC_DXZoomHigh,         // 0xab   "                   (HIGH)
        VMC_DYStartLow,         // 0xac Y start of display line (LOW)
        VMC_DYStartHigh,        // 0xad   "                     (HIGH)
        VMC_DYLengthLow,        // 0xae Y length of display line (LOW)
        VMC_DYLengthHigh,       // 0xaf   "                      (High)
        VMC_DYZoomLow,          // 0xb0 Y display zoom factor (LOW)
        VMC_DYZoomHigh,         // 0xb1   "                   (HIGH)  
        
        VMC_DiviLow=0xe0,       // 0xe0 Pixel Clock Divider for PLL (LOW)
        VMC_DiviHigh,           // 0xe1   "                         (HIGH)
        VMC_OESetLow,           // 0xe2 Start Window Fielddetection (LOW)
        VMC_OESetHigh,          // 0xe3   "                         (HIGH)
        VMC_OERestLow,          // 0xe4 Stop Window Fielddetection (LOW)
        VMC_OERestHigh,         // 0xe5   "                        (HIGH)
        VMC_HsOutSetLow,        // 0xe6 HSOUT phase definition (LOW)
        VMC_HsOutSetHigh,       // 0xe7   "                    (HIGH)
        VMC_SyncCtrl,           // 0xe8 polarity of sync
        VMC_AutoStop=0xef,      // 0xef dummy encoding to stop autoincrement
        VMC_LASTREG=0xff      
}VMC_Reg;


/********************************************************/ 
/* Register des VideoFrontend	 SAA7111				*/
/********************************************************/
#define SAA7111_ChipVersion0		0x00 
#define SAA7111_ChipVersion1		0x01 
#define SAA7111_AnalogInputControl1	0x02 
#define SAA7111_AnalogInputControl2	0x03 
#define SAA7111_StaticGainControl1	0x04  
#define SAA7111_StaticGainControl2	0x05 
#define SAA7111_HorizontalSyncBegin	0x06 
#define SAA7111_HorizontalSyncStop  0x07 
#define SAA7111_SyncControl			0x08 
#define SAA7111_LumaControl			0x09 
#define SAA7111_BrightnessControl	0x0a 
#define SAA7111_ContrastControl		0x0b 
#define SAA7111_SaturationControl	0x0c 
#define SAA7111_HueControl			0x0d 
#define SAA7111_ChrominanceControl	0x0e 
#define SAA7111_Reserved1			0x0f 
#define SAA7111_FormatDelayControl	0x10 
#define SAA7111_OutputControl		0x11 
#define SAA7111_OutputMode			0x12

/********************************************************/ 
/* Register des VideoBackend SAA7167					*/
/********************************************************/
#define SAA7167_Reg0				0x00 
#define SAA7167_Reg1				0x01 
#define SAA7167_Reg2				0x02 

/***********************/
/* Prototypen des VDHI */
/***********************/
VOID VC_I2CInit(VOID);
VOID VC_I2CSetLine(WORD wLine, WORD wLineState);
VOID VC_I2CStart(VOID);
VOID VC_I2CStop(VOID);
WORD VC_I2CGetAck(VOID);
VOID VC_I2CSendAck(VOID);
WORD VC_I2CWriteByte(WORD wData);
WORD VC_I2CReadByte(VOID);
WORD WriteToI2C(WORD wSlaveAddr, WORD wSubAddr, WORD wValue);
WORD ReadFromI2C(WORD wSlaveAddr, WORD wSubAddr);
WORD ScanI2C_Device(WORD wSlaveAddr);

VOID   ReadVCMemory(LPRECT Quad, WORD huge* lpBuffer);
VOID   WriteVCMemory(LPRECT Quad, WORD huge* lpBuffer);
VOID   FillVCMemory(LPRECT Quad, WORD nValue1, WORD nValue2);
VOID   FillVCMemoryArray(LPRECT Quad, DWORD *lpVal, WORD wCount);

VOID   WritePort(WORD wAdr, WORD wValue);
WORD   ReadPort(WORD wAdr);
VOID   InitVideo(VOID);
DWORD  GetInstalledRAM(void);
LPSTR  AccessMemory(WORD wType, DWORD dwAddr, DWORD dwLenght);
DWORD  MapPhysicalToLinear(DWORD, DWORD);
WORD   SynthSelector(DWORD, DWORD);
LPSTR  GetSelectorPointer(WORD);
VOID   ReadVCMemory(WORD wType, LPRECT Quad, WORD huge* lpBuffer);
VOID   WriteVCMemory(WORD wType, LPRECT Quad, WORD huge* lpBuffer);
VOID   VCMemory(WORD wType, LPRECT Quad, WORD nValue1, WORD nValue2);

// Export für den Debugger
WORD   __export ReadVCRegister(VMC_Reg nIndex);
VOID   __export WriteVCRegister(VMC_Reg nIndex, WORD wValue);




