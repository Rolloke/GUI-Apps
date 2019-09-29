// FormatView.cpp : Implementierung der Klasse CFormatView
//

#include "stdafx.h"
#include "AnyFileViewer.h"
#include "MainFrm.h"
#include "AnyFileViewerDoc.h"
#include "AnyFileViewerView.h"
#include "FormatView.h"
#include "DisplayType.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning( push )
#pragma warning( disable : 4238 )
#pragma warning( disable : 4239 )

// CFormatView

IMPLEMENT_DYNCREATE(CFormatView, CListView)

BEGIN_MESSAGE_MAP(CFormatView, CListView)
    ON_WM_SIZE()
    ON_WM_CREATE()
    ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
    ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
    ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
    ON_COMMAND(ID_EDIT_CUT, OnEditCut)
    ON_COMMAND(ID_FORMAT_SAVE, OnFormatSave)
    ON_COMMAND(ID_EDIT_CLEAR_ALL, OnEditClearAll)
    ON_COMMAND(ID_FORMAT_EDIT, OnFormatEdit)
    ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnLvnEndlabeledit)
    ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
    ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditClear)
    ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditClear)
    ON_UPDATE_COMMAND_UI(ID_FORMAT_EDIT, OnUpdateEditClear)
    ON_UPDATE_COMMAND_UI(ID_FORMAT_SAVE, OnUpdateFormatSave)
    ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
    ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnLvnItemchanged)
    ON_NOTIFY(HDN_ENDTRACKA, 0, OnHdnEndtrack)
    ON_NOTIFY(HDN_ENDTRACKW, 0, OnHdnEndtrack)
    ON_UPDATE_COMMAND_UI(ID_VIEW_CONCATENATED, OnUpdateViewConcatenated)
    ON_COMMAND(ID_VIEW_CONCATENATED, OnViewConcatenated)
    ON_UPDATE_COMMAND_UI(ID_VIEW_SAME_STARTPOS, OnUpdateViewSameStartpos)
    ON_COMMAND(ID_VIEW_SAME_STARTPOS, OnViewSameStartpos)
    ON_COMMAND(ID_FORMAT_ALL, OnFormatAll)
    ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


// CFormatView Erstellung/Zerstörung

CFormatView::CFormatView(): m_bInOnUpdate(FALSE)
, m1stColWidth(33)
, m2ndColWidth(33)
, m_bConcatenated(FALSE)

{
    m_nOrder[0] = 1;
    m_nOrder[1] = 2;
    m_nOrder[2] = 0;
    m1stColWidth = theApp.GetProfileInt(SECTION_SETTINGS, OF_MEMBER(m1stColWidth));
    m2ndColWidth = theApp.GetProfileInt(SECTION_SETTINGS, OF_MEMBER(m2ndColWidth));
}

CFormatView::~CFormatView()
{
    theApp.WriteProfileInt(SECTION_SETTINGS, OF_MEMBER(m1stColWidth));
    theApp.WriteProfileInt(SECTION_SETTINGS, OF_MEMBER(m2ndColWidth));
}

BOOL CFormatView::PreCreateWindow(CREATESTRUCT& cs)
{
    cs.style &= ~LVS_TYPEMASK;
    cs.style |= LVS_REPORT| LVS_EDITLABELS | LVS_SHOWSELALWAYS | WS_BORDER | WS_TABSTOP;

   return CListView::PreCreateWindow(cs);
}

void CFormatView::OnInitialUpdate()
{
   CListView::OnInitialUpdate();
}


// CFormatView Diagnose

#ifdef _DEBUG
void CFormatView::AssertValid() const
{
   CListView::AssertValid();
}

void CFormatView::Dump(CDumpContext& dc) const
{
   CListView::Dump(dc);
}

CAnyFileViewerDoc* CFormatView::GetDocument() // Nicht-Debugversion ist inline
{
   ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAnyFileViewerDoc)));
   return (CAnyFileViewerDoc*)m_pDocument;
}
#endif //_DEBUG


// CFormatView Meldungshandler

void CFormatView::OnSize(UINT nType, int cx, int cy)
{
    if (nType != 0xffffffff)
    {
        CListView::OnSize(nType, cx, cy);
    }
    int nW1 = MulDiv(cx, m1stColWidth, 100);
    int nW2 = MulDiv(cx, m2ndColWidth, 100);
    CListCtrl&theList = GetListCtrl();
    theList.SetColumnWidth(m_nOrder[0], nW1);
    theList.SetColumnWidth(m_nOrder[1], nW2);
    theList.SetColumnWidth(m_nOrder[2], cx-nW1-nW2);
}

int CFormatView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CListView::OnCreate(lpCreateStruct) == -1)
        return -1;

    CListCtrl&ctrl = GetListCtrl();

    ctrl.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
        LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT,
        LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

    CString string;
    string.LoadString(IDS_VALUES);
    ctrl.InsertColumn(0, string, LVCFMT_LEFT, 230);
    string.LoadString(IDS_NAME);
    ctrl.InsertColumn(1, string, LVCFMT_LEFT, 100);
    string.LoadString(IDS_FORMATS);
    ctrl.InsertColumn(2, string, LVCFMT_LEFT, 100);

    ctrl.SetColumnOrderArray(sizeof(m_nOrder)/sizeof(m_nOrder[0]), m_nOrder);

    return 0;
}

void  CFormatView::OnInsertDataType(CString &sName, int nDataType, int nLen)
{
    CListCtrl&ctrl = GetListCtrl();

    int nItem = -1;
    if (ctrl.GetSelectedCount())
    {
        nItem = ctrl.GetSelectionMark();
    }
    else
    {
        nItem = ctrl.GetItemCount();
    }
    nItem = ctrl.InsertItem(nItem, _T(""));
    ctrl.SetItemData(nItem, MAKELONG(nDataType, nLen));
    ctrl.SetItemText(nItem, 1, sName);
    ctrl.SetItemText(nItem, 2, theApp.GetDisplayType(nDataType).Type());
    OnUpdate(this, UPDATE_DATA_POS, 0);
}

void CFormatView::OnUpdate(CView* /*pSender*/, LPARAM lHint, CObject* pHint)
{
    m_bInOnUpdate = TRUE;
    if (lHint == UPDATE_DATA_POS)
    {
        CAnyFileViewerView *pView = theApp.GetMainFrame()->GetMainPane();
        CListCtrl&ctrl = GetListCtrl();
        int    i, nType, nLen;
        DWORD  dwType;
        int    nItems = ctrl.GetItemCount();
        size_t nPos   = pView->GetDataPosition();
        size_t nSize  = GetDocument()->GetSize();
        BYTE*  pData  = GetDocument()->GetData();
        if (pHint && pHint->IsKindOf(RUNTIME_CLASS(CHint)))
        {
            nPos = ((CHint*)pHint)->GetINT();
        }
        if (pData)
        {
            pData += nPos;
            for (i=0; i<nItems; i++)
            {
                dwType = (DWORD)ctrl.GetItemData(i);
                nType  = LOWORD(dwType);
                nLen   = HIWORD(dwType);
                if (nLen)
                {
                    theApp.GetDisplayType(nType).SetBytes(nLen);
                }
                ctrl.SetItemText(i, 0, theApp.GetDisplayType(nType).Display(pData));
                if (m_bConcatenated)
                {
                  pData += theApp.GetDisplayType(nType).GetByteLength();
                  nPos  += theApp.GetDisplayType(nType).GetByteLength();
                }
                if (nPos >= nSize)
                {
                    break;
                }
            }
        }
    }
    m_bInOnUpdate = FALSE;
}

void CFormatView::OnEditCopy()
{
    theApp.GetMainFrame()->SetClipboardData(GetItemDataType(GetListCtrl().GetSelectionMark()));
}

void CFormatView::OnEditClear()
{
    CListCtrl&ctrl = GetListCtrl();
    int i = ctrl.GetSelectionMark();
    ctrl.DeleteItem(i);
}

void CFormatView::OnEditPaste()
{
    CString str = theApp.GetMainFrame()->GetClipboardData();
    if (!str.IsEmpty())
    {
        InsertItemDataType(str);
    }
}

void CFormatView::OnEditCut()
{
    OnEditCopy();
    OnEditClear();
}

void CFormatView::OnLvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
    CAnyFileViewerView *pView = theApp.GetMainFrame()->GetMainPane();
    CListCtrl&ctrl = GetListCtrl();
    int    i, nType, nLen;
    DWORD  dwType;
    int    nItem  = pDispInfo->item.iItem;
    size_t nPos   = 0;
    size_t nSize  = GetDocument()->GetSize();
    BYTE*  pData  = GetDocument()->GetData();
       
    if (pData)
    {
        size_t nPosition = pView->GetDataPosition();
        pData += nPosition;
        nPos   = nPosition;
        for (i=0; i<nItem; i++)
        {
            dwType = (DWORD)ctrl.GetItemData(i);
            nType  = LOWORD(dwType);
            nLen   = HIWORD(dwType);
            if (nLen == 0) nLen = theApp.GetDisplayType(nType).GetByteLength();
            pData += nLen;
            nPos  += nLen;
            if (nPos >= nSize)
            {
                return;
            }
        }
        dwType = (DWORD)ctrl.GetItemData(i);
        nType  = LOWORD(dwType);
        if (nPos + theApp.GetDisplayType(nType).GetByteLength() >= nSize)
        {
            return;
        }
        if (pDispInfo->item.mask & LVIF_TEXT)
        {
            CString s(pDispInfo->item.pszText);
            if (theApp.GetDisplayType(nType).Write(pData, s))
            {
                m_bInOnUpdate = TRUE;
                ctrl.SetItemText(i, 0, s);
                m_bInOnUpdate = FALSE;
                GetDocument()->UpdateAllViews(this, UPDATE_DATA_AT_POS, &CHint(nPos));
                GetDocument()->SetModifiedFlag(TRUE);
            }
        }
        else 
        {
            GetDocument()->UpdateAllViews(this, UPDATE_DATA_FORMAT_POS, &CHint(nPos));
        }
    }

    *pResult = 0;
}

void CFormatView::OnUpdateEditClear(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(GetListCtrl().GetSelectedCount() != 0);
}

void CFormatView::OnFormatSave()
{
    CListCtrl&ctrl = GetListCtrl();
    CString fIdFmt;
    fIdFmt.LoadString(IDS_FORMAT_FILE_DLG);
    CFileDialog dlg(FALSE, _T("fmt"), NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, fIdFmt, this);
    dlg.m_ofn.lpstrInitialDir = theApp.mFormatFilesDir;
    if (dlg.DoModal() == IDOK)
    {
        theApp.mFormatFilesDir = dlg.GetPathName();
        int fPos = theApp.mFormatFilesDir.ReverseFind(_T('\\'));
        if (fPos != -1)
        {
            theApp.mFormatFilesDir = theApp.mFormatFilesDir.Left(fPos+1);
        }
        CFile file;
        if (file.Open(dlg.GetPathName(), CFile::modeCreate|CFile::modeWrite))
        {
            CString fUnicodeID((_TCHAR)0xfeff, 1);
            int i, fItems = ctrl.GetItemCount();
            for (i=0; i<fItems; i++)
            {
                fUnicodeID += GetItemDataType(i) + _T("\r\n");
            }
            file.Write((void*)LPCTSTR(fUnicodeID), fUnicodeID.GetLength()*sizeof(_TCHAR));
            file.Close();
       }
        theApp.GetMainFrame()->UpdateFormatFiles();
    }
}

void CFormatView::OnUpdateFormatSave(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(GetListCtrl().GetItemCount() > 0);
}

CString CFormatView::GetItemDataType(int i)
{
   CString str;
   if (i != -1)
   {
      CListCtrl&ctrl = GetListCtrl();
      DWORD dw = (DWORD)ctrl.GetItemData(i);
      str.Format(_T("%s:%s:%d"), LPCTSTR(ctrl.GetItemText(i, 1)),
          CDisplayType::getNameOfType(static_cast<CDisplayType::eType>(LOWORD(dw))), HIWORD(dw));
   }
   return str;
}

void CFormatView::InsertItemDataType(CString sFmt)
{
   int n1, n2;
   n1 = sFmt.Find(_T(":"));
   if (n1 != -1)
   {
      n2 = sFmt.Find(_T(":"), n1+1);
      if (n2 != -1)
      {
          CString sType = sFmt.Mid(n1+1, n2-n1-1);
          int nType = CDisplayType::Unknown;
          if (sType.GetLength() && isalpha(sType[0])) nType = CDisplayType::getTypeOfName(sType);
          else                                        nType = _ttoi(sType);
          OnInsertDataType(sFmt.Left(n1), nType, _ttoi(sFmt.Mid(n2+1)));
      }
   }
}

void CFormatView::OnEditClearAll()
{
   GetListCtrl().DeleteAllItems();
}

void CFormatView::OnUpdateEditPaste(CCmdUI *pCmdUI)
{
#ifdef _UNICODE
   pCmdUI->Enable(::IsClipboardFormatAvailable(CF_UNICODETEXT));
#else
   pCmdUI->Enable(::IsClipboardFormatAvailable(CF_TEXT));
#endif
}

void CFormatView::OnFormatEdit()
{
   CListCtrl&ctrl = GetListCtrl();
   int nItem = ctrl.GetSelectionMark();
   if (nItem != -1)
   {
      ctrl.EditLabel(nItem);
   }
}

void CFormatView::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
   LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
   if (!m_bInOnUpdate)
   {
      NMLVDISPINFO di = {0};
      di.item.iItem = pNMLV->iItem;
      OnLvnEndlabeledit((NMHDR*)&di, pResult);
   }
   *pResult = 0;
}

void CFormatView::LoadFormatFile(LPCTSTR sFile)
{
   CXFile file;
   if (file.Open(sFile, CFile::modeRead))
   {
      theApp.mFormatFilesDir = sFile;
      int n = theApp.mFormatFilesDir.ReverseFind(_T('\\'));
      if (n != -1)
      {
         theApp.mFormatFilesDir = theApp.mFormatFilesDir.Left(n+1);
      }
      CString str;
      while (file.ReadString(str))
      {
         InsertItemDataType(str);
      }
      m_bConcatenated = TRUE;
   }
}

void CFormatView::SetAllFormats()
{
   CString str;
   int i;
   OnEditClearAll();
   m_bConcatenated = FALSE;
   str = _T("-");
   for (i=0; i<theApp.GetNoOfDataTypes(); i++)
   {
      OnInsertDataType(str, i, 0);
   }
}

void CFormatView::OnHdnEndtrack(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

    CRect rc;
    GetClientRect(&rc);
    BOOL bUpdate = FALSE;
    if (phdr->iItem == m_nOrder[0])
    {
        int nOld = m1stColWidth;
        m1stColWidth  = MulDiv(phdr->pitem->cxy, 100, rc.right);
        m2ndColWidth -= ((m1stColWidth - nOld) / 2);
        bUpdate = TRUE;
    }
    else if (phdr->iItem == m_nOrder[1])
    {
        m2ndColWidth  = MulDiv(phdr->pitem->cxy, 100, rc.right);
        bUpdate = TRUE;
    }
    if (bUpdate)
    {
        OnSize(0xffffffff, rc.right, rc.bottom);
    }
    *pResult = 0;
}

#pragma warning( pop )

void CFormatView::OnUpdateViewConcatenated(CCmdUI *pCmdUI)
{
    pCmdUI->SetRadio(m_bConcatenated == TRUE);
}

void CFormatView::OnUpdateViewSameStartpos(CCmdUI *pCmdUI)
{
    pCmdUI->SetRadio(m_bConcatenated == FALSE);
}

void CFormatView::OnViewConcatenated()
{
    m_bConcatenated = TRUE;
}

void CFormatView::OnViewSameStartpos()
{
    m_bConcatenated = FALSE;
}

void CFormatView::OnFormatAll()
{
    SetAllFormats();
}


void CFormatView::OnContextMenu(CWnd* /* pWnd */, CPoint /* point */)
{
    CMenu fContextMenu;
    CPoint screen;
    GetCursorPos(&screen);
    fContextMenu.LoadMenu(IDR_CONTEXT_TREE_ITEM);
    fContextMenu.GetSubMenu(ContentFormat)->TrackPopupMenu(TPM_LEFTALIGN, screen.x, screen.y, AfxGetMainWnd());
}
