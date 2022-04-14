/* GlobalReplace: CSDITestSniDoc --> CDocSniDos */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: DocSniDos.cpp $
// ARCHIVE:		$Archive: /Project/TestPrograms/SDITest/DocSniDos.cpp $
// CHECKIN:		$Date: 19.12.05 16:30 $
// VERSION:		$Revision: 14 $
// LAST CHANGE:	$Modtime: 19.12.05 16:28 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "DocSniDos.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDocSniDos
IMPLEMENT_DYNCREATE(CDocSniDos, CSDITestDoc)
/////////////////////////////////////////////////////////////////////////////
CDocSniDos::CDocSniDos()
{
	m_eType = SDICT_SNI_DOS_CHASE;
	m_iPointOfData = 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDocSniDos::OnNewDocument()
{
	if (!CSDITestDoc::OnNewDocument())
		return FALSE;
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
CDocSniDos::~CDocSniDos()
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDocSniDos::OpenCom() 
{
	return m_pCom->Open(CBR_2400, 8, NOPARITY, ONESTOPBIT);
}
/////////////////////////////////////////////////////////////////////////////
void CDocSniDos::CreateData() 
{
	if (m_bRepeat) {
		switch (m_iRepeatHow) {
		case 0: // Chosen Units
			// Eingestellte Werte unveraendert lassen
			break;
		case 1: // Random
			m_iPointOfData = GetRandomNumberBetweenAsInt(0, 9);
			break;
		case 2: // Successive
			if ( !m_bStarted ) {
				m_iPointOfData = 0;
			}
			else {
				// Werte hochzaehlen
				m_iPointOfData++;
				if (9 < m_iPointOfData) {
					m_iPointOfData = 0;
				}
			}
			break;
		default: // Unknown
			WK_TRACE_ERROR(_T("CViewSniDos::PreCreateData() WRONG DATA RepeatHow %i\n"),
																			m_iRepeatHow);
			break;
		}
	}

	CreateDataIntern();
}
/////////////////////////////////////////////////////////////////////////////
void CDocSniDos::CreateDataIntern() 
{
	CString sData, sBuffer;
	// Erst mal Daten ohne STX und Laengenfeld zusammensetzen
	// Ansteuerungspunkt 2 Byte
	sBuffer.Format(_T("%02i"), m_iPointOfData);
	sData = sBuffer;

	switch (m_iPointOfData) {
	case 0:
		// Statusabfrage ~00:
		sData += _T("~00:");
		// Datum ~10: + 6 Byte JJMMTT
		sData += _T("~10:");
		sData += GetDate(_T("YYMMDD"));
		// Zeit ~11: + 6 Byte HHMMSS
		sData += _T("~11:");
		sData += GetTime(_T("HHMMSS"));
		// GA-Kennzeichen ~12: +  4 Byte
		sData += _T("~12:");
		sData += GetRandomNumberWithLen(4);
		break;
	case 1:
		// Datum ~10: + 6 Byte JJMMTT
		sData += _T("~10:");
		sData += GetDate(_T("YYMMDD"));
		// Zeit ~11: + 6 Byte HHMMSS
		sData += _T("~11:");
		sData += GetTime(_T("HHMMSS"));
		// GA-Kennzeichen ~12: +  4 Byte
		sData += _T("~12:");
		sData += GetRandomNumberWithLen(4);
		// Daten aus der seriellen Schnittstelle (GA) werden ignoriert
		break;
	case 2:
		// Datum ~10: + 6 Byte JJMMTT
		sData += _T("~10:");
		sData += GetDate(_T("YYMMDD"));
		// Zeit ~11: + 6 Byte HHMMSS
		sData += _T("~11:");
		sData += GetTime(_T("HHMMSS"));
		// Daten aus der seriellen Schnittstelle (GA) werden ignoriert
		break;
	case 3:
		// Datum ~10: + 6 Byte JJMMTT
		sData += _T("~10:");
		sData += GetDate(_T("YYMMDD"));
		// Zeit ~11: + 6 Byte HHMMSS
		sData += _T("~11:");
		sData += GetTime(_T("HHMMSS"));
		// Überwachungsmodus Person ~01:1
		sData += _T("~01:1");
		break;
	case 4:
		// Datum ~10: + 6 Byte JJMMTT
		sData += _T("~10:");
		sData += GetDate(_T("YYMMDD"));
		// Zeit ~11: + 6 Byte HHMMSS
		sData += _T("~11:");
		sData += GetTime(_T("HHMMSS"));
		// TAN ~13: + 4 Byte
		sData += _T("~13:");
		sData += GetTAN(4, TRUE);
		// BLZ ~20: + 8 Byte
		sData += _T("~20:");
		sData += GetBLZ(8);
		// Kontonummer ~21: + 10 Byte
		sData += _T("~21:");
		sData += GetKonto(10);
		// Kartenfolgenummer ~22: + 1 Byte
		sData += _T("~22:");
		sData += GetRandomNumberWithLen(1);
		break;
	case 5:
		// Datum ~10: + 6 Byte JJMMTT
		sData += _T("~10:");
		sData += GetDate(_T("YYMMDD"));
		// Zeit ~11: + 6 Byte HHMMSS
		sData += _T("~11:");
		sData += GetTime(_T("HHMMSS"));
		// Betrag ~30: + 8 Byte
		sData += _T("~30:");
		sData += GetBetrag(8);
		// Währung ~31: + 3 Byte, hier DEM
		sData += _T("~31:");
		sData += _T("DEM");
		// Daten aus der seriellen Schnittstelle (GA) werden ignoriert
		break;
	case 6:
		// Datum ~10: + 6 Byte JJMMTT
		sData += _T("~10:");
		sData += GetDate(_T("YYMMDD"));
		// Zeit ~11: + 6 Byte HHMMSS
		sData += _T("~11:");
		sData += GetTime(_T("HHMMSS"));
		// Überwachungsmodus Geldentnahme ~01:2
		sData += _T("~01:2");
		break;
	case 7:
		// Datum ~10: + 6 Byte JJMMTT
		sData += _T("~10:");
		sData += GetDate(_T("YYMMDD"));
		// Zeit ~11: + 6 Byte HHMMSS
		sData += _T("~11:");
		sData += GetTime(_T("HHMMSS"));
		// Status der Auszahlung, hier Auszahlung, ~32:0
		sData += _T("~32:0");
		break;
	case 8:
		// Überwachungsmodus Raum ~01:0
		sData += _T("~01:0");
		break;
	case 9:
		// Einfach nichts
		break;
	default: // Unknown
		WK_TRACE_ERROR(_T("CDocSniDos::CreateData() WRONG PointOfData %i\n"),
																m_iPointOfData);
		break;
	}

	// Datenstring zusammensetzen
	m_byaData.RemoveAll();
	// Startkommando
	// STX
	m_byaData.Add(ASCII_STX);
	// Laenge 3 Byte
	int iLen = sData.GetLength();
	sBuffer.Format(_T("%03i"), iLen);
	sData = sBuffer + sData;
	// Daten
	CStringA s(sData);

	for (int i=0 ; i<s.GetLength() ; i++) {
		m_byaData.Add(s.GetAt(i));
	}
	// ETX
	m_byaData.Add(ASCII_ETX);
}
/////////////////////////////////////////////////////////////////////////////
// CDocSniDos diagnostics
#ifdef _DEBUG
void CDocSniDos::AssertValid() const
{
	CSDITestDoc::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CDocSniDos::Dump(CDumpContext& dc) const
{
	CSDITestDoc::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
// CDocSniDos serialization
void CDocSniDos::Serialize(CArchive& ar)
{
	// Versionsnummer wird mit abgespeichert,
	// muss bei jeder relevanten Aenderung erhoeht werden
	m_uSerializeVersion = 0;

	CSDITestDoc::Serialize(ar);

	if (ar.IsStoring())
	{
		// TODO: add storing code here
		ar << m_iPointOfData; 
	}
	else
	{
		// TODO: add loading code here
		ar >> m_iPointOfData;
	}
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDocSniDos, CSDITestDoc)
	//{{AFX_MSG_MAP(CDocSniDos)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CDocSniDos commands
