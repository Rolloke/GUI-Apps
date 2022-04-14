#ifndef __REMOTECONTROLDEF_H__
#define __REMOTECONTROLDEF_H__

#define TYP00	0x00
#define TYP01	0x01
#define TYP02	0x02
#define TYP03	0x03
#define TYP04	0x04
#define TYP05	0x05

#define STATUS_CODE00	0x00
#define STATUS_CODE01	0x01
#define STATUS_CODE02	0x02
#define STATUS_CODE98	0x98
#define STATUS_CODE99	0x99

#define CAMERA_DEFECT		01
#define CAMERA_OK			02
#define CHECK_CAMERA		03
#define CHECK_ALL_CAMERAS	99

// Messages innerghalb des Servers.
#define WM_SOCKET_CLOSED			WM_USER + 40
#define WM_RTE_VIDO_MISSING			WM_USER + 41

// Maximale Anzahl der Miniclients
#define MAX_CLIENTS 32 

#define INIFILENAME _T("CAMERA.INI")

typedef struct
{
	WORD	wMaxCameras;		// Anzahl der verfügbareb Kameras
	WORD	wCamera;			// Kamera für die die Daten gelten (1...40)
	BOOL	bCameraOnline;		// TRUE Kamera ist zugeschaltet
	CRect	rcWndPos;			// Fensterposition
}CLIENT_STRUCT;

#define REQ_ALL_PARAM		1
#define REQ_SINGLE_PARAM	2
#define SET_SINGLE_PARAM	3

typedef struct
{
	WORD	wCamera;			// Anzahl der verfügbareb Kameras
	WORD	wBrightness;		// Helligkeit
	WORD	wContrast;			// Kontrast
	WORD	wSaturation;		// Farbsättigung
	BOOL	bIsZoomed;			// TRUE -> Zoom ist an
	int		nRequestTyp;		// REQ_ALL_PARAM, REQ_SINGLE_PARAM, SET_SINGLE_PARAM
}CAMERA_PARAM_STRUCT;

typedef struct
{
	CStringA	sFileName;
	CStringA	sContainerPrefix;
	CStringA	sContainerNumber;
	CStringA	sISOCode;
	CStringA	sIMDGCode1;
	CStringA	sIMDGCode2;
	CStringA	sSpecialID;
	CStringA	sRufzeichen;
	CStringA	sReiseNummer;
	CStringA	sReeder;
	CStringA	sSiegelNummer;
	CStringA	sRichtung;
	CStringA	sDamageCode; 
	CStringA	sDate;
	CStringA	sTime;
}INDEX_STRUCT;

typedef struct
{
	WORD	wCamera;
	int		nAction;	// PT03_FREEZE, PT03_SHOW, PT03_REFRESH
	BOOL	bMiniMize;	// TRUE -> Minimize
}
CAMERA_SET_STRUCT;

typedef struct
{
	DWORD	dwConnectConnectPort;
	DWORD	dwConnectWorkPort;
	DWORD	dwGetWorkPort;
	DWORD	dwNoTraffic;
	DWORD	dwNoResponseOnReq;
}TIMEOUT_STRUCT;


#endif // __REMOTECONTROLDEF_H__