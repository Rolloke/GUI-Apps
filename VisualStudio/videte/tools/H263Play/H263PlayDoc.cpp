// H263PlayDoc.cpp : implementation of the CH263PlayDoc class
//

#include "stdafx.h"
#include "H263Play.h"

#include "H263PlayDoc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CH263PlayDoc

IMPLEMENT_DYNCREATE(CH263PlayDoc, CDocument)

BEGIN_MESSAGE_MAP(CH263PlayDoc, CDocument)
	//{{AFX_MSG_MAP(CH263PlayDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CH263PlayDoc construction/destruction

CH263PlayDoc::CH263PlayDoc()
{
	m_pBuffer = NULL;
	m_dwLen = 0;
}

CH263PlayDoc::~CH263PlayDoc()
{
	if (m_pBuffer)
	{
		delete [] m_pBuffer;
		m_dwLen = 0;
	}
}

BOOL CH263PlayDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CH263PlayDoc serialization

void CH263PlayDoc::Serialize(CArchive& ar)
{
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
// CH263PlayDoc diagnostics

#ifdef _DEBUG
void CH263PlayDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CH263PlayDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CH263PlayDoc commands

BOOL CH263PlayDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	CFile temp;
	CFileException cfe;
	CFileStatus cfs;

	if (temp.Open(lpszPathName,CFile::modeRead,&cfe))
	{
		if (temp.GetStatus(cfs))
		{
			m_dwLen = cfs.m_size;
			m_pBuffer = new BYTE[m_dwLen];
			if (m_dwLen!=temp.Read(m_pBuffer,m_dwLen))
			{
				delete [] m_pBuffer;
				m_pBuffer = NULL;
				m_dwLen = 0;
			}
		}
		temp.Close();
	}
	
	return TRUE;
}
