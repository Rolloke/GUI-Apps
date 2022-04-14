// MSDNEditView.cpp : implementation of the CMSDNEditView class
//

#include "stdafx.h"
#include "MSDNEdit.h"

#include "MSDNEditDoc.h"
#include "MSDNEditView.h"
#include "EditColumnTexts.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMSDNEditView

IMPLEMENT_DYNCREATE(CMSDNEditView, CListView)

BEGIN_MESSAGE_MAP(CMSDNEditView, CListView)
	//{{AFX_MSG_MAP(CMSDNEditView)
	ON_WM_DESTROY()
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginlabeledit)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CListView::OnFilePrintPreview)
END_MESSAGE_MAP()

#define LIST_COLUMN_WIDTH "ListColumnWidth"
#define LIST_COLUMN_ORDER "ListColumnOrder"
/////////////////////////////////////////////////////////////////////////////
// CMSDNEditView construction/destruction
int CMSDNEditView::gm_nIDs[] = 
{
   IDS_LIST_HEAD_MODULE,                  // Stufe 1 DocCompilation
   IDS_LIST_HEAD_LANGID,
   IDS_LIST_HEAD_COL_NUM,                 // Stufe 2 LocationHistory
   IDS_LIST_HEAD_CHM_PATH,
   IDS_LIST_HEAD_CHI_PATH,
   IDS_LIST_HEAD_QUERY_LOC,
   IDS_LIST_HEAD_LOC_REF,
   IDS_LIST_HEAD_VERSION,
   IDS_LIST_HEAD_LAST_PROMPT_VERSION,
   IDS_LIST_HEAD_TITLE_SAMP_LOC,
   IDS_LIST_HEAD_TITLE_QUERY_LOC,
   IDS_LIST_HEAD_TITLE_SUPPORT_MERGE
};

CMSDNEditView::CMSDNEditView()
{
}

CMSDNEditView::~CMSDNEditView()
{
   CWinApp *pApp = AfxGetApp();
   pApp->WriteProfileBinary(REG_SETTINGS, LIST_COLUMN_ORDER, (BYTE*)m_nListColumnOrder , sizeof(m_nListColumnOrder));
   pApp->WriteProfileBinary(REG_SETTINGS, LIST_COLUMN_WIDTH, (BYTE*)m_nListColumnWidths, sizeof(m_nListColumnWidths));
}

BOOL CMSDNEditView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CListView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMSDNEditView drawing

void CMSDNEditView::OnDraw(CDC* pDC)
{
	CMSDNEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
   if (pDC->IsPrinting())
   {
      CRect rc;
      CSize size;

      int nMM = pDC->SetMapMode(MM_ISOTROPIC);
//		::GetWindowExtEx(pDC->m_hAttribDC, &size);
//      pDC->SetWindowExt(size);
//      GetClientRect(rc);
//      pDC->SetViewportExt(rc.Size());
//      pDC->SetWindowExt(rc.Size());
//      pDC->SetViewportExt(size);

      pDC->SetWindowExt(1, 1);
      pDC->SetViewportExt(6, 6);
      SendMessage(WM_PRINT, (WPARAM)pDC->m_hDC, PRF_CLIENT);
   }      
}

void CMSDNEditView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();


	// TODO: You may populate your ListView with items by directly accessing
	//  its list control through a call to GetListCtrl().
}

/////////////////////////////////////////////////////////////////////////////
// CMSDNEditView printing

BOOL CMSDNEditView::OnPreparePrinting(CPrintInfo* pInfo)
{
      
	return DoPreparePrinting(pInfo);
}

void CMSDNEditView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* pInfo)
{

}

void CMSDNEditView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* pInfo)
{

}

/////////////////////////////////////////////////////////////////////////////
// CMSDNEditView diagnostics

#ifdef _DEBUG
void CMSDNEditView::AssertValid() const
{
	CListView::AssertValid();
}

void CMSDNEditView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CMSDNEditDoc* CMSDNEditView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMSDNEditDoc)));
	return (CMSDNEditDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMSDNEditView message handlers
void CMSDNEditView::OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct)
{
	//TODO: add code to react to the user changing the view style of your window
}

BOOL CMSDNEditView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
   BOOL bReturn = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
   if (bReturn)
   {
      CListCtrl& theCtrl = GetListCtrl();
      CString str;
      int i=0;

      CWinApp *pApp = AfxGetApp();
      BYTE *pBValue = NULL;
      UINT  nSize   = 0;
      pApp->GetProfileBinary(REG_SETTINGS, LIST_COLUMN_ORDER, &pBValue, &nSize);
      if (pBValue)
      {
         if (nSize==sizeof(m_nListColumnOrder))
         {
            memcpy(m_nListColumnOrder, pBValue, nSize);
         }
         else nSize = -1;
         delete pBValue;
         pBValue = NULL;
      }
      else nSize = -1;

      if (nSize == -1)
      {
         m_nListColumnOrder[0]  =  0;
         m_nListColumnOrder[1]  =  1;
         m_nListColumnOrder[2]  =  2;
         m_nListColumnOrder[3]  =  3;
         m_nListColumnOrder[4]  =  4;
         m_nListColumnOrder[5]  =  5;
         m_nListColumnOrder[6]  =  6;
         m_nListColumnOrder[7]  =  7;
         m_nListColumnOrder[8]  =  8;
         m_nListColumnOrder[9]  =  9;
         m_nListColumnOrder[10] = 10;
         m_nListColumnOrder[11] = 11;
      }

      nSize = 0;
      pApp->GetProfileBinary(REG_SETTINGS, LIST_COLUMN_WIDTH, &pBValue, &nSize);
      if (pBValue)
      {
         if (nSize==sizeof(m_nListColumnWidths))
         {
            memcpy(m_nListColumnWidths, pBValue, nSize);
         }
         else nSize = -1;
         delete pBValue;
         pBValue = NULL;
      }
      else nSize = -1;
      if (nSize == -1)
      {
         m_nListColumnWidths[0]  = 80;
         m_nListColumnWidths[1]  = 50;
         m_nListColumnWidths[2]  = 60;
         m_nListColumnWidths[3]  = 250;
         m_nListColumnWidths[4]  = 250;
         m_nListColumnWidths[5]  = 200;
         m_nListColumnWidths[6]  = 150;
         m_nListColumnWidths[7]  = 100;
         m_nListColumnWidths[8]  = 60;
         m_nListColumnWidths[9]  = 100;
         m_nListColumnWidths[10] = 100;
         m_nListColumnWidths[11] = 50;
      }

      ModifyStyle(LVS_SINGLESEL|LVS_LIST|LVS_SORTASCENDING|LVS_SORTDESCENDING, LVS_REPORT|LVS_SHOWSELALWAYS|LVS_EDITLABELS);
      theCtrl.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_INFOTIP);
      for (i=0; i<LIST_COLUMNS; i++)
      {
         str.LoadString(gm_nIDs[i]);
         theCtrl.InsertColumn(i, str, LVCFMT_LEFT, m_nListColumnWidths[i]);
      }
      theCtrl.SetColumnOrderArray(LIST_COLUMNS, &m_nListColumnOrder[0]);
   }
   return bReturn;
}

void CMSDNEditView::OnEditDelete()
{
   CListCtrl& theCtrl = GetListCtrl();
   int i, nItem, nCount = theCtrl.GetSelectedCount();
   for (i=0; i<nCount; i++)
   { 
      nItem = -1;
      nItem = theCtrl.GetNextItem(nItem, LVNI_SELECTED);
      theCtrl.DeleteItem(nItem);
   }
   if (nCount)
   {
      GetDocument()->SetModifiedFlagEx(DOC_TYPE_HTMLHELPDOCINFO);
   }
}

void CMSDNEditView::OnUpdateEditDelete(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(GetListCtrl().GetSelectedCount() != 0);
}

void CMSDNEditView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
   CListCtrl& theCtrl = GetListCtrl();
	switch (lHint)
   {
      case HTMLHELPDOCINFO_INSERT:
      {
         MSDNDocComp *pMsdnF = (MSDNDocComp*)pHint;
         int i = 1, nCount = theCtrl.GetItemCount();
         CString str;

         theCtrl.InsertItem(nCount, pMsdnF->strDocCompID);
         str.Format("%d", pMsdnF->nDocCompLanguage);
         theCtrl.SetItemText(nCount, i++, str);
         str.Format("%d", pMsdnF->nColNum);
         theCtrl.SetItemText(nCount, i++, str);
         theCtrl.SetItemText(nCount, i++, pMsdnF->strTitleLocation);
         theCtrl.SetItemText(nCount, i++, pMsdnF->strIndexLocation);
         theCtrl.SetItemText(nCount, i++, pMsdnF->strQueryLocation);
         theCtrl.SetItemText(nCount, i++, pMsdnF->strLocationRef);
         str.Format("%d", pMsdnF->nVersion);
         theCtrl.SetItemText(nCount, i++, str);
         str.Format("%d", pMsdnF->nLastPromptedVersion);
         theCtrl.SetItemText(nCount, i++, str);
         theCtrl.SetItemText(nCount, i++, pMsdnF->strTitleSampleLocation);
         theCtrl.SetItemText(nCount, i++, pMsdnF->strTitleQueryLocation);
         str.Format("%d", pMsdnF->nSupportsMerge);
         theCtrl.SetItemText(nCount, i++, str);
      }break;
      case HTMLHELPCOLLECTION_SELCHANGE:
      {
         LVFINDINFO info;
         int nIndex;
         info.flags = LVFI_PARTIAL|LVFI_STRING;
         info.psz = &((char*)pHint)[1];
         nIndex = theCtrl.FindItem(&info);
         if (nIndex != -1)
         {
            theCtrl.SetSelectionMark(nIndex);
            theCtrl.SetItemState(nIndex, LVIS_SELECTED, LVIS_SELECTED);
            theCtrl.EnsureVisible(nIndex, false);
         }
      } break;
      case HTMLHELPCOLLECTION_GETFILENAMES:
      {
         LVFINDINFO info;
         MSDNDocComp *pDC = (MSDNDocComp*) pHint;
         int nIndex;
         info.flags = LVFI_PARTIAL|LVFI_STRING;
         info.psz = (char*)&LPCTSTR(pDC->strDocCompID)[1];
         nIndex = theCtrl.FindItem(&info);
         if (nIndex != -1)
         {
            pDC->strTitleLocation = theCtrl.GetItemText(nIndex, 3);
            pDC->strIndexLocation = theCtrl.GetItemText(nIndex, 4);
         }
      } break;
      case HTMLHELPDOCINFO_SAVE:
      {
         int i, nCount = theCtrl.GetItemCount();
         CArchive *pAr = (CArchive*)pHint;
         for (i=0; i<nCount; i++)
         {
            SaveListItem(i, *pAr);
         }
      }break;
      case HTMLHELPDOCINFO_EXPORT:
      {
         int i, nItem, nCount = theCtrl.GetSelectedCount();
         CArchive *pAr = (CArchive*)pHint;
         for (i=0, nItem = -1; i<nCount; i++)
         {
            nItem = theCtrl.GetNextItem(nItem, LVNI_SELECTED);
            SaveListItem(nItem, *pAr);
         }
      }break;
      case HTMLHELPDOCINFO_DELETE:
      {
         theCtrl.DeleteAllItems();
      }break;
   }
}

void CMSDNEditView::OnDestroy() 
{
	CListView::OnDestroy();
   CListCtrl& theCtrl = GetListCtrl();
   theCtrl.GetColumnOrderArray(m_nListColumnOrder, sizeof(m_nListColumnOrder));	
   for (int i=0; i<LIST_COLUMNS; i++)
   {
      m_nListColumnWidths[i] = theCtrl.GetColumnWidth(i);
   }
}

void CMSDNEditView::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	
	*pResult = 0;
}

void CMSDNEditView::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
   if (pDispInfo->item.mask & LVIF_TEXT)
   {
      CListCtrl& theCtrl = GetListCtrl();
      theCtrl.SetItemText(pDispInfo->item.iItem, pDispInfo->item.iSubItem, pDispInfo->item.pszText);
      GetDocument()->SetModifiedFlagEx(DOC_TYPE_HTMLHELPDOCINFO);
   }
   
	*pResult = 0;
}


void CMSDNEditView::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
   CListCtrl& theCtrl = GetListCtrl();
   int nCount = theCtrl.GetSelectedCount();
   if (nCount)
   {
      CEditColumnTexts dlg;
      int  i, nPos = -1,  nItem = -1;
      CString str;
      nItem = theCtrl.GetNextItem(nItem, LVNI_SELECTED);
      dlg.m_strEditWhat.LoadString(gm_nIDs[pNMListView->iSubItem]);
      dlg.m_strEditText = theCtrl.GetItemText(nItem, pNMListView->iSubItem);
      dlg.m_nNumberOfItems = nCount;
      ASSERT(nItem != -1);
      switch (pNMListView->iSubItem)
      {
         case 0: return; // IDS_LIST_HEAD_MODULE
         case 7:  // IDS_LIST_HEAD_VERSION
            dlg.m_bVersion = true;
            break;
         case 1:  // IDS_LIST_HEAD_LANGID: Sprache
         case 2:  // IDS_LIST_HEAD_COL_NUM
         case 8:  // IDS_LIST_HEAD_LAST_PROMPT_VERSION
         case 11: // IDS_LIST_HEAD_TITLE_SUPPORT_MERGE
            dlg.m_bNumber=true;
            break;
         case 3:  // IDS_LIST_HEAD_CHM_PATH
         case 4:  // IDS_LIST_HEAD_CHI_PATH
         case 5:  // IDS_LIST_HEAD_QUERY_LOC
         if (nCount > 1)
         {
            nPos = dlg.m_strEditText.ReverseFind('\\');
            if (nPos != -1)
            {
               dlg.m_strEditText = dlg.m_strEditText.Left(nPos+1);
            }
         }break;
         case 9:  break; // IDS_LIST_HEAD_TITLE_SAMP_LOC
         case 10: break; // IDS_LIST_HEAD_TITLE_QUERY_LOC
         case 6:  break; // IDS_LIST_HEAD_LOC_REF
      }
      if (dlg.DoModal() == IDOK)
      {
         if (nPos != -1)
         {
            if (dlg.m_strEditText.GetLength() > 3)
            {
               if (dlg.m_strEditText.GetAt(dlg.m_strEditText.GetLength()-1) != '\\')
               {
                  dlg.m_strEditText += _T("\\");
               }
            }
         }
         for (i=0, nItem = -1; i<nCount; i++)
         {
            nItem = theCtrl.GetNextItem(nItem, LVNI_SELECTED);
            ASSERT(nItem != -1);
            switch (pNMListView->iSubItem)
            {
               case 1: // IDS_LIST_HEAD_LANGID: Sprache
               case 2: // IDS_LIST_HEAD_COL_NUM
               case 7: // IDS_LIST_HEAD_VERSION
               case 8: // IDS_LIST_HEAD_LAST_PROMPT_VERSION
               case 11:// IDS_LIST_HEAD_TITLE_SUPPORT_MERGE
                  theCtrl.SetItemText(nItem, pNMListView->iSubItem, dlg.m_strEditText);
                  break;
               case 3: // IDS_LIST_HEAD_CHM_PATH
               case 4: // IDS_LIST_HEAD_CHI_PATH
               case 5: // IDS_LIST_HEAD_QUERY_LOC
                  if (dlg.m_bDeleteText)
                  {
                     theCtrl.SetItemText(nItem, pNMListView->iSubItem, "");
                  }
                  else if (nPos != -1)
                  {
                     str = theCtrl.GetItemText(nItem, pNMListView->iSubItem);
                     int nPosN = str.ReverseFind('\\');
                     if (nPosN != -1)
                     {
                        str = dlg.m_strEditText + str.Right(str.GetLength()-nPosN-1);
                     }
                     else
                     {
                        str = dlg.m_strEditText + str;
                     }
                     theCtrl.SetItemText(nItem, pNMListView->iSubItem, str);
                  }
                  else
                  {
                     theCtrl.SetItemText(nItem, pNMListView->iSubItem, dlg.m_strEditText);
                  }
                  break;
               case 9: break; // IDS_LIST_HEAD_TITLE_SAMP_LOC
               case 6: break; // IDS_LIST_HEAD_LOC_REF
               case 10: break; // IDS_LIST_HEAD_TITLE_QUERY_LOC
                  theCtrl.SetItemText(nItem, pNMListView->iSubItem, dlg.m_strEditText);
                  break;
            }
         }
         InvalidateRect(NULL);
         GetDocument()->SetModifiedFlagEx(DOC_TYPE_HTMLHELPDOCINFO);
      }
   }
	*pResult = 0;
}

void CMSDNEditView::SaveListItem(int nItem, CArchive &ar)
{
   CMSDNEditDoc *pDoc = GetDocument();
   CListCtrl& theCtrl = GetListCtrl();
   CString str;
   int i = 0;
   ar.WriteString(pDoc->m_strDocCompilation+pDoc->m_strNewLine);
   str.Format("\t%s\"%s\"/>%s", pDoc->m_strDocCompId, theCtrl.GetItemText(nItem, i++), pDoc->m_strNewLine);
	ar.WriteString(str);
   str.Format("\t%s%s/>%s", pDoc->m_strDocCompLanguage, theCtrl.GetItemText(nItem, i++), pDoc->m_strNewLine);
	ar.WriteString(str);
   ar.WriteString(_T("\t")+pDoc->m_strLocationHistory+pDoc->m_strNewLine);
   str.Format("\t\t%s%s/>%s", pDoc->m_strColNum, theCtrl.GetItemText(nItem, i++), pDoc->m_strNewLine);
	ar.WriteString(str);
   str.Format("\t\t%s\"%s\"/>%s", pDoc->m_strTitleLocation, theCtrl.GetItemText(nItem, i++), pDoc->m_strNewLine);
	ar.WriteString(str);
   str.Format("\t\t%s\"%s\"/>%s", pDoc->m_strIndexLocation, theCtrl.GetItemText(nItem, i++), pDoc->m_strNewLine);
	ar.WriteString(str);
   str.Format("\t\t%s\"%s\"/>%s", pDoc->m_strQueryLocation, theCtrl.GetItemText(nItem, i++), pDoc->m_strNewLine);
	ar.WriteString(str);
   str.Format("\t\t%s\"%s\"/>%s", pDoc->m_strLocationRef, theCtrl.GetItemText(nItem, i++), pDoc->m_strNewLine);
	ar.WriteString(str);
   str.Format("\t\t%s%s/>%s", pDoc->m_strDCVersion, theCtrl.GetItemText(nItem, i++), pDoc->m_strNewLine);
	ar.WriteString(str);
   str.Format("\t\t%s%s/>%s", pDoc->m_strLastPromptedVersion, theCtrl.GetItemText(nItem, i++), pDoc->m_strNewLine);
	ar.WriteString(str);
   str.Format("\t\t%s\"%s\"/>%s", pDoc->m_strTitleSampleLocation, theCtrl.GetItemText(nItem, i++), pDoc->m_strNewLine);
	ar.WriteString(str);
   str.Format("\t\t%s\"%s\"/>%s", pDoc->m_strTitleQueryLocation, theCtrl.GetItemText(nItem, i++), pDoc->m_strNewLine);
	ar.WriteString(str);
   str.Format("\t\t%s%s/>%s", pDoc->m_strSupportsMerge, theCtrl.GetItemText(nItem, i++), pDoc->m_strNewLine);
	ar.WriteString(str);
   ar.WriteString(_T("\t")+pDoc->m_strNLocationHistory+pDoc->m_strNewLine);
   ar.WriteString(pDoc->m_strNDocCompilation+pDoc->m_strNewLine);
}


void CMSDNEditView::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
}

void CMSDNEditView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
   CListCtrl& theCtrl = GetListCtrl();
   int nSel = theCtrl.GetSelectionMark();
	if (nSel != -1)
   {
      CString str = theCtrl.GetItemText(nSel, 0);
      GetDocument()->UpdateAllViews(this, HTMLHELPDOCINFO_SELCHANGE, (CObject*)&str);
   }
	*pResult = 0;
}
