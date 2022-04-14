/* GlobalReplace: CSDITestEngelDoc --> CDocEngelMultipass */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: DocEngelMultipass.cpp $
// ARCHIVE:		$Archive: /Project/TestPrograms/SDITest/DocEngelMultipass.cpp $
// CHECKIN:		$Date: 19.12.05 17:18 $
// VERSION:		$Revision: 13 $
// LAST CHANGE:	$Modtime: 19.12.05 16:47 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "DocEngelMultipass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDocEngelMultipass
IMPLEMENT_DYNCREATE(CDocEngelMultipass, CSDITestDoc)
/////////////////////////////////////////////////////////////////////////////
CDocEngelMultipass::CDocEngelMultipass()
{
	m_eType = SDICT_ENGEL_MULTIPASS;
	m_iCommand = 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDocEngelMultipass::OnNewDocument()
{
	if (!CSDITestDoc::OnNewDocument())
		return FALSE;
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
CDocEngelMultipass::~CDocEngelMultipass()
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDocEngelMultipass::OpenCom() 
{
	return m_pCom->Open(CBR_4800, 8, NOPARITY, ONESTOPBIT);
}
/////////////////////////////////////////////////////////////////////////////
void CDocEngelMultipass::CreateData() 
{
	if (m_bRepeat) {
		switch (m_iRepeatHow) {
		case 0: // Chosen Units
			// Eingestellte Werte unveraendert lassen
			break;
		case 1: // Random
			m_iCommand = GetRandomNumberBetweenAsInt(0, 1);
			break;
		case 2: // Successive
			if ( !m_bStarted) {
				m_iCommand = 0;
			}
			else {
				// Werte hochzaehlen
				m_iCommand++;
				if (2 <= m_iCommand) {
					m_iCommand = 0;
				}
			}
			break;
		default: // Unknown
			WK_TRACE_ERROR(_T("CDocEngelMultipass::PreCreateData() WRONG DATA RepeatHow %i\n"),
																			m_iRepeatHow);
			break;
		}
	}

	CreateDataIntern();
}
/////////////////////////////////////////////////////////////////////////////
void CDocEngelMultipass::CreateDataIntern() 
{
	CString sBuffer;
	int i = 0;
	// Erst mal Daten loeschen
	m_byaData.RemoveAll();

	switch (m_iCommand) {
	case 0:	// L-Kommando
		sBuffer += _T('L');
		break;
	case 1:	// E-Kommando
		sBuffer += _T('E');
		break;
	default: // Unknown
		WK_TRACE_ERROR(_T("CDocEngelMultipass::CreateData() WRONG Command %i\n"), m_iCommand);
		break;
	}
	sBuffer += (char)ASCII_SPACE;
	// Datum 8 Byte TT.MM.JJ
	sBuffer += GetDate(_T("DD.MM.YY"));
	// Zeit 8 Byte HH:MM:SS
	sBuffer += GetTime(_T("HH:MM:SS"));
	sBuffer += (char)ASCII_SPACE;

	switch (m_iCommand) {
	case 0:	// L-Kommando
		// BLZ 8 Byte
		sBuffer += GetBLZ(8);
		sBuffer += (char)ASCII_SPACE;
		// Kontonummer 10 Byte
		sBuffer += GetKonto(10);
		break;
	case 1:	// E-Kommando
		// Sonderschluessel 4 Byte
		sBuffer += GetBLZ(4);
		sBuffer += (char)ASCII_SPACE;
		// Kartennummer 14 Byte
		sBuffer += GetKonto(14);
		break;
	default: // Unknown
		WK_TRACE_ERROR(_T("CDocEngelMultipass::CreateData() WRONG Command %i\n"), m_iCommand);
		break;
	}
	sBuffer += (char)ASCII_SPACE;
	// CR
	sBuffer += (char)ASCII_CR;

	CStringA s(sBuffer);
	for (i=0 ; i<s.GetLength() ; i++) {
		m_byaData.Add(s.GetAt(i));
	}
}
/////////////////////////////////////////////////////////////////////////////
// CDocEngelMultipass diagnostics
#ifdef _DEBUG
void CDocEngelMultipass::AssertValid() const
{
	CSDITestDoc::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CDocEngelMultipass::Dump(CDumpContext& dc) const
{
	CSDITestDoc::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
// CDocEngelMultipass serialization
void CDocEngelMultipass::Serialize(CArchive& ar)
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
BEGIN_MESSAGE_MAP(CDocEngelMultipass, CSDITestDoc)
	//{{AFX_MSG_MAP(CDocEngelMultipass)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CDocEngelMultipass commands
