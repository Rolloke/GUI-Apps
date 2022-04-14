// DirectoryView.cpp : implementation of the CDirectoryView class
//

#include "stdafx.h"
#include "UpdateHandler.h"
#include "mainfrm.h"

#include "UpdateDoc.h"
#include "DirectoryView.h"
#include "DirContent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CUpdateHandler theApp;

/////////////////////////////////////////////////////////////////////////////
// CDirectoryView

IMPLEMENT_DYNCREATE(CDirectoryView, CTreeView)

BEGIN_MESSAGE_MAP(CDirectoryView, CTreeView)
	//{{AFX_MSG_MAP(CDirectoryView)
	ON_WM_SIZE()
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_WM_DROPFILES()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	// Standard printing commands
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirectoryView construction/destruction

CDirectoryView::CDirectoryView()
{
	// TODO: add construction code here

}

CDirectoryView::~CDirectoryView()
{
}

BOOL CDirectoryView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style = WS_CHILD | WS_VISIBLE | WS_BORDER | 
			   TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT;
	return CTreeView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CDirectoryView drawing

void CDirectoryView::OnDraw(CDC* pDC)
{
	CUpdateDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CDirectoryView printing

BOOL CDirectoryView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CDirectoryView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CDirectoryView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CDirectoryView diagnostics

#ifdef _DEBUG
void CDirectoryView::AssertValid() const
{
	CView::AssertValid();
}

void CDirectoryView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CUpdateDoc* CDirectoryView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CUpdateDoc)));
	return (CUpdateDoc*)m_pDocument;
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
void CDirectoryView::OnInitialUpdate() 
{
	GetTreeCtrl().SetImageList(theApp.GetSymbols(),LVSIL_NORMAL);
	DragAcceptFiles();
}
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CDirectoryView::AddDirectory(const CString& sDir, int ico)
{
	HTREEITEM hParent = TVI_ROOT;
	HTREEITEM hInsertAfter = TVI_LAST;
	TV_INSERTSTRUCT tvis;
	int p;
	BOOL bFound = FALSE;
	CString sOne,sText;
	CString sPath = sDir;

	if (sPath.Find(REG_PATH_ID) == -1)
	{
		sPath.MakeLower();
	}
	// make sure directory path ends with '\'
	if (sPath.GetAt(sPath.GetLength()-1) != '\\')
	{
		sPath += '\\';
	}

    tvis.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	p = sDir.Find('\\');
	hParent = TVI_ROOT;
	hInsertAfter = GetTreeCtrl().GetRootItem();
	while (p!=-1)
	{
		sOne = sPath.Left(p);
		sPath = sPath.Mid(p+1);

		if (sOne.IsEmpty())
		{
			break;
		}
		//
		if (hInsertAfter==NULL)
		{
			tvis.hParent = hParent;
			tvis.hInsertAfter = TVI_LAST;
			tvis.item.lParam = NULL;
			tvis.item.cchTextMax = sOne.GetLength();
			tvis.item.pszText = sOne.GetBuffer(0);
			tvis.item.iImage = ico;
			tvis.item.iSelectedImage = tvis.item.iImage;
			hParent = GetTreeCtrl().InsertItem(&tvis);
			hInsertAfter = GetTreeCtrl().GetNextSiblingItem(hParent);
			sOne.ReleaseBuffer();
		}
		else
		{
			// browse
			bFound = FALSE;
			while (hInsertAfter)
			{
				sText = GetTreeCtrl().GetItemText(hInsertAfter);
				if (sOne == sText)
				{
					// found
					hParent = hInsertAfter;
					hInsertAfter = GetTreeCtrl().GetChildItem(hParent); 
					bFound = TRUE;
					break;
				}
				hInsertAfter = GetTreeCtrl().GetNextSiblingItem(hInsertAfter);
			}
			if (bFound==FALSE)
			{
					// not found insert it
				tvis.hParent = hParent;
				tvis.hInsertAfter = TVI_LAST;
				tvis.item.lParam = NULL;
				tvis.item.cchTextMax = sOne.GetLength();
				tvis.item.pszText = sOne.GetBuffer(0);
				tvis.item.iImage = ico;
				tvis.item.iSelectedImage = tvis.item.iImage;
				hParent = GetTreeCtrl().InsertItem(&tvis);
				hInsertAfter = GetTreeCtrl().GetNextSiblingItem(hParent);
				sOne.ReleaseBuffer();
			}
		}

		p = sPath.Find('\\');
	}
	return hParent;
}
/////////////////////////////////////////////////////////////////////////////
void CDirectoryView::DeleteDirectory(const CString& sDir)
{
}
/////////////////////////////////////////////////////////////////////////////
void CDirectoryView::Clear()
{
	GetTreeCtrl().DeleteAllItems();
}
/////////////////////////////////////////////////////////////////////////////
void CDirectoryView::InsertDirInfo(CDirContent* pDirContent)
{
	LPWIN32_FIND_DATA lpWFD;
	int i;
	CString sDir,sSubDir;

	if (pDirContent==NULL)
	{
		return;
	}
	if (pDirContent->IsEmpty())
	{
		return;
	}
	
	sDir = pDirContent->GetDirectory();
	AddDirectory(sDir,0);
	if (sDir[sDir.GetLength()-1]=='\\')
	{
		sDir = sDir.Left(sDir.GetLength()-1);
	}

	for (i=0;i<pDirContent->GetSize();i++)
	{
		lpWFD = pDirContent->GetAt(i);
		if (lpWFD)
		{
			if (lpWFD->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				sSubDir = sDir + '\\' + lpWFD->cFileName;
				AddDirectory(sSubDir,0);
			}
		}
	}
	CRect rect;
	GetClientRect(rect);
	InvalidateRect(rect);

	CString sTemp = sDir + '\\';
	HTREEITEM hI = GetTreeItem(sTemp);

	if (hI)
	{
		 GetTreeCtrl().Expand(hI,TVE_EXPAND);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDirectoryView::OnSize(UINT nType, int cx, int cy) 
{
	CTreeView::OnSize(nType, cx, cy);
	
	CMainFrame* pWnd = theApp.GetMainFrame();

	if (pWnd)
	{
		pWnd->ActualizeTitleBarPaneSizes(this);
	}
}
/////////////////////////////////////////////////////////////////////////////
CString	CDirectoryView::GetDir(HTREEITEM hItem)
{
	CString sDir;
	HTREEITEM hI = hItem;

	while (hI)
	{
		sDir = GetTreeCtrl().GetItemText(hI) + '\\' + sDir;
		hI = GetTreeCtrl().GetParentItem(hI);
	}
	return sDir;
}
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CDirectoryView::GetTreeItem(const CString& sDir)
{
	HTREEITEM hI;
	CString sText;
	CString sHIDir;

	hI = GetTreeCtrl().GetRootItem();

	while (hI)
	{
		sText = GetTreeCtrl().GetItemText(hI);
		sHIDir = GetDir(hI);

		if (0==sDir.CompareNoCase(sHIDir))
		{
			return hI;
		}
		if (GetTreeCtrl().ItemHasChildren(hI))
		{
			hI = GetTreeCtrl().GetChildItem(hI);
		}
		else
		{
			hI = GetTreeCtrl().GetNextSiblingItem(hI);
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CDirectoryView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint pt;
	GetCursorPos(&pt);
	HTREEITEM hItem;
	UINT nFlags;

	ScreenToClient(&pt);
	hItem = GetTreeCtrl().HitTest(pt,&nFlags);
	if (hItem && ((nFlags & TVHT_ONITEMICON) || (nFlags & TVHT_ONITEMLABEL)))
	{
		CString sDir;
		GetTreeCtrl().SelectItem(hItem);
		sDir = GetDir(hItem);
		theApp.GetDirectory(sDir);
	}
	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CDirectoryView::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint pt;
	GetCursorPos(&pt);
	HTREEITEM hItem;
	UINT nFlags;

	ScreenToClient(&pt);
	hItem = GetTreeCtrl().HitTest(pt,&nFlags);
	if (hItem && ((nFlags & TVHT_ONITEMICON) || (nFlags & TVHT_ONITEMLABEL)))
	{
		CString sDir;
		sDir = GetDir(hItem);
		((CMainFrame*)(GetParent()->GetParent()))->SetFileViewDirectory(sDir);
	}
	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CDirectoryView::OnDropFiles(HDROP hDropInfo) 
{
	TCHAR szBuffer[_MAX_PATH];
	POINT pt;
	UINT i,c;
					  
	DragQueryPoint(hDropInfo,&pt);

	HTREEITEM hItem;
	hItem = GetTreeCtrl().GetDropHilightItem();
	if (hItem)
	{
		CString sDir;
		CString sFile;
		sDir = GetDir(hItem);
		if (sDir.Find(REG_PATH_ID) != -1)
		{
			return;
		}
		WK_TRACE(_T("on item %s\n"), sDir);

		WK_TRACE(_T("dropped at %d,%d\n"),pt.x,pt.y);
		c = DragQueryFile(hDropInfo, 0xFFFFFFFF, szBuffer, sizeof(szBuffer));
		for (i=0;i<c;i++)
		{
			DragQueryFile(hDropInfo, i, szBuffer, sizeof(szBuffer));
			WK_TRACE(_T("dropped %s\n"), szBuffer);
			sFile = szBuffer;
			theApp.SendFile(sFile, sDir);
		}
		GetTreeCtrl().SelectDropTarget(NULL);
	}


	CTreeView::OnDropFiles(hDropInfo);
}
/////////////////////////////////////////////////////////////////////////////
int CDirectoryView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_oleDropTarget.Register(this);
	
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
DROPEFFECT CDirectoryView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	HTREEITEM hItem;
	UINT nFlags;
	hItem = GetTreeCtrl().HitTest(point,&nFlags);
	if (hItem && ((nFlags & TVHT_ONITEMICON) || (nFlags & TVHT_ONITEMLABEL)))
	{
		GetTreeCtrl().SelectDropTarget(hItem);
		return DROPEFFECT_COPY;
	}
	else
	{
		GetTreeCtrl().SelectDropTarget(NULL);
		return DROPEFFECT_NONE;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDirectoryView::OnDragLeave() 
{
	GetTreeCtrl().SelectDropTarget(NULL);
}
