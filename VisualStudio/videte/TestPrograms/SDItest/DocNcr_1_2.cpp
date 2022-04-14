/* GlobalReplace: CSDITestNcrNewDoc --> CDocNcr_1_2 */
/* GlobalReplace: CSDITestNcrDoc --> CDocNcr_1_1 */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: DocNcr_1_2.cpp $
// ARCHIVE:		$Archive: /Project/TestPrograms/SDITest/DocNcr_1_2.cpp $
// CHECKIN:		$Date: 19.12.05 17:18 $
// VERSION:		$Revision: 8 $
// LAST CHANGE:	$Modtime: 19.12.05 16:58 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "DocNcr_1_2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDocNcr_1_2
IMPLEMENT_DYNCREATE(CDocNcr_1_2, CDocNcr_1_1)
/////////////////////////////////////////////////////////////////////////////
CDocNcr_1_2::CDocNcr_1_2()
{
	m_eType = SDICT_NCR_VER_1_2;
	m_iVariant = 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDocNcr_1_2::OnNewDocument()
{
	if (!CDocNcr_1_1::OnNewDocument()) {
		return FALSE;
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
CDocNcr_1_2::~CDocNcr_1_2()
{
}
/////////////////////////////////////////////////////////////////////////////
void CDocNcr_1_2::CreateDataIntern() 
{
	CString sData, sBuffer;

	// Startkommando
	// STX
	sData = (char)ASCII_STX;
	// Zeit 4 Byte HHMM revers
	sBuffer = GetTimeHHMMSS();
	sBuffer.MakeReverse();
	sData += sBuffer.Right(4);
	sData += (char)ASCII_US;
	// Datum 6 Byte TTMMJJ revers
	sBuffer = GetDate("YYMMDD");
	sBuffer.MakeReverse();
	sData += sBuffer;
	sData += (char)ASCII_US;

	// Beim Startkommando
	if (0 == m_iCommand) {
		// Bei Variante 3 ist TAN 4 Byte
		if (2 == m_iVariant) {
			sBuffer = GetTAN(4, m_bIncreaseTAN);
		}
		// Bei Variante 1 und 2 ist TAN 3 Byte
		else {
			sBuffer = GetTAN(3, m_bIncreaseTAN);
		}
		sBuffer.MakeReverse();
		sData += sBuffer;
		sData += (char)ASCII_US;
		// Bei Variante 2 und 3 ein leeres Feld
		if (0 != m_iVariant) {
			sData += (char)ASCII_US;
		}
		// BLZ 8 Byte
		sBuffer = GetBLZ(8);
		sBuffer.MakeReverse();
		sData += sBuffer;
		sData += (char)ASCII_US;
		// Kontonummer 10 Byte
		sBuffer = GetKonto(10);
		sBuffer.MakeReverse();
		sData += sBuffer;
		// Bei Variante 1 ein geheimes Byte anhaengen!!!
		if (0 == m_iVariant) {
			sData += (char)ASCII_US;
			sData += '!';
		}
	}
	// Bei der Diagnose-Startmeldung
	else  {
		// TAN 4 Byte
		sBuffer = GetTAN(4, TRUE);
		sBuffer.MakeReverse();
		sData += sBuffer;
		sData += (char)ASCII_US;
		// Terminalnummer 6 Byte
		sBuffer = GetRandomNumberWithLen(6);
		sBuffer.MakeReverse();
		sData += sBuffer;
	}
	// ETX
	sData += (char)ASCII_ETX;

	m_byaData.RemoveAll();
	CStringA s(sData);
	for (int i=0 ; i<s.GetLength() ; i++) {
		m_byaData.Add(s.GetAt(i));
	}
	// Checksumme XOR ohne STX mit ETX 1 Byte
	m_byaData.Add( GetXORCrcByte(m_byaData, 1) );
}
/////////////////////////////////////////////////////////////////////////////
// CDocNcr_1_2 diagnostics
#ifdef _DEBUG
void CDocNcr_1_2::AssertValid() const
{
	CDocNcr_1_1::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CDocNcr_1_2::Dump(CDumpContext& dc) const
{
	CDocNcr_1_1::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
// CDocNcr_1_2 serialization
void CDocNcr_1_2::Serialize(CArchive& ar)
{
	// Versionsnummer wird mit abgespeichert,
	// muss bei jeder relevanten Aenderung erhoeht werden
	m_uSerializeVersion = 0;

	CSDITestDoc::Serialize(ar);

	if (ar.IsStoring())
	{
		// TODO: add storing code here
		ar << m_iCommand; 
		ar << m_iVariant; 
	}
	else
	{
		// TODO: add loading code here
		ar >> m_iCommand;
		ar >> m_iVariant;
	}
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDocNcr_1_2, CDocNcr_1_1)
	//{{AFX_MSG_MAP(CDocNcr_1_2)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CDocNcr_1_2 commands
