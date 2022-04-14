// HtmlViewDoc.cpp : implementation of the CHtmlViewDoc class
//

#include "stdafx.h"
//#include "HtmlView.h"
#include "LogView.h"

#include "HtmlViewDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHtmlViewDoc

IMPLEMENT_DYNCREATE(CHtmlViewDoc, CDocument)

BEGIN_MESSAGE_MAP(CHtmlViewDoc, CDocument)
	//{{AFX_MSG_MAP(CHtmlViewDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHtmlViewDoc construction/destruction

CHtmlViewDoc::CHtmlViewDoc()
{

}

CHtmlViewDoc::~CHtmlViewDoc()
{
}

BOOL CHtmlViewDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;


	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CHtmlViewDoc serialization

void CHtmlViewDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	}
}

/////////////////////////////////////////////////////////////////////////////
// CHtmlViewDoc diagnostics

#ifdef _DEBUG
void CHtmlViewDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CHtmlViewDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHtmlViewDoc commands
