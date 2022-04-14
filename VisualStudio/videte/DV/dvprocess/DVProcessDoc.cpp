// DVProcessDoc.cpp : implementation of the CDVProcessDoc class
//

#include "stdafx.h"
#include "DVProcess.h"

#include "DVProcessDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDVProcessDoc

IMPLEMENT_DYNCREATE(CDVProcessDoc, CDocument)

BEGIN_MESSAGE_MAP(CDVProcessDoc, CDocument)
	//{{AFX_MSG_MAP(CDVProcessDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDVProcessDoc construction/destruction

CDVProcessDoc::CDVProcessDoc()
{
}

CDVProcessDoc::~CDVProcessDoc()
{
}

BOOL CDVProcessDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;


	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CDVProcessDoc serialization

void CDVProcessDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDVProcessDoc diagnostics

#ifdef _DEBUG
void CDVProcessDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDVProcessDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDVProcessDoc commands
