#include "stdafx.h"
#include ".\recordpos.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////
CRecordPOS::CRecordPOS(SDIControlType type,int iCom,BOOL bTraceAscii,BOOL bTraceHex,BOOL bTraceEvents)
: CSDIControlRecord(type, iCom, 0,NULL,bTraceAscii, bTraceHex, bTraceEvents)
{
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
CRecordPOS::~CRecordPOS()
{
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CRecordPOS::Create(CWK_Profile& wkp, const CString& sSection)
{
	BOOL bReturn = FALSE;
	// Stimmt der Typ
	if (m_Type == SDICT_POS) 
	{
		// Basisklassenfunktion aufrufen
		if ( CreateRecord() ) 
		{
			// RS232 oeffnen
			if ( OpenCom(CBR_9600, 8, NOPARITY, ONESTOPBIT) ) 
			{
				bReturn = TRUE;
			}
			else 
			{
				WK_TRACE_ERROR(_T("COM%i %s Create OpenCom FAILED\n"),GetCOMNumber(), CSDIControl::NameOfEnum(m_Type));
			}
		}
		else 
		{
			WK_TRACE_ERROR(_T("COM%i %s Create FAILED\n"),GetCOMNumber(), CSDIControl::NameOfEnum(m_Type));
		}
	}
	return bReturn;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CRecordPOS::OnReceivedData(LPBYTE pData, DWORD dwLen)
{
	DWORD dw = 0;
	while (pData && dw<dwLen) 
	{
		unsigned char c = pData[dw++];
		if (isprint(c))
		{
			m_sSingleData += c;
		}
		else
		{
			if (   c == ASCII_LF
				&& !m_sSingleData.IsEmpty())
			{
				//
				MySendAlarm();
			}
			else if (c == ASCII_FF)
			{
				MySendAlarm();
				// alarm zurücksetzen
				IndicateAlarm(1,0);
			}
			else
			{
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CRecordPOS::MySendAlarm()
{
	m_sSingleData.TrimLeft();
	m_sSingleData.TrimRight();
	m_sSingleData.Replace(_T('\t'),_T(' '));
	CIPCField cfField(FIELD_DBD_POS, m_sSingleData, 'C');
	m_DataFieldArrayActual.AddNoDuplicates(cfField);
	if (GetInput()->GetAlarmState(0))
	{
		// alarm liegt schon an Daten updaten
		UpdateAlarm(1);
	}
	else
	{
		// alarm melden
		IndicateAlarm(1,1);
	}
	ClearAllData();
	Sleep(10);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CRecordPOS::CheckForNewAlarmData()
{
	return TRUE;
}
