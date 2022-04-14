#pragma once
#include "ECBase.h"

#define PC87_BASE_ADDRESS		0x0000
#define PC87_INDEX_PORT			0x2E
#define PC87_DATA_PORT			0x2F

// LDNs
#define PC87_LDN_FDC			0x00
#define PC87_LDN_PP				0x01
#define PC87_LDN_SP				0x03
#define PC87_LDN_SWC			0x04
#define PC87_LDN_KBC_MOUSE		0x05
#define PC87_LDN_KBC			0x06
#define PC87_LDN_GPIO			0x07
#define PC87_LDN_FSM			0x09

// Environment Controller Configuration Register
#define PC87_LDN_SELECT			0x07
#define PC87_SID				0x20
#define PC87_SIOCF1				0x21
#define PC87_SIOCF2				0x22
#define PC87_SRID				0x27

#define PC87_ACTIVATE			0x30
#define PC87_BASE_HI			0x60
#define PC87_BASE_LO			0x61

// Environment Controller Register
#define PC87_FMTHRL1			0x00
#define PC87_FMTHRH1			0x01
#define PC87_FMSPRL1			0x02
#define PC87_FMSPRH1			0x03
#define PC87_FMCSR1				0x04
#define PC87_FMTHRL2			0x05
#define PC87_FMTHRH2			0x06
#define PC87_FMSPRL2			0x07
#define PC87_FMSPRH2			0x08
#define PC87_FMCSR2				0x09

class CIoPort;
class __declspec(dllexport) CPC87372 : public CECBase
{
public:
	CPC87372(DWORD dwBaseAddress);
	virtual ~CPC87372(void);
	virtual BOOL Open();
	virtual BOOL IsValid();
 
	virtual BOOL ReadFanDivisors(int &Fan1Divisor, int &Fan2Divisor, int &Fan3Divisor);
	virtual BOOL WriteFanDivisors(int Fan1Divisor, int Fan2Divisor, int Fan3Divisor);
	virtual BOOL ReadFanSpeeds(int &nFan1RPM, int &nFan2RPM, int &nFan3RPM);
	virtual BOOL GetFanCountLimits(int& nFan1Limit, int& nFan2Limit, int& nFan3Limit);
	virtual BOOL SetFanCountLimits(int nFan1Limit, int nFan2Limit, int nFan3Limit);
	virtual BOOL ReadTemperatures(int &nTemp1, int &nTemp2, int &nTemp3);
	virtual BOOL ReadVoltages(float &fVCoreA, float &fVCoreB, float& f33V, float& f5V, float& f12V, float& fN12V, float& fN5V);

	virtual BOOL EnableMonitoring();
	virtual BOOL DisableMonitoring();

protected:
	virtual BYTE ReadRegister(WORD wRegNum);
	virtual BOOL WriteRegister(WORD wRegNum, BYTE byVal);

	BYTE ReadECRegister(WORD wRegNum);
	BOOL WriteECRegister(WORD wRegNum, BYTE byVal);

private:
	void EnterMBPnP();
	void ExitMBPnP();
	BOOL SetConfig(BYTE byLdnNumber, BYTE byIndex, BYTE byData);
	BYTE GetConfig(BYTE byLDNNumber, BYTE byIndex);

private:
	CIoPort* m_pIoEC;
};
