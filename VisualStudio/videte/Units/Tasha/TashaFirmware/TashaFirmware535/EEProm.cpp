/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaFirmware535
// FILE:		$Workfile: EEProm.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/TashaFirmware535/EEProm.cpp $
// CHECKIN:		$Date: 19.04.04 10:08 $
// VERSION:		$Revision: 17 $
// LAST CHANGE:	$Modtime: 19.04.04 10:07 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include <gpiotest.h>
#include <cdefblackfin.h>
#include <CdefBF535.h>
#include <sys/exception.h>
#include <stdio.h>
#include <string.h> 

#include "..\Include\Helper.h"
#include "..\Include\SpiDefs.h"
#include "..\Include\TMMessages.h"
#include "EEProm.h"
#include "Sports.h"
#include "TashaFirmware535.h"

//==========================================================================
// Schreib- und Lesezugriffe auf das EEProm
//==========================================================================

/////////////////////////////////////////////////////////////////////////
BOOL ProgrammEEPromInit()
{
	// Baudraten der beiden SPIs einstellen	
	*pSPI0_BAUD = SYSTEM_CLK/(2*SPI0_EEPROM_BAUDRATE);	
	
	// DisableSPI, Master, 8Bit, Enable MISO, SlaveSelect disable, SendZero, Transmit
	*pSPI0_CTL = MSTR | EMISO | SZ | 0x01;

	*pFIO_DIR 	 = *pFIO_DIR | EEPROM_CS;						// EEPROM_CS Ausgang
	*pFIO_FLAG_S = EEPROM_CS;									// EEPROM_CS auf '1'
	*pFIO_POLAR	 = *pFIO_POLAR &~ EEPROM_CS; 					// Active high
	*pFIO_EDGE	 = *pFIO_EDGE  &~ EEPROM_CS;					// Level sensitive

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL ReadFromEEPromX(BYTE* pData, int nLen, int nStartAddr)
{
	BOOL bErr 	 = FALSE;

	// Enable SPI
	*pSPI0_CTL |= SPE;		

	// Datenbuffer an das EEProm übertragen
	for (int nI = 0; (nI < nLen) && !bErr; nI++)
	{
		if (!ReadFromEEProm8(nStartAddr+nI, &pData[nI]))
			bErr = TRUE;
	}		
		
	// Disable SPI
	*pSPI0_CTL &~ SPE;	
	

	return !bErr;		
}

// - Enable Write Operation
// - Send Write Instruction
// - Send Hi-Addressbyte
// - Send Lo-Addressbyte
// - Send up to 32 Databytes (All Bytes must reside in the same page)
///////////////////////////////////////////////////////////////////////////////////
BOOL WriteToEEPromX(BYTE* pData, int nLen, int nStartAddr)
{
	BOOL bResult = FALSE;
	BOOL bFirstAccess = TRUE;
	
	// Enable SPI
	*pSPI0_CTL |= SPE;		

	*pSPI0_ST 		= 0xff;

	for (int nAddr = 0; nAddr < nLen; nAddr++)
	{
		if (((nAddr % 32) == 0) || bFirstAccess)
		{
			bFirstAccess = FALSE;
			// EEPROM_CS   auf '1'
			*pFIO_FLAG_S = EEPROM_CS;					
		
			// Zwischen jedem Schreibzyklus muß mindestens 5ms gewartet werden.
			WaitMicro(5000);

			if (EEPromWriteEnable())
			{
				// Write-Instruction
				*pSPI0_TDBR = 0x02;	
				if (WaitUntilTransferComplete(10))
				{
					// Adressebits A15...A8 senden
					*pSPI0_TDBR = HIBYTE(nStartAddr+nAddr);
					if (WaitUntilTransferComplete(10))
					{
						// Adressebits A7...A0 senden
						*pSPI0_TDBR = LOBYTE(nStartAddr+nAddr);
						if (WaitUntilTransferComplete(10))
							bResult = (!(*pSPI0_ST & TXCOL) && !(*pSPI0_ST & TXE) && !(*pSPI0_ST & MODF));
					}
				}
			}
		}
		if (bResult)
		{
			// Date senden
			*pSPI0_TDBR = pData[nAddr];
			if (WaitUntilTransferComplete(10))
			{		
				// Ist ein Übertragungsfehler aufgetreten?
				bResult = (!(*pSPI0_ST & TXCOL) && !(*pSPI0_ST & TXE) && !(*pSPI0_ST & MODF));
			}
		}
		if (!bResult)
			break;
	}

	// EEPROM_CS   auf '1'
	*pFIO_FLAG_S = EEPROM_CS;					
	
	// Disable SPI
	*pSPI0_CTL &~ SPE;	
	
	// Zwischen jedem Schreibzyklus muß mindestens 5ms gewartet werden.
	WaitMicro(5000);
		
	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL EEPromWriteEnable()
{
	BOOL bResult 	= FALSE;

	*pSPI0_ST 		= 0xff;

	// EEPROM_CS   auf '0
	*pFIO_FLAG_C = EEPROM_CS;

	*pSPI0_TDBR 	= 0x06;	// 0x06 = WriteEnable Sequence
	
	// Warte bis Transfer abgeschlossen ist
	if (WaitUntilTransferComplete(100))
	{
		// Ist ein Übertragungsfehler aufgetreten?
		bResult = (!(*pSPI0_ST & TXCOL) && !(*pSPI0_ST & TXE) && !(*pSPI0_ST & MODF));
	}

	// EEPROM_CS   auf '1
	*pFIO_FLAG_S = EEPROM_CS;
	WaitMicro(1);
	// EEPROM_CS   auf '0
	*pFIO_FLAG_C = EEPROM_CS;
	
	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL EEPromWriteDisable()
{
	BOOL bResult 	= FALSE;

	*pSPI0_ST	= 0xff;

	// EEPROM_CS   auf '0
	*pFIO_FLAG_C = EEPROM_CS;

	*pSPI0_TDBR = 0x04;	// 0x04 = WriteDisable Sequence
	
	// Warte bis Transfer abgeschlossen ist
	if (WaitUntilTransferComplete(100))
	{
		// Ist ein Übertragungsfehler aufgetreten?
		bResult = (!(*pSPI0_ST & TXCOL) && !(*pSPI0_ST & TXE) && !(*pSPI0_ST & MODF));
	}
	
	// EEPROM_CS   auf '1
	*pFIO_FLAG_S = EEPROM_CS;
	WaitMicro(1);
	// EEPROM_CS   auf '0
	*pFIO_FLAG_C = EEPROM_CS;
	
	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL WriteToEEProm8(int nAddr, BYTE byVal)
{
	BOOL bResult = FALSE;

	*pSPI0_ST 		= 0xff;

	// EEPROM_CS   auf '0
	*pFIO_FLAG_C = EEPROM_CS;

	if (EEPromWriteEnable())
	{
		// Write-Instruction
		*pSPI0_TDBR = 0x02;	
		if (WaitUntilTransferComplete(10))
		{
			// Adressebits A15...A8 senden
			*pSPI0_TDBR = HIBYTE(nAddr);
			if (WaitUntilTransferComplete(10))
			{
				// Adressebits A7...A0 senden
				*pSPI0_TDBR = LOBYTE(nAddr);
				if (WaitUntilTransferComplete(10))
				{
					// Date senden
					*pSPI0_TDBR = byVal;
					if (WaitUntilTransferComplete(10))
					{		
						// Ist ein Übertragungsfehler aufgetreten?
						bResult = (!(*pSPI0_ST & TXCOL) && !(*pSPI0_ST & TXE) && !(*pSPI0_ST & MODF));
					}
				}
			}
		}
	}

	// EEPROM_CS   auf '1'
	*pFIO_FLAG_S = EEPROM_CS;					
	
	WaitMicro(5000);
	
	return bResult;	
}

///////////////////////////////////////////////////////////////////////////////////
BOOL ReadFromEEProm8(int nAddr, BYTE* byVal)
{
	BOOL bResult 	= FALSE;
	BYTE byDummy	= 0;

	// EEPROM_CS   auf '0
	*pFIO_FLAG_C = EEPROM_CS;

	*pSPI0_ST 		= 0xff;
	
	byDummy = (BYTE)*pSPI0_RDBR;

	// Read-Instruction
	*pSPI0_TDBR = 0x03;	
	if (WaitUntilTransferComplete(100))
	{
		byDummy = (BYTE)*pSPI0_RDBR;
		
		// Adressebits A15...A8 senden
		*pSPI0_TDBR = HIBYTE(nAddr);
		if (WaitUntilTransferComplete(100))
		{
			byDummy = (BYTE)*pSPI0_RDBR;
		
			// Adressebits A7...A0 senden
			*pSPI0_TDBR = LOBYTE(nAddr);
			if (WaitUntilTransferComplete(100))
			{
				byDummy = (BYTE)*pSPI0_RDBR;
			
				*pSPI0_TDBR = 0;
				if (WaitUntilTransferComplete(100))
				{
					// Date lesen
					*byVal = (BYTE)*pSPI0_RDBR;
				
					// Ist ein Übertragungsfehler aufgetreten?
					bResult = (!(*pSPI0_ST & TXCOL) && !(*pSPI0_ST & RBSY) && !(*pSPI0_ST & TXE) && !(*pSPI0_ST & MODF));
					if (!bResult)
					 	printf("Addr=%d Val=%d Status=%d\n", nAddr, (int)byDummy, (int)bResult);
				}
			}
		}
	}

	// EEPROM_CS   auf '1'	
	*pFIO_FLAG_S = EEPROM_CS;
//	if (bResult)
//	 	printf("Addr=%d Val=%d\n", nAddr, (int)*byVal);

//	WaitMicro(1000);

	return bResult;	
}

///////////////////////////////////////////////////////////////////////////////////
BOOL WaitUntilTransferComplete(int nTimeOut)
{
	int nCounter = 0;
	nTimeOut *= 1000;		// Es wird jeweils 1ys gewartet.
	while(!(*pSPI0_ST & SPIF) && (nCounter++ < nTimeOut))
		WaitMicro(1);

	return (*pSPI0_ST & SPIF);
}


