/********************************************************************/
/* Funktionen der Klasse CProgressDlg                                */
/********************************************************************/
#include "CProgressDlg.h"
#include "resource.h"
#include <COMMCTRL.H>
#include "BoxPropertySheet.h"
#include "Debug.h"

CProgressDlg::CProgressDlg()
{
   Constructor();
}

CProgressDlg::CProgressDlg(HINSTANCE hInstance, int nID, HWND hwndParent) : CEtsDialog(hInstance, nID, hwndParent)
{
   Constructor();
}

void CProgressDlg::Constructor()
{
   m_pPropertySheet = NULL;
}

bool CProgressDlg::OnInitDialog(HWND hWnd) 
{
   if (CEtsDialog::OnInitDialog(hWnd))
   {
      if (m_pPropertySheet == NULL)
      {
         ASSERT(false);
         return false;
      }
      HANDLE hThread = m_pPropertySheet->GetCalcThreadHandle();
      if (hThread != INVALID_HANDLE_VALUE)
      {
         m_pPropertySheet->m_hwndChildDlg = hWnd;
         ::ResumeThread(hThread);
         return true;
      }
   }
   return false;
}

void CProgressDlg::OnEndDialog(int nResult)
{
   if (nResult == IDCANCEL) return;
   CEtsDialog::OnEndDialog(nResult);                              // Funktion der Basisklasse aufrufen
   if (m_pPropertySheet)
      m_pPropertySheet->m_hwndChildDlg = NULL;
}

int CProgressDlg::OnOk(WORD)
{
   return IDOK;
}
int CProgressDlg::OnCommand(WORD nID , WORD nNotifyCode, HWND hwndControl)
{
   if ((nID == IDC_PRGR_BTN_BREAK) && (nNotifyCode == BN_CLICKED))
   {
      DWORD dwExitCode;
      if ((m_pPropertySheet->GetCalcThreadHandle() != INVALID_HANDLE_VALUE) &&
          (GetExitCodeThread(m_pPropertySheet->GetCalcThreadHandle(), &dwExitCode) != 0) &&
          (dwExitCode == STILL_ACTIVE))
      {
          m_pPropertySheet->TerminateThreadSave();
          Sleep(0);
      }
      return 0;
   }
   return CEtsDialog::OnCommand(nID , nNotifyCode, hwndControl);
}