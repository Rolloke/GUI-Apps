// DocPM100XML.cpp : implementation file
//

#include "stdafx.h"
#include "SDITest.h"
#include "DocPM100XML.h"


// CDocPM100XML

IMPLEMENT_DYNCREATE(CDocPM100XML, CSDITestDoc)

CDocPM100XML::CDocPM100XML()
{
	m_eType = SDICT_DESIGNA_PM_100_ALARM;
	m_iIPPort = 2140;

	m_dwBFR = 1;
	m_dwTCC = 1;
	m_dwAlarm = 1;
	m_dwTeleNum = 1;
	m_dwEvtCounter = 1;
	// m_sMessage
	m_dwTicSerNum = GetRandomNumberBetweenAsInt(100000,999999);
}

BOOL CDocPM100XML::OnNewDocument()
{
	if (!CSDITestDoc::OnNewDocument())
		return FALSE;
	m_iIPPort = 2140;
	m_iIPType = SOCK_DGRAM;
	m_sIPAddress = theApp.GetLocalIP();

	m_dwTeleNum = 1;
	m_dwBFR = 1;
	m_dwTCC = 1;
	m_dwAlarm = 1;
	m_dwEvtCounter = 1;
	m_sMessage.Empty();
	m_dwTicSerNum  = GetRandomNumberBetweenAsInt(100000,999999);
	return TRUE;
}

CDocPM100XML::~CDocPM100XML()
{
}


BEGIN_MESSAGE_MAP(CDocPM100XML, CSDITestDoc)
END_MESSAGE_MAP()


// CDocPM100XML diagnostics

#ifdef _DEBUG
void CDocPM100XML::AssertValid() const
{
	CSDITestDoc::AssertValid();
}

void CDocPM100XML::Dump(CDumpContext& dc) const
{
	CSDITestDoc::Dump(dc);
}
#endif //_DEBUG


// CDocPM100XML serialization

void CDocPM100XML::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CDocPM100XML commands
///////////////////////////////////////////////////////////
BOOL CDocPM100XML::IsDataOK()
{
	return TRUE;
}
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
///////////////////////////////////////////////////////////
void CDocPM100XML::CreateData()
{
	if (m_bRepeat)
	{
		switch (m_iRepeatHow)
		{
		case 0: // Chosen Units
			// Eingestellte Werte unveraendert lassen
			break;
		case 1: // Random
			m_dwAlarm = GetRandomNumberBetweenAsInt(0, 89);
			m_dwTicSerNum = GetRandomNumberBetweenAsInt(100000,999999);
			break;
		case 2: // Successive
			if ( !m_bStarted) 
			{
				m_dwAlarm = 1;
			}
			else {
				// Werte hochzaehlen
				m_dwAlarm++;
				if (m_dwAlarm>89) 
				{
					m_dwAlarm = 1;
				}
			}
			break;
		default: // Unknown
			break;
		}
	}

	m_sMessage = CSDIControl::GetPM100AlarmDescription(m_dwAlarm);

	CString sHTTP(_T("http://localhost/EventWeb/Template/InsertEvent.xml?"));
	CString s;

	s.Format(_T("TeleNum=%d"),m_dwTeleNum);
	sHTTP += s;

	s.Format(_T("PMTeleNum=%d"),GetRandomNumberBetweenAsInt(0,999));
	sHTTP += _T("&") + s;

	sHTTP += _T("&PMTeleVers=110");

	s.Format(_T("AbsTCC=%d"),m_dwTCC);
	sHTTP += _T("&") + s;

	s.Format(_T("AbsSlv=%d"),m_dwBFR);
	sHTTP += _T("&") + s;

	sHTTP += _T("&AuslNr=302.341");

	s.Format(_T("EvtCounter=%d"),m_dwEvtCounter);
	sHTTP += _T("&") + s;

	s.Format(_T("EvtType=%d"),m_dwAlarm);
	sHTTP += _T("&") + s;

	s.Format(_T("EvtTime=%d"),GetRandomNumberBetweenAsInt(0,12345678));
	sHTTP += _T("&") + s;

	s.Format(_T("SlvNum=%d"),m_dwBFR);
	sHTTP += _T("&") + s;

	s.Format(_T("TCCNum=%d"),m_dwTCC);
	sHTTP += _T("&") + s;

	s.Format(_T("TicTCCNum=%d"),m_dwTCC);
	sHTTP += _T("&") + s;

	s.Format(_T("TicSerNum=%d"),m_dwTicSerNum);
	sHTTP += _T("&") + s;

	s.Format(_T("TicTCCNum=%d"),m_dwTCC);
	sHTTP += _T("&") + s;
	
	s.Format(_T("TicTCCNum=%d"),m_dwTCC);
	sHTTP += _T("&") + s;

	// PM010010070000000526
	s.Format(_T("TicSerID=PM01%03d%03d%010d"),m_dwBFR,m_dwTCC,m_dwTicSerNum);
	sHTTP += _T("&") + s;

	CStringA sA(sHTTP);
	int iLen = sA.GetLength();
	m_byaData.SetSize(iLen);
	CopyMemory(m_byaData.GetData(),(LPCSTR)sA,iLen);
	
	m_sMessage.Empty();
	m_dwTicSerNum = 0;
	m_dwTeleNum++;
	m_dwEvtCounter += GetRandomNumberBetweenAsInt(1,5);
}