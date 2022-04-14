#include "stdafx.h"
#include <math.h>
#include ".\pc87372.h"
#include "cioport.h"

/////////////////////////////////////////////////////////////////////////////
CPC87372::CPC87372(DWORD dwBaseAddress)  : CECBase(dwBaseAddress)
{
	m_pIoEC = NULL;
}

/////////////////////////////////////////////////////////////////////////////
CPC87372::~CPC87372(void)
{
	WK_DELETE(m_pIoEC);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPC87372::IsValid()
{
	return m_pIo && m_pIoEC&& m_bValidObject;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPC87372::Open()
{
	m_bValidObject = FALSE;

	if (m_pIo)
	{
		// Enter Configuration mode
//		EnterMBPnP();

		// ChipID Register lesen
		m_wChipID = MAKEWORD(ReadRegister(PC87_SRID)>>5, ReadRegister(PC87_SID));
		WK_TRACE(_T("CPC87372::Open() ChipID=0x%x\n"), m_wChipID);
		
		if (m_wChipID == 0xF000)
		{
			m_wChipVersion = ReadRegister(PC87_SRID) & 0x01f;
			WK_TRACE(_T("CPC87372::Open() ChipRevision=0x%x\n"), m_wChipVersion);

			WORD wECBaseAddr = MAKEWORD(GetConfig(PC87_LDN_FSM, PC87_BASE_LO), GetConfig(PC87_LDN_FSM, PC87_BASE_HI));
			WK_TRACE(_T("CPC87372::Open() Baseadress=0x%x\n"), wECBaseAddr);
			if (wECBaseAddr != 0)
			{
				// Schnittstelle zum Environment Controller (EC) öffnen
				m_pIoEC = new CIoPort(wECBaseAddr);
				if (m_pIoEC)
				{
					m_sIdentifier.Format(_T("PC87372-%d"), m_wChipVersion);
					m_nMaxFans = 2; 
					m_nMaxTemps= 0;
					m_bValidObject = TRUE;
				}
			}
		}
	}


	return m_bValidObject;
}

/////////////////////////////////////////////////////////////////////////////
BYTE CPC87372::ReadRegister(WORD wRegNum)
{
	BYTE byVal = 0;

	if (m_pIo && m_pIo->IsValid())
	{
		// Register lesen
		m_pIo->Output8(PC87_INDEX_PORT, wRegNum);
		byVal = m_pIo->Input8(PC87_DATA_PORT);
	}

	return byVal;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPC87372::WriteRegister(WORD wRegNum, BYTE byVal)
{
	BOOL bResult = FALSE;
	
	if (m_pIo && m_pIo->IsValid())
	{
		// Register schreiben
		m_pIo->Output8(PC87_INDEX_PORT, wRegNum);
		m_pIo->Output8(PC87_DATA_PORT, byVal);
		bResult = TRUE;
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BYTE CPC87372::ReadECRegister(WORD wRegNum)
{
	BYTE byVal = 0;

	if (m_pIoEC && m_pIoEC->IsValid())
	{
		// Register lesen
		byVal = m_pIoEC->Input8(wRegNum);
	}

	return byVal;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPC87372::WriteECRegister(WORD wRegNum, BYTE byVal)
{
	BOOL bResult = FALSE;
	
	if (m_pIoEC && m_pIoEC->IsValid())
	{
		// Register schreiben
		m_pIoEC->Output8(wRegNum, byVal);
		bResult = TRUE;
	}

	return bResult;

}

/////////////////////////////////////////////////////////////////////////////
BOOL CPC87372::ReadFanDivisors(int &nFan1Divisor, int &nFan2Divisor, int &nFan3Divisor)
{
	BOOL bResult = FALSE;

	if (IsValid())
	{
		BYTE byVal = ReadECRegister(PC87_FMCSR1);
		nFan1Divisor = getbit(byVal, 7) ? 1 : 2;

		byVal = ReadECRegister(PC87_FMCSR2);
		nFan2Divisor = getbit(byVal, 7) ? 1 : 2;

		nFan3Divisor = 0; // PC87372 unterstützt nur zwei Lüfter
		bResult = TRUE;
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPC87372::WriteFanDivisors(int nFan1Divisor, int nFan2Divisor, int nFan3Divisor)
{
	BOOL bErr = FALSE;
	
	if (IsValid())
	{
		BYTE byVal = ReadECRegister(PC87_FMCSR1);
		if (nFan1Divisor == 1)
			byVal = setbit(byVal, 7, 1);
		else if (nFan1Divisor == 2)
			byVal = setbit(byVal, 7, 0);
		else
			bErr = TRUE;
		WriteECRegister(PC87_FMCSR2, byVal);

		byVal = ReadECRegister(PC87_FMCSR2);
		if (nFan2Divisor == 1)
			byVal = setbit(byVal, 7, 1);
		else if (nFan2Divisor == 2)
			byVal = setbit(byVal, 7, 0);
		else
			bErr = TRUE;
		WriteECRegister(PC87_FMCSR2, byVal);

	}

	return !bErr;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPC87372::ReadFanSpeeds(int &nFan1RPM,int &nFan2RPM, int &nFan3RPM)
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
				// Counter von FAN1, FAN2, FAN3 lesen
				int nFan1 = MAKEWORD(ReadECRegister(PC87_FMSPRL1), ReadECRegister(PC87_FMSPRH1));
				int nFan2 = MAKEWORD(ReadECRegister(PC87_FMSPRL2), ReadECRegister(PC87_FMSPRH2));
// WK_TRACE(_T("Fan1=%d, Fan2=%d\n"), nFan1, nFan2);

				// In RPM umrechnen
				if (nFan1)
					nFan1RPM = 60*32000/(nFan1*nFan1Div);	
				if (nFan2)
					nFan2RPM = 60*32000/(nFan2*nFan2Div);
				nFan3RPM = 0; // Unterstützt nur 2 Lüfter

				bResult = TRUE;	
			}
		}
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPC87372::ReadTemperatures(int &nTemp1, int &nTemp2, int &nTemp3)
{
	BOOL bResult = FALSE;

	if (IsValid())
	{

		bResult = TRUE;
	}

	return bResult;

}

/////////////////////////////////////////////////////////////////////////////
BOOL CPC87372::ReadVoltages(float &fVCoreA, float &fVCoreB, float& f33V, float& f5V, float& f12V, float& fN12V, float& fN5V)
{
	BOOL bResult = FALSE;

	if (IsValid())
	{
		WK_TRACE(_T("VCoreA=%f, VCoreB=%f, 3.3V=%f, 5V=%f, 12V=%f, -12V=%f, -5V=%f\n"),
					fVCoreA, fVCoreB, f33V, f5V, f12V, fN12V, fN5V);
	}

	return bResult;
}


/////////////////////////////////////////////////////////////////////////////
BOOL CPC87372::EnableMonitoring()
{
	BOOL bResult = FALSE;

	if (IsValid())
	{
		// Set GLOBEN-Bit to enable all Logical Devices
		BYTE byVal = ReadRegister(PC87_SIOCF1);
		byVal = setbit(byVal, 0, 1);
		WriteRegister(PC87_SIOCF1, byVal);

		// Enable monitoring operation
		bResult = SetConfig(PC87_LDN_FSM, PC87_ACTIVATE, 1);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPC87372::DisableMonitoring()
{
	BOOL bResult = FALSE;

	if (IsValid())
	{
		// Disable monitoring operation
		bResult = SetConfig(PC87_LDN_FSM, PC87_ACTIVATE, 0);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPC87372::SetFanCountLimits(int nFan1Limit, int nFan2Limit, int nFan3Limit)
{
	BOOL bResult = FALSE;
	
	if (IsValid())
	{
		if (WriteECRegister(PC87_FMTHRL1, LOBYTE(nFan1Limit)) && WriteECRegister(PC87_FMTHRH1, HIBYTE(nFan1Limit)))
			bResult = WriteECRegister(PC87_FMTHRL2, LOBYTE(nFan2Limit)) && WriteECRegister(PC87_FMTHRH2, HIBYTE(nFan2Limit));
	}
	
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPC87372::GetFanCountLimits(int& nFan1Limit, int& nFan2Limit, int& nFan3Limit)
{
	BOOL bResult = FALSE;
	
	if (IsValid())
	{
		nFan1Limit = MAKEWORD(ReadECRegister(PC87_FMTHRL1), ReadECRegister(PC87_FMTHRH1));
		nFan2Limit = MAKEWORD(ReadECRegister(PC87_FMTHRL2), ReadECRegister(PC87_FMTHRH2));
		nFan3Limit = 0; // PC87372 unterstützt nur zwei Lüfter

		bResult = TRUE;
	}
	
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
void CPC87372::EnterMBPnP()
{
	if (m_pIo && m_pIo->IsValid())
	{
		// Enter Configuration mode
		m_pIo->Output8(PC87_INDEX_PORT, 0x87);
		m_pIo->Output8(PC87_INDEX_PORT, 0x01);
		m_pIo->Output8(PC87_INDEX_PORT, 0x55);
		m_pIo->Output8(PC87_INDEX_PORT, 0x55);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CPC87372::ExitMBPnP()
{
	if (m_pIo && m_pIo->IsValid())
	{
		m_pIo->Output8(PC87_INDEX_PORT, 0x02);
		m_pIo->Output8(PC87_DATA_PORT, 0x02);
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPC87372::SetConfig(BYTE byLdnNumber, BYTE byIndex, BYTE byData)
{
	BOOL bResult = FALSE;
    // EnterMBPnP();				// Enter IT8712 MB PnP mode
	if (WriteRegister(PC87_LDN_SELECT, byLdnNumber))
		bResult = WriteRegister(byIndex, byData);

	// ExitMBPnP();
	
	return bResult;
}
     
/////////////////////////////////////////////////////////////////////////////
BYTE CPC87372::GetConfig(BYTE byLDNNumber, BYTE byIndex)
{
    BYTE byVal;
    // EnterMBPnP();	
    WriteRegister(PC87_LDN_SELECT, byLDNNumber);
    byVal = ReadRegister(byIndex);
	// ExitMBPnP();
    return byVal;
}

