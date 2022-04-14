/* GlobalReplace: CSDITestDtpDoc --> CDocLandauDtp */
/* GlobalReplace: CSDITestDtpView --> CViewLandauDtp */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: DocLandauDtp.cpp $
// ARCHIVE:		$Archive: /Project/TestPrograms/SDITest/DocLandauDtp.cpp $
// CHECKIN:		$Date: 19.12.05 17:18 $
// VERSION:		$Revision: 22 $
// LAST CHANGE:	$Modtime: 19.12.05 16:47 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "DocLandauDtp.h"
#include "ViewLandauDtp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDocLandauDtp
IMPLEMENT_DYNCREATE(CDocLandauDtp, CSDITestDoc)
/////////////////////////////////////////////////////////////////////////////
CDocLandauDtp::CDocLandauDtp()
{
	m_eType = SDICT_LANDAU_DTP;
	m_iPortNr = 0;
	m_iCameraNr = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CDocLandauDtp::CreateData() 
{
	CString sPortNr, sCameraNr, sTan;

	if (m_bRepeat) {
		switch (m_iRepeatHow) {
		case 0: // Chosen Units
			// Eingestellte Werte unveraendert lassen
			// TAN hochzaehlen
			sTan = GetTAN(4, TRUE);
			break;
		case 1: // Random
			m_iPortNr = GetRandomNumberBetweenAsInt(0, 4);
			m_iCameraNr = GetRandomNumberBetweenAsInt(0, 3);
			sTan = GetTAN(4, TRUE);
			break;
		case 2: // Successive
			if ( !m_bStarted) {
				m_iPortNr = 0;
				m_iCameraNr = 0;
				sTan = GetTAN(4, TRUE);
			}
			else {
				// Werte hochzaehlen
				m_iCameraNr++;
				if (4 <= m_iCameraNr) {
					// Naechste Schnittstelle
					m_iCameraNr = 0;
					m_iPortNr++;
					sTan = GetTAN(4, TRUE);
					if (5 <= m_iPortNr) {
						m_iPortNr = 0;
					}
				}
				else {
					sTan = GetTAN(4, FALSE);
				}
			}
			break;
		default: // Unknown
			WK_TRACE_ERROR(_T("CViewLandauDtp::PreCreateData() WRONG DATA RepeatHow %i\n"),
																			m_iRepeatHow);
			break;
		}
	}
	else {
		sTan = GetTAN(4, TRUE);
	}

	// Schnittstellennummer
	if (m_iPortNr == 4)
	{
		sPortNr = (char)ASCII_SPACE;
	}
	else
	{
		sPortNr.Format(_T("%i"), m_iPortNr);
	}
	// Kameranummer
	if (m_iCameraNr == 2)
	{
		sCameraNr.Format(_T("%i"), 0);
	}
	else if (m_iCameraNr == 3)
	{
		sCameraNr = (char)ASCII_SPACE;
	}
	else
	{
		sCameraNr.Format(_T("%i"), m_iCameraNr+1);
	}

	CreateDataIntern(sPortNr, sCameraNr, sTan);
}
/////////////////////////////////////////////////////////////////////////////
void CDocLandauDtp::CreateDataIntern(CString sPortNr, CString sCameraNr, CString sTAN) 
{
	CString sBuffer;
	CStringA s;
	int i = 0;
	// Erst mal Daten loeschen
	m_byaData.RemoveAll();
	// STX
	m_byaData.Add((char)ASCII_STX);
	// Schnittstellennummer 1 Byte
	sBuffer = sPortNr.Right(1);
	s = sBuffer;
	for (i=0 ; i<s.GetLength() ; i++) {
		m_byaData.Add(s.GetAt(i));
	}
	m_byaData.Add((char)ASCII_US);
	// Datum 6 Byte TTMMJJ
	sBuffer = GetDate(_T("DDMMYY"));
	s = sBuffer;
	for (i=0 ; i<s.GetLength() ; i++) {
		m_byaData.Add(s.GetAt(i));
	}
	m_byaData.Add((char)ASCII_US);
	// Zeit 4 Byte HHMM
	sBuffer = GetTime(_T("HHMM"));
	s = sBuffer;
	for (i=0 ; i<s.GetLength() ; i++) {
		m_byaData.Add(s.GetAt(i));
	}
	m_byaData.Add((char)ASCII_US);
	// TAN 4 Byte
	sBuffer = sTAN;
	s = sBuffer;
	for (i=0 ; i<s.GetLength() ; i++) {
		m_byaData.Add(s.GetAt(i));
	}
	m_byaData.Add((char)ASCII_US);
	// Automatennummer 6 Byte
	sBuffer = GetWSID(6);
	s = sBuffer;
	for (i=0 ; i<s.GetLength() ; i++) {
		m_byaData.Add(s.GetAt(i));
	}
	m_byaData.Add((char)ASCII_US);
	// BLZ 8 Byte
	sBuffer = GetBLZ(8);
	s = sBuffer;
	for (i=0 ; i<s.GetLength() ; i++) {
		m_byaData.Add(s.GetAt(i));
	}
	m_byaData.Add((char)ASCII_US);
	// Kontonummer 10 Byte
	sBuffer = GetKonto(10);
	s = sBuffer;
	for (i=0 ; i<s.GetLength() ; i++) {
		m_byaData.Add(s.GetAt(i));
	}
	m_byaData.Add((char)ASCII_US);
	// Betrag 4 Byte
	sBuffer = GetBetrag(4);
	s = sBuffer;
	for (i=0 ; i<s.GetLength() ; i++) {
		m_byaData.Add(s.GetAt(i));
	}
	m_byaData.Add((char)ASCII_US);
	// Kameranummer 1 Byte
	sBuffer = sCameraNr.Right(1);
	s = sBuffer;
	for (i=0 ; i<s.GetLength() ; i++) {
		m_byaData.Add(s.GetAt(i));
	}
	// ETX
	m_byaData.Add((char)ASCII_ETX);
	// Checksumme XOR ohne STX mit ETX 1 Byte
	m_byaData.Add( GetXORCrcByte(m_byaData, 1) );
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDocLandauDtp::OnNewDocument()
{
	if (!CSDITestDoc::OnNewDocument()) {
		return FALSE;
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
CDocLandauDtp::~CDocLandauDtp()
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDocLandauDtp::OpenCom() 
{
	return m_pCom->Open(CBR_9600, 8, NOPARITY, ONESTOPBIT);
}
/////////////////////////////////////////////////////////////////////////////
// CDocLandauDtp diagnostics
#ifdef _DEBUG
void CDocLandauDtp::AssertValid() const
{
	CSDITestDoc::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CDocLandauDtp::Dump(CDumpContext& dc) const
{
	CSDITestDoc::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
// CDocLandauDtp serialization
void CDocLandauDtp::Serialize(CArchive& ar)
{
	// Versionsnummer wird mit abgespeichert,
	// muss bei jeder relevanten Aenderung erhoeht werden
	m_uSerializeVersion = 0;

	CSDITestDoc::Serialize(ar);

	if (ar.IsStoring())
	{
		// TODO: add storing code here
		ar << m_iPortNr;
		ar << m_iCameraNr;
	}
	else
	{
		// TODO: add loading code here
		ar >> m_iPortNr;
		ar >> m_iCameraNr;
	}
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDocLandauDtp, CSDITestDoc)
	//{{AFX_MSG_MAP(CDocLandauDtp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CDocLandauDtp commands
