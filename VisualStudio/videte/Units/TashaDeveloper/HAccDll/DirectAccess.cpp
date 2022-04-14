/////////////////////////////////////////////////////////////////////////////
// PROJECT:		HAccDll
// FILE:		$Workfile: DirectAccess.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha(Developer)/HAccDll/DirectAccess.cpp $
// CHECKIN:		$Date: 5.01.04 9:52 $
// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 5.01.04 9:25 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\HAccSys\HAccIoctl.h"
#include "DirectAccess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern HANDLE	g_hDriver;
extern BOOL		g_bWin95;

#pragma warning (disable : 4245) // disable cast conversion warning

/////////////////////////////////////////////////////////////////////////////
BOOL HACCIsValid()
{
	return (g_hDriver != INVALID_HANDLE_VALUE);
}

/////////////////////////////////////////////////////////////////////////////
HDEVICE HACCOpenDriver(const SETTINGS &Settings)
{
	DWORD	dwReturned	= 0;
	HDEVICE	hDevice		= 0;
	
	if (!HACCIsValid())
		return NULL;

	if (!DeviceIoControl(g_hDriver, HACC_OPEN, (LPVOID)&Settings, sizeof(SETTINGS), &hDevice, sizeof(HDEVICE), &dwReturned, NULL))
		return NULL;

	return hDevice;
}

/////////////////////////////////////////////////////////////////////////////
BOOL HACCCloseDriver(HDEVICE hDevice)
{
	DWORD dwReturned = 0;

	if (!HACCIsValid())
		return FALSE;

	// Schließe den Gerätetreiber
	if (!DeviceIoControl(g_hDriver, HACC_CLOSE,	&hDevice, sizeof(HDEVICE), NULL, 0, &dwReturned, NULL))
  		return FALSE;
	
	return TRUE;   
}

/////////////////////////////////////////////////////////////////////////////
DWORD HACCGetDriverVersion()
{
	DWORD dwDriverVersion	= 0;
	DWORD dwReturned		= 0;

	if (!HACCIsValid())
		return 0;

	//Versionsnummer des Gerätetreibers erfragen
	if (!DeviceIoControl(g_hDriver, HACC_GET_DRIVERVERSION, NULL, 0, &dwDriverVersion, sizeof(DWORD), &dwReturned, NULL))
		return 0;

	return dwDriverVersion;
}

/////////////////////////////////////////////////////////////////////////////
DWORD HACCGetHAccDllVersion()
{
	return HACC_UNIT_VERSION;
}

/////////////////////////////////////////////////////////////////////////////
BOOL HACCReadEventPacket(HDEVICE hDevice, EVENT_PACKET &EventPacket)
{
	DWORD dwReturned = 0;

	if (!HACCIsValid())
		return FALSE;

	EventPacket.hDevice = hDevice;
	EventPacket.bValid	= FALSE;

	if (!DeviceIoControl(g_hDriver, HACC_READ_EVENT_PACKET,	&EventPacket, sizeof(EventPacket), &EventPacket, sizeof(EventPacket), &dwReturned, NULL))
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
DWORD HACCMapPhysToLin(DWORD dwPhysAddr, DWORD dwLen)
{
	DWORD		dwReturned = 0;
	MEMSTRUCT	mem;

	if (!HACCIsValid())
		return 0;

	mem.pPhysAddr	= (PVOID)dwPhysAddr;
	mem.dwLen		= dwLen;
	mem.pLinAddr	= NULL;

	// Physikalische Adresse in lineare Adresse umwandeln
	if (!DeviceIoControl(g_hDriver, HACC_MAP_PHYS_TO_LIN, &mem, sizeof(mem), &mem, sizeof(mem), &dwReturned, NULL))
		return NULL;

	return (DWORD)mem.pLinAddr;
}

/////////////////////////////////////////////////////////////////////////////
BOOL HACCUnmapPhysToLin(DWORD dwLinAddr, DWORD dwLen)
{
	DWORD		dwReturned = 0;
	MEMSTRUCT	mem;

	if (!HACCIsValid())
		return 0;

	mem.pPhysAddr	= NULL;
	mem.dwLen		= dwLen;
	mem.pLinAddr	= (PVOID)dwLinAddr;

	// Physikalische Adresse in lineare Adresse umwandeln
	if (!DeviceIoControl(g_hDriver, HACC_UNMAP_PHYS_TO_LIN, &mem, sizeof(mem), &mem, sizeof(mem), &dwReturned, NULL))
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
DWORD HACCMapLinToPhys(DWORD dwLinAddr)
{
	DWORD		dwReturned = 0;
	MEMSTRUCT	mem;

	if (!HACCIsValid())
		return 0;

	mem.pLinAddr	= (PVOID)dwLinAddr;
	mem.dwLen		= 0;
	mem.pPhysAddr	= NULL;

	// Physikalische Adresse in lineare Adresse umwandeln
	if (!DeviceIoControl(g_hDriver, HACC_MAP_LIN_TO_PHYS, &mem, sizeof(mem), &mem, sizeof(mem), &dwReturned, NULL))
		return NULL;

	return (DWORD)mem.pPhysAddr;
}

/////////////////////////////////////////////////////////////////////////////
BOOL HACCAllocFramebuffer(MEMSTRUCT& mem)
{
	BOOL bResult = FALSE;
	DWORD		dwReturned = 0;

	if (!HACCIsValid())
		return FALSE;

	mem.pPhysAddr	= NULL;
	mem.pLinAddr	= NULL;

	// Physikalische Adresse in lineare Adresse umwandeln
	if (DeviceIoControl(g_hDriver, HACC_ALLOC_MEMORY, &mem, sizeof(mem), &mem, sizeof(mem), &dwReturned, NULL))
		bResult = TRUE;

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL HACCFreeFramebuffer(MEMSTRUCT& mem)
{
	BOOL bResult = FALSE;
 	DWORD		dwReturned = 0;

	if (!HACCIsValid())
		return FALSE;

	// Physikalische Adresse in lineare Adresse umwandeln
	if (DeviceIoControl(g_hDriver, HACC_FREE_MEMORY, &mem, sizeof(mem), &mem, sizeof(mem), &dwReturned, NULL))
		bResult = TRUE;

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL HACCGetFramebuffer(MEMSTRUCT& mem)
{
	BOOL bResult = FALSE;
	DWORD		dwReturned = 0;

	if (!HACCIsValid())
		return FALSE;

	mem.pPhysAddr	= NULL;
	mem.pLinAddr	= NULL;

	// Physikalische Adresse in lineare Adresse umwandeln
	if (DeviceIoControl(g_hDriver, HACC_GET_FRAME_BUFFER, &mem, sizeof(mem), &mem, sizeof(mem), &dwReturned, NULL))
		bResult = TRUE;

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
HANDLE HACCCreateRing0Event(HANDLE hR3Event)
{
	OPENVXDH	fpOvh		= NULL;
	HANDLE		hR0Event	= NULL;

	if (!hR3Event)
		return NULL;

	if (g_bWin95)
	{
		// Die Funktion OpenVxDHandle wandelt ein R3EventHandle in ein R0EventHandle
		fpOvh = (OPENVXDH)GetProcAddress(GetModuleHandle("KERNEL32"), "OpenVxDHandle");
		if (!fpOvh)
			return NULL;

		hR0Event = (HANDLE)(DWORD)(*fpOvh)(hR3Event);
	}
	else
	{
		hR0Event = hR3Event;	// Unter NT: R3->R0 im HAcc.sys	
	}

	return hR0Event;
}

/////////////////////////////////////////////////////////////////////////////
BOOL HACCWritePort8(DWORD dwAddr, BYTE byVal, BOOL bLatch)
{
	DWORD		dwReturned = 0;
	IOSTRUCT	Io;

	if (!HACCIsValid())
		return FALSE;

	Io.bLatch		= bLatch;
	Io.dwAddr		= dwAddr;
	Io.byVal		= byVal;
	Io.wIOSize		= IO_SIZE8;

	if (!DeviceIoControl(g_hDriver, HACC_WRITE_PORT, &Io, sizeof(IOSTRUCT), NULL, 0, &dwReturned, NULL))
		return FALSE;

	return TRUE;
}																								

/////////////////////////////////////////////////////////////////////////////
BOOL HACCWritePort16(DWORD dwAddr, WORD wVal, BOOL bLatch)
{
	DWORD		dwReturned = 0;
	IOSTRUCT	Io;

	if (!HACCIsValid())
		return FALSE;

	Io.bLatch		= bLatch;
	Io.dwAddr		= dwAddr;
	Io.wVal			= wVal;
	Io.wIOSize		= IO_SIZE16;

	if (!DeviceIoControl(g_hDriver, HACC_WRITE_PORT, &Io, sizeof(IOSTRUCT), NULL, 0, &dwReturned, NULL))
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL HACCWritePort32(DWORD dwAddr, DWORD dwVal, BOOL bLatch)
{
	DWORD		dwReturned = 0;
	IOSTRUCT	Io;

	if (!HACCIsValid())
		return FALSE;

	Io.bLatch		= bLatch;
	Io.dwAddr		= dwAddr;
	Io.dwVal		= dwVal;
	Io.wIOSize		= IO_SIZE32;

	if (!DeviceIoControl(g_hDriver, HACC_WRITE_PORT, &Io, sizeof(IOSTRUCT), NULL, 0, &dwReturned, NULL))
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BYTE HACCReadPort8(DWORD dwAddr, BOOL bLatch)
{
	DWORD		dwReturned	= 0;
	IOSTRUCT	Io;

	if (!HACCIsValid())
		return 0;

	Io.bLatch		= bLatch;
	Io.dwAddr		= dwAddr;
	Io.wIOSize		= IO_SIZE8;
	Io.byVal		= 0;

	DeviceIoControl(g_hDriver, HACC_READ_PORT, &Io, sizeof(IOSTRUCT), &Io, sizeof(IOSTRUCT), &dwReturned,	NULL);

	return Io.byVal;
}

/////////////////////////////////////////////////////////////////////////////
WORD HACCReadPort16(DWORD dwAddr, BOOL bLatch)
{
	DWORD		dwReturned	= 0;
	IOSTRUCT	Io;

	if (!HACCIsValid())
		return 0;

	Io.bLatch		= bLatch;
	Io.dwAddr		= dwAddr;
	Io.wIOSize		= IO_SIZE16;
	Io.wVal			= 0;

	DeviceIoControl(g_hDriver, HACC_READ_PORT, &Io, sizeof(IOSTRUCT), &Io, sizeof(IOSTRUCT), &dwReturned,	NULL);

	return Io.wVal;
}

/////////////////////////////////////////////////////////////////////////////
DWORD HACCReadPort32(DWORD dwAddr, BOOL bLatch)
{
	DWORD		dwReturned	= 0;
	IOSTRUCT	Io;

	if (!HACCIsValid())
		return 0;

	Io.bLatch		= bLatch;
	Io.dwAddr		= dwAddr;
	Io.wIOSize		= IO_SIZE32;
	Io.dwVal		= 0;

	DeviceIoControl(g_hDriver, HACC_READ_PORT, &Io, sizeof(IOSTRUCT), &Io, sizeof(IOSTRUCT), &dwReturned,	NULL);

	return Io.dwVal;
}

/////////////////////////////////////////////////////////////////////////////
BOOL HACCWriteMemory8(DWORD dwAddr, BYTE byVal, BOOL bLatch)
{
	DWORD		dwReturned = 0;
	IOSTRUCT	Io;

	if (!HACCIsValid())
		return FALSE;

	Io.bLatch		= bLatch;
	Io.dwAddr		= dwAddr;
	Io.byVal		= byVal;
	Io.wIOSize		= IO_SIZE8;

	if (!DeviceIoControl(g_hDriver, HACC_WRITE_MEMORY, &Io, sizeof(IOSTRUCT), NULL, 0, &dwReturned, NULL))
		return FALSE;

	return TRUE;
}																								

/////////////////////////////////////////////////////////////////////////////
BOOL HACCWriteMemory16(DWORD dwAddr, WORD wVal, BOOL bLatch)
{
	DWORD		dwReturned = 0;
	IOSTRUCT	Io;

	if (!HACCIsValid())
		return FALSE;

	Io.bLatch		= bLatch;
	Io.dwAddr		= dwAddr;
	Io.wVal			= wVal;
	Io.wIOSize		= IO_SIZE16;

	if (!DeviceIoControl(g_hDriver, HACC_WRITE_MEMORY, &Io, sizeof(IOSTRUCT), NULL, 0, &dwReturned, NULL))
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL HACCWriteMemory32(DWORD dwAddr, DWORD dwVal, BOOL bLatch)
{
	DWORD		dwReturned = 0;
	IOSTRUCT	Io;

	if (!HACCIsValid())
		return FALSE;

	Io.bLatch		= bLatch;
	Io.dwAddr		= dwAddr;
	Io.dwVal		= dwVal;
	Io.wIOSize		= IO_SIZE32;

	if (!DeviceIoControl(g_hDriver, HACC_WRITE_MEMORY, &Io, sizeof(IOSTRUCT), NULL, 0, &dwReturned, NULL))
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL HACCWriteMemoryX(void* pSrcBuffer, DWORD dwAddr, DWORD dwLen, BOOL bLatch)
{
	DWORD		dwReturned	= 0;
	IOSTRUCT	Io;

	if (!HACCIsValid())
		return 0;

	Io.bLatch		= bLatch;
	Io.dwAddr		= dwAddr;
	Io.wIOSize		= IO_SIZEX;
	Io.pBuffer		= pSrcBuffer;
	Io.dwLen		= dwLen;

	if (!DeviceIoControl(g_hDriver, HACC_WRITE_MEMORY, &Io, sizeof(IOSTRUCT), NULL, 0, &dwReturned,	NULL))
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BYTE HACCReadMemory8(DWORD dwAddr, BOOL bLatch)
{
	DWORD		dwReturned	= 0;
	IOSTRUCT	Io;

	if (!HACCIsValid())
		return 0;

	Io.bLatch		= bLatch;
	Io.dwAddr		= dwAddr;
	Io.wIOSize		= IO_SIZE8;
	Io.byVal		= 0;

	DeviceIoControl(g_hDriver, HACC_READ_MEMORY, &Io, sizeof(IOSTRUCT), &Io, sizeof(IOSTRUCT), &dwReturned,	NULL);

	return Io.byVal;
}

/////////////////////////////////////////////////////////////////////////////
WORD HACCReadMemory16(DWORD dwAddr, BOOL bLatch)
{
	DWORD		dwReturned	= 0;
	IOSTRUCT	Io;

	if (!HACCIsValid())
		return 0;

	Io.bLatch		= bLatch;
	Io.dwAddr		= dwAddr;
	Io.wIOSize		= IO_SIZE16;
	Io.wVal			= 0;

	DeviceIoControl(g_hDriver, HACC_READ_MEMORY, &Io, sizeof(IOSTRUCT), &Io, sizeof(IOSTRUCT), &dwReturned,	NULL);

	return Io.wVal;
}

/////////////////////////////////////////////////////////////////////////////
DWORD HACCReadMemory32(DWORD dwAddr, BOOL bLatch)
{
	DWORD		dwReturned	= 0;
	IOSTRUCT	Io;

	if (!HACCIsValid())
		return 0;

	Io.bLatch		= bLatch;
	Io.dwAddr		= dwAddr;
	Io.wIOSize		= IO_SIZE32;
	Io.dwVal		= 0;

	DeviceIoControl(g_hDriver, HACC_READ_MEMORY, &Io, sizeof(IOSTRUCT), &Io, sizeof(IOSTRUCT), &dwReturned,	NULL);

	return Io.dwVal;
}

/////////////////////////////////////////////////////////////////////////////
BOOL HACCReadMemoryX(void* pDestBuffer, DWORD dwAddr, DWORD dwLen, BOOL bLatch)
{
	DWORD		dwReturned	= 0;
	IOSTRUCT	Io;

	if (!HACCIsValid())
		return 0;

	Io.bLatch		= bLatch;
	Io.dwAddr		= dwAddr;
	Io.wIOSize		= IO_SIZEX;
	Io.pBuffer		= pDestBuffer;
	Io.dwLen		= dwLen;

	if (!DeviceIoControl(g_hDriver, HACC_READ_MEMORY, &Io, sizeof(IOSTRUCT), &Io, sizeof(IOSTRUCT), &dwReturned,	NULL))
		return FALSE;

	return TRUE;
}
