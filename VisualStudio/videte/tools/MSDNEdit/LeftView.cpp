// LeftView.cpp : implementation of the CLeftView class
//

#include "stdafx.h"
#include "MSDNEdit.h"

#include "MSDNEditDoc.h"
#include "MSDNEditView.h"
#include "LeftView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define FOLDER       0
#define SUB_FOLDER   1
#define FOLDER_DATA  2

/////////////////////////////////////////////////////////////////////////////
// CLeftView

IMPLEMENT_DYNCREATE(CLeftView, CTreeView)

BEGIN_MESSAGE_MAP(CLeftView, CTreeView)
	//{{AFX_MSG_MAP(CLeftView)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_COMMAND(ID_EDIT_NEW_FOLDER, OnEditNewFolder)
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnBeginlabeledit)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndlabeledit)
	ON_COMMAND(ID_VIEW_EXPAND_TREE, OnViewExpandTree)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CTreeView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CTreeView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CTreeView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLeftView construction/destruction

CLeftView::CLeftView()
{
	m_hSelected = NULL;
   m_bExport   = false;
}

CLeftView::~CLeftView()
{
}

BOOL CLeftView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CTreeView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CLeftView drawing

void CLeftView::OnDraw(CDC* pDC)
{
	CMSDNEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
   if (pDC->IsPrinting())
   {
      int nMM = pDC->SetMapMode(MM_ISOTROPIC);
      pDC->SetWindowExt(100, 100);
      pDC->SetViewportExt(700, 700);
      SendMessage(WM_PRINT, (WPARAM)pDC->m_hDC, PRF_CLIENT);
   }
}


/////////////////////////////////////////////////////////////////////////////
// CLeftView printing

BOOL CLeftView::OnPreparePrinting(CPrintInfo* pInfo)
{
	return DoPreparePrinting(pInfo);
}

void CLeftView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{

}

void CLeftView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{

}

void CLeftView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();

	// TODO: You may populate your TreeView with items by directly accessing
	//  its tree control through a call to GetTreeCtrl().
}

/////////////////////////////////////////////////////////////////////////////
// CLeftView diagnostics

#ifdef _DEBUG
void CLeftView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CLeftView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

CMSDNEditDoc* CLeftView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMSDNEditDoc)));
	return (CMSDNEditDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLeftView message handlers

void CLeftView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
   CTreeCtrl&  theCtrl = GetTreeCtrl();
   CMSDNEditDoc *pDoc = GetDocument();
	switch (lHint)
   {
      case HTMLHELPCOLLECTION_INSERT:
      {
         CString str;
         MSDNFolders *pMsdnF = (MSDNFolders*)pHint;
         int nImage = (pMsdnF->nLangID) ? SUB_FOLDER : FOLDER;
         pMsdnF->hReturned   = theCtrl.InsertItem(pMsdnF->strTitle, nImage, nImage, pMsdnF->hParent);
         if (pMsdnF->nOrder != -1)
         {
            str.Format("%s%d>",pDoc->m_strFolderOrder, pMsdnF->nOrder);
            theCtrl.InsertItem(str, FOLDER_DATA, FOLDER_DATA, pMsdnF->hReturned);
         }
         if (pMsdnF->nLangID)
         {
            str.Format("%s%d>",pDoc->m_strLangID, pMsdnF->nLangID);
            theCtrl.InsertItem(str, FOLDER_DATA, FOLDER_DATA, pMsdnF->hReturned);
         }
         pMsdnF->strTitle.Empty();
         pMsdnF->nLangID = 0;
         pMsdnF->nOrder  = -1;
      }break;
      case HTMLHELPCOLLECTION_SAVE:
      {
         CArchive *pAr = (CArchive*)pHint;
         CString strSpace("   ");
         HTREEITEM hItem = theCtrl.GetRootItem();
         do 
         {
            SaveTree(hItem, *pAr, strSpace);
            hItem = theCtrl.GetNextItem(hItem, TVGN_NEXT);
         } while(hItem);
      }break;
      case HTMLHELPCOLLECTION_EXPORT:
      {
         CArchive *pAr = (CArchive*)pHint;
         CString strSpace("   ");
         m_bExport = true;
         SaveTree(GetTreeCtrl().GetSelectedItem(), *pAr, strSpace);
         m_bExport = false;
      }break;
      case HTMLHELPCOLLECTION_DELETE:
      {
         theCtrl.DeleteAllItems();
      }break;
      case HTMLHELPDOCINFO_SELCHANGE:
      {
         CString *pStr = (CString*) pHint;
         HTREEITEM hItem = theCtrl.GetRootItem();
         do 
         {
            FindItem(hItem, *pStr);
            hItem = theCtrl.GetNextItem(hItem, TVGN_NEXT);
         } while(hItem);
      }break;
   }
}

BOOL CLeftView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	BOOL bCreate = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
   CTreeCtrl&  theCtrl = GetTreeCtrl();
   theCtrl.ModifyStyle(0, TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES | TVS_EDITLABELS | TVS_SHOWSELALWAYS);
   m_Images.Attach((HIMAGELIST)ImageList_LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SDB_IMAGES),
      16, 1, ::GetSysColor(COLOR_WINDOW), IMAGE_BITMAP, LR_DEFAULTCOLOR|LR_LOADTRANSPARENT));

   theCtrl.SetImageList(&m_Images, TVSIL_NORMAL);
   
   return bCreate;
}

void CLeftView::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
   if (pNMTreeView->itemNew.mask & TVIF_HANDLE)
   {
      char szItem[256];
      pNMTreeView->itemNew.cchTextMax = 255;
      pNMTreeView->itemNew.pszText    = szItem;
      pNMTreeView->itemNew.mask |= TVIF_TEXT; 
      GetTreeCtrl().GetItem(&pNMTreeView->itemNew);
      GetDocument()->UpdateAllViews(this, HTMLHELPCOLLECTION_SELCHANGE, (CObject*)&szItem);
   }
	*pResult = 0;
}

HTREEITEM CLeftView::SaveTree(HTREEITEM hItem, CArchive &ar, CString &strSpace, int nOrder)
{
   if (hItem == NULL) return NULL;
   CMSDNEditDoc *pDoc = GetDocument();
   CTreeCtrl&  theCtrl = GetTreeCtrl();
   int  nImage, nSelected;
   CString str;
   theCtrl.GetItemImage(hItem, nImage, nSelected);
   switch (nImage)
   {
      case SUB_FOLDER:
      case FOLDER:
      {
         CString strItemText = theCtrl.GetItemText(hItem);
         ar.WriteString(strSpace+pDoc->m_strFolder+pDoc->m_strNewLine);
         strSpace += _T("   ");
         str.Format("%s\"%s\"/>%s", pDoc->m_strTitle, strItemText, pDoc->m_strNewLine);
         ar.WriteString(strSpace+str);
         SaveTree(theCtrl.GetNextItem(hItem, TVGN_CHILD), ar, strSpace);
         if (m_bExport && (nImage == SUB_FOLDER))
         {
            MSDNDocComp dc;
            char drive[_MAX_DRIVE];
            char dir[_MAX_DIR];
            char fname[_MAX_FNAME];
            char ext[_MAX_EXT];
            dc.strDocCompID = strItemText;
            pDoc->UpdateAllViews(this, HTMLHELPCOLLECTION_GETFILENAMES, (CObject*)&dc);

            _tsplitpath(dc.strTitleLocation, drive, dir, fname, ext);
            str.Format("%s\"%s%s\"/>%s", pDoc->m_strTitleLocation, fname, ext, pDoc->m_strNewLine);
            ar.WriteString(strSpace+str);

         	_tsplitpath(dc.strIndexLocation, drive, dir, fname, ext);
            str.Format("%s\"%s%s\"/>%s", pDoc->m_strIndexLocation, fname, ext, pDoc->m_strNewLine);
            ar.WriteString(strSpace+str);
         }
         strSpace = strSpace.Left(strSpace.GetLength()-3);
         ar.WriteString(strSpace+pDoc->m_strNFolder+pDoc->m_strNewLine);
      } break;
      case FOLDER_DATA:
         str = theCtrl.GetItemText(hItem);
         ar.WriteString(strSpace+str+pDoc->m_strNewLine);
         do 
         {
            hItem = theCtrl.GetNextItem(hItem, TVGN_NEXT);
            SaveTree(hItem, ar, strSpace);
         } while(hItem);
         break;
   }
   return hItem;
}

void CLeftView::OnUpdateEditDelete(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(GetTreeCtrl().GetSelectedItem() != NULL);
}

void CLeftView::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(m_hSelected != NULL);
}

void CLeftView::OnEditDelete()
{
   CTreeCtrl&  theCtrl = GetTreeCtrl();
   HTREEITEM hItem = theCtrl.GetSelectedItem();
   if (hItem)
   {
      if (hItem == m_hSelected) m_hSelected = NULL;
      theCtrl.DeleteItem(hItem);
      GetDocument()->SetModifiedFlagEx(DOC_TYPE_HTMLHELPCOLLECTION);
   }
}

void CLeftView::OnEditPaste() 
{
   CTreeCtrl&  theCtrl = GetTreeCtrl();
   HTREEITEM hItem = theCtrl.GetSelectedItem();
   if ((hItem && m_hSelected) && (hItem != m_hSelected))
   {
      bool bCut = (theCtrl.GetItemState(m_hSelected, TVIS_CUT) !=0) ? true : false;
      if (bCut) theCtrl.SetItemState(m_hSelected, 0, TVIS_CUT);
      CopyItem(m_hSelected, hItem);
      if (bCut) theCtrl.DeleteItem(m_hSelected);
      GetDocument()->SetModifiedFlagEx(DOC_TYPE_HTMLHELPCOLLECTION);
   }
}

void CLeftView::OnEditCopy(bool bCut) 
{
   CTreeCtrl&  theCtrl = GetTreeCtrl();
   HTREEITEM hItem = theCtrl.GetSelectedItem();
   
   if (bCut)
   {
      if (hItem)
      {
         m_hSelected = hItem;
         theCtrl.SetItemState(m_hSelected, TVIS_CUT, TVIS_CUT);
      }
   }
   else
   {
      if (m_hSelected) theCtrl.SetItemState(m_hSelected, 0, TVIS_CUT);
      if (m_hSelected != hItem)
      {
         m_hSelected = hItem;
         theCtrl.SetItemState(m_hSelected, 0, TVIS_CUT);
      }
   }
}

void CLeftView::CopyItem(HTREEITEM hFrom, HTREEITEM hTo)
{
   CTreeCtrl&  theCtrl = GetTreeCtrl();
   TV_INSERTSTRUCT tvis;
   char szText[256];
   int i;
	tvis.hParent         = hTo;
	tvis.hInsertAfter    = TVI_LAST;
	tvis.item.mask       = TVIF_HANDLE|TVIF_CHILDREN|TVIF_PARAM|TVIF_SELECTEDIMAGE|TVIF_IMAGE|TVIF_STATE|TVIF_TEXT;
   tvis.item.pszText    = szText;
   tvis.item.cchTextMax = 255;
   tvis.item.hItem      = hFrom;
   theCtrl.GetItem(&tvis.item);
   hTo = theCtrl.InsertItem(&tvis);
   for (i=0; hFrom != NULL; i++)
   {
      hFrom = theCtrl.GetNextItem(hFrom, (i==0) ? TVGN_CHILD : TVGN_NEXT);
      if (hFrom) CopyItem(hFrom, hTo);
   }
}


void CLeftView::OnEditNewFolder() 
{
   CTreeCtrl&  theCtrl = GetTreeCtrl();
   HTREEITEM hItem = theCtrl.GetSelectedItem();
   if (hItem == 0) hItem = TVI_ROOT;
	if (hItem)
   {
      TV_INSERTSTRUCT tvis;
	   tvis.hParent      = hItem;
	   tvis.hInsertAfter = TVI_LAST;
	   tvis.item.mask    = TVIF_SELECTEDIMAGE|TVIF_IMAGE;
      tvis.item.hItem   = hItem;
      if (hItem != TVI_ROOT) theCtrl.GetItem(&tvis.item);

      if ((tvis.item.iImage == 0) || (hItem == TVI_ROOT))
      {
         int i;
         tvis.item.hItem = hItem;
         for (tvis.item.cChildren=0, i=0; tvis.item.hItem != NULL; i++)
         {
            tvis.item.hItem = theCtrl.GetNextItem(tvis.item.hItem, (i==0) ? TVGN_CHILD : TVGN_NEXT);
            theCtrl.GetItem(&tvis.item);
            if(tvis.item.iImage == FOLDER) tvis.item.cChildren++;
         }
	      tvis.item.mask       = TVIF_SELECTEDIMAGE|TVIF_IMAGE|TVIF_TEXT;
         tvis.item.pszText    = "Neuer Ordner";
         tvis.item.cchTextMax = strlen(tvis.item.pszText);
         hItem = theCtrl.InsertItem(&tvis);
         if (hItem)
         {
      	   tvis.hParent = hItem;
            CString str;
            str.Format("%s%d\">", GetDocument()->m_strFolderOrder, tvis.item.cChildren);
            tvis.item.pszText    = (char*) LPCTSTR(str);
            tvis.item.cchTextMax = str.GetLength();
            tvis.item.iImage = tvis.item.iSelectedImage = FOLDER_DATA;
            theCtrl.InsertItem(&tvis);
            theCtrl.SelectItem(hItem);
            theCtrl.EditLabel(hItem);
         }
      }
   }
      
}

void CLeftView::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	
	*pResult = 0;
}

void CLeftView::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
   if (pTVDispInfo->item.mask & TVIF_TEXT)
   {
      CTreeCtrl&  theCtrl = GetTreeCtrl();
      theCtrl.SetItemText(pTVDispInfo->item.hItem, pTVDispInfo->item.pszText);
      GetDocument()->SetModifiedFlagEx(DOC_TYPE_HTMLHELPCOLLECTION);
   }
	*pResult = 0;
}

void CLeftView::OnViewExpandTree() 
{
   CTreeCtrl&  theCtrl = GetTreeCtrl();
   HTREEITEM hItem = theCtrl.GetSelectedItem();
	ExpandRec(hItem);
}

void CLeftView::ExpandRec(HTREEITEM hItem)
{
   if (hItem)
   {
      CTreeCtrl&  theCtrl = GetTreeCtrl();
      theCtrl.Expand(hItem, TVE_EXPAND);
      for (int i=0; hItem != NULL; i++)
      {
         hItem = theCtrl.GetNextItem(hItem, (i==0) ? TVGN_CHILD : TVGN_NEXT);
         ExpandRec(hItem);
      }
   }
}

int CLeftView::FindItem(HTREEITEM hItem, LPCTSTR str)
{
   if (hItem)
   {
      CTreeCtrl&  theCtrl = GetTreeCtrl();
      if (theCtrl.GetItemText(hItem).Find(str) != -1)
      {
         theCtrl.SelectItem(hItem);
         theCtrl.SetItemState(hItem, TVIS_SELECTED, TVIS_SELECTED);
         theCtrl.EnsureVisible(hItem);
         return 0;
      }
      for (int i=0; hItem != NULL; i++)
      {
         hItem = theCtrl.GetNextItem(hItem, (i==0) ? TVGN_CHILD : TVGN_NEXT);
         if (FindItem(hItem, str) == 0) return 0;
      }
   }
   return 1;
}

void CLeftView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
   CTreeCtrl&  theCtrl = GetTreeCtrl();
   theCtrl.SelectItem(NULL);
	
	*pResult = 0;
}
