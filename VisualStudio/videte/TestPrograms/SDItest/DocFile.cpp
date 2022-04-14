// FileDoc.cpp : implementation file
//

#include "stdafx.h"
#include "sditest.h"
#include "DocFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileDoc

IMPLEMENT_DYNCREATE(CFileDoc, CSDITestDoc)

CFileDoc::CFileDoc()
: m_ComParameters(_T("FileDoc"))
{
}

BOOL CFileDoc::OnNewDocument()
{
	if (!CSDITestDoc::OnNewDocument())
		return FALSE;
	m_Data.RemoveAll();
	return TRUE;
}

CFileDoc::~CFileDoc()
{
}


BEGIN_MESSAGE_MAP(CFileDoc, CSDITestDoc)
	//{{AFX_MSG_MAP(CFileDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileDoc diagnostics

#ifdef _DEBUG
void CFileDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CFileDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFileDoc serialization

void CFileDoc::Serialize(CArchive& ar)
{
	// Versionsnummer wird mit abgespeichert,
	// muss bei jeder relevanten Aenderung erhoeht werden
	m_uSerializeVersion = 0;

	CSDITestDoc::Serialize(ar);

	// the Original File
	m_Data.Serialize(ar);

	if (ar.IsStoring())
	{
		// TODO: add storing code here
		ar << m_ComParameters.GetBaudRate();
		ar << m_ComParameters.GetDataBits();
		ar << m_ComParameters.GetParity();
		ar << m_ComParameters.GetStopBits();
	}
	else
	{
		// TODO: add loading code here
		DWORD dw;
		BYTE b;
		ar >> dw;
		m_ComParameters.SetBaudRate(dw);
		ar >> b;
		m_ComParameters.SetDataBits(b);
		ar >> b;
		m_ComParameters.SetParity(b);
		ar >> b;
		m_ComParameters.SetStopBits(b);
	}
}
/////////////////////////////////////////////////////////////////////////////
// CFileDoc commands
BOOL CFileDoc::OpenCom()
{
	return m_pCom->Open(m_ComParameters.GetBaudRate(), 
						m_ComParameters.GetDataBits(), 
						m_ComParameters.GetParity(), 
						m_ComParameters.GetStopBits());
}
/////////////////////////////////////////////////////////////////////////////
BOOL CFileDoc::IsDataOK()
{
	return m_Data.GetSize()>0;
}
/////////////////////////////////////////////////////////////////////////////
void CFileDoc::CreateData()
{
	m_byaData.Append(m_Data);
}
/////////////////////////////////////////////////////////////////////////////
void CFileDoc::LoadFile(const CString& sPathName)
{
	CFile file;

	if (file.Open(sPathName,CFile::modeRead))
	{
		m_Data.RemoveAll();
		BYTE b = 0;

		while (1==file.Read(&b,1))
		{
			m_Data.Add(b);
		}

		file.Close();

		WK_TRACE(_T("loaded binary file %s\n"),sPathName);
	}
}
/////////////////////////////////////////////////////////////////////////////
CComParameters CFileDoc::GetComParameters()
{
	return m_ComParameters;
}
/////////////////////////////////////////////////////////////////////////////
void CFileDoc::SetComParameters(const CComParameters& com)
{
	m_ComParameters = com;
}
