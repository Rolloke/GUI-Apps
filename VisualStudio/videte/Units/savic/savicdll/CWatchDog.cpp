/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: cwatchdog.cpp $
// ARCHIVE:		$Archive: /Project/Units/SaVic/SavicDll/cwatchdog.cpp $
// CHECKIN:		$Date: 20.06.03 12:01 $
// VERSION:		$Revision: 11 $
// LAST CHANGE:	$Modtime: 20.06.03 11:35 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CWatchDog.h"
#include "CPCF8563.h"
#include "CIoMemory.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////
CWatchDog::CWatchDog(DWORD dwIOBase)
{
	m_pPCF8563	= new CPCF8563(dwIOBase);	// Timerchip als Watchdog
	m_pCIo		= new CIoMemory(dwIOBase);	// Zum Ansprechen der GPIO-Pins

	EnableWatchDog(); // Watchdog freigeben
}

///////////////////////////////////////////////////////////////////////////
CWatchDog::~CWatchDog()
{
	DisableWatchDog(); // Watchdog sperren
	
	WK_DELETE(m_pPCF8563);
	WK_DELETE(m_pCIo);
}

///////////////////////////////////////////////////////////////////////////
BOOL CWatchDog::GetWatchDogState() const
{
	WORD wAlarmState = (WORD)(m_pCIo->Inputdw(BT878_GPIO_DATA_CTL) & 0x0000000f);
	
	return  (TSTBIT(wAlarmState, BT878_GPIO_ALARM_IN0_BIT));
}

///////////////////////////////////////////////////////////////////////////
BOOL CWatchDog::EnableWatchDog() const
{
	BOOL bResult = FALSE;

	if (m_pPCF8563)
		bResult = m_pPCF8563->EnableCountdown();
					    
//	if (bResult)
//		ML_TRACE(_T("Enable Watchdog\n"));

	return bResult;
}

///////////////////////////////////////////////////////////////////////////
BOOL CWatchDog::DisableWatchDog() const
{
	BOOL bResult = FALSE;

	if (m_pPCF8563)
		bResult = m_pPCF8563->DisableCountdown();
	
//	if (bResult)
//		ML_TRACE(_T("Disable Watchdog\n"));

	return bResult;
}

///////////////////////////////////////////////////////////////////////////
BOOL CWatchDog::TriggerWatchDog(WORD wTi) const
{
	BOOL bResult = FALSE;

	if (EnableWatchDog())
		bResult = m_pPCF8563->TriggerCountdown(wTi);
							    
//	if (bResult)
//		ML_TRACE(_T("Trigger WatchDog wTi=%d\n"), wTi);

	return bResult;
}

///////////////////////////////////////////////////////////////////////////
BOOL CWatchDog::Diagnostic() const
{
	DWORD dwTC			= 0;
	DWORD dwCalcTime	= 5000; 
	BOOL  bOk			= TRUE;

	ML_TRACE_DIAGNOSTIC(_T("\nWatchdogtimer.\n"));
	TriggerWatchDog(dwCalcTime/1000);

	if (!GetWatchDogState())
	{
		dwTC = GetTickCount();
		while (!GetWatchDogState() && GetTimeSpan(dwTC) < dwCalcTime * 1.5)
		{
			Sleep(1000);
			ML_TRACE_DIAGNOSTIC(_T("@+."));

		}

		if (GetWatchDogState())
		{
			ML_TRACE_DIAGNOSTIC(_T("@-Watchdogtimer test: passed\n"));
			bOk = TRUE;
		}
		else
		{
			ML_TRACE_DIAGNOSTIC(_T("@-Watchdogtimer test: failed\n"));
			bOk = FALSE;
		}
	}
	else
	{
		ML_TRACE_DIAGNOSTIC(_T("@-Watchdogtimer test: failed\n"));
		bOk = FALSE;
	}
	DisableWatchDog();
	
	return bOk;
}
