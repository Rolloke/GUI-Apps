// ClientPicTestDoc.cpp : implementation of the CClientPicTestDoc class
//

#include "stdafx.h"
#include "ClientPicTest.h"

#include "ClientPicTestDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClientPicTestDoc

IMPLEMENT_DYNCREATE(CClientPicTestDoc, CDocument)

BEGIN_MESSAGE_MAP(CClientPicTestDoc, CDocument)
	//{{AFX_MSG_MAP(CClientPicTestDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClientPicTestDoc construction/destruction

CClientPicTestDoc::CClientPicTestDoc()
{
}

CClientPicTestDoc::~CClientPicTestDoc()
{
}

BOOL CClientPicTestDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CClientPicTestDoc serialization

void CClientPicTestDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	}
}

/////////////////////////////////////////////////////////////////////////////
// CClientPicTestDoc diagnostics

#ifdef _DEBUG
void CClientPicTestDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CClientPicTestDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CClientPicTestDoc commands
