/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaFirmware535
// FILE:		$Workfile: SPICommunication.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/TashaFirmware535/SPICommunication.h $
// CHECKIN:		$Date: 6.05.04 12:06 $
// VERSION:		$Revision: 22 $
// LAST CHANGE:	$Modtime: 6.05.04 7:59 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __SPI_H__
#define __SPI_H__

#include "..\Include\SpiDefs.h"

#define SPI1_COMMUNICATION_BAUDRATE 10000000

void HandleSPICommunication();
BOOL InitSPI();
void EnableSPI();
void DisableSPI();

BOOL SendSPIMessage(int nMessageID, DWORD dwUserData, WORD wSlave, DWORD dwParam1=0, DWORD dwParam2=0);
BOOL CheckIncommingData(SPIPacketStruct& SPIReceivePacket, WORD wSlave);
void SelectSlave(int nSlave);
BOOL WaitUntilSPITransferComplete(int nTimeOut);
DWORD GetCheckSum(SPIPacketStruct SPIPacket);


#endif // __SPI_H__
