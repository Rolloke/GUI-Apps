// DataView.cpp : implementation file
//

#include "stdafx.h"
#include "WkWizard.h"
#include "DataView.h"

#include "Scanner.h"
#include "TranslateString.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDataView

IMPLEMENT_DYNCREATE(CDataView, CListView)

CDataView::CDataView()
{
}

CDataView::~CDataView()
{
}


BEGIN_MESSAGE_MAP(CDataView, CListView)
	//{{AFX_MSG_MAP(CDataView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDataView drawing

void CDataView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CDataView diagnostics

#ifdef _DEBUG
void CDataView::AssertValid() const
{
	CListView::AssertValid();
}

void CDataView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDataView message handlers
void CDataView::OnInitialUpdate() 
{
	CListView::OnInitialUpdate();

	// do initialzing code here
}
/////////////////////////////////////////////////////////////////////////////
void CDataView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	switch (lHint)
	{
	case 1:
		// new Project file
		DeleteItemsAndColumns();
		break;
	case 2:
		// pHint contains a CTokenArray
		OnTokenArray((CTokenArray*)pHint);
		break;
	case 3:
		// pHint contains a CTSArray
		OnTSArray((CTSArray*)pHint);
		break;
	default:
		break;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDataView::DeleteItemsAndColumns()
{
	int i;
	GetListCtrl().DeleteAllItems();
	for (i=10;i>=0;i--)
	{
		GetListCtrl().DeleteColumn(i);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDataView::OnTokenArray(CTokenArray* pTokenArray)
{
	// insert the columns
	const int		iColumns = 3;
	CRect			rect;
	LV_COLUMN		lvcolumn;
	CString s[iColumns];
	int i;
	int w[] = {3,1,6};

	DeleteItemsAndColumns();

	GetListCtrl().GetClientRect(rect);
	s[0].LoadString(IDS_SYMBOL);
	s[1].LoadString(IDS_LINE);
	s[2].LoadString(IDS_VALUE);

	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;

	for (i = 0 ; i < iColumns ; i++)  // add the columns to the list control
	{
		lvcolumn.cx = (rect.Width() * w[i])/10;
		lvcolumn.pszText = s[i].GetBuffer(0);
		lvcolumn.iSubItem = i;
		GetListCtrl().InsertColumn(i, &lvcolumn);  // assumes return value is OK.
		s[i].ReleaseBuffer();
	}


	for (i=0;i<pTokenArray->GetSize();i++)
	{
		InsertToken(pTokenArray->GetAt(i));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDataView::InsertToken(CToken* pToken)
{
	// Token setzen
	LV_ITEM lvis;
	CString s;
	int iIndex;
	int iCount;

	// Am Ende einfuegen, daher ListControl-Count holen
	iCount = GetListCtrl().GetItemCount();

	s = pToken->GetSymbolName();

	// Item einfuegen
	lvis.mask = LVIF_TEXT;
	lvis.pszText = s.GetBuffer(0);
	lvis.cchTextMax = s.GetLength();
	lvis.iItem = iCount;
	lvis.iSubItem = 0;
	iIndex = GetListCtrl().InsertItem(&lvis);
	s.ReleaseBuffer();
	
	// line
	s.Format("%d",pToken->GetLine());
	lvis.mask = LVIF_TEXT;
	lvis.pszText = s.GetBuffer(0);
	lvis.cchTextMax = s.GetLength();
	lvis.iItem = iIndex;
	lvis.iSubItem = 1;
	GetListCtrl().SetItem(&lvis);
	s.ReleaseBuffer();

	// value
	s = pToken->GetOriginal();
	lvis.mask = LVIF_TEXT;
	lvis.pszText = s.GetBuffer(0);
	lvis.cchTextMax = s.GetLength();
	lvis.iItem = iIndex;
	lvis.iSubItem = 2;
	GetListCtrl().SetItem(&lvis);
	s.ReleaseBuffer();
}
/////////////////////////////////////////////////////////////////////////////
void CDataView::OnTSArray(CTSArray* pTSArray)
{
	// insert the columns
	const int		iColumns = 2;
	CRect			rect;
	LV_COLUMN		lvcolumn;
	CString s[iColumns];
	int i;
	int w[] = {3,7};

	DeleteItemsAndColumns();

	GetListCtrl().GetClientRect(rect);
	s[0].LoadString(IDS_ID);
	s[1].LoadString(IDS_STRING_VALUE);

	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;

	for (i = 0 ; i < iColumns ; i++)  // add the columns to the list control
	{
		lvcolumn.cx = (rect.Width() * w[i])/10;
		lvcolumn.pszText = s[i].GetBuffer(0);
		lvcolumn.iSubItem = i;
		GetListCtrl().InsertColumn(i, &lvcolumn);  // assumes return value is OK.
		s[i].ReleaseBuffer();
	}

	for (i=0;i<pTSArray->GetSize();i++)
	{
		InsertTranslateString(pTSArray->GetAt(i));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDataView::InsertTranslateString(CTranslateString* pTranslateString)
{
	// Token setzen
	LV_ITEM lvis;
	CString s;
	int iIndex;
	int iCount;

	// Am Ende einfuegen, daher ListControl-Count holen
	iCount = GetListCtrl().GetItemCount();

	s = pTranslateString->GetID();

	// Item einfuegen
	lvis.mask = LVIF_TEXT;
	lvis.pszText = s.GetBuffer(0);
	lvis.cchTextMax = s.GetLength();
	lvis.iItem = iCount;
	lvis.iSubItem = 0;
	iIndex = GetListCtrl().InsertItem(&lvis);
	s.ReleaseBuffer();
	
	// line
	s = pTranslateString->GetText();
	lvis.mask = LVIF_TEXT;
	lvis.pszText = s.GetBuffer(0);
	lvis.cchTextMax = s.GetLength();
	lvis.iItem = iIndex;
	lvis.iSubItem = 1;
	GetListCtrl().SetItem(&lvis);
	s.ReleaseBuffer();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDataView::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style = WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT;
	
	return CListView::PreCreateWindow(cs);
}
