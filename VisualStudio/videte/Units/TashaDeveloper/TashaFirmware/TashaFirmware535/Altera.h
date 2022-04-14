#ifndef __ALTERA_H__
#define __ALTERA_H__

#include "..\Include\SpiDefs.h" 

#define SPI1_ALTERA_BAUDRATE	10000000 	//10 MHz

#define pALTERA_REG ((volatile BYTE *)0x24000000)
#define pALTERA_REG_END ((volatile BYTE *)0x24000020)

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

#define NCONFIG 	PF12
#define CONF_DONE	PF13
#define DCOMI 		0x00000001;

void AlteraInit();
void writeAlteraReg(BYTE byReg, BYTE byDataW);
void readAlteraReg(BYTE byReg, BYTE *pbyDataR);
BOOL progAlteraInit();
BOOL progAlteraStart();
BOOL progAlteraTX(BYTE* pScr, DWORD dwProgLength);
BOOL CreateDescriptorList(BYTE* pSrc, DWORD dwLen);
BOOL WaitUntilAlteraTransferCompeted(int nTimeOut);


#endif __ALTERA_DEFINED
