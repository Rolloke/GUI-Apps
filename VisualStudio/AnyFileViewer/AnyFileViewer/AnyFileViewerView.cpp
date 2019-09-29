// AnyFileViewerView.cpp : Implementierung der Klasse CAnyFileViewerView
//

#include "stdafx.h"
#include "AnyFileViewer.h"
#include "MainFrm.h"

#include "dlgs.h"

#include "AnyFileViewerDoc.h"
#include "AnyFileViewerView.h"
#include ".\anyfileviewerview.h"

#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CFindReplaceDialogEx::CFindReplaceDialogEx()
{
    m_fr.Flags |=  FR_ENABLEHOOK;//|FR_NOMATCHCASE|FR_NOWHOLEWORD;
    m_fr.lpfnHook = FRHookProc;
    m_nCurrentFindPos = 0;
}

UINT_PTR CALLBACK CFindReplaceDialogEx::FRHookProc(HWND hdlg, UINT uiMsg, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    switch (uiMsg)
    {
        case WM_INITDIALOG:
        {
            CString str;
            str.LoadString(IDS_UNICODE_SEARCH);
            ::SetDlgItemText(hdlg, chx1, str);
        }   return 1;
    }
    return 0;
}

// CAnyFileViewerView

IMPLEMENT_DYNCREATE(CAnyFileViewerView, CListView)

BEGIN_MESSAGE_MAP(CAnyFileViewerView, CListView)
    ON_WM_STYLECHANGED()
    ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnLvnGetdispinfo)
    ON_WM_SIZE()
    ON_WM_CREATE()
    ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnLvnItemchanged)
    ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
    ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
    ON_UPDATE_COMMAND_UI(ID_EDIT_FIND, OnUpdateEditFind)
    ON_NOTIFY(HDN_ENDTRACKA, 0, OnHdnEndtrack)
    ON_NOTIFY(HDN_ENDTRACKW, 0, OnHdnEndtrack)
    ON_COMMAND(ID_ACCEL_LEFT, OnAccelLeft)
    ON_UPDATE_COMMAND_UI(ID_ACCEL_LEFT, OnUpdateAccelLeft)
    ON_COMMAND(ID_ACCEL_RIGHT, OnAccelRight)
    ON_UPDATE_COMMAND_UI(ID_ACCEL_RIGHT, OnUpdateAccelRight)
    ON_COMMAND(ID_VIEW_UPDATE, &CAnyFileViewerView::OnViewUpdate)
END_MESSAGE_MAP()

// CAnyFileViewerView Erstellung/Zerstörung

CAnyFileViewerView::CAnyFileViewerView() :m_nColumns(4)
, m_nColPosition(0)
, m_nCurrentLine(0)
, m_nType(CDisplayType::HEX8)
, m_nOffset(0)
, m_bInOnUpdate(TRUE)
, m1stColWidthOfView(50)
{
    m1stColWidthOfView = theApp.GetProfileInt(SECTION_SETTINGS, OF_MEMBER(m1stColWidthOfView));
}

CAnyFileViewerView::~CAnyFileViewerView()
{
   theApp.WriteProfileInt(SECTION_SETTINGS, OF_MEMBER(m1stColWidthOfView));
}

BOOL CAnyFileViewerView::PreCreateWindow(CREATESTRUCT& cs)
{
   cs.style &= ~LVS_TYPEMASK;
   cs.style |= LVS_REPORT | LVS_SHOWSELALWAYS | LVS_OWNERDATA | WS_BORDER | WS_TABSTOP;

    return CListView::PreCreateWindow(cs);
}

void CAnyFileViewerView::OnInitialUpdate()
{
    CListView::OnInitialUpdate();
}

// CAnyFileViewerView Diagnose

#ifdef _DEBUG
void CAnyFileViewerView::AssertValid() const
{
    CListView::AssertValid();
}

void CAnyFileViewerView::Dump(CDumpContext& dc) const
{
    CListView::Dump(dc);
}

CAnyFileViewerDoc* CAnyFileViewerView::GetDocument() const // Nicht-Debugversion ist inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAnyFileViewerDoc)));
    return (CAnyFileViewerDoc*)m_pDocument;
}
#endif //_DEBUG

void CAnyFileViewerView::OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
    if(pDispInfo->item.mask & LVIF_TEXT)
    {
        BYTE *pData = GetDocument()->GetData();
        if (pData)
        {
            if (m_nType == CDisplayType::UnicodeLine || m_nType == CDisplayType::AsciiLine)
            {
                if (pDispInfo->item.iSubItem == 0)
                {
                    CDisplayType&fDPL = theApp.GetDisplayType(m_nType == CDisplayType::AsciiLine ? CDisplayType::Ascii : CDisplayType::Unicode);
                    pData += mItems[pDispInfo->item.iItem].mStartPos;
                    fDPL.SetBytes(0);
                    m_sDisplay = fDPL.Display(pData);
                }
                else
                {
                    m_sDisplay = "";
                }
            }
            else if (m_nType == CDisplayType::Structure)
            {
                if (pDispInfo->item.iSubItem == 0)
                {
                    CDisplayType& fDPL = theApp.GetDisplayType(mItems[pDispInfo->item.iItem].mDisplayType);
                    pData += mItems[pDispInfo->item.iItem].mStartPos;
                    m_sDisplay = fDPL.Display(pData);
                }
                else
                {
                    m_sDisplay = mItems[pDispInfo->item.iItem].mDisplayName;
                    m_sDisplay += ": [";
                    m_sDisplay += CDisplayType::getNameOfType(static_cast<CDisplayType::eType>(mItems[pDispInfo->item.iItem].mDisplayType));
                    m_sDisplay += "]";
                }
            }
            else
            {
                int i, nBytesPerCol = theApp.GetDisplayType(m_nType).GetByteLength();
                int nBytesPerLine   = m_nColumns * nBytesPerCol;
                i = nBytesPerLine * (pDispInfo->item.iItem) + m_nOffset;
                pData += i;
                if (pDispInfo->item.iSubItem == 0)
                {
                    m_sDisplay.Empty();
                    int nColumn = nBytesPerCol > 0 ? m_nColPosition / nBytesPerCol : 0;
                    for (i=0; i<m_nColumns; i++)
                    {
                        if (pDispInfo->item.iItem == m_nCurrentLine)
                        {
                            if (nColumn == i)
                            {
                                CString sDisplay = theApp.GetDisplayType(m_nType).Display(pData);
                                if (m_nType == CDisplayType::HEX4 || m_nType == CDisplayType::HEX8)
                                {
                                    int n = (m_nColPosition - nColumn*nBytesPerCol)*2;
                                    m_sDisplay += sDisplay.Left(n) + _T("[") + sDisplay.Mid(n, 2) + _T("]") + sDisplay.Mid(n+2);
                                }
                                else
                                {
                                    m_sDisplay += _T("[") + sDisplay  + _T("]");
                                }
                            }
                            else
                            {
                                m_sDisplay += theApp.GetDisplayType(m_nType).Display(pData);
                            }
                        }
                        else
                        {
                            m_sDisplay += theApp.GetDisplayType(m_nType).Display(pData);
                        }
                        if (i < m_nColumns-1)
                        {
                            m_sDisplay += _T(" | ");
                        }
                        pData += nBytesPerCol;
                    }
                }
                else
                {
                    theApp.GetDisplayType(CDisplayType::Ascii).SetBytes(nBytesPerLine);
                    m_sDisplay = theApp.GetDisplayType(CDisplayType::Ascii).Display(pData);
                    int nPos = m_nColPosition;
                    if (pDispInfo->item.iItem == m_nCurrentLine)
                    {
                        m_sDisplay = m_sDisplay.Left(nPos) + _T("[") + m_sDisplay.Mid(nPos, 1) + _T("]") + m_sDisplay.Mid(nPos+1);
                    }
                }
            }
            pDispInfo->item.pszText = (LPTSTR)LPCTSTR(m_sDisplay);
            pDispInfo->item.cchTextMax = m_sDisplay.GetLength();
        }
    }
    *pResult = 0;
}

void CAnyFileViewerView::OnSelectChangeDataType(int nType)
{
    m_nType = static_cast<CDisplayType::eType>(nType);
    OnUpdate(NULL, UPDATE_LINE_SIZE, NULL);
    GetListCtrl().Update(-1);
}

int  CAnyFileViewerView::OnEnChangeEdtColumns(int nColumns)
{
    if (nColumns < 1)  nColumns = 1;
    if (nColumns > 32) nColumns = 32;
    if (m_nColumns != nColumns)
    {
        m_nColumns = nColumns;
        OnUpdate(NULL, UPDATE_LINE_SIZE, NULL);
        GetListCtrl().Update(-1);
    }
    return nColumns;
}

int  CAnyFileViewerView::OnEnChangeEdtOffset(int nOffset)
{
    if (nOffset < 0)  nOffset = 0;
    if (m_nOffset != nOffset)
    {
        m_nOffset = nOffset;
        OnUpdate(NULL, UPDATE_LINE_SIZE, NULL);
        GetListCtrl().Update(-1);
    }
    return nOffset;
}

void CAnyFileViewerView::OnEditFind()
{
    if (m_dlgFR.m_hWnd == NULL)
    {
        m_dlgFR.Create(TRUE, NULL);
    }
    else
    {
        m_dlgFR.DestroyWindow();
    }
}

void CAnyFileViewerView::OnSize(UINT nType, int cx, int cy)
{
    if (nType != 0xffffffff)
    {
        CListView::OnSize(nType, cx, cy);
    }

    int nW1 = MulDiv(cx, m1stColWidthOfView, 100);
    int nW2 = cx - nW1;
    CListCtrl& theList =GetListCtrl();
    theList.SetColumnWidth(0, nW1);
    theList.SetColumnWidth(1, nW2);
}

int CAnyFileViewerView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CListView::OnCreate(lpCreateStruct) == -1)
        return -1;
       
    CListCtrl&ctrl = GetListCtrl();
    LOGFONT lf;
    ctrl.GetFont()->GetLogFont(&lf);
    lf.lfPitchAndFamily = FIXED_PITCH;
    lf.lfFaceName[0] = 0;
    m_FixedFont.CreateFontIndirect(&lf);
    ctrl.SetFont(&m_FixedFont);
    ctrl.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
        LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT,
        LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

    CString string;
    string.LoadString(IDS_CONTENT);
    ctrl.InsertColumn(0, string, LVCFMT_LEFT, 100);
    string.LoadString(IDS_ASCII);
    ctrl.InsertColumn(1, string, LVCFMT_LEFT, 100);

   return 0;
}

void CAnyFileViewerView::OnUpdate(CView* /*pSender*/, LPARAM lHint, CObject* pObject)
{
    m_bInOnUpdate = TRUE;
    CHint *pHint = (CHint*)pObject;

    CListCtrl&ctrl = GetListCtrl();
    if (   lHint == UPDATE_INITIAL // initial update
        || lHint == UPDATE_LINE_SIZE)
    {  
        if (lHint == UPDATE_INITIAL)
        {
            m_nOffset = 0;
            theApp.GetMainFrame()->GetMainDlgBar().SetDlgItemInt(IDC_EDT_OFFSET, m_nOffset, 0);
        }
        int nRows    = 1;
        size_t nSize = GetDocument()->GetSize();
        if (m_nType == CDisplayType::AsciiLine || m_nType == CDisplayType::UnicodeLine)
        {
            BYTE *pData = GetDocument()->GetData();
            CDisplayType&fDPL = theApp.GetDisplayType(m_nType == CDisplayType::AsciiLine ? CDisplayType::Ascii : CDisplayType::Unicode);
            size_t nLen;
            size_t nPos = 0;
            mItems.clear();
            fDPL.SetBytes(0);
            while (nPos < GetDocument()->GetSize())
            {
                nLen = fDPL.GetByteLength(pData);
                if (nLen == 0)
                {
                    break; // exit, if no progress in data
                }
                pData += nLen;
                mItems.push_back(CStructureItem(nPos));
                nPos  += nLen;
            }
            nRows = static_cast<int>(mItems.size());
            theApp.GetDisplayType(CDisplayType::Ascii).SetBytes(0);
        }
        else if (m_nType == CDisplayType::Structure)
        {
            mItems.clear();
            CItemStructureHint fHint(mItems);
            GetDocument()->UpdateAllViews(this, UPDATE_STRUCTURE_VIEW, &fHint);
            nRows = static_cast<int>(mItems.size());
        }
        else
        {
            int nBytes   = 0;
            nBytes = m_nColumns*theApp.GetDisplayType(m_nType).GetByteLength();
            nRows  = (int)(nBytes > 0 ? nSize / nBytes : 1);
            theApp.GetDisplayType(CDisplayType::Ascii).SetBytes(nBytes);
        }
        ctrl.SetItemCount(nRows);
    }
    if (lHint == UPDATE_DATA_AT_POS)
    {
        ASSERT(pHint != NULL);
        ASSERT_KINDOF(CHint, pObject);
        size_t nPos   = pHint->GetSizeT();
        if (nPos >= GetDocument()->GetSize()) return;
        if (m_nType == CDisplayType::AsciiLine || m_nType == CDisplayType::UnicodeLine || m_nType == CDisplayType::Structure)
        {
            std::vector<CStructureItem>::iterator found = std::find(mItems.begin(), mItems.end(), CStructureItem(nPos));
            if (found != mItems.end())
            {
                int nLine = static_cast<int>(found - mItems.begin());
                ctrl.Update((int) nLine);
            }
            else
            {
                ctrl.Update((int) 0);
            }
        }
        else
        {
            size_t nBytes = m_nColumns*theApp.GetDisplayType(m_nType).GetByteLength();
            size_t nLine  = nBytes > 0 ? nPos / nBytes : 0;
            ctrl.Update((int) nLine);
        }
    }
    if (lHint == UPDATE_DATA_POS)
    {
        ASSERT(pHint != NULL);
        ASSERT_KINDOF(CHint, pObject);
        size_t nPos   = pHint->GetSizeT();
        if (nPos >= GetDocument()->GetSize()) return;
        if (m_nType == CDisplayType::AsciiLine || m_nType == CDisplayType::UnicodeLine || m_nType == CDisplayType::Structure)
        {
            unsigned int position = static_cast<unsigned int>(nPos);
            std::vector<CStructureItem>::iterator found = std::find(mItems.begin(), mItems.end(), CStructureItem(position));
            if (found != mItems.end())
            {
                int nLine = static_cast<int>(found - mItems.begin())+1;
                m_nCurrentLine = (int)nLine;
            }
            else
            {
                m_nCurrentLine = 0;
            }
            ctrl.EnsureVisible(m_nCurrentLine, TRUE);
            m_bInOnUpdate = FALSE;
            ctrl.SetItemState(m_nCurrentLine, LVIS_SELECTED, LVIS_SELECTED);
            m_bInOnUpdate = TRUE;
            ctrl.SetSelectionMark(m_nCurrentLine);
            theApp.GetMainFrame()->SetPaneInt(PANE_POSITION, GetDataPosition(m_nCurrentLine));
            theApp.GetMainFrame()->SetPaneInt(PANE_ROW, m_nCurrentLine+1);
            theApp.GetMainFrame()->SetPaneInt(PANE_COLUMN, m_nColPosition);
        }
        else
        {
            size_t nBytes = m_nColumns*theApp.GetDisplayType(m_nType).GetByteLength();
            size_t nLine  = nBytes > 0 ? nPos / nBytes : 0;
            nPos = nLine * nBytes;
            m_nColPosition = (int)(((CHint*)pHint)->GetSizeT() - nPos);
            ctrl.SetItemState(ctrl.GetSelectionMark(), 0, LVIS_SELECTED);
            m_nCurrentLine = (int)nLine;
            ctrl.EnsureVisible(m_nCurrentLine, TRUE);
            m_bInOnUpdate = FALSE;
            ctrl.SetItemState(m_nCurrentLine, LVIS_SELECTED, LVIS_SELECTED);
            m_bInOnUpdate = TRUE;
            ctrl.SetSelectionMark(m_nCurrentLine);
            theApp.GetMainFrame()->SetPaneInt(PANE_POSITION, GetDataPosition(m_nCurrentLine));
            theApp.GetMainFrame()->SetPaneInt(PANE_ROW, m_nCurrentLine+1);
            theApp.GetMainFrame()->SetPaneInt(PANE_COLUMN, m_nColPosition);
        }
    }
    else if (lHint == UPDATE_DATA_FORMAT_POS)
    {
        ASSERT(pHint != NULL);
        ASSERT_KINDOF(CHint, pObject);
        size_t nPos   = pHint->GetSizeT();
        if (m_nType == CDisplayType::AsciiLine || m_nType == CDisplayType::UnicodeLine || m_nType == CDisplayType::Structure)
        {
            // TODO! UPDATE_DATA_POS for text, find line in 
            std::vector<CStructureItem>::iterator found = std::lower_bound(mItems.begin(), mItems.end(), CStructureItem(nPos));
            size_t fIndex = std::distance(mItems.begin(), found);
            theApp.GetMainFrame()->SetPaneInt(PANE_ROW, fIndex);
        }
        else
        {
            size_t nBytes = m_nColumns*theApp.GetDisplayType(m_nType).GetByteLength();
            size_t nLine  = nBytes > 0 ? nPos / nBytes : 0;
            nPos = nLine * nBytes;
            nPos = (int)(pHint->GetSizeT() - nPos);
            theApp.GetMainFrame()->SetPaneInt(PANE_POSITION, (int)(nLine * nBytes + nPos));
            theApp.GetMainFrame()->SetPaneInt(PANE_ROW, (int)nLine+1);
            theApp.GetMainFrame()->SetPaneInt(PANE_COLUMN, (int)nPos);
        }
    }
    m_bInOnUpdate = FALSE;
}

void CAnyFileViewerView::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    if (!m_bInOnUpdate)
    {
        if (pNMLV->uNewState == (LVIS_SELECTED|LVIS_FOCUSED))
        {
            m_nColPosition = 0;
            m_nCurrentLine = pNMLV->iItem;
            size_t nPos = GetDataPosition(pNMLV->iItem);
            theApp.GetMainFrame()->SetPaneInt(PANE_POSITION, nPos);
            theApp.GetMainFrame()->SetPaneInt(PANE_ROW, pNMLV->iItem+1);
            theApp.GetMainFrame()->SetPaneInt(PANE_COLUMN, m_nColPosition);
            CHint cHint(nPos);
            GetDocument()->UpdateAllViews(this, UPDATE_DATA_POS, &cHint);
        }
    }
    *pResult = 0;
}

size_t CAnyFileViewerView::GetDataPosition(int nItem)
{
    if (nItem == -1)
    {
        CListCtrl&ctrl = GetListCtrl();
        nItem = ctrl.GetSelectionMark();
    }
    if (m_nType == CDisplayType::AsciiLine || m_nType == CDisplayType::UnicodeLine || m_nType == CDisplayType::Structure)
    {
        if (nItem >=0 && nItem < static_cast<int>(mItems.size()))
        {
            return mItems[nItem].mStartPos;
        }
        return 0;
    }
    else
    {
        int nBytesPerCol  = theApp.GetDisplayType(m_nType).GetByteLength();
        int nBytesPerLine = m_nColumns * nBytesPerCol;
        return nItem * nBytesPerLine + m_nColPosition;
    }
}

bool CAnyFileViewerView::GetSelection(size_t& aStartPos, size_t& aStopPos)
{
    CListCtrl&ctrl = GetListCtrl();
    UINT fCount = ctrl.GetSelectedCount();
    UINT fItem  = ctrl.GetSelectionMark();
    if (fCount)
    {
        int nBytesPerCol  = theApp.GetDisplayType(m_nType).GetByteLength();
        int nBytesPerLine = m_nColumns * nBytesPerCol;
        aStartPos = fItem * nBytesPerLine + m_nColPosition;
        aStopPos  = (fItem+fCount) * nBytesPerLine + m_nColPosition;
        return true;
    }
    return false;

}

void CAnyFileViewerView::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(GetListCtrl().GetSelectedCount());
}

void CAnyFileViewerView::OnUpdateEditFind(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(GetDocument()->GetSize() > 0);
}

void CAnyFileViewerView::OnEditCopy()
{
    int i, nBytesPerCol = theApp.GetDisplayType(m_nType).GetByteLength();
    int nBytesPerLine   = m_nColumns * nBytesPerCol;
    CString str;
    CListCtrl&ctrl = GetListCtrl();
    CString fSeparator = theApp.getSeparator();;
    POSITION pos = ctrl.GetFirstSelectedItemPosition();
    while (pos)
    {
        int n = ctrl.GetNextSelectedItem(pos);
        i = nBytesPerLine * n + m_nOffset;
        BYTE *pData = GetDocument()->GetData() + i;
        for (i=0; i<m_nColumns; i++)
        {
            str += theApp.GetDisplayType(m_nType).Display(pData) + fSeparator;
            pData += nBytesPerCol;
        }
    }
    if (str.GetLength())
    {
        theApp.GetMainFrame()->SetClipboardData(str);
    }
}

void CAnyFileViewerView::OnHdnEndtrack(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
    if (phdr->iItem == 0)
    {
        CRect rc;
        GetClientRect(&rc);
        m1stColWidthOfView = MulDiv(phdr->pitem->cxy, 100, rc.right);
        OnSize(0xffffffff, rc.right, rc.bottom);
    }
    *pResult = 0;
}

void CAnyFileViewerView::OnFind(LPFINDREPLACE lpfr)
{
    BYTE *pData = GetDocument()->GetData();
    if (   lpfr->Flags & FR_DIALOGTERM
        || pData == 0)
    {
        return;
    }
    size_t start = m_dlgFR.m_nCurrentFindPos, i, n = GetDocument()->GetSize();
    size_t nLen = _tcslen(lpfr->lpstrFindWhat);
    void * pFindWhat = lpfr->lpstrFindWhat;
    int (*pCmpFnc)(const void*, const void*, size_t);
    BOOL bFound = FALSE;

    if (lpfr->Flags & FR_WHOLEWORD)    // unicode
    {
        if (lpfr->Flags & FR_MATCHCASE)
        {
            pCmpFnc = (int(*)(const void*, const void*, size_t))wcsncmp;
        }
        else
        {
            pCmpFnc = (int(*)(const void*, const void*, size_t))_wcsnicmp;
        }
    }
    else                            // ansi
    {
        m_dlgFR.m_sSearchAnsi = CStringA(lpfr->lpstrFindWhat);
        pFindWhat = (void*)LPCSTR(m_dlgFR.m_sSearchAnsi);
        if (lpfr->Flags & FR_MATCHCASE)
        {
            pCmpFnc = (int(*)(const void*, const void*, size_t))strncmp;
        }
        else
        {
            pCmpFnc = (int(*)(const void*, const void*, size_t))_strnicmp;
        }
    }

    if (lpfr->Flags & FR_DOWN)
    {
        if (start >= n-nLen-1)
        {
            start = 0;
        }
        else
        {
            start++;
        }
        for (i=start; i<n-nLen; i++)
        {
            if (pCmpFnc(&pData[i], pFindWhat, nLen) == 0)
            {
                bFound = TRUE;
                break;
            }
        }
    }
    else
    {
        if (start == 0)
        {
            start = n;
        }
        else
        {
            start--;
        }
        for (i=start; i>0; i--)
        {
            if (pCmpFnc(&pData[i], lpfr->lpstrFindWhat, nLen) == 0)
            {
                bFound = TRUE;
                break;
            }
        }
    }
    if (bFound)
    {
        CHint cHint(i);
        OnUpdate(NULL, UPDATE_DATA_POS, &cHint);
        m_dlgFR.m_nCurrentFindPos = i;
    }
}

void CAnyFileViewerView::OnAccelLeft()
{
    size_t nPos = GetDataPosition() - 1;
    CHint cHint(nPos);
    GetDocument()->UpdateAllViews(NULL, UPDATE_DATA_POS, &cHint);
}

void CAnyFileViewerView::OnUpdateAccelLeft(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(GetDocument()->GetSize() > 0);
}

void CAnyFileViewerView::OnAccelRight()
{
    size_t nPos = GetDataPosition() + 1;
    CHint cHint(nPos);
    GetDocument()->UpdateAllViews(NULL, UPDATE_DATA_POS, &cHint);
}

void CAnyFileViewerView::OnUpdateAccelRight(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(GetDocument()->GetSize() > 0);
}


void CAnyFileViewerView::OnViewUpdate()
{
    if (m_nType == CDisplayType::Structure)
    {
        OnUpdate(this, UPDATE_LINE_SIZE, NULL);
    }
}
