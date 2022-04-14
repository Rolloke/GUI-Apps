/* GlobalReplace: m\_CyclesPerSecond --> m_CyclesPerMicroSecond */
/* GlobalReplace: CTimerClass --> CWK_Timer */
/////////////////////////////////////////////////////////////////////////////
// PROJECT:		MiCoUnitPCI
// FILE:		$Workfile: WK_Timer.cpp $
// ARCHIVE:		$Archive: /Project/CIPC/WK_Timer.cpp $
// CHECKIN:		$Date: 20.01.06 9:28 $
// VERSION:		$Revision: 7 $
// LAST CHANGE:	$Modtime: 19.01.06 16:10 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdCipc.h"
#include "WK_Timer.h"
#include "wkclasses\WK_TRACE.h"

LARGE_INTEGER	CWK_Timer::m_CyclesPerMicroSecond = {0};

// caller is responsible CWK_Timer theTimer;

////////////////////////////////////////////////////////////////////////////////////////////////
CWK_Timer::CWK_Timer (void)
{
	if (m_CyclesPerMicroSecond.QuadPart == 0)
	{
		LARGE_INTEGER PerfStart;
		LARGE_INTEGER PerfEnd;
		LARGE_INTEGER CalStart;
		LARGE_INTEGER CalEnd;
		LARGE_INTEGER Seconds;

		QueryPerformanceFrequency(&m_PerfFreq);
		QueryPerformanceCounter(&PerfStart);
		CalStart = ReadRDTSC();
		::Sleep(1000);
		CalEnd = ReadRDTSC();
		QueryPerformanceCounter(&PerfEnd);

		// Calculate the cycles/PERF_ENTRY, and the number of cycles/second
		Seconds.QuadPart = ((PerfEnd.QuadPart - PerfStart.QuadPart) * 1000 ) / m_PerfFreq.QuadPart;
		m_CyclesPerMicroSecond.QuadPart = Seconds.QuadPart ? ((CalEnd.QuadPart - CalStart.QuadPart) * 1000) / Seconds.QuadPart : 0;
		// convert to micro seconds
		m_CyclesPerMicroSecond.QuadPart /= (LONGLONG)1000000;

	}	
}

////////////////////////////////////////////////////////////////////////////////////////////////
void CWK_Timer::WaitMicroSecs(const LARGE_INTEGER &Micros)
{
	static LARGE_INTEGER Start, Stop;

	Start = ReadRDTSC();
	Stop.QuadPart  = Start.QuadPart +
					(m_CyclesPerMicroSecond.QuadPart * Micros.QuadPart);

	do
	{
		Start = ReadRDTSC();
	}while (Start.QuadPart < Stop.QuadPart);
}

////////////////////////////////////////////////////////////////////////////////////////////////
void CWK_Timer::SpecialSleep(DWORD dwMillis)
{
	LARGE_INTEGER ti64;
	ti64.QuadPart = dwMillis * 1000;

	WaitMicroSecs(ti64);
}

////////////////////////////////////////////////////////////////////////////////////////////////
DWORD CWK_Timer::SpecialGetTickCount(void)
{
	LARGE_INTEGER ti64;
	
	ti64.QuadPart = GetMicroTicks().QuadPart / (LONGLONG)1000;
	return ti64.LowPart;
}

////////////////////////////////////////////////////////////////////////////////////////////////
LARGE_INTEGER CWK_Timer::GetMicroTicks (void)
{
	LARGE_INTEGER ti64;
	ti64 = ReadRDTSC();
	// TESTHACK
	ti64.QuadPart = ti64.QuadPart / (m_CyclesPerMicroSecond.QuadPart);

	return ti64;
}

