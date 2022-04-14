// UpdateDoc.cpp : implementation of the CUpdateDoc class
//

#include "stdafx.h"
#include "UpdateHandler.h"

#include "UpdateDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUpdateDoc

IMPLEMENT_DYNCREATE(CUpdateDoc, CDocument)

BEGIN_MESSAGE_MAP(CUpdateDoc, CDocument)
	//{{AFX_MSG_MAP(CUpdateDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUpdateDoc construction/destruction

CUpdateDoc::CUpdateDoc()
{
	// TODO: add one-time construction code here

}

CUpdateDoc::~CUpdateDoc()
{
}

BOOL CUpdateDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CUpdateDoc serialization

void CUpdateDoc::Serialize(CArchive& ar)
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
// CUpdateDoc diagnostics

#ifdef _DEBUG
void CUpdateDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CUpdateDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CUpdateDoc commands
