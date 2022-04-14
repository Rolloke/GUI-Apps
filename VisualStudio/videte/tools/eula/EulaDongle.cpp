#include "stdafx.h"
#include ".\euladongle.h"

CEulaDongle::CEulaDongle(const CString &sFileName) :
	 CWK_Dongle (sFileName)
{
	m_bAllowTimers = FALSE;
	m_bAllowMultiMonitor = FALSE;
	m_bAllowHTMLmaps= FALSE;
	m_bRunCommUnit = FALSE;
}

CEulaDongle::~CEulaDongle(void)
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CEulaDongle::WriteDCF(const CString &sFilename, DWORD dwDongleDecode)
{
	BOOL bOkay=TRUE;
	CStdioFile f;

	bOkay = f.Open(sFilename,CFile::modeCreate | CFile::modeWrite );
	if (bOkay) 
	{

		CString sValue;
		BOOL bValue;

		CTime ct=CTime::GetCurrentTime();
		sValue.Format(_T(";created %d.%d, %d:%d\n"),
			ct.GetDay(), ct.GetMonth(),
			ct.GetHour(), ct.GetMinute()
			);
		f.WriteString(sValue);

		// non-BOOL values first
				sValue.Format(_T("VersionNumber=%s\n"),GetVersionString());
		f.WriteString(sValue);

				sValue.Format(_T("Major=%d\n"), m_bMajorVersion );
		f.WriteString(sValue);

				sValue.Format(_T("Minor=%d\n"), m_bMinorVersion );
		f.WriteString(sValue);

		sValue.Format(_T("ProductCode=%d\n"),GetProductCode());
		f.WriteString(sValue);

		sValue.Format(_T("OEMCode=%d\n"),(WORD)GetOemCode());
		f.WriteString(sValue);

		//check new dongle features from dwDongleDecode and set in dongle.dcf
		CheckDongleExpansions(dwDongleDecode);

		// call the member function via x()
		// convert to string #x
#define TRANSFER(x) \
	bValue = x(); \
	sValue.Format(_T("%s=%d\n"), _T(#x), (int)bValue); \
	f.WriteString(sValue);
// CAVEAT all Transfers as ONE , so all will get the SAME
#include "AllDongleTransfers.h"

		sValue=_T("MagicNumber=234567\n");
		f.WriteString(sValue);
		sValue.Format(_T("NrOfHosts=%d\n"), m_iNrOfHosts);
		f.WriteString(sValue);
		sValue.Format(_T("NrOfCameras=%d\n"), m_iNrOfCameras);
		f.WriteString(sValue);
		sValue.Format(_T("NrOfIPCameras=%d\n"), m_iNrOfIPCameras);
		f.WriteString(sValue);

#undef TRANSFER

		f.Close();
	}
	return bOkay;
}
/////////////////////////////////////////////////////////////////////////////
void CEulaDongle::CheckDongleExpansions(DWORD dwDongleDecode)
{
	//alle Features ausschalten/resetten 
	if(dwDongleDecode & FLAG_RESET)
	{
		//zurück zur Grundeinstellung
		m_iNrOfHosts			= 5;
		m_bAllowTimers			= FALSE;
		m_bRunCommUnit			= FALSE;
		m_bAllowMultiMonitor	= FALSE;
		m_bAllowHTMLmaps		= FALSE;
	}

	//neue Features dazu addiert. 
	if(dwDongleDecode & FLAG_TIMER)
	{
		m_bAllowTimers = TRUE;
	}

	if(dwDongleDecode & FLAG_PTZ)
	{
		m_bRunCommUnit = TRUE;
	}

	if(dwDongleDecode & FLAG_MONITOR)
	{
		m_bAllowMultiMonitor = TRUE;
	}

	if(dwDongleDecode & FLAG_MAPS)
	{
		m_bAllowHTMLmaps = TRUE;
	}

	if(dwDongleDecode & FLAG_HOST0) //unbegrenzte Anzahl Gegenstationen
	{
		m_iNrOfHosts = 0;
	}
	if(dwDongleDecode & FLAG_HOST5)
	{
		m_iNrOfHosts = 5;
	}
	if(dwDongleDecode & FLAG_HOST10)
	{
		m_iNrOfHosts = 10;
	}
	if(dwDongleDecode & FLAG_HOST20)
	{
		m_iNrOfHosts = 20;
	}
	if(dwDongleDecode & FLAG_HOST30)
	{
		m_iNrOfHosts = 30;
	}
	if(dwDongleDecode & FLAG_HOST40)
	{
		m_iNrOfHosts = 40;
	}
	if(dwDongleDecode & FLAG_HOST50)
	{
		m_iNrOfHosts = 50;
	}
	if(dwDongleDecode & FLAG_HOST60)
	{
		m_iNrOfHosts = 60;
	}
}