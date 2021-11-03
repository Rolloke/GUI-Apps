/********************************************************************/
/* Funktionen der Klasse CSPLViewDlg                                */
/********************************************************************/
#include "CSPLViewDlg.h"
#include "resource.h"
#include <COMMCTRL.H>

CSPLViewDlg::CSPLViewDlg()
{
   Constructor();
}

CSPLViewDlg::CSPLViewDlg(HINSTANCE hInstance, int nID, HWND hwndParent) : CEtsDialog(hInstance, nID, hwndParent)
{
   Constructor();
}

void CSPLViewDlg::Constructor()
{
   m_nWhat     = -1;
   m_dDistance = 1.0;
   m_nDistance = 0;
}

bool CSPLViewDlg::OnInitDialog(HWND hWnd) 
{
   if (CEtsDialog::OnInitDialog(hWnd))
   {
      SendDlgItemMessage(IDC_SPL_COMBO_DIAGRAM, CB_SETCURSEL, m_nWhat, 0);
      SendDlgItemMessage(IDC_SPL_DIST_COMBO, CB_SETCURSEL   , m_nDistance, 0);
      return true;
   }
   return false;
}

int CSPLViewDlg::OnOk(WORD)
{
   return IDOK;
}

int CSPLViewDlg::OnCommand(WORD nID , WORD nNotifyCode, HWND hwndControl)
{
   switch(nID)
   {
      case IDC_SPL_COMBO_DIAGRAM:
      if (nNotifyCode == CBN_SELCHANGE)
      {
         m_nWhat = SendDlgItemMessage(nID, CB_GETCURSEL, 0, 0);
      } break;
      case IDC_SPL_DIST_COMBO:
      if (nNotifyCode == CBN_SELCHANGE)
      {
         double dValues[4] = {1.0, 3.0, 5.0, 10.0};
         m_nDistance = SendDlgItemMessage(nID, CB_GETCURSEL, 0, 0);
         m_dDistance = dValues[m_nDistance];
      }break;
      default:
      return CEtsDialog::OnCommand(nID , nNotifyCode, hwndControl);
   }
   return 0;
}
