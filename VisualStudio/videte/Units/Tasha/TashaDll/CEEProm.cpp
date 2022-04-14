/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: CEEProm.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaDll/CEEProm.cpp $
// CHECKIN:		$Date: 2.09.04 9:38 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 2.09.04 9:27 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CEEProm.h"
#include "CCodec.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
CEEProm::CEEProm(CCodec* pCodec)
{
	m_pCodec = pCodec;
}

//////////////////////////////////////////////////////////////////////
CEEProm::~CEEProm()
{
}

//////////////////////////////////////////////////////////////////////
BOOL CEEProm::ClearEEProm()
{
	BOOL bResult = TRUE;

	if (!m_pCodec)
		return FALSE;

	BYTE Buffer[256];
	ZeroMemory(Buffer, 256);
	bResult = m_pCodec->DoRequestWriteToEEProm(Buffer, 256, EE_BASE);

	if (bResult)
		ML_TRACE(_T("Clearing EEprom\n"));
	else
 		ML_TRACE_WARNING(_T("Can't clear EEProm\n"));

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CEEProm::ReadMagicHeader(DWORD& dwHeader)
{
	BOOL bResult = FALSE;

	if (m_pCodec)
		bResult = m_pCodec->DoRequestReadFromEEProm((BYTE*)&dwHeader, EE_MH_LEN, EE_MH_OFFSET);

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CEEProm::ReadSN(CString& sSN)
{
	BOOL bResult = FALSE;

	// ! Darf kein Kein UNI-Code sein.
	char SN[EE_SN_LEN+1] = {0};
	if (m_pCodec)
		bResult = m_pCodec->DoRequestReadFromEEProm((BYTE*)&SN[0], EE_SN_LEN, EE_SN_OFFSET);
	
	sSN = SN;

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CEEProm::WriteSN(const CString &sSN)
{
	BOOL bResult = TRUE;

	if (m_pCodec)
	{
		// Seriennummer auf geforderte Länge auffüllen.
		CStringA sAnsi = sSN;
		sAnsi += + CString(_T(' '), EE_SN_LEN);
		sAnsi = sAnsi.Left(EE_SN_LEN);

		bResult = m_pCodec->DoRequestWriteToEEProm((BYTE*)sAnsi.GetBuffer(), EE_SN_LEN, EE_SN_OFFSET);
		sAnsi.ReleaseBuffer();
		if (bResult)
		{
			DWORD dwMagicHeader = MAGIC_HEADER;
			bResult = m_pCodec->DoRequestWriteToEEProm((BYTE*)&dwMagicHeader, EE_MH_LEN, EE_MH_OFFSET);
		}
	}
	if (bResult)
		ML_TRACE(_T("Write SN <%s> successful to EEprom\n"), sSN);
	else
		ML_TRACE_WARNING(_T("Can't write  SN to EEProm\n"));

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CEEProm::ReadDongleInformation(WORD& wDongle)
{
	BOOL bResult = FALSE;

	if (m_pCodec)
		bResult = m_pCodec->DoRequestReadFromEEProm((BYTE*)&wDongle, EE_DO_LEN, EE_DO_OFFSET);

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CEEProm::WriteDongleInformation(WORD wDongle)
{
	BOOL bResult = FALSE;

	if (m_pCodec)
		bResult = m_pCodec->DoRequestWriteToEEProm((BYTE*)&wDongle, EE_DO_LEN, EE_DO_OFFSET);

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CEEProm::DumpEEProm()
{
	BOOL bResult = FALSE;
	BYTE Buffer[256];
	ZeroMemory(Buffer, 256);
	if (m_pCodec)
	{
		if (m_pCodec->DoRequestReadFromEEProm(Buffer, 256, EE_BASE))
		{
			bResult = TRUE;
			CString sText, sVal;
			for (WORD wI = 0; wI < 16; wI++)
			{
				sText = _T("");
				for (WORD wJ = 0; wJ < 16; wJ++)
				{	
					BYTE byVal = Buffer[wI*16+wJ];
					sVal.Format(_T("0x%02x "), (WORD)byVal);
					sText += sVal;
				}
				ML_TRACE(_T("Addr=0x%02x: %s\n"), (WORD)wI*16, sText);
			}
		}
	}
	
	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CEEProm::WriteSubsystemID(WORD wSubsystemID, WORD wSubsystemVendorID)
{
	BOOL bResult = FALSE;

	if (m_pCodec)
	{
		if (m_pCodec->DoRequestWriteToEEProm((BYTE*)&wSubsystemID, EE_SSID_LEN, EE_SSID_OFFSET))
			bResult = m_pCodec->DoRequestWriteToEEProm((BYTE*)&wSubsystemVendorID, EE_SSVID_LEN, EE_SSVID_OFFSET);
	}

	if (bResult)
		ML_TRACE(_T("SubsystemID/SubsystemVendorID written successfull to EEprom <0x%02x,0x%02x>\n"), wSubsystemID, wSubsystemVendorID);
	else
		ML_TRACE_WARNING(_T("Can't write  SubsystemID/SubsystemVendorID to EEProm\n"));

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CEEProm::ReadSubsystemID(WORD& wSubsystemID, WORD& wSubsystemVendorID)
{
	BOOL bResult  = FALSE;

	if (m_pCodec)
	{
		if (m_pCodec->DoRequestReadFromEEProm((BYTE*)&wSubsystemID, EE_SSID_LEN, EE_SSID_OFFSET))
			bResult = m_pCodec->DoRequestReadFromEEProm((BYTE*)&wSubsystemVendorID, EE_SSVID_LEN, EE_SSVID_OFFSET);
	}
	return bResult;
}
