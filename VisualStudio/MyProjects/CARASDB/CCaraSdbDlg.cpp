/********************************************************************/
/* Funktionen der Klasse CCaraSdbDlg                                */
/********************************************************************/
#ifndef _WINDOWS_
   #define  STRICT
   #include <WINDOWS.H>
#endif

#include <search.h>
#include <SHLOBJ.H>
#include <SHELLAPI.H>
#include <process.h>
#include "CCaraSdbDlg.h"
#include "FileHeader.h"
#include "MyDebug.h"
#include "resource.h"
#include "CCaraMat.h"
#include "CCaraUnzip.h"
#include "CEtsLSMb.h"
#include "Ets3DGL.h"
#include "CustErr.h"
#include "SdbFileDlg.h"
#include "ETSBIND.h"
#include "CEtsHelp.h"

#define  ETSDEBUG_INCLUDE 
#include "CEtsDebug.h"

int   CCaraSdbDlg::gm_nButtonID[NO_OF_BUTTON_ICONS]  = {IDC_PHILEFT, IDC_PHIRIGHT, IDC_THETADOWN, IDC_THETAUP, IDC_PSILEFT, IDC_PSIRIGHT};
int   CCaraSdbDlg::gm_nBoxDescr[NO_OF_SPEAKER_TYPES] = {IDS_MAIN_SPEAKER, IDS_CENTER_SPEAKER_F, IDS_CENTER_SPEAKER_R, IDS_EFFECT_SPEAKER, IDS_ALL_UNITS, IDS_SAT_SUB_SYSTEM, IDS_ADD_ON_SPEAKER};
DWORD CCaraSdbDlg::gm_nBoxtype[ NO_OF_SPEAKER_TYPES] = {    MAIN_SPEAKER,     CENTER_SPEAKER_F,     CENTER_SPEAKER_R,     EFFECT_SPEAKER,     ALL_UNITS,     SAT_SUB_SYSTEM,     ADD_ON_SPEAKER};
const char CCaraSdbDlg::gm_szIniSection[] = "Loudspeaker";

#define PREVIEW_METAFILE      "PMF"
#define BOX_DESCRIPTION_DATA  "BDD"
#define BOX_SOUND_DATA        "BSD"
#define BOX_GEOMETRIC_DATA    "BGD"

CCaraSdbDlg::CCaraSdbDlg()
{
   Constructor();
}
CCaraSdbDlg::CCaraSdbDlg(HINSTANCE hInstance, int nID, HWND hwndParent) : CEtsDialog(hInstance, nID, hwndParent)
{
   Constructor();
}

void CCaraSdbDlg::Constructor()
{
   m_pCaraSdbP        = NULL;
   m_hSelectedBox     = NULL;
   m_pBoxView         = NULL;
   m_bViewDataOk      = true;
   m_nButtonTimer     = 0;
   m_BoxPos.pdPos[0]  = 0;
   m_BoxPos.pdPos[1]  = 0;
   m_BoxPos.pdPos[2]  = 0;
   m_nError           = 0;
   m_hThread          = NULL;
   m_nThreadID        = 0;
   m_hWatchBoxFolder  = NULL;
   m_hWatchBoxFolderThread = NULL;
   m_bWatchBoxInvalid = false;
   m_bCanUpdateBoxView= true;
   m_hwndGLView       = NULL;
   m_Manufacturer.SetDeleteFunction(CCaraSdbDlg::DeleteManufacturerData);
   m_Manufacturer.SetDestroyMode(true);
   m_BoxData.SetDeleteFunction(CCaraSdbDlg::DeleteBoxData);
   m_BoxData.SetDestroyMode(true);

   m_pszOldDownloadPath = NULL;
   ReadDataBase();
}

CCaraSdbDlg::~CCaraSdbDlg()
{
   if (m_pszOldDownloadPath)
   {
      SetDownloadPath(m_pszOldDownloadPath);
      free(m_pszOldDownloadPath);
   }
   SaveDataBase();
   DeleteSdBData();
   if (m_pBoxView) 
   {
      delete m_pBoxView;         // die Dll-Destruktorfunktion aufgerufen werden !!
      m_pBoxView   = NULL;
      m_hwndGLView = NULL;
   }
}

/******************************************************************************
* Name       : DeleteSdBData                                                  *
* Zweck      : Löschen der Datenbankstruktur                                  *
* Definition : 
* Aufruf     : DeleteSdBData();                                               *
* Parameter  :                                                                *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraSdbDlg::DeleteSdBData()
{
   try
   {
      m_Manufacturer.Destroy();
      m_BoxData.Destroy();
   }
   catch (DWORD dwError)
   {
      REPORT("DeleteSdBData Error %x", dwError);
   }
}

bool CCaraSdbDlg::OnInitDialog(HWND hWnd) 
{
   int i;

   CEtsDialog::OnInitDialog(hWnd);
   m_bCanUpdateBoxView = true;

   AutoInitBtnBmp(16, 16);

   m_hSelectedBox     = NULL;                                  // Selektierung auf 0
   m_BoxPos.nPhi      = 30;                                    // Winkel auf sinnvolle Anfangswerte
   m_BoxPos.nTheta    = 0;
   m_BoxPos.nPsi      = 0;

   if (m_pBoxView == NULL)                                     // wenn noch nicht vorhanden
   {
#ifndef _DEBUG
      m_pBoxView = new CCaraWalk;                              // Boxen 3D-View neu anlegen
#endif
   }

   if (m_pBoxView)
   {
      HWND hwndBoxView = GetDlgItem(IDC_BOX_VIEW);
      RECT rcClient;
      GetClientRect(hwndBoxView, &rcClient);
      InflateRect(&rcClient, -1, -1);
      m_hwndGLView = m_pBoxView->Create(BOX_VIEW, 0, &rcClient, hwndBoxView);
   }

   if ((m_hwndGLView == NULL) && (m_pBoxView != NULL))
   {
      delete m_pBoxView;
      m_pBoxView = NULL;
   }

   m_BoxTree.Init(GetDlgItem(IDC_BOX_TREE), IDC_BOX_TREE);     // Baumstrukturansicht
   m_BoxTree.SetbmImageList(IDB_SDB_IMAGES, 16);

   if (m_pCaraSdbP)                                            // Auswahlparameter für die Boxen
   {  
      char  text[128];
      UINT  n, nChoose = 0;
      int   k;
      SourceConfig sc;
      sc.sc = m_pCaraSdbP->Initial;                            // Konfigurationsinitialisierung
      if (sc.sc.bitMain1Front || sc.sc.bitMain2Front||         // Hauptlautsprecher
          sc.sc.bitMain3Rear  || sc.sc.bitMain4Rear )    nChoose |= HIWORD(MAIN_SPEAKER);
                                                               // Centerlautsprecher
      if (sc.sc.bitCenterFront)                          nChoose |= HIWORD(CENTER_SPEAKER_F);
      if (sc.sc.bitCenterRear )                          nChoose |= HIWORD(CENTER_SPEAKER_R);
      if (sc.sc.bitEffect1Front || sc.sc.bitEffect2Front ||    // Effectlautsprecher
          sc.sc.bitEffect3Rear  || sc.sc.bitEffect4Rear) nChoose |= HIWORD(EFFECT_SPEAKER);
      if (sc.sc.bitMain1Front   || sc.sc.bitMain2Front   ||
          sc.sc.bitMain3Rear    || sc.sc.bitMain4Rear    ||    // Mains
          sc.sc.bitEffect1Front || sc.sc.bitEffect2Front ||
          sc.sc.bitEffect3Rear  || sc.sc.bitEffect4Rear  ||    // Effect und
          sc.sc.bitCenterFront  || sc.sc.bitCenterRear)        // Center für Sateliten- Subwoofersysteme
                                                         nChoose |= HIWORD(SAT_SUB_SYSTEM);
      if (sc.sc.bitAddonUnit)                                  // Zusatzlautsprecher
                                                         nChoose |= HIWORD(ADD_ON_SPEAKER);
      if (sc.sc.bitAllUnits)                                   // Beschallungslautsprecher
                                                         nChoose |= HIWORD(ALL_UNITS)|HIWORD(ADD_ON_SPEAKER)|HIWORD(SAT_SUB_SYSTEM);

      for (i=0,n=1, k=0; i<NO_OF_SPEAKER_TYPES; i++, n<<=1)
      {
         if (((n&nChoose) != 0) && ::LoadString(m_hInstance, gm_nBoxDescr[i], text, 128))
         {
            SendDlgItemMessage(IDC_BOX_TYPE , CB_ADDSTRING  , 0, (LPARAM) text);
            SendDlgItemMessage(IDC_BOX_TYPE , CB_SETITEMDATA, (WPARAM)k, (LPARAM) gm_nBoxtype[i]);
            k++;
         }
      }
      m_nMaxSelectableBoxes = 0;                               // Anzahl der Selektierbaren Lautsprecher
      for (i=0,n=1; i<9; i++, n<<=1)                           // ermitteln
      {
         if (sc.sf & n) m_nMaxSelectableBoxes++;
      }
      if (m_nMaxSelectableBoxes == 0) m_nMaxSelectableBoxes = 1;
      if (sc.sc.bitAllUnits)          m_nMaxSelectableBoxes = 8;
   }
   else
   {
      m_nError = CARASDB_NO_PARAMETER;
      return false;
   }

   SendDlgItemMessage(IDC_BOX_TYPE, CB_SETCURSEL, 0, 0);       // Eintrag 0 auswählen
   m_nBoxTypes = SendDlgItemMessage(IDC_BOX_TYPE , CB_GETITEMDATA, 0, 0);

   BuildNewTree();
   UpdateDataBase();                                           // Datenbank aktualisieren

   if (m_nButtonTimer == 0)                                    // Timer starten
   {
      m_nButtonTimer = ::SetTimer(hWnd, BUTTON_TIMER_EVENT, BUTTON_TIMER_DELAY, NULL);
   }
   EnableWindow(GetDlgItem(IDOK), false);

   return true;
}

void CCaraSdbDlg::OnEndDialog(int nResult)
{
   if (m_hWatchBoxFolderThread)
   {
      TerminateThread(m_hWatchBoxFolderThread, 0);
      CloseHandle(m_hWatchBoxFolderThread);
      m_hWatchBoxFolderThread = NULL;
   }

   if (m_hWatchBoxFolder)
   {
      FindCloseChangeNotification(m_hWatchBoxFolder);
      m_hWatchBoxFolder = NULL;
   }

   if (m_nButtonTimer)                                         // Timer beenden
   {
      ::KillTimer(m_hWnd, m_nButtonTimer);
      m_nButtonTimer = 0;
   }
   
   m_BoxTree.DeleteItem();                                     // Einträge des TreeViews löschen
   if (m_nThreadID)
   {
      PostThreadMessage(m_nThreadID, WM_COMMAND, IDC_END_UPDATE_DATABASE_THREAD, 0);
      for (;;)
      {
         DWORD dwExitCode;                                     // und auf die Beendigung warten
         if (GetExitCodeThread(m_hThread, &dwExitCode) && (dwExitCode == STILL_ACTIVE))
         {
            Sleep(0);
         }
         else
            break;
      }
      if (m_hThread)
      {
         ::CloseHandle(m_hThread);
         m_hThread = NULL;
      }
   }

   DeleteAutoBtnBmp();

   CEtsDialog::OnEndDialog(nResult);                              // Funktion der Basisklasse aufrufen
}

int CCaraSdbDlg::OnOk(WORD)
{
   HWND hwndFocus = GetFocus();

   if (hwndFocus == GetDlgItem(IDOK))
   {
      if (m_hSelectedBox)                                      // ist eine Box ausgewählt
      {                                                        // Daten kopieren
         TVITEM   tvItem;
         HWND     hwndTV = GetDlgItem(IDC_BOX_TREE);
         BoxData *pBox = NULL;
         UINT     i, n;
         SourceConfig sc, *pSelect;
         ZeroMemory(&tvItem, sizeof(TVITEM));
         tvItem.hItem = m_hSelectedBox;
         tvItem.mask  = TVIF_IMAGE|TVIF_PARAM|TVIF_HANDLE;
         SendMessage(hwndTV, TVM_GETITEM, 0, (LPARAM)&tvItem);
         switch (tvItem.iImage)
         {
            case IMAGE_BOXES: case IMAGE_USER_BOXES:
               if (tvItem.lParam == NULL) return 0;
               pBox = (BoxData*)tvItem.lParam;                  // 
               break;
            case IMAGE_SLAVES: case IMAGE_SLAVES_MAIN: case IMAGE_SLAVES_CENTER_F: case IMAGE_SLAVES_CENTER_R: case IMAGE_SLAVES_EFFECT:
               tvItem.hItem = TreeView_GetParent(hwndTV, tvItem.hItem);
               SendMessage(hwndTV, TVM_GETITEM, 0, (LPARAM)&tvItem);
               if (tvItem.lParam == NULL) return 0;
               pBox = (BoxData*)tvItem.lParam;
               break;
            default:
               return 0;
         }

         strcpy(m_pCaraSdbP->szFileName[0], pBox->szName);
         sc.sc = m_pCaraSdbP->Initial;
         if      (m_nBoxTypes ==   MAIN_SPEAKER  ) sc.sf = sc.sf &   MAIN_SPEAKER_BITS;
         else if (m_nBoxTypes == CENTER_SPEAKER_F) sc.sf = sc.sf & CENTER_SPEAKER_F_BITS;
         else if (m_nBoxTypes == CENTER_SPEAKER_R) sc.sf = sc.sf & CENTER_SPEAKER_R_BITS;
         else if (m_nBoxTypes == EFFECT_SPEAKER  ) sc.sf = sc.sf & EFFECT_SPEAKER_BITS;
         else if (m_nBoxTypes == ADD_ON_SPEAKER  ) sc.sf = sc.sf & ADD_ON_SPEAKER_BITS;
         else if (m_nBoxTypes == ALL_UNITS       ) sc.sf = sc.sf &      ALL_UNITS_BITS;

         pSelect = (SourceConfig*) &m_pCaraSdbP->Selected[0];
         ZeroMemory(m_pCaraSdbP->Selected, sizeof(SourceConfig)*CARASDB_MAX_BOXES);
         for (i=0,n=1; i<NO_OF_SPEAKER_BITS; i++, n<<=1)
         {
            if (sc.sf & n)
            {
               pSelect->sf =  n;                               // erstes Bit setzen
               sc.sf      &= ~n;                               // und initial löschen
               break;
            }
         }
         m_pCaraSdbP->nCount = 1 + pBox->nFiles;               // Anzahl der Slaves übergeben
         if (m_pCaraSdbP->nCount > 1)                          // mehrere Lautsprecher nur bei Sub-Sat-System
         {
            int nCount  = 1;
            sc.sf      |= pSelect->sf;                         // erstes Bit wieder setzen
            pSelect->sf = 0;                                   // beim Master sind alle Bits 0
            if (!CheckMasterConfig(tvItem.hItem, (m_pCaraSdbP->Initial.bitAllUnits) ? GET_ALL_UNITS : GET_CROSS_REFERENCE, &nCount))
            {
               MessageBox(IDS_NO_CROSS_REFERENCE_T, IDS_NO_CROSS_REFERENCE_H, MB_OK|MB_ICONWARNING|MB_SYSTEMMODAL);
               TreeView_Expand(GetDlgItem(IDC_BOX_TREE), m_hSelectedBox, TVE_EXPAND);
               return 0;
            }
         }
         if (m_pBoxView)
            m_pBoxView->DoDataExchange(CARAWALK_SETBOXDATA, 0, NULL);
         m_bCanUpdateBoxView = false;
         return CEtsDialog::OnOk(0);
      }
   }
   else
   {
      for (int i=0; i<NO_OF_BUTTON_ICONS; i++)
      {
         if (hwndFocus == GetDlgItem(gm_nButtonID[i]))
         {
            OnCommand(gm_nButtonID[i], 0, 0);
            return 0;
         }
      }
      SetFocus(GetNextDlgTabItem(m_hWnd, hwndFocus, false));
   }
   return 0;
}


int CCaraSdbDlg::OnCancel()
{
   if (m_pBoxView)
      m_pBoxView->DoDataExchange(CARAWALK_SETBOXDATA, 0, NULL);
   m_bCanUpdateBoxView = false;
   return IDCANCEL;
}

int CCaraSdbDlg::OnCommand(WORD nID , WORD nNotifyCode, HWND hwndControl)
{
   switch(nID)
   {
/****************************************************************************************/
      case IDM_UPDATE_DATABASE:
      {
         TreeView_DeleteAllItems(GetDlgItem(IDC_BOX_TREE));
         DeleteSdBData();
         UpdateDataBase();
      } break;
      case IDM_LOAD_BOXFILES:
      {
         LoadBoxFiles();
      } break;
      case IDM_BOX_FROM_INTERNET:
      {
#ifdef AFX_TARG_DEU
         char szPath[] = "http://www.cara.de/GER/boxindex.html";
#endif
#ifdef AFX_TARG_ENU
         char szPath[] = "http://www.cara.de/ENU/boxindex.html";
#endif
         GetDownloadPath();
         char  szLsBoxPath[_MAX_PATH];
         GetLsBoxPath(szLsBoxPath);                            // Lautsprecherpfad ermitteln
/*
         SHELLEXECUTEINFO shi = {sizeof(SHELLEXECUTEINFO), 
            0,                                                 // Flags
            m_hWnd, "open", szPath,                            // Aufruf der Seite (E)
            NULL,                                              // Parameters (E)
            szLsBoxPath,                                       // Directory (E)
            SW_SHOW,                                           // Show Command (E)
            NULL,                                              // Instance (A)
            NULL,                                              // IDList
            NULL,                                              // lpClass
            NULL,                                              // keyClass
            0,                                                 // HotKey
            NULL,                                              // {Icon, Monitor}
            NULL};                                             // Processhandle


         if (::ShellExecuteEx(&shi) && (((int)shi.hInstApp) > 32))
         {
            HWND hwnd = ::GetForegroundWindow();
            PostMessage(m_hWnd, WM_COMMAND, IDM_BOX_SHOW_BROWSER, (LPARAM)hwnd);
            SendMessage(m_hWnd, WM_COMMAND, IDM_LOAD_BOXFILES, NULL);
         }
*/
         if (32 <= (int)::ShellExecute(m_hWnd, "open", szPath, NULL, NULL, SW_SHOW))
         {
            HWND hwnd = ::GetForegroundWindow();
            PostMessage(m_hWnd, WM_COMMAND, IDM_BOX_SHOW_BROWSER, (LPARAM)hwnd);
            SendMessage(m_hWnd, WM_COMMAND, IDM_LOAD_BOXFILES, NULL);
         }
      } break;
      case IDM_BOX_SHOW_BROWSER:
      if (hwndControl)
      {
         ::SetForegroundWindow(hwndControl);
      }break;
      case IDC_BOX_TYPE:                    // Combobox
      if (nNotifyCode == CBN_SELCHANGE)
      {
         OnSelectChangeBoxType(hwndControl);
      } break;
      case IDC_PSILEFT:
         m_BoxPos.nPsi += 90;
         if (m_BoxPos.nPsi  >  360) m_BoxPos.nPsi  = 90;
         UpdateAngle();
         break;
      case IDC_PSIRIGHT:
         m_BoxPos.nPsi -= 90;
         if (m_BoxPos.nPsi  <    0) m_BoxPos.nPsi = 270;
         UpdateAngle();
         break;
      case IDC_UPDATE_DATABASE:
         UpdateDataBase();
         break;
      case IDC_END_UPDATE_DATABASE_THREAD:                     // wird am Ende des UpdateDataBaseThreads aufgerufen
         if (m_hThread) ::CloseHandle(m_hThread);
         m_hThread          = NULL;
         m_nThreadID        = 0;
         if (m_BoxTree.GetItemCount() == 0)
            BuildNewTree();
         if (m_hWatchBoxFolderThread == 0)                     // Überwachung starten
         {
            char  szBoxFolderPath[MAX_PATH];
            GetLsBoxPath(szBoxFolderPath);
            int nLength = strlen(szBoxFolderPath)-1;
            szBoxFolderPath[nLength] = 0;
            m_hWatchBoxFolder = FindFirstChangeNotification(szBoxFolderPath, false,
                                                            FILE_NOTIFY_CHANGE_FILE_NAME|
                                                            FILE_NOTIFY_CHANGE_LAST_WRITE);
            if (m_hWatchBoxFolder != INVALID_HANDLE_VALUE)
            {
               unsigned int nThreadId;
               m_hWatchBoxFolderThread = (HANDLE) _beginthreadex(NULL, 0, CCaraSdbDlg::WatchBoxFolderThread, this, 0, &nThreadId);
               if (m_hWatchBoxFolderThread)
               {
                  int npr = GetThreadPriority(m_hWatchBoxFolderThread);
                  SetThreadPriority(m_hWatchBoxFolderThread, THREAD_PRIORITY_LOWEST);
               }
            }
         }
         break;
      default:
         if (nID>IDM_BOX_TYPE_SELECT)
         {
            HWND hwndBoxType = GetDlgItem(IDC_BOX_TYPE);
            int nCount = SendMessage(hwndBoxType, CB_GETCOUNT, 0, 0);

            if ((nID <(IDM_BOX_TYPE_SELECT+nCount)) &&
                (SendMessage(hwndBoxType, CB_SETCURSEL, nID-IDM_BOX_TYPE_SELECT, 0) != CB_ERR))
            {
               OnSelectChangeBoxType(hwndBoxType);
            }
         }
      return CEtsDialog::OnCommand(nID , nNotifyCode, hwndControl);
   }

   return 0;
}

int CCaraSdbDlg::OnMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
   switch(nMsg)
   {
      case WM_NOTIFY:
      {
//         TRACE("WM_NOTIFY(%d, %d)\n", wParam, lParam);
         switch(wParam)
         {
            case IDC_BOX_TREE:                                 // Auswahl der Box
            {
               switch (((LPNMHDR)lParam)->code)
               {
                  case TVN_SELCHANGED: 
                     if (m_bCanUpdateBoxView) OnSelectChangedBox((LPNM_TREEVIEW)lParam);
                     break;
/*
                  case NM_CUSTOMDRAW:
                  {
                     NMTVCUSTOMDRAW *pCustDraw = (NMTVCUSTOMDRAW*) lParam;
                     REPORT("Ds : %d",pCustDraw->nmcd.dwDrawStage);
                     if (pCustDraw->nmcd.dwDrawStage == CDDS_PREPAINT)
                     {
                        ::SetTextColor(pCustDraw->nmcd.hdc, RGB(255, 0, 0));
                        return CDRF_NOTIFYPOSTPAINT;
                     }
                     if (pCustDraw->nmcd.dwDrawStage & CDDS_ITEMPREPAINT)
                     {
                        pCustDraw->clrText = RGB(255, 0, 0);
                        return CDRF_NEWFONT;
                     }
                     TRACE_NOTIFICATIONMESSAGE(((LPNMHDR)lParam)->code);
                     return CDRF_DODEFAULT;
                  } break;
*/
               }
            } break;
         }
      } break;
      case WM_TIMER: return OnTimer(wParam);
      case WM_DROPFILES:
      {
         OnDropFiles((HDROP) wParam);
      } break;
      case WM_CONTEXTMENU:
      {
         POINT ptMouse;
         POINTSTOPOINT(ptMouse, lParam);
         OnContextMenu((HWND) wParam, ptMouse);
      } break;
      case WM_SYSCOLORCHANGE:
      {
         SendDlgItemMessage(IDC_BOX_TREE, nMsg, wParam, lParam);
         m_BoxTree.SetbmImageList(IDB_SDB_IMAGES, 16);
      }break;
      case WM_ENTERSIZEMOVE: case WM_EXITSIZEMOVE:
      if (m_hwndGLView)
      {
         ::SendMessage(m_hwndGLView, nMsg, wParam, lParam);
      } break;
      case WM_HELP:
      {
         HELPINFO *pHI = (HELPINFO*) lParam;
         if (pHI->iCtrlId == IDC_BOX_TREE)
         {
            TVHITTESTINFO TvHt = {{pHI->MousePos.x, pHI->MousePos.y}, 0, NULL};
            TVITEM tvItem;
            HWND hwndTree = GetDlgItem(pHI->iCtrlId);
            ZeroMemory(&tvItem, sizeof(TVITEM));
            tvItem.mask  = TVIF_IMAGE|TVIF_HANDLE|TVIF_PARAM;
            ScreenToClient(hwndTree, &TvHt.pt);
            tvItem.hItem = TreeView_HitTest(hwndTree, &TvHt);

            TreeView_GetItem(hwndTree, &tvItem);
            if ((tvItem.iImage == IMAGE_MANUFACTURER) || (tvItem.iImage == IMAGE_USER_DEFINED) || (tvItem.iImage == IMAGE_NONAME))
            {
               Manufacturer *pM = (Manufacturer*)tvItem.lParam;
               if (pM)
               {
                  char *str = LoadString(IDS_MANUFACTURER_INFO);
                  if (str)
                  {
                     char text[256];
                     char *pszName = pM->szName;
                     int   nBoxes  = 0;
                     if (tvItem.iImage != IMAGE_MANUFACTURER) pszName = &pM->szName[1];
                     if (pM->pBoxes)                          nBoxes  = pM->pBoxes->GetCounter();
                     wsprintf(text, str, pszName, nBoxes);
                     CEtsHelp::CreateContextWnd(text, pHI->MousePos.x, pHI->MousePos.y);
                     ::free(str);
                     return 1;
                  }
               }
            }
            else if ((tvItem.iImage >= IMAGE_NONAME) && (tvItem.iImage <= IMAGE_USER_BOXES))
            {
               BoxData*pData = (BoxData*)tvItem.lParam;
               if (pData)
               {
                  char szText[256];
                  char szFormat[256];
                  char szType[64];
                  char szUse[64];
                  char szUseAs[64];
                  int nIDUse = IDS_USE_AS;
                  ::LoadString(m_hInstance, IDS_LOUDSPEAKER_HELPTEXT, szFormat, 256);
                  ::LoadString(m_hInstance, IDS_LS_TYPE_NONAME+tvItem.iImage, szUseAs, 64);
                  switch (tvItem.iImage)
                  {
                     case IMAGE_NONAME: case IMAGE_MANUFACTURER: case IMAGE_USER_DEFINED: case IMAGE_USER_BOXES:
                        nIDUse = IDS_MANUFACTURER;
                        break;
                  }
                  ::LoadString(m_hInstance, nIDUse, szUse, 64);
                  ::LoadString(m_hInstance, IDS_LS_TYPE_STAND_ALONE+pData->nType-1, szType, 64);
                  wsprintf(szText, szFormat, pData->szName, pData->szDescription, szUse, szUseAs, szType);
                  CEtsHelp::CreateContextWnd(szText, pHI->MousePos.x, pHI->MousePos.y);
                  return 1;
               }
            }
         }
      }
      default: return CEtsDialog::OnMessage(nMsg, wParam, lParam);
   }
   return 0;
}

/******************************************************************************
* Name       : OnContextMenu                                                  *
* Zweck      : Aufrufen eines Contextmenus                                    *
* Definition : 
* Aufruf     : OnContextMenu(hwnd, ptScreen);                                 *
* Parameter  :                                                                *
* hwnd    (E): Aufrufendes Fenster                                   (HWND)   * 
* ptScreen(E): Mauscursorposition in Screencoordinatens              (POINT)  *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraSdbDlg::OnContextMenu(HWND hwnd, POINT &ptScreen)
{
   if (hwnd == GetDlgItem(IDC_BOX_TREE))                       // ist es der TreeView
   {
   // TRACE("OnContextMenu()\n");
      TVHITTESTINFO TvHt = {{ptScreen.x, ptScreen.y}, 0, NULL};
      HTREEITEM hSelItem = TreeView_GetSelection(hwnd);
      TVITEM tvItem;
      ZeroMemory(&tvItem, sizeof(TVITEM));
      tvItem.mask  = TVIF_IMAGE|TVIF_HANDLE|TVIF_PARAM;
      if ((ptScreen.x == -1) && (ptScreen.y == -1))
      {
         RECT rcItem;
         if (hSelItem && TreeView_GetItemRect(hwnd, hSelItem, &rcItem, true))
         {
            ptScreen.x = rcItem.left;
            ptScreen.y = rcItem.bottom;
            ::ClientToScreen(hwnd, &ptScreen);
         }
      }
      else
      {
         ScreenToClient(hwnd, &TvHt.pt);
         HTREEITEM hHitItem = TreeView_HitTest(hwnd,&TvHt);
         if ((hHitItem != NULL) && ((TvHt.flags&(TVHT_ONITEMLABEL|TVHT_ONITEMICON)) != 0))
         {
            if (hHitItem != hSelItem)
            {
               TreeView_SelectItem(hwnd, hHitItem);
               hSelItem = hHitItem;
            }
         }
         else hSelItem = NULL;
      }
      if (hSelItem)
      {
         tvItem.hItem = hSelItem;
         TreeView_GetItem(hwnd, &tvItem);
         bool   bChangeCrossReference = true;
         if ((tvItem.iImage == IMAGE_BOXES) || (tvItem.iImage == IMAGE_USER_BOXES))
         {
            HMENU    hMenu;
            HMENU    context;
            hMenu   = LoadMenu(m_hInstance, MAKEINTRESOURCE(IDR_CONTEXT_DELETE_BOXFILE1));
            context = GetSubMenu(hMenu, 0);
            EnableMenuItem(context, IDM_DELETEMATERIAL, MF_BYCOMMAND|( (((BoxData*)tvItem.lParam)->nMaterial != 0) ? 0 : MF_GRAYED) );
            switch (TrackPopupMenu(context, TPM_RETURNCMD|TPM_CENTERALIGN, ptScreen.x, ptScreen.y, 0, m_hWnd, NULL))
            {
               case IDM_DELETE_BOX_FILE:
                  DeleteBoxFileByItem(hwnd, tvItem.hItem);
                  break;
               case IDM_DELETEMATERIAL:
               {
                  if (tvItem.lParam)
                  {
                     if (((BoxData*)tvItem.lParam)->nMaterial != 0)
                     {
                        ((BoxData*)tvItem.lParam)->nMaterial = 0;
                        OnSelectChangedBox(NULL);
                     }
                  }
               } break;
               case IDM_SELECTMATERIAL:
                  SelectMaterial(hwnd, tvItem);
                  break;
            }
            DestroyMenu(hMenu);
         }
         else if (tvItem.iImage >= IMAGE_SLAVES)
         {
            HMENU    hMenu;
            HMENU    context;
            SourceConfig sc  = m_MasterConfig;

            if (tvItem.iImage>IMAGE_SLAVES)                    // Hat der Satellit schon eine Zuordnung
            {                                                  // kann die Zuordnung gelöscht werden
               hMenu = LoadMenu(m_hInstance, MAKEINTRESOURCE(IDR_CONTEXT_PURGE_CR));
               context = GetSubMenu(hMenu, 0);
            }
            else                                               // Hat der Satellit noch keine Zuordnung
            {                                                  // kann eine Zuordnung erfolgen
               hMenu = LoadMenu(m_hInstance, MAKEINTRESOURCE(IDR_CONTEXT_SPEAKER));
               context = GetSubMenu(hMenu, 0);
               EnableMenuItem(context, IDM_MAIN_SPEAKER    , MF_BYCOMMAND|((sc.sf&MAIN_SPEAKER_BITS    )?MF_ENABLED:MF_DISABLED|MF_GRAYED));
               EnableMenuItem(context, IDM_CENTER_SPEAKER_F, MF_BYCOMMAND|((sc.sf&CENTER_SPEAKER_F_BITS)?MF_ENABLED:MF_DISABLED|MF_GRAYED));
               EnableMenuItem(context, IDM_CENTER_SPEAKER_R, MF_BYCOMMAND|((sc.sf&CENTER_SPEAKER_R_BITS)?MF_ENABLED:MF_DISABLED|MF_GRAYED));
               EnableMenuItem(context, IDM_EFFECT_SPEAKER  , MF_BYCOMMAND|((sc.sf&EFFECT_SPEAKER_BITS  )?MF_ENABLED:MF_DISABLED|MF_GRAYED));
            }
            switch (TrackPopupMenu(context, TPM_RETURNCMD|TPM_CENTERALIGN, ptScreen.x, ptScreen.y, 0, m_hWnd, NULL))
            {
               case IDM_MAIN_SPEAKER:     tvItem.iImage = IMAGE_SLAVES_MAIN    ; break;// Soll er ein Mainlautsprecher sein
               case IDM_CENTER_SPEAKER_F: tvItem.iImage = IMAGE_SLAVES_CENTER_F; break;// Soll er ein Centerlautsprecher sein
               case IDM_CENTER_SPEAKER_R: tvItem.iImage = IMAGE_SLAVES_CENTER_R; break;// Soll er ein Centerlautsprecher sein
               case IDM_EFFECT_SPEAKER:   tvItem.iImage = IMAGE_SLAVES_EFFECT;   break;// Soll er ein Effektlautsprecher sein
               case IDM_PURGE_CROSS_REFERENCE:                 // Soll die Zuordnung gelöscht werden
                  tvItem.iImage = IMAGE_SLAVES; break;
               default:
                  bChangeCrossReference = false;
                  break;
               case IDM_DELETEMATERIAL:
               {
                  if (tvItem.lParam)
                  {
                     if (((BoxData*)tvItem.lParam)->nMaterial != 0)
                     {
                        ((BoxData*)tvItem.lParam)->nMaterial = 0;
                        OnSelectChangedBox(NULL);
                     }
                  }
               } break;
               case IDM_SELECTMATERIAL:
                  SelectMaterial(hwnd, tvItem);
                  break;
            }
            DestroyMenu(hMenu);
            if (bChangeCrossReference)
            {
               tvItem.mask  = TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_HANDLE;
               tvItem.iSelectedImage = tvItem.iImage;
               TreeView_SetItem(hwnd, (LPARAM)&tvItem);
               CheckMasterConfig(TreeView_GetParent(hwnd, tvItem.hItem));
            }
         }
      }
      else
      {
         HMENU    hMenu;
         HMENU    context;
         hMenu = LoadMenu(m_hInstance, MAKEINTRESOURCE(IDR_CONTEXT_TREEVIEW));
         context = GetSubMenu(hMenu, 0);
         HWND hwndBoxType = GetDlgItem(IDC_BOX_TYPE);
         int nOffset = IDM_BOX_TYPE_SELECT;
         char text[128];
         int i, nCount = SendMessage(hwndBoxType, CB_GETCOUNT, 0, 0);
         for (i=0; i<nCount; i++)
         {
            SendMessage(hwndBoxType, CB_GETLBTEXT, i, (LPARAM)text);
            InsertMenu(context, i, MF_BYPOSITION, i + nOffset, text);
         }
         
         i = SendMessage(hwndBoxType, CB_GETCURSEL, 0, 0);
         CheckMenuRadioItem(context, nOffset, nOffset+nCount, nOffset+i, MF_BYCOMMAND);
         TrackPopupMenu(context, TPM_CENTERALIGN, ptScreen.x, ptScreen.y, 0, m_hWnd, NULL);
         DestroyMenu(hMenu);
      }
   }
}

/******************************************************************************
* Name       : CheckMasterConfig                                              *
* Zweck      : Abgleichen der eingestellten Masterconfiguration durch         *
*              überprüfen aller Kindfelder des Boxenknotens.                  *
*              Füllen der Übergabefelder mit Slaves (mit und ohne Zuordnung)  *
* Definition : bool CheckMasterConfig(HTREEITEM, int, int *);                 *                                                                
* Aufruf     : CheckMasterConfig(hItem, nMode, &nCount);                      *
* Parameter  :                                                                *
* hItem   (E): TreeItemhandle des Boxenknotens                   (HTREEITEM)  *
* nMode   (E): Übergabemodus für die Slaves                      (int)        *
*              (0, GET_UN_REFERENCED, GET_CROSS_REFERENCE)                    *
* nCount (EA): Index-Zähler für die Übergabefelder               (int*)       *
* Funktionswert : MasterConfig vollständig (true, false)         (bool)       *
******************************************************************************/
bool CCaraSdbDlg::CheckMasterConfig(HTREEITEM hItem, int nMode, int *pnCount)
{
   m_MasterConfig.sc = m_pCaraSdbP->Initial;                // Konfiguration auf Anfangswert setzen
   if (hItem != NULL)
   {
      SourceConfig sc;
      TVITEM       tvItem;
      UINT         n, i, nRef = 0, nSlaves = 0;
      ZeroMemory(&tvItem, sizeof(TVITEM));
      tvItem.mask = TVIF_IMAGE|TVIF_HANDLE;
      if (nMode) tvItem.mask |= TVIF_PARAM;
      HWND hwnd = GetDlgItem(IDC_BOX_TREE);
      tvItem.hItem = TreeView_GetChild(hwnd, hItem);
      while (tvItem.hItem != NULL)
      {
         nSlaves++;                                            // Zähle Slaves
         TreeView_GetItem(hwnd, &tvItem);
         switch (tvItem.iImage)
         {
            case IMAGE_SLAVES_MAIN    : sc.sf = m_MasterConfig.sf&MAIN_SPEAKER_BITS;     break;
            case IMAGE_SLAVES_CENTER_F: sc.sf = m_MasterConfig.sf&CENTER_SPEAKER_F_BITS; break;
            case IMAGE_SLAVES_CENTER_R: sc.sf = m_MasterConfig.sf&CENTER_SPEAKER_R_BITS; break;
            case IMAGE_SLAVES_EFFECT  : sc.sf = m_MasterConfig.sf&EFFECT_SPEAKER_BITS;   break;
            case IMAGE_SLAVES         : sc.sf = 0;                                       break;
         }
         if (nMode == GET_ALL_UNITS)  sc.sc.bitAllUnits = 1; 
         if (sc.sf)
         {
            for (i=0,n=1; i<9; i++, n<<=1)                     // Konfigurationsbits anpassen
            {
               if ((sc.sf & n) || (nMode == GET_ALL_UNITS))
               {
                  m_MasterConfig.sf &= ~n;                     // bit löschen
                  if (nMode != 0)                              // Übergabe von Dateinamen
                  {
                     if ((pnCount != NULL) && ((*pnCount) < m_pCaraSdbP->nCount))
                     {
                        nRef++;                                // Zähle Zuordnungen
                        BoxData *pBox = (BoxData*) tvItem.lParam;
                        SourceConfig *psc = (SourceConfig*) &m_pCaraSdbP->Selected[(*pnCount)];
                        strcpy(m_pCaraSdbP->szFileName[(*pnCount)++], pBox->szName);
                        psc->sf = n;
                     }
                  }
                  break;
               }
            }
         }
         tvItem.hItem = TreeView_GetNextItem(hwnd, tvItem.hItem, TVGN_NEXT);		
      }
      return ((nRef == nSlaves) || (nMode == GET_ALL_UNITS)) ? true : false;
   }
   return false;
}

/******************************************************************************
* Name       : OnSelectChangeBoxType                                          *
* Zweck      : Ändern des Boxentyps.                                          *
* Definition : void OnSelectChangeBoxType(HWND);                              *
* Aufruf     : OnSelectChangeBoxType(idCombo, hwndCombo);                     *
* Parameter  :                                                                *
* hwndCombo(E): Fensterhandle der ComboBox                           (HWND)   *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraSdbDlg::OnSelectChangeBoxType(HWND hwndCombo)
{
   int nID = SendMessage(hwndCombo, CB_GETCURSEL, 0, 0);
   UINT nBoxTypes = SendMessage(hwndCombo, CB_GETITEMDATA, nID, 0);
   if (m_nBoxTypes != nBoxTypes)
   {
      if (LOWORD(m_nBoxTypes) != LOWORD(nBoxTypes))            // wenn sich die Flags geändert haben
      {
         m_nBoxTypes = nBoxTypes;
         BuildNewTree();                                       // den Baum neu erzeugen
      }
      else
      {
         m_nBoxTypes = nBoxTypes;
      }
   }
}

/******************************************************************************
* Name       : OnSelectChangedBox                                             *
* Zweck      : Auswahl bzw. Update einer Box und deren Daten im Dialog.       *
* Definition : void OnSelectChangedBox(NM_TREEVIEW *);                        *
* Aufruf     : OnSelectChangedBox(pNmTv);                                     *
* Parameter  :                                                                *
* pNmTv  (E) : TreeView Notification-Message-Struktur          (NM_TREEVIEW*) *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraSdbDlg::OnSelectChangedBox(NM_TREEVIEW *pNmTv)
{
   TVITEM tvItem;
   HWND   hwndTV;
   ZeroMemory(&tvItem, sizeof(TVITEM));
   if (pNmTv)                                                  // Neue Box
   {
      hwndTV = pNmTv->hdr.hwndFrom;
      tvItem.hItem = pNmTv->itemNew.hItem;
   }
   else                                                        // Boxupdate
   {
      hwndTV = GetDlgItem(IDC_BOX_TREE);
      tvItem.hItem = m_hSelectedBox;
   }

   if (tvItem.hItem)
   {
      BoxData *pBox = NULL;
      tvItem.mask  = TVIF_IMAGE|TVIF_PARAM|TVIF_HANDLE;
      SendMessage(hwndTV, TVM_GETITEM, 0, (LPARAM)&tvItem);
      if ((tvItem.iImage >= IMAGE_BOXES) && (tvItem.lParam != NULL))
      {
         m_hSelectedBox = tvItem.hItem;
         pBox = (BoxData*) tvItem.lParam;
         EnableWindow(GetDlgItem(IDOK), true);
         if      (pBox->nType == MASTER) CheckMasterConfig(tvItem.hItem);
         else if (pBox->nType == SLAVE ) CheckMasterConfig(TreeView_GetParent(hwndTV, tvItem.hItem));
         char szDescription[BOX_DESCRIPTION_LENGTH*2];
         CopyForStaticCtrl(szDescription, pBox->szDescription);
         SetDlgItemText(IDC_DESCRIPTION, szDescription);
         if (m_pBoxView)
         {
//            TRACE("SelectBox: %s\n", pBox->szName);
            m_bViewDataOk = m_pBoxView->DoDataExchange(CARAWALK_SETBOXDATA, MAKELONG(CARAWALK_SHOW_SCALE, pBox->nMaterial), pBox->szName);
            if (m_bViewDataOk)
            {
               m_pBoxView->DoDataExchange(CARAWALK_SETANGLE, 0, &m_BoxPos);
            }
         }
         return;
      }
   }

   m_bViewDataOk = true;
   if (m_pBoxView)
      m_pBoxView->DoDataExchange(CARAWALK_SETBOXDATA, 0, NULL);
   EnableWindow(GetDlgItem(IDOK), false);
   m_hSelectedBox = NULL;
   SetDlgItemText(IDC_DESCRIPTION, "");
}
 
LRESULT CCaraSdbDlg::OnTimer(WPARAM nIDEvent)
{
   bool bChanged = false;
   if ((!m_bViewDataOk) && (m_hSelectedBox != NULL))
   {
      OnSelectChangedBox();
   }
   if (SendDlgItemMessage(IDC_PHILEFT, BM_GETSTATE,0,0) & BST_PUSHED)
   {
      m_BoxPos.nPhi++;
      if (m_BoxPos.nPhi  >= 360) m_BoxPos.nPhi = 0;
      bChanged = true;
   }
   if (SendDlgItemMessage(IDC_PHIRIGHT, BM_GETSTATE,0,0) & BST_PUSHED)
   {
      m_BoxPos.nPhi--;
      if (m_BoxPos.nPhi   <=  0) m_BoxPos.nPhi = 360;
      bChanged = true;
   }
   if (SendDlgItemMessage(IDC_THETAUP, BM_GETSTATE,0,0) & BST_PUSHED)
   {
      if (m_BoxPos.nTheta <  90) m_BoxPos.nTheta++;
      bChanged = true;
   }
   if (SendDlgItemMessage(IDC_THETADOWN, BM_GETSTATE,0,0) & BST_PUSHED)
   {
      if (m_BoxPos.nTheta > -90) m_BoxPos.nTheta--;
      bChanged = true;
   }
   if (bChanged) UpdateAngle();

   return 0;   
}

/******************************************************************************
* Name       : UpdateDataBase                                                 *
* Zweck      : Abgleichen der Datenbank mit den Dateien im Boxenverzeichnis   *
* Definition : bool UpdateDataBase();                                         *
* Aufruf     : UpdateDataBase();                                              *
* Parameter  : keine                                                          *
* Funktionswert : (true, false)                                        (bool) *
******************************************************************************/
bool CCaraSdbDlg::UpdateDataBase()
{
//   TRACE("UpdateDataBase(%d, %d)\n", m_hThread, m_nThreadID);
   if ((m_hThread != 0) && (m_nThreadID != 0))    return false;
   unsigned int nThreadId;
   m_hThread = (HANDLE) _beginthreadex(NULL, 0, CCaraSdbDlg::UpdateDataBaseThread, this, 0, &nThreadId);
   m_nThreadID = nThreadId;
   if ((m_hThread != 0) && (m_nThreadID != 0)) return true;
   else                                        return false;
}

unsigned __stdcall CCaraSdbDlg::WatchBoxFolderThread(void *pThreadParam)
{
   ASSERT(pThreadParam != NULL);
   if (!pThreadParam) return 1;
   CCaraSdbDlg  *pThis = (CCaraSdbDlg*) pThreadParam;
   HWND   hWnd   = pThis->m_hWnd;
   HANDLE hWatch = pThis->m_hWatchBoxFolder;
   while(true)
   {
      DWORD dwResult = WaitForSingleObject(hWatch, INFINITE);
      if (dwResult == WAIT_OBJECT_0)
      {
//         TRACE("ChangeNotification()\n");
         if (pThis != NULL)
         {
            if (pThis->m_bCanUpdateBoxView)
               PostMessage(hWnd, WM_COMMAND, IDC_UPDATE_DATABASE, 0);
            pThis->m_bWatchBoxInvalid = true;
         }
      }
      if (dwResult == WAIT_FAILED) return 0;
      FindNextChangeNotification(hWatch);
   }
   return 0;
}

/******************************************************************************
* Name       : UpdateDataBaseThread                                           *
* Zweck      : Abgleichen der Datenbank mit den Dateien im Boxenverzeichnis   *
* Definition : static DWORD WINAPI UpdateDataBaseThread(LPVOID);              *
* Aufruf     : durch CreateThread(..);                                        *
* Parameter  : keine                                                          *
* Funktionswert : (0,...) Fehlercode                                  (DWORD) *
******************************************************************************/
unsigned __stdcall CCaraSdbDlg::UpdateDataBaseThread(void * pThreadParam)
{
   ASSERT(pThreadParam != NULL);
   if (!pThreadParam) return 1;
   CCaraSdbDlg      *pThis = (CCaraSdbDlg*) pThreadParam;
   return pThis->UpdateDataBaseThread();
}

unsigned CCaraSdbDlg::UpdateDataBaseThread()
{
   HANDLE            hFile     = INVALID_HANDLE_VALUE,
                     hNewFile  = INVALID_HANDLE_VALUE,
                     hFindFile = INVALID_HANDLE_VALUE;
   CFileHeader       fh;
   char              szFilePath[_MAX_PATH], *ptr;
   BoxData           Box, *pBox;
   WIN32_FIND_DATA   w32fd;
   int               nPathLength, nBox;
   long              nBoxType, nFiles;
   DWORD             dwBytesRead, dwActFilePointer;
   bool              bInsertBoxes;

   Sleep(400);
   while (m_bWatchBoxInvalid)
   {
      m_bWatchBoxInvalid = false;
      Sleep(1000);
   } 

   bInsertBoxes = (m_BoxTree.GetItemCount() == 0) ? false : true;
   try
   {
      ETSLSMB_CLSMB sLSMB;                                    // LoadSaveMsgBox initialisieren
      sLSMB.nLines    = 1;
      sLSMB.nLoadSave = 1;
      ::LoadString(m_hInstance, IDS_UPDATE_DATA_BASE, sLSMB.szText[0], 64);
      CEtsLsmb CLSMB(&sLSMB);

      GetLsBoxPath(szFilePath);
      nPathLength = strlen(szFilePath);
      strcat(szFilePath, "*.BDD");
      hFindFile = FindFirstFile(szFilePath, &w32fd);
      if (hFindFile != INVALID_HANDLE_VALUE)
      {
         pBox = (BoxData*) m_BoxData.GetFirst();
         while (pBox)
         {
            pBox->bFound = false;
            pBox = (BoxData*) m_BoxData.GetNext();
         }

         do
         {
            MSG msg;
            _asm CLD;
            if (PeekMessage(&msg, NULL, WM_COMMAND, WM_COMMAND, PM_REMOVE))
            {
               if (LOWORD(msg.wParam) == IDC_END_UPDATE_DATABASE_THREAD)
               {
                  return 1;
               }
            }
            strcpy(Box.szName, w32fd.cFileName);
            strupr(Box.szName);
            ptr = strstr(Box.szName, ".");
            if (ptr) ptr[0] = 0;
            pBox = NULL;                                       // Box in der Datenbank suchen
            nBox = m_BoxData.FindElement(CCaraSdbDlg::CompareBoxes, &Box);
            if (nBox != -1) pBox = (BoxData*)m_BoxData.GetAt(nBox);

            if (pBox)                                          // Datei gefunden
            {
               pBox->bFound = true;                            // markieren
            }
            else                                               // sonst, dazuladen
            {
               szFilePath[nPathLength] = 0;
               strncpy(&szFilePath[nPathLength], w32fd.cFileName, BDD_NAME_LENGTH);
               hFile = CreateFile(szFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
               if (hFile == INVALID_HANDLE_VALUE) {ASSERT(false); continue;}
                                                               // Boxendaten lesen und prüfen
               ReadFile(hFile, &fh, sizeof(CFileHeader), &dwBytesRead, NULL);
               if (!fh.IsType(BOX_SOUND_DATA)) {ReportFileError(hFile, szFilePath); continue;}
                                                               // Geometriedaten suchen
               dwActFilePointer = SetFilePointer(hFile, fh.GetLength(), NULL, FILE_CURRENT);
               ReadFile(hFile, &fh, sizeof(CFileHeader), &dwBytesRead, NULL);
               if (!fh.IsType(BOX_GEOMETRIC_DATA)) {ReportFileError(hFile, szFilePath); continue;}
                                                               // Geometriedaten lesen
               ReadFile(hFile, &nBoxType, sizeof(long), &dwBytesRead, NULL);
                                                               // Hersteller suchen
               Manufacturer * pM  = FindManufacturer((char*)fh.GetComment());
               pBox = new BoxData;                             // Box neu anlegen
               m_BoxData.ADDHead(pBox);                        // in der Boxen-Liste speichern
               ZeroMemory(pBox, sizeof(BoxData));
               pM->pBoxes->ADDHead(pBox);                      // dem Hersteller zuordnen

               strcpy(pBox->szName, Box.szName);
               SetFilePointer(hFile, sizeof(long), NULL, FILE_CURRENT);
               ReadFile(hFile, pBox->szDescription, sizeof(char)*BOX_DESCRIPTION_LENGTH, &dwBytesRead, NULL);

               if (nBoxType != SLAVE)                          // Slaves dürfen nicht in dieser Ebene angezeigt werden
               {                                               // werden aber in die Datenbank übernommen
                  SetFilePointer(hFile, sizeof(long)*16, NULL, FILE_CURRENT);
                  ReadFile(hFile, &nFiles, sizeof(long), &dwBytesRead, NULL);
                  if ((nBoxType == MASTER) && (nFiles > 0))    // Ein Master enthält alle Slaves
                  {
                     pBox->pszFiles = new char[nFiles*BDD_NAME_LENGTH];
                     for (nBox = 0; nBox<nFiles; nBox++)       // Slaves lesen
                     {
                        ptr = &pBox->pszFiles[nBox*BDD_NAME_LENGTH];
                        ReadFile(hFile, ptr, BDD_NAME_LENGTH*sizeof(char), &dwBytesRead, NULL);
                        ptr = strstr(ptr, ".");
                        if (ptr) ptr[0] = 0;
                     }
                     SetFilePointer(hFile, dwActFilePointer+fh.GetLength()+sizeof(CFileHeader), NULL, FILE_BEGIN);
                     ReadFile(hFile, &fh, sizeof(CFileHeader), &dwBytesRead, NULL);
                     if ((dwBytesRead == sizeof(CFileHeader)) && (fh.GetVersion() == 100))
                     {
                        if (fh.IsType(PREVIEW_METAFILE))       // Dateityp suchen/prüfen
                        {
                           SetFilePointer(hFile, fh.GetLength(), NULL, FILE_CURRENT);
                           ReadFile(hFile, &fh, sizeof(CFileHeader), &dwBytesRead, NULL);
                        }
                        if (!fh.IsType(BOX_DESCRIPTION_DATA))
                        {
                           HANDLE hDummy = INVALID_HANDLE_VALUE;
                           ReportFileError(hDummy, szFilePath);
                           continue;
                        }

                        for (nBox = 0; nBox<nFiles; nBox++)    // Slaves prüfen bzw. auspacken
                        {
                           szFilePath[nPathLength] = 0;
                           strncpy(&szFilePath[nPathLength], &pBox->pszFiles[nBox*BDD_NAME_LENGTH], BDD_NAME_LENGTH);
                           strcat(szFilePath, ".BDD");
                           hNewFile = CreateFile(szFilePath, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_ARCHIVE, NULL);
                           if (hNewFile == INVALID_HANDLE_VALUE)// wenn die Box schon vorhanden ist
                           {                                   // prüfen, ob vorhandene ein Slave ist !!
                              hNewFile = CreateFile(szFilePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL);
                              if (hNewFile == INVALID_HANDLE_VALUE) break;// nicht vorhanden Fehler

                              ReadFile(hNewFile, &fh, sizeof(CFileHeader), &dwBytesRead, NULL);
                              if (!fh.IsType(BOX_SOUND_DATA)) break;

                              SetFilePointer(hNewFile, fh.GetLength(), NULL, FILE_CURRENT);
                              ReadFile(hNewFile, &fh, sizeof(CFileHeader), &dwBytesRead, NULL);
                              if (!fh.IsType(BOX_GEOMETRIC_DATA)) break;
                              CloseHandle(hNewFile);
                              continue;
                           }

                           if (nBox>0)
                              ReadFile(hFile, &fh, sizeof(CFileHeader), &dwBytesRead, NULL);
                           if (!fh.IsType(BOX_DESCRIPTION_DATA)) break;

                           ReadFile(hFile, &fh, sizeof(CFileHeader), &dwBytesRead, NULL);
                           if (!fh.IsType(BOX_SOUND_DATA)) break;
                           
                           WriteFile(hNewFile, &fh, sizeof(CFileHeader), &dwBytesRead, NULL);
                           CopyFileData(hFile, hNewFile, fh.GetLength());

                           ReadFile(hFile, &fh, sizeof(CFileHeader), &dwBytesRead, NULL);
                           if (!fh.IsType(BOX_GEOMETRIC_DATA)) break;

                           WriteFile(hNewFile, &fh, sizeof(CFileHeader), &dwBytesRead, NULL);
                           CopyFileData(hFile, hNewFile, fh.GetLength());

                           CloseHandle(hNewFile);
                        }
                        CloseHandle(hNewFile);
                        if (nBox<nFiles)
                           ReportFileError(hNewFile, szFilePath);
                     }
                  }
               }
               else nFiles = 0;
               pBox->nType  = (WORD) nBoxType;
               pBox->nFiles = (WORD) nFiles;
               pBox->bFound = true;
               int nType = BOX_TYPE(pBox->nType);              // Bitposition ermitteln
               if ( bInsertBoxes                 &&            // dürfen sie eingefügt werden
                   ((nType & m_nBoxTypes) != 0) &&             // wählbarer Boxtyp und
                   (m_nMaxSelectableBoxes >= pBox->nFiles))    // Anzahl noch wählbarer Slaves prüfen
               {
                  m_BoxTree.ProcessWithItems(CCaraSdbDlg::InsertBox, (WPARAM)pBox, (LPARAM)pM);
               }
               CloseHandle(hFile);
            }
         }
         while (FindNextFile(hFindFile, &w32fd));
         FindClose(hFindFile);
         hFindFile = INVALID_HANDLE_VALUE;

         pBox = (BoxData*) m_BoxData.GetFirst();
         while (pBox)
         {
            if (pBox->bFound == false)
            {
               m_bCanUpdateBoxView = false;
               DeleteBoxFiles(pBox);
               m_bCanUpdateBoxView = true;
               UpdateManufacturer(pBox);
               if (bInsertBoxes)
                  m_BoxTree.ProcessWithItems(CCaraSdbDlg::DeleteItemByParam ,(WPARAM)pBox, 0);
               int nActPos = m_BoxData.GetActualPosition()-1;
               m_BoxData.Delete(pBox);
               pBox = NULL;
               if (nActPos>=0) pBox = (BoxData*)m_BoxData.GetAt(nActPos);
               if (!pBox) pBox = (BoxData*) m_BoxData.GetFirst();
               continue;
            }
            pBox = (BoxData*) m_BoxData.GetNext();
         }
         m_BoxData.Sort(CCaraSdbDlg::CompareBoxes);
      }
   }
   catch (DWORD dwError)
   {
      REPORT("UpdateDataBaseThread Error: %x", dwError);
   }

   if (hFindFile != INVALID_HANDLE_VALUE)
      FindClose(hFindFile);

   ::PostMessage(m_hWnd, WM_COMMAND, IDC_END_UPDATE_DATABASE_THREAD, 0);
   return 0;
}

/******************************************************************************
* Name       : CompareBoxes                                                   *
* Zweck      : Vergleichsfunktion für die Ansi-Funktion qsort zum sortieren   *
*              der Lautsprecherdaten.                                         *
* Definition : static int __cdecl CompareBoxes(const void*, const void*);     *
* Aufruf     : CompareBoxes(elem1, elem2);                                    *
* Parameter  :                                                                *
* Funktionswert : (-1, 0, 1)                                                  *
******************************************************************************/
int CCaraSdbDlg::CompareBoxes(const void *elem1, const void *elem2)
{
   return strcmp( ((BoxData*)elem1)->szName, ((BoxData*)elem2)->szName);
}

/******************************************************************************
* Name       : UpdateManufacturer                                             *
* Zweck      : Abgleichen der Hersteller beim Löschen einer Box aus der       *
*              Datenbank.                                                     *
* Definition : static void UpdateManufacturer(BoxData*,Manufacturer***,int&); *
* Aufruf     : UpdateManufacturer(pBox);                                       *
* Parameter  :                                                                *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraSdbDlg::UpdateManufacturer(BoxData *pBox)
{
   Manufacturer *pMan = (Manufacturer*) m_Manufacturer.GetFirst();
   while(pMan)
   {
      if (pMan->pBoxes->InList(pBox))
      {
         pMan->pBoxes->Remove(pBox);
         if (pMan->pBoxes->GetCounter() == 0)
         {
            m_BoxTree.ProcessWithItems(CCaraSdbDlg::DeleteItemByParam ,(WPARAM)pMan, 0);
            m_Manufacturer.Delete(pMan);
         }
         break;
      }
      pMan = (Manufacturer*) m_Manufacturer.GetNext();
   }
}

/******************************************************************************
* Name       : FindManufacturer                                               *
* Zweck      : Suchen eines Hersteller nach einem Namen und ggf. Anlegen eines*
*              weiteren Herstellers.                                          *
* Definition : static Manufacturer*                                           *
               FindManufacturer(char*, Manufacturer***, int&);                *
* Aufruf     : FindManufacturer(pszName);                                     *
* Parameter  :                                                                *
* pszName (E): Name des Herstellers                                  (char*)  *
* Funktionswert : Struktur mit Herstellerdaten               (Manufacturer*)  *
******************************************************************************/
Manufacturer * CCaraSdbDlg::FindManufacturer(char *szName)
{
   char szNoName[] =  NO_NAMEBOX;
   if (strcmp(szName, &szNoName[1])==0) szName = szNoName;
   Manufacturer *pMan = (Manufacturer*) m_Manufacturer.GetFirst();
   while (pMan)
   {
      if (strcmp(pMan->szName, szName)==0)
         return pMan;
      pMan = (Manufacturer*) m_Manufacturer.GetNext();
   }

   Manufacturer *pNewMan = new Manufacturer;
   ZeroMemory(pNewMan, sizeof(Manufacturer));
   strcpy(pNewMan->szName, szName);
   pNewMan->pBoxes = new CEtsList;
   pNewMan->pBoxes->SetDestroyMode(false);
   m_Manufacturer.ADDHead(pNewMan);
   return pNewMan;
}

/******************************************************************************
* Name       : BuildNewTree                                                   *
* Zweck      : Erzeugen eines neuen Baumes für die Strukturansicht            *
*              Hersteller, Boxen, Slaves nach Boxentypen ausgewählt.          *
* Definition : void BuildNewTree();                                           *
* Aufruf     : BuildNewTree()                                                 *
* Parameter  : keine                                                          *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraSdbDlg::BuildNewTree()
{
   m_BoxTree.DeleteItem();                                     // Baum löschen
   m_hSelectedBox    = NULL;                                   // die Selektion löschen
   OnSelectChangedBox();                                       // die Box wurde geändert

   bool            bContainsBoxes;
   UINT            nType;
   TVINSERTSTRUCT  tvMan;                                      // Herstellerknoten
   tvMan.hParent      = TVI_ROOT;
   tvMan.hInsertAfter = TVI_SORT;
   tvMan.itemex.mask    = TVIF_TEXT|TVIF_PARAM|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
   TVINSERTSTRUCT  tvBox;                                      // Boxenknoten
   tvBox.hInsertAfter = TVI_SORT;
   tvBox.itemex.mask  = TVIF_TEXT|TVIF_PARAM|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
   TVINSERTSTRUCT  tvSlaves;                                   // Slaveknoten
   tvSlaves.hInsertAfter = TVI_SORT;
   tvSlaves.itemex.mask    = TVIF_TEXT|TVIF_PARAM|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
   tvSlaves.itemex.iImage  = tvSlaves.itemex.iSelectedImage = IMAGE_SLAVES;

   Manufacturer * pM = (Manufacturer*) m_Manufacturer.GetFirst();
   while (pM)
   {
//      REPORT("%s", pM->szName);
      InsertManufacturer(m_BoxTree.GetWindowHandle(), tvMan, tvBox, pM, true);
      bContainsBoxes = false;
      BoxData *pBox = (BoxData*) pM->pBoxes->GetFirst();
      while (pBox)                                             // Boxen des Herstellers einfügen
      {
//         REPORT("   %s \t\t %s", pBox->szName, pBox->szDescription);
         nType = BOX_TYPE(pBox->nType);                        // Bitposition ermitteln
         if (((nType & m_nBoxTypes) != 0) &&                   // wählbarer Boxtyp und
             (m_nMaxSelectableBoxes >= pBox->nFiles))          // Anzahl noch wählbarer Slaves prüfen
         {
            bContainsBoxes = true;                             // dieser Knoten enthält mindesten eine Box
            InsertBox(m_BoxTree.GetWindowHandle(), tvBox, tvSlaves, pBox);
            if ((pBox->nType == MASTER) && (pBox->nFiles > 0)) // sind Slaves vorhanden
            {
               int     k;
               BoxData Bd;
               BoxData *pParam = &Bd;
               int nFound;
               for (k=0; k<pBox->nFiles; k++)                  // Alle Slaves einfügen
               {
                  tvSlaves.itemex.pszText = &pBox->pszFiles[k*BOX_DESCRIPTION_LENGTH];
                  strcpy(Bd.szName, tvSlaves.itemex.pszText);
                  nFound = m_BoxData.FindElement(CCaraSdbDlg::CompareBoxes, &Bd);
                  if (nFound != -1)                            // Boxedaten gefunden
                  {
                     BoxData *pBox = (BoxData*) m_BoxData.GetAt(nFound);
                     REPORT("      %s", pBox->szName);
                     tvSlaves.itemex.lParam = (LPARAM) pBox;
                     m_BoxTree.InsertItem(&tvSlaves);
                  }
                  else
                  {
                     MasterError(pBox);                        // Master Fehler ausgeben
                     UpdateDataBase();                         // Datenbank neu aufbauen
                     return;                                   // Funktion beenden
                  }
               }
            }
         }
         pBox = (BoxData*) pM->pBoxes->GetNext();
      }
      if (!bContainsBoxes)                                     // enthält der Herstellerknoten Boxen ?
      {
         m_BoxTree.DeleteItem(tvBox.hParent);                  // Nein, löschen
      }
      pM = (Manufacturer*) m_Manufacturer.GetNext();
   }
}

/******************************************************************************
* Name       : DeleteBoxFiles                                                 *
* Zweck      : Löschen eines Sateliten-Subwoofersystems mit allen Slaves, die *
*              dazugehören.                                                   *
* Definition : static void DeleteBoxFiles(BoxData *, bool, bool);             *
* Aufruf     : DeleteBoxFiles(pBox, bMaster, bSlaves);                         *
* Parameter  :                                                                *
* pBox     (E): BoxData-Struktur                                    (BoxData*) *
* bMaster (E): Master löschen (true, false)                          (bool)   *
* bSlaves (E): Slave löschen (true, false)                           (bool)   *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraSdbDlg::DeleteBoxFiles(BoxData *pBox, bool bMaster, bool bSlaves)
{
   char szFilePath[_MAX_PATH];
   int nPathLength;

   GetLsBoxPath(szFilePath);
   nPathLength = strlen(szFilePath);
   if (bMaster)
   {
      strcat(szFilePath, pBox->szName);
      strcat(szFilePath, ".BDD");
      DeleteFile(szFilePath);
   }

   if (bSlaves)
   {
      for (int nBox = 0; nBox<pBox->nFiles; nBox++)
      {
         szFilePath[nPathLength] = 0;
         strncpy(&szFilePath[nPathLength], &pBox->pszFiles[nBox*BDD_NAME_LENGTH], BDD_NAME_LENGTH);
         strcat(szFilePath, ".BDD");
         DeleteFile(szFilePath);
      }
   }
}

/******************************************************************************
* Name       : MasterError                                                    *
* Zweck      : Ausgabe einer Fehlermeldung über ein unvollständiges Sateliten-*
*              Subwoofersystem. Löschen des Datenbankeintrages auch aller     *
*              dazugehörenden Sateliten. Löschen aller Dateien des Systems,   *
*              wenn dies gewünscht wird.                                      *
* Definition : bool MasterError(BoxData *);                                   *
* Aufruf     : MasterError(pBox);                                              *
* Parameter  : keine                                                          *
* Funktionswert : true, wenn das System repariert werden soll.                *
******************************************************************************/
bool CCaraSdbDlg::MasterError(BoxData *pBox)
{
   if (pBox)
   {
      char text[512], heading[256];
      ::LoadString(m_hInstance, IDS_MASTER_ERROR_H, heading, 256);
      ::LoadString(m_hInstance, IDS_MASTER_ERROR_T, text   , 512);
      strncat(text, pBox->szName, BOX_NAME_LENGTH);
      int nReturn = ::MessageBox(m_hWnd, text, heading, MB_YESNO|MB_ICONERROR|MB_SYSTEMMODAL);

      m_bCanUpdateBoxView = false;
      if (nReturn == IDYES) DeleteBoxFiles(pBox);               // Master und Slave -Dateien löschen
      else                  DeleteBoxFiles(pBox, false, true);  // nur Slaves löschen
      m_bCanUpdateBoxView = true;

      char    *pszName;
      int      i;
      for (i=0; i<=pBox->nFiles; i++)                           // Datenbankeinträge immer löschen
      {
         if (i==pBox->nFiles) pszName = pBox->szName;            // den Master zuletzt löschen
         else                pszName = &pBox->pszFiles[i*BOX_DESCRIPTION_LENGTH];
         DeleteBoxByName(pszName);
      }
      return (nReturn == IDYES) ? false : true;
   }
   return false;
}

/******************************************************************************
* Name       : GetLsBoxPath                                                   *
* Zweck      : liefert den Pfad für das Verzeichnis der Boxendateien.         *
* Definition : static void GetLsBoxPath(char*);                               *
* Aufruf     : GetLsBoxPath(pszFilePath);                                     *
* Parameter  :                                                                *
* pszFilePath(A): Pfad für das Verzeichnis                           (char*)  *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraSdbDlg::GetLsBoxPath(char *pszFilePath)
{
/*
#ifdef _DEBUG
   strcpy(pszFilePath, "C:\\Programme\\ETS\\Cara_1_1");
#else
*/
   GetCurrentDirectory(_MAX_PATH, pszFilePath);
//#endif
   if (pszFilePath[lstrlen(pszFilePath)-1] != '\\') strcat(pszFilePath, "\\");
   strcat(pszFilePath, "LS_Box\\");
}

/******************************************************************************
* Name       : ReadDataBase                                                   *
* Zweck      : Lesen der gespeicherten Datenbank aus der Datei BOX_DATA_BASE  *
* Definition : void ReadDataBase();                                           *
*              im Lautsprecherverzeichnis.                                    *
* Aufruf     : ReadDataBase()                                                 *
* Parameter  : keine                                                          *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraSdbDlg::ReadDataBase()
{
   DeleteSdBData();
   char     szFilePath[_MAX_PATH];
   DWORD    dwBytesRead;
   int      i, j, nBoxes, nIndex, nVersion, nManufacturer;
   BoxData *pBox;
   long     nLength;
   HANDLE   hFile;

   GetLsBoxPath(szFilePath);
   strcat(szFilePath, BOX_DATA_BASE);
   try
   {
      hFile = CreateFile(szFilePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
      if (hFile == INVALID_HANDLE_VALUE) throw (DWORD) 0;

      ReadFile(hFile, &nBoxes, sizeof(int), &dwBytesRead, NULL);
      ReadFile(hFile, &nVersion, sizeof(int), &dwBytesRead, NULL);
      if ((nBoxes == CARA_TYPE) && (nVersion == CARAWALK_VERSION))
      {
         ReadFile(hFile, &nBoxes, sizeof(int), &dwBytesRead, NULL);
         for (i=0; i<nBoxes; i++)
         {
            pBox = new BoxData;
            ZeroMemory(pBox, sizeof(BoxData));
            m_BoxData.ADDHead(pBox);

            ReadFile(hFile, &nLength, sizeof(long), &dwBytesRead, NULL);
            ReadFile(hFile, pBox->szName, sizeof(char)*nLength, &dwBytesRead, NULL);
            pBox->szName[nLength] = 0;

            ReadFile(hFile, &nLength, sizeof(long), &dwBytesRead, NULL);
            ReadFile(hFile, pBox->szDescription, sizeof(char)*nLength, &dwBytesRead, NULL);
            pBox->szDescription[nLength] = 0;

            ReadFile(hFile, &pBox->nType, sizeof(WORD), &dwBytesRead, NULL);
            ReadFile(hFile, &pBox->nFiles, sizeof(WORD), &dwBytesRead, NULL);
            ReadFile(hFile, &pBox->nMaterial, sizeof(WORD), &dwBytesRead, NULL);

            if (pBox->nFiles>0)
            {
               int nFiles = pBox->nFiles*BDD_NAME_LENGTH*sizeof(char);
               pBox->pszFiles = new char[nFiles];
               ReadFile(hFile, pBox->pszFiles, nFiles, &dwBytesRead, NULL);
            }
         }

         ReadFile(hFile, &nManufacturer, sizeof(int), &dwBytesRead, NULL);
         for (i=0; i<nManufacturer; i++)
         {
            Manufacturer *pNewMan = new Manufacturer;
            ZeroMemory(pNewMan, sizeof(Manufacturer));
            m_Manufacturer.ADDHead(pNewMan);
            pNewMan->pBoxes = new CEtsList;
            pNewMan->pBoxes->SetDestroyMode(false);
            ReadFile(hFile, &nBoxes, sizeof(int), &dwBytesRead, NULL);
            ReadFile(hFile, &nLength, sizeof(long), &dwBytesRead, NULL);
            ReadFile(hFile, &pNewMan->szName, nLength, &dwBytesRead, NULL);
            pNewMan->szName[nLength] = 0;

            if (nBoxes)
            {
               for (j=0; j<nBoxes; j++)
               {
                  ReadFile(hFile, &nIndex, sizeof(int), &dwBytesRead, NULL);
                  pNewMan->pBoxes->ADDHead(m_BoxData.GetAt(nIndex));
               }
            }
         }
      }
   }
   catch(DWORD dwError)
   {
      REPORT("ReadDataBase Error: %x", dwError);
   }
   CloseHandle(hFile);
}

/******************************************************************************
* Name       : SaveDataBase                                                   *
* Zweck      : Speichern der Datenbank in der Datei BOX_DATA_BASE im          *
*              Lautsprecherverzeichnis.                                       *
* Definition : void SaveDataBase();                                           *
* Aufruf     : SaveDataBase();                                                *
* Parameter  : keine                                                          *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraSdbDlg::SaveDataBase()
{
   char  szFilePath[_MAX_PATH];
   DWORD dwBytesRead;
   int   nDummy;
   long  nLength;
   HANDLE hFile;

   if (m_BoxData.GetCounter() == 0) return;
   GetLsBoxPath(szFilePath);
   strcat(szFilePath, BOX_DATA_BASE);
   DWORD dwAttributes = ::GetFileAttributes(szFilePath);
   if (dwAttributes != 0xffffffff)
   {
      dwAttributes &= FILE_ATTRIBUTE_READONLY;
      if (dwAttributes != 0) ::SetFileAttributes(szFilePath, dwAttributes);
   }
   try
   {
      hFile = CreateFile(szFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
      if (hFile == INVALID_HANDLE_VALUE) throw (DWORD) 0;

      nDummy = CARA_TYPE;
      WriteFile(hFile, &nDummy, sizeof(int), &dwBytesRead, NULL);
      nDummy = CARAWALK_VERSION;
      WriteFile(hFile, &nDummy, sizeof(int), &dwBytesRead, NULL);
      nDummy = m_BoxData.GetCounter();
      WriteFile(hFile, &nDummy, sizeof(int), &dwBytesRead, NULL);
      BoxData *pBox = (BoxData*) m_BoxData.GetFirst();
      while (pBox)
      {
         nLength = strlen(pBox->szName);
         WriteFile(hFile, &nLength, sizeof(long), &dwBytesRead, NULL);
         WriteFile(hFile, pBox->szName, sizeof(char)*nLength, &dwBytesRead, NULL);

         nLength = strlen(pBox->szDescription);
         WriteFile(hFile, &nLength, sizeof(long), &dwBytesRead, NULL);
         WriteFile(hFile, pBox->szDescription, sizeof(char)*nLength, &dwBytesRead, NULL);

         WriteFile(hFile, &pBox->nType    , sizeof(WORD), &dwBytesRead, NULL);
         WriteFile(hFile, &pBox->nFiles   , sizeof(WORD), &dwBytesRead, NULL);
         WriteFile(hFile, &pBox->nMaterial, sizeof(WORD), &dwBytesRead, NULL);

         if ((pBox->pszFiles != NULL) && (pBox->nFiles>0))
         {
            WriteFile(hFile, pBox->pszFiles, sizeof(char)*BDD_NAME_LENGTH*pBox->nFiles, &dwBytesRead, NULL);
         }
         pBox = (BoxData*) m_BoxData.GetNext();
      }

      nDummy = m_Manufacturer.GetCounter();
      WriteFile(hFile, &nDummy, sizeof(int), &dwBytesRead, NULL);
      Manufacturer * pM = (Manufacturer*) m_Manufacturer.GetFirst();
      while (pM)
      {
         nDummy = pM->pBoxes->GetCounter();
         WriteFile(hFile, &nDummy, sizeof(int), &dwBytesRead, NULL);
         nLength = strlen(pM->szName);
         WriteFile(hFile, &nLength, sizeof(long), &dwBytesRead, NULL);
         WriteFile(hFile, pM->szName, sizeof(char)*nLength, &dwBytesRead, NULL);
         BoxData * pB = (BoxData*) pM->pBoxes->GetFirst();
         while (pB)
         {
            nDummy = m_BoxData.FindElement(NULL, pB);
            WriteFile(hFile, &nDummy, sizeof(int), &dwBytesRead, NULL);
            pB = (BoxData*) pM->pBoxes->GetNext();
         }
         pM = (Manufacturer*) m_Manufacturer.GetNext();
      }
   }
   catch (DWORD dwError)
   {
      REPORT(" %x", dwError);
   }
   CloseHandle(hFile);
}

/******************************************************************************
* Name       : CopyFileData                                                   *
* Zweck      : Kopieren der Daten von einer Datei in eine andere.             *
* Definition : static void CopyFileData(HANDLE, HANDLE, long);                *
* Aufruf     : CopyFileData(hSrc, hDest, nLength);                            *
* Parameter  :                                                                *
* hSrc    (E): Quelle                                                (HANDLE) *
* hDest   (E): Ziel                                                  (HANDLE) *
* nLength (E): Datenlänge                                            (long)   *
* Funktionswert : keiner                                                      *
******************************************************************************/
#define COPY_LENGTH 512
void CCaraSdbDlg::CopyFileData(HANDLE hSrc, HANDLE hDest, long nLength)
{
   char  ptr[COPY_LENGTH];
   DWORD nCopyLength = COPY_LENGTH;
   DWORD dwBytes;
   if ((hSrc != INVALID_HANDLE_VALUE) && (hDest != INVALID_HANDLE_VALUE))
   {
      do
      {
         nLength -= COPY_LENGTH;
         if (nLength < 0)
         {
            nCopyLength = COPY_LENGTH + nLength;
         }
         ReadFile( hSrc , ptr, nCopyLength, &dwBytes, NULL);
         ASSERT(nCopyLength == dwBytes);
         WriteFile(hDest, ptr, nCopyLength, &dwBytes, NULL);
         ASSERT(nCopyLength == dwBytes);
      } while (nLength > 0);
   }
}
#undef COPY_LENGTH

/******************************************************************************
* Name       : ReportFileError                                                *
* Zweck      : Ausgabe einer Fehlermeldung und Schließen einer Datei.         *
* Definition : void ReportFileError(HANDLE, char *);                          *
* Aufruf     : ReportFileError(hFile, pszFile);                               *
* Parameter  :                                                                *
* hFile   (E): zu schließender Dateihandle                           (HANDLE) *
* pszFile (E): Name der fehlerhaften Datei                           (char*)  *
* Funktionswert : keiner                                                      *
******************************************************************************/
#define ERRORTEXT_LENGTH 128
void CCaraSdbDlg::ReportFileError(HANDLE & hFile, char *pszFile)
{
   char text[ERRORTEXT_LENGTH];
   ::LoadString(m_hInstance, IDS_BOXFILE_ERROR, text, ERRORTEXT_LENGTH);
   CloseHandle(hFile);
   if (::MessageBox(m_hWnd, pszFile, text, MB_YESNO|MB_ICONQUESTION|MB_SYSTEMMODAL)== IDYES)
   {
      ::DeleteFile(pszFile);
   }

}
#undef ERRORTEXT_LENGTH


/******************************************************************************
* Name       : DeleteBoxByName                                                *
* Zweck      : Löschen einer Box aus der Datenbank, die anhand ihres Namens   *
*              gesucht wird.                                                  *
* Definition : void DeleteBoxByName(char*);                                   *
* Aufruf     : DeleteBoxByName(pszName);                                      *
* Parameter  :                                                                *
* pszName (E): Name der Box                                          (char*)  *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraSdbDlg::DeleteBoxByName(char *pszName)
{
   BoxData *pBoxData = (BoxData*) m_BoxData.GetFirst();
   while (pBoxData)                            // alle Einträge durchsuchen
   {
      if (strcmp(pBoxData->szName, pszName) ==0)     // nach dem Namen
      {
         if (pBoxData->nType == MASTER)
         {
            m_bCanUpdateBoxView = false;
            DeleteBoxFiles(pBoxData);
            m_bCanUpdateBoxView = true;
         }
         UpdateManufacturer(pBoxData);
         m_BoxData.Delete(pBoxData);
         break;
      }
      pBoxData = (BoxData*) m_BoxData.GetNext();
   }
}

/******************************************************************************
* Name       : DeleteBoxFileByItem                                            *
* Zweck      : Löschen einer Box mit allen dazugehörenden Dateien. Die Box    *
*              wird anhand des TreeViewItems identifiziert.                   *
* Definition : void DeleteBoxFileByItem(HWND, HTREEITEM);                     *
* Aufruf     : DeleteBoxFileByItem(hwnd, hItem);                              *
* Parameter  :                                                                *
* hwnd    (E): Handle des TreeViews                                    (HWND) *
* hItem   (E): Item aus diesem TreeView                           (HTREEITEM) *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraSdbDlg::DeleteBoxFileByItem(HWND hwnd, HTREEITEM hItem)
{
   TVITEM tvItem;
   ZeroMemory(&tvItem, sizeof(TVITEM));
   tvItem.mask  = TVIF_PARAM|TVIF_HANDLE;
   tvItem.hItem = hItem;
   TreeView_GetItem(hwnd, &tvItem);
   if (tvItem.lParam)
   {
      BoxData *pBox = (BoxData*)tvItem.lParam;
      m_bCanUpdateBoxView = false;
      DeleteBoxFiles(pBox);
      m_bCanUpdateBoxView = true;
      UpdateManufacturer(pBox);
      m_BoxData.Delete(pBox);
   }
   HTREEITEM hParent = TreeView_GetParent(hwnd, hItem);
   TreeView_DeleteItem(hwnd, hItem);
   hItem = TreeView_GetChild(hwnd, hParent);
   if (hItem == NULL) TreeView_DeleteItem(hwnd, hParent);
}

void CCaraSdbDlg::SelectMaterial(HWND hwnd, TVITEM& tvItem)
{
   if (tvItem.lParam)
   {
      BoxData    *pBox  = (BoxData*)tvItem.lParam;
      BITMAPINFO *pBmi = NULL;
      int         nMat = pBox->nMaterial;
      char *str = LoadString(IDS_SELECT_LS_MATERIAL);
      if (str!=NULL) 
      {
         if (CCaraMat::SelectMaterial(m_hWnd, &nMat, str))
         {
            if ((nMat != 0) && (nMat != pBox->nMaterial))
            {
               pBox->nMaterial = nMat;
               OnSelectChangedBox(NULL);
            }
         }
         free(str);
      }
   }
}

/******************************************************************************
* Name       : OnDropFiles                                                    *
* Zweck      : Kopieren von Boxendateien aus anderen Verzeichnissen in das    *
*              Ls_Box-Verzeichnis.                                            *
* Definition : void OnDropFiles(HDROP);                                       *
* Aufruf     : durch die Nachricht WM_DROPFILES                               *
* Parameter  :                                                                *
* hDrop   (E): Struktur HDROP__*                                     (HDROP)  *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraSdbDlg::OnDropFiles(HDROP hDrop)
{

   UINT i, nFiles  = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
   bool bChanged = false;
   if (nFiles > 0)
   {
      m_bCanUpdateBoxView = false;
      ETSLSMB_CLSMB sLSMB;                                    // LoadSaveMsgBox initialisieren
      sLSMB.nLoadSave = 1;

      for (sLSMB.nLines=0; sLSMB.nLines<(int)nFiles; sLSMB.nLines++)                                 // alle Files kopieren
      {
         if (sLSMB.nLines>=ETSLSMB_MAX_STRING) break;
         DragQueryFile(hDrop, sLSMB.nLines, sLSMB.szText[sLSMB.nLines], ETSLSMB_STRINGLENGTH);
      }
      CEtsLsmb CLSMB(&sLSMB);

      char szLsBoxPath[_MAX_PATH], szFilePath[_MAX_PATH], *ptr;// Boxendateipfad ermitteln
      GetLsBoxPath(szLsBoxPath);
      int nPathLength = strlen(szLsBoxPath)-1;
      CCaraUnzip unzip;                                        // Unzip Dll-Instanz

      for (i=0; i<nFiles; i++)                                 // alle Files kopieren
      {
         DragQueryFile(hDrop, i, szFilePath, _MAX_PATH);       // Dateinamen ermitteln
         _strupr(szFilePath);
         ptr = strstr(szFilePath, ".BDD");                     // Nur BDD-Files
         if (ptr)
         {
            if (strnicmp(szLsBoxPath, szFilePath, nPathLength) == 0)
               continue;
            while((*ptr) != '\\') ptr--;
            szLsBoxPath[nPathLength] = 0;
            strcat(&szLsBoxPath[nPathLength], ptr);
            if (CopyFile(szFilePath, szLsBoxPath, true))       // kopieren
               bChanged = true;                                // wurde kopiert, Changed = true
         }
         ptr = strstr(szFilePath, ".ZIP");                     // Nur ZIP-Files
         if (ptr)
         {
            szLsBoxPath[nPathLength] = 0;
            if (CCaraUnzip::Unzip(szFilePath, szLsBoxPath)>0)
               bChanged = true;
         }
         CLSMB.Step();
      }
      m_bCanUpdateBoxView = true;
      if (bChanged) UpdateDataBase();                          // Datenbank Updaten
      unzip.Destructor();                                      // Unzip-Dll entfernen
   }
   DragFinish(hDrop);
}

/******************************************************************************
* Name       : LoadBoxFiles                                                   *
* Zweck      : Importieren von Boxendateien aus anderen Verzeichnissen in das *
*              Ls_Box-Verzeichnis.                                            *
*              TestVersion für die Benutzung der Funktion SHFileOperation();  *
* Definition : void LoadBoxFiles();                                           *
* Aufruf     : LoadBoxFiles();                                                *
* Parameter  :                                                                *
* hDrop   (E): Struktur HDROP__*                                     (HDROP)  *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraSdbDlg::LoadBoxFiles()
{
   char  szLsBoxPath[_MAX_PATH];
   GetLsBoxPath(szLsBoxPath);                               // Lautsprecherpfad ermitteln
   CSdbFileDlg dlg(m_hInstance, 0, m_hWnd);
   dlg.SetCaraSdbDlg(this);
   if (ETSFoundInDefault("CARAUNZP.DLL"))
   {
      dlg.SetFilter(IDS_LOAD_BOX_FILTER2);
      dlg.m_nPreSel = 1;
   }
   else
   {
      dlg.SetFilter(IDS_LOAD_BOX_FILTER);
   }

   dlg.SetInitialDir(szLsBoxPath);
   dlg.SetTitle(IDS_LOAD_BOX_TITLE);
   dlg.ModifyFlags(OFN_FILEMUSTEXIST|OFN_ALLOWMULTISELECT|OFN_NOCHANGEDIR, OFN_SHOWHELP|OFN_HIDEREADONLY);
   dlg.SetMaxPathLength(4096);

   BOOL bOk = dlg.GetOpenFileName();

   OnSelectChangedBox();
   if(bOk)
   {
      int   nFNameLength = dlg.GetOffset();                    // Offset ist die Länge des Pfades
      char *pszPathName  = dlg.GetPathName();
      pszPathName[nFNameLength] = 0;
      if (dlg.GetFilterIndex() == 1)                           // Boxendateien 
      {
         char       *ptr;
         const char *pszFileName;
         int   nFp = 0, nBoxes = 0;
         SHFILEOPSTRUCT shFop = { m_hWnd, FO_COPY, NULL, szLsBoxPath,
                                  FOF_SIMPLEPROGRESS, false, NULL, dlg.LoadString(IDS_LOAD_BOX_FILES)};
         try
         {
            if (!pszPathName) throw (int)3;
            if (strnicmp(szLsBoxPath, pszPathName, nFNameLength-1) == 0)
               throw (int)4;
            pszFileName  = dlg.GetFileTitle();                 // ersten Dateinamen holen
            while (pszFileName)                                // vorhanden ?
            {
               _strupr((char*)pszFileName);                    // in große Buchstaben wandeln
               ptr = strstr(pszFileName, ".BDD");              // Endung vergleichen
               if (ptr)                                        // nur BDD Dateien kopieren
               {                                               // Länge anpassen
                  shFop.pFrom = (char*)realloc((void*)shFop.pFrom, nFp + nFNameLength + strlen(pszFileName)+10);
                  if (!shFop.pFrom) throw (int) 1;             // Fehler beim allociren ?
                  wsprintf((char*)&shFop.pFrom[nFp], "%s\\%s", pszPathName, pszFileName);
                  nFp += strlen(&shFop.pFrom[nFp]) + 1;        // Pfad und Namen kopieren, Zeiger erhöhen
                  nBoxes++;
               } 
               pszFileName = dlg.GetFileTitle();               // nächsten Dateinamen holen
            }
            if (!shFop.pFrom) throw (int) 0;                   // wurde nichts kopiert : Ende
            ((char*)shFop.pFrom)[nFp++] = 0;                   // mit doppelnull abschließen
            ((char*)shFop.pFrom)[nFp]   = 0;

            m_bCanUpdateBoxView = false;
            int nOp = SHFileOperation(&shFop);                 // Kopieren
            m_bCanUpdateBoxView = true;
            if (nOp==0) UpdateDataBase();                      // Updaten der Datenbank
         }
         catch(int nError)
         {
            REPORT("Filecopy Error %d", nError);
         }
         if (shFop.pFrom            ) free((void*)shFop.pFrom);
         if (shFop.lpszProgressTitle) free((void*)shFop.lpszProgressTitle);
         if (pszPathName)             free(pszPathName);
      }
      else if (dlg.GetFilterIndex() == 2)
      {
         const char *pszFileName = dlg.GetFileTitle();         // erstes File holen
         char  szZipFilePathName[_MAX_PATH];                   // Pfad für ZipFiles
         int   nAllFiles = 0;                                  // Zähler für entpackte BDD-Files
         CCaraUnzip unzip;                                     // Unzip Dll-Instanz
         m_bCanUpdateBoxView = false;                          // Updaten der Datenbank: nein
         while (pszFileName)
         {
            wsprintf(szZipFilePathName, "%s\\%s", pszPathName, pszFileName);
            int nFiles = CCaraUnzip::Unzip(szZipFilePathName, szLsBoxPath);
            if (nFiles > 0) nAllFiles += nFiles;               // entpackte Files liefern
            if (dlg.m_bDeleteZipFiles)                         // ZipDatei löschen
               ::DeleteFile(szZipFilePathName);
            pszFileName = dlg.GetFileTitle();                  // nächsten ZipFile holen
         }
         m_bCanUpdateBoxView = true;                           // Updaten der Datenbank: ja
         if (nAllFiles>0) UpdateDataBase();                    // Updaten der Datenbank
         unzip.Destructor();                                   // Unzip-Dll entfernen
      }
   }
}

void CCaraSdbDlg::UpdateDataBase(char *pszName)
{
   if (pszName)
   {
      strupr(pszName);
      DeleteBoxByName(pszName);
   }
}

void CCaraSdbDlg::DeleteBoxData(void *p)
{
   BoxData *pB = (BoxData*)p;
   if (pB->pszFiles) delete[] pB->pszFiles;
   delete pB;
}

void CCaraSdbDlg::DeleteManufacturerData(void *p)
{
   Manufacturer *pM = (Manufacturer*)p;
   delete pM->pBoxes;
   delete pM;
}

BOOL CCaraSdbDlg::CloseHandle(HANDLE &hObject)
{
   BOOL bReturn = true;
   if (INVALID_HANDLE_VALUE != hObject)
   {
      bReturn = ::CloseHandle(hObject);
      hObject = INVALID_HANDLE_VALUE;
   }
   return bReturn;
}

int CCaraSdbDlg::InsertBox(HWND hwndTree, HTREEITEM hItem, WPARAM wParam, LPARAM lParam)
{
   if (hItem == TVI_ROOT) return 1;
   BoxData      *pBox = (BoxData*)wParam;
   Manufacturer *pMan = (Manufacturer*)lParam;
   TVITEM tvItem;
   ZeroMemory(&tvItem, sizeof(TVITEM));
   tvItem.mask      = TVIF_HANDLE|TVIF_PARAM|TVIF_IMAGE;
   tvItem.hItem     = hItem;
   SendMessage(hwndTree, TVM_GETITEM, 0, (LPARAM)&tvItem);
   HTREEITEM hParent = NULL;
   HTREEITEM hMan    = NULL;
   bool bInsert      = false;
   
   if (pMan->pBoxes->GetCounter() == 1)
   {
      switch (tvItem.iImage)
      {
         case IMAGE_NONAME:
         case IMAGE_USER_DEFINED:
         case IMAGE_MANUFACTURER:
            hParent = TVI_ROOT;
            bInsert = true;
            break;
         default: return -1;
      }
   }
   else
   {
      if (tvItem.lParam == (LPARAM)pMan)
      {
         bInsert = true;
      }
   }
   if (bInsert)
   {
      TVINSERTSTRUCT  tvMan;                                      // Herstellerknoten
      tvMan.hParent      = hParent;
      tvMan.hInsertAfter = TVI_SORT;
      tvMan.itemex.mask  = TVIF_TEXT|TVIF_PARAM|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
      TVINSERTSTRUCT  tvBox;                                      // Boxenknoten
      tvBox.hInsertAfter = TVI_SORT;
      tvBox.itemex.mask  = TVIF_TEXT|TVIF_PARAM|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
      TVINSERTSTRUCT  tvSlaves;                                   // Slaveknoten
      tvSlaves.hInsertAfter = TVI_SORT;
      tvSlaves.itemex.mask    = TVIF_TEXT|TVIF_PARAM|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
      tvSlaves.itemex.iImage  = tvSlaves.itemex.iSelectedImage = IMAGE_SLAVES;
      if (!hParent) tvBox.hParent = hItem;
      InsertManufacturer(hwndTree, tvMan, tvBox, pMan, (hParent) ? true: false);
      InsertBox(hwndTree, tvBox, tvSlaves, pBox);
      return -1;
   }
   return 0;
}

int CCaraSdbDlg::DeleteItemByParam(HWND hwndTree, HTREEITEM hItem, WPARAM wParam, LPARAM lParam)
{
   if (hItem == TVI_ROOT) return 1;
   BoxData *pBox = (BoxData*)wParam;
   TVITEM tvItem;
   ZeroMemory(&tvItem, sizeof(TVITEM));
   tvItem.mask      = TVIF_HANDLE|TVIF_PARAM;
   tvItem.hItem     = hItem;
   SendMessage(hwndTree, TVM_GETITEM, 0, (LPARAM)&tvItem);
   if (tvItem.lParam == (LPARAM)pBox)
   {
      SendMessage(hwndTree, TVM_DELETEITEM, 0, (LPARAM)tvItem.hItem);
      return -1;
   }
   return 1;
}

void CCaraSdbDlg::InsertManufacturer(HWND hwndTree, TVINSERTSTRUCT &tvMan, TVINSERTSTRUCT &tvBox, Manufacturer*pM, bool bInsert)
{
   tvMan.itemex.lParam  = (LPARAM)pM;                         // Zeiger auf Herstellerstruktur
   tvBox.itemex.iImage  = tvBox.itemex.iSelectedImage = IMAGE_BOXES;
   if (strcmp(pM->szName, NO_NAMEBOX)==0)                   // Nonameboxen haben ein anderes Bild
   {
      tvMan.itemex.iImage = tvMan.itemex.iSelectedImage = IMAGE_NONAME;
      tvMan.itemex.pszText = &pM->szName[0];                  // Name des Herstellers
   }
   else if (pM->szName[0] == '?')
   {
      tvMan.itemex.iImage = tvMan.itemex.iSelectedImage = IMAGE_USER_DEFINED;
      tvBox.itemex.iImage = tvBox.itemex.iSelectedImage = IMAGE_USER_BOXES;
      tvMan.itemex.pszText = &pM->szName[1];                  // Name des Herstellers
   }
   else
   {
      tvMan.itemex.iImage = tvMan.itemex.iSelectedImage = IMAGE_MANUFACTURER;
      tvMan.itemex.pszText = &pM->szName[0];                // Name des Herstellers
   }
   if (bInsert) tvBox.hParent = (HTREEITEM) SendMessage(hwndTree, TVM_INSERTITEM, 0, (LPARAM)&tvMan);
}

void CCaraSdbDlg::InsertBox(HWND hwndTree, TVINSERTSTRUCT &tvBox, TVINSERTSTRUCT &tvSlaves, BoxData *pBox)
{
   tvBox.itemex.pszText = pBox->szName;               // Dateiname der Box
   tvBox.itemex.lParam  = (LPARAM)pBox;               // Zeiger auf BoxData-Struktur
   tvSlaves.hParent     = (HTREEITEM) SendMessage(hwndTree, TVM_INSERTITEM, 0, (LPARAM)&tvBox); // Box einfügen
}

void CCaraSdbDlg::UpdateAngle()
{
   if (m_pBoxView)
   {
      m_pBoxView->DoDataExchange(CARAWALK_SETANGLE, 0, &m_BoxPos);
   }
}

void CCaraSdbDlg::SetBoxData(char *szBoxFile)
{
   HANDLE hFile = CreateFile(szBoxFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
   if (hFile != INVALID_HANDLE_VALUE)
   {
      DWORD dwBytesRead, dwActFilePointer;
      bool bOk = false;
      CFileHeader fh;
      char szDescription[BOX_DESCRIPTION_LENGTH];
      char szEnhDescription[BOX_DESCRIPTION_LENGTH*2];
      ReadFile(hFile, &fh, sizeof(CFileHeader), &dwBytesRead, NULL);
      if (fh.IsType(BOX_SOUND_DATA))
      {  // Geometriedaten suchen
         dwActFilePointer = SetFilePointer(hFile, fh.GetLength(), NULL, FILE_CURRENT);
         ReadFile(hFile, &fh, sizeof(CFileHeader), &dwBytesRead, NULL);
         if (fh.IsType(BOX_GEOMETRIC_DATA))
         {  // Geometriedaten lesen
            SetFilePointer(hFile, sizeof(long)*2, NULL, FILE_CURRENT);
            ReadFile(hFile, szDescription, BOX_DESCRIPTION_LENGTH, &dwBytesRead, NULL);
            CopyForStaticCtrl(szEnhDescription, szDescription);
            SetDlgItemText(IDC_DESCRIPTION, szEnhDescription);
            bOk = true;
         }
      }
      CloseHandle(hFile);
      if (bOk && m_pBoxView)
      {
         bOk = m_pBoxView->DoDataExchange(CARAWALK_SETBOXDATA, MAKELONG(CARAWALK_SHOW_SCALE, 0xffff), szBoxFile);
         if (bOk) m_pBoxView->DoDataExchange(CARAWALK_SETANGLE, 0, &m_BoxPos);
      }
   }
}

int CCaraSdbDlg::CopyForStaticCtrl(LPTSTR strDest, LPCTSTR strSrc)
{
   int i, j;
   for (i=0, j=0; ; i++, j++)
   {
      strDest[j] = strSrc[i];
      if (strSrc[j] == '&') strDest[++j] = strSrc[i];
      if (strSrc[i] == 0) break;
   }
   return j;
}

#define  SOFTWARE_PATH_IE "Software\\Microsoft\\Internet Explorer"
#define  DOWNLOAD_DIR_IE  "Download Directory"

void CCaraSdbDlg::GetDownloadPath()
{
   if (m_pszOldDownloadPath) return;
   else
   {
      HKEY hKey = NULL;
      char szLsBoxPath[_MAX_PATH];  // Boxendateipfad ermitteln

      GetLsBoxPath(szLsBoxPath);
      if (::RegOpenKeyEx(HKEY_CURRENT_USER, SOFTWARE_PATH_IE, 0, KEY_READ|KEY_WRITE, &hKey)==ERROR_SUCCESS)
      {
         char szDownLoadPath[_MAX_PATH];
         DWORD dwType = REG_SZ, dwSize = _MAX_PATH;
         if (::RegQueryValueEx(hKey, DOWNLOAD_DIR_IE, NULL, &dwType, (BYTE*)szDownLoadPath, &dwSize)==ERROR_SUCCESS)
         {
            if ((dwType== REG_SZ)&&(dwSize!=0))
            {
               m_pszOldDownloadPath = _strdup(szDownLoadPath);
            }
         }
         ::RegSetValueEx(hKey, DOWNLOAD_DIR_IE, 0, REG_SZ, (CONST BYTE *)szLsBoxPath, strlen(szLsBoxPath));
      }
      if (hKey) ::RegCloseKey(hKey);
   }
}

void CCaraSdbDlg::SetDownloadPath(char *pszPath)
{
   if (pszPath != NULL)
   {
      HKEY hKey = NULL;
      if (::RegOpenKeyEx(HKEY_CURRENT_USER, SOFTWARE_PATH_IE, 0, KEY_WRITE, &hKey)==ERROR_SUCCESS)
      {
         ::RegSetValueEx(hKey, DOWNLOAD_DIR_IE, 0, REG_SZ, (CONST BYTE *)pszPath, sizeof(pszPath));
         ::RegCloseKey(hKey);
      }
   }
}
