#include "stdafx.h"
#include <math.h>
#include ".\it8712.h"
#include "cioport.h"

/////////////////////////////////////////////////////////////////////////////
CIT8712::CIT8712(DWORD dwBaseAddress)  : CECBase(dwBaseAddress)
{
	m_pIoEC = NULL;
}

/////////////////////////////////////////////////////////////////////////////
CIT8712::~CIT8712(void)
{
	WK_DELETE(m_pIoEC);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CIT8712::IsValid()
{
	return m_pIo && m_pIoEC && m_bValidObject;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CIT8712::Open()
{
	m_bValidObject = FALSE;

	if (m_pIo && m_pIo->IsValid())
	{
		// Enter Configuration mode
		EnterMBPnP();

		// ChipID Register lesen
		m_wChipID = MAKEWORD(ReadRegister(ITE_CHIP_ID_LO), ReadRegister(ITE_CHIP_ID_HI));
		if (m_wChipID == 0x8712)
		{
			m_wChipVersion = ReadRegister(ITE_CHIP_VERSION) & 0x0f;

			WORD wECBaseAddr = MAKEWORD(GetConfig(ITE_EC_CONFIG, ITE_BASE_LO), GetConfig(ITE_EC_CONFIG, ITE_BASE_HI));
			if (wECBaseAddr != 0)
			{
				// Schnittstelle zum Environment Controller (EC) öffnen
				m_pIoEC = new CIoPort(wECBaseAddr);
				if (m_pIoEC && m_pIoEC->IsValid())
				{
					m_wVendorID = ReadECRegister(ITE_VENDOR_ID);
					
					// ITE Vendor ID
					if (m_wVendorID == 0x90) 
					{
						m_sIdentifier = _T("IT8712F");
						switch (m_wChipVersion)
						{
							case 4:
								m_sIdentifier += _T("-D");
								break;
							case 5:
								m_sIdentifier += _T("-G");
								break;
							case 6:
								m_sIdentifier += _T("-H");
								break;
							case 7:
								m_sIdentifier += _T("-I");
								break;
						}
						m_nMaxFans = 3; // Eigentlich 5 aber wir nutzen nur 3
						m_nMaxTemps= 3;
						m_bVCoreA		= TRUE;
						m_bVCoreB		= TRUE;
						m_b3_3V			= TRUE;
						m_b5V			= TRUE;
						m_b12V			= TRUE;
						m_bM12V			= TRUE;
						m_bM5V			= TRUE;

						m_bValidObject = TRUE;
					}
				}
			}
		}
	}


	return m_bValidObject;
}

/////////////////////////////////////////////////////////////////////////////
BYTE CIT8712::ReadRegister(WORD wRegNum)
{
	BYTE byVal = 0;

	if (m_pIo)
	{
		// Register lesen
		m_pIo->Output8(ITE_INDEX_PORT, wRegNum);
		byVal = m_pIo->Input8(ITE_DATA_PORT);
	}

	return byVal;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CIT8712::WriteRegister(WORD wRegNum, BYTE byVal)
{
	BOOL bResult = FALSE;
	
	if (m_pIo)
	{
		// Register schreiben
		m_pIo->Output8(ITE_INDEX_PORT, wRegNum);
		m_pIo->Output8(ITE_DATA_PORT, byVal);
		bResult = TRUE;
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BYTE CIT8712::ReadECRegister(WORD wRegNum)
{
	BYTE byVal = 0;

	if (m_pIoEC && m_pIoEC->IsValid())
	{
		// Register lesen
		m_pIoEC->Output8(ITE_EC_INDEX_PORT, wRegNum);
		byVal = m_pIoEC->Input8(ITE_EC_DATA_PORT);
	}

	return byVal;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CIT8712::WriteECRegister(WORD wRegNum, BYTE byVal)
{
	BOOL bResult = FALSE;
	
	if (m_pIoEC && m_pIoEC->IsValid())
	{
		// Register schreiben
		m_pIoEC->Output8(ITE_EC_INDEX_PORT, wRegNum);
		m_pIoEC->Output8(ITE_EC_DATA_PORT, byVal);
		bResult = TRUE;
	}

	return bResult;

}

/////////////////////////////////////////////////////////////////////////////
BOOL CIT8712::ReadFanDivisors(int &nFan1Divisor, int &nFan2Divisor, int &nFan3Divisor)
{
	BOOL bResult = FALSE;

	if (IsValid())
	{
		BYTE byVal = ReadECRegister(ITE_FAN_DIV);

		nFan1Divisor = byVal & 0x07;
		nFan2Divisor = (byVal>>3) & 0x07;
		nFan3Divisor = (byVal>>6) & 0x01;
		nFan1Divisor = 1<<nFan1Divisor;
		nFan2Divisor = 1<<nFan2Divisor;
		nFan3Divisor = (nFan3Divisor = 0) ? 2 : 8;
		bResult = TRUE;
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CIT8712::WriteFanDivisors(int nFan1Divisor, int nFan2Divisor, int nFan3Divisor)
{
	BOOL bResult = FALSE;
	
	if (IsValid())
	{
		BYTE byFan1Div = (BYTE)ceil(log((double)nFan1Divisor)/log(2.0));
		BYTE byFan2Div = (BYTE)ceil(log((double)nFan2Divisor)/log(2.0));
		BYTE byFan3Div = 0;
		if (nFan3Divisor == 2)
			byFan3Div = 0;
		else if (nFan3Divisor == 8)
			byFan3Div = 1;
		else
		{
			WK_TRACE(_T("CIT8712::WriteFanDivisors Fan3Div unsupported (%d)\n"), nFan3Divisor);
			byFan3Div = 1;
		}

		//WriteECRegister(ITE_FAN_DIV, nFan1Divisor);
		BYTE byVal = ((byFan1Div & 0x07) | (byFan2Div & 0x07)<<3 | (byFan3Div << 6));
		bResult = WriteECRegister(ITE_FAN_DIV, byVal);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CIT8712::ReadFanSpeeds(int &nFan1RPM,int &nFan2RPM, int &nFan3RPM)
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
				// Counter von FAN1, FAN2, FAN3 lesen
				int nFan1 = ReadECRegister(ITE_FAN1_READING);
				int nFan2 = ReadECRegister(ITE_FAN2_READING);
				int nFan3 = ReadECRegister(ITE_FAN3_READING);

				// In RPM umrechnen
				if (nFan1)
					nFan1RPM = 1350000/(nFan1*nFan1Div);	
				if (nFan2)
					nFan2RPM = 1350000/(nFan2*nFan2Div);
				if (nFan3)
					nFan3RPM = 1350000/(nFan3*nFan3Div);

				if (nFan1 >= ReadECRegister(ITE_FAN1_LIMIT))
					nFan1RPM = 0;
				if (nFan2 >= ReadECRegister(ITE_FAN2_LIMIT))
					nFan2RPM = 0;
				if (nFan3 >= ReadECRegister(ITE_FAN3_LIMIT))
					nFan3RPM = 0;
				
				// Test ob Fan überwachung eingeschaltet ist.
				BYTE byVal = ReadECRegister(ITE_FAN_MAIN_CONTROL);
				if (!getbit(byVal, 4))
					nFan1RPM = 0;
				if (!getbit(byVal, 5))
					nFan2RPM = 0;
				if (!getbit(byVal, 6))
					nFan3RPM = 0;

				bResult = TRUE;	
			}
		}
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CIT8712::ReadTemperatures(int &nTemp1, int &nTemp2, int &nTemp3)
{
	BOOL bResult = FALSE;

	if (IsValid())
	{
		BYTE byVal = ReadECRegister(ITE_ADC_TEMP_ENABLE);
		if (getbit(byVal, 0) || getbit(byVal, 3))
			nTemp1 = ReadECRegister(ITE_TEMPIN1);
		else
			nTemp1 = 0;

		if (getbit(byVal, 1) || getbit(byVal, 4))
			nTemp2 = ReadECRegister(ITE_TEMPIN2);
		else
			nTemp2 = 0;

		if (getbit(byVal, 2) || getbit(byVal, 5))
			nTemp3 = ReadECRegister(ITE_TEMPIN3);
		else
			nTemp3 = 0;

		bResult = TRUE;
	}

	return bResult;

}

/////////////////////////////////////////////////////////////////////////////
BOOL CIT8712::ReadVoltages(float &fVCoreA, float &fVCoreB, float& f33V, float& f5V, float& f12V, float& fN12V, float& fN5V)
{
	BOOL bResult = FALSE;
	int nVIn0, nVIn1, nVIn2, nVIn3, nVIn4, nVIn5, nVIn6, nVIn7, nVBat = 0;
	if (IsValid())
	{
		nVIn0 = ReadECRegister(ITE_VIN0_READING);
		nVIn1 = ReadECRegister(ITE_VIN1_READING);
		nVIn2 = ReadECRegister(ITE_VIN2_READING);
		nVIn3 = ReadECRegister(ITE_VIN3_READING);
		nVIn4 = ReadECRegister(ITE_VIN4_READING);
		nVIn5 = ReadECRegister(ITE_VIN5_READING);
		nVIn6 = ReadECRegister(ITE_VIN6_READING);
		nVIn7 = ReadECRegister(ITE_VIN7_READING);
		nVBat = ReadECRegister(ITE_VBAT_READING);


		WK_TRACE(_T("VIn0=%d, VIn1=%d, VIn2=%d, VIn3=%d, VIn4=%d, VIn5=%d, VIn6=%d, VIn7=%d, VBat=%d\n"),
					nVIn0, nVIn1, nVIn2, nVIn3, nVIn4, nVIn5, nVIn6, nVIn7, nVBat);


		// Diese Umrechnungen sind empirisch ermittelt und
		// mit evtl. völlig falsch
		fVCoreA = (float)nVIn0*0.016;	// VCore
		fVCoreB = (float)nVIn1*0.016;	// AGP Volt.
		f33V = (float)nVIn2*0.016;		// 3,3V
		f5V = (float)nVIn3*0.016*1.68;
		f12V = (float)nVIn4*0.016*3.93;
		fN12V = (float)nVIn5*0.016;
		fN12V = (1+4.3897)*fN12V - (4.3897*4.096);
		fN5V = (float)nVIn6*0.016;
		fN5V =  (1+2.221)*fN5V - (2.221*4.096);

		// Ales auf 2 Stellen hinter dem Komma runden	
		fVCoreA = ceil(fVCoreA*100.0)/100.0;
		fVCoreB = ceil(fVCoreB*100.0)/100.0;
		f33V = ceil(f33V*100.0)/100.0;
		f5V = ceil(f5V*100.0)/100.0;
		f12V = ceil(f12V*100.0)/100.0;
		fN12V = ceil(fN12V*100.0)/100.0;
		fN5V = ceil(fN5V*100.0)/100.0;

//		fVCoreA = (float)nVIn7;
//		fVCoreA = (float)nVBat;
//		WK_TRACE(_T("VCoreA=%f, VCoreB=%f, 3.3V=%f, 5V=%f, 12V=%f, -12V=%f, -5V=%f\n"),
//					fVCoreA, fVCoreB, f33V, f5V, f12V, fN12V, fN5V);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CIT8712::EnableMonitoring()
{
	BOOL bResult = FALSE;

	if (IsValid())
	{
		// Enable monitoring operation
		bResult = SetConfig(ITE_EC_CONFIG, ITE_ACTIVATE, 1);
/*
		BYTE byVal = ReadECRegister(ITE_ADC_TEMP_ENABLE);
		byVal = CLRBIT(byVal, 0);	// Sensor 0: Disable Thermal Diode mode
		byVal = CLRBIT(byVal, 1);	// Sensor 1: Disable Thermal Diode mode
		byVal = CLRBIT(byVal, 2);	// Sensor 2: Disable Thermal Diode mode
		byVal = SETBIT(byVal, 3);	// Sensor 0: Enable Thermal Resistor mode
		byVal = SETBIT(byVal, 4);	// Sensor 1: Enable Thermal Resistor mode
		byVal = SETBIT(byVal, 5);	// Sensor 2: Enable Thermal Resistor mode
		WriteECRegister(ITE_ADC_TEMP_ENABLE, byVal);
*/	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CIT8712::DisableMonitoring()
{
	BOOL bResult = FALSE;

	if (IsValid())
	{
		// Disable monitoring operation
		bResult = SetConfig(ITE_EC_CONFIG, ITE_ACTIVATE, 0);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CIT8712::SetFanCountLimits(int nFan1Limit, int nFan2Limit, int nFan3Limit)
{
	BOOL bResult = FALSE;
	
	if (IsValid())
	{
		if (WriteECRegister(ITE_FAN1_LIMIT, nFan1Limit))
		{
			if (WriteECRegister(ITE_FAN2_LIMIT, nFan2Limit))
			{
				bResult = WriteECRegister(ITE_FAN3_LIMIT, nFan3Limit);
			}
		}
	}
	
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CIT8712::GetFanCountLimits(int& nFan1Limit, int& nFan2Limit, int& nFan3Limit)
{
	BOOL bResult = FALSE;
	
	if (IsValid())
	{
		nFan1Limit = ReadECRegister(ITE_FAN1_LIMIT);
		nFan2Limit = ReadECRegister(ITE_FAN2_LIMIT);
		nFan3Limit = ReadECRegister(ITE_FAN3_LIMIT);

		bResult = TRUE;
	}
	
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
void CIT8712::EnterMBPnP()
{
	if (m_pIo)
	{
		// Enter Configuration mode
		m_pIo->Output8(ITE_INDEX_PORT, 0x87);
		m_pIo->Output8(ITE_INDEX_PORT, 0x01);
		m_pIo->Output8(ITE_INDEX_PORT, 0x55);
		m_pIo->Output8(ITE_INDEX_PORT, 0x55);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CIT8712::ExitMBPnP()
{
	if (m_pIo)
	{
		m_pIo->Output8(ITE_INDEX_PORT, 0x02);
		m_pIo->Output8(ITE_DATA_PORT, 0x02);
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CIT8712::SetConfig(BYTE byLdnNumber, BYTE byIndex, BYTE byData)
{
	BOOL bResult = FALSE;
    // EnterMBPnP();				// Enter IT8712 MB PnP mode
	if (WriteRegister(ITE_LDN_SELECT, byLdnNumber))
		bResult = WriteRegister(byIndex, byData);

	// ExitMBPnP();
	
	return bResult;
}
     
/////////////////////////////////////////////////////////////////////////////
BYTE CIT8712::GetConfig(BYTE byLDNNumber, BYTE byIndex)
{
    BYTE byVal;
    // EnterMBPnP();	
    WriteRegister(ITE_LDN_SELECT, byLDNNumber);
    byVal = ReadRegister(byIndex);
	// ExitMBPnP();
    return byVal;
}

