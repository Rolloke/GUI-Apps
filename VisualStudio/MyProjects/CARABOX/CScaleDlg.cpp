/********************************************************************/
/* Funktionen der Klasse CScaleDlg                                */
/********************************************************************/
#include "CScaleDlg.h"
#include "resource.h"
#include "COpenGLView.h"
#include "C3DViewDlg.h"
#include "CCabinet.h"
#include <COMMCTRL.H>

CScaleDlg::CScaleDlg()
{
   Constructor();
}

CScaleDlg::CScaleDlg(HINSTANCE hInstance, int nID, HWND hwndParent) : CEtsDialog(hInstance, nID, hwndParent)
{
   Constructor();
}

void CScaleDlg::Constructor()
{
   m_nWhat      = SCALE_HEIGHT;
   m_nScaleMode = 0;
   m_nMode1     = -1;
   m_nMode2     = -1;
   m_bShow      = false;
   m_bInvDir    = false;
   m_szText[0]  = 0;
}

bool CScaleDlg::OnInitDialog(HWND hWnd) 
{
   if (CEtsDialog::OnInitDialog(hWnd))
   {
      char szText[CAB_NAME_LEN];
      char szRadio1[16];
      char szRadio2[16];
      int i, nCount, j, nSel;
      HWND hwndLst = ::GetDlgItem(::GetParent(hWnd), IDC_3D_LST_CAB);
      if (hwndLst)
      {
         nCount = ::SendMessage(hwndLst, CB_GETCOUNT, 0, 0);
         nSel   = ::SendMessage(hwndLst, CB_GETCURSEL, 0, 0);
         for (i=0, j=0; i<nCount; i++)
         {
            ::SendMessage(hwndLst, CB_GETLBTEXT, i, (LPARAM)szText);
            if (nCount > 2)
            {
               if (nSel == i) SetDlgItemText(IDC_SCA_GRP1, szText);
               else
               {
                  SendDlgItemMessage(IDC_SCA_LST_CAB, CB_ADDSTRING, 0, (LPARAM)szText);
                  if (i==m_nScale) SendDlgItemMessage(IDC_SCA_LST_CAB, CB_SETCURSEL, j, 0);
                  SendDlgItemMessage(IDC_SCA_LST_CAB, CB_SETITEMDATA, j++, i);
               }
            }
            else
            {
               if (nSel     == i) SetDlgItemText(IDC_SCA_GRP1, szText);
               if (m_nScale == i)
               {
                  SetDlgItemText(IDC_SCA_GRP2, szText);
                  ::ShowWindow(GetDlgItem(IDC_SCA_LST_CAB), SW_HIDE);
               }
            }
         }
      }

      for (i=IDC_SCA_R01; i<=IDC_SCA_R14; i++)
      {
         ::SetWindowContextHelpId(GetDlgItem(i), IDC_SCA_R00);
      }

      if (HIWORD(m_nScaleMode) & SCALE_SIDE1 ) m_nMode1 = 0;
      if (HIWORD(m_nScaleMode) & SCALE_SIDE1B) m_nMode1 = 1;
      if (HIWORD(m_nScaleMode) & SCALE_CENTER) m_nMode1 = 2;
      if (HIWORD(m_nScaleMode) & SCALE_SIDE2B) m_nMode1 = 3;
      if (HIWORD(m_nScaleMode) & SCALE_SIDE2 ) m_nMode1 = 4;
      CheckDlgRadioBtn(IDC_SCA_R00, m_nMode1);

      if (m_nScaleMode & SCALE_SIDE1 ) m_nMode2 = 0;
      if (m_nScaleMode & SCALE_SIDE1B) m_nMode2 = 1;
      if (m_nScaleMode & SCALE_CENTER) m_nMode2 = 2;
      if (m_nScaleMode & SCALE_SIDE2B) m_nMode2 = 3;
      if (m_nScaleMode & SCALE_SIDE2 ) m_nMode2 = 4;
      CheckDlgRadioBtn(IDC_SCA_R10, m_nMode2);

      m_bShow = (m_nScaleMode & SHOW_SCALE) ? true : false;
      CheckDlgButton(m_hWnd, IDC_SCA_CK_SHOW, m_bShow ? MF_CHECKED : MF_UNCHECKED);

      m_bInvDir = (m_nScaleMode & SCALE_DIR_R) ? true : false;
      CheckDlgButton(m_hWnd, IDC_SCA_CK_DIR, m_bInvDir ? MF_CHECKED : MF_UNCHECKED);

      if (m_nWhat == SCALE_WIDTH)
      {
         GetDlgItemText(IDC_SCA_R01, szRadio1, NULL, 16);
         GetDlgItemText(IDC_SCA_R11, szRadio2, NULL, 16);

         ::LoadString(m_hInstance, IDS_LEFT, szText, CAB_NAME_LEN);
         SetDlgItemText(IDC_SCA_R00, szText);
         SetDlgItemText(IDC_SCA_R10, szText);
         strcat(szText, szRadio1);
         SetDlgItemText(IDC_SCA_R01, szText);
         SetDlgItemText(IDC_SCA_R11, szText);

         ::LoadString(m_hInstance, IDS_RIGHT, szText, CAB_NAME_LEN);
         SetDlgItemText(IDC_SCA_R04, szText);
         SetDlgItemText(IDC_SCA_R14, szText);
         strcat(szText, szRadio2);
         SetDlgItemText(IDC_SCA_R03, szText);
         SetDlgItemText(IDC_SCA_R13, szText);

         ::GetDlgItemText(m_hWnd, IDC_SCA_CK_DIR, szText, CAB_NAME_LEN);
         nCount = strlen(szText);
         ::LoadString(m_hInstance, IDS_REAR, &szText[nCount], CAB_NAME_LEN-nCount);
         SetDlgItemText(IDC_SCA_CK_DIR, szText);
      }
      else if (m_nWhat == SCALE_DEPTH)
      {
         GetDlgItemText(IDC_SCA_R03, szRadio1, NULL, 16);
         GetDlgItemText(IDC_SCA_R13, szRadio2, NULL, 16);

         ::LoadString(m_hInstance, IDS_FRONT, szText, CAB_NAME_LEN);
         SetDlgItemText(IDC_SCA_R00, szText);
         SetDlgItemText(IDC_SCA_R10, szText);
         strcat(szText, szRadio1);
         SetDlgItemText(IDC_SCA_R01, szText);
         SetDlgItemText(IDC_SCA_R11, szText);

         ::LoadString(m_hInstance, IDS_REAR, szText, CAB_NAME_LEN);
         SetDlgItemText(IDC_SCA_R04, szText);
         SetDlgItemText(IDC_SCA_R14, szText);
         strcat(szText, szRadio2);
         SetDlgItemText(IDC_SCA_R03, szText);
         SetDlgItemText(IDC_SCA_R13, szText);

         ::GetDlgItemText(m_hWnd, IDC_SCA_CK_DIR, szText, CAB_NAME_LEN);
         nCount = strlen(szText);
         ::LoadString(m_hInstance, IDS_LEFT, &szText[nCount], CAB_NAME_LEN-nCount);
         SetDlgItemText(IDC_SCA_CK_DIR, szText);
      }
      else if (m_nWhat == SCALE_ANGLE_PHI)
      {
         ::LoadString(m_hInstance, IDS_SCALE_ANGLE_HEADLINE, szText, CAB_NAME_LEN);
         ::SetWindowText(m_hWnd, szText);

         ::LoadString(m_hInstance, IDS_FRONT, szText, CAB_NAME_LEN);
         SetDlgItemText(IDC_SCA_R00, szText);
         SetDlgItemText(IDC_SCA_R10, szText);
         ::LoadString(m_hInstance, IDS_REAR, szText, CAB_NAME_LEN);
         SetDlgItemText(IDC_SCA_R01, szText);
         SetDlgItemText(IDC_SCA_R11, szText);

         ::LoadString(m_hInstance, IDS_R_MIDDLE, szText, CAB_NAME_LEN);
         SetDlgItemText(IDC_SCA_R02, szText);
         SetDlgItemText(IDC_SCA_R12, szText);

         ::LoadString(m_hInstance, IDS_LEFT, szText, CAB_NAME_LEN);
         SetDlgItemText(IDC_SCA_R03, szText);
         SetDlgItemText(IDC_SCA_R13, szText);
         ::LoadString(m_hInstance, IDS_RIGHT, szText, CAB_NAME_LEN);
         SetDlgItemText(IDC_SCA_R04, szText);
         SetDlgItemText(IDC_SCA_R14, szText);
         ::ShowWindow(GetDlgItem(IDC_SCA_CK_DIR), SW_HIDE);
      }
      else
      {
         ::ShowWindow(GetDlgItem(IDC_SCA_R01), SW_HIDE);
         ::ShowWindow(GetDlgItem(IDC_SCA_R11), SW_HIDE);
         ::ShowWindow(GetDlgItem(IDC_SCA_R03), SW_HIDE);
         ::ShowWindow(GetDlgItem(IDC_SCA_R13), SW_HIDE);

         ::GetDlgItemText(m_hWnd, IDC_SCA_CK_DIR, szText, CAB_NAME_LEN);
         nCount = strlen(szText);
         ::LoadString(m_hInstance, IDS_LEFT, &szText[nCount], CAB_NAME_LEN-nCount);
         SetDlgItemText(IDC_SCA_CK_DIR, szText);
      }
      return true;
   }
   return false;
}

void CScaleDlg::OnEndDialog(int nResult)
{
   CEtsDialog::OnEndDialog(nResult);                              // Funktion der Basisklasse aufrufen
}

int CScaleDlg::OnOk(WORD)
{
   int nLen = 0;
   HWND hwndList = GetDlgItem(IDC_SCA_LST_CAB);

   m_nScaleMode = MAKELONG(SCALE_SIDE1 << m_nMode2, SCALE_SIDE1 << m_nMode1);
   if (m_bShow  ) m_nScaleMode |= SHOW_SCALE;
   if (m_bInvDir) m_nScaleMode |= SCALE_DIR_R;

   ::GetWindowText(m_hWnd, m_szText, 256);
   strcat(m_szText, ":\n");
   nLen = strlen(m_szText);
   GetDlgItemText(IDC_SCA_GRP1, &m_szText[nLen], NULL, 256-nLen);
   strcat(m_szText, ": ");
   nLen = strlen(m_szText);
   GetDlgItemText(IDC_SCA_R00+m_nMode1, &m_szText[nLen], NULL, 256-nLen);
   strcat(m_szText, ", ");
   nLen = strlen(m_szText);
   if (::IsWindowVisible(hwndList)) ::GetWindowText(hwndList, &m_szText[nLen], 256 - nLen);
   else                             GetDlgItemText(IDC_SCA_GRP2, &m_szText[nLen], NULL, 256-nLen);
   strcat(m_szText, ": ");
   nLen = strlen(m_szText);
   GetDlgItemText(IDC_SCA_R10+m_nMode2, &m_szText[nLen], NULL, 256-nLen);
   return IDOK;
}
int CScaleDlg::OnCommand(WORD nID, WORD nNotifyCode, HWND hwndControl)
{
   if (m_hWnd == NULL) return 0;
   if (nNotifyCode == BN_CLICKED)
   {
      switch(nID)
      {
         case IDC_SCA_R00: case IDC_SCA_R01: case IDC_SCA_R02: case IDC_SCA_R03: case IDC_SCA_R04:
            m_nMode1 = GetDlgRadioBtn(IDC_SCA_R00);
            break;
         case IDC_SCA_R10: case IDC_SCA_R11: case IDC_SCA_R12: case IDC_SCA_R13: case IDC_SCA_R14:
            m_nMode2 = GetDlgRadioBtn(IDC_SCA_R10);
            break;
         case IDC_SCA_CK_SHOW:
            if (IsDlgButtonChecked(m_hWnd, nID) == BST_CHECKED) m_bShow = true;
            else                                                m_bShow = false;
            break;
         case IDC_SCA_CK_DIR:
            if (IsDlgButtonChecked(m_hWnd, nID) == BST_CHECKED) m_bInvDir = true;
            else                                                m_bInvDir = false;
            break;
      }
   }
   else if ((nNotifyCode == CBN_SELCHANGE) && (nID == IDC_SCA_LST_CAB))
   {
      m_nScale = SendDlgItemMessage(nID, CB_GETITEMDATA, SendDlgItemMessage(nID, CB_GETCURSEL, 0, 0), 0);
   }

   return CEtsDialog::OnCommand(nID , nNotifyCode, hwndControl);
}