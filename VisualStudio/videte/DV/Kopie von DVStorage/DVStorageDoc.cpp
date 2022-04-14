// DVStorageDoc.cpp : implementation of the CDVStorageDoc class
//

#include "stdafx.h"
#include "DVStorage.h"

#include "DVStorageDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDVStorageDoc

IMPLEMENT_DYNCREATE(CDVStorageDoc, CDocument)

BEGIN_MESSAGE_MAP(CDVStorageDoc, CDocument)
	//{{AFX_MSG_MAP(CDVStorageDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDVStorageDoc construction/destruction

CDVStorageDoc::CDVStorageDoc()
{
	// add one-time construction code here

}

CDVStorageDoc::~CDVStorageDoc()
{
}

BOOL CDVStorageDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CDVStorageDoc serialization

void CDVStorageDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// add storing code here
	}
	else
	{
		// add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDVStorageDoc diagnostics

#ifdef _DEBUG
void CDVStorageDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDVStorageDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDVStorageDoc commands
