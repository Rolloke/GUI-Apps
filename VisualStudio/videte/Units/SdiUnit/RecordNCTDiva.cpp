// RecordNCTDiva.cpp: implementation of the CRecordNCTDiva class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RecordNCTDiva.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////
CRecordNCTDiva::CRecordNCTDiva(SDIControlType type,
								 int iCom,
								 BOOL bTraceAscii,
								 BOOL bTraceHex,
								 BOOL bTraceEvents)
	: CRecordConfigurable(type, iCom, bTraceAscii, bTraceHex, bTraceEvents)
{
}
//////////////////////////////////////////////////////////////////////
CRecordNCTDiva::~CRecordNCTDiva()
{
	Close();
}
//////////////////////////////////////////////////////////////////////
BOOL CRecordNCTDiva::Create(CWK_Profile& wkp, const CString& sSection)
{
	BOOL bReturn = FALSE;
	// Stimmt der Typ
	if (m_Type == SDICT_NCT_DIVA) {
		// Basisklassenfunktion aufrufen
		if ( CreateRecord() ) 
		{
			// RS232 oeffnen
			if ( OpenCom(CBR_9600, 8, NOPARITY, ONESTOPBIT) ) 
			{
				bReturn = TRUE;
			}
			else {
				WK_TRACE_ERROR(_T("%s Create OpenCom FAILED\n"),
								CSDIControl::NameOfEnum(m_Type));
			}
		}
		else {
			WK_TRACE_ERROR(_T("%s Create FAILED\n"), CSDIControl::NameOfEnum(m_Type));
		}
	}
	else {
		WK_TRACE_ERROR(_T("Create WRONG Type %s\n"), CSDIControl::NameOfEnum(m_Type));
	}
	return bReturn;
}
//////////////////////////////////////////////////////////////////////
void CRecordNCTDiva::OnReceivedData(LPBYTE pData, DWORD dwLen)
{
/*
<ESC>=<STX>
<VT>  HIER STEHT IHRE         WERBUNG       <ENDE>
<ESC>=<SOH>
<VT>                                        <ENDE>
<VT>Rückgabe            Di 27.05.   EUR 0,00
<VT>Zurückspulen                  EUR   0,85
<VT>Zurückspulen        *2        EUR   1,70
<VT>Gesamt:     0,85 EUR                    <ENDE>
<ESC>p  <ENDE>
*/
	DWORD dw = 0;
	while (pData && dw<dwLen)
	{
		// <VT> starts a new data record
		if (pData[dw] == ASCII_VT)
		{
			ClearAllData();
		}
		else
		{
			// <ESC> starts a special command
			if (pData[dw] == ASCII_ESC)
			{
				ClearAllData();
			}
			if (pData[dw] < 0x20)
			{
				m_sWholeData += GetAsciiSymbol(pData[dw]);
			}
			else
			{
				m_sWholeData += pData[dw];
			}
			m_iReceivedTotal++;
		}

		// "<ESC>p  " or "<ESC>p<NUL>  " command for cash system
		if (pData[dw] == ' ')
		{
			if (   (m_sWholeData == _T("<ESC>p  "))
				|| (m_sWholeData == _T("<ESC>p<NUL>  "))
				)
			{
				m_sSingleData = _T("Schublade");
				StoreData(PARAM_ACCOUNT);
				m_wProtocolPositionAsAlarm = 1;
				TransmitDataToSecurity();
				ClearAllData();
			}
		}
/*		// <STX> ends a special command
		else if (pData[dw] == ASCII_STX)
		{
			if (m_sWholeData == _T("<ESC>=<STX>"))
			{
				CheckAndStoreVariableData();
				TransmitDataToSecurity();
				ClearAllData();
			}
		}
		// <SOH> ends a special command
		else if (pData[dw] == ASCII_SOH)
		{
			if (m_sWholeData == _T("<ESC>=<SOH>"))
			{
				m_sWholeData += _T(" (Endsequenz für Bondrucker)");
				CheckAndStoreVariableData();
				TransmitDataToSecurity();
				ClearAllData();
			}
		}
*/
		// Wenn alle Zeichen empfangen wurden, ist ein Datensatz komplett
		if (m_iReceivedTotal == 40)
		{
			CheckAndStoreVariableData();
			TransmitDataToSecurity();
			ClearAllData();
		}
		if (m_iReceivedTotal > 40)
		{
			WK_TRACE_ERROR(_T("%s Data > 40\n"), CSDIControl::NameOfEnum(m_Type));
		}
		dw++;
	} // while
}
//////////////////////////////////////////////////////////////////////
BOOL CRecordNCTDiva::CheckForNewAlarmData()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CRecordNCTDiva::TransmitDataToSecurity()
{
	if (m_wProtocolPositionAsAlarm != 0)
	{
		IndicateAlarm(1);
	}
}
