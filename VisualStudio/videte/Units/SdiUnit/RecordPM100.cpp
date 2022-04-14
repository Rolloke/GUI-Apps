#include "stdafx.h"
#include ".\recordpm100.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////////////////
CRecordPM100Alarm::CRecordPM100Alarm(SDIControlType type,
						   int iPort,
						   LPCTSTR szIPAddress,
						   BOOL bTraceAscii,
						   BOOL bTraceHex)
 : CSDIControlRecord(type, 0, iPort,szIPAddress, bTraceAscii, bTraceHex,FALSE)
{
}
///////////////////////////////////////////////////////////////////////////////////////////
CRecordPM100Alarm::~CRecordPM100Alarm()
{
}
///////////////////////////////////////////////////////////////////////////////////////////
BOOL CRecordPM100Alarm::Create(CWK_Profile& wkp, const CString& sSection)
{
	BOOL bRet = FALSE;
	if (m_iPort>0)
	{
		m_AlarmNumbers.RemoveAll();
		m_AlarmNumbers.Load(wkp,sSection);
		WK_TRACE(_T("found %d PM 100 IP Alarms\n"),m_AlarmNumbers.GetSize());
		for (int i=0;i<m_AlarmNumbers.GetSize();i++)
		{
			WK_TRACE(_T("PM100Alarm %d BFR%d TCC%d Tele%d\n"),m_AlarmNumbers.GetNr(i),
				m_AlarmNumbers.GetBFR(i),m_AlarmNumbers.GetTCC(i),m_AlarmNumbers.GetAlarmNr(i));
		}

		if (CreateRecord())
		{
			if (m_sIPAddress.IsEmpty())
			{
				bRet = CAsyncSocket::Create(m_iPort,SOCK_DGRAM,FD_READ);
			}
			else
			{
				bRet = CAsyncSocket::Create(m_iPort,SOCK_DGRAM,FD_READ,m_sIPAddress);
			}
		}
	}
	return bRet;
}
///////////////////////////////////////////////////////////////////////////////////////////
void CRecordPM100Alarm::OnReceivedData(LPBYTE pData, DWORD dwLen)
{
	DWORD dwVersion = 0;
	DWORD dwSLV = 0;
	DWORD dwTCC = 0;
	DWORD dwData = 0;
	DWORD dwData1 = 0;
	DWORD dwData2 = 0;
	DWORD dwTeleNr = 0;
	DWORD dwSubnummer = 0;
	DWORD dwAction = 0;
	DWORD dwAction2 = 0;
	char szBuffer[512];
	// Version;Slv ,Tcc ,Data,Dat1,Dat2;FNr ,FSNr,FAkt,Fak2;Text
	// sprintf(ip_msg, "%04x;%02x,%02x,%08x,%08x,%08x;%04x,%08x,%04x,%08x=%s",
	//	a) 0x100, version-number
	// 	c) prot.prot_slv, slv
	//	d) prot.tcc_num, tcc
	//	e) prot.data, data
	//	f) prot.data1, data 1
	//	g) prot.data2, data 2
	//	i) kopf->tele_nr, Alarmnumber
	//	j) pkh_num, subnummer (eigentlich "zeilenende")
	//	k) aktion, aktion
	//	l) 0, aktion 2
	//	n) text);	

	/*
	a) Version-Number is actually fixed to "0100"
	b) ";" to separate version-number from the rest
	c) Slave-Number is the number of the datacentral and is "1" for Master and Standalonesystems
	and may start with "4" for Slave-Systems.
	d) The TCC-Number indicates the sender of this message (0x01 to 0x7f will represent
	physical TCCs, values from 0x80 are reserved for the BFR internally (see list below))
	e) "Data", depend on the type of alarm sent
	f) "Data 1", depend on the type of alarm sent
	g) "Data 2", depend on the type of alarm sent
	h) ";" to separate head from the rest
	i) "Alarmnumber" indicates the alarm that is sent (see list below).
	j) "Subnumber" is more a parameter of the alarm and depends on the alarm that is sent;
	Recently it's more used to indicate alarms and sub-alarms (like BSN-error 247 and its
	various differences)
	k) "Aktion" is bitcoded and represents the alarmhandling on PM100 (see list below)
	l) "Aktion 2" is actually fixed to "0"
	m) "=" to separate alarm-text from the rest
	n) The text corresponding to this alarm-number is sent.
	The text may contain the character 0x07 (BEL) and is terminated with CR/LF (0x0d/0x0a).
	*/

	/*
	3. Examples of alarm-messages
	The BFR (Senders-Number > 0x80) sends the alarm 721 (0x2d1):
	0100;01,91,00000000,00000000,00000000;02d1,00000000,0007,00000000= MASTER: 06.05.02 12:01: HOTL: Modem	Wartung AUS, Ende
	The Manual cashier (TCC No. 3) sends the alarm 179 (0xb3):
	0100;01,03,00000000,00000000,00000000;00b3,00000000,0000,00000000= MASTER: TCC 003 06.05.02 12:03: BFR Befehl "TCC in Betrieb
	*/

	if (11 == sscanf((LPCSTR)pData,"%04x;%02x,%02x,%08x,%08x,%08x;%04x,%08x,%04x,%08x=%s",
					&dwVersion,
					&dwSLV,
					&dwTCC,
					&dwData,
					&dwData1,
					&dwData2,
					&dwTeleNr,
					&dwSubnummer,
					&dwAction,
					&dwAction2,
					szBuffer))
	{
		WK_TRACE(_T("receive PM 100 Alarm from BFR=%d,TCC=%d,AlNr=%d\n"),dwSLV,dwTCC,dwTeleNr);

		// now look for the alarm number
		BOOL bFound = FALSE;
		for (int i=0;i<m_AlarmNumbers.GetSize() && !bFound;i++)
		{
			DWORD dwABFR = m_AlarmNumbers.GetBFR(i);
			DWORD dwATCC = m_AlarmNumbers.GetTCC(i);
			DWORD dwAAlarmNr = m_AlarmNumbers.GetAlarmNr(i);
			TRACE(_T("%d,%d,%d = %d,%d,%d ?\n"),dwSLV,dwTCC,dwTeleNr,dwABFR,dwATCC,dwAAlarmNr);
			if (   dwSLV == dwABFR
				&& dwTCC == dwATCC
				&& dwTeleNr == dwAAlarmNr)
			{
				m_sSingleData.Format(_T("%d"),dwSLV);
				StoreNewDataBaseFields(FIELD_DBD_BFR);
				m_sSingleData.Format(_T("%d"),dwTCC);
				StoreNewDataBaseFields(FIELD_DBD_TCC);
				m_sSingleData.Format(_T("%d"),dwTeleNr);
				StoreNewDataBaseFields(FIELD_DBD_ANR);
				m_sSingleData = (LPCSTR)szBuffer;
				StoreNewDataBaseFields(FIELD_DBD_COMMENT);

				/*
				CSystemTime s = GetSystemTimeFromDesignaTime(dwDesignaTime);
				m_sSingleData = s.GetDBDate();
				StoreNewDataBaseFields(CIPCField::m_sfDate);
				m_sSingleData = s.GetDBTime();
				StoreNewDataBaseFields(CIPCField::m_sfTime);*/

				IndicateAlarm((WORD)m_AlarmNumbers.GetNr(i));
				
				ClearActualData();

				bFound = TRUE;
			}
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("cannot scan data from PM 100 %d\n"),dwLen);
	}
}
///////////////////////////////////////////////////////////////////////////////////////////
BOOL CRecordPM100Alarm::CheckForNewAlarmData()
{
	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////////////////
void CRecordPM100Alarm::InitializeResponses()
{
}

