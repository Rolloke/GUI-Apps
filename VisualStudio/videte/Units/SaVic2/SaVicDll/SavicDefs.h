/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: SavicDefs.h $
// ARCHIVE:		$Archive: /Project/Units/SaVic2/savicdll/SavicDefs.h $
// CHECKIN:		$Date: 20/09/04 14:30 $
// VERSION:		$Revision: 18 $
// LAST CHANGE:	$Modtime: 20/09/04 14:20 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __SAVICDEFS_H__
#define __SAVICDEFS_H__

#include "..\SaVicSys\CJobQueue.h"

#pragma warning(disable : 4244)  // disable 'conversion' conversion from 'type1' to 'type2' warning

// Versionsnummer von SaVicUnit, SaVicDA.dll, SaVic.Dll, und SaVic.sys
#define SAVIC_UNIT_VERSION				0x00040040

////////////////////////////////////////////////////////////////
#define SETBIT(w,b)    ((DWORD)((DWORD)(w) | (1L << (b))))
#define CLRBIT(w,b)    ((DWORD)((DWORD)(w) & ~(1L << (b))))
#define TSTBIT(w,b)    ((BOOL)((DWORD)(w) & (1L << (b)) ? TRUE : FALSE))
#define TOGGLEBIT(w,b) (TSTBIT(w,b) ? CLRBIT(w,b) : SETBIT(w,b))

// DeviceID, VendorID der SaVic
#define	SAVIC_DEVICE_ID				0x036E
#define SAVIC_VENDOR_ID				0x109E

// SubVendorID der SaVic
#define SAVIC_SUBSYSTEM_ID1			0x1864
#define SAVIC_SUBSYSTEM_ID2			0x1865
#define SAVIC_SUBSYSTEM_ID3			0x1866
#define SAVIC_SUBSYSTEM_ID4			0x1867

// SubsystemID des SV823A-Barebones.
#define SV823_SUBSYSTEM_ID			0x0000

// SubsystemVendorID DRESEARCH
//#define SAVIC_SUBSYSTEM_VENDOR_ID	0x15EB

// SubsystemVendorID VideteIT
#define SAVIC_SUBSYSTEM_VENDOR_ID	0x1FC6

#ifndef WM_USER
#define WM_USER	0x0400
#endif

// Die unterschiedlichen Icons für die SaVicUnit
#define SAVIC_ICON_NOCOLOR	0
#define SAVIC_ICON_BLUE		1
#define SAVIC_ICON_GREEN	2
#define SAVIC_ICON_RED		3

// USERDEF Messages
#define WM_SAVIC_REQUEST_OUTPUT_RESET	SAVIC_MSGBASE + 2
#define WM_SAVIC_REQUEST_INPUT_RESET	SAVIC_MSGBASE + 3

// Bestückungsvaranten
#define VARIATION_ALARM_SWAP_BIT	0
#define VARIATION_VIDEO_SWAP_BIT	1			
#define VARIATION_ALARM4_PRESENT	2
#define VARIATION_SV823A			3


// VARIATION_ALARM_SWAP_BIT = 1 -> Alarmeingang 0 liegt auf Eingang 3
//								-> Alarmeingang 1 liegt auf Eingang 2
//								-> Alarmeingang 2 liegt auf Eingang 1
//								-> Alarmeingang 3 liegt auf Eingang 0

// VARIATION_VIDEO_SWAP_BIT = 1 -> Videoeingang 0 liegt auf Eingang 3
//								-> Videoeingang 1 liegt auf Eingang 2
//								-> Videoeingang 2 liegt auf Eingang 1
//								-> Videoeingang 3 liegt auf Eingang 0

// VideoQuellen (Müssen von 0 an aufsteigend nummeriert sein!!)
#define	VIDEO_SOURCE0				0
#define	VIDEO_SOURCE1				1
#define	VIDEO_SOURCE2				2
#define	VIDEO_SOURCE3				3
#define VIDEO_MAX_SOURCE			VIDEO_SOURCE3
#define VIDEO_SOURCE_CURRENT		254
#define VIDEO_SOURCE_NO				255
#define MAX_CAMERAS	(VIDEO_MAX_SOURCE + 1)

// Fields
#define EVEN_FIELD					0
#define ODD_FIELD					1
#define BOTH_FIELDS					3
					   
// MD-Sensitivities
#define MD_SENSITIVITY_LOW			1
#define MD_SENSITIVITY_NORMAL		2
#define MD_SENSITIVITY_HIGH			3

// Filtertyp
#define NOTCH_FILTER_ON				1
#define NOTCH_FILTER_OFF			2
#define NOTCH_FILTER_AUTO			3

// Video Formate (muß in dieser Reihenfolge bleiben)
#define VFMT_NTSC_CCIR				0
#define VFMT_PAL_CCIR				1
#define VFMT_INVALID				0xff		
		   
// VideoFrontEnd-Filter
#define LNOTCH_FILTER				1
#define LDEC_FILTER					2

// Encoder und Decoder Status
#define ENCODER_ON					1
#define ENCODER_OFF					2

// Diese Eventarten gibt es
#define EVENT_INFO						0x0001		
#define EVENT_WARNING					0x0002
#define EVENT_ERROR						0x0003
#define EVENT_STAT_PEAK_INFO			0x0004
#define EVENT_STAT_PEAK_WARNING			0x0005
#define EVENT_STAT_PEAK_ERROR			0x0006

// Kameratyp
#define CAMTYPE_ASYNC					0
#define CAMTYPE_SYNC					1

// Fehlermeldungen
#define ERR_NO_ERROR					0
#define ERR_WRONG_FE_ID					1
#define ERR_WRONG_SOURCE				2
#define ERR_WRONG_SOURCE_TYPE			3
#define ERR_WRONG_FE_VIDEO_FORMAT		4
#define ERR_PARM_OUT_OF_RANGE			5	
#define ERR_UNKNOWN_PARAMETER			6
#define ERR_WRONG_FILTER_TYPE			7 
#define ERR_WRONG_ENCODER_FORMAT		8 
#define ERR_WRONG_ENCODER_BPF			9 
#define ERR_TIMEOUT						10
#define ERR_WAITFAILED					11
#define ERR_UNVALID_FIELD				12
#define ERR_ENCODER_OFF					13
#define ERR_WRONG_IRQ_NUMBER			14
#define ERR_BT878_WRONG_ID				15
#define ERR_BT878_READBACK_FAILED		16
#define DEFAULT_ERROR					0xffff

// Wertebereich der Helligkeit
#define MIN_BRIGHTNESS				0
#define MAX_BRIGHTNESS				255

// Wertebereich des Kontrastes
#define MIN_CONTRAST				0
#define MAX_CONTRAST				511

// Wertebereich des Farbsättigung
#define MIN_SATURATION				0
#define MAX_SATURATION				511

// Wertebereich des Hue
#define MIN_HUE						0
#define MAX_HUE						255

// Wertebereich der horizontalen Skalierung
#define MIN_HSCALE					0
#define MAX_HSCALE					65535

// Wertebereich der vertikalen Skalierung
#define MIN_VSCALE					0
#define MAX_VSCALE					65535

// Wertebereich der BytesPerFrame
#define MIN_BPF						1000
#define MAX_BPF						JPEG_BUFFER_SIZE

// Wertebereich für MPI
#define MIN_FPS						1
#define MAX_FPS						25

// Bytes pro Frame für die 5 in PicDef.h definierten Kompressionsfaktoren
#define BPF_0						50000
#define BPF_1						40000
#define BPF_2						32000
#define BPF_3						24000
#define BPF_4						16000
#define BPF_5						8000
 
#define MAX_FIELD_BUFFER			8
#define FIELD_BUFFER_SIZE			(768*288*2+2048)	// Maximale Größe des Inlaybuffers
#define JPEG_BUFFER_SIZE			(256*1024)			// 256 KBytes	

#define MAX_MSG_LEN					256
#define MAX_EVENTS					64

#define	VS_MUX						0
#define AA1_MUX						1
#define AA2_MUX						2

typedef ULONG	HIRQ;			// IRQ Handle
typedef DWORD	HDEVICE	;

// Capturesize für MotionDetection
#define CAPTURE_SIZE_H			(768/4)
#define CAPTURE_SIZE_V			(576/4)

// Status der beiden LEDs
typedef enum
{
	eLED1_On,
	eLED1_Blink,
	eLED2_On,
	eLED2_Off
}eLEDState;

typedef enum
{
	eQueueFree,
	eQueuePending,
	eQueueComplete
}QueueState;

typedef enum
{
	eSkipFields,
	eFindVBlank
}eStabilisation;


#ifndef BGR24a
#define BGR24a				    
// 24Bit RGB
typedef struct
{
	BYTE bB;
	BYTE bG;
	BYTE bR;
}BGR24;
#endif

typedef struct
{
	BYTE bY1;
	BYTE bU;
	BYTE bY2;
	BYTE bV;
}YUV422;

typedef struct
{
	BYTE bY;
}Y8;

typedef struct
{
	 WORD	wSource;					// Kameraeingang, der gerade gescannt wird.
	 DWORD	dwCamMask;					// Maske des gerade laufenden Scannvorganges
	 WORD	wCnt;						// Anzahl der Pollvorgänge (z.B. 10)
	 BOOL	bResult[2];					// bResult[0]=Anzahl der Oddbilder, bResult[1]=Anzahl der Evenbilder
	 LARGE_INTEGER	i64LastSwitchTime;	// Zeitpunkt des letzten Kamerawechsels
	 DWORD	dwLastCamScanResult;		// Ergebnis des letzten kompletten Scanvorganges.
}CAM_SCAN_STRUCT;

typedef struct		
{
	BOOL	bValid;		// TRUE: Zeitangabe ist gültig, ansonsten FALSE
	WORD	wYear;
	WORD	wMonth;
	WORD	wDay;
	WORD	wHour;
	WORD	wMinute;
	WORD	wSecond;
	WORD	wMSecond;
}TIME_STRUCT;

#define SWAP(X) (((X&0xff)<<8) | (X>>8))

typedef struct
{
	DWORD	dwInBufferSize;
	void*	pInBuffer;
	DWORD	dwOutBufferSize;
	void*	pOutBuffer;
	DWORD*	pBytesToReturn;
}IOCtlStruct;

// Debug-Packet.
typedef struct
{
	HDEVICE		hDevice;
	WORD		wEventType;
//	_TCHAR		Message[MAX_MSG_LEN];
	char		Message[MAX_MSG_LEN];
	DWORD		dwMaxMsgLength;
	BOOL		bValid;
	DWORD		dwNumParam;	
	DWORD		dwParam1;
	DWORD		dwParam2;
	DWORD		dwParam3;
	DWORD		dwParam4;
}EVENT_PACKET;

typedef struct
{
	WORD		wSource[MAX_FIELD_BUFFER];		// Kameranummer
	QueueState	eQueueState[MAX_FIELD_BUFFER];	// Status des Queueeintrages
	WORD		wReadIndex;						// Leseindex
	WORD		wWriteIndex;					// Schreibindex
}QUEUE_INF;

// Diese Struktur enthält alle für eine Umschaltung notwendigen Parameter.
// Vorsicht bei Änderungen der Struktur! (Siehe in CCodec::SetInputSource)
typedef struct
{
	HDEVICE	hDevice;			// Identifiziert das Device
	DWORD	dwProzessID;		// ProzessID
	int		nPics;				// Anzahl der gewünschten Bilder
	WORD	wSource;			// Videoquelle (VIDEO_SOURCE0...VIDEO_SOURCE7)
	WORD	wPortYC;			// Physikalischer Kameraeingang	(Composite-Signal)
	WORD	wFormat;			// Bildformat (RESOLUTION_HIGH, RESOLUTION_MID, RESOLUTION_LOW)
	WORD	wBrightness;		// Helligkeit
	WORD	wContrast;			// Kontrast
	WORD	wSaturation;		// Farbsättigung
	WORD	wHue;				// Hue
	DWORD	dwBPF;				// BytesPro Field
	BOOL	bDoubleField;		// TRUE -> Vollbild (2Fields)
	WORD	wCompressionType;	// Art des gewünschten Bildes (Jpeg, DIB,...)
	int		nPriority;			// Umschaltpriorität (Derzeit noch nicht verwendet)
	BOOL	bSwitch;		
}VIDEO_JOB;
	
// Io-Größenangabe 8Bit, 16Bit 32Bit
#define IO_SIZE8	1
#define IO_SIZE16	2
#define IO_SIZE32	3

typedef struct
{
	WORD  wIOSize;
	DWORD dwAddr;
	DWORD dwVal;
	WORD  wVal;
	BYTE  byVal;
} IOSTRUCT;

typedef struct
{
	DWORD dwIOBase;
	BYTE  bySlaveAddr;
	BYTE  bySubAddr;
	BYTE  byVal;
} I2CSTRUCT;


typedef struct
{
	PVOID pPhysAddr;	// Physikalische Adresse
	PVOID pLinAddr;		// Lineare Adresse
	DWORD dwLen;
} MEMSTRUCT;

typedef struct
{
	DWORD	dwPhysAddr;
	DWORD	dwLen;
}FRAGMENTSTRUCT;

typedef struct
{
	WORD wSourceType;			// SVHS/FBAS
	WORD wFilter;				// NOTCH/DEC
	WORD wBrightness;			// Helligkeit
	WORD wContrast;				// Kontrast
	WORD wSaturation;			// Farbsättigung
	WORD wHue;					// Farbbalance
	WORD wPortYC;				// Port fürs Videosignal-Signal
	BOOL bDoubleField;			// TRUE -> Vollbild (2Fields)
} SOURCESTRUCT;

typedef struct
{
	SOURCESTRUCT Source[VIDEO_MAX_SOURCE+1];
} CODECSTRUCT;

typedef struct
{
	DWORD	dwIOBase;				// Basisadresse
	WORD	wIRQ;					// Interruptnummer
	DWORD	hFieldReadyEvent;		// Eventhandle: Bild fertig komprimiert
	DWORD	hSourceSwitchEvent;		// Eventhandle: Digitale Umschaltung fertig
	DWORD	hDebugEvent;			// Eventhandle: Message
	WORD	wField;					// Gibt an welches Field gespeichert wird
	WORD	wCameraType;			// 0=Async, 1=Sync
	WORD	wSkipFields;			// Anzahl der Fields, die nach einer Umschaltung verworfen werden.
	WORD	wCaptureSizeH;			// Horizontale Größe des MD-Capture Inlays
	WORD	wCaptureSizeV;			// Vertikale Größe des MD-Capture Inlays
	WORD	wMapping[MAX_CAMERAS];	// Das Kamerascannen muß das Mapping berücksichtigen.
	BOOL	bUseSyncSeperator;		// Interne/Externe Kamerasignalüberwachung
}SETTINGS;

typedef struct		
{
	DWORD				hDevice;			// Identifiziert das Device
	DWORD				dwProzessID;		// Eindeutige ProzeßID
	WORD				wSource;			// Videoquelle zu den die Bilddaten gehören
	TIME_STRUCT			TimeStamp;			// TimeStamp
	DWORD				dwFieldID;			// FieldCounter
	WORD				wField;				// 0->Even Field 1->Odd Field
	BYTE*				pImageData;			// Adresse der Bilddaten
	DWORD				dwImageLen;			// Länge der Bilddaten.
	WORD				wSizeH;				// Breite des Bildes
	WORD				wSizeV;				// Höhe des Bildes
	WORD				wCompressionType;	// Art des gewünschten Bildes (Jpeg, DIB,...)
	WORD				wFormat;			// Bildformat (RESOLUTION_HIGH, RESOLUTION_MID, RESOLUTION_LOW)
	DWORD				dwBPF;				// Gewünschte Bildgröße in BytesPerFrame
	BOOL				bValid;				// TRUE -> Bildaten können abgeholt werden
	BOOL				bVidPresent;		// TRUE->Video detektiert/FALSE->Video nicht detektiert.
	int					nOutstandingPics;	// Zählt die noch ausstehenden Bilder eines Jobs
} IMAGE;

// Capturestruct
typedef struct		
{
	DWORD				hDevice;			// Identifiziert das Device
	DWORD				dwProzessID;		// Eindeutige ProzeßID
	WORD				wSource;			// Videoquelle zu den die Bilddaten gehören
	MEMSTRUCT			Buffer;				// Buffer für die Bilddaten
	PVOID				pMdl;				// MDL für den Zugriff aus dem Usermode.
	PVOID				pUserBuffer;		// Usermode-Zeiger auf die Bilddaten,
	DWORD				dwLen;				// Länge der Bilddaten.
	WORD				wSizeH;				// Breite des Bildes
	WORD				wSizeV;				// Höhe des Bildes
	WORD				wSourceType;		// Art des Quellbildes (Derzeit nur YUV422)
	WORD				wFormat;			// Gewünschtes Bildformat (RESOLUTION_HIGH, RESOLUTION_MID, RESOLUTION_LOW)
	DWORD				dwBPF;				// Gewünschte Framegröße in Bytes.
	TIME_STRUCT			TimeStamp;			// TimeStamp
	DWORD				dwFieldID;			// FieldCounter
	WORD				wField;				// 0->Even Field 1->Odd Field
	WORD				wCompressionType;	// Art des gewünschten Bildes (Jpeg, DIB,...)
	QueueState			eQueueState;		// eFree: Leer, ePending: In Arbeit, eComplete: Fertig
	BOOL				bVidPresent;		// TRUE->Video detektiert/FALSE->Video nicht detektiert.
	LARGE_INTEGER		i64RDTSC;			// Zählerstand des RDTSC-Registers
	int					nOutstandingPics;	// Zählt die noch ausstehenden Bilder eines Jobs
} CAPTURE_STRUCT;

typedef struct	    
{
	WORD			wIRQ;					// Interruptnummer des SaVicboards
	DWORD			dwIOBase;				// Basisadresse des SaVicboards
	DWORD			dwIRR;					// Latch des Interrupt Request Registers
	WORD			wCardNr;				// Nummer des SaVicBoards (0...3)
	DWORD			hFieldReadyEvent;		// Ring0 Eventhandle fürs Encoden
	DWORD			hSourceSwitchEvent;		// Ring0 Eventhandle fürs Umschalten
	DWORD			hDebugEvent;			// Ring0 Eventhandle für Debugmeldungen

	CJobQueue		JobQueue;				// Jobqueue

	DWORD			dwProzessID;			// Eindeutige ProzeßID des laufenden Jobs
	int				nPics;					// Bildzähler
	WORD			wSource;				// Aktuelle Videoquelle
	WORD			wPortYC;				// Port der aktuellen Videoquelle (Zum mappen)
	WORD			wCompressionType;		// Aktuelle Bildkompression (z.B. JPeg, DIB,...)
	WORD			wFormat;				// Bildformat (RESOLUTION_HIGH, RESOLUTION_MID, RESOLUTION_LOW)
	DWORD			dwBPF;					// Gewünschte Framegröße in Bytes.
	WORD			wCameraType;			// 0=Async, 1=Sync
	WORD			wReqField;				// Legt fest welches Field genommen wird.

	WORD			wCurrentField;			// Enthält die aktuelle Fieldkennung

	EVENT_PACKET	TracePacket[MAX_EVENTS];// TracePacket
	WORD			wWrTP;					// Schreibindex des Tracepacket
	WORD			wRdTP;					// Leseindex des Tracepacket

	CAPTURE_STRUCT	FieldQueue[MAX_FIELD_BUFFER];	// Field-Queue
	WORD			wWriteIndex;			// Schreibindex
	WORD			wReadIndex;				// LeseIndex

	int				nSkipFields;			// Anzahl der Fields, die nach einer Umschaltung verworfen werden.
	int				nSkipFieldCnt;			// Anzahl der bereits geskipprten Bilder
	
	DWORD			dwFieldID;				// FieldCounter
	DWORD			dwMessage;				// MessageNummer für PostMessage

	HIRQ			hIrq;					// IRQ-handle
	void*			pInterruptObject;		// Pointer auf Interruptobjekt (WinNT)
	MEMSTRUCT		KDpcForIsr;				// Pointer auf Dpc (WinNT);
	MEMSTRUCT		KDpcSourceSwitch;		// (Obsolete) Pointer auf Dpc (WinNT);
	MEMSTRUCT		KDpcFieldReady;			// Pointer auf Dpc (WinNT);
	MEMSTRUCT		KDpcDebug;				// Pointer auf Dpc (WinNT);
	MEMSTRUCT		SpinLock;				// Pointer auf ISR-Spinlock
	MEMSTRUCT		OddInlayFieldBuffer;	// Adresse des Inlays (Odd)
	MEMSTRUCT		EvenInlayFieldBuffer;	// Adresse des Inlays (Even)
	MEMSTRUCT		RiscSpace;				// Adresse des Risc-Programmes
	WORD			wCaptureSizeH;			// MD-Capturesize horizontal
	WORD			wCaptureSizeV;			// MD-Capturesize vertical

	BOOL			bVidPresent;			// TRUE->Video detektiert/FALSE->Video nicht detektiert.
	eStabilisation	eStabilisationMethode[MAX_CAMERAS]; // Art der Bildstabilisierung
	CAM_SCAN_STRUCT	CameraScan;				// Enthält Informationen für die VideoSignalerkennung
	BOOL			bUseSyncSeperator;		// Interne/Externe Kamerasignalüberwachung

	WORD			wMapping[MAX_CAMERAS];	// Das Kamerascannen muß das Mapping berücksichtigen.
} CONFIG;		 

#endif // __SAVICDEFS_H__
