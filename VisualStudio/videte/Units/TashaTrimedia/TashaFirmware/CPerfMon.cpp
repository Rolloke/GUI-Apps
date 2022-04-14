/////////////////////////////////////////////////////////////////////////////
// PROJECT:		Tashafirmare
// FILE:		$Workfile: CPerfMon.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/CPerfMon.cpp $
// CHECKIN:		$Date: 11.12.01 8:13 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 11.12.01 8:13 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
#include "tm.h"
#include "CPerfMon.h"

// @doc

CInt64	CPerfMon::m_u64PerfStartStopCycles(0);
int		CPerfMon::m_nInstance = 0;

/////////////////////////////////////////////////////////////////////////////
// @mfunc Legt ein Zählerobjekt an, und startet es ggf.
// @parm sCounterName Name des Zählers, der bei ShowResults verwendet wird.
// @parm bAutoStart TRUE -> Zähler wird unmittelbar nach dem Anlegen gestartet.
CPerfMon::CPerfMon(const char* sCounterName, BOOL bAutoStart)
{
	m_u64PerfStart	= 0;
	m_u64PerfEnd	= 0;
	m_bStart	= FALSE;
	
	strcpy(m_sCounterName, sCounterName);

	m_u64PerfCyclesPerSecond = (CInt64)CLOCK_SPEED;

	if (m_u64PerfStartStopCycles == (CInt64)0)
	{
		m_PerfCounter.u64Count		= 0;
		m_PerfCounter.u64TotCycles	= 0;
		m_PerfCounter.u64MaxCycles	= 0;
		m_PerfCounter.u64MinCycles	= CInt64(0xffffffff,0xffffff);
		CInt64	u64CalStart;
		CInt64	u64CalEnd;

		// Calibrate the overhead of PERF_ENTRY and PERF_EXIT, so that
		// they can be subtracted from the output
		WK_TRACE("TARGET: CALIBRATING PERFORMANCE TIMER....");
		u64CalStart = ReadRDTSC();
		for(DWORD dwI = 0; dwI < CALIBRATION_LOOPS; dwI++ )
		{
			Start();
			Stop();
		}
		u64CalEnd = ReadRDTSC();

		// Calculate the cycles/PERF_ENTRY, and the number of cycles/second
		m_u64PerfStartStopCycles = (u64CalEnd - u64CalStart) / (CInt64)CALIBRATION_LOOPS;

		WK_TRACE("TARGET: Ready\n");
	}

	m_PerfCounter.u64Count		= (CInt64)0;
	m_PerfCounter.u64TotCycles	= (CInt64)0;
	m_PerfCounter.u64MaxCycles	= (CInt64)0;
	m_PerfCounter.u64MinCycles	= CInt64(0xffffffff,0xffffff);
	m_nInstance++;

	// Soll Performancemessung gleich gestartet werrden?
	if (bAutoStart)
		Start();
}

/////////////////////////////////////////////////////////////////////////////
// @mfunc Zerstört das Objekt und gibt das Ergebnis über ShowResults aus.
CPerfMon::~CPerfMon()
{
	// Wurde Stop durchgeführt?
	if (m_bStart)
		Stop();
	m_nInstance--;

	ShowResults();
}

/////////////////////////////////////////////////////////////////////////////
// @mfunc	Gibt den Namen und den Zählerstand des Counters aus.
//			Zusätzlich wird die durchschnittliche Zeitdauer zwischen <mf CPerfMon.Start>
//			und <mf CPerfMon.Stop> in ys ausgegeben.
void CPerfMon::ShowResults()
{
	CInt64		u64totLengthMs	= 0;
	CInt64		u64avgLengthMs	= 0;
	CInt64		u64MaxMs		= 0;
	CInt64		u64MinMs		= 0;
	CInt64		u64Diff			= 0;

	static BOOL	bTitle			= TRUE;

	// Wurde Stop durchgeführt?
	if (m_bStart)
		Stop();

	// Liegt ein neues Ergebnis vor?
	if (m_PerfCounter.u64Count == (CInt64)0)
		return;

	if (bTitle)
	{
		WK_TRACE("TARGET: ===============================================================================\n");
		WK_TRACE("TARGET: %-15s%10s%8s(uS)%6s(uS)%6s(uS)%10s(uS)\n", "Function", "Count", "Min", "Max", "Diff", "Average");
		WK_TRACE("TARGET: ===============================================================================\n");
		bTitle = FALSE;
	}

	m_PerfCounter.u64TotCycles -= (m_PerfCounter.u64Count * m_u64PerfStartStopCycles);
	u64totLengthMs	= (m_PerfCounter.u64TotCycles * (CInt64)1000000)  / m_u64PerfCyclesPerSecond;
	u64avgLengthMs	= (u64totLengthMs / (CInt64)m_PerfCounter.u64Count);
	
	m_PerfCounter.u64MaxCycles -= m_u64PerfStartStopCycles;
	m_PerfCounter.u64MinCycles -= m_u64PerfStartStopCycles;
	u64MaxMs	= (m_PerfCounter.u64MaxCycles * (CInt64)1000000)  / m_u64PerfCyclesPerSecond;
	u64MinMs	= (m_PerfCounter.u64MinCycles * (CInt64)1000000)  / m_u64PerfCyclesPerSecond;
 	u64Diff		= u64MaxMs - u64MinMs;

	WK_TRACE("TARGET: %-15s%10s%10s%10s%10s%10s\n", m_sCounterName,
											(LPCSTR)m_PerfCounter.u64Count,
											(LPCSTR)u64MinMs,
											(LPCSTR)u64MaxMs,
											(LPCSTR)u64Diff,
											(LPCSTR)u64avgLengthMs);

	if (m_nInstance == 0)
	{
		WK_TRACE("TARGET: ------------------------------------------------------------------------------\n");	  

		WK_TRACE("TARGET: Cycles Per Second   : %s\n", (LPCSTR)m_u64PerfCyclesPerSecond);
		WK_TRACE("TARGET: Cycles in PERF_XXXX : %s\n", (LPCSTR)m_u64PerfStartStopCycles);
		WK_TRACE("TARGET: ------------------------------------------------------------------------------\n\n");	  
	}

	m_PerfCounter.u64Count		= (CInt64)0;
	m_PerfCounter.u64TotCycles	= (CInt64)0;
	m_PerfCounter.u64MaxCycles	= (CInt64)0;
	m_PerfCounter.u64MinCycles	= CInt64(0xffffffff,0xffffff);
}

