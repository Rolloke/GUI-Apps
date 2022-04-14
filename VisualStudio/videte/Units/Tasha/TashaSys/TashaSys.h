/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaSys
// FILE:		$Workfile: TashaSys.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaSys/TashaSys.h $
// CHECKIN:		$Date: 13.04.04 10:06 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 13.04.04 9:50 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "TashaIoCtl.h"        // Get IOCTL interface definitions

#define DEVICEDRIVER
#include "..\TashaDA\DirectAccess.h"
#undef DEVICEDRIVER

// NT device name
#define TASHA_DEVICE_NAME L"\\Device\\Tasha0"

// File system device name.   When you execute a CreateFile call to open the
#define DOS_DEVICE_NAME L"\\DosDevices\\Tasha.sys"

#define MAKEWORD(a,b) (WORD)((WORD)b<<8 | (WORD)a)
#define MIN(a,b) ((a <= b) ? a : b)
#define MAX(a,b) ((a >= b) ? a : b)

// driver local data structure specific to each device object
typedef struct _LOCAL_DEVICE_INFO {
    ULONG               DeviceType;     // Our private Device Type
    PDEVICE_OBJECT      DeviceObject;   // The Tasha device object.
	ULONG				IrpSequenceNumber;
	UCHAR				IrpRetryCount;
} LOCAL_DEVICE_INFO, *PLOCAL_DEVICE_INFO;

/********************* function prototypes ***********************************/
NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath);
NTSTATUS TashaCreateDevice(IN PWSTR szPrototypeName, IN DEVICE_TYPE DeviceType, IN PDRIVER_OBJECT DriverObject, OUT PDEVICE_OBJECT *ppDevObj);
NTSTATUS TashaDispatch(IN PDEVICE_OBJECT pDO, IN PIRP pIrp);

VOID	TashaUnload(IN PDRIVER_OBJECT DriverObject);
BOOL	PageAllocate(MEMSTRUCT *pMem, DWORD dwLen, BOOL bContiguousMemory);
BOOL	PageFree(MEMSTRUCT *pMem, BOOL bContiguousMemory);
void	Out_Debug_String(char* lpString);


