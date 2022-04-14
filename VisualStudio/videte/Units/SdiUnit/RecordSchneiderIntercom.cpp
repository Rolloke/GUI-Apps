#include "stdafx.h"
#include "Recordschneiderintercom.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////////////////
CRecordSchneiderIntercom::CRecordSchneiderIntercom(SDIControlType type,
												   int iCom,
												   BOOL bTraceAscii,
												   BOOL bTraceHex,
												   BOOL bTraceEvents)
  : CSDIControlRecord(type,iCom,0,NULL,bTraceAscii,bTraceEvents,bTraceHex)
{
}
//////////////////////////////////////////////////////////////////////////////////////////
CRecordSchneiderIntercom::~CRecordSchneiderIntercom()
{
}
//////////////////////////////////////////////////////////////////////////////////////////
BOOL CRecordSchneiderIntercom::Create(CWK_Profile& wkp, const CString& sSection)
{
	BOOL bReturn = FALSE;
	// Stimmt der Typ
	if (m_Type == SDICT_SCHNEIDER_INTERCOM) 
	{
		m_AlarmNumbers.RemoveAll();
		m_AlarmNumbers.Load(wkp,sSection);
		WK_TRACE(_T("found %d Schneider Intercom Alarms\n"),m_AlarmNumbers.GetSize());

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
	else 
	{
		WK_TRACE_ERROR(_T("COM%i Create WRONG Type %s\n"),GetCOMNumber(), CSDIControl::NameOfEnum(m_Type));
	}
	return bReturn;
}
//////////////////////////////////////////////////////////////////////////////////////////
void CRecordSchneiderIntercom::OnReceivedData(LPBYTE pData, DWORD dwLen)
{
	for (DWORD dw=0; pData && dw<dwLen;dw++)
	{
		BYTE bSingle = pData[dw];
		switch (bSingle) 
		{
		case ASCII_STX:
			m_sData.Empty();
			break;
		case ASCII_ETX:
			ScanData();
			SendACK();
			break;
		default:
			m_sData += (char)bSingle;
			break;
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////////
void CRecordSchneiderIntercom::ScanData()
{
	CString sRR,sTask,sTT,sXXXX,sYYYY,sType,sSS;
	int pos = 0;

	TRACE(_T("%d %s\n"),m_sData.GetLength(),m_sData);

	if (m_sData.GetLength() == 18)
	{
		sRR = m_sData.Mid(0,2);
		sTask = m_sData.Mid(2,2);
		sTT = m_sData.Mid(4,2);
		sXXXX = m_sData.Mid(6,4);
		sYYYY = m_sData.Mid(10,4);
		sType = m_sData.Mid(14,2);
		sSS = m_sData.Mid(16,2);

		// fuehrende F löschen
		pos = sXXXX.ReverseFind(_T('F'));
		if (pos!=-1)
		{
			sXXXX = sXXXX.Mid(pos+1);
		}
		pos = sYYYY.ReverseFind(_T('F'));
		if (pos!=-1)
		{
			sYYYY = sYYYY.Mid(pos+1);
		}

		if (sTask == _T("42"))
		{
			// Gesprächsmeldungen
			if (   sType == _T("10")  // Ende Gespräch
				|| sType == _T("12")) // Anfang Gespräch
			{
				BOOL bFound = FALSE;
				for (int i=0;i<m_AlarmNumbers.GetSize() && !bFound;i++)
				{
					CString sSubscriberNr(m_AlarmNumbers.GetSubscriberNr(i));
					// Rufrichtung ist egal!
					if (   sSubscriberNr == sXXXX
						|| sSubscriberNr == sYYYY)
					{
						BOOL bAlarm = (sType == _T("12"));
						if (bAlarm)
						{
							m_sSingleData = sSubscriberNr;
							StoreNewDataBaseFields(FIELD_DBD_SUBSCRIBER);
							m_sSingleData = m_AlarmNumbers.GetComment(i);
							StoreNewDataBaseFields(FIELD_DBD_COMMENT);
						}
						IndicateAlarm((WORD)m_AlarmNumbers.GetNr(i),bAlarm ? 1 : 0);
						ClearAllData();
						bFound = TRUE;
					}
				}
			}
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("Schneider Intercom data too short %d\n"),m_sData.GetLength());
	}
}
//////////////////////////////////////////////////////////////////////////////////////////
BOOL CRecordSchneiderIntercom::CheckForNewAlarmData()
{
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////////////////
void CRecordSchneiderIntercom::InitializeResponses()
{
	m_byaACK.Add(ASCII_ACK);
}

