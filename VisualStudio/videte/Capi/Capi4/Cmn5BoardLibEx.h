#ifndef CMN5BOARDLIB_EX_H
#define CMN5BOARDLIB_EX_H

#pragma pack( push, Cmn5BoardLib_EX_H )
#pragma pack(8)

#	if defined(DVRDLL_EXPORTS)
#		define CMN5_API  __declspec(dllexport) __stdcall
#	else
#		define CMN5_API  __declspec(dllimport) __stdcall
#	endif

#if defined(KERNEL_MODE_CODE)
	typedef unsigned long DWORD;
	typedef int BOOL;
#endif


#define CMN5_SYSTEM_MAX_BOARD				8
#define CMN5_BOARD_MAX_VP					256
#define CMN5_BOARD_MAX_CHANNEL				256
#define CMN5_BOARD_MAX_DIO					256
#define CMN5_BOARD_MAX_CHANNEL_ARRAY_SIZE	8
#define CMN5_BOARD_MAX_DIO_ARRAY_SIZE		8

#define CMN5_INVALID_CHANNEL				0xFFFFFFFF		//jb
#define CMN5_ALL_BOARD						0xFF			//jb

// Media type  //jb
#define	CMN5_MEDIA_RAW_VIDEO				0x01	
#define	CMN5_MEDIA_RAW_AUDIO				0x02
#define	CMN5_MEDIA_OVERLAY					0x04
#define	CMN5_MEDIA_CODEC					0x08

//////////////////////////////////////////////////////////////////////////
// System Infomation
#define CMN5_MAX_RESERVED_SYSTEM_INFO		8
#define CMN5_MAX_RESERVED_BOARD_INFO_DESC	4
#define CMN5_MAX_RESERVED_BOARD_INFO		16

#define CMN5_BOARD_INFO_VERSION				1

typedef struct _CMN5_SYSTEM_INFO {
	ULONG	uNumOfBoard;
	ULONG	uDllVersion;
	ULONG	uSysVersion;
	ULONG	reserved[CMN5_MAX_RESERVED_SYSTEM_INFO - 3];
} CMN5_SYSTEM_INFO;

typedef struct _CMN5_BOARD_INFO_DESC {
	ULONG	uInfoSize;
	ULONG	uInfoVersion;
	ULONG	reserved[CMN5_MAX_RESERVED_BOARD_INFO_DESC - 2];
} CMN5_BOARD_INFO_DESC;


//////////////////////////////////////////////////////////////////////////
// Error Code
#define CMN5_MAX_ERROR_CODE_MSG_LENGTH		128		//jb
#define CMN5_MAX_SESSION_LENGTH				64		//jb
typedef struct _CMN5_ERROR_CODE_ITEM {
	ULONG			ErrorCode;										// code
	LARGE_INTEGER	TimeStamp;										// time-stamp
	ULONG			Reserved;										// reserved
	char			AuxMsg[CMN5_MAX_ERROR_CODE_MSG_LENGTH];	// aux message
	char			SessionName[CMN5_MAX_SESSION_LENGTH];	// session name
} CMN5_ERROR_CODE_ITEM;


//////////////////////////////////////////////////////////////////////////
// Video Format
#define CMN5_VIDEO_FORMAT_NONE		0		//jb
#define CMN5_VIDEO_FORMAT_NTSC_M	1
#define CMN5_VIDEO_FORMAT_PAL_B		2

#define CMN5_DEFAULT_VIDEO_FORMAT CMN5_VIDEO_FORMAT_NTSC_M


//////////////////////////////////////////////////////////////////////////
// Image Size
/* ImageSize define

	DWORD I;
	I[31..28] : reserved....
	I[27	] : Interace Frame(1)/weave Frame(0)
	I[26	] : W double
	I[25	] : H double
	I[24	] : VFLIP
	I[23..12] : H
	I[11.. 0] : W

  ex) 640x480
	0000 xxxxx 640 480
	I = (480<<12) | (640);
*/
union CMN5_IMAGESIZE {
	ULONG uAll;
	struct {
		ULONG nWidth			: 12;	//[0..11]
		ULONG nHeight			: 12;	//[12..23]
		ULONG bInvert			: 1;	//[24]
		ULONG bHDouble			: 1;	//[25]
		ULONG bVDouble			: 1;	//[26]
		ULONG bInteracedFrame	: 1;	//[27]
		ULONG nScaledDown		: 2;	//[29..28]
		ULONG reserved			: 2;	//[30..31]
	};
};

#define CMN5_MAKEIMGSIZE(w, h)		((w & 0xFFF) | ((h &0xFFF) << 12))
#define CMN5_GETIMGWIDTH(is)		(is & 0xFFF)
#define CMN5_GETIMGHEIGHT(is)		((is >> 12) & 0xFFF)
#define CMN5_IMAGESIZE_WDOUBLE		(1<<26)
#define CMN5_IMAGESIZE_HDOUBLE		(1<<25)
#define CMN5_IMAGESIZE_VFLIP		(1<<24)
#define CMN5_IMAGESIZE_SEP_FIELD	(1<<27)
#define CMN5_IMAGESIZE_SCALED_DOWN_CIF		(1<<28)
#define CMN5_IMAGESIZE_SCALED_DOWN_QCIF		(2<<28)

#define CMN5_IMAGESIZE_768X576	CMN5_MAKEIMGSIZE(768, 576)
#define CMN5_IMAGESIZE_768X288	CMN5_MAKEIMGSIZE(768, 288)
#define CMN5_IMAGESIZE_384X288	CMN5_MAKEIMGSIZE(384, 288)

#define CMN5_IMAGESIZE_720X576	CMN5_MAKEIMGSIZE(720, 576)
#define CMN5_IMAGESIZE_720X288	CMN5_MAKEIMGSIZE(720, 288)
#define CMN5_IMAGESIZE_360X288	CMN5_MAKEIMGSIZE(360, 288)
#define CMN5_IMAGESIZE_180X144	CMN5_MAKEIMGSIZE(180, 144)

#define CMN5_IMAGESIZE_720X480	CMN5_MAKEIMGSIZE(720, 480)
#define CMN5_IMAGESIZE_720X240	CMN5_MAKEIMGSIZE(720, 240)
#define CMN5_IMAGESIZE_360X240	CMN5_MAKEIMGSIZE(360, 240)

#define CMN5_IMAGESIZE_640X480	CMN5_MAKEIMGSIZE(640, 480)
#define CMN5_IMAGESIZE_640X240	CMN5_MAKEIMGSIZE(640, 240)
#define CMN5_IMAGESIZE_320X240	CMN5_MAKEIMGSIZE(320, 240)
#define CMN5_IMAGESIZE_160X120	CMN5_MAKEIMGSIZE(160, 120)

#define CMN5_IMAGESIZE_704X576	CMN5_MAKEIMGSIZE(704, 576)
#define CMN5_IMAGESIZE_704X288	CMN5_MAKEIMGSIZE(704, 288)
#define CMN5_IMAGESIZE_352X288	CMN5_MAKEIMGSIZE(352, 288)

#define CMN5_IMAGESIZE_704X480	CMN5_MAKEIMGSIZE(704, 480)
#define CMN5_IMAGESIZE_704X240	CMN5_MAKEIMGSIZE(704, 240)
#define CMN5_IMAGESIZE_352X240	CMN5_MAKEIMGSIZE(352, 240)

#define CMN5_IMAGESIZE_672X544	CMN5_MAKEIMGSIZE(672, 544)
#define CMN5_IMAGESIZE_672X272	CMN5_MAKEIMGSIZE(672, 272)
#define CMN5_IMAGESIZE_336X272	CMN5_MAKEIMGSIZE(336, 272)




#define CMN5_MAX_RESOLUTION_COUNT	(1<<6)	//6bit... 64...
#define CMN5_MAX_RESOLUTION_VALUE	(CMN5_MAX_RESOLUTION_COUNT-1)
/* Image Size Description
	ULONG ISD
	
	ISD[23..16]		Supported Video Format
	ISD[15..8]		Subimage,	Each bit represent.
	ISD[7..0 ]		Current Image, Only one bit is available.
*/
	
/* Image Size Description
	ULONG ISD
	
	ISD[9..8 ]		Group /sub/sep id...
	ISD[7..0 ]		Supported Video Format
*/

#define CMN5_IMAGESIZE_IS_GROUP	0x000
#define CMN5_IMAGESIZE_IS_SUB	0x100
#define CMN5_IMAGESIZE_IS_SEP	0x200


#define CMN5_IMAGESIZE_SUPPORT_NTSC_M	(0x01<<CMN5_VIDEO_FORMAT_NTSC_M)
#define CMN5_IMAGESIZE_SUPPORT_PAL_B	(0x01<<CMN5_VIDEO_FORMAT_PAL_B)
#define CMN5_IMAGESIZE_SUPPORT_BOTH		(CMN5_IMAGESIZE_SUPPORT_NTSC_M | CMN5_IMAGESIZE_SUPPORT_PAL_B)

typedef struct _CMN5_IMAGESIZE_DESC{
	ULONG uImageSize;
	ULONG uSizeDesc;
} CMN5_IMAGESIZE_DESC;

typedef struct _CMN5_RESOLUTION_INFO{
	BOOL	fSameGroupOnly;
	ULONG	uSizeCount;
	ULONG	uDefaultNTSCSize;
	ULONG	uDefaultPALSize;
	CMN5_IMAGESIZE_DESC aImgDesc[CMN5_MAX_RESOLUTION_COUNT];
} CMN5_RESOLUTION_INFO;

//////////////////////////////////////////////////////////////////////////////////
// 
#define CMN5_ACTIVATION_CODE_SIZE					16

//////////////////////////////////////////////////////////////////////////////////
// I2C & EEPROM
#define	CMN5_I2C_CMD_READ				1
#define	CMN5_I2C_CMD_WRITE				2
#define	CMN5_I2C_CMD_RANDOM_READ		3
#define	CMN5_I2C_CMD_RANDOM_WRITE		4
#define	CMN5_I2C_CMD_RANDOM_WRITE_WAIT	5

#define CMN5_USER_EEPROM_SIZE		16

#define CMN5_UID_ADDR				0x10000

//////////////////////////////////////////////////////////////////////////////////
// process state
//enum Cmn5ProcessState 
#define	CMN5_PS_INIT	1
#define	CMN5_PS_SETUP	2
#define	CMN5_PS_RUN		3

//enum WatchdogCmd {
#define	CMN5_WC_ENABLE						1
#define	CMN5_WC_DISABLE						2
#define	CMN5_WC_SET_TIMEOUT_VALUE			3
#define	CMN5_WC_TRIGGER						4
#define	CMN5_WC_SET_BUZZER_TIMEOUT_VALUE	5
//};

//enum QueryInfoCmd {
#define CMN5_QIC_GET_PROCESS_STATE	1
//};

typedef struct _CMN5_MULTI_COMMAND {
	ULONG				uCommand;
	ULONG				uBoard;
	ULONG				uChannel;
	union {
		ULONG			uParam[4];
		struct {
			ULONG		uParam0;
			ULONG		uParam1;
			ULONG		uParam2;
			ULONG		uParam3;
		};
	};
	BOOL				bRet;
} CMN5_MULTI_COMMAND;

//////////////////////////////////////////////////////////////////////////////////
// DataType
//enum DataType {
#define	CMN5_DT_VIDEO			1
#define	CMN5_DT_AUDIO			2
#define	CMN5_DT_COD				3
#define	CMN5_DT_SENSOR			4
#define	CMN5_DT_VSTATUS			5
#define	CMN5_DT_NET				6

#define	CMN5_DT_VIDEO_EX		0x11
#define	CMN5_DT_AUDIO_EX		0x12
#define	CMN5_DT_COD_EX			0x13
//#define	CMN5_DT_SENSOR_EX		0x14	// Reserved for future use
//#define	CMN5_DT_VSTATUS_EX		0x15	// Reserved for future use
#define	CMN5_DT_NET_EX			0x16

// Error code
//enum ErrorCode {
#define	CMN5_EC_NO_ERROR		0
#define	CMN5_EC_NO_DATA			1
#define	CMN5_EC_OVERFLOW		2
#define	CMN5_EC_ERROR			3
#define	CMN5_EC_FIRMWARE_ERROR	4
#define	CMN5_EC_FIRMWARE_DEAD	5

/*	
#define struct {	
	\ ULONG			uDataType;
	\ ULONG			uSize;
	\ ULONG			uErrCode;
	\ ULONG			uBoardNum;	// Board ID
	} DATA_INFO_HEADER
*/

// Abstract structure
typedef struct _CMN5_DATA_INFO_HEADER {
	ULONG			uDataType;
	ULONG			uSize;
	ULONG			uErrCode;
	
	ULONG			uBoardNum;				// Board ID
} CMN5_DATA_INFO_HEADER;

typedef struct _CMN5_QUERY_DATA_INFO{
	ULONG			uDataType;
	ULONG			uSize;
	ULONG			uErrCode;

	ULONG			uBoardNum;				// Board ID

	ULONG			reserved[16];
} CMN5_QUERY_DATA_INFO;

// Abstract structure
typedef struct _CMN5_DATA_INFO_DATA{
	ULONG			uDataType;
	ULONG			uStructSize;
	ULONG			uErrCode;
	
	ULONG			uBoardNum;				// Board ID
	ULONG			uChannelNum;			// Channel ID of the Board
	ULONG			uHasNextData;
	UCHAR			*pDataBuffer;			// data buffer address
	LARGE_INTEGER	TimeTag;				// KeQuerySystemTime()
	ULONG			uBufferIdx;				// internal use! do not touch!
	
} CMN5_DATA_INFO_DATA;

#define _CMN5_DATA_INFO_DATA_EX_CMM_BODY												\
	union {																				\
		ULONG		reservedCmn[32];														\
		struct {																		\
			ULONG			uDataType;													\
			ULONG			uStructSize;												\
			ULONG			uErrCode;													\
																						\
			ULONG			uBoardNum;				/* Board ID						*/	\
			ULONG			uChannelNum;			/* Channel ID of the Board		*/	\
			ULONG			uHasNextData;												\
			UCHAR			*pDataBuffer;			/* data buffer address			*/	\
			LARGE_INTEGER	TimeTag;				/* KeQuerySystemTime()			*/	\
			ULONG			uBufferIdx;				/* internal use! do not touch!	*/	\
		};																				\
	}

typedef struct _CMN5_DATA_INFO_DATA_EX {
	_CMN5_DATA_INFO_DATA_EX_CMM_BODY;
} CMN5_DATA_INFO_DATA_EX;

typedef struct _CMN5_VIDEO_STATUS_INFO {
	ULONG			uDataType;
	ULONG			uStructSize;
	ULONG			uErrCode;
	
	ULONG			uBoardNum;
	
	ULONG VideoStatus[CMN5_BOARD_MAX_CHANNEL_ARRAY_SIZE];
	ULONG VideoStatusMask[CMN5_BOARD_MAX_CHANNEL_ARRAY_SIZE];
} CMN5_VIDEO_STATUS_INFO;

typedef struct _CMN5_SENSOR_STATUS_INFO{
	ULONG			uDataType;
	ULONG			uStructSize;
	ULONG			uErrCode;
	
	ULONG			uBoardNum;										// Board ID
	ULONG			SensorStatus[CMN5_BOARD_MAX_DIO_ARRAY_SIZE];	//for sensor detect event..
	ULONG			SensorStatusMask[CMN5_BOARD_MAX_DIO_ARRAY_SIZE];
} CMN5_SENSOR_STATUS_INFO;

// Callback Prototype
//enum {
	// method
#define	CMN5_CAPTURE_METHOD_CALLBACK			0
//#define	CMN5_CAPTURE_METHOD_LOCK_AND_WAIT		1	// obsolete
#define	CMN5_CAPTURE_METHOD_QUERY				2

	// capture data type (We do not recommend that you use following names.)
#define	CMN5_CAPTURE_CALLBACK_ONCAPTURE			0x10	// legacy type
#define	CMN5_CAPTURE_CALLBACK_ONVIDEO			0x11	// legacy type
#define	CMN5_CAPTURE_CALLBACK_ONSENSOR			0x12	// legacy type
	// renamed (Use following names instead.)
#define	CMN5_CAPTURE_TYPE_ON_DATA				CMN5_CAPTURE_CALLBACK_ONCAPTURE	// Primary data of all 'Media'.
#define	CMN5_CAPTURE_TYPE_ON_CHANNEL			CMN5_CAPTURE_CALLBACK_ONVIDEO
#define	CMN5_CAPTURE_TYPE_ON_SENSOR				CMN5_CAPTURE_CALLBACK_ONSENSOR
//};

#define	CMN5_CAPTURE_TYPE_ON_DATA_EX			0x00010010
//#define	CMN5_CAPTURE_TYPE_ON_CHANNEL_EX			0x00010011	// Reserved for future use
//#define	CMN5_CAPTURE_TYPE_ON_SENSOR_EX			0x00010012	// Reserved for future use

extern "C"
{
BOOL CMN5_API Cmn5CreateInstance(void **pUnknown);

typedef BOOL (__stdcall *CMN5_CALLBACK_ONE_PARAM)(ULONG A);
typedef BOOL (__stdcall *CMN5_CALLBACK_TWO_PARAM)(ULONG A, ULONG B);
typedef BOOL (__stdcall *CMN5_CALLBACK_THREE_PARAM)(ULONG A, ULONG B, ULONG C);
}

#pragma pack( pop, Cmn5BoardLib_EX_H )

#endif // CMN5BOARDLIB_EX_H