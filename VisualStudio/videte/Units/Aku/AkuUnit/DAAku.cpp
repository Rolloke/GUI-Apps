/////////////////////////////////////////////////////////////////////////////
// PROJECT:		AkuUnit
// FILE:		$Workfile: DAAku.cpp $
// ARCHIVE:		$Archive: /Project/Units/Aku/AkuUnit/DAAku.cpp $
// CHECKIN:		$Date: 5.08.04 13:54 $
// VERSION:		$Revision: 6 $
// LAST CHANGE:	$Modtime: 5.08.04 13:52 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <winioctl.h>
#include <conio.h>
#include "wk_trace.h"
#include "DAAku.h"
#include "AkuDef.h"
#include "..\AkuSys\AkuIoctl.h"

#pragma warning(disable : 4245)

HANDLE 	g_hDriver = INVALID_HANDLE_VALUE;
BOOL	g_bWin95  = TRUE;
BYTE	g_Latch8[LATCH_SIZE];
WORD	g_Latch16[LATCH_SIZE];

///////////////////////////////////////////////////////////////////////////////////
BOOL OpenDevice()
{
	// Gibt Auskunft darüber ob Windows95 oder Windows NT läuft.
	OSVERSIONINFO VersionInformation;
	VersionInformation.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&VersionInformation);

	// MiCo-Objekt anlegen
	if (VersionInformation.dwPlatformId == VER_PLATFORM_WIN32_NT)
		g_bWin95 = FALSE;	
	else
		g_bWin95 = TRUE;	

	if (g_bWin95)
		return TRUE;

	// WinNT benötigt Aku.sys
	g_hDriver = CreateFile(
                _T("\\\\.\\AkuDev"),                    // Open the Device "file"
                GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ,
                NULL,
                OPEN_EXISTING,
                0,
                NULL
                );
                
    if (g_hDriver == INVALID_HANDLE_VALUE)        // Was the device opened?
    {
        WK_TRACE_ERROR(_T("OpenDevice failed\n"));
		return FALSE;
    }

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CloseDevice()
{
	if (g_bWin95)
		return TRUE;

    if (g_hDriver != INVALID_HANDLE_VALUE)
	{
		if (!CloseHandle(g_hDriver))                  // Close the Device "file".
		{
			WK_TRACE_ERROR(_T("CloseDevice failed\n"));
			return FALSE;
		}
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void DAWritePort8(DWORD dwAddr, BYTE byVal, BOOL bLatch)
{
	DWORD		dwReturned = 0;
	IOSTRUCT	Io;

	if (dwAddr == 0)
		return;

	if (g_bWin95)
		_outp((WORD)dwAddr, byVal);
	else
	{
		if (g_hDriver == INVALID_HANDLE_VALUE)
			return;

		Io.bLatch	= bLatch;
		Io.dwAddr	= dwAddr;
		Io.byVal	= byVal;
		Io.wIOSize	= IO_SIZE8;

		DeviceIoControl(g_hDriver,
						AKU_SYS_WRITE_PORT,
						&Io, sizeof(IOSTRUCT),
						NULL, 0,
						&dwReturned, NULL);
	}

	if (bLatch)
	{
		if (dwAddr <= LATCH_SIZE)
			g_Latch8[dwAddr] = byVal;	
	}
}																								

/////////////////////////////////////////////////////////////////////////////
void DAWritePort16(DWORD dwAddr, WORD wVal, BOOL bLatch)
{
	DWORD		dwReturned = 0;
	IOSTRUCT	Io;

	if (dwAddr == 0)
		return;

	if (g_bWin95)
		_outpw((WORD)dwAddr, wVal);
	else
	{
		if (g_hDriver == INVALID_HANDLE_VALUE)
			return;

		Io.bLatch	= bLatch;
		Io.dwAddr	= dwAddr;
		Io.wVal		= wVal;
		Io.wIOSize	= IO_SIZE16;

		DeviceIoControl(g_hDriver,
						AKU_SYS_WRITE_PORT,
						&Io, sizeof(IOSTRUCT),
						NULL, 0,
						&dwReturned, NULL);
	}

	if (bLatch)
	{
		if (dwAddr <= LATCH_SIZE)
			g_Latch16[dwAddr] = wVal;	
	}
}

/////////////////////////////////////////////////////////////////////////////
BYTE DAReadPort8(DWORD dwAddr, BOOL bLatch)
{
	DWORD		dwReturned	= 0;
	IOSTRUCT	Io;

	if (dwAddr == 0)
		return 0;

	if (bLatch)
	{
		if (dwAddr <= LATCH_SIZE)
			return g_Latch8[dwAddr];
		else
			return 0;
	}

	if (g_bWin95)
	{
		return (BYTE)_inp((WORD)dwAddr);
	}
	else
	{
		if (g_hDriver == INVALID_HANDLE_VALUE)
			return 0;

		Io.bLatch	= bLatch;
		Io.dwAddr	= dwAddr;
		Io.wIOSize	= IO_SIZE8;
		Io.byVal	= 0;

		DeviceIoControl(g_hDriver,
						AKU_SYS_READ_PORT,
						&Io, sizeof(IOSTRUCT),
						&Io, sizeof(IOSTRUCT),
						&dwReturned,	NULL);

		return Io.byVal;
	}
}

/////////////////////////////////////////////////////////////////////////////
WORD DAReadPort16(DWORD dwAddr, BOOL bLatch)
{
	DWORD		dwReturned	= 0;
	IOSTRUCT	Io;

	if (dwAddr == 0)
		return 0;

	if (bLatch)
	{
		if (dwAddr <= LATCH_SIZE)
			return g_Latch16[dwAddr];
		else
			return 0;
	}

	if (g_bWin95)
	{
		return (WORD)_inpw((WORD)dwAddr);
	}
	else
	{
		if (g_hDriver == INVALID_HANDLE_VALUE)
			return 0;

		Io.bLatch	= bLatch;
		Io.dwAddr	= dwAddr;
		Io.wIOSize	= IO_SIZE16;
		Io.wVal	= 0;

		DeviceIoControl(g_hDriver,
						AKU_SYS_READ_PORT,
						&Io, sizeof(IOSTRUCT),
						&Io, sizeof(IOSTRUCT),
						&dwReturned,	NULL);

		return Io.wVal;
	}
}


