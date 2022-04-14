/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: WatchDog.cpp $
// ARCHIVE:		$Archive: /Project/SecurityLauncher/WatchDog.cpp $
// CHECKIN:		$Date: 4.07.06 15:05 $
// VERSION:		$Revision: 35 $
// LAST CHANGE:	$Modtime: 4.07.06 15:04 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "SecurityLauncher.h"
#include "WatchDog.h"

extern CSecurityLauncherApp theApp;

// WatchDog will be disabled at Windows NT

///////////////////////////////////////////////////////////////////////////
CWatchDog::CWatchDog()
{
	m_bTraceWatchDogTrigger = FALSE;
	m_wIOBase	= 0;
	m_nCardType = WD_UNKNOWN;
	m_wPtcCount0Offset = 0;
	m_wPtcCount1Offset = 0;
	m_wPtcCount2Offset = 0;
	m_wPtcControlOffset = 0;
	m_wWdRelOutOffset = 0;

	m_bEnabled = FALSE;
	m_bInit = FALSE;
	m_bUsePiezo = FALSE;

	m_dwPiezoFlags = 0;
	m_hWatchDogTriggerEvent = NULL;
	m_hPiezo = NULL;

	CWK_Profile wkp;
	m_bTraceWatchDogTrigger = (BOOL)wkp.GetInt(szSectionDebug, _T("TraceWatchDogTrigger"), m_bTraceWatchDogTrigger);
	wkp.WriteInt(szSectionDebug, _T("TraceWatchDogTrigger"), (int)m_bTraceWatchDogTrigger);
}
///////////////////////////////////////////////////////////////////////////
CWatchDog::~CWatchDog()
{
	WK_CLOSE_HANDLE(m_hWatchDogTriggerEvent);
	WK_CLOSE_HANDLE(m_hPiezo);
}
///////////////////////////////////////////////////////////////////////////
BOOL CWatchDog::Init(int nCardType, WORD wTi, BOOL bUsePiezo)
{
	WORD	wF		= 0;
	CString	sBuffer;
	
	// ML 13.02.2006: CWatchDog::Init muß bei Änderungen in der Systemverwaltung (Tasha, SaVic, JaCob...)
	// erneut aufgerufen werden, da ansonsten der Launcher den Watchdog evtl. zur falschen Karte triggert.
	// z.B. Die Jacob wird bei einem bestehenden System ausgetragen, und eine Tasha stattdessen aktiviert.
	// Bislang war es so, das der Watchdogtrigger vom Launcher bis zu einem Systemneustart weiterhin zur JaCob
	// geschickt wurde. Nun wird CWatchDog::Init alle 20 Sekunden aufgerufen. 
	if (nCardType == m_nCardType)
		return FALSE;
	else
	{
		WK_CLOSE_HANDLE(m_hWatchDogTriggerEvent);
		WK_CLOSE_HANDLE(m_hPiezo);

		m_bUsePiezo = bUsePiezo;
		m_nCardType = nCardType;
		if (m_nCardType == WD_COCO)
		{
			GetPrivateProfileString(_T("CoCoISA0"), _T("IOBase"), _T("0"), sBuffer.GetBuffer(IO_BASE_SIZE), IO_BASE_SIZE, _T("system.ini"));
			sBuffer.ReleaseBuffer();
			m_wIOBase = (WORD)_tcstoul(sBuffer, 0, 16);
			m_wPtcCount0Offset = COCO_PTC_COUNT0_OFFSET;
			m_wPtcCount1Offset = COCO_PTC_COUNT1_OFFSET;
			m_wPtcCount2Offset = COCO_PTC_COUNT2_OFFSET;
			m_wPtcControlOffset = COCO_PTC_CONTROL_OFFSET;
			m_wWdRelOutOffset = COCO_WD_REL_OUT_OFFSET;
		}
		else if (m_nCardType == WD_MICO)
		{
			// MicoPCI
			// nothing to do all in unit
			// Mit Hilfe diese Event läßt sich der Watchdog von Außen triggern
			m_hWatchDogTriggerEvent = CreateEvent(NULL, FALSE, FALSE, EV_MICO_TRIGGER_WATCHDOG);
			m_hPiezo = CreateEvent(NULL, FALSE, FALSE, EV_MICO_PIEZO);
			m_wIOBase = 1;
		}
		else if (m_nCardType == WD_JACOB)
		{
			// JaCob
			// nothing to do all in unit
			// Mit Hilfe diese Event läßt sich der Watchdog von Außen triggern
			m_hWatchDogTriggerEvent = CreateEvent(NULL, FALSE, FALSE, EV_JACOB_TRIGGER_WATCHDOG);
			m_hPiezo = CreateEvent(NULL, FALSE, FALSE, EV_JACOB_PIEZO);
			m_wIOBase = 1;
		}
		else if (m_nCardType == WD_SAVIC)
		{
			// Savic
			// nothing to do all in unit
			// Mit Hilfe diese Event läßt sich der Watchdog von Außen triggern
			m_hWatchDogTriggerEvent = CreateEvent(NULL, FALSE, FALSE, EV_SAVIC_TRIGGER_WATCHDOG);
			m_hPiezo = CreateEvent(NULL, FALSE, FALSE, EV_SAVIC_PIEZO);
			m_wIOBase = 1;
		}
		else if (m_nCardType == WD_TASHA)
		{
			// Tasha
			// nothing to do all in unit
			// Mit Hilfe diese Event läßt sich der Watchdog von Außen triggern
			m_hWatchDogTriggerEvent = CreateEvent(NULL, FALSE, FALSE, EV_TASHA_TRIGGER_WATCHDOG);
			m_hPiezo = CreateEvent(NULL, FALSE, FALSE, EV_TASHA_PIEZO);
			m_wIOBase = 1;
		}
		else if (m_nCardType == WD_Q)
		{
			// Q
			// nothing to do all in unit
			// Mit Hilfe diese Event läßt sich der Watchdog von Außen triggern
			m_hWatchDogTriggerEvent = CreateEvent(NULL, FALSE, FALSE, EV_Q_TRIGGER_WATCHDOG);
			m_hPiezo = CreateEvent(NULL, FALSE, FALSE, EV_Q_PIEZO);
			m_wIOBase = 1;
		}

		if (m_wIOBase == 0)
			return FALSE;

		// Beeper aus
		PiezoSetFlag(PF_OFF);

		// Watchdog sperren
		Disable();

		if (IsCoco())
		{
			// Initialisiere Zähler 2: Mode 3
			// coco
			// Eingang: 4,4 MHz, Ausgang: 1000 Hz
			wF = 4400;

			_outp((WORD)(m_wIOBase + m_wPtcControlOffset), 0xb6);
			_outp((WORD)(m_wIOBase + m_wPtcCount2Offset),  LOBYTE(wF));
			_outp((WORD)(m_wIOBase + m_wPtcCount2Offset),  HIBYTE(wF));
	    
			// Initialisiere Zähler 1: Mode 3
			//  Eingang: 1000 Hz, Ausgang: 0.1 Hz
			wF = 10000;
			_outp((WORD)(m_wIOBase + m_wPtcControlOffset), 0x74);
			_outp((WORD)(m_wIOBase + m_wPtcCount1Offset),  LOBYTE(wF));
			_outp((WORD)(m_wIOBase + m_wPtcCount1Offset),  HIBYTE(wF));
	    
			// Initialisiere Zähler 0: Mode 0
			// Eingang: 0.1 Hz, Ausgang: wTi Sekunden
			wTi /= 10; // ML Timergenauigkeit erhöhen!
			_outp((WORD)(m_wIOBase + m_wPtcControlOffset), 0x30);
			_outp((WORD)(m_wIOBase + m_wPtcCount0Offset),  LOBYTE(wTi));
			_outp((WORD)(m_wIOBase + m_wPtcCount0Offset),  HIBYTE(wTi));
		}
	    
		// Watchdog freigeben
		Enable();

		m_bInit = TRUE;
	}

	return TRUE;
}
///////////////////////////////////////////////////////////////////////////
BOOL CWatchDog::Reset(WORD wTi)
{
	if (m_wIOBase == 0)
		return FALSE;

	if (IsCoco())
	{
		// Initialisiere Zähler 0: Mode 0
		// Eingang: 0.1 Hz, Ausgang: wTi Sekunden
		wTi /= 10; // ML Timergenauigkeit erhöhen!
		_outp((WORD)(m_wIOBase + m_wPtcControlOffset), 0x30);
		_outp((WORD)(m_wIOBase + m_wPtcCount0Offset),  LOBYTE(wTi));
		_outp((WORD)(m_wIOBase + m_wPtcCount0Offset),  HIBYTE(wTi));
		if (m_bTraceWatchDogTrigger)
		{
			WK_TRACE(_T("DEBUG: CoCo WatchdogTrigger %u s \n"), wTi);
		}
	}
	else if (IsMico())
	{
		CWK_Profile wkp;
		wkp.WriteInt(_T("MiCoUnitPCI\\Device1\\General"),_T("WatchdogTimeOut"),wTi);
		if (m_bTraceWatchDogTrigger)
		{
			WK_TRACE(_T("DEBUG: MiCo WatchdogTrigger %u s \n"), wTi);
		}
		SetEvent(m_hWatchDogTriggerEvent);
	}
	else if (IsJaCob())
	{
		CWK_Profile wkp;
		wkp.WriteInt(_T("JaCobUnit\\Device1\\General"),_T("WatchdogTimeOut"),wTi);
		if (m_bTraceWatchDogTrigger)
		{
			WK_TRACE(_T("DEBUG: JaCob WatchdogTrigger %u s \n"), wTi);
		}
		SetEvent(m_hWatchDogTriggerEvent);
	}
	else if (IsSaVic())
	{
		CWK_Profile wkp;
		wkp.WriteInt(_T("SaVicUnit\\Device1\\General"),_T("WatchdogTimeOut"),wTi); 
		if (m_bTraceWatchDogTrigger)
		{
			WK_TRACE(_T("DEBUG: SaVic WatchdogTrigger %u s \n"), wTi);
		}
		SetEvent(m_hWatchDogTriggerEvent);
	}
	else if (IsTasha())
	{
		CWK_Profile wkp;
		wkp.WriteInt(_T("TashaUnit\\Device1\\General"),_T("WatchdogTimeOut"),wTi); 
		if (m_bTraceWatchDogTrigger)
		{
			WK_TRACE(_T("DEBUG: Tasha WatchdogTrigger %u s \n"), wTi);
		}
		SetEvent(m_hWatchDogTriggerEvent);
	}
	else if (IsQ())
	{
		CWK_Profile wkp;
		wkp.WriteInt(_T("QUnit\\General"),_T("WatchdogTimeOut"),wTi); 
		if (m_bTraceWatchDogTrigger)
		{
			WK_TRACE(_T("DEBUG: Q WatchdogTrigger %u s \n"), wTi);
		}
		SetEvent(m_hWatchDogTriggerEvent);
	}

	return TRUE;
}
///////////////////////////////////////////////////////////////////////////
BOOL CWatchDog::PiezoSetFlag(DWORD dwPiezoFlag)
{
	BYTE byReg = 0;
	BOOL bOn;
	DWORD dwOldPiezoFlags = m_dwPiezoFlags;

	if ( (m_wIOBase == 0) || (m_bUsePiezo==FALSE) )
	{
		return FALSE;
	}

	if (dwPiezoFlag>0)
	{
		m_dwPiezoFlags |= dwPiezoFlag;
	}
	else
	{
		m_dwPiezoFlags = 0;
	}
	bOn = (m_dwPiezoFlags>0);

	if ((dwOldPiezoFlags != m_dwPiezoFlags) || (dwPiezoFlag==0))
	{
		if (IsCoco())
		{
			byReg = (BYTE)_inp((WORD)(m_wIOBase + m_wWdRelOutOffset));
			byReg = bOn ? (BYTE)CLRBIT(byReg, RELAY3_BIT) : (BYTE)SETBIT(byReg, RELAY3_BIT);
			_outp((WORD)(m_wIOBase + m_wWdRelOutOffset), byReg);
			WK_STAT_LOG(_T("STM"),bOn,_T("CoCo STM"));
		}
		else if (IsMico())
		{
			CWK_Profile wkp;
			wkp.WriteInt(_T("MiCoUnitPCI\\Device1\\General"),_T("PiezoState"),bOn);
			SetEvent(m_hPiezo);
			WK_STAT_LOG(_T("STM"),bOn,_T("MiCo STM"));
		}
		else if (IsJaCob())
		{
			CWK_Profile wkp;
			wkp.WriteInt(_T("JaCobUnit\\Device1\\General"),_T("PiezoState"),bOn);
			SetEvent(m_hPiezo);
			WK_STAT_LOG(_T("STM"),bOn,_T("JaCob STM"));
		}
		else if (IsSaVic())
		{
			CWK_Profile wkp;
			wkp.WriteInt(_T("SavicUnit\\Device1\\General"),_T("PiezoState"),bOn);
			SetEvent(m_hPiezo);
			WK_STAT_LOG(_T("STM"),bOn,_T("Savic STM"));
		}
		else if (IsTasha())
		{
			CWK_Profile wkp;
			wkp.WriteInt(_T("TashaUnit\\Device1\\General"),_T("PiezoState"),bOn);
			SetEvent(m_hPiezo);
			WK_STAT_LOG(_T("STM"),bOn,_T("Tasha STM"));
		}
		WritePiezoStatus(bOn);
	}

	return TRUE;	
}
///////////////////////////////////////////////////////////////////////////
BOOL CWatchDog::PiezoClearFlag(DWORD dwPiezoFlag)
{
	BYTE byReg = 0;
	BOOL bOn;
	DWORD dwOldPiezoFlags = m_dwPiezoFlags;

	if ( (m_wIOBase == 0) || (m_bUsePiezo==FALSE) )
	{
		return FALSE;
	}

	if (dwPiezoFlag>0)
	{
		m_dwPiezoFlags &= ~dwPiezoFlag;
	}
	else
	{
		m_dwPiezoFlags = 0;
	}

	bOn = (m_dwPiezoFlags>0);

	if (dwOldPiezoFlags != m_dwPiezoFlags)
	{
		if (IsCoco())
		{
			byReg = (BYTE)_inp((WORD)(m_wIOBase + m_wWdRelOutOffset));
			byReg = bOn ? (BYTE)CLRBIT(byReg, RELAY3_BIT) : (BYTE)SETBIT(byReg, RELAY3_BIT);
			_outp((WORD)(m_wIOBase + m_wWdRelOutOffset), byReg);
			WK_STAT_LOG(_T("STM"),bOn,_T("CoCo STM"));
		}
		else if (IsMico())
		{
			CWK_Profile wkp;
			wkp.WriteInt(_T("MiCoUnitPCI\\Device1\\General"),_T("PiezoState"),bOn);
			SetEvent(m_hPiezo);
			WK_STAT_LOG(_T("STM"),bOn,_T("MiCo STM"));
		}
		else if (IsJaCob())
		{
			CWK_Profile wkp;
			wkp.WriteInt(_T("JaCobUnit\\Device1\\General"),_T("PiezoState"),bOn);
			SetEvent(m_hPiezo);
			WK_STAT_LOG(_T("STM"),bOn,_T("JaCob STM"));
		}
		else if (IsSaVic())
		{
			CWK_Profile wkp;
			wkp.WriteInt(_T("SaVicUnit\\Device1\\General"),_T("PiezoState"),bOn);
			SetEvent(m_hPiezo);
			WK_STAT_LOG(_T("STM"),bOn,_T("SaVic STM"));
		}
		else if (IsTasha())
		{
			CWK_Profile wkp;
			wkp.WriteInt(_T("TashaUnit\\Device1\\General"),_T("PiezoState"),bOn);
			SetEvent(m_hPiezo);
			WK_STAT_LOG(_T("STM"),bOn,_T("Tasha STM"));
		}

		WritePiezoStatus(bOn);
	}
	return TRUE;	
}
///////////////////////////////////////////////////////////////////////////
BOOL CWatchDog::HardwareReboot()
{
	if (m_wIOBase == 0)
	{
		return FALSE;
	}
	
	if (!m_bInit)
	{
		return FALSE;
	}

	Enable();
	Reset(0);
	
	return TRUE;	
}
///////////////////////////////////////////////////////////////////////////
BOOL CWatchDog::Enable()
{
	BYTE byReg	= 0;

	if (m_wIOBase == 0)
	{
		return FALSE;
	}

	if (m_bEnabled)
	{
		return TRUE;
	}

	if (IsCoco())
	{
		byReg = (BYTE)_inp((WORD)(m_wIOBase + m_wWdRelOutOffset));
		byReg = (BYTE)SETBIT(byReg, WATCHDOG_BIT); 
		_outp((WORD)(m_wIOBase + m_wWdRelOutOffset), byReg);
	}
	else if (IsMico())
	{
		// auto enable by Reset
	}
	else if (IsJaCob())
	{
		// auto enable by Reset
	}
	else if (IsSaVic())
	{
		// auto enable by Reset
	}
	else if (IsTasha())
	{
		// auto enable by Reset
	}

	m_bEnabled = TRUE;

	return TRUE;	
}
///////////////////////////////////////////////////////////////////////////
BOOL CWatchDog::Disable()
{
	BYTE	byReg	= 0;

	if (m_wIOBase == 0)
	{
		return FALSE;
	}

	if (!m_bEnabled)
	{
		return TRUE;
	}

	if (IsCoco())
	{
		byReg = (BYTE)_inp((WORD)(m_wIOBase + m_wWdRelOutOffset));
		byReg = (BYTE)CLRBIT(byReg, WATCHDOG_BIT); 
		_outp((WORD)(m_wIOBase + m_wWdRelOutOffset), byReg);
	}
	else if (IsMico())
	{
		// auto disable by MicoUnitPCI end
		CWK_Profile wkp;
		wkp.WriteInt(_T("MiCoUnitPCI\\Device1\\General"),_T("WatchdogTimeOut"),-1);
		SetEvent(m_hWatchDogTriggerEvent);
	}
	else if (IsJaCob())
	{
		// auto disable by JaCobUnit end
		CWK_Profile wkp;
		wkp.WriteInt(_T("JaCobUnit\\Device1\\General"),_T("WatchdogTimeOut"),-1);
		SetEvent(m_hWatchDogTriggerEvent);
	}
	else if (IsSaVic())
	{
		// auto disable by SaVicUnit end
		CWK_Profile wkp;
		wkp.WriteInt(_T("SaVicUnit\\Device1\\General"),_T("WatchdogTimeOut"),-1);
		SetEvent(m_hWatchDogTriggerEvent);
	}
	else if (IsTasha())
	{
		// auto disable by SaVicUnit end
		CWK_Profile wkp;
		wkp.WriteInt(_T("TashaUnit\\Device1\\General"),_T("WatchdogTimeOut"),-1);
		SetEvent(m_hWatchDogTriggerEvent);
	}
	
	m_bEnabled = FALSE;

	return TRUE;	
}
