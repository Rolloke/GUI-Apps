#ifndef __MICOVXD_H__
#ifndef WM_USER
#define WM_USER             0x0400
#endif
#include "wk_msg.h"

// Diese Versionsnummer wird in der MiCoUnit abgeprüft.
#define MICO_VXD_VERSION	0x352

// Größe des Latches
#define LATCH_SIZE			2048

// Definition auch im Project MiCoUnit: CMico.h
#define	MICO_SOURCE0				0
#define	MICO_SOURCE1				1
#define	MICO_SOURCE2				2
#define	MICO_SOURCE3				3
#define	MICO_SOURCE4				4
#define	MICO_SOURCE5				5
#define	MICO_SOURCE6				6
#define	MICO_SOURCE7				7
#define MICO_MAX_SOURCE				MICO_SOURCE7

// Erweiterungskarten  (Müssen von 0 an aufsteigend nummeriert sein!!)
#define MICO_EXTCARD0				0
#define MICO_EXTCARD1				1
#define MICO_EXTCARD2				2
#define MICO_EXTCARD3				3
#define MICO_MAX_EXTCARD			MICO_EXTCARD3

#define MICO_MAX_CAM	MICO_MAX_EXTCARD * (MICO_MAX_SOURCE + 1)

// Mico Frontends
#define MICO_FE0					0
#define MICO_FE1					1
#define MICO_MAX_FE					2

#define WM_MICO_ALARM					MICO_MSGBASE + 0
#define WM_MICO_VXD_REKURSION			MICO_MSGBASE + 1
#define WM_MICO_VXD_LOST_FIELD			MICO_MSGBASE + 2
#define WM_MICO_VXD_UNVALID_FIELD		MICO_MSGBASE + 3
#define WM_MICO_VXD_UNVALID_BUFFER		MICO_MSGBASE + 4
#define WM_MICO_VIDEO_STATE_CHANGE		MICO_MSGBASE + 5
#define WM_MICO_VXD_UNVALID_FRAMELEN	MICO_MSGBASE + 6

#define EVEN_FIELD			0
#define ODD_FIELD			1

#define MAX_JPEG_BUFFER		32
#define JPEG_BUFFER_SIZE	65535

typedef DIOCPARAMETERS *LPDIOC;
typedef int		HWND;

// OOPS wird seit der Neuinstallation von MSDEV 4.2 auf die lokale Platte nicht mehr
// gefunden, deshalb wird sie hier definiert.
typedef long HRESULT;

typedef struct
{
	DWORD	dwProzessID;				// Eindeutige ProzeßID
	WORD	wSource;					// Videoquelle zu den die Bilddaten gehören
	WORD	wExtCard;					// CoVi zu den die Bilddaten gehören

	char*	lpBuffer;					// Adresse an der die Bilddaten liegen
	DWORD	dwLen;						// Länge der Bilddaten.
	
	DWORD	dwTime;						// YYYYYYMMMMDDDDDhhhhhmmmmmmssssss
	DWORD	dwFieldID;					// FieldCounter
	WORD	wField;						// 0->Even Field 1->Odd Field
	
	BOOL	bValid;						// TRUE -> Bildaten können abgeholt werden
	BYTE	byFCNT;						// Zoran FieldCounter (For Debug only)
	BYTE	byPage;						// Zoran Page		  (For Debug only)
} JPEG;

typedef struct
{
	WORD	wIRQ;						// Interruptnummer des MiCo's
	WORD	wIOBase[MICO_MAX_EXTCARD+1];// Basisadresse des Mico und der CoVi's
	HWND	hWnd;						// Fensterhandle der MicoUnit			
	DWORD	hEvent;						// Ring0 Eventhandle
	WORD	wCSVDIN;					// Dient zum Auswählen des VideoFrontEnds 

	DWORD	dwReqProzessID;				// Eindeutige ProzeßID des neuen Jobs
	WORD	wReqSource;					// Quelle auf die umgeschaltet werden soll.
	WORD	wReqExtCard;				// CoVi auf die umgeschaltet werden soll

	DWORD	dwProzessID;				// Eindeutige ProzeßID des laufenden Jobs
	WORD	wSource;					// Aktuelle Videoquelle
	WORD	wExtCard;					// Aktuelle CoVi

	WORD	wSkipFields;				// Anzahl der Fields, die nach einer Umschaltung gedroppt werden.
	BOOL	bHardwareSkip;				// TRUE = Hardwareskip ein
	BOOL	bSwitchDelay;				// TRUE->20ms Delay vor digitaler Umschaltung

	WORD	wBPF;						// Sollgröße eines Jpegbildes
	WORD	wField;						// Legt fest welches Field genommen wird.
	WORD	wMinLen;					// Mindestgröße eines Jpegbildes
	WORD	wMaxLen;					// Maximalgröße eines Jpegbildes
	BOOL	bSwitch;					// TRUE->Kamera umschalten

	JPEG	Jpeg[MAX_JPEG_BUFFER];		// JPEG-Strukturen
} CONFIG;

/////////////////////////////////////////////////////////////////////////////////////
VXDINLINE BOOL __declspec (naked) _SHELL_PostMessage(DWORD hWnd, DWORD uMsg, DWORD wParam, DWORD lParam, PFN pfnCallback, DWORD dwRefData)
{
	VxDJmp(_SHELL_PostMessage);
}

DWORD _stdcall MICO_VXD_W32_DeviceIOControl(DWORD, DWORD, DWORD, LPDIOC);
DWORD _stdcall MICO_VXD_CleanUp(void);
DWORD _stdcall MICO_VXD_W32_Open(DWORD, DWORD, LPDIOC);
DWORD _stdcall MICO_VXD_W32_Close(DWORD, DWORD, LPDIOC);
DWORD _stdcall MICO_VXD_W32_GetVersion(DWORD, DWORD, LPDIOC);

void  _stdcall ServiceMICOBdInt(void);

void*  VirtualizeAdress(void *pAdr, DWORD dwLen);
void   UnvirtualizeAdress(void* pAdr, DWORD dwLen);
BOOL   PostMessage(HWND hW, DWORD uM, DWORD wP, DWORD lP);
BOOL   InstallISR(WORD wIRQ);
void   RemoveISR();
BOOL   ServiceZoranInt(WORD wField);
void   ServiceSourceSelection();
void   SetBPF(WORD wBPF);
DWORD  GetCurrentTime();
HEVENT FieldReadyEventCallback();
HEVENT PostMessageEventCallback();
BOOL   SetEvent(DWORD hEvent);
BOOL   CloseVxDHandle(DWORD hEvent);
void   Output(WORD wAdr, BYTE byVal);
void   Outputw(WORD wAdr, WORD wVal);
BYTE   Input(WORD wAdr);
WORD   Inputw(WORD wAdr);

#endif // __MICOVXD_H__