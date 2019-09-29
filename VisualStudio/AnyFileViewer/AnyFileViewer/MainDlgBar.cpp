// MainDlgBar.cpp : implementation file
//

#include "stdafx.h"
#include "AnyFileViewer.h"
#include "MainDlgBar.h"
#include "MainFrm.h"
#include "FormatView.h"
#include "AnyFileViewerView.h"

// CMainDlgBar dialog

IMPLEMENT_DYNAMIC(CMainDlgBar, CDialogBar)
CMainDlgBar::CMainDlgBar(CWnd* /*pParent =NULL*/)
{
}

CMainDlgBar::~CMainDlgBar()
{
}

BEGIN_MESSAGE_MAP(CMainDlgBar, CDialogBar)
   ON_CBN_SELCHANGE(IDC_COMBO_DATA_TYPE, OnCbnSelchangeComboDataType)
   ON_CBN_SELCHANGE(IDC_COMBO_FORMATFILES, OnCbnSelchangeComboFormatfiles)
   ON_EN_CHANGE(IDC_EDT_COLUMNS, OnEnChangeEdtColumns)
   ON_EN_CHANGE(IDC_EDT_OFFSET, OnEnChangeEdtOffset)
   ON_UPDATE_COMMAND_UI(IDC_COMBO_DATA_TYPE, OnUpdateDlgCtrl)
   ON_UPDATE_COMMAND_UI(IDC_EDT_COLUMNS, OnUpdateDlgCtrl)
   ON_UPDATE_COMMAND_UI(IDC_SPIN_COLUMNS, OnUpdateDlgCtrl)
   ON_UPDATE_COMMAND_UI(IDC_EDT_OFFSET, OnUpdateDlgCtrl)
END_MESSAGE_MAP()


// CMainDlgBar message handlers
void CMainDlgBar::OnUpdateDlgCtrl (CCmdUI *pCmdUI)
{
    switch(pCmdUI->m_nID)
    {
        case IDC_COMBO_DATA_TYPE:
        case IDC_EDT_COLUMNS:
        case IDC_SPIN_COLUMNS:
        case IDC_EDT_OFFSET:
            pCmdUI->Enable();
            break;
    }
}

void CMainDlgBar::OnCbnSelchangeComboDataType()
{
   CMainFrame *pF = theApp.GetMainFrame();
   if (pF)
   {
       int nSel = static_cast<int>(SendDlgItemMessage(IDC_COMBO_DATA_TYPE, CB_GETCURSEL, 0, 0));
       nSel = static_cast<int>(SendDlgItemMessage(IDC_COMBO_DATA_TYPE, CB_GETITEMDATA, nSel, 0));
       pF->GetMainPane()->OnSelectChangeDataType(nSel);
   }
}

void CMainDlgBar::OnEnChangeEdtColumns()
{
   CMainFrame *pF = theApp.GetMainFrame();
   if (pF)
   {
      int nColumns = GetDlgItemInt(IDC_EDT_COLUMNS, 0, 0);
      int nResult  = pF->GetMainPane()->OnEnChangeEdtColumns(nColumns);
      if (nColumns != nResult)
      {
         SetDlgItemInt(IDC_EDT_COLUMNS, nResult, 0);
      }
   }
}

void CMainDlgBar::OnEnChangeEdtOffset()
{
   CMainFrame *pF = theApp.GetMainFrame();
   if (pF)
   {
      int nColumns = GetDlgItemInt(IDC_EDT_OFFSET, 0, 0);
      int nResult  = pF->GetMainPane()->OnEnChangeEdtOffset(nColumns);
      if (nColumns != nResult)
      {
         SetDlgItemInt(IDC_EDT_OFFSET, nResult, 0);
      }
   }
}

void CMainDlgBar::OnCbnSelchangeComboFormatfiles()
{
    CComboBox*pCB = (CComboBox*) GetDlgItem(IDC_COMBO_FORMATFILES);
    CMainFrame *pF = theApp.GetMainFrame();
    if (pF && pCB)
    {
        int nCurSel = pCB->GetCurSel();
        CFormatView*pView =  pF->GetFormatPane();
        CString sFile;
        if (nCurSel == 0)
        {
            pView->SetAllFormats();
        }
        else
        {
            pCB->GetLBText(nCurSel, sFile);
            pView->SendMessage(WM_COMMAND, ID_EDIT_CLEAR_ALL);
            pView->LoadFormatFile(theApp.mFormatFilesDir + sFile);
        }
    }
}
