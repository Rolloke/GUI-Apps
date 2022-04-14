// MPEG4PlayDoc.cpp : implementation of the CMPEG4PlayDoc class
//

#include "stdafx.h"
#include "MPEG4Play.h"

#include "MPEG4PlayDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMPEG4PlayDoc

IMPLEMENT_DYNCREATE(CMPEG4PlayDoc, CDocument)

BEGIN_MESSAGE_MAP(CMPEG4PlayDoc, CDocument)
	//{{AFX_MSG_MAP(CMPEG4PlayDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMPEG4PlayDoc construction/destruction

CMPEG4PlayDoc::CMPEG4PlayDoc()
{
	// TODO: add one-time construction code here
	m_pMPEG4Decoder = NULL;
	m_pMPEG4Encoder = NULL;

	ZeroMemory(streamInfoBuffer, STREAM_INFO2_LENGTH);
}

CMPEG4PlayDoc::~CMPEG4PlayDoc()
{
	if(m_pMPEG4Decoder != NULL)
	{
		delete m_pMPEG4Decoder;
		m_pMPEG4Decoder = NULL;
	}

	if(m_pMPEG4Encoder != NULL)
	{
		delete m_pMPEG4Encoder;
		m_pMPEG4Encoder = NULL;
	}
}

BOOL CMPEG4PlayDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CMPEG4PlayDoc serialization

void CMPEG4PlayDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
		TRACE(_T("open document\n"));
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMPEG4PlayDoc diagnostics

#ifdef _DEBUG
void CMPEG4PlayDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMPEG4PlayDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMPEG4PlayDoc commands

BOOL CMPEG4PlayDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	
	// TODO: Add your specialized creation code here
	// evtl. altes MPEG4Encoder-Objekt loeschen
	if(m_pMPEG4Encoder != NULL)
	{
		delete m_pMPEG4Encoder;
		m_pMPEG4Encoder = NULL;
	}

	// evtl. altes MPEG4Decoder-Objekt loeschen 
	if(m_pMPEG4Decoder != NULL)
	{
		delete m_pMPEG4Decoder;
		m_pMPEG4Decoder = NULL;
	}

	// Soll ein YUV-File kodiert oder ein H263/MPEG4-File dekodiert werden?
	CString str(lpszPathName);
	if(str.Find(_T(".yuv")) >= 0)
	{	// ...YUV-File kodieren
		m_pMPEG4Encoder = new CMPEG4Encoder(lpszPathName);
	}
	else
	{// ...H263/MPEG4-File dekodieren
		m_pMPEG4Decoder = new CMPEG4Decoder(lpszPathName);
	}

	return TRUE;
}


CMPEG4Decoder* CMPEG4PlayDoc::GetMPEG4Decoder()
{
	return m_pMPEG4Decoder;
}


CMPEG4Encoder* CMPEG4PlayDoc::GetMPEG4Encoder()
{
	return m_pMPEG4Encoder;
}
