// genport.h    Include file for Generic Port I/O Example Driver
//
// Robert R. Howell         January 6, 1993
// Robert B. Nelson (MS)    January 12, 1993
//      Modified comments prior to posting to Compuserve
//      Changed Wkd* to Gpd*
// Robert B. Nelson (MS)    March 1, 1993
//      Cleanup and bug fixes for Beta 2
//
#include <ntddk.h>
#include <string.h>
#include <devioctl.h>
#include "akuioctl.h"        // Get IOCTL interface definitions

/* Default base port, and # of ports */
#define BASE_PORT       0x00
#define NUMBER_PORTS    0x10

// Innerhalb dieses Adressbereiches müssen die AKU-Karten liegen.
#define BASEPORT_LOW	0x200
#define BASEPORT_COUNT	0x1F0

// NT device name
#define AKU_DEVICE_NAME L"\\Device\\Aku0"
#define DOS_DEVICE_NAME L"\\DosDevices\\AkuDev"

// driver local data structure specific to each device object
typedef struct _LOCAL_DEVICE_INFO {
    ULONG               DeviceType;     // Our private Device Type
    BOOLEAN             PortWasMapped;  // If TRUE, we have to unmap on unload
    PVOID               PortBase;		// base port address
    ULONG               PortCount;		// Count of I/O addresses used.
	ULONG               PortMemoryType; // HalTranslateBusAddress MemoryType
    PDEVICE_OBJECT      DeviceObject;   // The Gpd device object.
} LOCAL_DEVICE_INFO, *PLOCAL_DEVICE_INFO;

/********************* function prototypes ***********************************/

NTSTATUS    DriverEntry(       IN  PDRIVER_OBJECT DriverObject,
                               IN  PUNICODE_STRING RegistryPath );

NTSTATUS    AkuCreateDevice(   IN  PWSTR szPrototypeName,
                               IN  DEVICE_TYPE DeviceType,
                               IN  PDRIVER_OBJECT DriverObject,
                               OUT PDEVICE_OBJECT *ppDevObj     );


NTSTATUS    AkuDispatch(       IN  PDEVICE_OBJECT pDO,
                               IN  PIRP pIrp                    );

NTSTATUS    AkuIoctlReadPort(  IN  PLOCAL_DEVICE_INFO pLDI,
                               IN  PIRP pIrp,
                               IN  PIO_STACK_LOCATION IrpStack,
                               IN  ULONG IoctlCode              );

NTSTATUS    AkuIoctlWritePort( IN  PLOCAL_DEVICE_INFO pLDI,
                               IN  PIRP pIrp,
                               IN  PIO_STACK_LOCATION IrpStack,
                               IN  ULONG IoctlCode              );
                               
VOID        AkuUnload(         IN  PDRIVER_OBJECT DriverObject );

