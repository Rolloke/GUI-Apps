/////////////////////////////////////////////////////////////////////////////
// PROJECT:		AkuUnit
// FILE:		$Workfile: AkuSys.h $
// ARCHIVE:		$Archive: /Project/Units/Aku/AkuSys/AkuSys.h $
// CHECKIN:		$Date: 5.08.98 9:53 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 5.08.98 9:53 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "Driver.h"

typedef ULONG HEVENT;
#define MAKEWORD(a,b) (WORD)((WORD)b<<8 | (WORD)a)

#define MIN(a,b) ((a <= b) ? a : b)
 
#define AKU_SYS_VERSION	0x00040000

// NT device name
#define AKU_DEVICE_NAME L"\\Device\\Aku0"

// File system device name.   When you execute a CreateFile call to open the
#define DOS_DEVICE_NAME L"\\DosDevices\\AkuDev"

// Größe des Latches
#define LATCH_SIZE 1024

// driver local data structure specific to each device object
typedef struct _LOCAL_DEVICE_INFO {
    ULONG               DeviceType;     // Our private Device Type
    PDEVICE_OBJECT      DeviceObject;   // The MiCo device object.
	ULONG				IrpSequenceNumber;
	UCHAR				IrpRetryCount;
} LOCAL_DEVICE_INFO, *PLOCAL_DEVICE_INFO;

/********************* function prototypes ***********************************/

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath);
NTSTATUS AkuCreateDevice(IN PWSTR szPrototypeName,
						 IN DEVICE_TYPE DeviceType,
						 IN PDRIVER_OBJECT DriverObject,
						 OUT PDEVICE_OBJECT *ppDevObj);

NTSTATUS AkuDispatch(IN PDEVICE_OBJECT pDO, IN PIRP pIrp);
NTSTATUS AkuIOCtlWritePort(IN PIRP pIrp, PIO_STACK_LOCATION pIrpStack);
NTSTATUS AkuIOCtlReadPort(IN PIRP pIrp, PIO_STACK_LOCATION pIrpStack);

VOID	 AkuUnload(IN PDRIVER_OBJECT DriverObject);

DWORD	 ReadProfile(IN PWCHAR RegistryPath, PWCHAR ValueName, DWORD Default);
void	 ClaimPortResource(WORD wI, CM_RESOURCE_LIST *pResList, DWORD dwAddr, DWORD dwCnt);
void	 ClaimMemoryResource(WORD wI, CM_RESOURCE_LIST *pRLst, DWORD dwAddr, DWORD dwCnt);
void	 ClaimIRQResource(WORD wI, CM_RESOURCE_LIST *pResList, DWORD dwInterrupt);
HANDLE	 CreateRing0Event(HANDLE h3Event);

void	 Out_Debug_String(char* lpString);
BOOL	 PageAllocate(MEMSTRUCT *pMem, DWORD dwLen);
BOOL	 PageFree(MEMSTRUCT *pMem);

NTSTATUS CleanUp();

void	ISAOutput8(DWORD dwAddr, BYTE byVal);
void	ISAOutput16(DWORD dwAddr, WORD wVal);
void	ISAOutput32(DWORD dwAddr, DWORD wVal);
BYTE	ISAInput8(DWORD dwAddr);
WORD	ISAInput16(DWORD dwAddr);
DWORD	ISAInput32(DWORD dwAddr);
void	ISAOutputLatch8(DWORD dwAddr, BYTE byVal);
void	ISAOutputLatch16(DWORD dwAddr, WORD wVal);
void	ISAOutputLatch32(DWORD dwAddr, DWORD wVal);
BYTE	ISAInputLatch8(DWORD dwAddr);
WORD	ISAInputLatch16(DWORD dwAddr);
DWORD	ISAInputLatch32(DWORD dwAddr);

