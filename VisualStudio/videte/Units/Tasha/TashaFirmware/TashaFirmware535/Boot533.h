/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaFirmware535
// FILE:		$Workfile: Boot533.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/TashaFirmware535/Boot533.h $
// CHECKIN:		$Date: 10.02.05 12:03 $
// VERSION:		$Revision: 20 $
// LAST CHANGE:	$Modtime: 10.02.05 11:29 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "..\Include\SpiDefs.h"

//#define SPI1_BOOTBF533_BAUDRATE 10000000
#define SPI1_BOOTBF533_BAUDRATE 5000000 
//#define USE_DMA	// Der DMA-Transfer funktioniert leider noch nicht

BOOL SPIBootTransferBootLoader(int nBFNr, BYTE* pBuffer, DWORD dwLen);
BOOL SPIBootTransferFirmware(int nBFNr, BYTE* pBuffer, DWORD dwLen);

BOOL SPIBootInitSPI(BOOL bBF535Master);
BOOL SPIBootMasterTransfer(int nSlave, BYTE* pBuffer, DWORD dwLen);
void SPIBootSelectSlave(int nSlave);
BOOL SPIBootWaitUntilSlaveIsReady(int nSlave, int nTimeOut);

BOOL SPIBootMasterDMATransfer(int nSlave, BYTE* pBuffer, DWORD dwLen);
BOOL SPIBootCreateDescriptorList(BYTE* pBuffer, DWORD dwLen);
BOOL SPIBootWaitUntilFirmwareTransferCompleted(int nTimeOut);

