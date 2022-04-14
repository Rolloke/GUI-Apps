/* GlobalReplace: CSDITestKebaDoc --> CDocKebaPc2000 */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: DocKebaPc2000.cpp $
// ARCHIVE:		$Archive: /Project/TestPrograms/SDITest/DocKebaPc2000.cpp $
// CHECKIN:		$Date: 19.12.05 17:18 $
// VERSION:		$Revision: 15 $
// LAST CHANGE:	$Modtime: 19.12.05 16:47 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "DocKebaPc2000.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDocKebaPc2000
IMPLEMENT_DYNCREATE(CDocKebaPc2000, CSDITestDoc)
/////////////////////////////////////////////////////////////////////////////
CDocKebaPc2000::CDocKebaPc2000()
{
	m_eType = SDICT_KEBA_PC2000;
	m_iCardReader = 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDocKebaPc2000::OnNewDocument()
{
	if (!CSDITestDoc::OnNewDocument()) {
		return FALSE;
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
CDocKebaPc2000::~CDocKebaPc2000()
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDocKebaPc2000::OpenCom() 
{
	return m_pCom->Open(CBR_9600, 8, NOPARITY, ONESTOPBIT);
}
/////////////////////////////////////////////////////////////////////////////
void CDocKebaPc2000::CreateData() 
{
	CString sCardReader;

	if (m_bRepeat) {
		switch (m_iRepeatHow) {
		case 0: // Chosen Units
			// Eingestellte Werte unveraendert lassen
			break;
		case 1: // Random
			m_iCardReader = GetRandomNumberBetweenAsInt(0, 1);
			break;
		case 2: // Successive
			if ( !m_bStarted) {
				m_iCardReader = 0;
			}
			else {
				// Werte hochzaehlen
				m_iCardReader++;
				if (2 <= m_iCardReader) {
					m_iCardReader = 0;
				}
			}
			break;
		default: // Unknown
			WK_TRACE_ERROR(_T("CViewKebaPc2000::PreCreateData() WRONG DATA RepeatHow %i\n"),
																			m_iRepeatHow);
			break;
		}
	}

	// Kartenleser
	sCardReader.Format(_T("L(%i)"), m_iCardReader+1);

	CreateDataIntern(sCardReader);
}
/////////////////////////////////////////////////////////////////////////////
void CDocKebaPc2000::CreateDataIntern(CString sCardReader)
{
	CString sBuffer;
	// 5 mal Space
	sBuffer = (char)ASCII_SPACE;
	sBuffer += (char)ASCII_SPACE;
	sBuffer += (char)ASCII_SPACE;
	sBuffer += (char)ASCII_SPACE;
	sBuffer += (char)ASCII_SPACE;
	// Kartenleser 4 Byte
	sBuffer += sCardReader.Right(4);
	sBuffer += (char)ASCII_SPACE;
	// Wochentag deutsch 2 Byte
	sBuffer += GetDay(2, TRUE);
	sBuffer += (char)ASCII_SPACE;
	// Datum 8 Byte TT.MM.JJ
	sBuffer += GetDate(_T("DD.MM.YY"));
	sBuffer += (char)ASCII_SPACE;
	// Zeit 4 Byte HH:MM:SS
	sBuffer += GetTime(_T("HH:MM:SS"));
	sBuffer += (char)ASCII_SPACE;
	sBuffer += (char)ASCII_SPACE;
	sBuffer += (char)ASCII_SPACE;
	// Trenner _T("BLZ:") 4 Byte
	sBuffer += _T("BLZ:");
	sBuffer += (char)ASCII_SPACE;
	// BLZ 8 Byte
	sBuffer += GetBLZ(8);
	sBuffer += (char)ASCII_SPACE;
	sBuffer += (char)ASCII_SPACE;
	// Trenner _T("Kto:") 4 Byte
	sBuffer += _T("Kto:");
	sBuffer += (char)ASCII_SPACE;
	// Kontonummer 16 Byte
	sBuffer += GetKonto(16);
	sBuffer += (char)ASCII_CR;
	sBuffer += (char)ASCII_LF;

	m_byaData.RemoveAll();
	CStringA s(sBuffer);
	for (int i=0 ; i<s.GetLength() ; i++) {
		m_byaData.Add(s.GetAt(i));
	}
}
/////////////////////////////////////////////////////////////////////////////
// CDocKebaPc2000 diagnostics
#ifdef _DEBUG
void CDocKebaPc2000::AssertValid() const
{
	CSDITestDoc::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CDocKebaPc2000::Dump(CDumpContext& dc) const
{
	CSDITestDoc::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
// CDocKebaPc2000 serialization
void CDocKebaPc2000::Serialize(CArchive& ar)
{
	// Versionsnummer wird mit abgespeichert,
	// muss bei jeder relevanten Aenderung erhoeht werden
	m_uSerializeVersion = 0;

	CSDITestDoc::Serialize(ar);

	if (ar.IsStoring())
	{
		// TODO: add storing code here
		ar << m_iCardReader;
	}
	else
	{
		// TODO: add loading code here
		ar >> m_iCardReader;
	}
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDocKebaPc2000, CSDITestDoc)
	//{{AFX_MSG_MAP(CDocKebaPc2000)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CDocKebaPc2000 commands
