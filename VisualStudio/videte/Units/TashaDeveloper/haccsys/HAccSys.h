/////////////////////////////////////////////////////////////////////////////
// PROJECT:		HAcc.sys
// FILE:		$Workfile: HAccSys.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha(Developer)/haccsys/HAccSys.h $
// CHECKIN:		$Date: 5.01.04 9:52 $
// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 5.01.04 9:25 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "HAccIoCtl.h"        // Get IOCTL interface definitions

#define DEVICEDRIVER
#include "..\HAccDll\DirectAccess.h"
#undef DEVICEDRIVER

// NT device name
#define HACC_DEVICE_NAME L"\\Device\\HAcc0"

// File system device name.   When you execute a CreateFile call to open the
#define DOS_DEVICE_NAME L"\\DosDevices\\HAcc.sys"

#define MAKEWORD(a,b) (WORD)((WORD)b<<8 | (WORD)a)
#define MIN(a,b) ((a <= b) ? a : b)
#define MAX(a,b) ((a >= b) ? a : b)

// driver local data structure specific to each device object
typedef struct _LOCAL_DEVICE_INFO {
    ULONG               DeviceType;     // Our private Device Type
    PDEVICE_OBJECT      DeviceObject;   // The HAcc device object.
	ULONG				IrpSequenceNumber;
	UCHAR				IrpRetryCount;
} LOCAL_DEVICE_INFO, *PLOCAL_DEVICE_INFO;

/********************* function prototypes ***********************************/
NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath);
NTSTATUS HAccCreateDevice(IN PWSTR szPrototypeName, IN DEVICE_TYPE DeviceType, IN PDRIVER_OBJECT DriverObject, OUT PDEVICE_OBJECT *ppDevObj);
NTSTATUS HAccDispatch(IN PDEVICE_OBJECT pDO, IN PIRP pIrp);

VOID	HAccUnload(IN PDRIVER_OBJECT DriverObject);
DWORD	ReadProfile(IN PWCHAR RegistryPath, PWCHAR ValueName, DWORD Default);
WORD    ClaimHAccXResource(WCHAR* sDevice, PCM_RESOURCE_LIST pResourceList, BOOL bFirst);
void	ClaimPortResource(WORD wI, CM_RESOURCE_LIST *pResList, DWORD dwAddr, DWORD dwCnt);
void	ClaimMemoryResource(WORD wI, CM_RESOURCE_LIST *pRLst, DWORD dwAddr, DWORD dwCnt);
void	ClaimIRQResource(WORD wI, CM_RESOURCE_LIST *pResList, DWORD dwInterrupt);
DWORD	CreateRing0Event(DWORD h3Event);
BOOL	MapPhysToLin(MEMSTRUCT *pMem);
BOOL	UnmapPhysToLin(MEMSTRUCT *pMem);
BOOL	MapLinToPhys(MEMSTRUCT *pMem);
BOOL	AllocateMemory(MEMSTRUCT *pMem);
BOOL	FreeMemory(MEMSTRUCT *pMem);
BOOL	GetFrameBuffer(MEMSTRUCT *pMem);
BOOL	UnmapIOSpace(CONFIG* pConfig);
BOOL	PageAllocate(MEMSTRUCT *pMem, DWORD dwLen, BOOL bContiguousMemory);
BOOL	PageFree(MEMSTRUCT *pMem, BOOL bContiguousMemory);
BOOL	InstallISR(CONFIG *pConfig);
void	RemoveISR(CONFIG *pConfig);
BOOL	CloseAllEventHandles(CONFIG *pConfig);
BOOL	SetDebugEvent(CONFIG* pConfig);
void	Out_Debug_String(char* lpString);
void	Sleep(DWORD dwSleep);
BOOL	ServiceHAccBdInt(IN PKINTERRUPT pInterruptObject,IN PVOID Context);


