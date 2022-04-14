#pragma once
#include "W83627.h"

class CW83627;
class CIoPort;
class __declspec(dllexport) CW83697 : public CW83627
{
public:
	CW83697(DWORD dwBaseAddress);
	virtual ~CW83697(void);
	virtual BOOL Open();
	virtual BOOL ReadFanDivisors(int &Fan1Divisor, int &Fan2Divisor, int &Fan3Divisor);
	virtual BOOL WriteFanDivisors(int Fan1Divisor, int Fan2Divisor, int Fan3Divisor);
	virtual BOOL ReadFanSpeeds(int &nFan1RPM, int &nFan2RPM, int &nFan3RPM);
	virtual BOOL GetFanCountLimits(int& nFan1Limit, int& nFan2Limit, int& nFan3Limit);
	virtual BOOL SetFanCountLimits(int nFan1Limit, int nFan2Limit, int nFan3Limit);
	virtual BOOL ReadTemperatures(int &nTemp1, int &nTemp2, int &nTemp3);
	virtual BOOL ReadVoltages(float &fVCoreA, float &fVCoreB, float& f33V, float& f5V, float& f12V, float& fN12V, float& fN5V);

};
