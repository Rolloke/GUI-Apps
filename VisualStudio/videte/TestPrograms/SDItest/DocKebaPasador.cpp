/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: DocKebaPasador.cpp $
// ARCHIVE:		$Archive: /Project/TestPrograms/SDITest/DocKebaPasador.cpp $
// CHECKIN:		$Date: 19.12.05 17:18 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 19.12.05 16:47 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "DocKebaPasador.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CDocKebaPasador
IMPLEMENT_DYNCREATE(CDocKebaPasador, CSDITestDoc)
//////////////////////////////////////////////////////////////////////
CDocKebaPasador::CDocKebaPasador()
{
	m_eType = SDICT_KEBA_PASADOR;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDocKebaPasador::OnNewDocument()
{
	if (!CSDITestDoc::OnNewDocument()) {
		return FALSE;
	}
	return TRUE;
}
CDocKebaPasador::~CDocKebaPasador()
{

}
/////////////////////////////////////////////////////////////////////////////
BOOL CDocKebaPasador::OpenCom() 
{
	return m_pCom->Open(CBR_9600, 8, NOPARITY, ONESTOPBIT);
}
/////////////////////////////////////////////////////////////////////////////
void CDocKebaPasador::CreateData() 
{
	if (m_bRepeat) {
		switch (m_iRepeatHow) {
		case 0: // Chosen Units
			// Eingestellte Werte unveraendert lassen
			break;
		case 1: // Random
			break;
		case 2: // Successive
			if ( !m_bStarted) {
			}
			else {
				// Werte hochzaehlen
			}
			break;
		default: // Unknown
			WK_TRACE_ERROR(_T("CViewKebaPc2000::PreCreateData() WRONG DATA RepeatHow %i\n"),
																			m_iRepeatHow);
			break;
		}
	}

	CreateDataIntern();
}
/////////////////////////////////////////////////////////////////////////////
void CDocKebaPasador::CreateDataIntern()
{
	CString sBuffer = m_sFileName;
	sBuffer = StringReplace(sBuffer, _T("<CR>"), CString((char)ASCII_CR));
	sBuffer = StringReplace(sBuffer, _T("<LF>"), CString((char)ASCII_LF));

	m_byaData.RemoveAll();
	CStringA s(sBuffer);
	for (int i=0 ; i<s.GetLength() ; i++) {
		m_byaData.Add(s.GetAt(i));
	}
}
/////////////////////////////////////////////////////////////////////////////
// CDocKebaPasador diagnostics
#ifdef _DEBUG
void CDocKebaPasador::AssertValid() const
{
	CSDITestDoc::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CDocKebaPasador::Dump(CDumpContext& dc) const
{
	CSDITestDoc::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
// CDocKebaPasador serialization
void CDocKebaPasador::Serialize(CArchive& ar)
{
	// Versionsnummer wird mit abgespeichert,
	// muss bei jeder relevanten Aenderung erhoeht werden
	m_uSerializeVersion = 0;

	CSDITestDoc::Serialize(ar);

	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDocKebaPasador, CSDITestDoc)
	//{{AFX_MSG_MAP(CDocKebaPasador)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CDocKebaPasador commands
