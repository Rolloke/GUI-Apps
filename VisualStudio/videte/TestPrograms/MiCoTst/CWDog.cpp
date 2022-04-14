/////////////////////////////////////////////////////////////////////////////
// PROJECT:		MiCoTst
// FILE:		$Workfile: CWDog.cpp $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/MiCoTst/CWDog.cpp $
// CHECKIN:		$Date: 5.08.98 10:03 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 5.08.98 10:02 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "micotst.h"
#include "MiCoDefs.h"
#include "MiCoReg.h"
#include "stdafx.h"
#include "conio.h"
#include "CWDog.h"

///////////////////////////////////////////////////////////////////////////
CWatchDog::CWatchDog(WORD wIOBase)
{
	m_wIOBase			= wIOBase;
}

///////////////////////////////////////////////////////////////////////////
CWatchDog::~CWatchDog()
{
}

///////////////////////////////////////////////////////////////////////////
BOOL CWatchDog::Init(WORD wTi)
{
	BYTE	byReg	= 0;
	WORD	wF		= 0;

	if (m_wIOBase == 0)
		return FALSE;

	// Watchdog sperren
    byReg = (BYTE)CLRBIT(byReg, WATCHDOG_ENABLE_BIT); 
	m_IO.Output(m_wIOBase | WATCHDOG_OFFSET, byReg);

    // Initialisiere Zähler 2: Mode 3
    // Eingang: 5,9 MHz, Ausgang: 10000 Hz
	wF = 590;
	m_IO.Output(m_wIOBase + PTC_CONTROL_OFFSET, 0xb6);
    m_IO.Output(m_wIOBase + PTC_COUNT2_OFFSET,  LOBYTE(wF));
    m_IO.Output(m_wIOBase + PTC_COUNT2_OFFSET,  HIBYTE(wF));
    
	// Initialisiere Zähler 1: Mode 2
    //  Eingang: 10000 Hz, Ausgang: 5000 Hz
     wF = 2;
	m_IO.Output(m_wIOBase + PTC_CONTROL_OFFSET, 0x74);
    m_IO.Output(m_wIOBase + PTC_COUNT1_OFFSET,  LOBYTE(wF));
    m_IO.Output(m_wIOBase + PTC_COUNT1_OFFSET,  HIBYTE(wF));
    
	// Initialisiere Zähler 0: Mode 0
    // Eingang: 5000 Hz, Ausgang: wTi Sekunden
	m_IO.Output(m_wIOBase + PTC_CONTROL_OFFSET, 0x30);
    m_IO.Output(m_wIOBase + PTC_COUNT0_OFFSET,  LOBYTE(wTi));
    m_IO.Output(m_wIOBase + PTC_COUNT0_OFFSET,  HIBYTE(wTi));
    

	// Watchdog freigeben
	byReg = (BYTE)SETBIT(byReg, WATCHDOG_ENABLE_BIT); 
	m_IO.Output(m_wIOBase + WATCHDOG_OFFSET, byReg);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////
BOOL CWatchDog::Reset(WORD wTi)
{
	if (m_wIOBase == 0)
		return FALSE;

	// Initialisiere Zähler 0: Mode 0
	m_IO.Output(m_wIOBase + PTC_CONTROL_OFFSET, 0x30);
    m_IO.Output(m_wIOBase + PTC_COUNT0_OFFSET,  LOBYTE(wTi));
    m_IO.Output(m_wIOBase + PTC_COUNT0_OFFSET,  HIBYTE(wTi));

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////
BOOL CWatchDog::Piezo(BOOL bOn)
{
	if (m_wIOBase == 0)
		return FALSE;
	
	BYTE byReg = m_IO.Input(m_wIOBase + WATCHDOG_OFFSET);

	if (bOn)
		byReg = (BYTE)CLRBIT(byReg, RELAY3_BIT);
	else
		byReg = (BYTE)SETBIT(byReg, RELAY3_BIT);

	m_IO.Output(m_wIOBase + WATCHDOG_OFFSET, byReg);

	return TRUE;	
}

///////////////////////////////////////////////////////////////////////////
BOOL CWatchDog::CheckIt()
{
	WORD  wPTC0Start	= 0L;
    WORD  wPTC0Span		= 0L;
	DWORD dwCalcTime    = 5000L;
	float fPTCClk	    = 0.0;              
	
	Sleep(100);                        
	wPTC0Start 	= GetCurrentPTC0();
	Sleep(dwCalcTime);	
    wPTC0Span = wPTC0Start - GetCurrentPTC0();
    
    fPTCClk = (float)(2L * 590L * (DWORD)wPTC0Span / dwCalcTime) / 1000.0;
    
    // PTC Clock < 5 MHz
	if (fPTCClk < 5.0)
	{
		WK_TRACE(TRUE, "\tPD71054 \ttest failed (f=%01.2fMHz)\n", fPTCClk);
		return FALSE;
	}
	
	WK_TRACE(TRUE, "\tPD71054 \ttest passed (f=%01.2fMHz)\n", fPTCClk);
	
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////
WORD CWatchDog::GetCurrentPTC0()
{
	BYTE byPTC0Lo, byPTC0Hi;

	// PTC-Counter 1 auslesen	
/*	m_IO.Output(m_wIOBase | PTC_CONTROL_OFFSET, 0x40);
	byPTC0Lo  	= m_IO.Input(m_wIOBase | PTC_COUNT1_OFFSET);
	byPTC0Hi  	= m_IO.Input(m_wIOBase | PTC_COUNT1_OFFSET);	
*/
	// PTC-Counter 0 auslesen	
	m_IO.Output(m_wIOBase | PTC_CONTROL_OFFSET, 0x00);
	byPTC0Lo  	= m_IO.Input(m_wIOBase | PTC_COUNT0_OFFSET);
	byPTC0Hi  	= m_IO.Input(m_wIOBase | PTC_COUNT0_OFFSET);	

	return (WORD)MAKEWORD(byPTC0Lo, byPTC0Hi);
}
