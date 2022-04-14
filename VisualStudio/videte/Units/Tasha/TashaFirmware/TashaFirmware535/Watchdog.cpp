/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaFirmware535
// FILE:		$Workfile: Watchdog.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/TashaFirmware535/Watchdog.cpp $
// CHECKIN:		$Date: 2.02.05 12:35 $
// VERSION:		$Revision: 7 $
// LAST CHANGE:	$Modtime: 2.02.05 11:11 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include <gpiotest.h>
#include <sys/exception.h>
#include <cdefblackfin.h>
#include <CdefBF535.h>
#include "..\Include\Helper.h"
#include "TashaFirmware535.h"
#include "Watchdog.h"
#include "Altera.h"

int g_nTimer 	= 0;
int g_nTimeOut 	= 60;

///////////////////////////////////////////////////////////////////////////////////
BOOL EnableWatchdog(int nTimeOut)
{
	static BOOL bInit = FALSE;
	if (!bInit)
	{
		InitWatchdogTimer(1);
		bInit = TRUE;	
	}
	
	*pTIMER0_STATUS = IRQ0;		
	*pTIMER0_STATUS = TIMEN0;
    *pIMASK 	= *pIMASK | EVT_IVG13;			// enable IVG13

	*pFIO_DIR 	 = *pFIO_DIR | PF_WATCHDOG;		// PF11 auf Ausgang
	*pFIO_FLAG_C = PF_WATCHDOG;					// PF11 auf '0'

	g_nTimeOut = nTimeOut;
    
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL DisableWatchdog()
{
  	*pIMASK 	= *pIMASK &~ EVT_IVG13;			// disable IVG13
	*pTIMER0_STATUS = TIMDIS0;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL TriggerWatchdog(int nTimeOut)
{
	g_nTimer = 0;

	if (nTimeOut != -1)
		EnableWatchdog(nTimeOut);
	else
		DisableWatchdog();
	
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////
EX_INTERRUPT_HANDLER(WatchdogTimerISR) 
{ 
	static BOOL bState = FALSE;

	if (g_nTimer++ >= g_nTimeOut)
	{
		g_nTimer =0;
		
		*pFIO_DIR 	 = *pFIO_DIR | PF_WATCHDOG;			// PF11 auf Ausgang
		*pFIO_FLAG_S = PF_WATCHDOG;						// PF11 auf '1'
	}

		
	// Clear IRQ
	*pTIMER0_STATUS = IRQ0 | OVF_ERR0;		
}

///////////////////////////////////////////////////////////////////////////////////
BOOL InitWatchdogTimer(int nFreq)
{
	int nPeriod 		= SYSTEM_CLK/nFreq;

	register_handler(ik_ivg13 , WatchdogTimerISR); 	// Register ISR


 	*pSIC_IAR1 	= *pSIC_IAR1  & 0xf0ffffff;		// P0-Bits löschen         
    *pSIC_IAR1 	= *pSIC_IAR1  | P6_IVG13;		// Assign Timer0 to IVG13         

	*pSIC_IMASK = *pSIC_IMASK | SIC_MASK14;		// enable Timer0 
	*pTIMER0_CONFIG = 0;
    *pTIMER0_CONFIG = *pTIMER0_CONFIG | PERIOD_CNT | PWM_OUT | IRQ_ENA;

    *pTIMER0_PERIOD_HI = HIWORD(nPeriod);
    *pTIMER0_PERIOD_LO = LOWORD(nPeriod);
    
    *pTIMER0_WIDTH_HI = HIWORD(nPeriod/2);
    *pTIMER0_WIDTH_LO = LOWORD(nPeriod/2);
  	
	return TRUE;
}


