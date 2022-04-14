/* GlobalReplace: CSDITestGermanParcelDoc --> CDocGermanParcel */
// SDITestGermanParcelDoc.cpp : implementation file
//

#include "stdafx.h"
#include "DocGermanParcel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDocGermanParcel
IMPLEMENT_DYNCREATE(CDocGermanParcel, CSDITestDoc)
/////////////////////////////////////////////////////////////////////////////
CDocGermanParcel::CDocGermanParcel()
{
	m_eType = SDICT_GERMAN_PARCEL;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDocGermanParcel::OnNewDocument()
{
	if (!CSDITestDoc::OnNewDocument())
		return FALSE;
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
CDocGermanParcel::~CDocGermanParcel()
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDocGermanParcel::OpenCom() 
{
	return m_pCom->Open(CBR_9600, 8, NOPARITY, ONESTOPBIT);
}
/////////////////////////////////////////////////////////////////////////////
void CDocGermanParcel::CreateData() 
{
	CreateDataIntern();
}
/////////////////////////////////////////////////////////////////////////////
void CDocGermanParcel::CreateDataIntern()
{
	CString sBuffer;
	int i = 0;
	// Erst mal Daten loeschen
	m_byaData.RemoveAll();

	sBuffer = GetKonto(11);

	// Paketnummer
	CStringA s(sBuffer);
	for (i=0 ; i<s.GetLength() ; i++) {
		m_byaData.Add(s.GetAt(i));
	}

	// Umdrehen, um leichter rechnen zu koennen
	s.MakeReverse();

	// Pruefsumme berechnen
	int iSum = 0;
	int iDigit = 0;
	int iWeight = 3;
	char cChar = '0';
	for (i=0 ; i<s.GetLength() ; i++) {
		cChar = s.GetAt(i);
		iDigit = atoi(&cChar);
		iSum += iDigit * iWeight;
		if (iWeight == 3) {
			iWeight = 1;
		}
		else {
			iWeight = 3;
		}
	}

	// Ggf. Summe erhoehen
	iSum += 1;

	// Pruefziffer berechnen
	int iRemains = iSum % 10;
	iDigit = (10-iRemains)%10;

	// Pruefziffer
	m_byaData.Add( (BYTE)(iDigit + 0x30) );
	// CR
	m_byaData.Add(ASCII_CR);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDocGermanParcel, CSDITestDoc)
	//{{AFX_MSG_MAP(CDocGermanParcel)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDocGermanParcel diagnostics

#ifdef _DEBUG
void CDocGermanParcel::AssertValid() const
{
	CSDITestDoc::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CDocGermanParcel::Dump(CDumpContext& dc) const
{
	CSDITestDoc::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDocGermanParcel serialization

void CDocGermanParcel::Serialize(CArchive& ar)
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
// CDocGermanParcel commands
