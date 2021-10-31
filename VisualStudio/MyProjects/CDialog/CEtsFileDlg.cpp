/********************************************************************/
/* Funktionen der Klasse CEtsDialog                                 */
/********************************************************************/
#ifndef _WINDOWS_
   #define  STRICT
   #include <WINDOWS.H>
#endif

#include <commctrl.h>

#include "CEtsFileDlg.h"

#include "Debug.h"

CEtsFileDlg::CEtsFileDlg()
{
   Constructor();
}

CEtsFileDlg::CEtsFileDlg(HINSTANCE hInstance, int nID, HWND hwndParent) : CEtsDialog(hInstance, nID, hwndParent)
{
   Constructor();
}

void CEtsFileDlg::Constructor()
{
   ZeroMemory(&m_OF, sizeof(OPENFILENAME));
   m_OF.lStructSize        = sizeof(OPENFILENAME);
   m_OF.hwndOwner          = m_hWndParent;
   m_OF.hInstance          = m_hInstance;
   m_OF.lpstrFilter        = NULL;
   m_OF.lpstrCustomFilter  = NULL;
   m_OF.nMaxCustFilter     = 0;
   m_OF.nFilterIndex       = 0;
   m_OF.nMaxFile           = MAX_PATH*2;
   m_OF.lpstrFile          = (char*)calloc(m_OF.nMaxFile, sizeof(char));
   m_OF.lpstrFileTitle     = NULL;
   m_OF.nMaxFileTitle      = 0;
   m_OF.lpstrInitialDir    = NULL;
   m_OF.lpstrTitle         = NULL;
   m_OF.Flags              = OFN_HIDEREADONLY|OFN_NOCHANGEDIR|OFN_OVERWRITEPROMPT|OFN_EXPLORER|OFN_ENABLEHOOK|OFN_ENABLESIZING;
   m_OF.nFileOffset        = 0;
   m_OF.nFileExtension     = 0;
   m_OF.lpstrDefExt        = NULL;
   m_OF.lCustData          = (DWORD)this;
   m_OF.lpfnHook           = CEtsFileDlg::OFNHookProc;
   m_OF.lpTemplateName     = NULL;
}

CEtsFileDlg::~CEtsFileDlg()
{
   if (m_OF.lpstrFile      ) free(m_OF.lpstrFile);
   if (m_OF.lpstrFileTitle ) free(m_OF.lpstrFileTitle);
   if (m_OF.lpstrFilter    ) free((void*)m_OF.lpstrFilter);
   if (m_OF.lpstrInitialDir) free((void*)m_OF.lpstrInitialDir);
   if (m_OF.lpstrDefExt    ) free((void*)m_OF.lpstrDefExt);
   if (m_OF.lpstrTitle     ) free((void*)m_OF.lpstrTitle);
}

bool CEtsFileDlg::OnInitDialog(HWND hWnd)
{
   if (CEtsDialog::OnInitDialog(hWnd))
   {
      HWND hWndFileName = ::GetDlgItem(::GetParent(hWnd), edt1);
      if (hWndFileName)
      {
         SetWindowLong(hWndFileName, GWL_USERDATA, SetWindowLong(hWndFileName, GWL_WNDPROC, (long)EditFileNameProc));
      }
      return true;
   }
   return false;
}

BOOL CEtsFileDlg::GetOpenFileName()
{
   return ::GetOpenFileName(&m_OF);
}

BOOL CEtsFileDlg::GetSaveFileName()
{
   return ::GetSaveFileName(&m_OF);
}

bool CEtsFileDlg::SetFilter(int nID)
{
   int nLen = 0;
   char *psz = LoadString(nID, &nLen);
   return SetFilter(psz, nLen, false);
}

bool CEtsFileDlg::SetFilter(char *pszFilter, int nLen, bool bAlloc)
{
   int i;
   if (m_OF.lpstrFilter) free((void*)m_OF.lpstrFilter);
   if (bAlloc) m_OF.lpstrFilter = _strdup(pszFilter);
   else        m_OF.lpstrFilter = pszFilter;
   if (m_OF.lpstrFilter)
   {
      char *pszFilter = (char*) m_OF.lpstrFilter;
      for (i=0; i<nLen; i++)
      {
         if (pszFilter[i] == '\n') pszFilter[i] = 0;
      }
      return true;
   }
   return false;
}

bool CEtsFileDlg::SetTitle(int nID)
{
   if (m_OF.lpstrTitle) free((void*)m_OF.lpstrTitle);
   m_OF.lpstrTitle = LoadString(nID);
   return (m_OF.lpstrTitle) ? true : false;
}

bool CEtsFileDlg::SetMaxPathLength(int nSize)
{
   if (nSize > 0)
   {
      if (m_OF.lpstrFile) m_OF.lpstrFile = (char*) realloc(m_OF.lpstrFile, nSize);
      else                m_OF.lpstrFile = (char*) malloc(nSize);
      if (m_OF.lpstrFile) m_OF.nMaxFile  = nSize;
      return true;
   }
   else
   {
      if (m_OF.lpstrFile) free(m_OF.lpstrFile);
      m_OF.lpstrFile = NULL;
      m_OF.nMaxFile  = 0;
      return false;
   }
}

void CEtsFileDlg::SetInitialDir(LPCTSTR pstrInitialDir)
{
   m_OF.lpstrInitialDir = _strdup(pstrInitialDir);
}

void CEtsFileDlg::SetFileTitle(LPCTSTR pszFileTitle)
{
   m_OF.lpstrFileTitle =_strdup(pszFileTitle);
   CopyValidChars(m_OF.lpstrFileTitle, pszFileTitle, strlen(pszFileTitle));
}

void CEtsFileDlg::SetFile(LPCTSTR pszFile)
{
   CopyValidChars(m_OF.lpstrFile, pszFile, m_OF.nMaxFile);
//   strncpy(m_OF.lpstrFile, pszFile, m_OF.nMaxFile);
}

void CEtsFileDlg::CopyValidChars(LPTSTR pszDest, LPCTSTR pszSrc, int nMax)
{
   int i, j = 0;
   for (i=0; i<nMax; i++)
   {
      if (pszSrc[i] == 0) 
         break;
      if      ((pszSrc[i] == ' ') && (j != 0)) pszDest[j++] = pszSrc[i];
      else if (IsValidChar(pszSrc[i]))         pszDest[j++] = pszSrc[i];
   }
}

void CEtsFileDlg::ModifyFlags(DWORD dwSet, DWORD dwRemove)
{
   m_OF.Flags |=  dwSet;
   m_OF.Flags &= ~dwRemove;
}

UINT CALLBACK CEtsFileDlg::OFNHookProc(HWND hwndDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
   if (uiMsg == WM_INITDIALOG)
   {
      CEtsFileDlg *pDlg = (CEtsFileDlg*)((OPENFILENAME*)lParam)->lCustData;
      ((OPENFILENAME*)lParam)->lCustData = ::SetWindowLong(hwndDlg, DWL_USER, (LONG)((OPENFILENAME*)lParam)->lCustData);
      if (pDlg) pDlg->OnInitDialog(hwndDlg);
   }
   else
   {
      CEtsFileDlg *pDlg = (CEtsFileDlg*) ::GetWindowLong(hwndDlg, DWL_USER);
      if (pDlg)
      {
         switch (uiMsg)
         {
            case WM_NOTIFY:
            {
               return pDlg->OnNotify((OFNOTIFY*) lParam);
            } break;
            case WM_DESTROY:
               ::SetWindowLong(hwndDlg, DWL_USER, (LONG)pDlg->m_OF.lCustData);
               break;
            case WM_COMMAND:
               return pDlg->OnCommand(LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
            default:
               return pDlg->OnMessage(uiMsg, wParam, lParam);
               break;
         }
      }
   }
   return 0;
}

UINT CEtsFileDlg::OnNotify(OFNOTIFY *pNotify)
{
   return 0;
}

char * CEtsFileDlg::GetPathName()
{
   if (m_OF.lpstrFile)
   {
      int i;
      char* str = strstr(m_OF.lpstrFile,".");
      char  c   = 0;
      if (str)
      {
         for (i=0; str[i] != m_OF.lpstrFile[0]; i--)
         {
            if (str[i] == '\\')
            {
               c = str[i+1];
               str[i+1] = 0;
               break;
            }
         }
      }
      char *strPath = _strdup(m_OF.lpstrFile);
      if (c) str[i+1] = c;
      return strPath;
   }
   return NULL;
}

void CEtsFileDlg::SetDefaultExt(LPCTSTR pszDefExt)
{
   m_OF.lpstrDefExt = _strdup(pszDefExt);
}

const char * CEtsFileDlg::GetFileTitle()
{
   char *pszFile = NULL;
   if (m_OF.lpstrFileTitle != NULL)
      return m_OF.lpstrFileTitle;

   if (m_OF.lpstrFile && (m_OF.nFileOffset < m_OF.nMaxFile))
   {
      if (m_OF.lpstrFile[m_OF.nFileOffset] != 0)
      {
         pszFile = &m_OF.lpstrFile[m_OF.nFileOffset];
         if (m_OF.Flags & OFN_ALLOWMULTISELECT)
            m_OF.nFileOffset += strlen(&m_OF.lpstrFile[m_OF.nFileOffset])+1;
      }
      else
      {
         if (m_OF.Flags & OFN_ALLOWMULTISELECT)
            m_OF.nFileOffset = strlen(m_OF.lpstrFile)+1;
      }
      if (m_OF.nFileOffset >= (WORD)m_OF.nMaxFile) m_OF.nFileOffset = (WORD)m_OF.nMaxFile-1;
   }
   return pszFile;
}

LRESULT CALLBACK CEtsFileDlg::EditFileNameProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
   if (nMsg == WM_CHAR)
   {
      bool bOk = IsValidChar(wParam);
      switch (wParam)
      {
         case 8:
            bOk = true;       // Backspace Ok
            break;
         case ' ':            // Leerzeichen nicht am Anfang
         {
            DWORD dwStart=0, dwEnd=0;
            ::SendMessage(hWnd, EM_GETSEL, (WPARAM)&dwStart, (LPARAM)&dwEnd);
            if (dwStart == 0)
            {
               bOk = false;
               break;
            }
         }
         default: break;
      }
      if (!bOk)
      {
         ::MessageBeep(0xFFFFFFFF);
         return 0;
      }
   }
   return CallWindowProc((WNDPROC) GetWindowLong(hWnd, GWL_USERDATA), hWnd, nMsg, wParam, lParam);
}

bool CEtsFileDlg::IsValidChar(int nChar)
{
   switch (nChar)
   {
      case (int)'/': case (int)'\\': case (int)':': case (int)'*':
      case (int)'?': case (int)'\"': case (int)'<': case (int)'>': case (int)'|':
      case (int)(0xff&'ä'): case (int)(0xff&'ö'): case (int)(0xff&'ü'):
      case (int)(0xff&'Ä'): case (int)(0xff&'Ö'): case (int)(0xff&'Ü'):
      case (int)(0xff&'ß'):
      return false;
      default:
      if (nChar < (int)' ') return false;
      else                  return true;
   }
}

void CEtsFileDlg::SetTemplateName(UINT nTemplate)
{
   if (nTemplate)
   {
      m_OF.lpTemplateName = MAKEINTRESOURCE(nTemplate);
      m_OF.Flags |= OFN_ENABLETEMPLATE;
   }
   else m_OF.Flags &= ~OFN_ENABLETEMPLATE;
}
