// Packet.h: interface for the CPacket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PACKET_H__B9137BCA_93F8_11D2_A9C3_004005A11E32__INCLUDED_)
#define AFX_PACKET_H__B9137BCA_93F8_11D2_A9C3_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define PACKET_HEADER_SIZE		22

#define GLI(c,d)	(int)(c+d)	// GetLastIndex

// Offset- und Längendefinitionen des Packetheaders
#define PTHEADER_CHECKSUM		0,	2
#define PTHEADER_PACKETLENGTH	2,	5
#define PTHEADER_PACKETID		7,	1
#define PTHEADER_PACKETCOUNTER	8,	2
#define PTHEADER_APPLICATIONID	10, 8
#define PTHEADER_PACKETTYPE		18, 2

// Offset- und Längendefinitionen des Packettyps 00
#define PT00_STATUSCODE			PACKET_HEADER_SIZE + 0, 2
#define PT00_MAXERRORS			PACKET_HEADER_SIZE + 3, 2
#define PT00_TIMEOUT			PACKET_HEADER_SIZE + 6, 2
#define PT00_WORKERPORT			PACKET_HEADER_SIZE + 9, 5

// Offset- und Längendefinitionen des Packettyps 01
#define PT01_KAMERAZAHL			PACKET_HEADER_SIZE + 0, 2	
#define PT01_KAMERAONOFF(c)		PACKET_HEADER_SIZE + c*17+2,  1	
#define PT01_TOP(c)				PACKET_HEADER_SIZE + c*17+3,  4				
#define PT01_LEFT(c)			PACKET_HEADER_SIZE + c*17+7,  4				
#define PT01_WIDTH(c)			PACKET_HEADER_SIZE + c*17+11, 4				
#define PT01_HEIGHT(c)			PACKET_HEADER_SIZE + c*17+15, 4				

// Offset- und Längendefinitionen des Packettyps 02
#define PT02_FILENAME			PACKET_HEADER_SIZE + 0,		128
#define PT02_CONTAINERPREFIX	PACKET_HEADER_SIZE + 128,	4
#define PT02_CONTAINERNUMBER	PACKET_HEADER_SIZE + 132,	10
#define PT02_ISOCODE			PACKET_HEADER_SIZE + 142,	4
#define PT02_IMDGCODE1			PACKET_HEADER_SIZE + 146,	3
#define PT02_IMDGCODE2			PACKET_HEADER_SIZE + 149,	3
#define PT02_SPECIAL_ID			PACKET_HEADER_SIZE + 152,	2
#define PT02_RUFZEICHEN			PACKET_HEADER_SIZE + 154,	8
#define PT02_REISENUMMER		PACKET_HEADER_SIZE + 162,	8
#define PT02_REEDER				PACKET_HEADER_SIZE + 170,	20
#define PT02_SIEGELNUMMER		PACKET_HEADER_SIZE + 190,	8
#define PT02_DIRECTION			PACKET_HEADER_SIZE + 198,	1	
#define PT02_DAMAGECODE			PACKET_HEADER_SIZE + 199,	32
#define PT02_DATE				PACKET_HEADER_SIZE + 231,	8
#define PT02_TIME				PACKET_HEADER_SIZE + 239,	6
  
// Offset- und Längendefinitionen des Packettyps 03
#define PT03_CAMERA(c)			PACKET_HEADER_SIZE + c*4+0, 2
#define PT03_ACTION(c)			PACKET_HEADER_SIZE + c*4+2, 1
#define PT03_VIEW(c)			PACKET_HEADER_SIZE + c*4+3, 1

// Offset- und Längendefinitionen des Packettyps 04
#define PT04_KAMERAZAHL			PACKET_HEADER_SIZE + 0,	2
#define PT04_KAMERA				PACKET_HEADER_SIZE + 2,	2
#define PT04_BRIGHTNESS			PACKET_HEADER_SIZE + 4,	2
#define PT04_CONTRAST			PACKET_HEADER_SIZE + 6,	2
#define PT04_SATURATION			PACKET_HEADER_SIZE + 8,	2
#define PT04_VIEWTYPE			PACKET_HEADER_SIZE + 10,1

// Offset- und Längendefinitionen des Packettyps 05
#define PT05_STATUSCODE			PACKET_HEADER_SIZE + 0,	2
#define PT05_KAMERA				PACKET_HEADER_SIZE + 2,	2

class CPacket  
{
public:
	CPacket();
	virtual ~CPacket();

	void SetPacketMsg(const CStringA &sMsg);
	CStringA GetPacketMsg();

	// Telegramm-Header
	WORD		GetPacketCheckSum();				// Ok
	DWORD		GetPacketLength();					// Ok
	CStringA	GetPacketID();						// Ok
	WORD		GetPacketCounter();					// Ok
	CStringA	GetApplicationID();					// Ok
	WORD		GetPacketType();					// Ok

	// Telegrammtype00
	WORD	GetType00StatusCode();				// Ok
	WORD	GetType00MaxErrors();				// Ok
	WORD	GetType00TimeOut();					// Ok
	DWORD	GetType00WorkerPort();				// Ok

	// Telegrammtype01
	WORD	GetType01MaxCam();					// Ok
	BOOL	GetType01CamOnOff(WORD wCamera);	// Ok
	CRect	GetType01WndPos(WORD wCamera);		// Ok

	// Telegrammtype02
	CStringA GetType02FileName();				// Ok
	CStringA GetType02ContainerPrefix();			// Ok
	CStringA GetType02ContainerNumber();			// Ok
	CStringA GetType02ISOCode();					// Ok
	CStringA GetType02IMDGCode1();				// Ok
	CStringA GetType02IMDGCode2();				// Ok
	CStringA GetType02SpecialID();				// Ok
	CStringA GetType02Rufzeichen();				// Ok
	CStringA GetType02ReiseNummer();				// Ok
	CStringA GetType02Reeder();					// Ok
	CStringA GetType02SiegelNummer();			// Ok
	CStringA GetType02Direction();				// Ok
	CStringA GetType02DamageCode();				// Ok
	CStringA GetType02Date();					// Ok
	CStringA GetType02Time();					// Ok

	// Telegrammtype03
	WORD	GetType03Camera(WORD wIndex);		// Ok
	int		GetType03Action(WORD wIndex);		// Ok
	BOOL	GetType03Minimized(WORD wIndex);	// Ok
					 
	// Telegrammtype04
	WORD	GetType04MaxCam();					// Ok
	WORD	GetType04Camera();					// Ok
	WORD	GetType04Brightness();				// Ok
	WORD	GetType04Contrast();				// Ok
	WORD	GetType04Saturation();				// Ok
	BOOL	GetType04IsZoomed();
							
	// Telegrammtype05
	WORD	GetType05Camera();
	WORD	GetType05StatusCode();

private:
	WORD	 AsciiToInt(WORD wI, WORD wSize);
	CStringA ExtractSubString(WORD wI, WORD wSize);

	CStringA m_sPacket;
};

#endif // !defined(AFX_PACKET_H__B9137BCA_93F8_11D2_A9C3_004005A11E32__INCLUDED_)
