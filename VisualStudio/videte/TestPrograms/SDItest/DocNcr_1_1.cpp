/* GlobalReplace: CSDITestNcrDoc --> CDocNcr_1_1 */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: DocNcr_1_1.cpp $
// ARCHIVE:		$Archive: /Project/TestPrograms/SDITest/DocNcr_1_1.cpp $
// CHECKIN:		$Date: 19.12.05 17:18 $
// VERSION:		$Revision: 16 $
// LAST CHANGE:	$Modtime: 19.12.05 16:47 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "DocNcr_1_1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDocNcr_1_1
IMPLEMENT_DYNCREATE(CDocNcr_1_1, CSDITestDoc)
/////////////////////////////////////////////////////////////////////////////
CDocNcr_1_1::CDocNcr_1_1()
{
	m_eType = SDICT_NCR_VER_1_1;
	m_iCommand = 0;
	m_bIncreaseTAN = TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDocNcr_1_1::OnNewDocument()
{
	if (!CSDITestDoc::OnNewDocument()) {
		return FALSE;
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
CDocNcr_1_1::~CDocNcr_1_1()
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDocNcr_1_1::OpenCom() 
{
	return m_pCom->Open(CBR_1200, 7, ODDPARITY, ONESTOPBIT);
}
/////////////////////////////////////////////////////////////////////////////
void CDocNcr_1_1::CreateData() 
{
	if (m_bRepeat) {
		switch (m_iRepeatHow) {
		case 0: // Chosen Units
			// Eingestellte Werte unveraendert lassen
			if (m_iCommand == 0) {
				m_bIncreaseTAN = !m_bIncreaseTAN;
			}
			break;
		case 1: // Random
			m_iCommand = GetRandomNumberBetweenAsInt(0, 1);
			if (m_iCommand == 0) {
				m_bIncreaseTAN = !m_bIncreaseTAN;
			}
			break;
		case 2: // Successive
			if ( !m_bStarted) {
				m_iCommand = 0;
				m_bIncreaseTAN = TRUE;
			}
			else {
				// 
				if (m_iCommand == 0) {
					if (m_bIncreaseTAN) {
						m_bIncreaseTAN = FALSE;
					}
					else {
						m_iCommand = 1;
						m_bIncreaseTAN = TRUE;
					}
				}
				else if (m_iCommand == 1) {
					m_iCommand = 0;
				}
				else {
					// Kann eigentlich gar nicht vorkommen
					m_iCommand = 0;
					m_bIncreaseTAN = TRUE;
				}
			}
			break;
		default: // Unknown
			WK_TRACE_ERROR(_T("CDocNcr_1_1::PreCreateData() WRONG DATA RepeatHow %i\n"),
																			m_iRepeatHow);
			break;
		}
	}

	CreateDataIntern();
}
/////////////////////////////////////////////////////////////////////////////
void CDocNcr_1_1::CreateDataIntern() 
{
	CString sData, sBuffer;

	// Startkommando
	// STX
	sData = (char)ASCII_STX;
	// Zeit 4 Byte HHMM revers
	sBuffer = GetTimeHHMMSS();
	sBuffer.MakeReverse();
	sData += sBuffer.Right(4);
	sData += (TCHAR)(char)ASCII_US;
	// Datum 6 Byte TTMMJJ revers
	sBuffer = GetDate(_T("YYMMDD"));
	sBuffer.MakeReverse();
	sData += sBuffer;
	sData += (TCHAR)(char)ASCII_US;

	if (0 == m_iCommand) {
		// TAN 3 Byte
		sBuffer = GetTAN(3, m_bIncreaseTAN);
		sBuffer.MakeReverse();
		sData += sBuffer;
		sData += (TCHAR)(char)ASCII_US;
		// Ein leeres Feld !!!
		sData += (TCHAR)(char)ASCII_US;
		// BLZ 8 Byte
		sBuffer = GetBLZ(8);
		sBuffer.MakeReverse();
		sData += sBuffer;
		sData += (TCHAR)(char)ASCII_US;
		// Kontonummer 10 Byte
		sBuffer = GetKonto(10);
		sBuffer.MakeReverse();
		sData += sBuffer;
	}
	else  {
		// TAN 4 Byte
		sBuffer = GetTAN(4, TRUE);
		sBuffer.MakeReverse();
		sData += sBuffer;
		sData += (TCHAR)(char)ASCII_US;
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
// CDocNcr_1_1 diagnostics
#ifdef _DEBUG
void CDocNcr_1_1::AssertValid() const
{
	CSDITestDoc::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CDocNcr_1_1::Dump(CDumpContext& dc) const
{
	CSDITestDoc::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
// CDocNcr_1_1 serialization
void CDocNcr_1_1::Serialize(CArchive& ar)
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
BEGIN_MESSAGE_MAP(CDocNcr_1_1, CSDITestDoc)
	//{{AFX_MSG_MAP(CDocNcr_1_1)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CDocNcr_1_1 commands
