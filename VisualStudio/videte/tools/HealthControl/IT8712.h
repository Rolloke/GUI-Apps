#pragma once
#include "ECBase.h"

#define ITE_BASE_ADDRESS		0x0000
#define ITE_INDEX_PORT			0x2E
#define ITE_DATA_PORT			0x2F

// LDNs
#define ITE_FDC_CONFIG			0x00
#define ITE_SERIAL1_CONFIG		0x01
#define ITE_SERIAL2_CONFIG		0x02
#define ITE_PARAL1_CONFIG		0x03
#define ITE_EC_CONFIG			0x04
#define ITE_KBC_CONFIG			0x05
#define ITE_MOUSE_CONFIG		0x06
#define ITE_GPIO_CONFIG			0x07
#define ITE_MIDI_CONFIG			0x08
#define ITE_GAME_CONFIG			0x09
#define ITE_IR_CONFIG			0x0A

// Environment Controller Configuration Register
#define ITE_CONFIGURATION		0x00
#define ITE_CONFIGURE_CONTROL	0x02
#define ITE_LDN_SELECT			0x07
#define ITE_CHIP_ID_HI			0x20
#define ITE_CHIP_ID_LO			0x21
#define ITE_CHIP_VERSION		0x22

#define ITE_ACTIVATE			0x30
#define ITE_BASE_HI				0x60
#define ITE_BASE_LO				0x61

// Environment Controller Register
#define ITE_FAN_DIV				0x0B
#define ITE_FAN_COUNTER_ENABLE	0x0C
#define ITE_FAN1_READING		0x0D
#define ITE_FAN2_READING		0x0E
#define ITE_FAN3_READING		0x0F
#define ITE_FAN1_LIMIT			0x10
#define ITE_FAN2_LIMIT			0x11
#define ITE_FAN3_LIMIT			0x12
#define ITE_FAN_MAIN_CONTROL	0x13
#define ITE_VIN0_READING		0x20
#define ITE_VIN1_READING		0x21
#define ITE_VIN2_READING		0x22
#define ITE_VIN3_READING		0x23
#define ITE_VIN4_READING		0x24
#define ITE_VIN5_READING		0x25
#define ITE_VIN6_READING		0x26
#define ITE_VIN7_READING		0x27
#define ITE_VBAT_READING		0x28
#define ITE_TEMPIN1				0x29
#define ITE_TEMPIN2				0x2A
#define ITE_TEMPIN3				0x2B
#define ITE_TEMP1_LOW_LIMIT		0x40
#define ITE_TEMP2_LOW_LIMIT		0x42
#define ITE_TEMP3_LOW_LIMIT		0x44
#define ITE_TEMP1_HIGH_LIMIT	0x41
#define ITE_TEMP2_HIGH_LIMIT	0x43
#define ITE_TEMP3_HIGH_LIMIT	0x45

#define ITE_ADC_TEMP_ENABLE		0x51
#define ITE_VENDOR_ID			0x58

#define ITE_EC_INDEX_PORT		0x05
#define ITE_EC_DATA_PORT		0x06

class CIoPort;
class __declspec(dllexport) CIT8712 : public CECBase
{
public:
	CIT8712(DWORD dwBaseAddress);
	virtual ~CIT8712(void);
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
