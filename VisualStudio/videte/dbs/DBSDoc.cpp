// DBSDoc.cpp : implementation of the CDBSDoc class
//

#include "stdafx.h"
#include "DBS.h"

#include "DBSDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDBSDoc

IMPLEMENT_DYNCREATE(CDBSDoc, CDocument)

BEGIN_MESSAGE_MAP(CDBSDoc, CDocument)
	//{{AFX_MSG_MAP(CDBSDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDBSDoc construction/destruction

CDBSDoc::CDBSDoc()
{
	// TODO: add one-time construction code here

}

CDBSDoc::~CDBSDoc()
{
}

BOOL CDBSDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CDBSDoc serialization

void CDBSDoc::Serialize(CArchive& ar)
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
// CDBSDoc diagnostics

#ifdef _DEBUG
void CDBSDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDBSDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDBSDoc commands
CDBSView* CDBSDoc::GetView()
{
	POSITION pos = GetFirstViewPosition();
	if (pos) {
		CDBSView* pView = (CDBSView*)GetNextView(pos);
		if (pView) {
			return pView;
		}
	}
	return NULL;
}
