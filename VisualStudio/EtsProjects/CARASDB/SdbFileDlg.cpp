/********************************************************************/
/* Funktionen der Klasse CSdbFileDlg                                */
/********************************************************************/
#ifndef _WINDOWS_
   #define  STRICT
   #include <WINDOWS.H>
#endif

#include "SdbFileDlg.h"
#include "FileHeader.h"
#include "CCaraSdbDlg.h"
#include "resource.h"

CSdbFileDlg::CSdbFileDlg()
{
   CSdbFileDlg::Constructor();
}

CSdbFileDlg::CSdbFileDlg(HINSTANCE hInstance, int nID, HWND hwndParent) : CEtsFileDlg(hInstance, nID, hwndParent)
{
   CSdbFileDlg::Constructor();
}

void CSdbFileDlg::Constructor()
{
   m_pCaraSdbDlg     = NULL;
   m_bDeleteZipFiles = true;
   m_nPreSel         = 0;
}

CSdbFileDlg::~CSdbFileDlg()
{
}

bool CSdbFileDlg::OnInitDialog(HWND hWnd) 
{
   if (CEtsDialog::OnInitDialog(hWnd))
   {
      HWND hwndParent   = ::GetParent(hWnd);
//      m_OF.nFileExtension = m_nPreSel;
//      ::SendDlgItemMessage(hwndParent, cmb1, CB_SETCURSEL, m_nPreSel, 0);
      HWND hwnd1stCkBtn = ::GetDlgItem(hwndParent, chx1);
      if (hwnd1stCkBtn)                                        // Checkboxwindow ermitteln
      {
         DWORD dwC = ::GetWindowContextHelpId(hwnd1stCkBtn);
         ::SetWindowLong(hwnd1stCkBtn, GWL_ID, chx2);
         if (::SendDlgItemMessage(hwndParent, cmb1, CB_GETCOUNT, 0, 0) < 2)
         {                                                     // sind mehrere Dateiendungen vorhanden (.BOX, .ZIP)
            ::ShowWindow(hwnd1stCkBtn, SW_HIDE);               // nein, keine Auswahl
         }
         else                                                  // ja Checkbox für löschen der Zip-Files
         {
            char *str = LoadString(IDS_DELETE_FILES);          // Text ermitteln
            if (str)
            {
               WINDOWPLACEMENT wpl, wpl2;
               
               ::SetWindowText(hwnd1stCkBtn, str);             // Text setzen
               free(str);

               wpl.length = sizeof(WINDOWPLACEMENT);
               ::GetWindowPlacement(hwnd1stCkBtn, &wpl);       // Position ermitteln
               wpl2.length = sizeof(WINDOWPLACEMENT);
               ::GetWindowPlacement(::GetDlgItem(hwndParent, stc2), &wpl2);
               wpl.rcNormalPosition.left = wpl2.rcNormalPosition.left;// vergrößern nach links
               ::GetWindowPlacement(::GetDlgItem(hwndParent, cmb1), &wpl2);
               wpl.rcNormalPosition.right = wpl2.rcNormalPosition.right;// vergrößern nach rechts
               ::SetWindowPlacement(hwnd1stCkBtn, &wpl);       // neue Positon setzen
                                                               // Checkbutton initalisieren
               SendMessage(hwnd1stCkBtn, BM_SETCHECK, m_bDeleteZipFiles ? BST_CHECKED:BST_UNCHECKED, 0);
            }
         }
      }
      return true;
   }
   return false;
}

UINT CSdbFileDlg::OnNotify(OFNOTIFY *pNotify)
{
   switch(pNotify->hdr.code)
   {
      case CDN_FILEOK:
         if (m_pCaraSdbDlg) m_pCaraSdbDlg->SetBoxData(NULL);   // Box im 3D-View aktualisieren
         // Checkbox für ZipFile löschen auswerten.
         m_bDeleteZipFiles = (::SendDlgItemMessage(pNotify->hdr.hwndFrom, chx2, BM_GETCHECK, 0, 0) == BST_CHECKED) ? true : false;
         break;
      case CDN_FOLDERCHANGE:
         break;
      case CDN_HELP:
         break;
      case CDN_INITDONE:
      {
//         char szText[128];
//         ::SendDlgItemMessage(pNotify->hdr.hwndFrom, cmb1, CB_GETLBTEXT, m_nPreSel, (LPARAM)szText);
//         ::SendDlgItemMessage(pNotify->hdr.hwndFrom, cmb1, CB_SELECTSTRING, 0xffffffff, (LPARAM)szText);

//         ::SendDlgItemMessage(pNotify->hdr.hwndFrom, cmb1, CB_SETCURSEL, m_nPreSel, 0);
//         ::SendMessage(m_hWnd, WM_COMMAND, MAKELONG(cmb1, CBN_SELCHANGE), (LPARAM)GetDlgItem(pNotify->hdr.hwndFrom, cmb1));
      } break;
      case CDN_SELCHANGE:
      {
         char szFilePathName[_MAX_PATH];
         SendMessage(pNotify->hdr.hwndFrom, CDM_GETFILEPATH, 256, (LPARAM)szFilePathName);
         strupr(szFilePathName);
         if ((m_pCaraSdbDlg != NULL) && (strstr(szFilePathName, ".BDD")!=NULL))
            m_pCaraSdbDlg->SetBoxData(szFilePathName);
         int nSel = ::SendDlgItemMessage(pNotify->hdr.hwndFrom, cmb1, CB_GETCURSEL, 0, 0);
         // Checkbox für ZipFile löschen, nur wenn Zip-Files aktiv
         ::EnableWindow(::GetDlgItem(pNotify->hdr.hwndFrom, chx2), (nSel == 1) ? true : false);
      } break;
      case CDN_SHAREVIOLATION:
         break;
      case CDN_TYPECHANGE:
      {
         int nSel = ::SendDlgItemMessage(pNotify->hdr.hwndFrom, cmb1, CB_GETCURSEL, 0, 0);
         ::EnableWindow(::GetDlgItem(pNotify->hdr.hwndFrom, chx2), (nSel == 1));
      }break;
   }
   return 0;
}

