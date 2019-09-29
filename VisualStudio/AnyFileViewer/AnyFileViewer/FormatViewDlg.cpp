// FormatViewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AnyFileViewer.h"
#include "FormatViewDlg.h"
#include "MainFrm.h"
#include "FormatView.h"
#include "AnyFileViewerView.h"
#include "AnyFileViewerTreeView.h"


// CFormatViewDlgBar dialog

IMPLEMENT_DYNAMIC(CFormatViewDlgBar, CDialogBar)
CFormatViewDlgBar::CFormatViewDlgBar(CWnd* pParent /*=NULL*/)
{
   pParent = 0;
}

CFormatViewDlgBar::~CFormatViewDlgBar()
{
}


BEGIN_MESSAGE_MAP(CFormatViewDlgBar, CDialogBar)
   ON_CBN_SELCHANGE(IDC_COMBO_DATA_TYPE_INSERT, OnCbnSelchangeComboDataTypeInsert)
   ON_BN_CLICKED(IDC_BTN_INSERT, OnBnClickedBtnInsert)
   ON_UPDATE_COMMAND_UI(IDC_BTN_INSERT, OnUpdateBtnInsert)
   ON_UPDATE_COMMAND_UI(IDC_EDT_NAME_INSERT, OnUpdateDlgCtrl)
   ON_UPDATE_COMMAND_UI(IDC_COMBO_DATA_TYPE_INSERT, OnUpdateDlgCtrl)
   ON_UPDATE_COMMAND_UI(IDC_EDT_WIDTH_INSERT, OnUpdateVariableType)
   ON_UPDATE_COMMAND_UI(IDC_COMBO_INSERT_POSITION, OnUpdateInsertPosition)
   ON_UPDATE_COMMAND_UI(IDC_SPIN_WIDTH_INSERT, OnUpdateVariableType)
END_MESSAGE_MAP()


void CFormatViewDlgBar::OnUpdateBtnInsert (CCmdUI *pCmdUI)
{
   CMainFrame *pF = theApp.GetMainFrame();
   if (pF)
   {
      CView*pActive = pF->GetActiveView();
      if (pActive)
      {
          if (pActive->IsKindOf(RUNTIME_CLASS(CFormatView)))
          {
              pCmdUI->Enable();
          }
          else if (pActive->IsKindOf(RUNTIME_CLASS(CAnyFileViewerTreeView)))
          {
              pCmdUI->Enable(((CAnyFileViewerTreeView*)pActive)->isStructureView());
          }
          else
          {
              pCmdUI->Enable(FALSE);
          }
      }
   }
}

void CFormatViewDlgBar::OnUpdateVariableType (CCmdUI *pCmdUI)
{
    int nSel = (int)SendDlgItemMessage(IDC_COMBO_DATA_TYPE_INSERT, CB_GETCURSEL, 0, 0);
    pCmdUI->Enable(   nSel == CDisplayType::Ascii 
                   || nSel == CDisplayType::Unicode
                   || nSel == CDisplayType::Binary);
}

void CFormatViewDlgBar::OnUpdateInsertPosition(CCmdUI *pCmdUI)
{
   CMainFrame *pF = theApp.GetMainFrame();
   if (pF)
   {
      CView*pActive = pF->GetActiveView();
      if (pActive)
      {
          if (pActive->IsKindOf(RUNTIME_CLASS(CAnyFileViewerTreeView)))
          {
              pCmdUI->Enable(((CAnyFileViewerTreeView*)pActive)->isStructureView());
              return;
          }
      }
   }
   pCmdUI->Enable(FALSE);
}

// CFormatViewDlgBar message handlers

void CFormatViewDlgBar::OnUpdateDlgCtrl (CCmdUI *pCmdUI)
{
    pCmdUI->Enable();
}

void CFormatViewDlgBar::OnCbnSelchangeComboDataTypeInsert()
{
   LRESULT lItem = SendDlgItemMessage(IDC_COMBO_DATA_TYPE_INSERT, CB_GETCURSEL, 0, 0);
   SetDlgItemInt(IDC_EDT_WIDTH_INSERT, theApp.GetDisplayType(static_cast<unsigned int>(lItem)).GetByteLength(), 0);
}

void CFormatViewDlgBar::OnBnClickedBtnInsert()
{
   CMainFrame *pF = theApp.GetMainFrame();
   if (pF)
   {
      CString sName;
      int nLen = 0;
      int nDataType = (int)SendDlgItemMessage(IDC_COMBO_DATA_TYPE_INSERT, CB_GETCURSEL, 0, 0);
      LRESULT lData = SendDlgItemMessage(IDC_COMBO_DATA_TYPE_INSERT, CB_GETITEMDATA, (WPARAM)nDataType, 0);
      if (lData == CDisplayType::Structure)
      {
          nDataType = CDisplayType::Structure;
          lData = 0;
      }
      if (lData)
      {
         nLen = GetDlgItemInt(IDC_EDT_WIDTH_INSERT, 0, 0);
      }
      GetDlgItemText(IDC_EDT_NAME_INSERT, sName);
      CView*pActive = pF->GetActiveView();
      if (pActive)
      {
          if (pActive->IsKindOf(RUNTIME_CLASS(CFormatView)))
          {
              ((CFormatView*)pActive)->OnInsertDataType(sName, nDataType, nLen);
              ((CFormatView*)pActive)->m_bConcatenated = TRUE;

          }
          if (pActive->IsKindOf(RUNTIME_CLASS(CAnyFileViewerTreeView)))
          {
              int fInsertPosition = (int)SendDlgItemMessage(IDC_COMBO_INSERT_POSITION, CB_GETCURSEL, 0, 0);
              ((CAnyFileViewerTreeView*)pActive)->OnInsertDataType(sName, nDataType, nLen, fInsertPosition);
          }
      }
   }
}

