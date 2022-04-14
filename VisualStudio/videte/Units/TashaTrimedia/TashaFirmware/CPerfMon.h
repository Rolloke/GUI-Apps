/////////////////////////////////////////////////////////////////////////////
// PROJECT:		JaCobDll
// FILE:		$Workfile: CPerfMon.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/CPerfMon.h $
// CHECKIN:		$Date: 11.12.01 8:13 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 11.12.01 8:13 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __CPERF_H__
#define __CPERF_H__

// @doc

typedef struct
{
	CInt64	u64Count;
	CInt64	u64TotCycles;
	CInt64	u64MaxCycles;	
	CInt64	u64MinCycles;	

} PROFILE_COUNTER, *PPROFILE_COUNTER;


// Number of calibration loops
#define CALIBRATION_LOOPS 500000

// @class Mit Hilfe dieser Klasse l‰ﬂt sich ermitteln wie oft ein bestimmter
// Codeabschnitt eines Programmes aufgerufen wird und wie lange die Abarbeitung dieses
// Abschnittes im Mittel dauert. Die Messung ist sehr genau (ca. 1 Microsekunde)
class CPerfMon
{
public:
// @cmember Konstruktor
	CPerfMon(const char* sCounterName, BOOL bAutoStart = FALSE);

// @cmember Destruktor	
	~CPerfMon();

// @cmember Ergebnisausgabe
	void ShowResults();

// @cmember	Incrementiert den Z‰hler und startet ihn
	inline void Start();

// @cmember	Stopt den Z‰hler
	inline void Stop();

private:
	inline const CInt64 ReadRDTSC();

	CInt64	m_u64PerfStart;
	CInt64	m_u64PerfEnd;
	BOOL	m_bStart;

	// Performance counters
	PROFILE_COUNTER m_PerfCounter;

	// Countername
	char m_sCounterName[256]; 

	// Number of cycles per second
	CInt64  m_u64PerfCyclesPerSecond;

	// Number of cycles for a Start and Stop
	static CInt64	m_u64PerfStartStopCycles;

	// Anzahl der Instanzen
	static int		m_nInstance;
};

/////////////////////////////////////////////////////////////////////////////
// @mfunc Incrementiert den Z‰hler und startet ihn
inline void CPerfMon::Start()
{
	m_u64PerfStart = ReadRDTSC();

	m_PerfCounter.u64Count++;
	m_bStart = TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// @mfunc Stopt den Z‰hler
inline void CPerfMon::Stop()
{
	m_u64PerfEnd = ReadRDTSC();				    

	m_u64PerfEnd -= m_u64PerfStart;
	m_PerfCounter.u64TotCycles += m_u64PerfEnd;
	m_PerfCounter.u64MaxCycles = MAX(m_PerfCounter.u64MaxCycles, m_u64PerfEnd);
	m_PerfCounter.u64MinCycles = MIN(m_PerfCounter.u64MinCycles, m_u64PerfEnd);
	m_bStart = FALSE;
}

#ifdef WIN32
/////////////////////////////////////////////////////////////////////////////
inline const CInt64 CPerfMon::ReadRDTSC()
{
	LARGE_INTEGER lRDTSC;
		_asm
			{             
		_asm push eax  
		_asm push edx  
		_asm _emit 0Fh 
		_asm _emit 31h 
		_asm mov lRDTSC.LowPart, eax 
		_asm mov lRDTSC.HighPart, edx
		_asm pop edx            
		_asm pop eax            
	} 

	return (CInt64(lRDTSC.LowPart, lRDTSC.HighPart));
}

#else

/////////////////////////////////////////////////////////////////////////////
inline const CInt64 CPerfMon::ReadRDTSC()
{
	return CInt64(CYCLES(), HICYCLES());
}

#endif


#endif // __CPERF_H__



