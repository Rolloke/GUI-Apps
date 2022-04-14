// UnitTestDoc.cpp : implementation of the CUnitTestDoc class
//

#include "stdafx.h"
#include "UnitTest.h"

#include "UnitTestDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUnitTestDoc

IMPLEMENT_DYNCREATE(CUnitTestDoc, CDocument)

BEGIN_MESSAGE_MAP(CUnitTestDoc, CDocument)
	//{{AFX_MSG_MAP(CUnitTestDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUnitTestDoc construction/destruction

CUnitTestDoc::CUnitTestDoc()
{
}

CUnitTestDoc::~CUnitTestDoc()
{
}

BOOL CUnitTestDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CUnitTestDoc serialization

void CUnitTestDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	}
}

/////////////////////////////////////////////////////////////////////////////
// CUnitTestDoc diagnostics

#ifdef _DEBUG
void CUnitTestDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CUnitTestDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CUnitTestDoc commands
