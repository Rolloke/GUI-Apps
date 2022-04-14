// pt.h

#ifndef PT_H
#define PT_H

#include "wk.h"

//////////////////////////////////////////////////////////////////
#define HC_BASIC     0x0001
#define HC_IOMACROS  0x0002
#define HC_VM422     0x0004
#define HC_PHONE     0x0008

//////////////////////////////////////////////////////////////////
#define MAX_ALARMLINES	4
#define MAX_CAMERAS		3
#define MAX_RELAIS		16 // Tatsaechlich nur max. 3

//////////////////////////////////////////////////////////////////
#define CMD_GET_BOX_INFO	0x80
#define CMD_GET_CAM_STATE	0x84
#define CMD_SET_CAM_STATE	0x85
#define CMD_GET_BOX_CONFIG	0x86
#define CMD_SET_BOX_CONFIG	0x87
#define CMD_REMOTE_OUT		0x8C
#define CMD_REMOTE_IN		0x8D
#define CMD_GET_CAMERA		0x8E
#define CMD_SET_CAMERA		0x8F
#define CMD_GRAB_FRAME		0x90
#define CMD_EXIT			0x98
#define CMD_CLEAR_ALARM		0xA0
#define CMD_GET_ALARM_STATE	0xA4
#define CMD_CHECK_BOX_PWD	0xA8
#define CMD_CLEAR_ERROR		0xAF
#define CMD_SET_CLOCK		0x9B


//////////////////////////////////////////////////////////////////
#define ACK_GET_BOX_INFO	0xC0 
#define ACK_GET_BOX_CONFIG	0xC6 
#define ACK_SET_BOX_CONFIG	0xC7 
#define ACK_REMOTE_OUT		0xCC 
#define ACK_REMOTE_IN		0xCD 
#define ACK_GET_CAMERA		0xCE
#define ACK_CLEAR_ALARM		0xE0 
#define ACK_GET_ALARM_STATE	0xE4 
#define ACK_CHECK_BOX_PWD	0xE8 
#define ACK_CLEAR_ERROR		0xEF 

//////////////////////////////////////////////////////////////////
#define ACK_PICTURE			0xD3 
#define ACK_ERROR			0xFF 

//////////////////////////////////////////////////////////////////
#define DS_BOX_INFO			0 
#define DS_ALARM_ACTIONS	8 
#define DS_ALARM_LINES		9 
#define DS_CAMERAS			16 
#define DS_CAMERA_STATE		17 

//////////////////////////////////////////////////////////////////
#define CS_RECORDING_STOPPED	0x01 
#define CS_COLOR_SIGNAL			0x04 
#define CS_PAL_SIGNAL			0x08  /* Sonst SECAM/NTSC */
#define CS_GRABBER_VERSION		0xF0  /* Maske */
#define CS_GRABBER0				0x00  /* Version 0 */
#define CS_GRABBER1				0x10  /* Version 1: Farbe & DCT moeglich */

#define CF_HDELTA64				0x20  /* Intraframe */
#define CF_HDELTA64DIFF			0x60  /* Interframe */
             /* Sonstige Formate (DCT-basiert) von diesem
                Decoder nicht unterstuetzt */

#define ACI_ARMED   0x01 
#define ACI_ALARM   0x02 
#define ACI_ERROR	0x04 

//////////////////////////////////////////////////////////////////
#define GF_INTRA	0x01 
#define GF_STREAM	0x10 

//////////////////////////////////////////////////////////////////
#define MSG_SIZE_ERROR     0x31 
#define MSG_ADDRESS_ERROR  0x32 
#define MSG_CAMERA_ERROR   0x33 
#define MSG_PARAM_ERROR    0x34 
#define MSG_NOCOM_ERROR    0x35 
#define MSG_CHANNEL_ERROR  0x36 
#define MSG_END_OF_LIST    0x37 
#define MSG_ERROR		   0xFF  /* WORD ErrorCode, WORD ErrorParams follows */
//////////////////////////////////////////////////////////////////
#define CHANNEL_SIO        0x01 
#define CHANNEL_IO_PORT    0x08 
//////////////////////////////////////////////////////////////////
#define PORT_RELAIS        0x00CA 
#define PORT_TTL_OUT       0x0120 
#define PORT_TT_LIN        0x0121 
//////////////////////////////////////////////////////////////////
#define IOMODE_GET        0x00 
#define IOMODE_TEST_BIT   0x01 
#define IOMODE_MASK       0x02 
#define IOMODE_TEST_CLEAR 0x03 
//////////////////////////////////////////////////////////////////
#define IOMODE_PUT        0x00 
#define IOMODE_OR         0x01 
#define IOMODE_AND        0x02 
#define IOMODE_XOR        0x03 
//////////////////////////////////////////////////////////////////
#define CCT_RATE          0x02  /* Nicht Gesetzt: Oversampling an */
#define CCT_SMOOTH        0x04  /* Gesetzt: Pixel glaetten */
#define CCT2_DCT          0x01  /* Gesetzt: DCT-basierte Kompression (hier nicht unterstuetzt */
//////////////////////////////////////////////////////////////////
class CPTCmd 
{
public:
	CPTCmd(int iLen) 
	{
		m_pData = new BYTE[iLen];
		for (int i=0;i<iLen;i++) 
		{
			m_pData[i]=0;
		}
		m_iLen = iLen;
	}
	~CPTCmd() 
	{
		WK_DELETE_ARRAY(m_pData);
		m_iLen=0;
	}
protected:
	BYTE *m_pData;
	int m_iLen;
};

class CPTCmdCheckBoxPwd : public CPTCmd 
{
public:
	CPTCmdCheckBoxPwd(const CString &sAccessPwd,
					  const CString &sConfigPwd,
					  WORD wHostVersion,
					  WORD wHostCapabilities);
};
//////////////////////////////////////////////////////////////////
typedef struct
{
	BYTE Cmd;
	BYTE Len;
	BYTE ReplyParams[255];
} ptMsg;
//////////////////////////////////////////////////////////////////
typedef struct
{
	BYTE AccessPwdLen;
	BYTE AccessPwd[8]; /* Laengenbyte & 8 Zeichen */
	BYTE ConfigPwdLen;
	BYTE ConfigPwd[8];
	WORD HostVersion;
	WORD HostCapabilities;  /* HC_xxx */
} TCMDCheckBoxPwd;
//////////////////////////////////////////////////////////////////
typedef struct
{
	BYTE Index;
	BYTE HostID;
	BYTE Len;
	BYTE DSId;
} TCMDGetBoxConfig;
//////////////////////////////////////////////////////////////////
typedef struct
{
	BYTE Index;
	BYTE HostID;
	BYTE Len;
	BYTE DSId;
	BYTE data[255];
} TACKGetBoxConfig;
//////////////////////////////////////////////////////////////////
typedef struct
{
	BYTE Index;
	BYTE HostID;
	BYTE Len;
	BYTE DSId;
	BYTE data[255];
} TCMDSetBoxConfig;
//////////////////////////////////////////////////////////////////
typedef struct
{
    char CallerID[20];
} TCMDGetBoxInfo;
//////////////////////////////////////////////////////////////////
typedef struct
{
	char VersionNameNumber[70];
} TACKGetBoxInfo;
//////////////////////////////////////////////////////////////////
typedef struct
{
	BYTE Active; /* 0xFF=Kamera angeschlossen & aktiv, 0x00 Kein Videosignal */
	BYTE Flags;   /* CS_xxx */
} TCameraState;
//////////////////////////////////////////////////////////////////
typedef struct
{
	// offset 0
	WORD XSize;
	WORD YSize;
	// offset 4
	BYTE Saturation; /* 0..255 */
	BYTE MaxGrey;    /* Grau: 15, 31, 63, Farbe: 31+128, 63+128 */
	BYTE Contrast;   /* 0..255 */
	BYTE Brightness; /* 0..255 */
	BYTE Sensitivity; /* Min.Pixelaenderungen pro 8x8 Block */
	BYTE Threshold;   /* Min.Helligkeitsabweichung pro Pixel */
	BYTE Control;     /* CCT_xxx */
	BYTE Control2;    /* CCT2_xxx */
	// Offset 12
	WORD DCTyq12;     /* Nur DCT-Verfahren */
	WORD DCTuvq12;    /* " */
	WORD DestOfs; 
	BYTE DestPage;
	BYTE DestPages;
	BYTE DCTyq0;      /* Nur DCT-Verfahrend */
	BYTE DCTuvq0;     /* " */
	BYTE YSensitivity;/* " */
	BYTE UVSensitivity;/* " */
	WORD FirstLine;    /* <> 0: Teilbild */
	WORD FirstColumn;  /* <> 0: Teilbild */
	DWORD Reserved3_1;
	DWORD Reserved3_2;
	BYTE m_iNameLen;
	BYTE m_szName[20]; /* KameraName */
	BYTE IntraFrameSeconds;
	DWORD AutoGrabTime;  /* Autom. Aufzeichung */
	BYTE GrabFlags;      /* " */
	DWORD PostAlarmRecordTime; /* " */
	WORD FromTime, ToTime; /* " */
} TCameraInfo;
//////////////////////////////////////////////////////////////////
typedef struct
{
	BYTE ChannelOperations[255];
} TCMDRemoteOut;
//////////////////////////////////////////////////////////////////
typedef struct
{
	BYTE OptionalReply; /* Msg_xxx */
} TACKRemoteOut;
//////////////////////////////////////////////////////////////////
typedef struct
{
	BYTE ChannelOperations [255];
} TCMDRemoteIn;
//////////////////////////////////////////////////////////////////
typedef struct
{
	BYTE ChannelReplies [255];
} TACKRemoteIn;
//////////////////////////////////////////////////////////////////
typedef struct
{
	BYTE Format; /* CF_xxx */
} TACKPicture;
//////////////////////////////////////////////////////////////////
typedef struct
{
	BYTE Flags;
} TCMDGetAlarmState;
//////////////////////////////////////////////////////////////////
typedef struct
{
	WORD offset;
	WORD camerano;
	BYTE data [250];
} TCMDSetCamera;
//////////////////////////////////////////////////////////////////
typedef struct
{
	WORD offset;
	WORD size;
	WORD camerano;
} TCMDGetCamera;
//////////////////////////////////////////////////////////////////
typedef struct
{
	BYTE Control;
	BYTE VideoSource;
} TCMDGrabFrame;
//////////////////////////////////////////////////////////////////
typedef struct
{
	BYTE LatchedInputs;
	BYTE CurrentInputs;
	BYTE ControlInfo; /* Bitmasken: ACI_xxx */
	BYTE InvolvedCameras;
	/* Die folgenden Parameter sind nur vorhanden, wenn ControlInfo, Bit 2 gesetzt ist */
	WORD ErrorType;
	WORD ErrorParam;
	WORD ErrorPC;
} TACKGetAlarmState;
//////////////////////////////////////////////////////////////////
typedef struct
{
	BYTE NameLen;
	BYTE Name[12];
	WORD Port;
	BYTE BitNo;
} TRelaisInfo;
//////////////////////////////////////////////////////////////////
typedef struct
{
	BOOL Smooth;
	BOOL Oversampling;
	BYTE LuminanceBits;
	BYTE ChangesPer8x8;
	BYTE ValueChangeThreshold;
} TEncoderConfig;


typedef struct 
{
	BYTE Kameras;
    BYTE TriesNAction;
	BYTE Flags;
	BYTE NameLen;
	BYTE Name[12];
	DWORD AutoClearTime;
} TAlarmLine;

// BCD coded
// 45 seconds -> 4*16+5 = 0x45
typedef struct 
{
	BYTE Sec;
	BYTE Min;
	BYTE Hour;
	BYTE Day;
	BYTE Month;
	BYTE Year;
} TCMDSetTime;

#endif