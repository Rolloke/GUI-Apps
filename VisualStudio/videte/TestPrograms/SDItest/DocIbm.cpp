/* GlobalReplace: CSDITestIbmDoc --> CDocIbm */
/* GlobalReplace: CSDITestIbmView --> CViewIbm */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: DocIbm.cpp $
// ARCHIVE:		$Archive: /Project/TestPrograms/SDITest/DocIbm.cpp $
// CHECKIN:		$Date: 19.12.05 17:18 $
// VERSION:		$Revision: 13 $
// LAST CHANGE:	$Modtime: 19.12.05 16:47 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "DocIbm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDocIbm
IMPLEMENT_DYNCREATE(CDocIbm, CSDITestDoc)
/////////////////////////////////////////////////////////////////////////////
CDocIbm::CDocIbm()
{
	m_eType = SDICT_IBM;
	m_iPointOfData = 0;
	m_bStatusRequest = FALSE;
	m_bWithTerminalID = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDocIbm::OnNewDocument()
{
	if (!CSDITestDoc::OnNewDocument())
		return FALSE;
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
CDocIbm::~CDocIbm()
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDocIbm::OpenCom() 
{
	return m_pCom->Open(CBR_1200, 7, ODDPARITY, ONESTOPBIT);
}
/////////////////////////////////////////////////////////////////////////////
void CDocIbm::CreateData() 
{
	if (m_bRepeat) {
		switch (m_iRepeatHow) {
		case 0: // Chosen Units
			// Eingestellte Werte unveraendert lassen
			break;
		case 1: // Random
			m_iPointOfData = GetRandomNumberBetweenAsInt(0, 15);
			break;
		case 2: // Successive
			if ( !m_bStarted) {
				m_iPointOfData = 0;
			}
			else {
				// Werte hochzaehlen
				m_iPointOfData++;
				if (16 <= m_iPointOfData) {
					m_iPointOfData = 0;
				}
			}
			break;
		default: // Unknown
			WK_TRACE_ERROR(_T("CDocIbm::PreCreateData() WRONG DATA RepeatHow %i\n"),
																			m_iRepeatHow);
			break;
		}
	}

	CreateDataIntern();
}
/////////////////////////////////////////////////////////////////////////////
void CDocIbm::CreateDataIntern() 
{
	if (m_bStatusRequest) {
		CreateStatusRequest();
	}
	else {
		CreateNormalData() ;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDocIbm::CreateStatusRequest() 
{
	CString sData;
	// STX
	sData  = (char)ASCII_STX;
	// A fuer Status-Abfrage
	sData += _T('A');
	if (m_bWithTerminalID) {
		// US anhaengen
		sData += (char)ASCII_US;
		// WSID
		CString sBuffer = GetWSID(6, TRUE);
		sBuffer.MakeReverse();
		sData += sBuffer;
	}
	// ETX anhaengen
	sData += (char)ASCII_ETX;

	// DatenArray zusammensetzen
	m_byaData.RemoveAll();
	CStringA s(sData);
	for (int i=0 ; i<s.GetLength() ; i++) {
		m_byaData.Add(s.GetAt(i));
	}
	// Checksumme XOR ohne STX mit ETX 1 Byte
	m_byaData.Add( GetXORCrcByte(m_byaData, 1) );
}
/////////////////////////////////////////////////////////////////////////////
void CDocIbm::CreateNormalData() 
{
	CString sData, sActualData, sWSID, sDate, sTime, sBuffer;

	// Erst mal die doppelt benutzten Strings zusammenstellen
	// WSID
	sWSID = GetRandomNumberWithLen(5);
	// Datum
	sDate = GetDate(_T("MMDDYY"));
	// Zeit
	sTime = GetTime(_T("HHMMSS"));

	// Erst mal Daten ohne STX und Laengenfeld zusammensetzen
	// STX
	sData  = (char)ASCII_STX;
	// 3 Byte Laenge
	sData += _T("LEN");
	// 1 Byte record format
	sData += _T('0');
	// WSID
	sData += sWSID;
	// Datum
	sData += sDate;
	// Zeit
	sData += sTime;
	// Sequence
	sData += GetRandomNumberWithLen(4);
	// Ansteuerungspunkt 2 Byte, intern 0-15, aber im Protokoll 1-16
	sBuffer.Format(_T("%02i"), m_iPointOfData + 1);
	sData += sBuffer.Right(2);
	// machine state 1 byte
	sData += _T('0');

	// Jetzt kommen die variablen Daten
	m_sVarData = m_sDataFormat;
	int iStart = 0;
	int iEnd = 0;
	// Scanne das Datenformat nach den Platzhaltern und ersetze sie
	for (int e=PARAM_ACCOUNT ; e<=PARAM_LAST ; e++) {
		switch (e) {
			case PARAM_ACCOUNT:
				iStart = m_sDataFormat.Find(_T('a'));
				iEnd = m_sDataFormat.ReverseFind(_T('a'));
				sActualData = GetKonto(iEnd-iStart+1);
				break;
			case PARAM_BANKCODE:
				iStart = m_sDataFormat.Find(_T('b'));
				iEnd = m_sDataFormat.ReverseFind(_T('b'));
				sActualData = GetBLZ(iEnd-iStart+1);
				break;
			case PARAM_VALUE:
				iStart = m_sDataFormat.Find(_T('v'));
				iEnd = m_sDataFormat.ReverseFind(_T('v'));
				sActualData = GetBetrag(iEnd-iStart+1);
				break;
			case PARAM_WORKSTATION:
				iStart = m_sDataFormat.Find(_T('w'));
				iEnd = m_sDataFormat.ReverseFind(_T('w'));
				sActualData = sWSID;
				break;
			case PARAM_TRANSAKTION:
				iStart = m_sDataFormat.Find(_T('t'));
				iEnd = m_sDataFormat.ReverseFind(_T('t'));
				sActualData = GetTAN(iEnd-iStart+1, TRUE);
				break;
			case PARAM_DAY:
				iStart = m_sDataFormat.Find(_T("DD"));
				iEnd = iStart + 1;
				sActualData = sDate.Mid(2,2);
				break;
			case PARAM_MONTH:
				iStart = m_sDataFormat.Find(_T("MM"));
				iEnd = iStart + 1;
				sActualData = sDate.Left(2);
				break;
			case PARAM_YEAR:
				iStart = m_sDataFormat.Find(_T("YY"));
				iEnd = iStart + 1;
				sActualData = sDate.Mid(4,2);
				break;
			case PARAM_HOUR:
				iStart = m_sDataFormat.Find(_T("hh"));
				iEnd = iStart + 1;
				sActualData = sTime.Left(2);
				break;
			case PARAM_MINUTE:
				iStart = m_sDataFormat.Find(_T("mm"));
				iEnd = iStart + 1;
				sActualData = sTime.Mid(2,2);
				break;
			case PARAM_SECOND:
				iStart = m_sDataFormat.Find(_T("ss"));
				iEnd = iStart + 1;
				sActualData = sTime.Mid(4,2);
				break;
			case PARAM_LAST:
			default:
				break;
			// end of cases
		} // switch
		ReplaceVarData(iStart, iEnd, sActualData);
	}
	sData += m_sVarData;

	// Jetzt wird das Laengenfeld gesetzt, aber ohne STX berechnen!
	int iLen = sData.GetLength() - 1;
	sBuffer.Format(_T("%03i"), iLen);
	for (int i=0 ; i<3 ; i++) {
		sData.SetAt( i+1, sBuffer.GetAt(i) );
	}
	// ETX anhaengen
	sData += (char)ASCII_ETX;

	// DatenArray zusammensetzen
	m_byaData.RemoveAll();
	CStringA s(sData);
	for (i=0 ; i<s.GetLength() ; i++) {
		m_byaData.Add(s.GetAt(i));
	}
	// Checksumme XOR ohne STX mit ETX 1 Byte
	m_byaData.Add( GetXORCrcByte(m_byaData, 1) );
}
/////////////////////////////////////////////////////////////////////////////
void CDocIbm::ReplaceVarData(int iStart, int iEnd, CString sReplace)
{
	if (-1 != iStart) {
		if (-1 != iEnd) {
			int iLen = iEnd-iStart+1;
			if ( sReplace.IsEmpty() ) {
				sReplace = GetRandomNumberWithLen(iLen);
			}
			// Der Eintrag muss die richtige Laenge haben
			CString sBuffer = TrimDataToLength(sReplace, iLen);
			for (int i=0 ; i<sBuffer.GetLength() ; i++) {
				m_sVarData.SetAt(i+iStart, sBuffer.GetAt(i) );
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDocIbm, CSDITestDoc)
	//{{AFX_MSG_MAP(CDocIbm)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CDocIbm diagnostics
#ifdef _DEBUG
void CDocIbm::AssertValid() const
{
	CSDITestDoc::AssertValid();
}

void CDocIbm::Dump(CDumpContext& dc) const
{
	CSDITestDoc::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
// CDocIbm serialization
void CDocIbm::Serialize(CArchive& ar)
{
	// Versionsnummer wird mit abgespeichert,
	// muss bei jeder relevanten Aenderung erhoeht werden
	m_uSerializeVersion = 0;

	CSDITestDoc::Serialize(ar);
	if (ar.IsStoring())
	{
		// TODO: add storing code here
		ar << m_sDataFormat;
	}
	else
	{
		// TODO: add loading code here
		ar >> m_sDataFormat;
	}
}
/////////////////////////////////////////////////////////////////////////////
// CDocIbm commands
