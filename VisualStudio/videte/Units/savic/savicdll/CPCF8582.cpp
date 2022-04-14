/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: CPCF8582.cpp $
// ARCHIVE:		$Archive: /Project/Units/SaVic/SavicDll/CPCF8582.cpp $
// CHECKIN:		$Date: 20.06.03 12:01 $
// VERSION:		$Revision: 11 $
// LAST CHANGE:	$Modtime: 20.06.03 11:50 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CPCF8582.h"
#include "CI2C.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
CPCF8582::CPCF8582(DWORD dwIOBase)
{
 	// I2C-Objekt für den Zugriff auf das EEProm
	m_pCI2C = new CI2C(dwIOBase, PCF8582_SLAVE_ADR);
}

//////////////////////////////////////////////////////////////////////
CPCF8582::~CPCF8582()
{
	WK_DELETE(m_pCI2C);
}

//////////////////////////////////////////////////////////////////////
BOOL CPCF8582::ClearEEProm()
{
	BOOL bResult = TRUE;

	if (!m_pCI2C)
		return FALSE;

	for (WORD wI = 0; wI < EE_SVI_OFFSET; wI++)
	{
		if (WriteToEEProm(wI, 0) == FALSE)
			bResult = FALSE;
		Sleep(15);
	}
	if (bResult)
		ML_TRACE(_T("Clearing EEprom\n"));
	else
 		ML_TRACE_WARNING(_T("Can't clear EEProm\n"));

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CPCF8582::WriteToEEProm(WORD wAddr, BYTE byVal)
{
	if (!m_pCI2C)
		return FALSE;

	return m_pCI2C->WriteToI2C(wAddr, byVal);
}

//////////////////////////////////////////////////////////////////////
BOOL CPCF8582::ReadFromEEProm(WORD wAddr, BYTE& byVal)
{
	if (!m_pCI2C)
		return FALSE;

	return m_pCI2C->ReadFromI2C(wAddr, byVal);
}

//////////////////////////////////////////////////////////////////////
BOOL CPCF8582::ReadMagicHeader(DWORD& dwHeader)
{
	BOOL bResult = FALSE;

	if (!m_pCI2C)
		return FALSE;

	BYTE by1=0, by2=0, by3=0, by4=0;
	if (m_pCI2C->ReadFromI2C(EE_MH_OFFSET + 0, by1))
	{
		if (m_pCI2C->ReadFromI2C(EE_MH_OFFSET + 1, by2))
		{
			if (m_pCI2C->ReadFromI2C(EE_MH_OFFSET + 2, by3))
			{
				if (m_pCI2C->ReadFromI2C(EE_MH_OFFSET + 3, by4))
				{
					dwHeader =  by1 | by2 << 8 | by3 << 16  | by4 << 24;
					bResult = TRUE;
				}
			}
		}
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CPCF8582::ReadSN(CString& sSN)
{
	char SN[17] = {0};

	if (!m_pCI2C)
		return FALSE;

	for (WORD wI = 0; wI < EE_SN_LEN; wI++)
	{
		if (!m_pCI2C->ReadFromI2C(EE_SN_OFFSET + wI, (BYTE&)SN[wI]))
			return FALSE;
	}
	
	sSN = SN;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CPCF8582::WriteSN(const CString &sSN)
{
	BOOL bResult = TRUE;

	if (!m_pCI2C)
		return FALSE;

	for (WORD wI = 0; wI < EE_SN_LEN; wI++)
	{
		if (WriteToEEProm(EE_SN_OFFSET + wI, (char)sSN.GetAt(wI)) == FALSE)
			bResult = FALSE;
		Sleep(15);
	}

	if (bResult)
	{
		WriteToEEProm(EE_MH_OFFSET + 3, MAGIC_HEADER>>24 & 0xff);
		Sleep(15);
		WriteToEEProm(EE_MH_OFFSET + 2, MAGIC_HEADER>>16 & 0xff);
		Sleep(15);
		WriteToEEProm(EE_MH_OFFSET + 1, MAGIC_HEADER>>8 & 0xff);
		Sleep(15);
		WriteToEEProm(EE_MH_OFFSET + 0, MAGIC_HEADER>>0 & 0xff);
		Sleep(15);
	
		ML_TRACE(_T("Write SN <%s> successful to EEprom\n"), sSN);
	}
	else
		ML_TRACE_WARNING(_T("Can't write  SN to EEProm\n"));


	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CPCF8582::ReadDongleInformation(WORD& wDongle)
{
	BOOL bResult = FALSE;

	BYTE byLo = 0;
	BYTE byHi = 0;

	if (!m_pCI2C)
		return FALSE;


	if (m_pCI2C->ReadFromI2C(EE_DO_OFFSET , byHi))
	{
		if (m_pCI2C->ReadFromI2C(EE_DO_OFFSET+1 , byLo))
		{
			wDongle = MAKEWORD(byLo, byHi);
			bResult = TRUE;
		}
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CPCF8582::WriteDongleInformation(WORD wDongle)
{
	BOOL bResult = FALSE;

	BYTE byLo = 0;
	BYTE byHi = 0;

	if (!m_pCI2C)
		return FALSE;

	if (WriteToEEProm(EE_DO_OFFSET , wDongle>>8))
	{
		Sleep(15);
		if (WriteToEEProm(EE_DO_OFFSET+1 , wDongle & 0xff))
			bResult = TRUE;
		Sleep(15);
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CPCF8582::DumpEEProm()
{
	BOOL bErr = FALSE;

	BYTE byVal = 0;
	CString sText, sVal;
	for (WORD wI = 0; wI < 16; wI++)
	{
		sText = _T("");
		for (WORD wJ = 0; wJ < 16; wJ++)
		{	
			if (ReadFromEEProm(wI*16+wJ, byVal))
			{
				sVal.Format(_T("0x%02x "), (WORD)byVal);
				sText += sVal;
			}
			else
				bErr = TRUE;
		}
		ML_TRACE(_T("Addr=0x%02x: %s\n"), (WORD)wI*16, sText);
	}
	
	return !bErr;
}

//////////////////////////////////////////////////////////////////////
BOOL CPCF8582::WriteSubsystemID(WORD wSubsystemID)
{
	BOOL bResult = FALSE;

	if (!m_pCI2C)
		return FALSE;

	if (WriteToEEProm(EE_SVI_OFFSET + 0, HIBYTE(wSubsystemID)))
	{
		Sleep(15);
		if (WriteToEEProm(EE_SVI_OFFSET + 1, LOBYTE(wSubsystemID)))
		{
			Sleep(15);
			if (WriteToEEProm(EE_SVI_OFFSET + 2, HIBYTE(SAVIC_SUBSYSTEM_VENDOR_ID)))
			{
				Sleep(15);
				if (WriteToEEProm(EE_SVI_OFFSET + 3, LOBYTE(SAVIC_SUBSYSTEM_VENDOR_ID)))
				{
					Sleep(15);
					bResult = TRUE;
				}
			}	
		}	
	}
	
	if (bResult)
		ML_TRACE(_T("SubsystemID written successfull to EEprom <0x%04x>\n"), wSubsystemID);
	else
		ML_TRACE_WARNING(_T("Can't write  SubsystemID to EEProm\n"));

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CPCF8582::ReadSubsystemID(WORD& wSubsystemID)
{
	BOOL bResult  = FALSE;
	BYTE byLoByte = 0;
	BYTE byHiByte = 0;

	if (!m_pCI2C)
		return FALSE;

	if (m_pCI2C->ReadFromI2C(EE_SVI_OFFSET + 0, byHiByte))
	{
		Sleep(15);
		if (m_pCI2C->ReadFromI2C(EE_SVI_OFFSET + 1, byLoByte))
		{
			wSubsystemID = MAKEWORD(byLoByte, byHiByte);
			bResult = TRUE;
			
		}	
	}

	return bResult;
}
