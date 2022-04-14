#ifndef __SPI_H__
#define __SPI_H__

#include "..\Include\SpiDefs.h"

#define SPI1_COMMUNICATION_BAUDRATE 10000000

void HandleSPICommunication();
BOOL InitSPI();
void EnableSPI();
void DisableSPI();

BOOL SendSPIMessage(int nMessageID, DWORD dwUserData, WORD wSlave, DWORD dwParam1=0, DWORD dwParam2=0);
BOOL CheckIncommingData(SPIPacketStruct& SPIReceivePacket);
void SelectSlave(int nSlave);
BOOL WaitUntilSPITransferComplete(int nTimeOut);


#endif // __SPI_H__
