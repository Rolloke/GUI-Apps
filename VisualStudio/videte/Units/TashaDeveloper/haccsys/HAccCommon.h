/////////////////////////////////////////////////////////////////////////////
// PROJECT:		HAcc
// FILE:		$Workfile: HAccCommon.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha(Developer)/haccsys/HAccCommon.h $
// CHECKIN:		$Date: 5.01.04 9:52 $
// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 5.01.04 9:25 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

// Common-part of HAcc Driver
/////////////////////////////////////////////////////////////////////////////////////

#define HANDLED_IRQ			TRUE
#define NO_HANDLED_IRQ		FALSE

// IOCtlXXX-Funktionen Implementierung in HAccCommon.c
NTSTATUS IOCtlDispatch(int nControlCode, IOCtlStruct IOCtl);
NTSTATUS IOCtlOpen(IOCtlStruct IOCtl);
NTSTATUS IOCtlClose(IOCtlStruct IOCtl);
NTSTATUS IOCtlGetVersion(IOCtlStruct IOCtl);
NTSTATUS IOCtlWritePort(IOCtlStruct IOCtl);
NTSTATUS IOCtlReadPort(IOCtlStruct IOCtl);
NTSTATUS IOCtlWriteMemory(IOCtlStruct IOCtl);
NTSTATUS IOCtlReadMemory(IOCtlStruct IOCtl);
NTSTATUS IOCtlReadEventPacket(IOCtlStruct IOCtl);
NTSTATUS IOCtlMapPhysToLin(IOCtlStruct IOCtl);
NTSTATUS IOCtlUnmapPhysToLin(IOCtlStruct IOCtl);
NTSTATUS IOCtlMapLinToPhys(IOCtlStruct IOCtl);
NTSTATUS IOCtlAllocateMemory(IOCtlStruct IOCtl);
NTSTATUS IOCtlFreeMemory(IOCtlStruct IOCtl);
NTSTATUS IOCtlGetFrameBuffer(IOCtlStruct IOCtl);

// Hilfs-Funktionen (Implementierung in HAccCommon.c)
LARGE_INTEGER ReadRDTSC();
void	WaitMicroSecs(CONFIG *pConfig, DWORD dwMicros);
DWORD   GetMicroTicks(CONFIG *pConfig);
BOOL	CleanUp(CONFIG *pConfig);
void	FastMemCopy(void* pDestination, void* pSource, DWORD dwLen);
void	MemCopy(void* pDestination, void* pSource, DWORD dwLen);
BOOL	ResetEvent(DWORD hEvent);

BOOL	SendDebugMessageToUnit0(CONFIG* pConfig, WORD wEventType, char* lpMsg);
BOOL	SendDebugMessageToUnit1(CONFIG* pConfig, WORD wEventType, char* lpMsg, DWORD dwP1);
BOOL	SendDebugMessageToUnit2(CONFIG* pConfig, WORD wEventType, char* lpMsg, DWORD dwP1, DWORD dwP2);
BOOL	SendDebugMessageToUnit3(CONFIG* pConfig, WORD wEventType, char* lpMsg, DWORD dwP1, DWORD dwP2, DWORD dwP3);
BOOL	SendDebugMessageToUnit4(CONFIG* pConfig, WORD wEventType, char* lpMsg, DWORD dwP1, DWORD dwP2, DWORD dwP3, DWORD dwP4);
BOOL	SendDebugMessageToUnit(CONFIG*  pConfig, WORD wEventType, char* lpMsg, DWORD dwNP, DWORD dwP1, DWORD dwP2, DWORD dwP3, DWORD dwP4);

// ISA-spezifische Routinen (Implementierung in MiCoISAStuff.c)
BOOL	ISAServiceHAccBdInt(CONFIG *pConfig);
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

// PCI-spezifische Routinen (Implementierung in MiCoPCIStuff.c)
BOOL	PCIServiceHAccBdInt(CONFIG *pConfig);
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
