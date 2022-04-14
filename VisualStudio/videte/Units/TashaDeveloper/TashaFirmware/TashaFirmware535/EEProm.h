#ifndef __EEPROM_H__
#define __EEPROM_H__

#include "..\Include\SpiDefs.h" 

#define SPI1_EEPROM_BAUDRATE	1000000 	// 1MHz

#define PF0 	0x0001
#define PF1 	0x0002
#define PF2 	0x0004
#define PF3 	0x0008
#define PF4 	0x0010
#define PF5 	0x0020
#define PF6 	0x0040
#define PF7 	0x0080
#define PF8 	0x0100
#define PF9 	0x0200
#define PF10 	0x0400
#define PF11 	0x0800
#define PF12 	0x1000
#define PF13 	0x2000
#define PF14 	0x4000
#define PF15 	0x8000

//Altera Porogrammierung

#define EEPROM_CS 	PF13

BOOL ProgrammEEPromInit();
BOOL EEPromWriteEnable();
BOOL EEPromWriteDisable();

BOOL WriteToEEPromX(BYTE* pData, int nLen, int nStartAddr);
BOOL ReadFromEEPromX(BYTE* pData, int nLen, int nStartAddr);

BOOL WriteToEEProm8(int nAddr, BYTE byVal);
BOOL ReadFromEEProm8(int nAddr, BYTE* byVal);

BOOL WaitUntilTransferComplete(int nTimeOut);

#endif // __EEPROM_H__
