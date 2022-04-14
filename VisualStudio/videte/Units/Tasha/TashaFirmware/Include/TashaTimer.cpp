#include "TashaTimer.h"

/////////////////////////////////////////////////////////////////////////
// Liefert den Cyclecounter als 64Bit integer
// Ca. 67 Takte
CInt64 GetCycleCounter()
{
	DWORD dwLo;
	DWORD dwHi;
	
	asm("%0 = cycles;  ": "=d" (dwLo) );
	asm("%0 = cycles2; ": "=d" (dwHi) );
	
	return CInt64(dwLo, dwHi);	
}

/////////////////////////////////////////////////////////////////////////
// Liefert die Zeit in microsekunden seit dem Start des Cyclecounters
// 1868 Takte
// Anmerkung: Da der 'operator /=' nicht zuverlässig funktioniert, sollte die GetTime-
//            Funktion nicht verwendet werden. Stattdessen lieber GetCycleCounter benutzen
CInt64 GetTime()
{
	return (GetCycleCounter() / (CInt64)(CORE_CLK/1000000));
}

/////////////////////////////////////////////////////////////////////////
// Liefert die Zeitdifferenz zwischen den angegebenen und dem aktuellen 
// Timer.
// 1892 Takte
// Anmerkung: Da der 'operator /=' nicht zuverlässig funktioniert, sollte die GetTimeSpan-
//            Funktion nicht verwendet werden. Stattdessen lieber GetCycleCounter benutzen
CInt64 GetTimeSpan(const CInt64& i64Time)
{
	return (GetTime() - i64Time);	
}

/////////////////////////////////////////////////////////////////////////
// Wartet eine vorgegebene Anzahl von microsekunden.
// 535 Takte
void WaitMicro(int nMicroSecond)
{
	CInt64 i64EndCounter = GetCycleCounter();
	i64EndCounter += CInt64((CORE_CLK/1000000)*nMicroSecond);
	i64EndCounter -= 535;					// 535 Takte overhead abziehen.
	
	while (GetCycleCounter() < i64EndCounter)
		asm("nop;");

	return;
}

