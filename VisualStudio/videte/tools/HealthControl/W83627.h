#pragma once
#include "ECBase.h"

#define WINBOND_BASE_ADDRESS		0x290
#define WINBOND_INDEX_PORT			0x05
#define WINBOND_DATA_PORT			0x06

#define HI_BYTE						0x8000
#define LO_BYTE						0x0000

#define BANK0						0
#define BANK1						1
#define BANK2						2
#define BANK3						3
#define BANK4						4
#define BANK5						5
#define BANK6						6

#define WINBOND_VRAM_VCOREA				0x20
#define WINBOND_VRAM_VCORE				0x20 // W83627THF
#define WINBOND_VRAM_VCOREB				0x21
#define WINBOND_VRAM_VIN0				0x21 // W83627THF
#define WINBOND_VRAM_3_3V				0x22
#define WINBOND_VRAM_VIN1				0x22 // W83627THF
#define WINBOND_VRAM_5V					0x23
#define WINBOND_VRAM_AVCC				0x23 // W83627THF
#define WINBOND_VRAM_12V				0x24
#define WINBOND_VRAM_VIN2				0x24 // W83627THF
#define WINBOND_VRAM_M12V				0x25
#define WINBOND_VRAM_M5V				0x26

#define WINBOND_VRAM_TEMP1				0x27
#define WINBOND_VRAM_FAN1				0x28
#define WINBOND_VRAM_FAN2				0x29
#define WINBOND_VRAM_FAN3				0x2A
#define WINBOND_VRAM_TEMP1_HIGH_LIMIT	0x39
#define WINBOND_VRAM_FAN1_COUNT_LIMIT	0x3B
#define WINBOND_VRAM_FAN2_COUNT_LIMIT	0x3C
#define WINBOND_VRAM_FAN3_COUNT_LIMIT	0x3D

#define WINBOND_CONFIGURATION_REG		0x40
#define WINBOND_VID_FAN_DIVISOR_REG		0x47
#define WINBOND_VID_DEVICE_ID_REG		0x49
#define WINBOND_PIN_CONTROL_REG			0x4B
#define WINBOND_BANK_SELECT_REG			0x4E
#define WINBOND_VENDOR_ID_REG			0x4F
#define WINBOND_CIPID_REG				0x58
#define WINBOND_VBAT_MONITOR_CTRL_REG	0x5D
#define WINBOND_REALT_HARD_STATUS1_REG	0x59
#define WINBOND_REALT_HARD_STATUS2_REG	0x5A

// Bank0 Register
#define WINBOND_VRM_OVT					0x18

// Bank1 Register
#define WINBOND_TEMP2_HI				0x50
#define WINBOND_TEMP2_LO				0x51
#define WINBOND_TEMP2_CONFIG			0x52

// Bank2 Register
#define WINBOND_TEMP3_HI				0x50
#define WINBOND_TEMP3_LO				0x51
#define WINBOND_TEMP3_CONFIG			0x52

// Bank4 Register
#define WINBOND_TEMP1_OFFSET			0x54
#define WINBOND_TEMP2_OFFSET			0x55
#define WINBOND_TEMP3_OFFSET			0x56

class CECBase;
class CIoPort;
class __declspec(dllexport) CW83627 : public CECBase
{
public:
	CW83627(DWORD dwBaseAddress);
	virtual ~CW83627(void);
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
	virtual BOOL SwitchBank(WORD wBank);
};
