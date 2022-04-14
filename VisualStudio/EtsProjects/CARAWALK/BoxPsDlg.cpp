/********************************************************************/
/* Funktionen der Klasse CBoxPositionDlg                                  */
/********************************************************************/
#include "stdafx.h"
#include "BoxPsDlg.h"
#include "BoxDscr.h"
#include "resource.h"

#define  ETSDEBUG_INCLUDE 
#include "CEtsDebug.h"

#define STRINGSIZE 256

extern HINSTANCE g_hInstance;

CBoxPositionDlg::CBoxPositionDlg()
{
   BEGIN("CBoxPositionDlg()");
   Constructor();
}
CBoxPositionDlg::CBoxPositionDlg(HINSTANCE hInstance, int nID, HWND hwndParent) : CEtsDialog(hInstance, nID, hwndParent)
{
   BEGIN("CBoxPositionDlg(..)");
   Constructor();
}

void CBoxPositionDlg::Constructor()
{
   BEGIN("Constructor");
   ZeroMemory(&m_Bp, sizeof(CARAWALK_BoxPos));
   m_nCurSel           = 0;
   gm_nIDAPPLY         = IDAPPLY;
   m_Boxes.SetDestroyMode(false);
   m_BoxPos.SetDestroyMode(true);
}

CBoxPositionDlg::~CBoxPositionDlg()
{
   BEGIN("~CBoxPositionDlg()");
   SetpBox(NULL);
}

bool CBoxPositionDlg::OnInitDialog(HWND hWnd) 
{
   BEGIN("OnInitDialog");
   CEtsDialog::OnInitDialog(hWnd);
   UpdateNewData();
   return true;
}

void CBoxPositionDlg::CatchBox(CVector *pvEye, CVector *pvDir)
{
   BEGIN("CatchBox");
   if (m_Boxes.GetCounter()>0)
   {
      int      j, nNoOfPos, nSel = 0;
      double   dTemp, dCosMax = -1;
      m_nCurSel = 0;
      CBoxObj *pBox;
      for (pBox=(CBoxObj*)m_Boxes.GetFirst(); pBox!=NULL; pBox=(CBoxObj*) m_Boxes.GetNext())
      {
         nNoOfPos = pBox->GetNoOfPositions();
         for (j=0; j<nNoOfPos; j++)
         {
            CBoxPosition* pBp = pBox->GetpPosition(j);
            CVector vBoxPos(pBp->m_pdPos[0], pBp->m_pdPos[1], pBp->m_pdPos[2]);
            dTemp = Cosinus(*pvDir, (vBoxPos - (*pvEye)));
            if (dTemp > 0)
            {
               if (dTemp > dCosMax)
               {
                  dCosMax   = dTemp;
                  m_nCurSel = nSel;
               }
            }
            nSel++;
         }
      }
   }
}

void CBoxPositionDlg::SetpBox(CBoxObj *pBdd)
{
   BEGIN("SetpBox");
   if ((pBdd != NULL) && (pBdd->GetNoOfPositions() > 0))
   {
      int i, nBoxIndex, nBoxPositions;
      nBoxIndex     = m_Boxes.GetCounter();
      nBoxPositions = pBdd->GetNoOfPositions();
      m_Boxes.ADDHead(pBdd);
      REPORT("Box:%s (%d)", pBdd->GetBoxName(), nBoxIndex);

      for (i=0; i<nBoxPositions; i++)
      {
         BoxPositions *pBP = new BoxPositions;
         m_BoxPos.ADDHead(pBP);
         pBP->nBox      = nBoxIndex;
         pBP->nPosition = i;
      }
   }
   else
   {
      m_Boxes.Destroy();
      m_BoxPos.Destroy();
   }
}

void CBoxPositionDlg::UpdateNewData()
{
   BEGIN("UpdateNewData");
   if (m_Boxes.GetCounter()>0)
   {
      char text[STRINGSIZE];
      int i;
      SendDlgItemMessage(IDC_BOXDLG_BOXCOMBO, CB_RESETCONTENT, 0, 0);

      CBoxObj *pBox;
      for (pBox=(CBoxObj*)m_Boxes.GetFirst(); pBox!=NULL; pBox=(CBoxObj*) m_Boxes.GetNext())
      {
         wsprintf(text,"%s", pBox->GetBoxName());
         for (i=0; i<pBox->GetNoOfPositions(); i++)
            SendDlgItemMessage(IDC_BOXDLG_BOXCOMBO, CB_ADDSTRING, 0, (LPARAM)text);
      }
      SendDlgItemMessage(IDC_BOXDLG_BOXCOMBO, CB_SETCURSEL, m_nCurSel, 0);
      SendDlgItemMessage(IDC_BOXDLG_BOXCOMBO, CB_GETLBTEXT, m_nCurSel, (LPARAM)text);
      SetDlgItemText(    IDC_BOXDLG_BOXCOMBO, text);

      BoxPositions *pBP = (BoxPositions*) m_BoxPos.GetAt(m_nCurSel);
      if (pBP)
      {
         CBoxObj *pBox = (CBoxObj*) m_Boxes.GetAt(pBP->nBox);
         if (pBox)
         {
            pBox->GetPosition(pBP->nPosition, m_Bp);
            pBox->Select(pBP->nPosition, true);
         }
      }
      SetChanged(true);
      CEtsDialog::OnOk(0);
      UpdateParams();
   }
}

int CBoxPositionDlg::OnOk(WORD)
{
   BEGIN("OnOk");
   HWND hwndFocus = GetFocus();

   if ((hwndFocus == GetDlgItem(IDOK)) ||
       (hwndFocus == GetDlgItem(IDAPPLY)) ||
       (hwndFocus == GetDlgItem(IDC_BOXDLG_BOXCOMBO)))
   {
      BoxPositions *pBP = (BoxPositions*) m_BoxPos.GetAt(m_nCurSel);
      if (pBP)
      {
         CBoxObj *pBox = (CBoxObj*) m_Boxes.GetAt(pBP->nBox);
         if (pBox)
         {
            pBox->GetPosition(pBP->nPosition, m_Bp);
         }
         return CEtsDialog::OnOk((WORD)m_nCurSel);
      }
      return 0;
   }
   else
   {
      SetFocus(GetNextDlgTabItem(m_hWnd, hwndFocus, false));
   }
   return 0;
}

void CBoxPositionDlg::UpdateParams()
{
   BEGIN("UpdateParams");
   if (m_nCurSel < m_BoxPos.GetCounter())
   {
      SetDlgItemInt(IDC_BOXDLG_E_THETA, m_Bp.nTheta, true);
      SetDlgItemInt(IDC_BOXDLG_E_PHI  , m_Bp.nPhi  , true);
      SetDlgItemInt(IDC_BOXDLG_C_PSI  , m_Bp.nPsi  , true);
/*
      SendDlgItemMessage(IDC_BOXDLG_C_PSI  , CB_SETCURSEL, (m_Bp.nPsi / 90), 0);
      SetDlgItemDouble(  IDC_BOXDLG_E_XPOS , m_Bp.pdPos[0], 2);
      SetDlgItemDouble(  IDC_BOXDLG_E_YPOS , m_Bp.pdPos[1], 2);
      SetDlgItemDouble(  IDC_BOXDLG_E_ZPOS , m_Bp.pdPos[2], 2);
      int nRotate = m_ppBdd[m_pnBox[m_nCurSel]]->GetRotate();
      HWND hwnd;
      hwnd = GetDlgItem(IDC_BOXDLG_C_PSI);
      if (hwnd)
      {
         if (nRotate&1) EnableWindow(hwnd, false);
         else           EnableWindow(hwnd, true);
      }
      hwnd = GetDlgItem(IDC_BOXDLG_E_THETA);
      if (hwnd)
      {
         if (nRotate&2) EnableWindow(hwnd, false);
         else           EnableWindow(hwnd, true);
      }
*/
   }
   SetChanged(false);
}

int CBoxPositionDlg::OnCancel()
{
   BEGIN("OnCancel");
   BoxPositions *pBP = (BoxPositions*) m_BoxPos.GetAt(m_nCurSel);
   if (pBP)
   {
      CBoxObj *pBox = (CBoxObj*)m_Boxes.GetAt(pBP->nBox);
      if (pBox)
      {
         pBox->Select(pBP->nPosition, false);
         SetChanged(true);
         CEtsDialog::OnOk(0);
      }
   }
   return IDCANCEL;
}

int CBoxPositionDlg::OnCommand(WORD nID , WORD nNotifyCode, HWND hwndControl)
{
//   BEGIN("OnCommand");
   switch(nID)
   {
/****************************************************************************************/
      case IDC_BOXDLG_BOXCOMBO:                    // Combobox
      if (nNotifyCode == CBN_SELCHANGE)
      {
//         REPORT("CBN_SELCHANGE");
         if (m_BoxPos.GetCounter() > 0)
         {
            if (IsChanged())
            {
               if (MessageBox(IDS_VALUES_CHANGED, IDS_MSG_CARA_WALK, MB_YESNO|MB_ICONQUESTION) == IDYES)
               {
                  OnOk(0);
                  SetChanged(false);
               }
            }
            CBoxObj *pBox;
            BoxPositions        *pBP;
            pBP = (BoxPositions*) m_BoxPos.GetAt(m_nCurSel);
            if (pBP)
            {
               pBox = (CBoxObj*)m_Boxes.GetAt(pBP->nBox);
               if (pBox) pBox->Select(pBP->nPosition, false);
            }
            m_nCurSel = SendDlgItemMessage(nID, CB_GETCURSEL, 0, 0);
            bool bUpdate = false;
            pBP = (BoxPositions*) m_BoxPos.GetAt(m_nCurSel);
            if (pBP)
            {
               pBox = (CBoxObj*)m_Boxes.GetAt(pBP->nBox);
               if (pBox)
               {
                  REPORT("%s, %d, %d", pBox->GetBoxName(), pBP->nBox, m_nCurSel);
                  pBox->Select(pBP->nPosition, true);
                  bUpdate = pBox->GetPosition(pBP->nPosition, m_Bp);
               }
            }
            SetChanged(true);
            CEtsDialog::OnOk(0);
            if (bUpdate) UpdateParams();
         }
      } break;
      case ID_CLOSE:
        OnCancel();
        OnEndDialog(0);
        break;
/****************************************************************************************/
/*
      case IDC_BOXDLG_C_PSI:
      if (nNotifyCode == CBN_SELCHANGE)
      {
         m_Bp.nPsi = SendDlgItemMessage(nID, CB_GETCURSEL, 0, 0);
         SetChanged(true);
      }
*/
/****************************************************************************************/
/*
      case IDC_BOXDLG_E_PHI:                       // Phi
      if (nNotifyCode == EN_KILLFOCUS)
      {
         int nValue;
         if (GetDlgItemInt(nID, nValue, true))
         {
            CheckMinMaxInt(nID, 0, 360, true, nValue);
            long nOld = m_Bp.nPhi;
            m_Bp.nPhi = nValue;
            bool bSet = hwndControl != GetDlgItem(nID);
            if (!CheckBoxPosition(bSet)) m_Bp.nPhi = nOld;
         }
      } break;
*/
/****************************************************************************************/
/*
      case IDC_BOXDLG_E_THETA:                     // Theta
      if (nNotifyCode == EN_KILLFOCUS)
      {
         int   nValue;
         if (GetDlgItemInt(nID, nValue, true))
         {
            CheckMinMaxInt(nID, -90, 90, true, nValue);
            long nOld   = m_Bp.nTheta;
            m_Bp.nTheta = nValue;
            bool bSet = hwndControl != GetDlgItem(nID);
            if (!CheckBoxPosition(bSet)) m_Bp.nTheta = nOld;
         }
      } break;
*/
/****************************************************************************************/
/*
      case IDC_BOXDLG_E_XPOS:                      // vor, zurück
      if (nNotifyCode == EN_KILLFOCUS)
      {
         double dOld = m_Bp.pdPos[0];
         if (GetDlgItemDouble(nID, m_Bp.pdPos[0]))
         {
            if (!CheckBoxPosition(false)) m_Bp.pdPos[0] = dOld;
         }
      } break;
*/
/****************************************************************************************/
/*
      case IDC_BOXDLG_E_YPOS:                      // hoch, runter
      if (nNotifyCode == EN_KILLFOCUS)
      {
         double dOld = m_Bp.pdPos[0];
         if (GetDlgItemDouble(nID, m_Bp.pdPos[1]))
         {
            if (!CheckBoxPosition(false)) m_Bp.pdPos[1] = dOld;
         }
      } break;
*/
/****************************************************************************************/
/*
      case IDC_BOXDLG_E_ZPOS:                      // recht, links
      if (nNotifyCode == EN_KILLFOCUS)
      {
         double dOld = m_Bp.pdPos[0];
         if (GetDlgItemDouble(nID, m_Bp.pdPos[2]))
         {
            if (!CheckBoxPosition(false)) m_Bp.pdPos[2] = dOld;
         }
      } break;
*/
/****************************************************************************************/
      case IDOK:    OnOk(0);   break;
      case IDAPPLY: OnApply(); break;
      default:
      return CEtsDialog::OnCommand(nID , nNotifyCode, hwndControl);
   }
   return 0;
}

int CBoxPositionDlg::OnMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
//   BEGIN("OnMessage");
   switch(nMsg)
   {

      case WM_NOTIFY:
      {
/*
         NMHDR *nmHdr = (NMHDR*) lParam;
         switch(wParam)
         {
// ****************************************************************************************

            case IDC_BOXDLG_S_THETA:               // Theta
            {
               OnCommand(IDC_BOXDLG_E_THETA, EN_KILLFOCUS, nmHdr->hwndFrom);
            }
            break;
// ****************************************************************************************
            case IDC_BOXDLG_S_PHI:                 // Phi
            {
               OnCommand(IDC_BOXDLG_E_PHI  , EN_KILLFOCUS, nmHdr->hwndFrom);
            }
            break;
         }
*/
      }
      break;
      default:  return CEtsDialog::OnMessage(nMsg, wParam, lParam);
   }
   return 0;
}

bool CBoxPositionDlg::CheckBoxPosition(bool /* bSet */)
{
   BEGIN("CheckBoxPosition");
/*
   HWND hwndGrandParent = GetParent(m_hWndParent);
   if (IsWindow(hwndGrandParent))
   {
      int rValue = SendMessage(hwndGrandParent, WM_CHECKPOS, BOX_POSITION, (LPARAM)&m_Bp);
      if (rValue!=0)
      {
         SetChanged(true);
         if (bSet) CEtsDialog::OnOk(0);
      }
   }
*/
   return true;
}
