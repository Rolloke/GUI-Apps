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

#define MAX_HSIZE					768
#define MAX_VSIZE					576

#define CHANNEL_SIZE				64*1024	// Ein Kanal hat 64KByte
#define CHANNEL_COUNT				8		// Insgesamt 8 Kanäle

// Wertebereich der Helligkeit
#define MIN_BRIGHTNESS				0
#define MAX_BRIGHTNESS				255

// Wertebereich des Kontrastes
#define MIN_CONTRAST				0
#define MAX_CONTRAST				255

// Wertebereich des Farbsättigung
#define MIN_SATURATION				0
#define MAX_SATURATION				255

#define MAGIC_MARKER_START			0x12345678
#define MAGIC_MARKER_END			0x11071964

//////////////////////////////////////////////////////////////////////
typedef enum _DataType
{
	eTypeUnvalid	= 0,
	eTypeJpeg		= 1,
	eTypeH263		= 2,
	eTypeMpeg		= 3,
	eTypeYUV422		= 4
} DataType;

//////////////////////////////////////////////////////////////////////
typedef enum _DataSubType
{
	eSubTypeUnvalid	= 0,
	eSubIFrame		= 1,
	eSubPFrame		= 2,
	eSubTypeBFrame	= 3
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
	eImageResLow			= 1,
	eImageResHigh			= 2
}ImageRes;

//////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////
typedef struct
{
	WORD	cx;
	WORD	cy;
	int		nValue;
}MDPoint;

typedef struct
{
	BOOL 	bValid;
	BOOL	bHasChanged;
	int		nDimH;
	int		nDimV;
	BYTE	byMask[36][45];		// nDimV, nDimH!!
}
MASK_STRUCT;

#define MAX_DEBUG_LEN	256			// Maximale Stringlänge
typedef struct
{
	BOOL	bValid;					// TRUE -> String vorhanden
	char	sText[MAX_DEBUG_LEN];	// String
}DEBUG_STRUCT;

//////////////////////////////////////////////////////////////////////
typedef struct
{
	DWORD			dwStartMarker;		// Magic Marker1 (Startmarkierung des Headers)
	DWORD			dwCheckSum;			// Prüfsumme über die Bilddaten.
	DWORD			dwProzessTime;		// Gibt die Ausführungszeit des BF533 in MikroSeconds an
	BOOL			bValid;				// TRUE: Bilddaten können abgeholt werden
	WORD			wSize;				// Größe der Struktur ('sizeof(DATA_PACKET)')
	DataType		eType;				// Gibt den Typ des Bildes an (Jpeg, Dib, YUV422,...)	
	DataSubType		eSubType;			// Beschreibt den Bildtyp näher (I-Frame, P-Frame,....)
	WORD			wSource;			// Kameranummer
	WORD			wSizeH;				// Breite des Bildes
	WORD			wSizeV;				// Höhe des Bildes
	WORD			wBytesPerPixel;		// Bytes pro Pixel: (z.B. 2Bytes bei YUV422)
	WORD			wField;				// 0->Even Field 1->Odd Field
	BOOL			bVidPresent;		// TRUE->Video detektiert/FALSE->Video nicht detektiert.
	DWORD			dwFieldID;			// FieldCounter
	DWORD			dwProzessID;		// ProzessID für den Server
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

//////////////////////////////////////////////////////////////////////
typedef struct
{
	BOOL		bValid;			// TRUE->Werte sind gültig
	DWORD		dwProzessID;	// ProzessID
	WORD		wSource;		// Videoquelle auf die als nächstes gewechselt werden soll
	WORD		wBrightness;	// Gewünschte Helligkeit
	WORD		wContrast;		// Gewünschte Contrast
	WORD		wSaturation;	// Gewünschte Farbsättigung;
	ImageRes	eResolution;	// Bildformat
	DataType	eCompressenType;// Gewünschte Kompression
	DWORD		dwBPF;			// Bytes pro Frame
	DWORD		dwPics;			// Anzahl der gewünschten Bilder
}SOURCE_SELECTION;

typedef struct
{
	WORD		wBrightness;	// Gewünschte Helligkeit
	WORD		wContrast;		// Gewünschte Contrast
	WORD		wSaturation;	// Gewünschte Farbsättigung;
} COLOR_SETTINGS;

#ifdef WIN32
#pragma pack(pop)
#endif

#endif // _TASHAUNITDEFS_H_
