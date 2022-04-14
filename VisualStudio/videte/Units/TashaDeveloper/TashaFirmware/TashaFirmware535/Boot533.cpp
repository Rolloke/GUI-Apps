#include <CdefBF535.h>
#include <sys/exception.h>

#include "..\Include\Helper.h"
#include "Boot533.h"
#include "TashaFirmware535.h"

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

#define SLAVE_RESET		PF12

/////////////////////////////////////////////////////////////////////////
BOOL Boot533(int nBFNr, BYTE* pBuffer, DWORD dwLen)
{
	BOOL bValidAddress 	= FALSE;
	DWORD dwErrorCnt	= 0;
	WORD wAddr			= 0;
	
	*pFIO_DIR 	 		= *pFIO_DIR | SLAVE_RESET;			// SLAVE_RESET auf Ausgang
	*pFIO_DIR			= *pFIO_DIR & ~PF1;					// SPISS auf Eingang
	*pFIO_FLAG_C 		= SLAVE_RESET;						// SLAVE_RESET auf '0' (BF533 in den Reset bringen)
		
	// MSB-First, Enable MISO, SendZero, SlaveSelect disable, Slave, 8Bit, EnableSPI
	*pSPI1_CTL = EMISO | SZ | SPE | PSSE | 0x01;		

	// BF533 mindestens 100ys im Reset belassen.
	WaitMicro(100);									
	
	// SLAVE_RESET auf '1' (BF533 aus dem Reset holen)
	*pFIO_FLAG_S = SLAVE_RESET;						

	do
	{
		// Schon eine gültige Leseadresse erhalten?
		if (bValidAddress)
			*pSPI1_TDBR = pBuffer[wAddr++];
			
		if (WaitUntilSPITransferCompleted(10))
		{
			// Read Kommando?
			if (*pSPI1_RDBR == 0x03)						
			{
				// Ja, dann folgt als nächstes eine 16Bit Adresse
				if (WaitUntilSPITransferCompleted(10))
				{		
					wAddr = (*pSPI1_RDBR & 0xff)<<8;
			
					if (WaitUntilSPITransferCompleted(10))
					{
						// Dies ist die gewünschte Leseadresse.
						wAddr = wAddr | (*pSPI1_RDBR & 0xff);
						bValidAddress = TRUE;
					}
				}
			}
		}
		else
			dwErrorCnt++;
	}
	while ((wAddr < dwLen) && (dwErrorCnt < 100));

	*pSPI1_CTL &= ~SPE;					// SPI Ausschalten
	
	return dwErrorCnt<100;
}
///////////////////////////////////////////////////////////////////////////////////
BOOL WaitUntilSPITransferCompleted(int nTimeOut)
{
	int nCounter = 0;
	nTimeOut *= 1000;		// Es wird jeweils 1ys gewartet.
	while(!(*pSPI1_ST & RXS) && (nCounter++ < nTimeOut))
		WaitMicro(1);
		
	return (*pSPI1_ST & RXS);
}

