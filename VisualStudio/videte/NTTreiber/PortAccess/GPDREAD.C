// Read a byte from a port using the wkd driver.
// Robert R. Howell             January 6, 1993
// Robert B. Nelson (Microsoft) January 12, 1993


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <winioctl.h>
#include "gpioctl.h"        // This defines the IOCTL constants.

HANDLE  hndFile;            // Handle to device, obtain from CreateFile
BOOL OpenDevice();
BOOL CloseDevice();
BYTE Input(DWORD dwPort);
WORD Inputw(DWORD dwPort);
void Output(DWORD dwPort, BYTE byVal);
void Outputw(DWORD dwPort, WORD wVal);

void __cdecl main(int argc, char ** argv)
{
	WORD  wVal;
    DWORD dwI;
	DWORD dwTC;
	DWORD dwBase = 0;

    if (!OpenDevice())        // Was the device opened?
    {
        printf("Unable to open the device.\n");
        exit(1);
    }

	dwTC = GetTickCount();

	for (dwI = 0; dwI < 10000; dwI++)
	{
		Output(dwBase | 0x14, 0x01);
		Sleep(10);
		Output(dwBase | 0x14, 0x02);
		Sleep(10);
		Output(dwBase | 0x14, 0x04);
		Sleep(10);
		Output(dwBase | 0x14, 0x08);
		Sleep(10);
	}
	
	printf("WriteTime=%lu\n", GetTickCount() - dwTC);

	dwTC = GetTickCount();

	for (dwI = 0; dwI < 10000; dwI++)
		wVal = Inputw(dwBase | 0x14);

	printf("ReadTime=%lu\n", GetTickCount() - dwTC);

    CloseDevice();
	exit(0);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL OpenDevice()
{
    hndFile = CreateFile(
                "\\\\.\\GpdDev1",                    // Open the Device "file"
                GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ,
                NULL,
                OPEN_EXISTING,
                0,
                NULL
                );
                
    if (hndFile == INVALID_HANDLE_VALUE)        // Was the device opened?
    {
		return FALSE;
    }

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CloseDevice()
{
    if (!hndFile)
		return FALSE;

	if (!CloseHandle(hndFile))                  // Close the Device "file".
    {
        printf("Failed to close device.\n");
		return FALSE;
    }
	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////////
BYTE Input(DWORD dwPortNumber)
{
    // The following is returned by IOCTL.  It is true if the read succeeds.
    DWORD   ReturnedLength	= 0;     // Number of bytes returned
	BYTE	byVal			= 0;

    if (!DeviceIoControl(hndFile,						// Handle to device
                        IOCTL_GPD_READ_PORT_UCHAR,      // IO Control code for Read
                        &dwPortNumber,					// Buffer to driver.
                        sizeof(DWORD),					// Length of buffer in bytes.
                        &byVal,							// Buffer from driver.
                        sizeof(BYTE),					// Length of buffer in bytes.
                        &ReturnedLength,				// Bytes placed in DataBuffer.
                        NULL))							// NULL means wait till op. completes.
	{
		printf("Input failed\n");
		return 0;	
	}

	return byVal;
}

///////////////////////////////////////////////////////////////////////////////////
WORD Inputw(DWORD dwPortNumber)
{
    // The following is returned by IOCTL.  It is true if the read succeeds.
    DWORD   ReturnedLength	= 0;     // Number of bytes returned
	WORD	wVal			= 0;

    if (!DeviceIoControl(hndFile,						// Handle to device
                        IOCTL_GPD_READ_PORT_USHORT,     // IO Control code for Read
                        &dwPortNumber,					// Buffer to driver.
                        sizeof(DWORD),					// Length of buffer in bytes.
                        &wVal,							// Buffer from driver.
                        sizeof(WORD),					// Length of buffer in bytes.
                        &ReturnedLength,				// Bytes placed in DataBuffer.
                        NULL))							// NULL means wait till op. completes.
	{
		printf("Inputw failed\n");
		return 0;	
	}

	return wVal;
}

///////////////////////////////////////////////////////////////////////////////////
void Output(DWORD dwPortNumber, BYTE byVal)
{
    DWORD   ReturnedLength	= 0;		// Number of bytes returned
    GENPORT_WRITE_INPUT InputBuffer;    // Input buffer for DeviceIoControl
    
	InputBuffer.PortNumber	= dwPortNumber;
	InputBuffer.CharData	= (UCHAR)byVal;

    if (!DeviceIoControl(
                    hndFile,								// Handle to device
                    IOCTL_GPD_WRITE_PORT_UCHAR,				// IO Control code for Write
                    &InputBuffer,							// Buffer to driver.  Holds port & data.
                    sizeof(dwPortNumber) + sizeof(byVal),   // Length of buffer in bytes.
                    NULL,									// Buffer from driver.   Not used.
                    0,										// Length of buffer in bytes.
                    &ReturnedLength,						// Bytes placed in outbuf.  Should be 0.
                    NULL))									// NULL means wait till I/O completes.

	{
		printf("Outputw failed\n");
	}
}

///////////////////////////////////////////////////////////////////////////////////
void Outputw(DWORD dwPortNumber, WORD wVal)
{
    DWORD   ReturnedLength	= 0;		// Number of bytes returned
    GENPORT_WRITE_INPUT InputBuffer;    // Input buffer for DeviceIoControl
    
	InputBuffer.PortNumber	= dwPortNumber;
	InputBuffer.ShortData	= (USHORT)wVal;

    if (!DeviceIoControl(
                    hndFile,								// Handle to device
                    IOCTL_GPD_WRITE_PORT_USHORT,			// IO Control code for Write
                    &InputBuffer,							// Buffer to driver.  Holds port & data.
                    sizeof(dwPortNumber) + sizeof(wVal),	// Length of buffer in bytes.
                    NULL,									// Buffer from driver.   Not used.
                    0,										// Length of buffer in bytes.
                    &ReturnedLength,						// Bytes placed in outbuf.  Should be 0.
                    NULL))									// NULL means wait till I/O completes.

	{
		printf("Outputw failed\n");
	}
}