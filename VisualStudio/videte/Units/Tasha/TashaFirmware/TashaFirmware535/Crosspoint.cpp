/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaFirmware535
// FILE:		$Workfile: Crosspoint.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/TashaFirmware535/Crosspoint.cpp $
// CHECKIN:		$Date: 2.03.04 11:59 $
// VERSION:		$Revision: 8 $
// LAST CHANGE:	$Modtime: 2.03.04 11:11 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include <gpiotest.h>
#include <sys/exception.h>
#include <cdefblackfin.h>
#include <CdefBF535.h>
#include "..\Include\Helper.h"
#include "TashaFirmware535.h"
#include "Crosspoint.h"
#include "Altera.h"
#include "EEprom.h"

static DWORD g_dwCrosspointMask = 0;

/////////////////////////////////////////////////////////////////////////
BOOL SetCrosspoint(DWORD dwCrosspointMask)
{
	BOOL bResult = FALSE;
	WORD wDummy	 = 0;
	WORD wVal 	 = 0;
	
	// dwCrosspointMask=xxxx3333xxxx2222xxxx1111xxxx0000
	// wMask		   =3333222211110000
	WORD wMask =  ((dwCrosspointMask>>0) & 0x0f)<<0 |
				  ((dwCrosspointMask>>8) & 0x0f)<<4 |
				  ((dwCrosspointMask>>16)& 0x0f)<<8 |
				  ((dwCrosspointMask>>24)& 0x0f)<<12;

	// CS des EEProms aus
	*pFIO_DIR 	 = *pFIO_DIR | EEPROM_CS;						// EEPROM_CS Ausgang
	*pFIO_FLAG_S = EEPROM_CS;									// EEPROM_CS auf '1'

	// Selectiere den Crosspoint
	if (readAlteraReg(ALT_CTRL, wVal))
	{	
		wVal = SETBIT(wVal, ALT_CTRL_CROSSPOINT_CS_BIT);	
		writeAlteraReg(ALT_CTRL, wVal);
	}
	
	// Baudraten der beiden SPIs einstellen	
	*pSPI0_BAUD = SYSTEM_CLK/(2*SPI0_CROSSPOINT_BAUDRATE);	
	
	// EnableSPI, Master, 16Bit, Disable MISO, SlaveSelect disable, SendZero, Transmit
	*pSPI0_CTL = SPE | MSTR | SIZE | SZ | 0x01;

	*pSPI0_ST 	= 0xff;
	*pSPI0_TDBR = wMask;
					  
	if (WaitUntilSPI0TransferComplete(100))
	{
		wDummy = *pSPI0_RDBR;
		bResult = TRUE;
		
		// Zwecks Auslesens den Wert zwischenspeichern
		g_dwCrosspointMask = dwCrosspointMask;
	}
	
	// Disable SPI0
	*pSPI0_CTL &= ~SPE;	
	
	// Crosspoint CS wieder löschen
	if (readAlteraReg(ALT_CTRL, wVal))
	{	
		wVal = CLRBIT(wVal, ALT_CTRL_CROSSPOINT_CS_BIT);	
		writeAlteraReg(ALT_CTRL, wVal);
	}

	return bResult;	
}

/////////////////////////////////////////////////////////////////////////
BOOL GetCrosspoint(DWORD& dwCrosspointMask)
{
	dwCrosspointMask = g_dwCrosspointMask;
			
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL WaitUntilSPI0TransferComplete(int nTimeOut)
{
	int nCounter = 0;
	nTimeOut *= 1000;		// Es wird jeweils 1ys gewartet.
	while(!(*pSPI0_ST & SPIF) && (nCounter++ < nTimeOut))
		WaitMicro(1);

	return (*pSPI0_ST & SPIF);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL EnableCrosspoint()
{
	WORD wVal = 0;
	BOOL bResult = FALSE;
	
	// Enable Crosspoint
	if (readAlteraReg(ALT_CTRL, wVal))
	{
		wVal = CLRBIT(wVal, ALT_CTRL_CROSSPOINT_SHUTDOWN_BIT);
		bResult = writeAlteraReg(ALT_CTRL, wVal);
	}
	
	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DisableCrosspoint()
{
	WORD wVal = 0;
	BOOL bResult = FALSE;

	// Enable Crosspoint
	if (readAlteraReg(ALT_CTRL, wVal))
	{
		wVal = SETBIT(wVal, ALT_CTRL_CROSSPOINT_SHUTDOWN_BIT);
		bResult = writeAlteraReg(ALT_CTRL, wVal);
	}

	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL GetCrosspointEnableState(BOOL& bState)
{
	WORD wVal = 0;
	BOOL bResult = FALSE;
	
	if (readAlteraReg(ALT_CTRL, wVal))
	{
		bState  = !TSTBIT(wVal, ALT_CTRL_CROSSPOINT_SHUTDOWN_BIT);
		bResult = TRUE; 	
	}
	
	return bResult;
}

