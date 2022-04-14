// DocSchneiderIntercom.cpp : implementation file
//

#include "stdafx.h"
#include "SDITest.h"
#include "DocSchneiderIntercom.h"


// CDocSchneiderIntercom

IMPLEMENT_DYNCREATE(CDocSchneiderIntercom, CSDITestDoc)

CDocSchneiderIntercom::CDocSchneiderIntercom()
{
	//m_sSubscriberNr;
	m_iAlarmType = 0;
}

BOOL CDocSchneiderIntercom::OnNewDocument()
{
	if (!CSDITestDoc::OnNewDocument())
		return FALSE;
	m_sSubscriberNr.Empty();
	m_iAlarmType = 0;
	return TRUE;
}

CDocSchneiderIntercom::~CDocSchneiderIntercom()
{
}


BEGIN_MESSAGE_MAP(CDocSchneiderIntercom, CSDITestDoc)
END_MESSAGE_MAP()


// CDocSchneiderIntercom diagnostics

#ifdef _DEBUG
void CDocSchneiderIntercom::AssertValid() const
{
	CSDITestDoc::AssertValid();
}

void CDocSchneiderIntercom::Dump(CDumpContext& dc) const
{
	CSDITestDoc::Dump(dc);
}
#endif //_DEBUG


// CDocSchneiderIntercom serialization

void CDocSchneiderIntercom::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
		ar << m_iAlarmType;
		ar << m_sSubscriberNr;
	}
	else
	{
		// TODO: add loading code here
		ar >> m_iAlarmType;
		ar >> m_sSubscriberNr;
	}
}


// CDocSchneiderIntercom commands
BOOL CDocSchneiderIntercom::OpenCom()
{
	return m_pCom->Open(CBR_9600, 8, ODDPARITY, ONESTOPBIT);
}
BOOL CDocSchneiderIntercom::IsDataOK()
{
	return TRUE;
}
void CDocSchneiderIntercom::CreateData()
{
	DWORD dwMax = 9999;
	CString sAlarm;
	CString sXXXX, sYYYY, sType;

	_stscanf(m_sSubscriberNr,_T("%d"),&dwMax);
	sYYYY = _T("0000");
	sXXXX = m_sSubscriberNr;
	
	if (m_bRepeat) 
	{
		switch (m_iRepeatHow) 
		{
		case 0: // Chosen Units
			// Eingestellte Werte unveraendert lassen
			// TAN hochzaehlen
			break;
		case 1: // Random
			sXXXX.Format(_T("%d"),GetRandomNumberBetweenAsInt(1,dwMax));
			m_iAlarmType = GetRandomNumberBetweenAsInt(0,1);
			sYYYY.Format(_T("%d"),GetRandomNumberBetweenAsInt(1,dwMax));
			break;
		case 2: // Successive
			m_iAlarmType = !m_iAlarmType;
			break;
		default: // Unknown
			WK_TRACE_ERROR(_T("CDocSchneiderIntercom::CreateData() WRONG DATA RepeatHow %i\n"),m_iRepeatHow);
			break;
		}
	}
	m_byaData.RemoveAll();
	m_byaData.Add(ASCII_STX);


	while (sXXXX.GetLength()<4)
	{
		sXXXX = _T("F")+sXXXX;
	}
	while (sYYYY.GetLength()<4)
	{
		sYYYY = _T("F")+sYYYY;
	}
	if (m_iAlarmType == 0)
	{
		sType = _T("12");
	}
	else if (m_iAlarmType == 1)
	{
		sType = _T("10");
	}


	sAlarm.Format(_T("%02d42%02d%s%s%s"),
				  GetRandomNumberBetweenAsInt(1,99),
				  GetRandomNumberBetweenAsInt(1,99),
				  sXXXX,sYYYY,sType);
	{
		CStringA s(sAlarm);
		for (int i=0 ; i<s.GetLength() ; i++) 
		{
			m_byaData.Add(s.GetAt(i));
		}
	}
	BYTE bSS = GetXORCrcByte(m_byaData);
	CString sSS;
	sSS.Format(_T("%02x"),bSS);
	{
		CStringA s(sSS);
		for (int i=0 ; i<s.GetLength() ; i++) 
		{
			m_byaData.Add(s.GetAt(i));
		}
	}

	m_byaData.Add(ASCII_ETX);
}
