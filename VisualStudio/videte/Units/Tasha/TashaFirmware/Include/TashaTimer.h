#include "CInt64.h"

#ifndef __TASHATIMER_H__

CInt64 GetCycleCounter();
CInt64 GetTime();
CInt64 GetTimeSpan(const CInt64& i64Time);
void WaitMicro(int nMicroSecond);

#endif // __TASHATIMER_H__
