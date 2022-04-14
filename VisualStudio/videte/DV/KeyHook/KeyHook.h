//======================================================================
// 
// KeyHook.h
//
// Copyright (C) 1996-1999 Mark Russinovich
//
// Hook onto the keyboard I/O path and massage the input stream
// converting caps-locks into controls. This example works on
// NT 4 version is very losely based on the
// i8042 port filter driver sample, kbfiltr, from the Win2K DDK.
//
//======================================================================

//
// Define scan codes of interest here. For scan code tables, see 
// "The Undocumented PC", by Frank Van Gullwe, Addison-Wesley 1994.
//

#define KEY_UP         1
#define KEY_DOWN       0

#define KEY_CTRL			((USHORT)0x1D)
#define KEY_ALT			    ((USHORT)0x38)
#define KEY_CAPS_LOCK		((USHORT)0x3A)
#define KEY_LWIN			((USHORT)0x5B)			
#define KEY_RWIN1			((USHORT)0x5C)			
#define KEY_RWIN2			((USHORT)0x5D)
#define KEY_TAB				((USHORT)0x0F)			
#define KEY_ESC				((USHORT)0x01)			
#define KEY_DEL				((USHORT)0x53)			
#define KEY_ROLL			((USHORT)0x46)
#define KEY_F4				((USHORT)0x3E)

typedef int BOOL;

//
//
// Undocumented NT 4 function to print to the startup screen
//
NTSYSAPI
NTSTATUS
NTAPI
ZwDisplayString( PUNICODE_STRING String );


//
// Our device extension definition
//
typedef struct _DEVICE_EXTENSION
{
    //
    // The top of the stack before this filter was added.  AKA the location
    // to which all IRPS should be directed.
    //
    PDEVICE_OBJECT  TopOfStack;

} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

//
// Forwards
//
NTSTATUS DriverEntry(IN PDRIVER_OBJECT  DriverObject, IN PUNICODE_STRING RegistryPath);
NTSTATUS KeyHookInit(IN PDRIVER_OBJECT DriverObject);
NTSTATUS KeyHookAddDevice(IN PDRIVER_OBJECT Driver, IN PDEVICE_OBJECT PDO);
NTSTATUS KeyHookPnP(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS KeyHookPower(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
VOID KeyHookUnload(IN PDRIVER_OBJECT Driver);

NTSTATUS KeyHookDispatchRead(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS KeyHookDispatchGeneral(IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp);
VOID KeyHookUnload(IN PDRIVER_OBJECT DriverObject);
VOID OnKeyDown(USHORT uScanCode);
VOID OnKeyUp(USHORT uScanCode);
