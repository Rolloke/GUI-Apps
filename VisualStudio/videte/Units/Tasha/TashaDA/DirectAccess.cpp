/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaDA
// FILE:		$Workfile: DirectAccess.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaDA/DirectAccess.cpp $
// CHECKIN:		$Date: 13.04.04 10:06 $
// VERSION:		$Revision: 7 $
// LAST CHANGE:	$Modtime: 13.04.04 10:03 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\TashaSys\TashaIoctl.h"
#include "DirectAccess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern HANDLE	g_hDriver;
extern BOOL		g_bWin95;

#pragma warning (disable : 4245) // disable cast conversion warning

/////////////////////////////////////////////////////////////////////////////
BOOL DAIsValid()
{
	return (g_hDriver != INVALID_HANDLE_VALUE);
}

/////////////////////////////////////////////////////////////////////////////
DWORD DAGetDriverVersion()
{
	DWORD dwDriverVersion	= 0;
	DWORD dwReturned		= 0;

	if (!DAIsValid())
		return 0;

	//Versionsnummer des Gerätetreibers erfragen
	if (!DeviceIoControl(g_hDriver, TASHA_GET_DRIVERVERSION, NULL, 0, &dwDriverVersion, sizeof(DWORD), &dwReturned, NULL))
		return 0;

	return dwDriverVersion;
}

/////////////////////////////////////////////////////////////////////////////
DWORD DAGetTashaDAVersion()
{
	return TASHA_UNIT_VERSION;
}

/////////////////////////////////////////////////////////////////////////////
BOOL DAGetFramebuffer(int nCardIndex, MEMSTRUCT& mem)
{
	BOOL bResult = FALSE;
	DWORD		dwReturned = 0;

	if (!DAIsValid()) 
		return FALSE;

	FRAMEBUFFERSTRUCT FrameBuffer;
	FrameBuffer.nCardIndex		= nCardIndex;
	FrameBuffer.Buffer.dwLen	= 0;
	FrameBuffer.Buffer.pLinAddr = NULL;
	FrameBuffer.Buffer.pPhysAddr= NULL;

	// Physikalische Adresse in lineare Adresse umwandeln
	if (DeviceIoControl(g_hDriver, TASHA_GET_FRAME_BUFFER, &FrameBuffer, sizeof(FrameBuffer), &FrameBuffer, sizeof(FrameBuffer), &dwReturned, NULL))
	{
		mem.dwLen		= FrameBuffer.Buffer.dwLen;
		mem.pLinAddr	= FrameBuffer.Buffer.pLinAddr;
		mem.pPhysAddr	= FrameBuffer.Buffer.pPhysAddr;
		bResult = TRUE;
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL DAGetTransferbuffer(int nCardIndex, MEMSTRUCT& mem)
{
	BOOL bResult = FALSE;
	DWORD		dwReturned = 0;

	if (!DAIsValid()) 
		return FALSE;

	FRAMEBUFFERSTRUCT FrameBuffer;
	FrameBuffer.nCardIndex		= nCardIndex;
	FrameBuffer.Buffer.dwLen	= 0;
	FrameBuffer.Buffer.pLinAddr = NULL;
	FrameBuffer.Buffer.pPhysAddr= NULL;

	// Physikalische Adresse in lineare Adresse umwandeln
	if (DeviceIoControl(g_hDriver, TASHA_GET_TRANSFER_BUFFER, &FrameBuffer, sizeof(FrameBuffer), &FrameBuffer, sizeof(FrameBuffer), &dwReturned, NULL))
	{
		mem.dwLen		= FrameBuffer.Buffer.dwLen;
		mem.pLinAddr	= FrameBuffer.Buffer.pLinAddr;
		mem.pPhysAddr	= FrameBuffer.Buffer.pPhysAddr;
		bResult = TRUE;
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL DAWritePort8(DWORD dwAddr, BYTE byVal)
{
	DWORD		dwReturned = 0;
	IOSTRUCT	Io;

	if (!DAIsValid())
		return FALSE;

	Io.dwAddr		= dwAddr;
	Io.byVal		= byVal;
	Io.wIOSize		= IO_SIZE8;

	if (!DeviceIoControl(g_hDriver, TASHA_WRITE_PORT, &Io, sizeof(IOSTRUCT), NULL, 0, &dwReturned, NULL))
		return FALSE;

	return TRUE;
}																								

/////////////////////////////////////////////////////////////////////////////
BOOL DAWritePort16(DWORD dwAddr, WORD wVal)
{
	DWORD		dwReturned = 0;
	IOSTRUCT	Io;

	if (!DAIsValid())
		return FALSE;

	Io.dwAddr		= dwAddr;
	Io.wVal			= wVal;
	Io.wIOSize		= IO_SIZE16;

	if (!DeviceIoControl(g_hDriver, TASHA_WRITE_PORT, &Io, sizeof(IOSTRUCT), NULL, 0, &dwReturned, NULL))
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL DAWritePort32(DWORD dwAddr, DWORD dwVal)
{
	DWORD		dwReturned = 0;
	IOSTRUCT	Io;

	if (!DAIsValid())
		return FALSE;

	Io.dwAddr		= dwAddr;
	Io.dwVal		= dwVal;
	Io.wIOSize		= IO_SIZE32;

	if (!DeviceIoControl(g_hDriver, TASHA_WRITE_PORT, &Io, sizeof(IOSTRUCT), NULL, 0, &dwReturned, NULL))
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BYTE DAReadPort8(DWORD dwAddr)
{
	DWORD		dwReturned	= 0;
	IOSTRUCT	Io;

	if (!DAIsValid())
		return 0;

	Io.dwAddr		= dwAddr;
	Io.wIOSize		= IO_SIZE8;
	Io.byVal		= 0;

	DeviceIoControl(g_hDriver, TASHA_READ_PORT, &Io, sizeof(IOSTRUCT), &Io, sizeof(IOSTRUCT), &dwReturned,	NULL);

	return Io.byVal;
}

/////////////////////////////////////////////////////////////////////////////
WORD DAReadPort16(DWORD dwAddr)
{
	DWORD		dwReturned	= 0;
	IOSTRUCT	Io;

	if (!DAIsValid())
		return 0;

	Io.dwAddr		= dwAddr;
	Io.wIOSize		= IO_SIZE16;
	Io.wVal			= 0;

	DeviceIoControl(g_hDriver, TASHA_READ_PORT, &Io, sizeof(IOSTRUCT), &Io, sizeof(IOSTRUCT), &dwReturned,	NULL);

	return Io.wVal;
}

/////////////////////////////////////////////////////////////////////////////
DWORD DAReadPort32(DWORD dwAddr)
{
	DWORD		dwReturned	= 0;
	IOSTRUCT	Io;

	if (!DAIsValid())
		return 0;

	Io.dwAddr		= dwAddr;
	Io.wIOSize		= IO_SIZE32;
	Io.dwVal		= 0;

	DeviceIoControl(g_hDriver, TASHA_READ_PORT, &Io, sizeof(IOSTRUCT), &Io, sizeof(IOSTRUCT), &dwReturned,	NULL);

	return Io.dwVal;
}

/////////////////////////////////////////////////////////////////////////////
BOOL DAWriteMemory8(DWORD dwAddr, BYTE byVal)
{
	DWORD		dwReturned = 0;
	IOSTRUCT	Io;

	if (!DAIsValid())
		return FALSE;

	Io.dwAddr		= dwAddr;
	Io.byVal		= byVal;
	Io.wIOSize		= IO_SIZE8;

	if (!DeviceIoControl(g_hDriver, TASHA_WRITE_MEMORY, &Io, sizeof(IOSTRUCT), NULL, 0, &dwReturned, NULL))
		return FALSE;

	return TRUE;
}																								

/////////////////////////////////////////////////////////////////////////////
BOOL DAWriteMemory16(DWORD dwAddr, WORD wVal)
{
	DWORD		dwReturned = 0;
	IOSTRUCT	Io;

	if (!DAIsValid())
		return FALSE;

	Io.dwAddr		= dwAddr;
	Io.wVal			= wVal;
	Io.wIOSize		= IO_SIZE16;

	if (!DeviceIoControl(g_hDriver, TASHA_WRITE_MEMORY, &Io, sizeof(IOSTRUCT), NULL, 0, &dwReturned, NULL))
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL DAWriteMemory32(DWORD dwAddr, DWORD dwVal)
{
	DWORD		dwReturned = 0;
	IOSTRUCT	Io;

	if (!DAIsValid())
		return FALSE;

	Io.dwAddr		= dwAddr;
	Io.dwVal		= dwVal;
	Io.wIOSize		= IO_SIZE32;

	if (!DeviceIoControl(g_hDriver, TASHA_WRITE_MEMORY, &Io, sizeof(IOSTRUCT), NULL, 0, &dwReturned, NULL))
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL DAWriteMemoryX(void* pSrcBuffer, DWORD dwAddr, DWORD dwLen)
{
	DWORD		dwReturned	= 0;
	IOSTRUCT	Io;

	if (!DAIsValid())
		return 0;

	Io.dwAddr		= dwAddr;
	Io.wIOSize		= IO_SIZEX;
	Io.pBuffer		= pSrcBuffer;
	Io.dwLen		= dwLen;

	if (!DeviceIoControl(g_hDriver, TASHA_WRITE_MEMORY, &Io, sizeof(IOSTRUCT), NULL, 0, &dwReturned,	NULL))
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BYTE DAReadMemory8(DWORD dwAddr)
{
	DWORD		dwReturned	= 0;
	IOSTRUCT	Io;

	if (!DAIsValid())
		return 0;

	Io.dwAddr		= dwAddr;
	Io.wIOSize		= IO_SIZE8;
	Io.byVal		= 0;

	DeviceIoControl(g_hDriver, TASHA_READ_MEMORY, &Io, sizeof(IOSTRUCT), &Io, sizeof(IOSTRUCT), &dwReturned,	NULL);

	return Io.byVal;
}

/////////////////////////////////////////////////////////////////////////////
WORD DAReadMemory16(DWORD dwAddr)
{
	DWORD		dwReturned	= 0;
	IOSTRUCT	Io;

	if (!DAIsValid())
		return 0;

	Io.dwAddr		= dwAddr;
	Io.wIOSize		= IO_SIZE16;
	Io.wVal			= 0;

	DeviceIoControl(g_hDriver, TASHA_READ_MEMORY, &Io, sizeof(IOSTRUCT), &Io, sizeof(IOSTRUCT), &dwReturned,	NULL);

	return Io.wVal;
}

/////////////////////////////////////////////////////////////////////////////
DWORD DAReadMemory32(DWORD dwAddr)
{
	DWORD		dwReturned	= 0;
	IOSTRUCT	Io;

	if (!DAIsValid())
		return 0;

	Io.dwAddr		= dwAddr;
	Io.wIOSize		= IO_SIZE32;
	Io.dwVal		= 0;

	DeviceIoControl(g_hDriver, TASHA_READ_MEMORY, &Io, sizeof(IOSTRUCT), &Io, sizeof(IOSTRUCT), &dwReturned,	NULL);

	return Io.dwVal;
}

/////////////////////////////////////////////////////////////////////////////
BOOL DAReadMemoryX(void* pDestBuffer, DWORD dwAddr, DWORD dwLen)
{
	DWORD		dwReturned	= 0;
	IOSTRUCT	Io;

	if (!DAIsValid())
		return 0;

	Io.dwAddr		= dwAddr;
	Io.wIOSize		= IO_SIZEX;
	Io.pBuffer		= pDestBuffer;
	Io.dwLen		= dwLen;

	if (!DeviceIoControl(g_hDriver, TASHA_READ_MEMORY, &Io, sizeof(IOSTRUCT), &Io, sizeof(IOSTRUCT), &dwReturned,	NULL))
		return FALSE;

	return TRUE;
}
