// RecordScanner.cpp: implementation of the CRecordScanner class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RecordScanner.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRecordScanner::CRecordScanner(SDIControlType type,
								   int iCom,
								   BOOL bTraceAscii,
								   BOOL bTraceHex,
								   BOOL bTraceEvents)
	: CSDIControlRecord(type, iCom, 0,NULL,bTraceAscii, bTraceHex, bTraceEvents)
{

}
//////////////////////////////////////////////////////////////////////
CRecordScanner::~CRecordScanner()
{
	Close();
}
//////////////////////////////////////////////////////////////////////
BOOL CRecordScanner::Create(CWK_Profile& wkp, const CString& sSection)
{
	BOOL bReturn = FALSE;
	// Stimmt der Typ
	if (m_Type == SDICT_SCANNER) {
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
void CRecordScanner::OnReceivedData(LPBYTE pData, DWORD dwLen)
{
	DWORD dw = 0;
	while (pData && dw<dwLen) 
	{
		char c = pData[dw++];
		if (isprint(c))
		{
			m_sSingleData += c;
		}
		else
		{
			if (!m_sSingleData.IsEmpty())
			{
				StoreAccount();
				IndicateAlarm(1);
				ClearAllData();
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CRecordScanner::CheckForNewAlarmData()
{
	return TRUE;
}
