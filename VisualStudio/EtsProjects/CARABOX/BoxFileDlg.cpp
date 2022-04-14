/********************************************************************/
/* Funktionen der Klasse CBoxFileDlg                                */
/********************************************************************/
#ifndef _WINDOWS_
   #define  STRICT
   #include <WINDOWS.H>
#endif

#include <process.h>
#include "BoxFileDlg.h"
#include "FileHeader.h"
#include "BoxPropertySheet.h"
#include "resource.h"
#include "Debug.h"

#define  ETSDEBUG_INCLUDE 
#include "CEtsDebug.h"

CBoxFileDlg::CBoxFileDlg()
{
   CBoxFileDlg::Constructor();
}

CBoxFileDlg::CBoxFileDlg(HINSTANCE hInstance, int nID, HWND hwndParent) : CEtsFileDlg(hInstance, nID, hwndParent)
{
   CBoxFileDlg::Constructor();
}

void CBoxFileDlg::Constructor()
{
   m_pPropertySheet = NULL;
   m_bOpen          = false;
}

CBoxFileDlg::~CBoxFileDlg()
{
   if (m_pPropertySheet)
      m_pPropertySheet->m_hwndChildDlg = NULL;
}

bool CBoxFileDlg::OnInitDialog(HWND hWnd) 
{
   if (CEtsFileDlg::OnInitDialog(hWnd))
   {
//      HWND hwndParent = ::GetParent(hWnd);
      if (m_pPropertySheet) m_pPropertySheet->m_hwndChildDlg = hWnd;
      return true;
   }
   return false;
}

UINT CBoxFileDlg::OnNotify(OFNOTIFY *pNotify)
{
//   REPORT("%d", pNotify->hdr.code);
   switch(pNotify->hdr.code)
   {
      case CDN_FILEOK:
      if (!m_bOpen)                                            // beim speichern
      {
         if (pNotify->lpOFN->lpstrFile[0] != 0)
         {
            int nLen = strlen(pNotify->lpOFN->lpstrFile);
            char * psz = strstr(pNotify->lpOFN->lpstrFile, ".");
            if (psz) psz[0] = 0;
            strcat(pNotify->lpOFN->lpstrFile, ".BDD");
            HANDLE  hFile = CreateFile(pNotify->lpOFN->lpstrFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL);
            pNotify->lpOFN->lpstrFile[nLen] = 0;               // Dateien prüfen
            if (hFile != INVALID_HANDLE_VALUE)
            {
               CFileHeader fh;
               DWORD dwRead;
               ReadFile(hFile, &fh, sizeof(fh), &dwRead, NULL);
               CloseHandle(hFile);
               if (fh.IsType("BSD"))
               {
                  const char *pszComment = fh.GetComment();
                  if (pszComment[0] != '?')                    // Überschreiben von Originaldateien verhindern
                  {
                     MessageBox(IDS_BDD_ORGFILE ,IDS_PROPSHCAPTION, MB_OK|MB_ICONSTOP);
                     SetWindowLong(m_hWnd, DWL_MSGRESULT, 1);
                     return 1;
                  }                                            // vor überschreiben von vorhandenen warnen
                  if (MessageBox(IDS_BDDEXIST, IDS_PROPSHCAPTION, MB_YESNO|MB_ICONWARNING) == IDNO)
                  {
                     SetWindowLong(m_hWnd, DWL_MSGRESULT, 1);
                     return 1;
                  }
               }
            }
         }
         return 0;
      } break;
      case CDN_FOLDERCHANGE:
      {
      } return 0;
      break;
      case CDN_HELP: break;
      case CDN_INITDONE:
         break;
      case CDN_SELCHANGE:
      if (m_bOpen)
      {
         char szFilePathName[_MAX_PATH];
         SendMessage(GetParent(m_hWnd), CDM_GETFILEPATH, _MAX_PATH, (LPARAM)szFilePathName);
         HANDLE  hFile = CreateFile(szFilePathName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL);
         if (hFile != INVALID_HANDLE_VALUE)
         {
            CFileHeader fh;
            DWORD dwRead;
            ReadFile(hFile, &fh, sizeof(fh), &dwRead, NULL);
            if (fh.IsType("BOX"))
            {
               BoxText bt;
               int nDummy;
               m_pPropertySheet->SetVersionText(m_hWnd, IDC_BD_EDT_FILE_VERSION, fh.GetVersion());
               ReadFile(hFile, &nDummy, sizeof(int), &dwRead, NULL);
               if (fh.GetVersion() > 100)             
                  ReadFile(hFile, &nDummy, sizeof(int), &dwRead, NULL);

               ReadFile(hFile, &bt, sizeof(bt), &dwRead, NULL);
               SetDlgItemText(IDC_BD_EDT_DESCRIPTION, bt.szDescrpt);
               SetDlgItemText(IDC_BD_EDT_MANUFACTURER, bt.szTradeMark);
               SetDlgItemText(IDC_BD_EDT_COMMENT, bt.szComment);
               EnableWindow(::GetDlgItem(GetParent(m_hWnd), IDOK), true);
            }
            else if (fh.IsType("BSD"))
            {
               int nTemp;
               char szText[32];
               const char *pszComent = fh.GetComment();
               if (pszComent[0] == '?') *pszComent++;
               wsprintf(szText, "%d.%02d", fh.GetVersion()/100, fh.GetVersion()%100);
               SetDlgItemText(IDC_BD_EDT_FILE_VERSION, szText);
               SetDlgItemText(IDC_BD_EDT_MANUFACTURER, pszComent);
               ::SetFilePointer(hFile, fh.GetLength(), NULL, FILE_CURRENT);
               ReadFile(hFile, &fh, sizeof(fh), &dwRead, NULL);
               ReadFile(hFile, &nTemp, sizeof(int), &dwRead, NULL);// LSType
               ReadFile(hFile, &nTemp, sizeof(int), &dwRead, NULL);// Rotate
               ReadFile(hFile, &szText, 32*sizeof(char), &dwRead, NULL);// Rotate
               SetDlgItemText(IDC_BD_EDT_DESCRIPTION, szText);
               SetDlgItemText(IDC_BD_EDT_COMMENT    , "");
            }
            else
            {
               SetDlgItemText(IDC_BD_EDT_FILE_VERSION, "");
               SetDlgItemText(IDC_BD_EDT_DESCRIPTION , "");
               SetDlgItemText(IDC_BD_EDT_MANUFACTURER, "");
               SetDlgItemText(IDC_BD_EDT_COMMENT     , "");
               EnableWindow(::GetDlgItem(GetParent(m_hWnd), IDOK), false);
               m_pPropertySheet->SetVersionText(GetParent(m_hWnd), stc32, 0);
            }
            CloseHandle(hFile);
            
            ShowBoxFile(szFilePathName);
         }
         else
         {
            EnableWindow(::GetDlgItem(GetParent(m_hWnd), IDOK), false);
         }
      } break;
      case CDN_SHAREVIOLATION:
         break;
      case CDN_TYPECHANGE:
         break;
   }
   return 0;
}

void CBoxFileDlg::ShowBoxFile(char *pszFilePathName)
{
   if (m_pPropertySheet)
   {
      char * psz = strstr(pszFilePathName, ".");
      if (psz) psz[0] = 0;
      strcat(pszFilePathName, ".BDD");
      m_pPropertySheet->ShowThisBox(pszFilePathName);
   }
}

int CBoxFileDlg::OnCommand(WORD nID , WORD nNotifyCode, HWND hwndControl)
{
   if (nID == IDC_OBF_PREVIEW)
   {
      if (m_pPropertySheet)
      {
         m_pPropertySheet->Create3DView();
      }
      return 1;
   }
   return CEtsFileDlg::OnCommand(nID, nNotifyCode, hwndControl);
}
