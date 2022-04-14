#ifndef __CIO_H__ 
#define __CIO_H__

#include <afxmt.h>

// Größe des Speicherbereiches in denen die Latchregister gesichert werden
#define LATCH_SIZE 2048

class CIo
{
public:
	CIo();
	~CIo();

	BYTE Input(WORD wPort);							// 8Bit  Lese-Zugriff
	WORD Inputw(WORD wPort);						// 16Bit Lese-Zugriff
	void Output(WORD wPort, BYTE byDatabyte);		// 8Bit	 Schreib-Zugriff
	void Outputw(WORD wPort, WORD wDatabyte); 		// 16Bit Schreib-Zugriff
	BYTE ReadFromLatch(WORD wPort);					// 8Bit  Lesezugriff (Aus Latch)
	void WriteToLatch(WORD wPort, BYTE byDatabyte);	// 8Bit  Schreibzugriff (latched)

private:
	static BYTE	byLatchBuffer[LATCH_SIZE];			// Speicher zum Latchen
	static int  m_nIoInstCnt;						// Instanzenzähler
	static CCriticalSection m_csIo;					// Synchronisationsobjekt
};												

#endif // __CIO_H__