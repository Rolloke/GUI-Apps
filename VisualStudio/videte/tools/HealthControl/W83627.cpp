#include "stdafx.h"
#include <math.h>
#include ".\w83627.h"
#include ".\it8712.h"
#include "cioport.h"

/////////////////////////////////////////////////////////////////////////////
CW83627::CW83627(DWORD dwBaseAddress) : CECBase(dwBaseAddress)
{
}

/////////////////////////////////////////////////////////////////////////////
CW83627::~CW83627(void)
{

}

/////////////////////////////////////////////////////////////////////////////
BOOL CW83627::IsValid()
{
	return m_pIo && m_bValidObject;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CW83627::Open()
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
				case 0x21:
					m_sIdentifier	= _T("W83627HF");
					m_nMaxFans		= 3;
					m_nMaxTemps		= 3;
					m_bVCoreA		= TRUE;
					m_bVCoreB		= TRUE;
					m_b3_3V			= TRUE;
					m_b5V			= TRUE;
					m_b12V			= TRUE;
					m_bM12V			= TRUE;
					m_bM5V			= TRUE;
					m_bValidObject	= TRUE;
					break;
				case 0x90:
					m_sIdentifier	= _T("W83627THF");
					m_nMaxFans		= 3;
					m_nMaxTemps		= 3;
					m_bVCoreA		= TRUE;
					m_bVCoreB		= FALSE;
					m_b3_3V			= TRUE;
					m_b5V			= TRUE;
					m_b12V			= TRUE;
					m_bM12V			= TRUE;
					m_bM5V			= FALSE;
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
BYTE CW83627::ReadRegister(WORD wRegNum)
{
	BYTE byVal = 0;

	if (m_pIo && m_pIo->IsValid())
	{
		if (wRegNum & HI_BYTE)
		{
			// Hi byte access Bit setzen
			m_pIo->Output8(WINBOND_INDEX_PORT, WINBOND_BANK_SELECT_REG);
			byVal = m_pIo->Input8(WINBOND_DATA_PORT);
			m_pIo->Output8(WINBOND_DATA_PORT, (BYTE)setbit(byVal, 7, 1));
		}
		else
		{
			// Hi byte access Bit löschen
			m_pIo->Output8(WINBOND_INDEX_PORT, WINBOND_BANK_SELECT_REG);
			byVal = m_pIo->Input8(WINBOND_DATA_PORT);
			m_pIo->Output8(WINBOND_DATA_PORT, (BYTE)setbit(byVal, 7, 0));
		}

		// Register lesen
		m_pIo->Output8(WINBOND_INDEX_PORT, (BYTE)(wRegNum & 0xff));
		byVal = m_pIo->Input8(WINBOND_DATA_PORT);
	}

	return byVal;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CW83627::WriteRegister(WORD wRegNum, BYTE byVal)
{
	BOOL bResult = FALSE;
	
	if (m_pIo && m_pIo->IsValid())
	{
		if (wRegNum & HI_BYTE)
		{
			// Hi byte access Bit setzen
			m_pIo->Output8(WINBOND_INDEX_PORT, WINBOND_BANK_SELECT_REG);
			BYTE byTmp = m_pIo->Input8(WINBOND_DATA_PORT);
			m_pIo->Output8(WINBOND_DATA_PORT, (BYTE)setbit(byTmp, 7, 1));
		}
		else
		{
			// Hi byte access Bit löschen
			m_pIo->Output8(WINBOND_INDEX_PORT, WINBOND_BANK_SELECT_REG);
			BYTE byTmp = m_pIo->Input8(WINBOND_DATA_PORT);
			m_pIo->Output8(WINBOND_DATA_PORT, (BYTE)setbit(byTmp, 7, 0));
		}

		// Register schreiben
		m_pIo->Output8(WINBOND_INDEX_PORT, (BYTE)(wRegNum & 0xff));
		m_pIo->Output8(WINBOND_DATA_PORT, byVal);
		bResult = TRUE;
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CW83627::SwitchBank(WORD wBank)
{
	BOOL bResult = FALSE;

	if (m_pIo && m_pIo->IsValid())
	{
		BYTE byVal = ReadRegister(WINBOND_BANK_SELECT_REG);
		byVal = (byVal & 0xf8) | (BYTE)(wBank & 0x07);
		bResult  = WriteRegister(WINBOND_BANK_SELECT_REG, byVal);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CW83627::ReadFanDivisors(int &nFan1Divisor, int &nFan2Divisor, int &nFan3Divisor)
{
	BOOL bResult = FALSE;

	if (IsValid())
	{
		// DivisorBit0 und 1 von Fan 1 und Fan 2 lesen
		BYTE byVal1 = ReadRegister(WINBOND_VID_FAN_DIVISOR_REG);
		
		// DivisorBit0 und 1 von Fan 3 lesen
		BYTE byVal2 = ReadRegister(WINBOND_PIN_CONTROL_REG);

		// Bank0 selection
		SwitchBank(BANK0);
	
		// DivisorBit2 von Fan1, Fan2, Fan3 lesen.
		BYTE byVal3 = ReadRegister(WINBOND_VBAT_MONITOR_CTRL_REG);
		
		// Alle 3 Bits zu den Divisionsfaktoren zusammen setzen
		nFan1Divisor = (byVal1>>4)&0x03 | (byVal3>>3)&0x04;
		nFan2Divisor = (byVal1>>6)&0x03 | (byVal3>>4)&0x04; 
		nFan3Divisor = (byVal2>>6)&0x03 | (byVal3>>5)&0x04;
		nFan1Divisor = 1<<nFan1Divisor;
		nFan2Divisor = 1<<nFan2Divisor;
		nFan3Divisor = 1<<nFan3Divisor;
		    
		bResult = TRUE;
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CW83627::WriteFanDivisors(int nFan1Divisor, int nFan2Divisor, int nFan3Divisor)
{
	BOOL bResult = FALSE;
	
	if (IsValid())
	{
		BYTE byFan1Div = (BYTE)ceil(log((double)nFan1Divisor)/log(2.0));
		BYTE byFan2Div = (BYTE)ceil(log((double)nFan2Divisor)/log(2.0));
		BYTE byFan3Div = (BYTE)ceil(log((double)nFan3Divisor)/log(2.0));

		// DivisorBit0 und 1 von Fan 1 und Fan 2 setzen
		BYTE byVal = ReadRegister(WINBOND_VID_FAN_DIVISOR_REG) & 0x0f;
		byVal |= ((byFan2Div&0x03)<<6 | (byFan1Div&0x03)<<4);	
		WriteRegister(WINBOND_VID_FAN_DIVISOR_REG, byVal);
		
		// DivisorBit0 und 1 von Fan 3 setzen
		byVal = ReadRegister(WINBOND_PIN_CONTROL_REG) & 0x3F;
		byVal |= (byFan3Div<<6);	
		WriteRegister(WINBOND_PIN_CONTROL_REG, byVal);

		// Bank0 selection
		SwitchBank(BANK0);
	
		// DivisorBit2 von Fan1, Fan2, Fan3 lesen.
		byVal = ReadRegister(WINBOND_VBAT_MONITOR_CTRL_REG) & 0x1f;
		byVal |= ((byFan3Div&0x04)<<5 | (byFan2Div&0x04)<<4 | (byFan1Div&0x04)<<3);	
		WriteRegister(WINBOND_VBAT_MONITOR_CTRL_REG, byVal);

		bResult = TRUE;
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CW83627::ReadFanSpeeds(int &nFan1RPM,int &nFan2RPM, int &nFan3RPM)
{
	BOOL bResult = FALSE;
	int nFan1Div = 0;
	int nFan2Div = 0;
	int nFan3Div = 0;

	if (IsValid())
	{
		if (ReadFanDivisors(nFan1Div, nFan2Div, nFan3Div))
		{
			if (nFan1Div && nFan2Div && nFan3Div)
			{
				// Bank0 selection
				SwitchBank(BANK0);
				
				// Counter von FAN1, FAN2, FAN3 lesen
				int nFan1 = ReadRegister(WINBOND_VRAM_FAN1);
				int nFan2 = ReadRegister(WINBOND_VRAM_FAN2);
				int nFan3 = ReadRegister(WINBOND_VRAM_FAN3);
				
				// In RPM umrechnen
				if (nFan1 && nFan1Div)
					nFan1RPM = 1350000/(nFan1*nFan1Div);	
				if (nFan2 && nFan2Div)
					nFan2RPM = 1350000/(nFan2*nFan2Div);
				if (nFan3 && nFan3Div)
					nFan3RPM = 1350000/(nFan3*nFan3Div);

				// Bank4 selection
				SwitchBank(BANK4);

				// Fancounter limits von FAN1 und FAN2 lesen
				BYTE byVal= ReadRegister(WINBOND_REALT_HARD_STATUS1_REG);
				if (getbit(byVal, 6))
					nFan1RPM = 0;
				if (getbit(byVal, 7))
					nFan2RPM = 0;

				// Fancounter limits von FAN3 lesen
				byVal= ReadRegister(WINBOND_REALT_HARD_STATUS2_REG);
				if (getbit(byVal, 3))
					nFan3RPM = 0;
			
				bResult = TRUE;		
			}
		}
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CW83627::ReadTemperatures(int &nTemp1, int &nTemp2, int &nTemp3)
{
	BOOL bResult = FALSE;

	if (IsValid())
	{
			// Temperatur sensor 1 lesen
			nTemp1 = (int)(char)ReadRegister(WINBOND_VRAM_TEMP1);
			
			SwitchBank(BANK1);		
			BYTE byVal = ReadRegister(WINBOND_TEMP2_CONFIG);
			if (!getbit(byVal, 0))
				nTemp2 = (int)(char)(ReadRegister(WINBOND_TEMP2_HI)<<1 | ReadRegister(WINBOND_TEMP2_LO)>>7)/2;
			else
				nTemp2 = 0;

			SwitchBank(BANK2);
			byVal = ReadRegister(WINBOND_TEMP3_CONFIG);
			if (!getbit(byVal, 0))
				nTemp3 = (int)(char)(ReadRegister(WINBOND_TEMP2_HI)<<1 | ReadRegister(WINBOND_TEMP2_LO)>>7)/2;
			else
				nTemp3 = 0;

			bResult = TRUE;
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CW83627::ReadVoltages(float &fVCoreA, float &fVCoreB, float& f33V, float& f5V, float& f12V, float& fN12V, float& fN5V)
{
	BOOL bResult = FALSE;

	if (IsValid())
	{
		if (m_wChipID == 0x21) // W83627HF
		{
			fVCoreA = 4.096 * ((float)ReadRegister(WINBOND_VRAM_VCOREA))/255.0;	
			fVCoreB = 4.096 * ((float)ReadRegister(WINBOND_VRAM_VCOREB))/255.0;
			f33V	= 4.096 * ((float)ReadRegister(WINBOND_VRAM_3_3V))/255.0;
			f5V		= 4.096 * ((float)ReadRegister(WINBOND_VRAM_5V))/255.0;	
			f5V		= f5V * (float)((50.0+34.0)/50.0);							
			
			f12V	= 4.096 * ((float)ReadRegister(WINBOND_VRAM_12V))/255.0;	
			f12V	= f12V * (float)((28.0+10.0)/10.0);	// (28K + 10K)/10K

			fN12V	= (float)ReadRegister(WINBOND_VRAM_M12V) * 0.016;	// *16mV
			float	fBeta = (float)(232.0/(232.0+56.0));	// 232K / (232K +56K)
			fN12V	= (fN12V - 3.6 * fBeta) / (1.0-fBeta);

			fN5V	= (float)ReadRegister(WINBOND_VRAM_M5V) * 0.016;	// *16mV
			float	fGamma = (float)(120.0/(120.0+56.0));	// 120K / (120K +56K)
			fN5V	= (fN5V - 3.6 * fGamma) / (1.0-fGamma);

			// Ales auf 2 Stellen hinter dem Komma runden	
			fVCoreA = ceil(fVCoreA*100.0)/100.0;
			fVCoreB = ceil(fVCoreB*100.0)/100.0;
			f33V = ceil(f33V*100.0)/100.0;
			f5V = ceil(f5V*100.0)/100.0;
			f12V = ceil(f12V*100.0)/100.0;
			fN12V = ceil(fN12V*100.0)/100.0;
			fN5V = ceil(fN5V*100.0)/100.0;
		}
		else if (m_wChipID == 0x90) // W83627THF
		{
			// VCORE voltage detection method
			SwitchBank(BANK0);
			if (getbit(ReadRegister(WINBOND_VRM_OVT), 0))
				fVCoreA = (float)ReadRegister(WINBOND_VRAM_VCORE)*0.00488+0.69; //VRM9
			else
				fVCoreA = (float)ReadRegister(WINBOND_VRAM_VCORE)*0.016; // VRM8

			fVCoreB = 0;
			f33V	= 4.096 * ((float)ReadRegister(WINBOND_VRAM_VIN1))/255.0;	// ok

			f5V		= 4.096 * ((float)ReadRegister(WINBOND_VRAM_AVCC))/255.0;		// ok
			f5V		= f5V * (float)((51.0+34.0)/51.0);							

			f12V	= 4.096 * ((float)ReadRegister(WINBOND_VRAM_VIN0))/255.0;	// ok
			f12V	= f12V * (float)((28.0+10.0)/10.0);	// (28K + 10K)/10K

			fN12V	= (float)ReadRegister(WINBOND_VRAM_VIN2) * 0.016;	// *16mV
			float	fBeta = (float)(232.0/(232.0+56.0));	// 232K / (232K +56K)
			fN12V	= (fN12V - 3.6 * fBeta) / (1.0-fBeta);
			
			fN5V = 0;
		
			// Ales auf 2 Stellen hinter dem Komma runden	
			fVCoreA = ceil(fVCoreA*100.0)/100.0;
			fVCoreB = ceil(fVCoreB*100.0)/100.0;
			f33V = ceil(f33V*100.0)/100.0;
			f5V = ceil(f5V*100.0)/100.0;
			f12V = ceil(f12V*100.0)/100.0;
			fN12V = ceil(fN12V*100.0)/100.0;
			fN5V = ceil(fN5V*100.0)/100.0;

		}
		bResult = TRUE;

//		WK_TRACE(_T("VCoreA=%f, VCoreB=%f, 3.3V=%f, 5V=%f, 12V=%f, -12V=%f, -5V=%f\n"),
//					fVCoreA, fVCoreB, f33V, f5V, f12V, fN12V, fN5V);
	}

	return bResult;
}


/////////////////////////////////////////////////////////////////////////////
BOOL CW83627::EnableMonitoring()
{
	BOOL bResult = FALSE;

	if (IsValid())
	{
/*		// Enable monitoring operation
		BYTE byVal = ReadRegister(WINBOND_CONFIGURATION_REG);
		WriteRegister(WINBOND_CONFIGURATION_REG, (BYTE)SETBIT(byVal, 0));
	
		// Enable Temperature Sensor 2
		SwitchBank(BANK1);		
		byVal = ReadRegister(WINBOND_TEMP2_CONFIG);
		WriteRegister(WINBOND_TEMP2_CONFIG, CLRBIT(byVal, 0));

		// Enable Temperature Sensor 3
		SwitchBank(BANK2);		
		byVal = ReadRegister(WINBOND_TEMP3_CONFIG);
		WriteRegister(WINBOND_TEMP3_CONFIG, CLRBIT(byVal, 0));
*/	
		bResult = TRUE;
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CW83627::DisableMonitoring()
{
	BOOL bResult = FALSE;

	if (IsValid())
	{
/*
		// Disable monitoring operation
		BYTE byVal = ReadRegister(WINBOND_CONFIGURATION_REG);
		WriteRegister(WINBOND_CONFIGURATION_REG, (BYTE)CLRBIT(byVal, 0));

		// Disable Temperature Sensor 2
		SwitchBank(BANK1);		
		byVal = ReadRegister(WINBOND_TEMP2_CONFIG);
		WriteRegister(WINBOND_TEMP2_CONFIG, SETBIT(byVal, 0));

		// Disable Temperature Sensor 3
		SwitchBank(BANK2);		
		byVal = ReadRegister(WINBOND_TEMP3_CONFIG);
		WriteRegister(WINBOND_TEMP3_CONFIG, SETBIT(byVal, 0));
*/	
		bResult = TRUE;
}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CW83627::SetFanCountLimits(int nFan1Limit, int nFan2Limit, int nFan3Limit)
{
	BOOL bResult = FALSE;
	
	if (IsValid())
	{
		// Bank0 selection
		SwitchBank(BANK0);

		// Fan Speed Limits setzen
		if (WriteRegister(WINBOND_VRAM_FAN1_COUNT_LIMIT, nFan1Limit))
		{
			if (WriteRegister(WINBOND_VRAM_FAN2_COUNT_LIMIT, nFan2Limit))
			{
				bResult = WriteRegister(WINBOND_VRAM_FAN3_COUNT_LIMIT, nFan3Limit);
			}
		}
	}
	
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CW83627::GetFanCountLimits(int& nFan1Limit, int& nFan2Limit, int& nFan3Limit)
{
	BOOL bResult = FALSE;
	
	if (IsValid())
	{
		// Bank0 selection
		SwitchBank(BANK0);

		// Fan Speed Limits setzen
		nFan1Limit = (int)ReadRegister(WINBOND_VRAM_FAN1_COUNT_LIMIT);
		nFan2Limit = (int)ReadRegister(WINBOND_VRAM_FAN2_COUNT_LIMIT);
		nFan3Limit = (int)ReadRegister(WINBOND_VRAM_FAN3_COUNT_LIMIT);
		bResult = TRUE;
	}
	
	return bResult;
}

