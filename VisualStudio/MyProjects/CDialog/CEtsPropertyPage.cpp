/********************************************************************/
/* Funktionen der Klasse CEtsDialog                                    */
/********************************************************************/
#ifndef _WINDOWS_
   #define  STRICT
   #include <WINDOWS.H>
#endif

#include <commctrl.h>

#include "CEtsPropertyPage.h"
#include "CEtsPropertySheet.h"
#include "AfxRes.h"
#include "Debug.h"

#ifdef INCLUDE_ETS_HELP
   #include "CEtsHelp.h"
#endif

#define STRINGSIZE 256

CEtsPropertyPage::CEtsPropertyPage()
{
   TRACE("CEtsPropertyPage()\n");
   m_bModal         = true;
   m_pParent        = NULL;
   m_pPropSheetPage = NULL;
}

CEtsPropertyPage::~CEtsPropertyPage()
{
   TRACE("~CEtsPropertyPage()\n");
}

bool CEtsPropertyPage::OnInitDialog(HWND hWnd)
{
   if (CEtsDialog::OnInitDialog(hWnd))
   {
      m_hWndParent = GetParent(m_hWnd);
      if ((m_pParent != NULL) &&  m_pParent->OnInitSheet(m_hWndParent))
      {
         if (m_pPropSheetPage)
         {
            if ((m_pPropSheetPage->dwFlags & PSP_DLGINDIRECT) == 0)
            {
               HRSRC   hDlgInit;
               hDlgInit = ::FindResource(m_hInstance, m_pPropSheetPage->pszTemplate, RT_DLGINIT);
               if (hDlgInit!=NULL)
                  ExecuteDlgInit(hDlgInit);
            }
            else
            {
               if (m_pPropSheetPage->pResource) ExecuteDlgInit((void*)m_pPropSheetPage->pResource);
            }
         }
         return true;
      }
   }
   return false;
}

int  CEtsPropertyPage::OnMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
   switch (nMsg)
   {
      case WM_NOTIFY:
      if (lParam)
      {
         NMHDR *pN = (NMHDR*) lParam;
         switch (pN->code)
         {
            case PSN_APPLY:       
               if (!pN->idFrom) pN->idFrom = ID_APPLY_NOW;
               return OnApply((PSHNOTIFY*)lParam, true);
            case PSN_HELP:
               if (!pN->idFrom) pN->idFrom = ID_HELP;
               return OnHelp((PSHNOTIFY*)lParam);
            case PSN_WIZBACK:     
               if (!pN->idFrom) pN->idFrom = ID_WIZBACK;
               return OnWizardBack((PSHNOTIFY*)lParam, true);
            case PSN_WIZFINISH:
               if (!pN->idFrom) pN->idFrom = ID_WIZFINISH;
               return OnWizardFinish((PSHNOTIFY*)lParam, true);
            case PSN_WIZNEXT:
               if (!pN->idFrom) pN->idFrom = ID_WIZNEXT;
               return OnWizardNext((PSHNOTIFY*)lParam  , true);
            case PSN_GETOBJECT:   return OnGetObject((NMHDR*)lParam);
            case PSN_KILLACTIVE:  return OnKillActive((PSHNOTIFY*)lParam  , false);
            case PSN_QUERYCANCEL: return OnQueryCancel((PSHNOTIFY*)lParam , true);
            case PSN_RESET:       return OnReset((PSHNOTIFY*)lParam);
            case PSN_SETACTIVE:   return OnSetActive((PSHNOTIFY*)lParam   , true);
         }
      } break;
      case WM_USER+1:
         ::SetFocus(::GetNextDlgTabItem(m_hWnd, ::GetFocus(), false));
         break;
      case WM_DESTROY:
         OnEndDialog(0);
         break;
      default:
         break;
   }
   return CEtsDialog::OnMessage(nMsg, wParam, lParam);
}

void CEtsPropertyPage::OnEndDialog(int)
{
   if (IsWindow(m_hWnd))
   {
      VERIFY(this == (CEtsDialog*) ::SetWindowLong(m_hWnd, DWL_USER, NULL));
   }
}

UINT CALLBACK CEtsPropertyPage::PropSheetPageProc(HWND hwnd, UINT nMsg, LPPROPSHEETPAGE ppsp)
{
   switch (nMsg)
   {
      case PSPCB_CREATE:
         if (ppsp->lParam == NULL) return 0;
         return 1;
         break;
      case PSPCB_RELEASE:
         break;
   }
   return 0;
}

int  CEtsPropertyPage::OnApply(PSHNOTIFY*pN, bool bOk)
{
   if (bOk)
   {
      ::SetWindowLong(m_hWnd, DWL_MSGRESULT, (LONG)PSNRET_NOERROR);
   }
   else
   {
      ::SetWindowLong(m_hWnd, DWL_MSGRESULT, (LONG)PSNRET_INVALID_NOCHANGEPAGE);
   }
   return 1;
}

int  CEtsPropertyPage::OnGetObject(NMHDR*)
{
   return 0;
}

int  CEtsPropertyPage::OnHelp(PSHNOTIFY*)
{

   return 1;
}

int  CEtsPropertyPage::OnKillActive(PSHNOTIFY*pN, bool bResult)
{
   ::SetWindowLong(m_hWnd, DWL_MSGRESULT, (LONG)bResult);
   return 1;
}

int  CEtsPropertyPage::OnQueryCancel(PSHNOTIFY*, bool bCancel)
{
   if (bCancel)
   {
//      ::RemoveProp(m_hWnd, THIS_POINTER_CDIALOG);
   }
   ::SetWindowLong(m_hWnd, DWL_MSGRESULT, bCancel ? 0 : 1);
   return 1;
}

int  CEtsPropertyPage::OnReset(PSHNOTIFY*)
{
   return 0;
}

int  CEtsPropertyPage::OnSetActive(PSHNOTIFY*, bool bAccept)
{
   ::SetWindowLong(m_hWnd, DWL_MSGRESULT, bAccept ? 0 : -1);
   return 0;
}

int  CEtsPropertyPage::OnWizardBack(PSHNOTIFY*, bool bAccept)
{
   ::SetWindowLong(m_hWnd, DWL_MSGRESULT, bAccept ? 0 : -1);
   return 1;
}

int  CEtsPropertyPage::OnWizardNext(PSHNOTIFY*, bool bAccept)
{
   ::SetWindowLong(m_hWnd, DWL_MSGRESULT, bAccept ? 0 : -1);
   return 1;
}

int  CEtsPropertyPage::OnWizardFinish(PSHNOTIFY*, bool bFinish)
{
   ::SetWindowLong(m_hWnd, DWL_MSGRESULT, bFinish ? 0 : 1);
   return 1;
}

void CEtsPropertyPage::SetParent(CEtsPropertySheet *pThis)
{
   m_pParent = pThis;
}

bool CEtsPropertyPage::SwitchDlgItem(int nIDFocusTest, HWND hWndNext, bool bNext)
{
   HWND hwndFocus     = GetFocus();
   HWND hWndFocusTest = ::GetDlgItem(m_hWndParent, nIDFocusTest);
   if (!hWndFocusTest) hWndFocusTest = ::GetDlgItem(m_hWnd, nIDFocusTest);

   if ((hWndFocusTest != NULL) && (hwndFocus != hWndFocusTest))
   {
      ASSERT(::IsWindow(hWndFocusTest));
      WPARAM wParam;
      LPARAM lParam;
      if (hWndNext)                    // wurde ein gültiges Fensterhandle übegeben
      {
         ASSERT(::IsWindow(hWndNext));
         lParam = 1;                   // zu diesem Fenster springen
         wParam = (WPARAM) hWndNext;
      }
      else                             // sonst
      {
         lParam = 0;                   // zum nächsten bzw, vorhergehenden
         wParam = !bNext;              // TabDlgItem gehen
      }
      ASSERT(::IsWindow(m_hWnd));
      ::SendMessage(m_hWnd, WM_NEXTDLGCTL, wParam, lParam);
      return true;
   }
   return false;
}

void CEtsPropertyPage::SetPageButtonFocus(PSHNOTIFY *pN)
{
   HWND hwnd = ::GetDlgItem(pN->hdr.hwndFrom, pN->hdr.idFrom);
   if (hwnd) ::SetFocus(hwnd);
}

bool CEtsPropertyPage::HasPageButtonFocus(PSHNOTIFY *pN)
{
   HWND hwnd = ::GetDlgItem(pN->hdr.hwndFrom, pN->hdr.idFrom);
   if (hwnd == NULL) return true;
   return (hwnd == ::GetFocus()) ? true : false;
}
