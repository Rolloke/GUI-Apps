// DocRondo.cpp : implementation file
//

#include "stdafx.h"
#include "sditest.h"
#include "DocRondo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDocRondo
IMPLEMENT_DYNCREATE(CDocRondo, CSDITestDoc)
/////////////////////////////////////////////////////////////////////////////
CDocRondo::CDocRondo()
{
	m_eType = SDICT_RONDO;
	m_iCommand = 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDocRondo::OnNewDocument()
{
	if (!CSDITestDoc::OnNewDocument())
		return FALSE;
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
CDocRondo::~CDocRondo()
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDocRondo::OpenCom() 
{
	return m_pCom->Open(CBR_1200, 7, ODDPARITY, ONESTOPBIT);
}
/////////////////////////////////////////////////////////////////////////////
void CDocRondo::CreateData() 
{
	if (m_bRepeat) {
		switch (m_iRepeatHow) {
		case 0: // Chosen Units
			// Eingestellte Werte unveraendert lassen
			break;
		case 1: // Random
			m_iCommand = GetRandomNumberBetweenAsInt(0, 3);
			break;
		case 2: // Successive
			if ( !m_bStarted) {
				m_iCommand = 0;
			}
			else {
				// Werte hochzaehlen
				m_iCommand++;
				if (3 < m_iCommand) {
					m_iCommand = 0;
				}
			}
			break;
		default: // Unknown
			WK_TRACE_ERROR("CDocRondo::PreCreateData() WRONG DATA RepeatHow %i\n",
																			m_iRepeatHow);
			break;
		}
	}

	CreateDataIntern();
}
/////////////////////////////////////////////////////////////////////////////
void CDocRondo::CreateDataIntern() 
{
	if (m_iCommand == 0) {
		CreateStatusRequest();
	}
	else {
		CreateNormalData() ;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDocRondo::CreateStatusRequest() 
{
	CString sData;
	// STX
	sData  = ASCII_STX;
	// A fuer Status-Abfrage
	sData += 'E';
	// ETX anhaengen
	sData += ASCII_ETX;

	// DatenArray zusammensetzen
	m_byaData.RemoveAll();
	for (int i=0 ; i<sData.GetLength() ; i++) {
		m_byaData.Add(sData.GetAt(i));
	}
	// Checksumme XOR ohne STX mit ETX 1 Byte
	m_byaData.Add( GetXORCrcByte(m_byaData, 1) );
}
/////////////////////////////////////////////////////////////////////////////
void CDocRondo::CreateNormalData() 
{
	CString sData, sDate, sTime, sBuffer;
	BOOL bIncreaseTan = FALSE;

	// Datum
	sDate = GetDate("DDMMYY");
	// Zeit
	sTime = GetTime("HHMMSS");

	// STX
	sData  = ASCII_STX;
	// Kommando
	switch (m_iCommand) {
		case 1:
			sData += '0';
			break;
		case 2:
			sData += '1';
			bIncreaseTan = TRUE;
			break;
		case 3:
			sData += '2';
			break;
		default:
			sData += '0';
			break;
	}

	// 1 Byte Trennzeichen US
	sData += ASCII_US;
	// 6 Byte Datum
	sData += sDate;
	// 6 Byte Zeit
	sData += sTime;
	// 1 Byte Trennzeichen US
	sData += ASCII_US;
	// 6 Byte WSID
	sData += GetWSID(6);
	// 1 Byte Leerzeichen
	sData += ASCII_SPACE;
	// 4 Byte TAN
	sData += GetTAN(4, bIncreaseTan);
	// 1 Byte Leerzeichen
	sData += ASCII_SPACE;
	// 9 Byte Transaktionsart
	sData += "TraAktion";
	// 1 Byte Leerzeichen
	sData += ASCII_SPACE;
	// 11 Byte Betrag inkl. Komma und zwei Nachkommastellen
	sData += GetBetrag(8);
	sData += ",00";
	// 3 Byte Waehrung
	sData += "DEM";
	// 1 Byte CR
	sData += ASCII_CR;
	// 8 Byte BLZ Sender
	sData += GetBLZ(8);
	// 10 Byte Konto Sender
	sData += GetKonto(10);
	// 8 Byte BLZ Empfaenger
	sData += GetRandomNumberWithLen(8);
	// 10 Byte Konto Empfaenger
	sData += GetRandomNumberWithLen(10);
	
	// ETX anhaengen
	sData += ASCII_ETX;

	// DatenArray zusammensetzen
	m_byaData.RemoveAll();
	for (int i=0 ; i<sData.GetLength() ; i++) {
		m_byaData.Add(sData.GetAt(i));
	}
	// Checksumme XOR ohne STX mit ETX 1 Byte
	m_byaData.Add( GetXORCrcByte(m_byaData, 1) );
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDocRondo, CSDITestDoc)
	//{{AFX_MSG_MAP(CDocRondo)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CDocRondo diagnostics
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CDocRondo::AssertValid() const
{
	CSDITestDoc::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CDocRondo::Dump(CDumpContext& dc) const
{
	CSDITestDoc::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
// CDocRondo serialization
/////////////////////////////////////////////////////////////////////////////
void CDocRondo::Serialize(CArchive& ar)
{
	// Versionsnummer wird mit abgespeichert,
	// muss bei jeder relevanten Aenderung erhoeht werden
	m_uSerializeVersion = 0;

	CSDITestDoc::Serialize(ar);

	if (ar.IsStoring())
	{
		// TODO: add storing code here
		ar << m_iCommand;
	}
	else
	{
		// TODO: add loading code here
		ar >> m_iCommand;
	}
}
/////////////////////////////////////////////////////////////////////////////
// CDocRondo commands
