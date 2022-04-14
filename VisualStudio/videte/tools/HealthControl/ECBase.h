#pragma once

class CIoPort;

// pure virtual class
class __declspec(dllexport) CECBase
{
public:
	CECBase(DWORD dwBaseAddress) = NULL; 
	virtual ~CECBase(void) = NULL;
	virtual BOOL Open() = NULL;
	virtual BOOL IsValid() = NULL;

	virtual BOOL ReadFanDivisors(int &Fan1Divisor, int &Fan2Divisor, int &Fan3Divisor) = NULL;
	virtual BOOL WriteFanDivisors(int Fan1Divisor, int Fan2Divisor, int Fan3Divisor) = NULL;
	virtual BOOL ReadFanSpeeds(int &nFan1RPM, int &nFan2RPM, int &nFan3RPM) = NULL;
	virtual BOOL GetFanCountLimits(int& nFan1Limit, int& nFan2Limit, int& nFan3Limit) = NULL;
	virtual BOOL SetFanCountLimits(int nFan1Limit, int nFan2Limit, int nFan3Limit) = NULL;
	virtual BOOL ReadTemperatures(int &nTemp1, int &nTemp2, int &nTemp3) = NULL;
	virtual BOOL ReadVoltages(float &fVCoreA, float &fVCoreB, float& f33V, float& f5V, float& f12V, float& fN12V, float& fN5V) = NULL;

	virtual BOOL EnableMonitoring() = NULL;
	virtual BOOL DisableMonitoring() = NULL;

	CString GetIdentifier(){return m_sIdentifier;};
	int  GetMaxFans(){return m_nMaxFans;};
	int  GetMaxTemps(){return m_nMaxTemps;};

	BOOL HasVCoreASensor(){return m_bVCoreA;};
	BOOL HasVCoreBSensor(){return m_bVCoreB;};
	BOOL Has12VSensor(){return m_b12V;};
	BOOL Has5VSensor(){return m_b5V;};
	BOOL Has3_3VSensor(){return m_b3_3V;};
	BOOL HasN12VSensor(){return m_bM12V;};
	BOOL HasN5VSensor(){return m_bM5V;};

protected:
	virtual BYTE ReadRegister(WORD wRegNum) = NULL;
	virtual BOOL WriteRegister(WORD wRegNum, BYTE byVal) = NULL;
	virtual BOOL SwitchBank(WORD wBank){return FALSE;};
	
protected:
	CIoPort*	m_pIo;
	BOOL		m_bValidObject;
	WORD		m_wVendorID;
	WORD		m_wChipID;
	WORD		m_wDeviceID;
	WORD		m_wChipVersion;
	int			m_nMaxFans;
	int			m_nMaxTemps;
	BOOL		m_bVCoreA;
	BOOL		m_bVCoreB;
	BOOL		m_b3_3V;
	BOOL		m_b5V;
	BOOL		m_b12V;
	BOOL		m_bM12V;
	BOOL		m_bM5V;

	CString		m_sIdentifier;
};
