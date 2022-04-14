/////////////////////////////////////////////////////////////////////////////
// PROJECT:		ISASpeedTest
// FILE:		$Workfile: CProfile.cpp $
// ARCHIVE:		$Archive: /Project/Tools/ISASpeedTest/CProfile.cpp $
// CHECKIN:		$Date: 5.08.98 9:54 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 5.08.98 9:54 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CProfile.h"

LARGE_INTEGER  CProfile::m_PerfFreq	= {0};
LARGE_INTEGER  CProfile::m_PerfStartStopCycles= {0};
LARGE_INTEGER  CProfile::m_PerfCyclesPerSecond= {0};

/////////////////////////////////////////////////////////////////////////////
CProfile::CProfile(CString sCounterName, BOOL bAutoStart)
{
	LARGE_INTEGER	perfStart;
	LARGE_INTEGER	perfEnd;

	m_CalStart.QuadPart	= 0;
	m_CalEnd.QuadPart	= 0;
	m_PerfStart.QuadPart= 0;
	m_PerfEnd.QuadPart	= 0;
	m_Seconds.QuadPart	= 0;
	m_bStart			= FALSE;
	m_sCounterName		= sCounterName;

	if (m_PerfCyclesPerSecond.QuadPart == 0)
	{
		// Calibrate the overhead of PERF_ENTRY and PERF_EXIT, so that
		// they can be subtracted from the output
		printf("CALIBRATING PERFORMANCE TIMER....\n");
		QueryPerformanceFrequency(&m_PerfFreq);
		QueryPerformanceCounter(&perfStart);
		m_CalStart = ReadRDTSC();
		for(DWORD dwI = 0; dwI < CALIBRATION_LOOPS; dwI++ )
		{
			Start();
			Stop();
		}
		m_CalEnd = ReadRDTSC();
		QueryPerformanceCounter(&perfEnd);

		// Calculate the cycles/PERF_ENTRY, and the number of cycles/second
		m_PerfStartStopCycles.QuadPart = (m_CalEnd.QuadPart - m_CalStart.QuadPart) / CALIBRATION_LOOPS;
		m_Seconds.QuadPart = ((perfEnd.QuadPart - perfStart.QuadPart) * 1000 ) / m_PerfFreq.QuadPart;
		m_PerfCyclesPerSecond.QuadPart = m_Seconds.QuadPart ? ((m_CalEnd.QuadPart - m_CalStart.QuadPart) * 1000) / m_Seconds.QuadPart : 0;
	
		printf("Cycles Per Second: %I64d (%lu MHz)\n", m_PerfCyclesPerSecond.QuadPart, (DWORD)m_PerfCyclesPerSecond.QuadPart / 1000000);
	}

	m_PerfCounter.Count = 0;
	m_PerfCounter.TotalCycles.QuadPart = 0;

	// Soll Performancemessung gleich gestartet werrden?
	if (bAutoStart)
		Start();
}

/////////////////////////////////////////////////////////////////////////////
CProfile::~CProfile()
{
	// Wurde Stop durchgeführt?
	if (m_bStart)
		Stop();
}

/////////////////////////////////////////////////////////////////////////////
LARGE_INTEGER CProfile::GetTimeDiff()
{
	LARGE_INTEGER TotLengthMs = {0};
	LARGE_INTEGER AvgLengthMs = {0};

	// Liegt ein neues Ergebnis vor?
	if (m_PerfCounter.Count == 0)
		return AvgLengthMs;

	m_PerfCounter.TotalCycles.QuadPart -= (m_PerfCounter.Count * m_PerfStartStopCycles.QuadPart);
	TotLengthMs.QuadPart = m_PerfCyclesPerSecond.QuadPart ? (m_PerfCounter.TotalCycles.QuadPart * 1000000)/
					m_PerfCyclesPerSecond.QuadPart : 0;
	AvgLengthMs.QuadPart = m_PerfCounter.Count ? TotLengthMs.QuadPart / m_PerfCounter.Count : 0;

	return AvgLengthMs;
}

