/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaViCDA
// FILE:		$Workfile: SavicDirectAccess.h $
// ARCHIVE:		$Archive: /Project/Units/SaVic2/SavicDA/SavicDirectAccess.h $
// CHECKIN:		$Date: 11.09.03 8:40 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 11.09.03 8:40 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "..\SavicDll\SavicDefs.h"

typedef HANDLE (WINAPI *OPENVXDH) (HANDLE);
#ifdef __cplusplus
extern "C" {
#endif

__declspec( dllexport ) HDEVICE	DAOpenDriver(const SETTINGS  &Settings);
__declspec( dllexport ) BOOL	DACloseDriver(HDEVICE hDevice);
__declspec( dllexport ) BOOL	DARequestSourceSwitch(HDEVICE hDevice, VIDEO_JOB& VideoJob);
__declspec( dllexport ) BOOL	DAClearSourceSwitchRequest(HDEVICE hDevice);
__declspec( dllexport ) BOOL	DAReadEventPacket(HDEVICE hDevice, EVENT_PACKET &EventPacket);
__declspec( dllexport ) BOOL	DATransferRiscProgram(HDEVICE hDevice, DWORD* pRiscProgram, DWORD dwLen);
__declspec( dllexport ) BOOL	DACaptureFrame(HDEVICE, CAPTURE_STRUCT &Capture);
__declspec( dllexport ) BOOL	DAReleaseQueueBuffer(HDEVICE hDevice);
__declspec( dllexport ) BOOL	DAIsValid();
__declspec( dllexport ) DWORD	DAGetDriverVersion();
__declspec( dllexport ) DWORD	DAGetSaVicDADllVersion();
__declspec( dllexport ) HANDLE	DACreateRing0Event(HANDLE hR3Event);
__declspec( dllexport ) DWORD	DAMapPhysToLin(DWORD dwPhysAddr, DWORD dwLen);
__declspec( dllexport ) DWORD	DAMapLinToPhys(DWORD dwLinAddr);
__declspec( dllexport ) DWORD	DAUnmapIoSpace(DWORD dwLinAddr, DWORD dwLen);
__declspec( dllexport ) BOOL	DAReadQueueState(HDEVICE hDevice, QUEUE_INF &QInf);
__declspec( dllexport ) BOOL	DAWritePort8(DWORD dwAddr,  BYTE  byVal);
__declspec( dllexport ) BOOL	DAWritePort16(DWORD dwAddr, WORD  wVal);
__declspec( dllexport ) BOOL	DAWritePort32(DWORD dwAddr, DWORD dwVal);
__declspec( dllexport ) BYTE	DAReadPort8(DWORD dwAddr);
__declspec( dllexport ) WORD	DAReadPort16(DWORD dwAddr);
__declspec( dllexport ) DWORD	DAReadPort32(DWORD dwAddr);
__declspec( dllexport ) BOOL	DAWriteMemory8(DWORD dwAddr,  BYTE  byVal);
__declspec( dllexport ) BOOL	DAWriteMemory16(DWORD dwAddr, WORD  wVal);
__declspec( dllexport ) BOOL	DAWriteMemory32(DWORD dwAddr, DWORD dwVal);
__declspec( dllexport ) BYTE	DAReadMemory8(DWORD dwAddr);
__declspec( dllexport ) WORD	DAReadMemory16(DWORD dwAddr);
__declspec( dllexport ) DWORD	DAReadMemory32(DWORD dwAddr);
__declspec( dllexport ) BOOL	DAWriteToI2C(DWORD dwIOBase, BYTE bySlaveAddr, BYTE bySubAddr, BYTE byVal);
__declspec( dllexport ) BOOL	DAReadFromI2C(DWORD dwIOBase, BYTE bySlaveAddr, BYTE bySubAddr, BYTE& byVal);
__declspec( dllexport ) BOOL	DAGetLastCameraScanResult(HDEVICE hDevice, DWORD& dwCamMask);

#ifdef __cplusplus
}
#endif

