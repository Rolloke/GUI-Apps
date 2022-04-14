/* GlobalReplace: m\_CyclesPerSecond --> m_CyclesPerMicroSecond */
/* GlobalReplace: CTimer\.h --> WK_Timer.h */
/* GlobalReplace: CTimerClass --> CWK_Timer */
/////////////////////////////////////////////////////////////////////////////
// PROJECT:		MiCoUnitPCI
// FILE:		$Workfile: WK_Timer.h $
// ARCHIVE:		$Archive: /Project/CIPC/WK_Timer.h $
// CHECKIN:		$Date: 28.05.02 14:46 $
// VERSION:		$Revision: 7 $
// LAST CHANGE:	$Modtime: 28.05.02 14:46 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __WK_TIMER_H__
#define __WK_TIMER_H__

class AFX_EXT_CLASS CWK_Timer		    
{
public:
	CWK_Timer (void);
	
	// Milliseconds
	DWORD			SpecialGetTickCount(void);
	void			SpecialSleep(DWORD dwMillis);
	
	// Microseconds
	LARGE_INTEGER	GetMicroTicks(void);
	void			WaitMicroSecs(const LARGE_INTEGER &Micros);

	DWORD			GetCPUClk(){return m_CyclesPerMicroSecond.LowPart*1000000;}
private:
	static LARGE_INTEGER  m_CyclesPerMicroSecond;
	LARGE_INTEGER	m_PerfFreq;

	inline LARGE_INTEGER ReadRDTSC();
};

/////////////////////////////////////////////////////////////////////////////
inline LARGE_INTEGER CWK_Timer::ReadRDTSC()
{
	LARGE_INTEGER lRDTSC;
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

extern CWK_Timer theTimer;	// caller is responsible

inline DWORD WK_GetTickCount()
{
	return theTimer.SpecialGetTickCount();
}
inline DWORD WK_GetTimeSpan(DWORD dwStartTime)
{
	DWORD dwTC = theTimer.SpecialGetTickCount();

	if (dwStartTime <= dwTC)
		return dwTC-dwStartTime;
	else
		return (((DWORD)(-1)) - dwStartTime) + dwTC;
}
inline DWORD WK_GetTimeSpan(DWORD dwStartTime, DWORD dwCurrentTime)
{
	DWORD dwTC = dwCurrentTime;

	if (dwStartTime <= dwTC)
		return dwTC-dwStartTime;
	else
		return (((DWORD)(-1)) - dwStartTime) + dwTC;
}

inline void WK_Sleep(DWORD dwMilliSecs)
{
	theTimer.SpecialSleep(dwMilliSecs);
}



#endif	// __CTIMER_H__
