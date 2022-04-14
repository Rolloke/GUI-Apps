#include "stdafx.h"
#include <math.h>
#include ".\w83697.h"
#include ".\it8712.h"
#include "cioport.h"

/////////////////////////////////////////////////////////////////////////////
CW83697::CW83697(DWORD dwBaseAddress) : CW83627(dwBaseAddress)
{
}

/////////////////////////////////////////////////////////////////////////////
CW83697::~CW83697(void)
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CW83697::Open()
{
	m_bValidObject = FALSE;

	if (m_pIo && m_pIo->IsValid())
	{
		// Low byte des VendorID Registers lesen
		BYTE byVendorID_LO = ReadRegister(WINBOND_VENDOR_ID_REG);

		// Hi byte des VendorID Registers lesen
		BYTE byVendorID_HI = ReadRegister(WINBOND_VENDOR_ID_REG|HI_BYTE);

		// VendorID zusammensetzen
		m_wVendorID = MAKEWORD(byVendorID_LO, byVendorID_HI);

		// WinbondW83627 gefunden?
		if (m_wVendorID == 0x5CA3)
		{
			m_wDeviceID = (BYTE)ReadRegister(WINBOND_VID_DEVICE_ID_REG) & 0xfe;

			// Bank0 selection
			SwitchBank(BANK0);

			// ChipID Register lesen
			m_wChipID = (BYTE)ReadRegister(WINBOND_CIPID_REG);

			// ChipID korrekt?
			switch (m_wChipID)
			{
				case 0x60:
					m_sIdentifier	= _T("W83697HF");
					m_nMaxFans		= 2;
					m_nMaxTemps		= 2;

					m_bVCoreA		= TRUE;
					m_bVCoreB		= TRUE;
					m_b3_3V			= TRUE;
					m_b5V			= TRUE;
					m_b12V			= TRUE;
					m_bM12V			= TRUE;
					m_bM5V			= TRUE;

					m_bValidObject	= TRUE;
					break;
				default:
					break;
			}
		}
	}

	return m_bValidObject;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CW83697::ReadFanDivisors(int &nFan1Divisor, int &nFan2Divisor, int &nFan3Divisor)
{
	BOOL bResult = FALSE;

	if (IsValid())
	{
		// DivisorBit0 und 1 von Fan 1 und Fan 2 lesen
		BYTE byVal1 = ReadRegister(WINBOND_VID_FAN_DIVISOR_REG);
		
		// Bank0 selection
		SwitchBank(BANK0);
	
		// DivisorBit2 von Fan1, Fan2 lesen.
		BYTE byVal3 = ReadRegister(WINBOND_VBAT_MONITOR_CTRL_REG);
		
		// Alle 3 Bits zu den Divisionsfaktoren zusammen setzen
		nFan1Divisor = (byVal1>>4)&0x03 | (byVal3>>3)&0x04;
		nFan2Divisor = (byVal1>>6)&0x03 | (byVal3>>4)&0x04; 
		nFan3Divisor = 0;	// W83697 unterstützt nur 2 Lüfter
		nFan1Divisor = 1<<nFan1Divisor;
		nFan2Divisor = 1<<nFan2Divisor;
		nFan3Divisor = 0;	// W83697 unterstützt nur 2 Lüfter
		    
		bResult = TRUE;
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CW83697::WriteFanDivisors(int nFan1Divisor, int nFan2Divisor, int nFan3Divisor)
{
	BOOL bResult = FALSE;
	
	if (IsValid())
	{
		BYTE byFan1Div = (BYTE)ceil(log((double)nFan1Divisor)/log(2.0));
		BYTE byFan2Div = (BYTE)ceil(log((double)nFan2Divisor)/log(2.0));

		// DivisorBit0 und 1 von Fan 1 und Fan 2 setzen
		BYTE byVal = ReadRegister(WINBOND_VID_FAN_DIVISOR_REG) & 0x0f;
		byVal |= ((byFan2Div&0x03)<<6 | (byFan1Div&0x03)<<4);	
		WriteRegister(WINBOND_VID_FAN_DIVISOR_REG, byVal);
		
		// Bank0 selection
		SwitchBank(BANK0);
	
		// DivisorBit2 von Fan1 und Fan2 lesen.
		byVal = ReadRegister(WINBOND_VBAT_MONITOR_CTRL_REG) & 0x1f;
		byVal |= ((byFan2Div&0x04)<<4 | (byFan1Div&0x04)<<3);	
		WriteRegister(WINBOND_VBAT_MONITOR_CTRL_REG, byVal);

		bResult = TRUE;
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CW83697::ReadFanSpeeds(int &nFan1RPM,int &nFan2RPM, int &nFan3RPM)
{
	BOOL bResult = FALSE;
	int nFan1Div = 0;
	int nFan2Div = 0;
	int nFan3Div = 0;

	if (IsValid())
	{
		if (ReadFanDivisors(nFan1Div, nFan2Div, nFan3Div))
		{
			if (nFan1Div && nFan2Div)
			{
				// Bank0 selection
				SwitchBank(BANK0);
				
				// Counter von FAN1, FAN2, FAN3 lesen
				int nFan1 = ReadRegister(WINBOND_VRAM_FAN1);
				int nFan2 = ReadRegister(WINBOND_VRAM_FAN2);
				
				// In RPM umrechnen
				if (nFan1 && nFan1Div)
					nFan1RPM = 1350000/(nFan1*nFan1Div);	
				if (nFan2 && nFan2Div)
					nFan2RPM = 1350000/(nFan2*nFan2Div);
				nFan3RPM = 0; // W83697 unterstützt nur zwei Lüfter

				// Bank4 selection
				SwitchBank(BANK4);

				// Fancounter limits von FAN1 und FAN2 lesen
				BYTE byVal= ReadRegister(WINBOND_REALT_HARD_STATUS1_REG);
				if (getbit(byVal, 6))
					nFan1RPM = 0;
				if (getbit(byVal, 7))
					nFan2RPM = 0;
			
				bResult = TRUE;		
			}
		}
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CW83697::SetFanCountLimits(int nFan1Limit, int nFan2Limit, int nFan3Limit)
{
	BOOL bResult = FALSE;
	
	if (IsValid())
	{
		// Bank0 selection
		SwitchBank(BANK0);

		// Fan Speed Limits setzen
		if (WriteRegister(WINBOND_VRAM_FAN1_COUNT_LIMIT, nFan1Limit))
			bResult = WriteRegister(WINBOND_VRAM_FAN2_COUNT_LIMIT, nFan2Limit);
	}
	
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CW83697::GetFanCountLimits(int& nFan1Limit, int& nFan2Limit, int& nFan3Limit)
{
	BOOL bResult = FALSE;
	
	if (IsValid())
	{
		// Bank0 selection
		SwitchBank(BANK0);

		// Fan Speed Limits setzen
		nFan1Limit = (int)ReadRegister(WINBOND_VRAM_FAN1_COUNT_LIMIT);
		nFan2Limit = (int)ReadRegister(WINBOND_VRAM_FAN2_COUNT_LIMIT);
		nFan3Limit = (int)0; // W83697 unterstützt nur 2 Lüfter
		bResult = TRUE;
	}
	
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CW83697::ReadTemperatures(int &nTemp1, int &nTemp2, int &nTemp3)
{
	return  CW83627::ReadTemperatures(nTemp1, nTemp2, nTemp3);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CW83697::ReadVoltages(float &fVCoreA, float &fVCoreB, float& f33V, float& f5V, float& f12V, float& fN12V, float& fN5V)
{
	BOOL bResult = FALSE;

	if (IsValid())
	{
		fVCoreA = 4.096f * ((float)ReadRegister(WINBOND_VRAM_VCOREA))/255.0f;	
		fVCoreB = 4.096f * ((float)ReadRegister(WINBOND_VRAM_VCOREB))/255.0f;
		f33V	= 4.096f * ((float)ReadRegister(WINBOND_VRAM_3_3V))/255.0f;
		f5V		= 4.096f * ((float)ReadRegister(WINBOND_VRAM_5V))/255.0f;	
		f5V		= f5V * (float)((50.0f+34.0f)/50.0f);							
		
		f12V	= 4.096f * ((float)ReadRegister(WINBOND_VRAM_12V))/255.0f;	
		f12V	= f12V * (float)((28.0f+10.0f)/10.0f);	// (28K + 10K)/10K

		fN12V	= (float)ReadRegister(WINBOND_VRAM_M12V) * 0.016f;	// *16mV
		float	fBeta = (float)(232.0f/(232.0+56.0f));	// 232K / (232K +56K)
		fN12V	= (fN12V - 3.6f * fBeta) / (1.0f-fBeta);

		fN5V	= (float)ReadRegister(WINBOND_VRAM_M5V) * 0.016f;	// *16mV
		float	fGamma = (float)(120.0f/(120.0f+56.0f));	// 120K / (120K +56K)
		fN5V	= (fN5V - 3.6f * fGamma) / (1.0f-fGamma);

		// Ales auf 2 Stellen hinter dem Komma runden	
		fVCoreA = ceil(fVCoreA*100.0f)/100.0f;
		fVCoreB = ceil(fVCoreB*100.0f)/100.0f;
		f33V = ceil(f33V*100.0f)/100.0f;
		f5V = ceil(f5V*100.0f)/100.0f;
		f12V = ceil(f12V*100.0f)/100.0f;
		fN12V = ceil(fN12V*100.0f)/100.0f;
		fN5V = ceil(fN5V*100.0f)/100.0f;
	
		bResult = TRUE;
	}

	return bResult;
}


