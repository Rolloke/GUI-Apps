// RecordACTAccessControl.cpp: implementation of the CRecordACTAccessControl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RecordACTAccessControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
CRecordACTAccessControl::CRecordACTAccessControl(SDIControlType type,
											     int iCom,
											     BOOL bTraceAscii,
											     BOOL bTraceHex,
											     BOOL bTraceEvents)
	: CRecordConfigurable(type, iCom, bTraceAscii, bTraceHex, bTraceEvents)
{
	m_sEndOfLine += ASCII_LF;
	m_sEndOfLine += ASCII_CR;
	m_sEndOfLine += ASCII_CR;
	m_iEndOfLineLen = m_sEndOfLine.GetLength();
}
/////////////////////////////////////////////////////////////////////////////
CRecordACTAccessControl::~CRecordACTAccessControl()
{
	Close();
}
//////////////////////////////////////////////////////////////////////
BOOL CRecordACTAccessControl::Create(CWK_Profile& wkp, const CString& sSection)
{
	BOOL bReturn = FALSE;
	// Stimmt der Typ
	if (m_Type == SDICT_ACCESS_ACT) {
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
void CRecordACTAccessControl::OnReceivedData(LPBYTE pData, DWORD dwLen)
{
	DWORD dw = 0;
	while (pData && dw<dwLen)
	{
		// Alle druckbaren Zeichen in den Buffer sichern
		if (   isprint(pData[dw])
			|| pData[dw] == ASCII_CR
			|| pData[dw] == ASCII_LF
			)
		{
			m_sWholeData += pData[dw];
		}
		else
		{
			CString sMsg;
			sMsg.Format(_T("COM%i received 0x%02x %s"), GetCOMNumber(), pData[dw], GetAsciiSymbol(pData[dw]));
			WK_STAT_PEAK(_T("AsciiInvalid"), 1, sMsg);
			// Wird hier ignoriert
		}
		// Wenn ein Trennzeichen kommt, ist ein Datensatz komplett
		if (m_sWholeData.Right(m_iEndOfLineLen) == m_sEndOfLine)
		{
			// First remove the CR_LF
			int iCompleteLen = m_sWholeData.GetLength();
			m_sWholeData = m_sWholeData.Left(iCompleteLen - m_iEndOfLineLen);
			CheckAndStoreVariableData();
			TransmitDataToSecurity();
			ClearAllData();
		}
		dw++;
	} // while
}
//////////////////////////////////////////////////////////////////////
BOOL CRecordACTAccessControl::CheckForNewAlarmData()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CRecordACTAccessControl::TransmitDataToSecurity()
{
	if (m_wProtocolPositionAsAlarm != 0)
	{
		IndicateAlarm(m_wProtocolPositionAsAlarm);
	}
}
