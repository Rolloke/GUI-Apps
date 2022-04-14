/////////////////////////////////////////////////////////////////////////////
// PROJECT:		GenericUnit
// FILE:		$Workfile: GenericDefs.h $
// ARCHIVE:		$Archive: /Project/Units/GenericUnit/GenericDefs.h $
// CHECKIN:		$Date: 20.01.06 12:40 $
// VERSION:		$Revision: 6 $
// LAST CHANGE:	$Modtime: 20.01.06 12:30 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __GENERICDEFS_H__
#define __GENERICDEFS_H__

#pragma warning(disable : 4244)  // disable 'conversion' conversion from 'type1' to 'type2' warning

////////////////////////////////////////////////////////////////
#define SETBIT(w,b)    ((DWORD)((DWORD)(w) | (1L << (b))))
#define CLRBIT(w,b)    ((DWORD)((DWORD)(w) & ~(1L << (b))))
#define TSTBIT(w,b)    ((BOOL)((DWORD)(w) & (1L << (b)) ? TRUE : FALSE))

#define TOGGLEBIT(w,b) (TSTBIT(w,b) ? CLRBIT(w,b) : SETBIT(w,b))

// Die unterschiedlichen Icons für die GenericUnit
#define GENERIC_ICON_NOCOLOR	0
#define GENERIC_ICON_BLUE		1
#define GENERIC_ICON_GREEN	2
#define GENERIC_ICON_RED		3

// USERDEF Messages
#define WM_GENERIC_REQUEST_OUTPUT_RESET	WM_USER + 2
#define WM_GENERIC_REQUEST_INPUT_RESET	WM_USER + 3

// VideoQuellen (Müssen von 0 an aufsteigend nummeriert sein!!)
#define	VIDEO_SOURCE0				0
#define	VIDEO_SOURCE1				1
#define	VIDEO_SOURCE2				2
#define	VIDEO_SOURCE3				3
#define	VIDEO_SOURCE4				4
#define	VIDEO_SOURCE5				5
#define	VIDEO_SOURCE6				6
#define	VIDEO_SOURCE7				7
#define	VIDEO_SOURCE8				8
#define	VIDEO_SOURCE9				9
#define	VIDEO_SOURCE10				10
#define	VIDEO_SOURCE11				11
#define	VIDEO_SOURCE12				12
#define	VIDEO_SOURCE13				13
#define	VIDEO_SOURCE14				14
#define	VIDEO_SOURCE15				15
#define VIDEO_MAX_SOURCE			VIDEO_SOURCE15
#define VIDEO_SOURCE_CURRENT		254
#define VIDEO_SOURCE_NO				255
#define MAX_CAMERAS	(VIDEO_MAX_SOURCE + 1)

// Video Formate (muß in dieser Reihenfolge bleiben)
#define VFMT_NTSC_CCIR				0
#define VFMT_PAL_CCIR				1
#define VFMT_INVALID				0xff		
		   
// Art der Eingangsquelle
#define SRCTYPE_FBAS				0
#define SRCTYPE_SVHS				1

// Mögliche Bildformate
#define ENCODER_RES_LOW				0
#define ENCODER_RES_HIGH			1
#define ENCODER_RES_UNKNOWN			0xffff

// Encoder Status
#define ENCODER_ON					1
#define ENCODER_OFF					2

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

// Fields
#define EVEN_FIELD					0
#define ODD_FIELD					1
#define BOTH_FIELDS					3

// Bytes pro Frame für die 5 in PicDef.h definierten Kompressionsfaktoren
#define BPF_0						50000
#define BPF_1						40000
#define BPF_2						32000
#define BPF_3						24000
#define BPF_4						16000
#define BPF_5						8000
 
#define JPEG_BUFFER_SIZE			0x20000	// 128KByte

typedef DWORD	HDEVICE	;

// Capturesize für MotionDetection
#define CAPTURE_SIZE_H			(768/4)
#define CAPTURE_SIZE_V			(576/4)
#define CAPTURE_BITS_PER_PIXEL	(32)
#define CAPTURE_BYTES_PER_PIXEL	((CAPTURE_BITS_PER_PIXEL + 7) / 8)
#define CAPTURE_BYTES_PER_LINE	(CAPTURE_SIZE_H * CAPTURE_BYTES_PER_PIXEL)
				    
// Capturestruct
typedef struct
{
	HDEVICE		hDevice;
	BGR24*		pFrameBuffer;
	BOOL		bHistogramm;
	BOOL		bAutoLuminance;
	BOOL		bFilter;
}CAPTURE_STRUCT;

			    
// Diese Struktur enthält alle für eine Umschaltung notwendigen Parameter.
// Vorsicht bei Änderungen der Struktur! (Siehe in CCodec::SetInputSource)
typedef struct
{
	HDEVICE	hDevice;		// Identifiziert das Device
	DWORD	dwProzessID;	// ProzessID
	DWORD	dwPics;			// Anzahl der gewünschten Bilder
	WORD	wSource;		// Videoquelle (VIDEO_SOURCE0...VIDEO_SOURCE7)
	WORD	wFormat;		// Bildformat (ENCODER_RES_HIGH, ENCODER_RES_LOW)
	DWORD	dwBPF;			// BytesPro Field
	DWORD	dwMinLen;		// Mindestgröße des Fields in Byte
	DWORD	dwMaxLen;		// Maximalgröße des Fields in Byte
	BOOL	bDoubleField;	// TRUE -> Vollbild (2Fields)
	BYTE	byPortY;		// Physikalischer Kameraeingang	(Y-Signal) (0...7)
	BYTE	byPortC;		// Physikalischer Kameraeingang	(C-Signal) (0...7)
	BOOL	bSwitch;		
}SOURCE_SELECT_STRUCT;
	
typedef struct
{
	PVOID pPhysAddr;	// Physikalische Adresse
	PVOID pLinAddr;		// Lineare Adresse
	DWORD dwLen;
} MEMSTRUCT;

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
}GENERIC_TIME_STRUCT;

typedef struct		
{
	DWORD				hDevice;		// Identifiziert das Device
	DWORD				dwProzessID;	// Eindeutige ProzeßID
	WORD				wSource;		// Videoquelle zu den die Bilddaten gehören
	MEMSTRUCT			Buffer;
	DWORD				dwLen;			// Länge der Bilddaten.
	GENERIC_TIME_STRUCT	TimeStamp;		// TimeStamp
	DWORD				dwFieldID;		// FieldCounter
	WORD				wField;			// 0->Even Field 1->Odd Field
	BOOL				bValid;			// TRUE -> Bildaten können abgeholt werden
	BYTE				byFCNT;			// Zoran FieldCounter (For Debug only)
	BYTE				byPage;			// Zoran Page		  (For Debug only)
} JPEG;

#endif // __GENERICDEFS_H__
