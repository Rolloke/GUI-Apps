// CWatchDog.cpp: implementation of the CWatchDog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DVStarter.h"
#include "CWatchDog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
CWatchDog::CWatchDog()
{
	m_hJaCobEventWD = OpenEvent(EVENT_ALL_ACCESS,FALSE, EV_JACOB_TRIGGER_WATCHDOG);
	if (m_hJaCobEventWD == NULL)
	{
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(sa);
		sa.bInheritHandle = TRUE;
		sa.lpSecurityDescriptor = NULL;
		m_hJaCobEventWD = CreateEvent(&sa, FALSE, FALSE, EV_JACOB_TRIGGER_WATCHDOG);
	}
	else
	{
		WK_TRACE(_T("JaCob Trigger Event already open\n"));
	}
	
	m_hSaVicEventWD = OpenEvent(EVENT_ALL_ACCESS,FALSE, EV_SAVIC_TRIGGER_WATCHDOG);
	if (m_hSaVicEventWD == NULL)
	{
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(sa);
		sa.bInheritHandle = TRUE;
		sa.lpSecurityDescriptor = NULL;
		m_hSaVicEventWD = CreateEvent(&sa, FALSE, FALSE, EV_SAVIC_TRIGGER_WATCHDOG);
	}
	else
	{
		WK_TRACE(_T("SaVic Trigger Event already open\n"));
	}
	
	m_hTashaEventWD = OpenEvent(EVENT_ALL_ACCESS,FALSE, EV_TASHA_TRIGGER_WATCHDOG);
	if (m_hTashaEventWD == NULL)
	{
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(sa);
		sa.bInheritHandle = TRUE;
		sa.lpSecurityDescriptor = NULL;
		m_hTashaEventWD = CreateEvent(&sa, FALSE, FALSE, EV_TASHA_TRIGGER_WATCHDOG);
	}
	else
	{
		WK_TRACE(_T("Tasha Trigger Event already open\n"));
	}

	m_hQEventWD = OpenEvent(EVENT_ALL_ACCESS,FALSE, EV_Q_TRIGGER_WATCHDOG);
	if (m_hQEventWD == NULL)
	{
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(sa);
		sa.bInheritHandle = TRUE;
		sa.lpSecurityDescriptor = NULL;
		m_hQEventWD = CreateEvent(&sa, FALSE, FALSE, EV_Q_TRIGGER_WATCHDOG);
	}
	else
	{
		WK_TRACE(_T("Q Trigger Event already open\n"));
	}

	Disable();


}

/////////////////////////////////////////////////////////////////////////////
CWatchDog::~CWatchDog()
{
	Disable();
	WK_CLOSE_HANDLE(m_hJaCobEventWD);
	WK_CLOSE_HANDLE(m_hSaVicEventWD);
	WK_CLOSE_HANDLE(m_hTashaEventWD);
	WK_CLOSE_HANDLE(m_hQEventWD);
}

/////////////////////////////////////////////////////////////////////////////
void CWatchDog::Trigger(int nTime)
{
	CWK_Profile prof;

	// Sofern eine JaCobUnit läuft, den WatchdogTimeout über den JaCobzweig in der Registry mitteilen
	if (WK_IS_RUNNING(GetAppnameFromId(WAI_JACOBUNIT_1)))
	{
		prof.WriteInt(_T("JaCobUnit\\Device1\\General"), _T("WatchDogTimeout"), nTime);
		if (m_hJaCobEventWD)
			SetEvent(m_hJaCobEventWD);
	}
	
	if (WK_IS_RUNNING(GetAppnameFromId(WAI_SAVICUNIT_1)))
	{
		prof.WriteInt(_T("SaVicUnit\\Device1\\General"), _T("WatchDogTimeout"), nTime);
		if (m_hSaVicEventWD)
			SetEvent(m_hSaVicEventWD);
	}
	if (WK_IS_RUNNING(GetAppnameFromId(WAI_TASHAUNIT_1)))
	{
		prof.WriteInt(_T("TashaUnit\\Device1\\General"), _T("WatchDogTimeout"), nTime);
		if (m_hTashaEventWD)
			SetEvent(m_hTashaEventWD);
	}
	if (WK_IS_RUNNING(GetAppnameFromId(WAI_QUNIT)))
	{
		prof.WriteInt(_T("QUnit\\General"), _T("WatchDogTimeout"), nTime);
		if (m_hQEventWD)
			SetEvent(m_hQEventWD);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CWatchDog::Disable()
{
	Trigger(-1);
}
