#include "stdafx.h"
#include "SDIUnit.h"
#include "Recordpm100xml.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////////////////////
CRecordPM100XML::CRecordPM100XML(SDIControlType type,
								 int iPort,
								 LPCTSTR szIPAddress,
								 BOOL bTraceAscii,
								 BOOL bTraceHex)
 : CSDIControlRecord(type, 0, iPort,szIPAddress, bTraceAscii, bTraceHex,FALSE)
{
}
///////////////////////////////////////////////////////////////////////////////////////////////
CRecordPM100XML::~CRecordPM100XML(void)
{
}
///////////////////////////////////////////////////////////////////////////////////////////////
BOOL CRecordPM100XML::Create(CWK_Profile& wkp, const CString& sSection)
{
	BOOL bRet = FALSE;
	if (m_iPort>0)
	{
		m_AlarmNumbers.RemoveAll();
		m_AlarmNumbers.Load(wkp,sSection);
		WK_TRACE(_T("found %d PM 100 XML\n"),m_AlarmNumbers.GetSize());

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
///////////////////////////////////////////////////////////////////////////////////////////////
/*
http://localhost/EventWeb/Template/InsertEvent.xml?
TeleNum=119518&
PMTeleNum=957&
PMTeleVers=110&
AbsTCC=20&
AbsSlv=4&
AuslNr=302.341&
EvtCounter=290538&
EvtType=186&
EvtTime=782140296&
SlvNum=4&
PHNum=15&
TCCNum=20&
EvtTCCType=2&
TicPHNum=15&
TicTCCNum=47&
TicTime=782139833&
TicPara1=0&
TicPara2=0&
TicStatus=0&
TicSerNum=245551&
TicSerTCC=17&
TicSerSlv=4&
TicArt=1&
NewTicTCC=0&
NewTicTime=782140291&
NewTicPH=15&
TicKul=0&
EvtKFZNum=%4d%31%33%32%36%54%42&
EvtISOString=%50%4d%30%31%30%30%34%30%31%37%30%30%30%30%32%34%35%35%35%31&
TicSerID=%50%4d%30%31%30%30%34%30%31%37%30%30%30%30%32%34%35%35%35%31
*/

void CRecordPM100XML::OnReceivedData(LPBYTE pData, DWORD dwLen)
{
	CString sTelegramm((LPCSTR)pData,dwLen);
	CString sAbsTCC,sAbsSlv,sEvtType,sEvtTime,sEvtISOString,sTicSerNum;

	sAbsTCC = GetValue(sTelegramm,_T("AbsTCC"));
	sAbsSlv = GetValue(sTelegramm,_T("AbsSlv"));
	sEvtType = GetValue(sTelegramm,_T("EvtType"));
	sEvtTime = GetValue(sTelegramm,_T("EvtTime"));
	sTicSerNum = GetValue(sTelegramm,_T("TicSerID"));

	if (!sAbsSlv.IsEmpty() && !sAbsTCC.IsEmpty())
	{
		WK_TRACE(_T("receive PM 100 Alarm from %s,%s\n"),sAbsSlv,sAbsTCC);
		DWORD dwSLV,dwTCC,dwTeleNr,dwDesignaTime;
		dwSLV = dwTCC = dwTeleNr = dwDesignaTime = 0;
		_stscanf(sAbsSlv,_T("%d"),&dwSLV);
		_stscanf(sAbsTCC,_T("%d"),&dwTCC);
		_stscanf(sEvtType,_T("%d"),&dwTeleNr);
		_stscanf(sEvtTime,_T("%d"),&dwDesignaTime);


		BOOL bFound = FALSE;
		for (int i=0;i<m_AlarmNumbers.GetSize() && !bFound;i++)
		{
			DWORD dwABFR = m_AlarmNumbers.GetBFR(i);
			DWORD dwATCC = m_AlarmNumbers.GetTCC(i);
			DWORD dwAlarmNr = m_AlarmNumbers.GetAlarmNr(i);
			TRACE(_T("%d,%d,%d = %d,%d,%d ?\n"),dwSLV,dwTCC,dwTeleNr,dwABFR,dwATCC,dwAlarmNr);
			if (   dwSLV == dwABFR
				&& dwTCC == dwATCC
				&& dwTeleNr == dwAlarmNr)
			{
				m_sSingleData = sAbsSlv;
				StoreNewDataBaseFields(FIELD_DBD_BFR);
				m_sSingleData = sAbsTCC;
				StoreNewDataBaseFields(FIELD_DBD_TCC);
				m_sSingleData = sEvtType;
				StoreNewDataBaseFields(FIELD_DBD_ANR);
				m_sSingleData = sTicSerNum;
				StoreNewDataBaseFields(FIELD_DBD_TSN);

				CSystemTime st = GetSystemTimeFromDesignaTime(dwDesignaTime);
				m_sSingleData = st.GetDBDate();
				StoreNewDataBaseFields(CIPCField::m_sfDate);
				m_sSingleData = st.GetDBTime();
				StoreNewDataBaseFields(CIPCField::m_sfTime);


				CString s = CSDIControl::GetPM100XMLDescription(dwAlarmNr);
				if (!s.IsEmpty())
				{
					m_sSingleData = s;
					StoreNewDataBaseFields(FIELD_DBD_COMMENT);
				}

				IndicateAlarm((WORD)m_AlarmNumbers.GetNr(i));

				ClearActualData();

				bFound = TRUE;
			}
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////
BOOL CRecordPM100XML::CheckForNewAlarmData()
{
	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////////////////////
void CRecordPM100XML::InitializeResponses()
{
}
