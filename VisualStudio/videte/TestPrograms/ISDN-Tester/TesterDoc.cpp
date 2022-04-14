// TesterDoc.cpp : implementation of the CTesterDoc class
//

#include "stdafx.h"
#include "Tester.h"

#include "TesterDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTesterDoc

IMPLEMENT_DYNCREATE(CTesterDoc, CDocument)

BEGIN_MESSAGE_MAP(CTesterDoc, CDocument)
	//{{AFX_MSG_MAP(CTesterDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTesterDoc construction/destruction

CTesterDoc::CTesterDoc()
{
	// TODO: add one-time construction code here

}

CTesterDoc::~CTesterDoc()
{
}

BOOL CTesterDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CTesterDoc serialization

void CTesterDoc::Serialize(CArchive& ar)
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
// CTesterDoc diagnostics

#ifdef _DEBUG
void CTesterDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTesterDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTesterDoc commands
