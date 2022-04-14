/////////////////////////////////////////////////////////////////////////////
// PROJECT:		ISASpeedTest
// FILE:		$Workfile: CProfile.h $
// ARCHIVE:		$Archive: /Project/Tools/ISASpeedTest/CProfile.h $
// CHECKIN:		$Date: 5.08.98 9:54 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 5.08.98 9:54 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __CPERF_H__
#define __CPERF_H__

typedef struct
{
	LONG            Count;
	LARGE_INTEGER   TotalCycles;
} PROFILE_COUNTER, *PPROFILE_COUNTER;


// Number of calibration loops
#define CALIBRATION_LOOPS 10000000

class CProfile
{
public:
	CProfile(CString sCounterName, BOOL bAutoStart = FALSE);
	~CProfile();

	LARGE_INTEGER	GetTimeDiff();

	inline void Start();
	inline void Stop();

private:
	inline LARGE_INTEGER ReadRDTSC();

	LARGE_INTEGER	m_CalStart;
	LARGE_INTEGER	m_CalEnd;
	LARGE_INTEGER	m_PerfStart;
	LARGE_INTEGER	m_PerfEnd;
	LARGE_INTEGER	m_Seconds;
	BOOL			m_bStart;

	// Performance counters
	PROFILE_COUNTER m_PerfCounter;

	// Countername
	CString m_sCounterName; 

	// Frequency of the highperformance counter
	static LARGE_INTEGER  m_PerfFreq;

	// Number of cycles for a Start and Stop
	static LARGE_INTEGER  m_PerfStartStopCycles;

	// Number of cycles per second
	static LARGE_INTEGER  m_PerfCyclesPerSecond;
};

/////////////////////////////////////////////////////////////////////////////
inline void CProfile::Start()
{
	m_PerfStart = ReadRDTSC();

	m_PerfCounter.Count++;
	m_bStart = TRUE;
}

/////////////////////////////////////////////////////////////////////////////
inline void CProfile::Stop()
{
	m_PerfEnd = ReadRDTSC();

	m_PerfEnd.QuadPart -= m_PerfStart.QuadPart;
	m_PerfCounter.TotalCycles.QuadPart += m_PerfEnd.QuadPart;
	m_bStart = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
inline LARGE_INTEGER CProfile::ReadRDTSC()
{
	static LARGE_INTEGER lRDTSC;
	_asm {             
		_asm push eax  
		_asm push edx  
		_asm _emit 0Fh 
		_asm _emit 31h 
		_asm mov lRDTSC.LowPart, eax 
		_asm mov lRDTSC.HighPart, edx
		_asm pop edx            
		_asm pop eax            
	} 
	return lRDTSC;
}

#endif // __CPERF_H__



