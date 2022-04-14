//
//
// 32-bit Video Capture driver
//
// hardware-specific data structures for Mico.
//
//
// This header describes the structures that are shared between
// the hardware-specific user-mode code and the hardware-specific
// kernel mode code.
//
// The vckernel library does not define the format of the
// config structures passed to the config functions beyond the fact that
// the first DWORD contains the length of the entire structure. Note that
// any pointers within this struct will need special handling.
//
// Julian Wolff
//

#ifndef _MICO_H_
#define _MICO_H_

#pragma pack(8)
//
// BEGIN OF *TMP* - Remove of DLL is no longer needed.....
//
//
// Possible video standards
//
typedef enum _VIDSTD {
    NTSC = 0,
    PAL,
    MAXVIDEOSTD
} VIDSTD, * PVIDSTD;

//
// Possible capture formats
//
typedef enum _CAPTUREFORMAT {
    FmtInvalid = 0,		// default fmt is 'not set yet'
    FmtYUYV,			// YUV 4:1:1
    FmtPal8,                    // Palettized 8-bit
    FmtRT21,                    // RBG Indeo V2.1
    FmtIV31,                    // RBG Indeo V3.1
    FmtYVU9                     // YUV 4:2:2
} CAPTUREFORMAT;

#ifndef FOURCC_YUV
#define FOURCC_YUV 	mmioFOURCC('Y','4','1','1')
#endif

#ifndef FOURCC_YUV9
#define FOURCC_YUV9	mmioFOURCC('S','4','2','2')
#endif

#ifndef FOURCC_IV31
#define FOURCC_IV31     mmioFOURCC('i','v','3','1')
#endif

#ifndef FOURCC_RT21
#define FOURCC_RT21     mmioFOURCC('R','T','2','1')
#endif

//
// Possible source types
//
typedef enum _VIDEOSOURCE {
    Input_A = 0,
    Input_B,
    Input_C
} VIDEOSOURCE, * PVIDEOSOURCE;

//
// END OF *TMP* - Remove of DLL is no longer needed.....
//

//
// This structure is passed to the ConfigFormat function. it
// defines the size and format of the destination bitmap.
//
// note that VC_AccessData will need to be called to access this table safely
// from kernel mode.
//
typedef struct _CONFIG_FORMAT {
    DWORD ulSize;
    DWORD ulBPF;
    DWORD ulFPS;
    DWORD ulResolution;
    DWORD ulFeVideoFormat;
    DWORD ulBeVideoFormat;
    DWORD ulBeVideoType;
// *TMP*
    CAPTUREFORMAT Format;
    DWORD ulWidth;
    DWORD ulHeight;
    PVOID pXlate;
    DWORD ulSizeXlate;
// *TMP*
			 
} CONFIG_FORMAT, *PCONFIG_FORMAT;


//
// This structure is passed to the ConfigSource function. it defines the
// source video selector, the source video standard and adjustable variables
// that control the source acquisition.
//
typedef struct _CONFIG_SOURCE {
    DWORD ulSize;
    DWORD ulHue;      		// acquisition hue adjustment 0..ff
    DWORD ulBright;
    DWORD ulContrast;
    DWORD ulSat;
    DWORD ulInpSource;
    DWORD ulExtCard;
	DWORD ulProzessID;
// *TMP* DLL used.
    BOOL bFilter;
    VIDSTD VideoStd;	 	// pal/ntsc type of source
    VIDEOSOURCE VideoSrc;	// which connector (A,B or C)
// *TMP*

} CONFIG_SOURCE, *PCONFIG_SOURCE;

//
// WatchDog
//

//
// Functions
//
#define WDOG_GO         0
#define WDOG_RESET      1
#define WDOG_WRITE      2

typedef struct _CONFIG_WDOG {
    DWORD ulSize;
    UCHAR ubFunction;
    DWORD ulParameter;
} CONFIG_WDOG, *PCONFIG_WDOG;


//
// Relais
//

//
// Functions
//
#define WR_RELAIS_1    0
#define WR_RELAIS_2    1
#define WR_RELAIS_3    2
#define WR_RELAIS_4    3

#define RD_RELAIS_1    4
#define RD_RELAIS_2    5
#define RD_RELAIS_3    6
#define RD_RELAIS_4    7

typedef struct _CONFIG_RELAIS {
    DWORD ulSize;
    UCHAR ubFunction;
    DWORD ulExtCard;
    DWORD ulState;
} CONFIG_RELAIS, *PCONFIG_RELAIS;

//
// Alarm
//

//
// Functions
//
#define WR_ALARM        0

#define RD_ALARM        1

#define ALARM_EDGE      0
#define ALARM_ACK       1
#define ALARM_STATE     2
#define ALARM_CUR_STATE 3

typedef struct _CONFIG_ALARM {
    DWORD ulSize;
    UCHAR ubFunction;
    DWORD ulExtCard;
    DWORD ulState;
    DWORD ulStateType;
} CONFIG_ALARM, *PCONFIG_ALARM;

//
// Constants for Format etc.
//
// Video Formate (muß in dieser Reihenfolge bleiben)
#define MICO_NTSC_SQUARE			0
#define MICO_PAL_SQUARE	    		1
#define MICO_NTSC_CCIR				2
#define MICO_PAL_CCIR				3

// Art des Ausgangssignals
#define MICO_COMPOSITE				1
#define MICO_RGB            	   	2

// Mögliche Bildformate
#define MICO_ENCODER_LOW			0
#define MICO_ENCODER_HIGH			1
#define MICO_ENCODER_NO_FORMAT		0xffff

//
// size of xlate table should be 32k bytes
//
#define PAL_TO_RGB555_SIZE	(32 * 1024)
#define MAX_HUE		 63
#define DEFAULT_HUE	 0
#define MAX_BRIGHT	 255
#define DEFAULT_BRIGHT   180
#define MAX_CONTRAST     255
#define DEFAULT_CONTRAST 90
#define MAX_SAT		 255
#define DEFAULT_SAT      90
#define MAX_FILTER       1 
#define DEFAULT_FILTER   0

//
// Default settings for port, interrupt and frame buffer
//
#define DEF_PORT 	0x200
#define DEF_INTERRUPT	5
#define DEF_FRAME       0x8000000               // Map at 128MB
//
// Parameter Names:
//
// These are the names of Values in the Parameters key (or driver section
// of the profile) used for communicating configuration information and errors
// between the kernel and user-mode drivers.
//
#define PARAM_PORT	  L"Base Port"	        // port i/o address
#define PARAM_INTERRUPT	  L"Base Interrupt"		// interrupt number
#define PARAM_ERROR	  L"Base InstallError"	// config error/success code (below)
#define PARAM_DIAGS       L"Base Last Diagnostic"    // Last Diagnostic passed
#define PARAM_FAIL        L"Base Last Failure"       // Last Diagnostic not passed
#define PARAM_VERSION     L"Base HardwareVersion"
#define PARAM_SVERSION    L"Base Driver Version"
#define PARAM_FRAME       L"Base FrameAddress"       // Virtual Frame Address

// [MICOISA]
#define PARAM_TMO         L"MicoISA TimeOut"            // Default = 1000
#define PARAM_EXC1        L"MicoISA ExtCard1IOBase"
#define PARAM_EXC2        L"MicoISA ExtCard2IOBase"
#define PARAM_EXC3        L"MicoISA ExtCard3IOBase"
#define PARAM_BPF         L"MicoISA BytesPerFrame"      // Default = 20000
#define PARAM_FPS         L"MicoISA FramesPerSecond"    // Default = 25
#define PARAM_IGNREL4     L"MicoISA IgnoreRelais4"      // Default = 0 (don't ignore)
#define PARAM_SKP         L"MicoISA SkipFields"         // Default = 2
#define PARAM_RES         L"MicoISA Resolution"         // Default = 1 (=High, 0=Low)
#define PARAM_CAM_TYPE    L"MicoISA CameraType"			// Default = 0         
#define PARAM_HRD_SKP     L"MicoISA HardwareSkip"       // Default = 1  
#define PARAM_SWITCH_DELAY L"MicoISA SwitchDelay"       // Default = 1 
#define PARAM_STORE_FIELD L"MicoISA StoreField"         // Default = EVEN_FIELD
#define PARAM_MIN_SIZE    L"MicoISA MinJpegSize"        // Default = 80 
#define PARAM_MAX_SIZE    L"MicoISA MaxJpegSize"        // Default = 140 

// [BackEnd]
#define PARAM_BE_VF       L"Video BE VideoFormat"        // Default = PAL_CCIR
#define PARAM_BE_VT       L"Video BE VideoType"          // Default = COMPOSITE

// [FrontEnd]
#define PARAM_FE_VF       L"Video FE VideoFormat"        // Default = PAL_CCIR
#define PARAM_FE_BR       L"Video FE Brightness"         // Default = 137
#define PARAM_FE_SAT      L"Video FE Saturation"         // Default = 290
#define PARAM_FE_CONT     L"Video FE Contrast"           // Default = 285
#define PARAM_FE_HUE      L"Video FE Hue"                // Default = 128

// [Source0-0]
#define PARAM_S00_AVPY    L"Source 0-0 AVPortY"           // Default = 0
#define PARAM_S00_AVPC    L"Source 0-0 AVPortC"           // Default = 0
#define PARAM_S00_ST      L"Source 0-0 SourceType"         // Default = FBAS
#define PARAM_S00_NF      L"Source 0-0 NotchFilter"        // Default = Auto

// [Source0-1]
#define PARAM_S01_AVPY    L"Source 0-1 AVPortY"           // Default = 1
#define PARAM_S01_AVPC    L"Source 0-1 AVPortC"           // Default = 1
#define PARAM_S01_ST      L"Source 0-1 SourceType"         // Default = FBAS
#define PARAM_S01_NF      L"Source 0-1 NotchFilter"        // Default = Auto

// [Source0-2]
#define PARAM_S02_AVPY    L"Source 0-2 AVPortY"           // Default = 0
#define PARAM_S02_AVPC    L"Source 0-2 AVPortC"           // Default = 0
#define PARAM_S02_ST      L"Source 0-2 SourceType"         // Default = FBAS
#define PARAM_S02_NF      L"Source 0-2 NotchFilter"        // Default = Auto

// [Source0-3]
#define PARAM_S03_AVPY    L"Source 0-3 AVPortY"           // Default = 1
#define PARAM_S03_AVPC    L"Source 0-3 AVPortC"           // Default = 1
#define PARAM_S03_ST      L"Source 0-3 SourceType"         // Default = FBAS
#define PARAM_S03_NF      L"Source 0-3 NotchFilter"        // Default = Auto

// [Source0-4]
#define PARAM_S04_AVPY    L"Source 0-4 AVPortY"           // Default = 0
#define PARAM_S04_AVPC    L"Source 0-4 AVPortC"           // Default = 0
#define PARAM_S04_ST      L"Source 0-4 SourceType"         // Default = FBAS
#define PARAM_S04_NF      L"Source 0-4 NotchFilter"        // Default = Auto

// [Source0-5]
#define PARAM_S05_AVPY    L"Source 0-5 AVPortY"           // Default = 1
#define PARAM_S05_AVPC    L"Source 0-5 AVPortC"           // Default = 1
#define PARAM_S05_ST      L"Source 0-5 SourceType"         // Default = FBAS
#define PARAM_S05_NF      L"Source 0-5 NotchFilter"        // Default = Auto

// [Source0-6]
#define PARAM_S06_AVPY    L"Source 0-6 AVPortY"           // Default = 0
#define PARAM_S06_AVPC    L"Source 0-6 AVPortC"           // Default = 0
#define PARAM_S06_ST      L"Source 0-6 SourceType"         // Default = FBAS
#define PARAM_S06_NF      L"Source 0-6 NotchFilter"        // Default = Auto

// [Source0-7]
#define PARAM_S07_AVPY    L"Source 0-7 AVPortY"           // Default = 1
#define PARAM_S07_AVPC    L"Source 0-7 AVPortC"           // Default = 1
#define PARAM_S07_ST      L"Source 0-7 SourceType"         // Default = FBAS
#define PARAM_S07_NF      L"Source 0-7 NotchFilter"        // Default = Auto

// [Source1-0]
#define PARAM_S10_AVPY    L"Source 1-0 AVPortY"           // Default = 0
#define PARAM_S10_AVPC    L"Source 1-0 AVPortC"           // Default = 0
#define PARAM_S10_ST      L"Source 1-0 SourceType"         // Default = FBAS
#define PARAM_S10_NF      L"Source 1-0 NotchFilter"        // Default = Auto

// [Source1-1]
#define PARAM_S11_AVPY    L"Source 1-1 AVPortY"           // Default = 1
#define PARAM_S11_AVPC    L"Source 1-1 AVPortC"           // Default = 1
#define PARAM_S11_ST      L"Source 1-1 SourceType"         // Default = FBAS
#define PARAM_S11_NF      L"Source 1-1 NotchFilter"        // Default = Auto

// [Source1-2]
#define PARAM_S12_AVPY    L"Source 1-2 AVPortY"           // Default = 0
#define PARAM_S12_AVPC    L"Source 1-2 AVPortC"           // Default = 0
#define PARAM_S12_ST      L"Source 1-2 SourceType"         // Default = FBAS
#define PARAM_S12_NF      L"Source 1-2 NotchFilter"        // Default = Auto

// [Source1-3]
#define PARAM_S13_AVPY    L"Source 1-3 AVPortY"           // Default = 1
#define PARAM_S13_AVPC    L"Source 1-3 AVPortC"           // Default = 1
#define PARAM_S13_ST      L"Source 1-3 SourceType"         // Default = FBAS
#define PARAM_S13_NF      L"Source 1-3 NotchFilter"        // Default = Auto

// [Source1-4]
#define PARAM_S14_AVPY    L"Source 1-4 AVPortY"           // Default = 0
#define PARAM_S14_AVPC    L"Source 1-4 AVPortC"           // Default = 0
#define PARAM_S14_ST      L"Source 1-4 SourceType"         // Default = FBAS
#define PARAM_S14_NF      L"Source 1-4 NotchFilter"        // Default = Auto

// [Source1-5]
#define PARAM_S15_AVPY    L"Source 1-5 AVPortY"           // Default = 1
#define PARAM_S15_AVPC    L"Source 1-5 AVPortC"           // Default = 1
#define PARAM_S15_ST      L"Source 1-5 SourceType"         // Default = FBAS
#define PARAM_S15_NF      L"Source 1-5 NotchFilter"        // Default = Auto

// [Source1-6]
#define PARAM_S16_AVPY    L"Source 1-6 AVPortY"           // Default = 0
#define PARAM_S16_AVPC    L"Source 1-6 AVPortC"           // Default = 0
#define PARAM_S16_ST      L"Source 1-6 SourceType"         // Default = FBAS
#define PARAM_S16_NF      L"Source 1-6 NotchFilter"        // Default = Auto

// [Source1-7]
#define PARAM_S17_AVPY    L"Source 1-7 AVPortY"           // Default = 1
#define PARAM_S17_AVPC    L"Source 1-7 AVPortC"           // Default = 1
#define PARAM_S17_ST      L"Source 1-7 SourceType"         // Default = FBAS
#define PARAM_S17_NF      L"Source 1-7 NotchFilter"        // Default = Auto

// [Source2-0]
#define PARAM_S20_AVPY    L"Source 2-0 AVPortY"           // Default = 0
#define PARAM_S20_AVPC    L"Source 2-0 AVPortC"           // Default = 0
#define PARAM_S20_ST      L"Source 2-0 SourceType"         // Default = FBAS
#define PARAM_S20_NF      L"Source 2-0 NotchFilter"        // Default = Auto

// [Source2-1]
#define PARAM_S21_AVPY    L"Source 2-1 AVPortY"           // Default = 1
#define PARAM_S21_AVPC    L"Source 2-1 AVPortC"           // Default = 1
#define PARAM_S21_ST      L"Source 2-1 SourceType"         // Default = FBAS
#define PARAM_S21_NF      L"Source 2-1 NotchFilter"        // Default = Auto

// [Source2-2]
#define PARAM_S22_AVPY    L"Source 2-2 AVPortY"           // Default = 0
#define PARAM_S22_AVPC    L"Source 2-2 AVPortC"           // Default = 0
#define PARAM_S22_ST      L"Source 2-2 SourceType"         // Default = FBAS
#define PARAM_S22_NF      L"Source 2-2 NotchFilter"        // Default = Auto

// [Source2-3]
#define PARAM_S23_AVPY    L"Source 2-3 AVPortY"           // Default = 1
#define PARAM_S23_AVPC    L"Source 2-3 AVPortC"           // Default = 1
#define PARAM_S23_ST      L"Source 2-3 SourceType"         // Default = FBAS
#define PARAM_S23_NF      L"Source 2-3 NotchFilter"        // Default = Auto

// [Source2-4]
#define PARAM_S24_AVPY    L"Source 2-4 AVPortY"           // Default = 0
#define PARAM_S24_AVPC    L"Source 2-4 AVPortC"           // Default = 0
#define PARAM_S24_ST      L"Source 2-4 SourceType"         // Default = FBAS
#define PARAM_S24_NF      L"Source 2-4 NotchFilter"        // Default = Auto

// [Source2-5]
#define PARAM_S25_AVPY    L"Source 2-5 AVPortY"           // Default = 1
#define PARAM_S25_AVPC    L"Source 2-5 AVPortC"           // Default = 1
#define PARAM_S25_ST      L"Source 2-5 SourceType"         // Default = FBAS
#define PARAM_S25_NF      L"Source 2-5 NotchFilter"        // Default = Auto

// [Source2-6]
#define PARAM_S26_AVPY    L"Source 2-6 AVPortY"           // Default = 0
#define PARAM_S26_AVPC    L"Source 2-6 AVPortC"           // Default = 0
#define PARAM_S26_ST      L"Source 2-6 SourceType"         // Default = FBAS
#define PARAM_S26_NF      L"Source 2-6 NotchFilter"        // Default = Auto

// [Source2-7]
#define PARAM_S27_AVPY    L"Source 2-7 AVPortY"           // Default = 1
#define PARAM_S27_AVPC    L"Source 2-7 AVPortC"           // Default = 1
#define PARAM_S27_ST      L"Source 2-7 SourceType"         // Default = FBAS
#define PARAM_S27_NF      L"Source 2-7 NotchFilter"        // Default = Auto

// [Source3-0]
#define PARAM_S30_AVPY    L"Source 3-0 AVPortY"           // Default = 0
#define PARAM_S30_AVPC    L"Source 3-0 AVPortC"           // Default = 0
#define PARAM_S30_ST      L"Source 3-0 SourceType"         // Default = FBAS
#define PARAM_S30_NF      L"Source 3-0 NotchFilter"        // Default = Auto

// [Source3-1]
#define PARAM_S31_AVPY    L"Source 3-1 AVPortY"           // Default = 1
#define PARAM_S31_AVPC    L"Source 3-1 AVPortC"           // Default = 1
#define PARAM_S31_ST      L"Source 3-1 SourceType"         // Default = FBAS
#define PARAM_S31_NF      L"Source 3-1 NotchFilter"        // Default = Auto

// [Source3-2]
#define PARAM_S32_AVPY    L"Source 3-2 AVPortY"           // Default = 0
#define PARAM_S32_AVPC    L"Source 3-2 AVPortC"           // Default = 0
#define PARAM_S32_ST      L"Source 3-2 SourceType"         // Default = FBAS
#define PARAM_S32_NF      L"Source 3-2 NotchFilter"        // Default = Auto

// [Source3-3]
#define PARAM_S33_AVPY    L"Source 3-3 AVPortY"           // Default = 1
#define PARAM_S33_AVPC    L"Source 3-3 AVPortC"           // Default = 1
#define PARAM_S33_ST      L"Source 3-3 SourceType"         // Default = FBAS
#define PARAM_S33_NF      L"Source 3-3 NotchFilter"        // Default = Auto

// [Source3-4]
#define PARAM_S34_AVPY    L"Source 3-4 AVPortY"           // Default = 0
#define PARAM_S34_AVPC    L"Source 3-4 AVPortC"           // Default = 0
#define PARAM_S34_ST      L"Source 3-4 SourceType"         // Default = FBAS
#define PARAM_S34_NF      L"Source 3-4 NotchFilter"        // Default = Auto

// [Source3-5]
#define PARAM_S35_AVPY    L"Source 3-5 AVPortY"           // Default = 1
#define PARAM_S35_AVPC    L"Source 3-5 AVPortC"           // Default = 1
#define PARAM_S35_ST      L"Source 3-5 SourceType"         // Default = FBAS
#define PARAM_S35_NF      L"Source 3-5 NotchFilter"        // Default = Auto

// [Source3-6]
#define PARAM_S36_AVPY    L"Source 3-6 AVPortY"           // Default = 0
#define PARAM_S36_AVPC    L"Source 3-6 AVPortC"           // Default = 0
#define PARAM_S36_ST      L"Source 3-6 SourceType"         // Default = FBAS
#define PARAM_S36_NF      L"Source 3-6 NotchFilter"        // Default = Auto

// [Source3-7]
#define PARAM_S37_AVPY    L"Source 3-7 AVPortY"           // Default = 1
#define PARAM_S37_AVPC    L"Source 3-7 AVPortC"           // Default = 1
#define PARAM_S37_ST      L"Source 3-7 SourceType"         // Default = FBAS
#define PARAM_S37_NF      L"Source 3-7 NotchFilter"        // Default = Auto

// [Timing_PAL_CCIR]
#define PARAM_T58PC_FeHStart     L"Timing 58Mhz PAL_CCIR FeHStart"        // Default = 8
#define PARAM_T58PC_FeHEnd       L"Timing 58Mhz PAL_CCIR FeHEnd"          // Default = 712
#define PARAM_T58PC_FeHTotal     L"Timing 58Mhz PAL_CCIR FeHTotal"        // Default = 864
#define PARAM_T58PC_FeHSStart    L"Timing 58Mhz PAL_CCIR FeHSStart"       // Default = 732
#define PARAM_T58PC_FeHSEnd      L"Timing 58Mhz PAL_CCIR FeHSEnd"         // Default = 784
#define PARAM_T58PC_FeBlankStart L"Timing 58Mhz PAL_CCIR FeBlankStart"    // Default = 720
#define PARAM_T58PC_FeVStart     L"Timing 58Mhz PAL_CCIR FeVStart"        // Default = 16
#define PARAM_T58PC_FeVEnd       L"Timing 58Mhz PAL_CCIR FeVEnd"          // Default = 304
#define PARAM_T58PC_FeVTotal     L"Timing 58Mhz PAL_CCIR FeVTotal"        // Default = 625
#define PARAM_T58PC_FeNMCU       L"Timing 58Mhz PAL_CCIR FeNMCU"          // Default = 43

// [Timing_PAL_SQUARE]
#define PARAM_T58PS_FeHStart     L"Timing 58Mhz PAL_SQUARE FeHStart"        // Default = 8
#define PARAM_T58PS_FeHEnd       L"Timing 58Mhz PAL_SQUARE FeHEnd"          // Default = 744
#define PARAM_T58PS_FeHTotal     L"Timing 58Mhz PAL_SQUARE FeHTotal"        // Default = 928
#define PARAM_T58PS_FeHSStart    L"Timing 58Mhz PAL_SQUARE FeHSStart"       // Default = 766
#define PARAM_T58PS_FeHSEnd      L"Timing 58Mhz PAL_SQUARE FeHSEnd"         // Default = 820
#define PARAM_T58PS_FeBlankStart L"Timing 58Mhz PAL_SQUARE FeBlankStart"    // Default = 752
#define PARAM_T58PS_FeVStart     L"Timing 58Mhz PAL_SQUARE FeVStart"        // Default = 17
#define PARAM_T58PS_FeVEnd       L"Timing 58Mhz PAL_SQUARE FeVEnd"          // Default = 305
#define PARAM_T58PS_FeVTotal     L"Timing 58Mhz PAL_SQUARE FeVTotal"        // Default = 625
#define PARAM_T58PS_FeNMCU       L"Timing 58Mhz PAL_SQUARE FeNMCU"          // Default = 45

// [Timing_NTSC_CCIR]
#define PARAM_T58NC_FeHStart     L"Timing 58Mhz NTSC_CCIR FeHStart"        // Default = 8
#define PARAM_T58NC_FeHEnd       L"Timing 58Mhz NTSC_CCIR FeHEnd"          // Default = 712
#define PARAM_T58NC_FeHTotal     L"Timing 58Mhz NTSC_CCIR FeHTotal"        // Default = 858
#define PARAM_T58NC_FeHSStart    L"Timing 58Mhz NTSC_CCIR FeHSStart"       // Default = 736
#define PARAM_T58NC_FeHSEnd      L"Timing 58Mhz NTSC_CCIR FeHSEnd"         // Default = 784
#define PARAM_T58NC_FeBlankStart L"Timing 58Mhz NTSC_CCIR FeBlankStart"    // Default = 720
#define PARAM_T58NC_FeVStart     L"Timing 58Mhz NTSC_CCIR FeVStart"        // Default = 14
#define PARAM_T58NC_FeVEnd       L"Timing 58Mhz NTSC_CCIR FeVEnd"          // Default = 254
#define PARAM_T58NC_FeVTotal     L"Timing 58Mhz NTSC_CCIR FeVTotal"        // Default = 525
#define PARAM_T58NC_FeNMCU       L"Timing 58Mhz NTSC_CCIR FeNMCU"          // Default = 43

// [Timing_NTSC_SQUARE]
#define PARAM_T58NS_FeHStart     L"Timing 58Mhz NTSC_SQUARE FeHStart"        // Default = 0
#define PARAM_T58NS_FeHEnd       L"Timing 58Mhz NTSC_SQUARE FeHEnd"          // Default = 640
#define PARAM_T58NS_FeHTotal     L"Timing 58Mhz NTSC_SQUARE FeHTotal"        // Default = 780
#define PARAM_T58NS_FeHSStart    L"Timing 58Mhz NTSC_SQUARE FeHSStart"       // Default = 658
#define PARAM_T58NS_FeHSEnd      L"Timing 58Mhz NTSC_SQUARE FeHSEnd"         // Default = 698
#define PARAM_T58NS_FeBlankStart L"Timing 58Mhz NTSC_SQUARE FeBlankStart"    // Default = 640
#define PARAM_T58NS_FeVStart     L"Timing 58Mhz NTSC_SQUARE FeVStart"        // Default = 14
#define PARAM_T58NS_FeVEnd       L"Timing 58Mhz NTSC_SQUARE FeVEnd"          // Default = 254
#define PARAM_T58NS_FeVTotal     L"Timing 58Mhz NTSC_SQUARE FeVTotal"        // Default = 525
#define PARAM_T58NS_FeNMCU       L"Timing 58Mhz NTSC_SQUARE FeNMCU"          // Default = 39

// [Timing_PAL_SQUARE_59MHZ]
#define PARAM_T59PS_FeHStart     L"Timing 59Mhz PAL_SQUARE FeHStart"        // Default = 0
#define PARAM_T59PS_FeHEnd       L"Timing 59Mhz PAL_SQUARE FeHEnd"          // Default = 768
#define PARAM_T59PS_FeHTotal     L"Timing 59Mhz PAL_SQUARE FeHTotal"        // Default = 944
#define PARAM_T59PS_FeHSStart    L"Timing 59Mhz PAL_SQUARE FeHSStart"       // Default = 782
#define PARAM_T59PS_FeHSEnd      L"Timing 59Mhz PAL_SQUARE FeHSEnd"         // Default = 838
#define PARAM_T59PS_FeBlankStart L"Timing 59Mhz PAL_SQUARE FeBlankStart"    // Default = 768
#define PARAM_T59PS_FeVStart     L"Timing 59Mhz PAL_SQUARE FeVStart"        // Default = 16
#define PARAM_T59PS_FeVEnd       L"Timing 59Mhz PAL_SQUARE FeVEnd"          // Default = 304
#define PARAM_T59PS_FeVTotal     L"Timing 59Mhz PAL_SQUARE FeVTotal"        // Default = 625
#define PARAM_T59PS_FeNMCU       L"Timing 59Mhz PAL_SQUARE FeNMCU"          // Default = 47

// [Timing_NTSC_SQUARE_59MHZ]
#define PARAM_T59NS_FeHStart     L"Timing 59Mhz NTSC_SQUARE FeHStart"        // Default = 0
#define PARAM_T59NS_FeHEnd       L"Timing 59Mhz NTSC_SQUARE FeHEnd"          // Default = 640
#define PARAM_T59NS_FeHTotal     L"Timing 59Mhz NTSC_SQUARE FeHTotal"        // Default = 780
#define PARAM_T59NS_FeHSStart    L"Timing 59Mhz NTSC_SQUARE FeHSStart"       // Default = 658
#define PARAM_T59NS_FeHSEnd      L"Timing 59Mhz NTSC_SQUARE FeHSEnd"         // Default = 698
#define PARAM_T59NS_FeBlankStart L"Timing 59Mhz NTSC_SQUARE FeBlankStart"    // Default = 640
#define PARAM_T59NS_FeVStart     L"Timing 59Mhz NTSC_SQUARE FeVStart"        // Default = 14
#define PARAM_T59NS_FeVEnd       L"Timing 59Mhz NTSC_SQUARE FeVEnd"          // Default = 254
#define PARAM_T59NS_FeVTotal     L"Timing 59Mhz NTSC_SQUARE FeVTotal"        // Default = 525
#define PARAM_T59NS_FeNMCU       L"Timing 59Mhz NTSC_SQUARE FeNMCU"          // Default = 39


//
// Configuration error handling
//
// during startup of the kernel-driver, the PARAM_ERROR value is written with
// one of the values below. These are the IDs of strings in
// aviator\dll\aviator.rc that are produced in a dialog box by the user-mode
// driver during configuration if not VC_ERR_OK
// 
#define VC_ERR_OK		0	// no configuration error
#define VC_ERR_CREATEDEVICE	1001	// failed to create device object
#define VC_ERR_CONFLICT		1002	// resource conflict
#define VC_ERR_DETECTFAILED	1003	// could not find hardware
#define VC_ERR_INTERRUPT	1004	// interrupt did not occur

#endif

