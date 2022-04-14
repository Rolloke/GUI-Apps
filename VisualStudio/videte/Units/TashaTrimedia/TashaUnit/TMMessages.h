#define TMM_QUIT						-1

#define TMM_REQUEST_PAUSE_CAPTURE		1
#define TMM_CONFIRM_PAUSE_CAPTURE		2

#define TMM_REQUEST_RESUME_CAPTURE		3
#define TMM_CONFIRM_RESUME_CAPTURE		4

#define TMM_REQUEST_START_CAPTURE		5
#define TMM_CONFIRM_START_CAPTURE		6

#define TMM_REQUEST_STOP_CAPTURE		7
#define TMM_CONFIRM_STOP_CAPTURE		8
										
#define TMM_REQUEST_SET_BRIGHTNESS		9
#define TMM_CONFIRM_SET_BRIGHTNESS		10

#define TMM_REQUEST_SET_CONTRAST		11
#define TMM_CONFIRM_SET_CONTRAST		12
			   
#define TMM_REQUEST_SET_SATURATION		13
#define TMM_CONFIRM_SET_SATURATION		14

#define TMM_REQUEST_GET_BRIGHTNESS		15
#define TMM_CONFIRM_GET_BRIGHTNESS		16

#define TMM_REQUEST_GET_CONTRAST		17
#define TMM_CONFIRM_GET_CONTRAST		18

#define TMM_REQUEST_GET_SATURATION		19
#define TMM_CONFIRM_GET_SATURATION		20

#define TMM_NEW_CODEC_DATA				21
#define TMM_CONFIRM_NEW_CODEC_DATA		22

#define TMM_REQUEST_SET_INPUT_SOURCE	23
#define TMM_CONFIRM_SET_INPUT_SOURCE	24

#define TMM_REQUEST_GET_INPUT_SOURCE	25
#define TMM_CONFIRM_GET_INPUT_SOURCE	26

#define TMM_REQUEST_SCAN_FOR_CAMERAS	27
#define TMM_CONFIRM_SCAN_FOR_CAMERAS	28

#define TMM_REQUEST_SET_RELAIS_STATE	29
#define TMM_CONFIRM_SET_RELAIS_STATE	30

#define TMM_REQUEST_GET_RELAIS_STATE	31
#define TMM_CONFIRM_GET_RELAIS_STATE	32

#define TMM_REQUEST_GET_ALARM_STATE		33
#define TMM_CONFIRM_GET_ALARM_STATE		34

#define TMM_REQUEST_SET_ALARM_EDGE		35
#define TMM_CONFIRM_SET_ALARM_EDGE		36

#define TMM_REQUEST_GET_ALARM_EDGE		37
#define TMM_CONFIRM_GET_ALARM_EDGE		38

#define TMM_NOTIFY_ALARM				39

#define TMM_CONFIRM_INIT_COMPLETE		40

#define MESSAGE_NAME	(signed char*)"Message"
#define BUFFER_NAME		(signed char*)"JpegBuffer"
#define SRC_SEL1		(signed char*)"SourceSel1"
#define SRC_SEL2		(signed char*)"SourceSel2"

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


// Wertebereich der Helligkeit
#define MIN_BRIGHTNESS				0
#define MAX_BRIGHTNESS				255

// Wertebereich des Kontrastes
#define MIN_CONTRAST				0
#define MAX_CONTRAST				255

// Wertebereich des Farbsättigung
#define MIN_SATURATION				0
#define MAX_SATURATION				255


#ifndef WIN32
#define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#define LOWORD(l)           ((WORD)(l))
#define HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))
#endif

#define SM_BUFFER_SIZE		45*1024
///#define USE_SDRAM_MAPPING

#ifdef WIN32
#pragma pack( push, 4 )	// WICHTIG für den TriMedia

#endif
//////////////////////////////////////////////////////////////////////
typedef enum _DataType
{
	eTypeUnvalid	= 0,
	eTypeJpeg		= 1,
	eTypeH263		= 2,
	eTypeMpeg		= 3
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

//////////////////////////////////////////////////////////////////////
typedef struct
{
	BOOL		bValid;
	DataType	eType;	
	DataSubType	eSubType;
	TIME_STRUCT	TimeStamp;
	BOOL		bMotion;
	MDPoint		Point[5];
	WORD		wSource;
	DWORD		dwProzessID;
	DWORD		dwDataLen;	
	DWORD		dwData;
}DataPacket;

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
#pragma pack( pop )
#endif
