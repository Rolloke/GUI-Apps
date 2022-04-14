// ReportView.cpp : implementation file
//

#include "stdafx.h"
#include "UpdateHandler.h"
#include "mainfrm.h"
#include "ReportView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReportView

IMPLEMENT_DYNCREATE(CReportView, CListView)

CReportView::CReportView()
{
}

CReportView::~CReportView()
{
}


BEGIN_MESSAGE_MAP(CReportView, CListView)
	//{{AFX_MSG_MAP(CReportView)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReportView drawing

void CReportView::OnDraw(CDC* pDC)
{
//	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CReportView diagnostics

#ifdef _DEBUG
void CReportView::AssertValid() const
{
	CListView::AssertValid();
}

void CReportView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CReportView message handlers
void CReportView::OnSize(UINT nType, int cx, int cy) 
{
	CListView::OnSize(nType, cx, cy);
	GetListCtrl().SetColumnWidth(0, cx);  // assumes return value is OK.
	CMainFrame* pWnd = theApp.GetMainFrame();
	if (pWnd)
	{
		pWnd->ActualizeTitleBarPaneSizes(this);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CReportView::AddLine(const CString& line)
{
	LV_ITEM lvis;
	CString sLine = line;
	int iIndex;
	int iCount;

	// always insert at the end, we need the count of existing items
	iCount = GetListCtrl().GetItemCount();
	if (iCount>200)
	{
		GetListCtrl().DeleteItem(0);
	}
	iCount = GetListCtrl().GetItemCount();

	// insert new item with name of the file
	lvis.mask = LVIF_TEXT | LVIF_IMAGE;
	lvis.pszText = sLine.GetBuffer(0);
	lvis.cchTextMax = sLine.GetLength();
	lvis.lParam = 0L;
	lvis.iItem = iCount;
	lvis.iSubItem = 0;
	lvis.iImage=1;
	iIndex = GetListCtrl().InsertItem(&lvis);
	sLine.ReleaseBuffer();
	GetListCtrl().EnsureVisible(iIndex,TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CReportView::Clear()
{
	GetListCtrl().DeleteAllItems();
}

BOOL CReportView::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style = LVS_REPORT | LVS_SINGLESEL | LVS_NOLABELWRAP | LVS_ALIGNLEFT | LVS_NOCOLUMNHEADER |
			   WS_BORDER | WS_TABSTOP | WS_CHILD | WS_VISIBLE;
	return CListView::PreCreateWindow(cs);
}

void CReportView::OnInitialUpdate() 
{
	CListView::OnInitialUpdate();
	
	CRect			rect;
	LV_COLUMN		lvcolumn;
	CString s;

	// initialize columns of ListCtrl
	GetClientRect(rect);

	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;

	lvcolumn.cx = rect.Width();
	lvcolumn.pszText = s.GetBuffer(0);
	lvcolumn.iSubItem = 0;
	GetListCtrl().InsertColumn(0, &lvcolumn);  // assumes return value is OK.
	s.ReleaseBuffer();
}
