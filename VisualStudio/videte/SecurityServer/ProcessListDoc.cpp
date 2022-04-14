// ProcessListDoc.cpp : implementation file
//

#include "stdafx.h"
#include "sec3.h"
#include "ProcessListDoc.h"
#include "ProcessListView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProcessListDoc

IMPLEMENT_DYNCREATE(CProcessListDoc, CDocument)

CProcessListDoc::CProcessListDoc()
{
}

BOOL CProcessListDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument()) {
		return FALSE;
	}
	CRect rect;
	GetView()->GetClientRect(rect);
	GetView()->GetListCtrl().SetColumnWidth(0,rect.Width());
	return TRUE;
}

CProcessListDoc::~CProcessListDoc()
{
}


BEGIN_MESSAGE_MAP(CProcessListDoc, CDocument)
	//{{AFX_MSG_MAP(CProcessListDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcessListDoc diagnostics

#ifdef _DEBUG
void CProcessListDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CProcessListDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CProcessListDoc serialization

void CProcessListDoc::Serialize(CArchive& ar)
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
// CProcessListDoc commands


CProcessListView* CProcessListDoc::GetView()
{
	POSITION pos = GetFirstViewPosition();
	if (pos) {
		CProcessListView* pView = (CProcessListView*)GetNextView(pos);
		if (pView) {
			return pView;
		}
	}
	return NULL;
}	

