// DocDialogbank.cpp : implementation file
//

#include "stdafx.h"
#include "DocDialogbank.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDocDialogbank
IMPLEMENT_DYNCREATE(CDocDialogbank, CSDITestDoc)
/////////////////////////////////////////////////////////////////////////////
CDocDialogbank::CDocDialogbank()
{
	m_eType = SDICT_DIALOGBANK;
	m_iCommand = 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDocDialogbank::OnNewDocument()
{
	if (!CSDITestDoc::OnNewDocument())
		return FALSE;
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
CDocDialogbank::~CDocDialogbank()
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDocDialogbank::OpenCom() 
{
	return m_pCom->Open(CBR_9600, 8, NOPARITY, ONESTOPBIT);
}
/////////////////////////////////////////////////////////////////////////////
void CDocDialogbank::CreateData() 
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
				if (1 < m_iCommand) {
					m_iCommand = 0;
				}
			}
			break;
		default: // Unknown
			WK_TRACE_ERROR("CDocDialogbank::PreCreateData() WRONG DATA RepeatHow %i\n",
																			m_iRepeatHow);
			break;
		}
	}

	CreateDataIntern();
}
/////////////////////////////////////////////////////////////////////////////
void CDocDialogbank::CreateDataIntern() 
{
	if (m_iCommand == 0) {
		CreateECCard();
	}
	else {
		CreateTANCard() ;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDocDialogbank::CreateECCard() 
{
	CString sData;

	// Track 2
	// STX
	sData  = ASCII_STX;
	// 
	sData += "2;6726532100005435606=99121010711610000?=";
	// ETX anhaengen
	sData += ASCII_ETX;

	// Track 3
	// STX
	sData  = ASCII_STX;
	// A fuer Status-Abfrage
	sData += "3;0159";
	sData += GetBLZ(8);
	sData += '=';
	sData += GetKonto(10);
	sData += GetRandomNumberWithLen(1);
	sData += '=';
	sData += "2809540050000008097013010000020000099121243329609==1=9147000000005180?;";
	// ETX anhaengen
	sData += ASCII_ETX;

	// DatenArray zusammensetzen
	m_byaData.RemoveAll();
	for (int i=0 ; i<sData.GetLength() ; i++) {
		m_byaData.Add(sData.GetAt(i));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDocDialogbank::CreateTANCard() 
{
	CString sData;

	// Track 2
	// STX
	sData  = ASCII_STX;
	// 
	sData += "2;0000000000000000000000000000000000000?4";
	// ETX anhaengen
	sData += ASCII_ETX;

	// Track 3
	// STX
	sData  = ASCII_STX;
	// A fuer Status-Abfrage
	sData += "3;450099";
	sData += GetTAN(4, TRUE);
	sData += '=';
	sData += GetBLZ(8);
	sData += "00000=2800000000000000000000000000000000000000000000000000000000000000000000000000000?3";
	// ETX anhaengen
	sData += ASCII_ETX;

	// DatenArray zusammensetzen
	m_byaData.RemoveAll();
	for (int i=0 ; i<sData.GetLength() ; i++) {
		m_byaData.Add(sData.GetAt(i));
	}
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDocDialogbank, CSDITestDoc)
	//{{AFX_MSG_MAP(CDocDialogbank)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CDocDialogbank diagnostics
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CDocDialogbank::AssertValid() const
{
	CSDITestDoc::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CDocDialogbank::Dump(CDumpContext& dc) const
{
	CSDITestDoc::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
// CDocDialogbank serialization
/////////////////////////////////////////////////////////////////////////////
void CDocDialogbank::Serialize(CArchive& ar)
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
// CDocDialogbank commands
/////////////////////////////////////////////////////////////////////////////
