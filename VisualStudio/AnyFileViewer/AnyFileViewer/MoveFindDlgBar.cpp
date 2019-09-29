// MoveFindDlgBar.cpp : implementation file
//

#include "stdafx.h"
#include "AnyFileViewer.h"
#include "MainFrm.h"
#include "MoveFindDlgBar.h"
#include "AnyFileViewerView.h"
#include "AnyFileViewerDoc.h"
#include ".\movefinddlgbar.h"


// CMoveFindDlgBar dialog

IMPLEMENT_DYNAMIC(CMoveFindDlgBar, CDialogBar)
CMoveFindDlgBar::CMoveFindDlgBar(CWnd* pParent /*=NULL*/)
{
   pParent = 0;
}

CMoveFindDlgBar::~CMoveFindDlgBar()
{
}

BEGIN_MESSAGE_MAP(CMoveFindDlgBar, CDialogBar)
    ON_WM_CREATE()
    ON_BN_CLICKED(IDC_BTN_GOTO_POSITION, OnBnClickedBtnGotoPosition)
    ON_BN_CLICKED(IDC_BTN_STEP_BY, OnBnClickedBtnStepBy)
    ON_BN_CLICKED(IDC_BTN_RE_STEP_BY, OnBnClickedBtnReStepBy)
    ON_UPDATE_COMMAND_UI(IDC_EDT_GOTO_POSITION, OnUpdateDlgCtrl)
    ON_UPDATE_COMMAND_UI(IDC_BTN_GOTO_POSITION, OnUpdateDlgCtrl)
    ON_UPDATE_COMMAND_UI(IDC_EDT_STEP_BY, OnUpdateDlgCtrl)
    ON_UPDATE_COMMAND_UI(IDC_BTN_STEP_BY, OnUpdateDlgCtrl)
    ON_UPDATE_COMMAND_UI(IDC_BTN_RE_STEP_BY, OnUpdateDlgCtrl)
END_MESSAGE_MAP()

void CMoveFindDlgBar::OnUpdateDlgCtrl (CCmdUI *pCmdUI)
{
    switch(pCmdUI->m_nID)
    {
        case IDC_EDT_GOTO_POSITION:
        case IDC_BTN_GOTO_POSITION:
        case IDC_EDT_STEP_BY:
        case IDC_BTN_STEP_BY:
        case IDC_BTN_RE_STEP_BY:
            pCmdUI->Enable();
            break;
    }
}
// CMoveFindDlgBar message handlers

void CMoveFindDlgBar::OnBnClickedBtnGotoPosition()
{
    CAnyFileViewerView *pView = theApp.GetMainFrame()->GetMainPane();
    CHint cHint(static_cast<size_t>(GetDlgItemInt(IDC_EDT_GOTO_POSITION)));
    pView->GetDocument()->UpdateAllViews(NULL, UPDATE_DATA_POS, &cHint);
}

void CMoveFindDlgBar::OnBnClickedBtnStepBy()
{
    CAnyFileViewerView *pView = theApp.GetMainFrame()->GetMainPane();
    size_t nPos = pView->GetDataPosition() + GetDlgItemInt(IDC_EDT_STEP_BY);
    CHint cHint(nPos);
    pView->GetDocument()->UpdateAllViews(NULL, UPDATE_DATA_POS, &cHint);
}

void CMoveFindDlgBar::OnBnClickedBtnReStepBy()
{
    CAnyFileViewerView *pView = theApp.GetMainFrame()->GetMainPane();
    size_t nPos = pView->GetDataPosition() - GetDlgItemInt(IDC_EDT_STEP_BY);
    if (nPos < 0) nPos = 0;
    CHint cHint(nPos);
    pView->GetDocument()->UpdateAllViews(NULL, UPDATE_DATA_POS, &cHint);
}
