/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaUnit
// FILE:		$Workfile: TashaUnitDefs.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaUnit/TashaUnitDefs.h $
// CHECKIN:		$Date: 25.01.05 15:41 $
// VERSION:		$Revision: 50 $
// LAST CHANGE:	$Modtime: 25.01.05 15:40 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef _TASHAUNITDEFS_H_
#define _TASHAUNITDEFS_H_

#ifdef WIN32
// Der Blackfin mag lieber ein Structalignment von 8
#pragma pack(push)
#pragma pack(8)
#endif

// Encoder und Decoder Status
#define UNKNOWN						0
#define ENCODER_ON					1
#define ENCODER_OFF					2

#define SETBIT(w,b)    ((DWORD)((DWORD)(w) | (1L << (b))))
#define CLRBIT(w,b)    ((DWORD)((DWORD)(w) & ~(1L << (b))))
#define TSTBIT(w,b)    ((BOOL)((DWORD)(w) & (1L << (b)) ? TRUE : FALSE))

// DeviceID, VendorID der Tasha
#define	TASHA_DEVICE_ID				0x1535
#define TASHA_VENDOR_ID				0x11D4

// SubVendorID der SaVic
#define TASHA_SUBSYSTEM_ID1			0x1864
#define TASHA_SUBSYSTEM_ID2			0x1865
#define TASHA_SUBSYSTEM_ID3			0x1866
#define TASHA_SUBSYSTEM_ID4			0x1867
#define MAX_TASHA_BOARDS			4

// VideoQuellen (Müssen von 0 an aufsteigend nummeriert sein!!)
#define	VIDEO_SOURCE0				0
#define	VIDEO_SOURCE1				1
#define	VIDEO_SOURCE2				2
#define	VIDEO_SOURCE3				3
#define	VIDEO_SOURCE4				4
#define	VIDEO_SOURCE5				5
#define	VIDEO_SOURCE6				6
#define	VIDEO_SOURCE7				7
#define VIDEO_MAX_SOURCE			VIDEO_SOURCE7
#define VIDEO_SOURCE_CURRENT		254
#define VIDEO_SOURCE_NO				255
#define MAX_CAMERAS	(VIDEO_MAX_SOURCE + 1)

// Maxiamle Anzahl der Streams pro DSP
#define MAX_STREAMS					(2)

// Anzahl der aufeinanderfolgenden Bootversuche pro DSP
#define MAX_BOOT_ATTEMPTS			3

#define MAX_HSIZE					768
#define MAX_VSIZE					576

// Auflösung, die der Bewegungserkennung zugrunde liegt. Daraus ergeben sich 45x36 8x8-Blöcke
#define MD_RESOLUTION_H				360
#define MD_RESOLUTION_V				288

#define CHANNEL_SIZE				64*1024		// Ein Kanal hat 64KByte
#define CHANNEL_COUNT				8			// Insgesamt 8 Kanäle
#define MAX_FRAME_BUFFERS			8			// Anzahl der Framebuffer pro Tasha. MAX_FRAME_BUFFERS*(8*64KByte)
#define UNIVERSAL_BUFFER_SIZE		512*1024	// Buffer größe zum Transfer der BF533Firmware/Alterafile/EEPromdaten	

#define MAGIC_MARKER_START			0x25041978
#define MAGIC_MARKER_END			0x11071964

#define TASHA_ICON_NOCOLOR	0
#define TASHA_ICON_DISABLED 1
#define TASHA_ICON_ENABLED  2
#define TASHA_ICON_ACTIVITY	3
#define TASHA_ICON_FAILURE	4

//////////////////////////////////////////////////////////////////////
typedef enum _DataType
{
	eTypeUnvalid	= 0,
	eTypeJpeg		= 1,
	eTypeH263		= 2,
	eTypeMpeg4		= 3,
	eTypeYUV422		= 4
} DataType;

//////////////////////////////////////////////////////////////////////
typedef enum _DataSubType
{
	eSubIFrame		= 0,
	eSubPFrame		= 1,
	eSubTypeBFrame	= 2,
	eSubTypeUnvalid	= 3
} DataSubType;

//////////////////////////////////////////////////////////////////////
typedef enum _EncoderState
{
	eEncoderStateUnvalid	= 0,
	eEncoderStateOn			= 1,
	eEncoderStateOff		= 2,
	eEncoderStatePause		= 3
}EncoderState;

//////////////////////////////////////////////////////////////////////
typedef enum _ImageRes
{
	eImageResUnvalid		= 0,
	eImageResLow			= 1,	// QCIF
	eImageResMid			= 2,	// CIF
	eImageResHigh			= 3,	// 2CIF
	eImageResUltra			= 4,	// 4CIF	
}ImageRes;

//////////////////////////////////////////////////////////////////////
typedef struct _TIME_STRUCT		
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

typedef struct _EncoderParam
{
	DataType	eType;
	ImageRes	eRes;
	int			nBitrate;
	int			nFps;
	int			nIFrameIntervall;
	int			nStreamID;
}EncoderParam;

//////////////////////////////////////////////////////////////////////
typedef struct _MDPoint
{
	WORD	cx;
	WORD	cy;
	int		nValue;
}MDPoint;

typedef struct _MASK_STRUCT
{
	BOOL 	bValid;
	BOOL	bHasChanged;
	int		nDimH;
	int		nDimV;
	BYTE	byMask[MD_RESOLUTION_V/8][MD_RESOLUTION_H/8];		// nDimV, nDimH!!
}
MASK_STRUCT;

#define MAX_DEBUG_LEN	256			// Maximale Stringlänge
typedef struct _DEBUG_STRUCT
{
	BOOL	bValid;					// TRUE -> String vorhanden
	char	sText[MAX_DEBUG_LEN];	// String
}DEBUG_STRUCT;

//////////////////////////////////////////////////////////////////////
typedef struct _DATA_PACKET
{
	DWORD			dwDummy;			// Die ersten Bytes der SPORT-Übertragung enthalten oft Bitfehler, daher die Dummyvariable
	DWORD			dwStartMarker;		// Magic Marker1 (Startmarkierung des Headers)
	DWORD			dwCheckSum;			// Prüfsumme über die Bilddaten.
	DWORD			dwProzessTime;		// Gibt die Ausführungszeit des BF533 in MikroSeconds an
	BOOL			bValid;				// TRUE: Bilddaten können abgeholt werden
	WORD			wSize;				// Größe der Struktur ('sizeof(DATA_PACKET)')
	DataType		eType;				// Gibt den Typ des Bildes an (Jpeg, Mpeg4, YUV422,...)	
	DataSubType		eSubType;			// Beschreibt den Bildtyp näher (I-Frame, P-Frame,....)
	WORD			wSource;			// Kameranummer
	WORD			wSizeH;				// Breite des Bildes
	WORD			wSizeV;				// Höhe des Bildes
	WORD			wBytesPerPixel;		// Bytes pro Pixel: (z.B. 2Bytes bei YUV422)
	WORD			wField;				// 0->Even Field 1->Odd Field
	BOOL			bVidPresent;		// TRUE->Video detektiert/FALSE->Video nicht detektiert.
	DWORD			dwFieldID;			// FieldCounter
	DWORD			dwProzessID;		// ProzessID für den Server
	DWORD			dwStreamID;			// Streamnummer für BF533 Video requests 
	DWORD			dwImageDataLen;		// Länge der eigentlichen Bilddaten
	BOOL			bMotion;			// TRUE: Dieses Bild enthält Bewegungskoordinaten
	MDPoint			Point[5];			// Auf 1000 normierte Bewegungskoordinaten
	TIME_STRUCT		TimeStamp;			// Teitpunkt der Aufnahme
	MASK_STRUCT		AdaptiveMask;		// Adaptive Maske
	MASK_STRUCT		PermanentMask;		// Permanente Maske
	MASK_STRUCT		Diff;				// Differenzbild
	DEBUG_STRUCT	Debug;				// Debug 
	DWORD			dwEndMarker;		// Magic Marker2 (Endmarkierung des Headers)
	BYTE			pImageData;			// Adresse der Bilddaten
}DATA_PACKET;

#ifdef WIN32
#pragma pack(pop)
#endif

#endif // _TASHAUNITDEFS_H_
