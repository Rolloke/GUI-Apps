// DocPM100IPAlarm.cpp : implementation file
//

#include "stdafx.h"
#include "SDITest.h"
#include "DocPM100IPAlarm.h"


// CDocPM100IPAlarm

IMPLEMENT_DYNCREATE(CDocPM100IPAlarm, CSDITestDoc)

CDocPM100IPAlarm::CDocPM100IPAlarm()
{
	m_eType = SDICT_DESIGNA_PM_100_ALARM;
	m_iIPPort = 2100;
	m_iIPType = SOCK_DGRAM;

	m_dwBFR = 1;
	m_dwTCC = 1;
	m_dwAlarm = 1;
	//m_sMessage;
}

BOOL CDocPM100IPAlarm::OnNewDocument()
{
	if (!CSDITestDoc::OnNewDocument())
		return FALSE;
	
	m_iIPPort = 2100;
	m_iIPType = SOCK_DGRAM;
	m_sIPAddress = theApp.GetLocalIP();

	m_dwBFR = 1;
	m_dwTCC = 1;
	m_dwAlarm = 1;
	m_sMessage.Empty();

	return TRUE;
}

CDocPM100IPAlarm::~CDocPM100IPAlarm()
{
}


BEGIN_MESSAGE_MAP(CDocPM100IPAlarm, CSDITestDoc)
END_MESSAGE_MAP()


// CDocPM100IPAlarm diagnostics

#ifdef _DEBUG
void CDocPM100IPAlarm::AssertValid() const
{
	CSDITestDoc::AssertValid();
}

void CDocPM100IPAlarm::Dump(CDumpContext& dc) const
{
	CSDITestDoc::Dump(dc);
}
#endif //_DEBUG


// CDocPM100IPAlarm serialization

void CDocPM100IPAlarm::Serialize(CArchive& ar)
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


// CDocPM100IPAlarm commands
///////////////////////////////////////////////////////////
BOOL CDocPM100IPAlarm::IsDataOK()
{
	return TRUE;
}
///////////////////////////////////////////////////////////
void CDocPM100IPAlarm::CreateData()
{
	DWORD dwVersion = 0x100;
	DWORD dwSLV = 0;
	DWORD dwTCC = 0;
	DWORD dwData = 0;
	DWORD dwData1 = 0;
	DWORD dwData2 = 0;
	DWORD dwTeleNr = 0;
	DWORD dwSubnummer = 0;
	DWORD dwAction = 0;
	DWORD dwAction2 = 0;
	char ip_msg[512];

	if (m_bRepeat)
	{
		switch (m_iRepeatHow)
		{
		case 0: // Chosen Units
			// Eingestellte Werte unveraendert lassen
			break;
		case 1: // Random
			m_dwAlarm = GetRandomNumberBetweenAsInt(0, 89);
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
	dwTeleNr = m_dwAlarm;
	dwSLV = m_dwBFR;
	dwTCC = m_dwTCC;

	m_sMessage = CSDIControl::GetPM100AlarmDescription(m_dwAlarm);


	CStringA s(m_sMessage);
	int iLen = sprintf(ip_msg, "%04x;%02x,%02x,%08x,%08x,%08x;%04x,%08x,%04x,%08x=%s\r\n",
			dwVersion,
			dwSLV,
			dwTCC,
			dwData,
			dwData1,
			dwData2,
			dwTeleNr,
			dwSubnummer,
			dwAction,
			dwAction2,
			s);


	m_byaData.SetSize(iLen);
	CopyMemory(m_byaData.GetData(),ip_msg,iLen);
	m_sMessage.Empty();
}
