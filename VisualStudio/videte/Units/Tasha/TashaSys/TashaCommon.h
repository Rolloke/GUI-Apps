/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaSys
// FILE:		$Workfile: TashaCommon.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaSys/TashaCommon.h $
// CHECKIN:		$Date: 13.04.04 10:06 $
// VERSION:		$Revision: 6 $
// LAST CHANGE:	$Modtime: 13.04.04 9:56 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

// Common-part of Tasha Driver
/////////////////////////////////////////////////////////////////////////////////////

#define HANDLED_IRQ			TRUE
#define NO_HANDLED_IRQ		FALSE

// IOCtlXXX-Funktionen
NTSTATUS IOCtlDispatch(int nControlCode, IOCtlStruct IOCtl);
NTSTATUS IOCtlGetVersion(IOCtlStruct IOCtl);
NTSTATUS IOCtlGetFrameBuffer(IOCtlStruct IOCtl);
NTSTATUS IOCtlGetTransferBuffer(IOCtlStruct IOCtl);

NTSTATUS IOCtlWritePort(IOCtlStruct IOCtl);
NTSTATUS IOCtlReadPort(IOCtlStruct IOCtl);
NTSTATUS IOCtlWriteMemory(IOCtlStruct IOCtl);
NTSTATUS IOCtlReadMemory(IOCtlStruct IOCtl);

// Hilfs-Funktionen
BOOL	CleanUp();
void	FastMemCopy(void* pDestination, void* pSource, DWORD dwLen);
void	MemCopy(void* pDestination, void* pSource, DWORD dwLen);

// ISA-spezifische Routinen
void	ISAOutput8(DWORD dwAddr, BYTE byVal);
void	ISAOutput16(DWORD dwAddr, WORD wVal);
void	ISAOutput32(DWORD dwAddr, DWORD wVal);
BYTE	ISAInput8(DWORD dwAddr);
WORD	ISAInput16(DWORD dwAddr);
DWORD	ISAInput32(DWORD dwAddr);

// PCI-spezifische Routinen
BYTE	PCIInput8(DWORD dwPort);
WORD	PCIInput16(DWORD dwPort);
DWORD	PCIInput32(DWORD dwPort);
void	PCIInputX(void* pDestBuffer, DWORD dwPort, DWORD dwLen);
void	PCIOutput8(DWORD dwPort, BYTE byData);
void	PCIOutput16(DWORD dwPort, WORD wData); 
void	PCIOutput32(DWORD dwPort, DWORD dwData); 
void	PCIOutputX(void* pSrcBuffer, DWORD dwPort, DWORD dwLen);

BOOL HasMMX();
BOOL SupportCheckForSFence();
