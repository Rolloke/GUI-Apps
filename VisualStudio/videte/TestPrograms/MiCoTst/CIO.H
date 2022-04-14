#ifndef __CIO_H__ 
#define __CIO_H__

#include "conio.h"

// Größe des Speicherbereiches in denen die Latchregister gesichert werden
#define LATCH_SIZE 2048

// auf dieses Mico Register müssen Launcher und MicoUnit zugreifen!

class CWatchDogAndRelaisRegister;

class CIo
{
public:
	CIo();
	~CIo();

	BYTE Input(WORD wPort);
	WORD Inputw(WORD wPort);
	void Output(WORD wPort, BYTE byDatabyte);
	void Outputw(WORD wPort, WORD wDatabyte); 
	BYTE ReadFromLatch(WORD wPort);
	void WriteToLatch(WORD wPort, BYTE byDatabyte);

private:
	static BYTE	byLatchBuffer[LATCH_SIZE];
	static int  m_nIoInstCnt;
	static CWatchDogAndRelaisRegister* m_pWatchDogAndRelaisRegister;
	static CCriticalSection m_csIo;
};

#endif // __CIO_H__