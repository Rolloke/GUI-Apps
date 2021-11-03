/********************************************************************/
/* Funktionen der Klasse CEtsPropertySheet                          */
/********************************************************************/
#ifndef _WINDOWS_
   #define  STRICT
   #include <WINDOWS.H>
#endif

#include <commctrl.h>

#include "CEtsPropertyPage.h"
#include "CEtsPropertySheet.h"
#include "Debug.h"
#include "AFXRES.H"

#ifdef INCLUDE_ETS_HELP
   #include "CEtsHelp.h"
#endif

#define STRINGSIZE 256
#define THIS_POINTER_CPROPERTY_SHEET  "CEtsPropSheet"

LONG CEtsPropertySheet::gm_nStdWndProc = NULL;

CEtsPropertySheet::CEtsPropertySheet()
{
   TRACE("CEtsPropertySheet()\n");
   ZeroMemory(&m_PS_Header, sizeof(PROPSHEETHEADER));
   m_PS_Header.dwSize      = sizeof(PROPSHEETHEADER);
   m_PS_Header.dwFlags     = PSH_USECALLBACK|PSH_HASHELP;
   m_PS_Header.pfnCallback = PropertySheetProc;
   m_hWnd                  = NULL;
   m_bCaptionAllocated     = false;
}

CEtsPropertySheet::~CEtsPropertySheet()
{
   TRACE("~CEtsPropertySheet()\n");
   FreeCaption();
   FreePages();
}

int CEtsPropertySheet::PropertySheetProc(HWND hWnd, UINT nMsg, LPARAM lParam)
{
   switch (nMsg)
   {
      case PSCB_INITIALIZED:
         if (hWnd)
         {
            ASSERT(::IsWindow(hWnd));
            gm_nStdWndProc = SetWindowLong(hWnd, GWL_WNDPROC, (LONG)CEtsPropertySheet::WindowProc);
         }
         break;
      case PSCB_PRECREATE:
         break;
   }
   return 0;
}

void CEtsPropertySheet::SetCaption(char *pszCaption)
{
   FreeCaption();
   if (pszCaption)
   {
      int nLen = strlen(pszCaption)+1;
      m_PS_Header.pszCaption = new char[nLen];
      strcpy((char*)m_PS_Header.pszCaption, pszCaption);
      m_bCaptionAllocated = true;
   }
}

void CEtsPropertySheet::SetCaption(UINT nID)
{
   FreeCaption();
   m_PS_Header.pszCaption = (LPSTR) MAKEINTRESOURCE(nID);
}

void CEtsPropertySheet::FreeCaption()
{
   if (m_PS_Header.pszCaption)
   {
      if (m_bCaptionAllocated) delete[] (char*)m_PS_Header.pszCaption;
      m_PS_Header.pszCaption = NULL;
      m_bCaptionAllocated    = false;
   }
}

void CEtsPropertySheet::SetNoOfPages(int nPages)
{
   FreePages();
   if (nPages > 0)
   {
      m_PS_Header.ppsp = new PROPSHEETPAGE[nPages];
      if (m_PS_Header.phpage)
      {
         int   i;
         DWORD dwSize = sizeof(PROPSHEETPAGE);
         ZeroMemory((void*)m_PS_Header.ppsp, dwSize * nPages);
         for (i=0; i<nPages; i++)
         {
            ((PROPSHEETPAGE*)m_PS_Header.ppsp)[i].dwSize = dwSize;
         }
         m_PS_Header.nPages = nPages;
      }
   }
}

void CEtsPropertySheet::FreePages()
{
   if (m_PS_Header.phpage)
   {
      delete[] m_PS_Header.phpage;
      m_PS_Header.ppsp   = NULL;
      m_PS_Header.nPages = 0;
   }
}

void CEtsPropertySheet::SetInstanceHandle(HINSTANCE hInstance)
{
   m_PS_Header.hInstance = hInstance;
   if (m_PS_Header.phpage)
   {
      UINT   i;
      for (i=0; i<m_PS_Header.nPages; i++)
      {
         ((PROPSHEETPAGE*)m_PS_Header.ppsp)[i].hInstance = hInstance;
      }
   }
}

bool CEtsPropertySheet::SetPageTitle(UINT nP, LPCSTR pszTitle)
{
   if (/*(nP >= 0) && */(nP < m_PS_Header.nPages))
   {
      ((PROPSHEETPAGE*)m_PS_Header.ppsp)[nP].pszTitle = pszTitle;
      if (pszTitle) ((PROPSHEETPAGE*)m_PS_Header.ppsp)[nP].dwFlags |=  PSP_USETITLE;
      else          ((PROPSHEETPAGE*)m_PS_Header.ppsp)[nP].dwFlags &= ~PSP_USETITLE;
      return true;
   }
   return false;
}

bool CEtsPropertySheet::SetPageIcon(UINT nP, HICON hIcon)
{
   if (/*(nP >= 0) && */(nP < m_PS_Header.nPages))
   {
      ((PROPSHEETPAGE*)m_PS_Header.ppsp)[nP].hIcon   = hIcon;
      ((PROPSHEETPAGE*)m_PS_Header.ppsp)[nP].dwFlags &= ~PSP_USEICONID;
      if (hIcon) ((PROPSHEETPAGE*)m_PS_Header.ppsp)[nP].dwFlags |= PSP_USEHICON;
      else       ((PROPSHEETPAGE*)m_PS_Header.ppsp)[nP].dwFlags &= ~PSP_USEHICON;
      return true;
   }
   if (nP == 0xffffffff)
   {
      m_PS_Header.dwFlags |= PSH_USEHICON;
      m_PS_Header.hIcon    = hIcon;
   }
   return false;
}

bool CEtsPropertySheet::SetPageIcon(UINT nP, LPCSTR pszIcon)
{
   if (/*(nP >= 0) &&*/ (nP < m_PS_Header.nPages))
   {
      ((PROPSHEETPAGE*)m_PS_Header.ppsp)[nP].pszIcon = pszIcon;
      ((PROPSHEETPAGE*)m_PS_Header.ppsp)[nP].dwFlags &= ~PSP_USEHICON;
      if (pszIcon) ((PROPSHEETPAGE*)m_PS_Header.ppsp)[nP].dwFlags |= PSP_USEICONID;
      else         ((PROPSHEETPAGE*)m_PS_Header.ppsp)[nP].dwFlags &= ~PSP_USEICONID;
      return true;
   }
   if (nP == 0xffffffff)
   {
      m_PS_Header.dwFlags |= PSH_USEICONID;
      m_PS_Header.pszIcon    = pszIcon;
   }
   return false;
}

bool CEtsPropertySheet::SetPageTemplate(UINT nP, LPCSTR pszTemplate)
{
   if (/*(nP >= 0) && */(nP < m_PS_Header.nPages))
   {
      m_PS_Header.dwFlags |= PSH_PROPSHEETPAGE;
      ((PROPSHEETPAGE*)m_PS_Header.ppsp)[nP].pszTemplate = pszTemplate;
      ((PROPSHEETPAGE*)m_PS_Header.ppsp)[nP].dwFlags &= ~PSP_DLGINDIRECT;
      return true;
   }
   return false;
}

bool CEtsPropertySheet::SetPageResource(UINT nP, LPCDLGTEMPLATE pResource)
{
   if (/*(nP >= 0) && */(nP < m_PS_Header.nPages))
   {
      ((PROPSHEETPAGE*)m_PS_Header.ppsp)[nP].pResource = pResource;
      if (pResource) ((PROPSHEETPAGE*)m_PS_Header.ppsp)[nP].dwFlags |= PSP_DLGINDIRECT;
      else           ((PROPSHEETPAGE*)m_PS_Header.ppsp)[nP].dwFlags &= ~PSP_DLGINDIRECT;
      return true;
   }
   return false;
}

bool CEtsPropertySheet::SetPropertyPage(UINT nP, CEtsPropertyPage *pEPP)
{
   ASSERT(pEPP != NULL);
   if (/*(nP >= 0) && */(nP < m_PS_Header.nPages))
   {
      ((PROPSHEETPAGE*)m_PS_Header.ppsp)[nP].lParam      = (LPARAM) pEPP;
      ((PROPSHEETPAGE*)m_PS_Header.ppsp)[nP].pfnDlgProc  = CEtsDialog::DialogProc;
      ((PROPSHEETPAGE*)m_PS_Header.ppsp)[nP].dwFlags    |= PSP_USECALLBACK;
      ((PROPSHEETPAGE*)m_PS_Header.ppsp)[nP].pfnCallback = CEtsPropertyPage::PropSheetPageProc;
      pEPP->SetParent(this);
      return true;
   }
   return false;
}

bool CEtsPropertySheet::SetPageDlgProc(UINT nP, DLGPROC DlgProg, LPARAM lParam)
{
   if (/*(nP >= 0) && */(nP < m_PS_Header.nPages))
   {
      ((PROPSHEETPAGE*)m_PS_Header.ppsp)[nP].lParam     = lParam;
      ((PROPSHEETPAGE*)m_PS_Header.ppsp)[nP].pfnDlgProc = DlgProg;
      return true;
   }
   return false;
}

bool CEtsPropertySheet::SetPageHelp(UINT nP, bool bHelp)
{
   if (/*(nP >= 0) && */(nP < m_PS_Header.nPages))
   {
      if (bHelp) ((PROPSHEETPAGE*)m_PS_Header.ppsp)[nP].dwFlags |=  PSP_HASHELP;
      else       ((PROPSHEETPAGE*)m_PS_Header.ppsp)[nP].dwFlags &= ~PSP_HASHELP;
      return true;
   }
   return false;
}

void CEtsPropertySheet::SetHelp(bool bHelp)
{
   if (bHelp) m_PS_Header.dwFlags |=  PSH_HASHELP;
   else       m_PS_Header.dwFlags &= ~PSH_HASHELP;
}

void CEtsPropertySheet::SetApplyButton(bool bApplyBtn)
{
   if (bApplyBtn) m_PS_Header.dwFlags &= ~PSH_NOAPPLYNOW;
   else           m_PS_Header.dwFlags |=  PSH_NOAPPLYNOW;
}

void CEtsPropertySheet::SetWizard(bool bWizard)
{
   if (bWizard) m_PS_Header.dwFlags |=  PSH_WIZARD;
   else         m_PS_Header.dwFlags &= ~PSH_WIZARD;
}

void CEtsPropertySheet::ExtendTitle(bool bExtend)
{
   if (bExtend) m_PS_Header.dwFlags |=  PSH_PROPTITLE;
   else         m_PS_Header.dwFlags &= ~PSH_PROPTITLE;
}

void CEtsPropertySheet::SetStartPage(int nPage)
{
   m_PS_Header.dwFlags &= ~PSH_USEPSTARTPAGE;
   m_PS_Header.nStartPage = nPage;
}

void CEtsPropertySheet::SetStartPage(LPCTSTR pPage)
{
   m_PS_Header.dwFlags |= PSH_USEPSTARTPAGE;
   m_PS_Header.pStartPage = pPage;
}

int CEtsPropertySheet::DoModal(HWND hwndParent)
{
   m_PS_Header.hwndParent = hwndParent;
   m_PS_Header.dwFlags &= ~PSH_MODELESS;
   return PropertySheet(&m_PS_Header);
}

HWND CEtsPropertySheet::Create(HWND hwndParent)
{
   m_PS_Header.hwndParent = hwndParent;
   m_PS_Header.dwFlags |= PSH_MODELESS;
   return (HWND) PropertySheet(&m_PS_Header);
}

void CEtsPropertySheet::SetHeaderBmp(LPCSTR pszHeader)
{
   if (m_PS_Header.dwFlags & PSH_WIZARD)
   {
      m_PS_Header.dwFlags &= ~PSH_WIZARD;
      m_PS_Header.dwFlags |= PSH_WIZARD97;
   }
   m_PS_Header.pszbmHeader = pszHeader;
   m_PS_Header.dwFlags    &= ~PSH_USEHBMHEADER;
   m_PS_Header.dwFlags    |= PSH_HEADER;
}

void CEtsPropertySheet::SetWatermarkBmp(LPCSTR pszWatermark)
{
   if (m_PS_Header.dwFlags & PSH_WIZARD)
   {
      m_PS_Header.dwFlags &= ~PSH_WIZARD;
      m_PS_Header.dwFlags |= PSH_WIZARD97;
   }
   m_PS_Header.pszbmWatermark = pszWatermark;
   m_PS_Header.dwFlags       &= ~PSH_USEHBMWATERMARK;
   m_PS_Header.dwFlags       |= PSH_WATERMARK;
}

void CEtsPropertySheet::SetWizardContextHelp(bool bSet)
{
   if (bSet) m_PS_Header.dwFlags |= PSH_WIZARDCONTEXTHELP;
   else      m_PS_Header.dwFlags &= ~PSH_WIZARDCONTEXTHELP;
}

void CEtsPropertySheet::SetFinishBtn(bool bSet)
{
   if (bSet) m_PS_Header.dwFlags |= PSH_WIZARDHASFINISH;
   else      m_PS_Header.dwFlags &= ~PSH_WIZARDHASFINISH;
}

bool CEtsPropertySheet::OnInitSheet(HWND hWnd)
{
   if (hWnd)
   {
      ASSERT(::IsWindow(hWnd));
      if (m_hWnd == NULL)
      {
         m_hWnd = hWnd;
         ::SetWindowLong(hWnd, GWL_USERDATA, (LONG)this);
      }
      else
      {
         ASSERT(m_hWnd == hWnd);
      }
      return true;
   }
   return false;
}

void CEtsPropertySheet::SetModified(bool bEnable)
{
   if (m_hWnd)
   {
      ASSERT(::IsWindow(m_hWnd));
      HWND hWndApply = ::GetDlgItem(m_hWnd, ID_APPLY_NOW);
      if (hWndApply)
      {
         ASSERT(::IsWindow(hWndApply));
         ::EnableWindow(hWndApply, bEnable);
      }
   }
}

bool CEtsPropertySheet::IsModified()
{
   if (m_hWnd)
   {
      ASSERT(::IsWindow(m_hWnd));
      HWND hWndApply = ::GetDlgItem(m_hWnd, ID_APPLY_NOW);
      if (hWndApply)
      {
         ASSERT(::IsWindow(hWndApply));
         return (::IsWindowEnabled(hWndApply) != 0)? true : false;
      }
   }
   return false;
}

HWND CEtsPropertySheet::GetWndOkBtn()
{
   if (m_hWnd)
   {
      ASSERT(::IsWindow(m_hWnd));
      return GetDlgItem(m_hWnd, IDOK);
   }
   return NULL;
}
HWND CEtsPropertySheet::GetWndNextBtn()
{
   if (m_hWnd)
   {
      ASSERT(::IsWindow(m_hWnd));
      return GetDlgItem(m_hWnd, ID_WIZNEXT);
   }
   return NULL;
}
HWND CEtsPropertySheet::GetWndBackBtn()
{
   if (m_hWnd)
   {
      ASSERT(::IsWindow(m_hWnd));
      return GetDlgItem(m_hWnd, ID_WIZBACK);
   }
   return NULL;
}
HWND CEtsPropertySheet::GetWndFinishBtn()
{
   if (m_hWnd)
   {
      ASSERT(::IsWindow(m_hWnd));
      return GetDlgItem(m_hWnd, ID_WIZFINISH);
   }
   return NULL;
}
HWND CEtsPropertySheet::GetWndHelpBtn()
{
   if (m_hWnd)
   {
      ASSERT(::IsWindow(m_hWnd));
      return GetDlgItem(m_hWnd, IDHELP);
   }
   return NULL;
}
HWND CEtsPropertySheet::GetWndTabControl()
{
   if (m_hWnd)
   {
      ASSERT(::IsWindow(m_hWnd));
      return GetDlgItem(m_hWnd, AFX_IDC_TAB_CONTROL);
   }
   return NULL;
}

LRESULT CALLBACK CEtsPropertySheet::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   switch (uMsg)
   {
      case WM_HELP:
      {
         HWND hWndCurrent = PropSheet_GetCurrentPageHwnd(hWnd);
         if (hWndCurrent)
         {
            ASSERT(::IsWindow(hWndCurrent));
            int nResult = 0;
            ::SendMessage(hWndCurrent, uMsg, (WPARAM)&nResult, lParam);
            if (nResult) return 1;
         }
      } break;
      case WM_DESTROY:
         CallWindowProc((WNDPROC)gm_nStdWndProc, hWnd, uMsg, wParam, lParam);
         ::SetWindowLong(hWnd, GWL_WNDPROC, gm_nStdWndProc);
         ::SetWindowLong(hWnd, GWL_USERDATA, NULL);
         return 1;
      default :
      {
         CEtsPropertySheet *pP = (CEtsPropertySheet*) ::GetWindowLong(hWnd, GWL_USERDATA);
         if (pP) return pP->OnMessage(uMsg, wParam, lParam);
      } break;
   }
   return CallWindowProc((WNDPROC)gm_nStdWndProc, hWnd, uMsg, wParam, lParam);
}
int CEtsPropertySheet::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   return ::CallWindowProc((WNDPROC)gm_nStdWndProc, m_hWnd, uMsg, wParam, lParam);
}

CEtsPropertyPage * CEtsPropertySheet::GetActivePage()
{
   HWND hWndCurrent = PropSheet_GetCurrentPageHwnd(m_hWnd);
   if (hWndCurrent)
   {
      ASSERT(::IsWindow(hWndCurrent));
      CEtsPropertyPage* pDlg = (CEtsPropertyPage*)::GetWindowLong(hWndCurrent, DWL_USER);
      ASSERT(pDlg!=NULL);
      ASSERT(pDlg->m_hWnd == hWndCurrent);
      return pDlg;
   }
   return NULL;
}
