// ProjectView.cpp : implementation of the CProjectView class
//

#include "stdafx.h"
#include "WkWizard.h"

#include "WkWizardDoc.h"
#include "ProjectView.h"

#include "InsertDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProjectView

IMPLEMENT_DYNCREATE(CProjectView, CTreeView)

BEGIN_MESSAGE_MAP(CProjectView, CTreeView)
	//{{AFX_MSG_MAP(CProjectView)
	ON_COMMAND(ID_EDIT_EXTRACT_RESOURCE, OnEditExtractResource)
	ON_COMMAND(ID_EDIT_SCAN, OnEditScan)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_COMMAND(ID_CONTEXT_MENU, OnContextMenu)
	ON_COMMAND(ID_EDIT_INSERT, OnEditInsert)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CTreeView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CTreeView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CTreeView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProjectView construction/destruction

CProjectView::CProjectView()
{
	// TODO: add construction code here

}

CProjectView::~CProjectView()
{
}

BOOL CProjectView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style = WS_CHILD | WS_VISIBLE | WS_BORDER | 
				TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT;
	return CTreeView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CProjectView drawing

void CProjectView::OnDraw(CDC* pDC)
{
	CWkWizardDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}

void CProjectView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();

	// TODO: You may populate your TreeView with items by directly accessing
	//  its tree control through a call to GetTreeCtrl().
}

/////////////////////////////////////////////////////////////////////////////
// CProjectView printing

BOOL CProjectView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CProjectView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CProjectView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CProjectView diagnostics

#ifdef _DEBUG
void CProjectView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CProjectView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

CWkWizardDoc* CProjectView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWkWizardDoc)));
	return (CWkWizardDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CProjectView message handlers

void CProjectView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	CWkWizardDoc* pDoc = GetDocument();
	int i,c;
	CString name;
	TV_INSERTSTRUCT tvis;
	CRect rect;
	HTREEITEM hCpp;
	HTREEITEM hRc;
	HTREEITEM hDep;

	switch (lHint)
	{
	case 1:
		GetTreeCtrl().DeleteAllItems();

		tvis.hParent = TVI_ROOT;
		tvis.hInsertAfter = TVI_SORT;
		tvis.item.mask = TVIF_TEXT;

		name = "CXX-Files";
		tvis.item.cchTextMax = name.GetLength();
		tvis.item.pszText = name.GetBuffer(0);
		hCpp = GetTreeCtrl().InsertItem(&tvis);
		name.ReleaseBuffer();

		name = "RC-Files";
		tvis.item.cchTextMax = name.GetLength();
		tvis.item.pszText = name.GetBuffer(0);
		hRc = GetTreeCtrl().InsertItem(&tvis);
		name.ReleaseBuffer();

		name = "Dependencies";
		tvis.item.cchTextMax = name.GetLength();
		tvis.item.pszText = name.GetBuffer(0);
		hDep = GetTreeCtrl().InsertItem(&tvis);
		name.ReleaseBuffer();

		c = pDoc->m_saCPPFiles.GetSize();
		for (i=0;i<c;i++)
		{
			name = pDoc->m_saCPPFiles[i];
			tvis.hParent = hCpp;
			tvis.item.cchTextMax = name.GetLength();
			tvis.item.pszText = name.GetBuffer(0);
			GetTreeCtrl().InsertItem(&tvis);
			name.ReleaseBuffer();
		}
		c = pDoc->m_saRCFiles.GetSize();
		for (i=0;i<c;i++)
		{
			name = pDoc->m_saRCFiles[i];
			tvis.hParent = hRc;
			tvis.item.cchTextMax = name.GetLength();
			tvis.item.pszText = name.GetBuffer(0);
			GetTreeCtrl().InsertItem(&tvis);
			name.ReleaseBuffer();
		}

		c = pDoc->m_saDependenciesFiles.GetSize();
		for (i=0;i<c;i++)
		{
			name = pDoc->m_saDependenciesFiles[i];
			tvis.hParent = hDep;
			tvis.item.cchTextMax = name.GetLength();
			tvis.item.pszText = name.GetBuffer(0);
			GetTreeCtrl().InsertItem(&tvis);
			name.ReleaseBuffer();
		}
		GetClientRect(rect);
		InvalidateRect(rect);

		break;
	case 2:
		// used by CDataView
		break;
	default:
		break;
	}
	
	
}
/////////////////////////////////////////////////////////////////////////////////////
void CProjectView::OnEditExtractResource() 
{
	CWkWizardDoc* pDoc = GetDocument();
	HTREEITEM hRC = GetTreeCtrl().GetSelectedItem();
	CString name;

	if (hRC)
	{
		name = GetTreeCtrl().GetItemText(hRC);
		if (-1!=name.Find(".rc"))
		{
			pDoc->ExtractResource(name);
		}
		else
		{
			CString mes;
			mes.LoadString(IDP_NO_RC);
			mes = name + mes;
			AfxMessageBox(mes);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////
void CProjectView::OnEditScan() 
{
	CWkWizardDoc* pDoc = GetDocument();
	HTREEITEM hT = GetTreeCtrl().GetSelectedItem();
	CString name;

	if (hT)
	{
		name = GetTreeCtrl().GetItemText(hT);
		pDoc->ScanFile(name);
	}
}
/////////////////////////////////////////////////////////////////////////////////////
void CProjectView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint pt;
	GetCursorPos(&pt);
	HTREEITEM hItem;
	UINT nFlags;

	ScreenToClient(&pt);
	hItem = GetTreeCtrl().HitTest(pt,&nFlags);
	if (hItem && ((nFlags & TVHT_ONITEMICON) || (nFlags & TVHT_ONITEMLABEL)))
	{
		GetTreeCtrl().SelectItem(hItem);
		ContextMenu(&pt);
	}

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CProjectView::OnContextMenu() 
{
	CPoint pt;
	CRect  rect;

	HTREEITEM hItem;
	hItem = GetTreeCtrl().GetSelectedItem();

	if (hItem)
	{
		GetTreeCtrl().GetItemRect(hItem,&rect,TRUE);
		pt.x = (rect.left + rect.right) /2;
		pt.y = (rect.top + rect.bottom) /2;
		ContextMenu(&pt);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CProjectView::ContextMenu(LPPOINT lpPoint) 
{
	CMenu menu;
	CMenu* pm;
	HTREEITEM hItem;
	CString name;
	UINT i=2;

	hItem = GetTreeCtrl().GetSelectedItem();

	if (hItem)
	{
		name = GetTreeCtrl().GetItemText(hItem);
		if (-1!=name.Find(".rc"))
		{
			i = 0;
		}
		else if (-1!=name.Find(".c"))
		{
			i = 1;
		}
		else
		{
			i = 2;
		}
		menu.LoadMenu(IDR_CONTEXT);
		pm = menu.GetSubMenu(i);
		ClientToScreen(lpPoint);
		pm->TrackPopupMenu(TPM_LEFTALIGN,lpPoint->x,lpPoint->y,AfxGetMainWnd());		
	}
}

void CProjectView::OnEditInsert() 
{
	CWkWizardDoc* pDoc = GetDocument();
	HTREEITEM hRC = GetTreeCtrl().GetSelectedItem();
	CString name;

	if (hRC)
	{
		int p;
		name = GetTreeCtrl().GetItemText(hRC);
		p = name.Find(".rc");
		if (-1!=p)
		{
			CInsertDialog dlg(this);

			dlg.m_Orig = name.Left(p);
			dlg.m_Dir = pDoc->GetDirectory();
			if (IDOK==dlg.DoModal())
			{
				pDoc->InsertResource(name,dlg.m_Dest,dlg.m_Lang);
			}
		}
		else
		{
			CString mes;
			mes.LoadString(IDP_NO_RC);
			mes = name + mes;
			AfxMessageBox(mes);
		}
	}
}
