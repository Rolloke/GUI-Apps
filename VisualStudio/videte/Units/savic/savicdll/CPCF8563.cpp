/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: Cpcf8563.cpp $
// ARCHIVE:		$Archive: /Project/Units/SaVic/SavicDll/Cpcf8563.cpp $
// CHECKIN:		$Date: 20.06.03 12:01 $
// VERSION:		$Revision: 11 $
// LAST CHANGE:	$Modtime: 20.06.03 11:35 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CPCF8563.h"
#include "CI2C.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
CPCF8563::CPCF8563(DWORD dwIOBase)
{
	BYTE byVal = 0;

	// I2C-Objekt für den Zugriff auf den PCF8563 über den I2C-Bus
	m_pCI2C = new CI2C(dwIOBase, PCF8563_SLAVE_ADR);

	m_csPCF8563.Lock();
	// Controlregister 1 initialisieren
	if (m_pCI2C->ReadFromI2C(PCF8563_CONTROL_STATUS1, byVal))
	{
		byVal = CLRBIT(byVal, PCF8563_CTRL1_TEST1_BIT);
		byVal = CLRBIT(byVal, PCF8563_CTRL1_STOP_BIT);
		byVal = CLRBIT(byVal, PCF8563_CTRL1_TESTC_BIT);
		m_pCI2C->WriteToI2C(PCF8563_CONTROL_STATUS1, byVal);
	}
												  
	// Controlregister 2 initialisieren
	if (m_pCI2C->ReadFromI2C(PCF8563_CONTROL_STATUS2, byVal))
	{
		// Ist das Countdown flag gesetzt?
		if (TSTBIT(byVal, PCF8563_CTRL2_TF_BIT))
			ML_TRACE_WARNING(_T("Timer count down flag is set: Watchdog was active!\n"));
		
		byVal = CLRBIT(byVal, PCF8563_CTRL2_TITP_BIT);
		byVal = CLRBIT(byVal, PCF8563_CTRL2_TF_BIT);
		byVal = CLRBIT(byVal, PCF8563_CTRL2_AIE_BIT);	// Alarminterrupt disable
		byVal = SETBIT(byVal, PCF8563_CTRL2_TIE_BIT);	// Timerinterrupt enable
		m_pCI2C->WriteToI2C(PCF8563_CONTROL_STATUS2, byVal);
	}
	
	// Clockout initialisieren
	if (m_pCI2C->ReadFromI2C(PCF8563_CLKOUT_FREQUENCY, byVal))
	{
		byVal = SETBIT(byVal, PCF8563_CTKOUT_FE_BIT);  // Activate Clkoutput
		byVal = SETBIT(byVal, PCF8563_CTKOUT_FD1_BIT); // 32Hz
		byVal = CLRBIT(byVal, PCF8563_CTKOUT_FD0_BIT); //  "
		m_pCI2C->WriteToI2C(PCF8563_CLKOUT_FREQUENCY, byVal);
	}

	// Timercontrol initialisieren
	if (m_pCI2C->ReadFromI2C(PCF8563_TIMER_CONTROL, byVal))
	{
		byVal = CLRBIT(byVal, PCF8563_TICTRL_TE_BIT);  // Disable timer
		byVal = SETBIT(byVal, PCF8563_TICTRL_TD1_BIT); // Timer SourceClock = 32Hz
		byVal = CLRBIT(byVal, PCF8563_TICTRL_TD0_BIT);
		m_pCI2C->WriteToI2C(PCF8563_TIMER_CONTROL, byVal);
	}

	// Min-Alarmregister initialisieren
	if (m_pCI2C->ReadFromI2C(PCF8563_MINUTE_ALARM, byVal))
	{
		byVal = CLRBIT(byVal, PCF8563_ALARM_AE_MIN_BIT);  // Disable Minutenalarm 
		m_pCI2C->WriteToI2C(PCF8563_MINUTE_ALARM, byVal);
	}

	// Hour-Alarmregister initialisieren
	if (m_pCI2C->ReadFromI2C(PCF8563_HOUR_ALARM, byVal))
	{
		byVal = CLRBIT(byVal, PCF8563_ALARM_AE_HOUR_BIT);  // Disable Houralarm 
		m_pCI2C->WriteToI2C(PCF8563_HOUR_ALARM, byVal);
	}

	// Day-Alarmregister initialisieren
	if (m_pCI2C->ReadFromI2C(PCF8563_ALARM_AE_DAY_BIT, byVal))
	{
		byVal = CLRBIT(byVal, PCF8563_ALARM_AE_HOUR_BIT);  // Disable Dayalarm 
		m_pCI2C->WriteToI2C(PCF8563_ALARM_AE_DAY_BIT, byVal);
	}

	// WeekDay-Alarmregister initialisieren
	if (m_pCI2C->ReadFromI2C(PCF8563_ALARM_AE_WEEKDAY_BIT, byVal))
	{
		byVal = CLRBIT(byVal, PCF8563_ALARM_AE_HOUR_BIT);  // Disable WeekDayalarm 
		m_pCI2C->WriteToI2C(PCF8563_ALARM_AE_WEEKDAY_BIT, byVal);
	}

	if (m_pCI2C->ReadFromI2C(PCF8563_SECONDS, byVal))
	{
		// Voltage low detector flag set?
		if (TSTBIT(byVal, PCF8563_VL_BIT))
			ML_TRACE_WARNING(_T("Power low detector flag is set: Power lost\n"));
	
		byVal = CLRBIT(byVal, PCF8563_VL_BIT); 
		m_pCI2C->WriteToI2C(PCF8563_SECONDS, byVal);

	}
	
	m_csPCF8563.Unlock();
}

//////////////////////////////////////////////////////////////////////
CPCF8563::~CPCF8563()
{
	WK_DELETE(m_pCI2C);
}

//////////////////////////////////////////////////////////////////////
BOOL  CPCF8563::SetTime(const CTime &time)
{
	if (!m_pCI2C)
		return FALSE;

	m_csPCF8563.Lock();

	m_pCI2C->WriteToI2C(PCF8563_SECONDS,		DecToBCD(time.GetSecond()));
	m_pCI2C->WriteToI2C(PCF8563_MINUTES,		DecToBCD(time.GetMinute()));
	m_pCI2C->WriteToI2C(PCF8563_HOURS,			DecToBCD(time.GetHour()));
	m_pCI2C->WriteToI2C(PCF8563_DAYS,			DecToBCD(time.GetDay()));
	m_pCI2C->WriteToI2C(PCF8563_WEEKDAYS,		DecToBCD(time.GetDayOfWeek()-1));

	int nYear  = time.GetYear();
	int nMonth = time.GetMonth(); 
	if (nYear <= 1999)
	{
		nMonth = DecToBCD(nMonth);
		nMonth = SETBIT(nMonth, PCF8563_CENTURY_BIT);
	}
	else
	{
		nMonth = DecToBCD(nMonth);
		nMonth = CLRBIT(nMonth, PCF8563_CENTURY_BIT);
	}

	m_pCI2C->WriteToI2C(PCF8563_MONTH_CENTURY,	nMonth);
	m_pCI2C->WriteToI2C(PCF8563_YEARS,			DecToBCD(nYear % 100));

	m_csPCF8563.Unlock();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CPCF8563::GetTime(CTime& Time) const
{
	Time = CTime::GetCurrentTime();
	if (!m_pCI2C)
		return FALSE;

	int nSeconds	= 0;
	int nMinutes	= 0;
	int nHours		= 0;
	int nDays		= 0;
	int nWeekDays	= 0;
	int nMonths		= 0;
	int nYears		= 0;

	if (!m_pCI2C->ReadFromI2C(PCF8563_SECONDS,		(BYTE&)nSeconds))
		 return FALSE;
	if (!m_pCI2C->ReadFromI2C(PCF8563_MINUTES,		(BYTE&)nMinutes))
		return FALSE;
	if (!m_pCI2C->ReadFromI2C(PCF8563_HOURS,		(BYTE&)nHours))
		return FALSE;
	if (!m_pCI2C->ReadFromI2C(PCF8563_DAYS,			(BYTE&)nDays))
		return FALSE;
	if (!m_pCI2C->ReadFromI2C(PCF8563_WEEKDAYS,		(BYTE&)nWeekDays))
		return FALSE;
	if (!m_pCI2C->ReadFromI2C(PCF8563_MONTH_CENTURY,(BYTE&)nMonths))
		return FALSE;
	if (!m_pCI2C->ReadFromI2C(PCF8563_YEARS,		(BYTE&)nYears))
		return FALSE;
	
	if (TSTBIT(nSeconds, PCF8563_VL_BIT))
	{
		ML_TRACE_WARNING(_T("PCFTime not guranteed. Using PCTime\n"));
		return FALSE;
	}				    

	nSeconds	= BCDToDec(nSeconds & 0x7f);
	nMinutes	= BCDToDec(nMinutes & 0x7f);
	nHours		= BCDToDec(nHours   & 0x3f);
	nDays		= BCDToDec(nDays    & 0x3f);
	nWeekDays	= BCDToDec(nWeekDays& 0x07)+1;
	nMonths		= BCDToDec(nMonths & 0x01f);
	nYears		= BCDToDec(nYears);

	if (nYears >= 99)
		nYears = nYears + 1900;
	else
		nYears = nYears + 2000;

	if ((nSeconds >= 60)	||
		(nMinutes >= 60)	||
		(nHours   >= 24)	||
		(nDays    >= 32)	||
		(nDays	  <= 0)		||
		(nMonths  >= 13)	||
		(nMonths  <= 0)		||
		(nYears   >= 2039)	||
		(nYears	  <= 1969))
	{
		ML_TRACE_WARNING(_T("PCFTime not shure. Using PCTime\n"));
		return FALSE;
	}

	Time = CTime(nYears, nMonths, nDays, nHours, nMinutes, nSeconds);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CPCF8563::EnableCountdown()
{
	BYTE byVal		= 0;
	BOOL bResult	= FALSE;

	m_csPCF8563.Lock();

	if (m_pCI2C->ReadFromI2C(PCF8563_CONTROL_STATUS2, byVal))
	{
		byVal = SETBIT(byVal, PCF8563_CTRL2_TIE_BIT);				// Timerinterrupt enable
		if (m_pCI2C->WriteToI2C(PCF8563_CONTROL_STATUS2, byVal))
		{
			if (m_pCI2C->ReadFromI2C(PCF8563_TIMER_CONTROL, byVal))
			{
				byVal = SETBIT(byVal, PCF8563_TICTRL_TE_BIT);		// Enable Countdown
				byVal = SETBIT(byVal, PCF8563_TICTRL_TD1_BIT);		// Timer SourceClock = 32Hz
				byVal = CLRBIT(byVal, PCF8563_TICTRL_TD0_BIT);
				if (m_pCI2C->WriteToI2C(PCF8563_TIMER_CONTROL, byVal))
					bResult = TRUE;
			}
		}
	}

	m_csPCF8563.Unlock();

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CPCF8563::DisableCountdown()
{
	BYTE	byVal = 0;
	BOOL	bResult = FALSE;

	m_csPCF8563.Lock();

	if (m_pCI2C->ReadFromI2C(PCF8563_CONTROL_STATUS2, byVal))
	{
		byVal = CLRBIT(byVal, PCF8563_CTRL2_TIE_BIT);				// Timerinterrupt disable
		if (m_pCI2C->WriteToI2C(PCF8563_CONTROL_STATUS2, byVal))
		{
			if (m_pCI2C->ReadFromI2C(PCF8563_TIMER_CONTROL, byVal))
			{
				byVal = CLRBIT(byVal, PCF8563_TICTRL_TE_BIT);		// Disable Countdown
				if (m_pCI2C->WriteToI2C(PCF8563_TIMER_CONTROL, byVal))
					bResult = TRUE;
			}
		}
	}
			
	m_csPCF8563.Unlock();

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CPCF8563::TriggerCountdown(WORD wTi)
{
	BOOL bResult = FALSE;

	wTi = min(wTi, 255); // maximal 255 Sekunden

	m_csPCF8563.Lock();

	if (DisableCountdown())
	{
		if (m_pCI2C->WriteToI2C(PCF8563_TIMER_COUNTDOWN, wTi))
		{
			if (EnableCountdown())
				bResult = TRUE;
		}
	}

	m_csPCF8563.Unlock();

	return bResult;
}

//////////////////////////////////////////////////////////////////////
int CPCF8563::DecToBCD(int nDec) const
{
	return ((nDec / 10)<<4) | (nDec % 10);
}

//////////////////////////////////////////////////////////////////////
int CPCF8563::BCDToDec(int nBCD) const
{
	return (nBCD & 0x0f) + 10*(nBCD >> 4);
}
