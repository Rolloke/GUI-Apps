/********************************************************************/
/*                                                                  */
/*                     DLL ETSCDPL                                  */
/*                                                                  */
/*     ETS Multimediaplayer für das Abspielen von CD´s und          */
/*     Wavedateien mit Einstellung der Sounkartenparameter für      */
/*     optimale Wiedergabe                                          */
/*                                                                  */
/*     programmed by Rolf Kary-Ehlers                               */
/*                                                                  */
/*     Version 2.2            04.04.2002                            */
/*                                                                  */
/********************************************************************/
// ETSCDPL.cpp : Definiert den Einsprungpunkt für die DLL-Anwendung.
//

#include "stdafx.h"
#include "CCDPLDlg.h"
#include "MixerDlg.h"
#include "resource.h"
#include "EtsRegistry.h"
bool     g_bDoReport = 1;

#define  ETSCDPLAYER   "ETSCDPL"
#define  ETS_DEBUGNAME ETSCDPLAYER
#define  ETSDEBUG_INCLUDE 
#define  ETSDEBUG_CREATEDATA ETS_DEBUGNAME, &g_bDoReport
#include "CEtsDebug.h"

HINSTANCE g_hInstance = NULL;

CMixerDlg *g_pMixer = NULL;

struct FolderCallBackStruct
{
   char pszPath[_MAX_PATH];
   UINT nFlags;
};

struct BrowseFolderStruct
{
   char       *pszFolderPath;
   char       *pszTitle;
   ITEMIDLIST *ppidl;
   UINT        nFlags;
   HWND        hwndParent;
};

#define BROWSE_FOR_FOLDER 1
#define FREE_PIDL         2
#define DO_MESSAGE        3
#define DO_TITLEDLG       4
#define GET_CONTEXT_MENU  5
#define GET_WINDOW_HANDLE 6
#define DO_MIXER_DLG      7

// Prototypen
extern "C"
{
   void Destructor(void*);
   BOOL DoModal(HWND, void*);
   HWND Create(HWND, void*);
   long DoDataExchange(void*, DWORD, int, void *);
   void Destroy(void*);
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID)
{
   switch (ul_reason_for_call)
   {
      case DLL_PROCESS_ATTACH:                                 // Einbinden der DLL in den Adressraum des Prozesses
      {
         g_hInstance = (HINSTANCE)hModule;                     // Instance-Handle global speichern
         DisableThreadLibraryCalls(g_hInstance);
         SetGlobalRegParam(ETSCDPLAYER, NULL, NULL, false);
         HKEY hKey = NULL;
         OpenRegistry(g_hInstance, KEY_READ, &hKey, NULL, &g_bDoReport);
         if (hKey) EtsRegCloseKey(hKey);
      }  break;

      case DLL_PROCESS_DETACH:                                 // Die DLL wird freigegeben
         break;
   }

   return TRUE;
}

void Destructor(void* pParam)
{
   Destroy(pParam);
   CEtsDialog::Destructor();
   ETSDEBUGEND;
}

BOOL DoModal(HWND hwndParent, void*pParam)
{
   CCDPLDlg dlg(g_hInstance, IDD_CD_PLAYER, hwndParent);
   dlg.m_pCDDlgStruct = (CCDPLDlgStruct*)pParam;
   dlg.m_pCDDlgStruct->pThis = NULL;
   return (dlg.DoModal() == IDOK) ? 1 : 0;
}

HWND Create(HWND hwndParent, void*pParam)
{
   CEtsDialog::GetWindowThreadID(hwndParent);
   CCDPLDlgStruct *pDlgSt = (CCDPLDlgStruct*)pParam;
   if (pDlgSt->pThis)
   {
      HWND hwndDlg = pDlgSt->pThis->GetWindowHandle();
      if (hwndDlg && ::IsWindow(hwndDlg))
      {
         pDlgSt->pThis->m_CDPlayer.ClickStop();
         pDlgSt->pThis->InitTitles();
         pDlgSt->pThis->m_TitleDlg.FindCommonPath();
         if (pDlgSt->pThis->m_CDPlayer.m_dwWaveTrack == 1)
            pDlgSt->pThis->m_CDPlayer.m_dwWaveTrack = 0;
         ::SendMessage(hwndDlg, WM_UPDATE_CD_CTRLS, NEW_CD_MEDIA, 0);
         ::SendMessage(hwndDlg, WM_UPDATE_CD_CTRLS, NEW_CD_TRACK, 1);
         if (::IsIconic(hwndDlg))
         {
            pDlgSt->pThis->Create();//::ShowWindow(hwndDlg, SW_SHOW);
         }
         return hwndDlg;
      }
   }
   else
   {
      pDlgSt->pThis = new CCDPLDlg(g_hInstance, IDD_CD_PLAYER, hwndParent);
      pDlgSt->pThis->m_pCDDlgStruct = pDlgSt;
   }

   if (pDlgSt->pThis)
   {
      return pDlgSt->pThis->Create();
   }
   return NULL;
}

long DoDataExchange(void*pDlgParam, DWORD dwType, int nSize, void *pParam)
{
   CCDPLDlgStruct *pDlgSt = (CCDPLDlgStruct*)pDlgParam;
   switch (dwType)
   {
      case BROWSE_FOR_FOLDER:
      if ((pParam != NULL) && (nSize == sizeof(BrowseFolderStruct)))
      {
         BrowseFolderStruct *pBFS = (BrowseFolderStruct*)pParam;
         return (GetFolderPath(pBFS->pszFolderPath, &pBFS->ppidl, pBFS->nFlags, pBFS->pszTitle, pBFS->hwndParent) ? 1 : 0);
      }break;
      case FREE_PIDL:
      if ((pParam != NULL) && (nSize == sizeof(BrowseFolderStruct)))
      {
         BrowseFolderStruct *pBFS = (BrowseFolderStruct*)pParam;
         ::CoTaskMemFree((void*)pBFS->ppidl);
         return 1;
      }break;
      case DO_MESSAGE:
      if ((pParam != NULL) && (nSize == sizeof(MSG)) && (pDlgSt->pThis != NULL))
      {
         MSG *pMsg = (MSG*) pParam;
         HWND hWnd = pDlgSt->pThis->GetWindowHandle();
         LRESULT lResult = 0;
         if (hWnd==NULL)        return 0;
         if (!::IsWindow(hWnd)) return 0;
         if (pMsg->time == 1)
         {
            lResult = ::PostMessage(hWnd, pMsg->message, pMsg->wParam, pMsg->lParam);
         }
         else
         {
            lResult = ::SendMessage(hWnd, pMsg->message, pMsg->wParam, pMsg->lParam);
         }
         pMsg->time = 2; // Dialogfenster ist vorhanden
         return lResult;
      }break;
      case DO_TITLEDLG:
      if ((pParam != NULL) && (nSize == sizeof(BrowseFolderStruct)))
      {
         BrowseFolderStruct *pBFS = (BrowseFolderStruct*)pParam;
         CDTitleDlg dlg(g_hInstance, IDD_CD_TITLE, pBFS->hwndParent);
         dlg.ReadMediaInfo(pBFS->pszFolderPath);
         dlg.m_bEditable = true;
         if (dlg.DoModal() == IDOK)
         {
            dlg.WriteMediaInfo(pBFS->pszFolderPath);
            return 1;
         }
      }break;
      case DO_MIXER_DLG:
      if ((pParam != NULL) && (nSize == sizeof(BrowseFolderStruct)))
      {
         BrowseFolderStruct *pBFS = (BrowseFolderStruct*)pParam;
         if (g_pMixer == NULL) g_pMixer = new CMixerDlg;
         if (g_pMixer)
         {
            g_pMixer->Init(g_hInstance, IDD_MIXER, pBFS->hwndParent);
            if (pBFS->nFlags)
            {
               *((HWND*)pBFS->ppidl) = g_pMixer->Create();
               return 1;
            }
            else
            {
               if (g_pMixer->DoModal() == IDOK)
               {
                  return 1;
               }
            }
         }
      }break;
      case GET_CONTEXT_MENU:
      if ((pParam != NULL) && (nSize == sizeof(HMENU)))
      {
         ((HMENU*)pParam)[0] = ::LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_CONTEXT1));
         return nSize;
      }break;
      case GET_WINDOW_HANDLE:
      if ((pParam != NULL) && (nSize == sizeof(HWND)) && (pDlgSt->pThis != NULL))
      {
         ((HWND*)pParam)[0] = pDlgSt->pThis->GetWindowHandle();
         return nSize;
      }break;
   }
   return false;
}

void Destroy(void*pParam)
{
   if (pParam)
   {
      CCDPLDlgStruct *pDlgSt = (CCDPLDlgStruct*)pParam;
      if (pDlgSt->pThis)
      {
         HWND hwnd = pDlgSt->pThis->GetWindowHandle();
         if (hwnd && ::IsWindow(hwnd)) ::SendMessage(hwnd, WM_DESTROY, 0, 0);
         delete pDlgSt->pThis;
         pDlgSt->pThis = NULL;
      }
   }
}

int RemoveCharacter(char*pszText, char cSign, bool bSearch)
{
   int nlen;
   for (nlen=strlen(pszText); nlen>0; nlen--)
   {
      if (pszText[nlen-1] == cSign)
      {
         pszText[--nlen] = 0;
         break;
      }
      if (!bSearch) break;
   }
   return nlen;
}

int ConcatCharacter(char*pszText, char cSign, bool bOnlyOnce)
{
   int nLen = strlen(pszText);
   if (bOnlyOnce)
   {
      if (pszText[nLen-1] != cSign) pszText[nLen++] = cSign;
   }
   else pszText[nLen++] = cSign;
   pszText[nLen] = 0;
   return nLen;
}

int CALLBACK BrowseFolderCallback(HWND hwndDlg, UINT nMsg, LPARAM /*lParam*/, LPARAM lData)
{
//   BROWSEINFO *pbi = (BROWSEINFO*) lParam;
   switch (nMsg)
   {
      case BFFM_INITIALIZED:
         if (lData)
         {
            FolderCallBackStruct *pfcs = (FolderCallBackStruct*)lData;
            if (pfcs->pszPath)
            {
               ::SendMessage(hwndDlg, BFFM_SETSELECTION, 1, (LPARAM)pfcs->pszPath);
            }
            if (pfcs->nFlags & FOLDERPATH_NO_BROWSER)
            {
               ::SendMessage(hwndDlg, WM_COMMAND, IDOK, 0);
            }
         }
         break;
      case BFFM_SELCHANGED:
         break;
      case BFFM_VALIDATEFAILED:
         break;
   }
/*
      case BFFM_ENABLEOK:
         break;
      case BFFM_SETSELECTION:
         break;
      case BFFM_SETSTATUSTEXT:
         break;
*/
   
   return 0;
}

bool GetFolderPath(char *pszFolderPath, ITEMIDLIST**ppidl, UINT nFlags, LPCTSTR pszTitle, HWND hwndParent)
{
   FolderCallBackStruct fcs = {"", nFlags};
   BROWSEINFO bi;
   bi.hwndOwner = hwndParent;
   
   HRESULT hResult = ::SHGetSpecialFolderLocation(bi.hwndOwner, CSIDL_DESKTOP, (ITEMIDLIST**)&bi.pidlRoot);
   if (hResult == NOERROR)
   {
      strncpy(fcs.pszPath, pszFolderPath, _MAX_PATH);
      RemoveCharacter(fcs.pszPath, '\\', false);
      bi.pszDisplayName = fcs.pszPath;
      bi.lpszTitle      = pszTitle;
      if (nFlags & FOLDERPATH_RETURN_FILES)
         bi.ulFlags     = BIF_BROWSEINCLUDEFILES;
      else
         bi.ulFlags     = BIF_RETURNONLYFSDIRS;
      bi.lpfn           = BrowseFolderCallback;
      bi.lParam         = (long)&fcs;
      bi.iImage         = 0;
      ITEMIDLIST *pidl  = ::SHBrowseForFolder(&bi);
      if (pidl)
      {
         ::SHGetPathFromIDList(pidl, fcs.pszPath);
         if (ppidl)
         {
            if (*ppidl) ::CoTaskMemFree((void*)*ppidl);
            *ppidl = pidl;
         }
         else
         {
            ::CoTaskMemFree((void*)pidl);
         }
         strcpy(pszFolderPath, fcs.pszPath);
         if (nFlags &FOLDERPATH_CONCAT_SLASH) strcat(pszFolderPath, "\\");
         return true;
      }
      ::CoTaskMemFree((void*)bi.pidlRoot);
   }
   return false;
}

int GetColorDiff(DWORD c1, DWORD c2)
{
   return (abs(GetRValue(c1) - GetRValue(c2)) + 
           abs(GetGValue(c1) - GetBValue(c2)) +
           abs(GetBValue(c1) - GetBValue(c2))) / 3;
}
