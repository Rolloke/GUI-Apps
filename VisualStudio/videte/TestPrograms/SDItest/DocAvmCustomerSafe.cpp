// DocAvmCustomerSafe.cpp : implementation file
//

#include "stdafx.h"
#include "DocAvmCustomerSafe.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDocAvmCustomerSafe
IMPLEMENT_DYNCREATE(CDocAvmCustomerSafe, CSDITestDoc)
/////////////////////////////////////////////////////////////////////////////
CDocAvmCustomerSafe::CDocAvmCustomerSafe()
{
	m_eType = SDICT_AVM_CUSTOMER_SAFE;
	m_iCommand = 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDocAvmCustomerSafe::OnNewDocument()
{
	if (!CSDITestDoc::OnNewDocument())
		return FALSE;
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
CDocAvmCustomerSafe::~CDocAvmCustomerSafe()
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDocAvmCustomerSafe::OpenCom() 
{
	return m_pCom->Open(CBR_9600, 8, NOPARITY, ONESTOPBIT);
}
/////////////////////////////////////////////////////////////////////////////
void CDocAvmCustomerSafe::CreateData() 
{
	if (m_bRepeat) {
		switch (m_iRepeatHow) {
		case 0: // Chosen Units
			// Eingestellte Werte unveraendert lassen
			break;
		case 1: // Random
			m_iCommand = GetRandomNumberBetweenAsInt(0, 2);
			break;
		case 2: // Successive
			if ( !m_bStarted) {
				m_iCommand = 0;
			}
			else {
				// Werte hochzaehlen
				m_iCommand++;
				if (2 < m_iCommand) {
					m_iCommand = 0;
				}
			}
			break;
		default: // Unknown
			WK_TRACE_ERROR("CDocAvmCustomerSafe::PreCreateData() WRONG DATA RepeatHow %i\n",
																			m_iRepeatHow);
			break;
		}
	}

	CreateDataIntern();
}
/////////////////////////////////////////////////////////////////////////////
void CDocAvmCustomerSafe::CreateDataIntern() 
{
	CString sBuffer, sTemp;
	int i = 0;
	// Erst mal Daten loeschen
	m_byaData.RemoveAll();

	// Vorgangsnummer 4 Byte
	sBuffer += GetTAN(4, TRUE);
	sBuffer += ASCII_SPACE;
	sBuffer += ASCII_SPACE;
	// Datum 8 Byte TT.MM.JJ
	sBuffer += GetDate("DD.MM.YY");
	sBuffer += ASCII_SPACE;
	// Zeit 5 Byte HH:MM
	sBuffer += GetTime("HH:MM");
	sBuffer += ASCII_SPACE;
	sBuffer += ASCII_SPACE;
	sBuffer += "KL#1";
	sBuffer += ASCII_SPACE;
	sBuffer += ASCII_SPACE;
	sBuffer += ASCII_SPACE;
	sBuffer += ASCII_SPACE;
	sBuffer += ASCII_SPACE;
	sBuffer += ASCII_SPACE;

	// Vorgang
	switch (m_iCommand) {
	case 0:	// Zugang erlaubt
		sBuffer += "Zugang prüfen : (Kunde)";
		break;
	case 1:	// Zugang erlaubt
		sBuffer += "Zugang erlaubt : (Kunde)";
		break;
	case 2:	// Fachfreigabe
		sBuffer += "Fachfreigabe vorgemerkt :";
		break;
	default: // Unknown
		WK_TRACE_ERROR("CDocAvmCustomerSafe::CreateDataIntern() WRONG Command %i\n",
																		m_iCommand);
		break;
	}
	sBuffer += ASCII_SPACE;

	// Kundennummer
	sBuffer += '#';
	sBuffer += GetWSID(4, TRUE);
	sBuffer += ASCII_SPACE;
	// Vorname
	sBuffer += GetFirstName();
	sBuffer += ASCII_SPACE;
	// Nachname
	sBuffer += GetLastName();
	// Fach Nr
	switch (m_iCommand) {
	case 0:	// Zugang erlaubt
		// nichts
		break;
	case 2:	// Fachfreigabe
		sBuffer += ": #";
		sBuffer += GetBetrag(4, TRUE);
		break;
	default: // Unknown
		WK_TRACE_ERROR("CDocAvmCustomerSafe::CreateDataIntern() WRONG Command %i\n",
																		m_iCommand);
		break;
	}

	// CR
	sBuffer += ASCII_CR;

	for (i=0 ; i<sBuffer.GetLength() ; i++) {
		m_byaData.Add(sBuffer.GetAt(i));
	}
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDocAvmCustomerSafe, CSDITestDoc)
	//{{AFX_MSG_MAP(CDocAvmCustomerSafe)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CDocAvmCustomerSafe diagnostics
#ifdef _DEBUG
void CDocAvmCustomerSafe::AssertValid() const
{
	CSDITestDoc::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CDocAvmCustomerSafe::Dump(CDumpContext& dc) const
{
	CSDITestDoc::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
// CDocAvmCustomerSafe serialization
void CDocAvmCustomerSafe::Serialize(CArchive& ar)
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
// CDocAvmCustomerSafe commands
