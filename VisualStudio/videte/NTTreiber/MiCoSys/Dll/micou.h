//
// micou.h
//
// 32-bit Video Capture driver
// Data structures and function entry-points for Mico user-mode driver
//
// Julian Wolff
//

#ifndef _MICOU_H_
#define _MICOU_H_

//
// Include needed headers
//
#include <vcstruct.h>
#include <vcuser.h>
#include <mico.h>

//
// a 256-colour palette. There is no
// proper windows defn for this since the
// structure varies according to the number of colours
//
// The generic structure (without the full complement of paletteentries)
// is a LOGPALETTE.
// 
typedef struct _PAL256 {
    WORD palVersion;
    WORD palNumEntries;
    PALETTEENTRY palPalEntry[256];
} PAL256, *PPAL256;

//
// information needed to locate/initialise hardware. Set by user
// in ConfigDlgProc and written to registry to kernel driver.
// 
typedef struct _Config_Location {
    DWORD MemoryWindow;         // Memory window
    DWORD Port; 		// Port Number
    DWORD Interrupt;		// interrupt number
} CONFIG_LOCATION, *PCONFIG_LOCATION;

//
// This structure holds the information we need for a given
// kernel-driver instance (ie for one board).
//
typedef struct _BU_INFO {
    VCUSER_HANDLE vh;		// driver access handle
//
// Flag which channels are in use (we don't support the 'out' channel)
//
    BOOL ExtInOpen;
    BOOL ExtOutOpen;
    BOOL InOpen;
//
// Total usage count
//
    int DriverUsage;
    VCCALLBACK vcCallback;	// stream-init callback parameters
    PAL256 palCurrent;		// colours for current palette in CfgFormat.pXlate
    CONFIG_FORMAT CfgFormat;
    CONFIG_SOURCE CfgSource;
    BITMAPINFOHEADER biDest;	// same info as CfgFormat
    PVC_PROFILE_INFO pProfile;	// profile access for this driver's params

} BU_INFO, * PBU_INFO;

//
// DLL module handle
//
extern HANDLE ghModule;

//
// Name of the kernel-mode driver
//
#define DRIVERNAME_U   L"Mico"

//
// Functions in vmsg.c
//
LRESULT vidProcessMessage(DWORD, UINT, LPARAM, LPARAM);
LRESULT vidOpen(PVC_PROFILE_INFO, LPVIDEO_OPEN_PARMS);
LRESULT vidClose(DWORD);
DWORD SetDestFormat(LPBITMAPINFOHEADER lpbi, DWORD dwSize);

//
// Functions in install.c
//
LRESULT vidInstall(HWND hDlg, PCONFIG_LOCATION pLoc, PVC_PROFILE_INFO pProf);

//
// Functions in dialog.c 
//
VOID cfg_InitDefaults(PBU_INFO pBoardInfo);
int VideoFormatDlgProc(HWND hDlg, UINT msg, UINT wParam, LONG lParam);
int VideoSourceDlgProc(HWND hDlg, UINT msg, UINT wParam, LONG lParam);
int Config(HWND hWnd, HANDLE hModule, PVC_PROFILE_INFO pProfile);
VOID GetWidthHeight(DWORD dwWidth,LPDWORD lpdwWidth,LPDWORD lpdwHeight,VIDSTD VideoStd,BOOL bReturnTheoreticalMax);
BOOL vidSetDefaultPalette(PBU_INFO pBoardInfo);
VOID TransRecalcPal(HPALETTE hpal, LPBYTE pXlate);

#endif
