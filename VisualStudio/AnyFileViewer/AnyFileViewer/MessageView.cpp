// MessageView.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "AnyFileViewer.h"
#include "MessageView.h"
#include "AnyFileViewerDoc.h"


// CMessageView

IMPLEMENT_DYNCREATE(CMessageView, CListView)

CMessageView::CMessageView()
{

}

CMessageView::~CMessageView()
{
}

BEGIN_MESSAGE_MAP(CMessageView, CListView)
    ON_WM_CREATE()
END_MESSAGE_MAP()


// CMessageView-Diagnose

#ifdef _DEBUG
void CMessageView::AssertValid() const
{
    CListView::AssertValid();
}

#ifndef _WIN32_WCE
void CMessageView::Dump(CDumpContext& dc) const
{
    CListView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMessageView-Meldungshandler
BOOL CMessageView::PreCreateWindow(CREATESTRUCT& cs)
{
    cs.style &= ~LVS_TYPEMASK;
    cs.style |= LVS_REPORT | LVS_EDITLABELS | LVS_SHOWSELALWAYS | WS_BORDER | WS_TABSTOP;

    return CListView::PreCreateWindow(cs);
}


void CMessageView::OnUpdate(CView* /*pSender*/, LPARAM lHint, CObject* pHint)
{
    if (lHint == UPDATE_INSERT_MESSAGE_VIEW)
    {
        CListCtrl&ctrl = GetListCtrl();
        CMessageHint* pMessage = DYNAMIC_DOWNCAST(CMessageHint, pHint);
        if (pMessage)
        {
            int fPos = ctrl.InsertItem(ctrl.GetItemCount(), pMessage->mName);
            ctrl.SetItem(fPos, 1, LVIF_PARAM, 0, 0, 0, 0, pMessage->mPosition);
            ctrl.SetItemText(fPos, 1, pMessage->mMessage);
        }
    }
    else if (lHint == UPDATE_CLEAR_MESSAGE_VIEW)
    {
        GetListCtrl().DeleteAllItems();
    }
}


int CMessageView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CListView::OnCreate(lpCreateStruct) == -1)
        return -1;
    CListCtrl&ctrl = GetListCtrl();

    ctrl.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
        LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT,
        LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

    CString string;
    string.LoadString(IDS_NAME);
    ctrl.InsertColumn(0, string, LVCFMT_LEFT, 100);
    string.LoadString(IDS_MESSAGE);
    ctrl.InsertColumn(1, string, LVCFMT_LEFT, 250);
    string.LoadString(IDS_POSITION);
    ctrl.InsertColumn(2, string, LVCFMT_LEFT, 50);


    return 0;
}
