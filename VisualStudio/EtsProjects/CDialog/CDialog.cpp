/********************************************************************/
/*                                                                  */
/*                     Library ETS-Dialog                           */
/*                                                                  */
/*        mit Klassen für Dialoge und PropertySheets                */
/*                                                                  */
/*     programmed by Rolf Kary-Ehlers                               */
/*                                                                  */
/*     Version 2.2            04.04.2002                            */
/*                                                                  */
/********************************************************************/
#ifndef _WINDOWS_
   #define  STRICT
   #include <WINDOWS.H>
#endif

#include <math.h>
#include <commctrl.h>
#include <winuser.h>

#include "CEtsDlg.h"
#include "CEtsPropertyPage.h"
#include "AfxRes.h"
#include "Debug.h"

#ifdef INCLUDE_ETS_HELP
   #include "CEtsHelp.h"
   #include "CEtsDiv.h"
#else
   #include <stdio.h>
#endif

#define COMPILE_MULTIMON_STUBS
#include "MultiMon.h"

#define STRINGSIZE 256
#define THIS_POINTER_CDIALOG     "CEtsDialog"
#define IDAPPLY                         10

#ifdef INCLUDE_ETS_HELP
   CEtsHelp g_EtsHelp;
   CEtsDiv  g_EtsDiv;
#endif

COLORREF CEtsDialog::gm_CustomColors[16] = {0x00ffffff,0x00ffffff,0x00ffffff,0x00ffffff,0x00ffffff,0x00ffffff,0x00ffffff,0x00ffffff,0x00ffffff,0x00ffffff,0x00ffffff,0x00ffffff,0x00ffffff,0x00ffffff,0x00ffffff,0x00ffffff};
HHOOK    CEtsDialog::gm_hDll_Hook               = NULL;
int      CEtsDialog::gm_nWindowThreadProcessID  = 0;
int      CEtsDialog::gm_nNonModalDlg            = 0;
HWND     CEtsDialog::gm_hWndTopLevelParent      = NULL;
int      CEtsDialog::gm_nIDAPPLY                = IDAPPLY;
int      CEtsDialog::gm_nIDFmtMinMaxDouble      = 0;
int      CEtsDialog::gm_nIDFmtMinMaxInt         = 0;
int      CEtsDialog::gm_nIDFmtNoNumber          = 0;
char *   CEtsDialog::gm_pszHelp                 = NULL;
#ifdef INCLUDE_ETS_HELP
   UINT  CEtsDialog::gm_nmode                   = ETSDIV_NM_STD|ETSDIV_NM_NO_END_NULLS; // Rundungsmodus NM_STD, NM_SCI, NM_ENG. Die Modi koennen mit | kombiniert werden.
#endif

CEtsDialog::CEtsDialog()
{
   m_hWnd = NULL;
   Init(NULL, 0, NULL);
}

CEtsDialog::CEtsDialog(HINSTANCE hInstance, int nID, HWND hWndParent)
{
   m_hWnd = NULL;
   Init(hInstance, nID, hWndParent);
}

CEtsDialog::~CEtsDialog()
{
   if ((!m_bModal) && (m_hWnd != NULL) &&  IsWindow(m_hWnd))
      DestroyWindow(m_hWnd);
}

#ifdef INCLUDE_ETS_HELP
void CEtsDialog::Destructor()
{
   g_EtsHelp.Destructor();
   g_EtsDiv.Destructor();
}
#else
void CEtsDialog::SetHelpPath(char *)
{

}
#endif

bool CEtsDialog::Init(HINSTANCE hInstance, int nID, HWND hWndParent)
{
   bool bReturn = true;
   if ((m_hWnd != NULL) || (hInstance == NULL) || (nID == 0)) bReturn = false;
   m_hInstance  = hInstance;
   m_nID        = nID;
   m_hWndParent = hWndParent;
   m_hWnd       = NULL;
   m_nFlags     = 0;
   m_hWndForeground = NULL;
   m_bModal     = true;
   m_lOlpParentSubClass = 0;
   m_hwndDock = NULL;
   return bReturn;
}

/******************************************************************************
* Name       : SendDlgItemMessage                                             *
* Zweck      : Senden einer Nachricht an den Dialogitem                       *
* Aufruf     : SendDlgItemMessage(nIDDlgItem, Msg, wParam, lParam);           *
* Parameter  :                                                                *
* nIDDlgItem(E): DialogitemID                                        (int)    *
* Msg       (E): Nachrichten ID                                      (UINT)   *
* wParam    (E): wParam der Nachricht                                (WPARAM) *
* lParam    (E): lParam der Nachricht                                (LPARAM) *
* Funktionswert : Rückgebewert von SendMessage(..)                   (LONG)   *
******************************************************************************/
LONG CEtsDialog::SendDlgItemMessage(int nIDDlgItem, UINT Msg, WPARAM wParam, LPARAM lParam)
{
   if (m_hWnd)
   {
      ASSERT(::IsWindow(m_hWnd));
      return ::SendDlgItemMessage(m_hWnd, nIDDlgItem, Msg, wParam, lParam);
   }
   else return 0;
}

/******************************************************************************
* Name       : SetDlgItemText                                                 *
* Zweck      : Setzen des Textes eines Dialogitemfensters                     *
* Aufruf     : SetDlgItemText(nIDDlgItem, lpString);                          *
* Parameter  :                                                                *
* nIDDlgItem(E): DialogitemID                                        (int)    *
* lpString  (E): Text                                                (LPCTSTR)*
* Funktionswert : (true, false)                                      (bool)   *
******************************************************************************/
BOOL CEtsDialog::SetDlgItemText(int nIDDlgItem, LPCTSTR lpString, int nMaxLen)
{
   if (m_hWnd)
   {
      ASSERT(::IsWindow(m_hWnd));
      if (nMaxLen > 0) ::SendDlgItemMessage(m_hWnd, nIDDlgItem, EM_LIMITTEXT, (WPARAM)nMaxLen, 0);
      BOOL bReturn = ::SetDlgItemText(m_hWnd, nIDDlgItem, lpString);
      SendDlgItemMessage(nIDDlgItem, EM_SETMODIFY, 0, 0);
      return bReturn;
   }
   else return 0;
}

/******************************************************************************
* Name       : SetDlgItemInt                                                  *
* Zweck      : Setzen des Integerwertes eines Dialogitemfensters              *
* Aufruf     : SetDlgItemInt(nIDDlgItem, nValue, bSigned);                    *
* Parameter  :                                                                *
* nIDDlgItem(E): DialogitemID                                        (int)    *
* nValue    (E): Integerwert                                         (int)    *
* bSigned   (E): Vorzeichen (true, false)                            (bool)   *
* Funktionswert : (true, false)                                      (bool)   *
******************************************************************************/
BOOL CEtsDialog::SetDlgItemInt(int nIDDlgItem, int nValue, bool bSigned)
{
   if (m_hWnd)
   {
      ASSERT(::IsWindow(m_hWnd));
      BOOL bReturn = ::SetDlgItemInt(m_hWnd, nIDDlgItem, nValue, bSigned);
      SendDlgItemMessage(nIDDlgItem, EM_SETMODIFY, 0, 0);
      return bReturn;
   }
   else return 0;
}

/******************************************************************************
* Name       : GetDlgItem                                                     *
* Zweck      : liefert den Fensterhandle des Dialogitems                      *
* Aufruf     : GetDlgItem(nID);                                               *
* Parameter  :                                                                *
* nID     (E): ID des Dialogitems                                    (int)    *
* Funktionswert : Fensterhandle des Dialogitems                      (HWND)   *
******************************************************************************/
HWND CEtsDialog::GetDlgItem(int nID)
{
   if (m_hWnd)
   {
      ASSERT(::IsWindow(m_hWnd));
      return ::GetDlgItem(m_hWnd, nID);
   }
   else return NULL;
}

/******************************************************************************
* Name       : GetDlgRadioBtn                                                 *
* Definition : int GetDlgRadioBtn(nID);                                       *
* Zweck      : Ermittelt den Index des markierten Radiobuttons.               *
* Aufruf     : GetDlgRadioBtn(nID);                                           *
* Parameter  :                                                                *
* nID     (E): Erster Dialogelement in einer Radiobuttongruppe. Dieses  (int) *
*              muß den Windowstil WS_GROUP haben. Alle anderen dürfen diesen  *
*              Stil nicht haben! Das nächste Dialogelement mit dem Stil       *
*              WS_GROUP beendet die Radiobuttongruppe.                        *
* Funktionswert : Index des markierten Radiobuttons                  (int)    *
******************************************************************************/
int CEtsDialog::GetDlgRadioBtn(int nID)
{
   HWND hWndDlgItem = GetDlgItem(nID);
   ASSERT(::IsWindow(hWndDlgItem));
   long lstyle = ::GetWindowLong(hWndDlgItem, GWL_STYLE);
   ASSERT(lstyle & WS_GROUP);
   //               pn[0]            , pn[1], pn[2], pn[3]
   long plArr[4] = {(long)hWndDlgItem,  -1  , -1   , 1    };

   ::EnumChildWindows(m_hWnd, CEtsDialog::EnumRadioItems, (LPARAM)plArr);
   return plArr[2];
}

/******************************************************************************
* Name       : CheckDlgRadioBtn                                               *
* Definition : bool CheckDlgRadioBtn(int, int);                               *
* Zweck      : Markieren des Radiobuttons mit dem Index nIBtn in der          *
*              Radiobuttongruppe.                                             *
* Aufruf     : CheckDlgRadioBtn(nID, nIBtn);                                  *
* Parameter  :                                                                *
* nID     (E): Erster Dialogelement in einer Radiobuttongruppe. Dieses  (int) *
*              muß den Windowstil WS_GROUP haben. Alle anderen dürfen diesen  *
*              Stil nicht haben! Das nächste Dialogelement mit dem Stil       *
*              WS_GROUP beendet die Radiobuttongruppe.                        *
* nBItn   (E): Index des Buttons der markiert werden soll.                    *
* [nCount](E): Anzahl der Buttons                                       (int) *
* Funktionswert : (true, false)                                        (bool) *
******************************************************************************/
bool CEtsDialog::CheckDlgRadioBtn(int nID, int nIBtn)
{
   HWND hWndDlgItem = GetDlgItem(nID);
   ASSERT(::IsWindow(hWndDlgItem));
   long lstyle = ::GetWindowLong(hWndDlgItem, GWL_STYLE);
   ASSERT(lstyle & WS_GROUP);
   //               pn[0]            , pn[1], pn[2]      , pn[3]
   long plArr[4] = {(long)hWndDlgItem,  -1  , (long)nIBtn, 0    };

   ::EnumChildWindows(m_hWnd, CEtsDialog::EnumRadioItems, (LPARAM)plArr);
   return (plArr[0] != NULL) ? true : false;
}

BOOL CALLBACK CEtsDialog::EnumRadioItems(HWND hWnd, LPARAM lParam)
{
   long *pn = (long*) lParam;
   if (hWnd == (HWND) pn[0]) pn[1] = 0;  // erster Radiobutton;

   if (pn[1] != -1)
   {
      ASSERT(::IsWindow((HWND) pn[0]));
      long lstyle = ::GetWindowLong(hWnd, GWL_STYLE);
//      long luser  = ::GetWindowLong(hWnd, GWL_USERDATA);
      if ((lstyle & BS_AUTORADIOBUTTON) == 0) return false;
      if (pn[3] == 0)         // CheckButton
      {
         ::SendMessage(hWnd, BM_SETCHECK, (pn[1] == pn[2]) ? BST_CHECKED : BST_UNCHECKED, 0);
      }
      else                    // GetCheckedButton
      {
         if (::SendMessage(hWnd, BM_GETCHECK, 0, 0))
         {
            pn[2] = pn[1];
            return false;
         }
      }
      if ((pn[1] > 0) && (lstyle & WS_GROUP)) return false;
      pn[1]++;
   }
   return true;
}

BOOL CALLBACK CEtsDialog::EnumBmpButtons(HWND hWnd, LPARAM lParam)
{
   long lstyle = ::GetWindowLong(hWnd, GWL_STYLE);
   if (lstyle & (BS_BITMAP|BS_ICON))
   {
      UINT uType = ((lstyle & BS_BITMAP)!=0) ? IMAGE_BITMAP : IMAGE_ICON;
      CEtsDialog *pThis = (CEtsDialog*)((long*)lParam)[0];
      int nID = ::GetWindowLong(hWnd, GWL_ID);
      HANDLE hImageLoad, hImageCtrl = NULL;
      if (((long*)lParam)[1] != 0)
      {
         hImageLoad = ::LoadImage(pThis->m_hInstance, MAKEINTRESOURCE(nID), uType,
                                                  ((long*)lParam)[2], ((long*)lParam)[3], LR_DEFAULTCOLOR);
         if (hImageLoad)
         {
            ((long*)lParam)[1] = 2;
            hImageCtrl = (HANDLE) pThis->SendDlgItemMessage(nID, BM_SETIMAGE, (WPARAM)uType, (LPARAM)hImageLoad);
         }
      }
      else
      {
         hImageCtrl = (HANDLE) pThis->SendDlgItemMessage(nID, BM_SETIMAGE, (WPARAM)uType, NULL);
      }
      if (hImageCtrl)
      {
         if (IMAGE_BITMAP==uType) ::DeleteObject(hImageCtrl);
         else                     ::DestroyIcon((HICON)hImageCtrl);
      }
   }
   return true;
}

/******************************************************************************
* Name       : EnableGroupItems                                               *
* Definition : void EnableGroupItems(HWND, bool, long);                       *
* Zweck      : Enablen oder disablen von Dialogelementen innerhalb einer      *
*              Gruppe (GROUPBOX-Control).                                     *
* Aufruf     : EnableGroupItems(hWndGroup, bEnable, lFlags);                  *
* Parameter  :                                                                *
* hWndGroup(E): WindowHandle der Gruppe                                       *
* bEnable  (E): (true, false)=>(Enable, Disable)                       (bool) *
* lFlags   (E): Windowstil Flags um Dialogelemente auszuschließen      (long) *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CEtsDialog::EnableGroupItems(HWND hWndGroup, bool bEnable, long lFlags)
{
   RECT rcGroup;
   long plArr[3] = {(long)bEnable, lFlags, (long)&rcGroup};
   ASSERT(::IsWindow(hWndGroup));
   if (::GetWindowRect(hWndGroup, &rcGroup))
      ::EnumChildWindows(m_hWnd, CEtsDialog::EnumGroupItems, (LPARAM)plArr);
}

BOOL CALLBACK CEtsDialog::EnumGroupItems(HWND hWnd, LPARAM lParam)
{
   RECT rcDest, rcWnd;
   ASSERT(::IsWindow(hWnd));
   if (::GetWindowRect(hWnd, &rcWnd))
   {
      ASSERT(lParam != NULL);
      long *pl  = (long*) lParam;
      if (pl[1])
      {
         long lStyle = GetWindowLong(hWnd, GWL_STYLE);
         if ((lStyle & pl[1])== pl[1]) return true;
      }
      RECT * prc = (RECT*)pl[2];
      BOOL bSect = ::IntersectRect(&rcDest, prc, &rcWnd);
      if (bSect) EnableWindow(hWnd, pl[0]);
      return true;
   }
   return false;
}

/******************************************************************************
* Name       : GetDlgItemText                                                 *
* Zweck      : Ermittelt den Text eines Dialogitemfensters                    *
* Aufruf     : GetDlgItemText(nIDDlgItem, lpString, nMaxCount);               *
* Parameter  :                                                                *
* nIDDlgItem(E): DialogitemID                                        (int)    *
* lpString  (A): Text                                                (LPCTSTR)*
* nMaxCount (E): maximale Länge des Textstrings                      (int)    *
* Funktionswert : (true, false)                                      (bool)   *
******************************************************************************/
BOOL CEtsDialog::GetDlgItemText(int nIDDlgItem, LPTSTR lpString, bool *pbChanged, int nMaxLen)
{
   if (m_hWnd)
   {
      ASSERT(::IsWindow(m_hWnd));
      ASSERT(lpString != NULL);
      if (pbChanged)
      {
         *pbChanged = (SendDlgItemMessage(nIDDlgItem, EM_GETMODIFY, 0, 0)) ? true : false;
         SendDlgItemMessage(nIDDlgItem, EM_SETMODIFY, 0, 0);
      }
      return ::GetDlgItemText(m_hWnd, nIDDlgItem, lpString, nMaxLen);
   }
   else return 0;
}

/******************************************************************************
* Name       : GetDlgItemText                                                 *
* Zweck      : Ermittelt den Text eines Dialogitemfensters und alloziert      *
*              Speicherplatz für diesen Text mit der Funktion alloc(..).      *
* Hinweis !  : Der Speicherplatz muß mit der Funktion free(..) freigegeben    *
*              werden.                                                        *
* Aufruf     : GetDlgItemText(nIDDlgItem);                                    *
* Parameter  :                                                                *
* nIDDlgItem(E): DialogitemID                                        (int)    *
* Funktionswert : Zeiger auf Textarray                               (char*)  *
******************************************************************************/
char * CEtsDialog::GetDlgItemText(HWND hwndCtrl, char *pszOld, bool *pbChanged)
{
   char *pszNew = NULL;
   int nLen = ::GetWindowTextLength(hwndCtrl);
   if (nLen > 0)
   {
      nLen ++;
      if (pszOld == NULL) pszNew = (char*) malloc(nLen+1);
      else                pszNew = (char*) realloc(pszOld, nLen+1);
      if (pszNew)                                              // Speicherplatz allociert
      {
         ::GetWindowText(hwndCtrl, pszNew, nLen);
         pszNew[nLen] = 0;
      }
      else if (pszOld!= NULL)                                  // Fehler !
      {
         free(pszOld);                                         // Speicher freigeben
      }
   }
   else if (pszOld)                                            // Textlänge 0
   {
      free(pszOld);                                            // Speicher freigeben
   }
   if (pbChanged)
   {
      *pbChanged = (::SendMessage(hwndCtrl, EM_GETMODIFY, 0, 0)) ? true : false;
      ::SendMessage(hwndCtrl, EM_SETMODIFY, 0, 0);
   }
   return pszNew;
}

char * CEtsDialog::LoadString(int nID, int *pnLen)
{
/*
   OSVERSIONINFO osvi;
   osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
   BOOL bRet = GetVersionEx(& osvi);
   osvi.dwPlatformID == VER_PLATFORM_WIN32_CE

   LoadString
   Windows CE: If lpBuffer is set to NULL, the return value is a pointer to the requested string.
   The caller should cast the return value to an LPCTSTR. This pointer points directly to the resource,
   so the string is read-only. The length of the string, not including any NULL terminator, can be found
   in the word preceding the string.
*/
   int nLen, nSize = 256;
   if (pnLen) *pnLen = -1;
   char *pszNew = (char*) malloc(nSize);
   while (pszNew)
   {
      nLen= ::LoadString(m_hInstance, nID, pszNew, nSize);
      if (nLen == 0)
      {
         free(pszNew);
         break;
      }
      if (nLen < nSize-1)
//      if (nLen < nSize-20)
      {
//         char szID[32];
//         wsprintf(szID, " %d", nID);
         pszNew[nLen] = 0;
         if (pnLen) *pnLen = nLen;
//         strcat(pszNew, szID);         
         return pszNew;
      }
      else
      {
         nSize += 256;
         pszNew = (char*)realloc(pszNew, nSize);
      }
   }
   return NULL;
}

/******************************************************************************
* Name       : GetDlgItemInt                                                  *
* Zweck      : liefert den Integerwert eines Dialogitemfensters               *
* Aufruf     : GetDlgItemInt(nIDDlgItem, nValue, bSigned);                    *
* Parameter  :                                                                *
* nIDDlgItem(E): DialogitemID                                        (int)    *
* nValue    (A): Integerwert                                         (int&)   *
* bSigned   (E): Vorzeichen (true, false)                            (bool)   *
* Funktionswert : (true, false)                                      (bool)   *
******************************************************************************/
BOOL CEtsDialog::GetDlgItemInt(int nIDDlgItem, int &nValue, bool bSigned, int nDefault, bool *pbChanged)
{
   if (m_hWnd)
   {
      ASSERT(::IsWindow(m_hWnd));
      BOOL bReturn;
      int nReturn = ::GetDlgItemInt(m_hWnd, nIDDlgItem, &bReturn, bSigned);
      if (bReturn)
      {
         if (nValue != nReturn)
         {
            nValue = nReturn;
            if (pbChanged) *pbChanged = true;
         }
      }
      else
      {
         char text[STRINGSIZE];
         if (::LoadString(m_hInstance, gm_nIDFmtNoNumber, text, STRINGSIZE)==0)
            strcpy(text, "Format Error");
         CreateErrorWnd(nIDDlgItem, text, true);
         nValue = nDefault;
      }
      return bReturn;
   }
   else return false;
}

void CEtsDialog::OnApply() 
{
   OnOk(0);
}

/******************************************************************************
* Name       : DoModal                                                        *
* Zweck      : Aufruf der Dialogbox als Modales Dialogfenster                 *
* Aufruf     : DoModal();                                                     *
* Parameter  : keine                                                          *
* Funktionswert : (0, IDOK, IDCANCEL) Fehler, Ok, Abbruch               (int) *
******************************************************************************/
int CEtsDialog::DoModal()
{
   if ((m_hInstance != NULL) && (m_nID != 0))
   {
      m_bModal = true;
      return DialogBoxParam(m_hInstance, MAKEINTRESOURCE(m_nID), m_hWndParent,
                            (DLGPROC) DialogProc, (LPARAM) this);
   }
   return 0;
}

/******************************************************************************
* Name       : Create                                                         *
* Zweck      : Aufruf der Dialogbox als nichtmodales Dialogfenster.           *
* Aufruf     : Create()                                                       *
* Parameter  : keine                                                          *
* Funktionswert : Fensterhandle des Dialogfensters                   (HWND)   *
******************************************************************************/
HWND CEtsDialog::Create()
{
   if (m_hWnd && IsWindow(m_hWnd))
   {
      if (IsIconic(m_hWnd))
      {
         ChangeModalStyles(false);
         ShowWindow(m_hWnd, SW_RESTORE);
      }
      else
      {
         ChangeModalStyles(true);
         ShowWindow(m_hWnd, SW_MINIMIZE);
      }
      return m_hWnd;
   }
   else
   {
      if ((m_hInstance != NULL) && (m_nID != 0))
      {
         m_bModal = false;
         return CreateDialogParam(m_hInstance, MAKEINTRESOURCE(m_nID), m_hWndParent,
                               (DLGPROC) DialogProc, (LPARAM) this);
      }
      return NULL;
   }
}

/******************************************************************************
* Name       : SetChanged                                                     *
* Zweck      : Setzt den Zustand des Buttons mit der ID-Nummer                *
*              CEtsDialog::gm_nIDAPPLY, um den Zustand (nicht) geändert zu       *
*              speichern.                                                     *
* Aufruf     : SetChanged(bChanged);                                          *
* Parameter  :                                                                *
* bChanged(E): true  : Der Applybutton wird Enabled                    (bool) *
*              false : Der Applybutton wird Disabled                          *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CEtsDialog::SetChanged(bool bChanged)
{
   HWND hwndApply = GetDlgItem(CEtsDialog::gm_nIDAPPLY);
   if (hwndApply)
   {
      EnableWindow(hwndApply, bChanged);
   }
}

/******************************************************************************
* Name       : IsChanged                                                      *
* Zweck      : Fragt den Zustand des Applybuttons ab, um den Zustand (nicht)  *
*              geändert zu ermitteln.                                         *
* Aufruf     : IsChanged();                                                   *
* Parameter  : keine                                                          *
* Funktionswert : (true, false)                                      (BOOL)   *
******************************************************************************/
BOOL CEtsDialog::IsChanged()
{
   HWND hwndApply = GetDlgItem(CEtsDialog::gm_nIDAPPLY);
   if (hwndApply)
   {
      return IsWindowEnabled(hwndApply);
   }
   return false;
}

/******************************************************************************
* Name       : OnInitDialog                                                   *
* Zweck      : Initialisierung der Dialogboxparameter. Setzt Fensterhandle.   *
*              Bei einer nicht modalen Dialogbox wird eine Hookfunktion als   *
*              Messagefilter installiert, die die Dialog-Box-Messages für den *
*              nichtmodalen Dialog, der den Eingabefocus hat herausfiltert.   *
*              * siehe MsgFilterProc !!!                                      *
*              Wird diese Funktion überladen, muß sie am Anfang der           *
*              überladenen Funktion folgendermaßen aufgerufen werden :        *
*              CEtsDialog::OnInitDialog(hWnd);                                *
* Definition : virtual bool OnInitDialog(HWND hWnd);                          *
* Aufruf     : OnInitDialog(hWnd);                                            *
* Parameter  :                                                                *
* hWnd   (E) : Fensterhandle der Dialogbox                           (HWND)   *
* Funktionswert : (true, false)                                      (bool)   *
******************************************************************************/
bool CEtsDialog::OnInitDialog(HWND hWnd)
{
   HRSRC   hDlgInit;
   m_hWnd = hWnd;
   if (!m_bModal)                                              // ein nichtmodaler Dialog
   {
      HWND hP = ::GetParent(m_hWnd);
      if (hP == NULL)
         ::SetParent(m_hWnd, m_hWndParent);
      if (gm_hWndTopLevelParent == NULL)
      {
         gm_hWndTopLevelParent = GetTopLevelParent(m_hWndParent); // Top Level Parent !!
      }
      if ((gm_nNonModalDlg == 0) && (gm_nWindowThreadProcessID != 0) && (gm_hDll_Hook == NULL))
      {
         DWORD dwID = ::GetCurrentThreadId();
         gm_hDll_Hook = SetWindowsHookEx(WH_MSGFILTER, (HOOKPROC)MsgFilterProc, NULL, gm_nWindowThreadProcessID);
      }
      gm_nNonModalDlg++;                                       // Instanzen nicht modaler Dialoge zählen
   }

   long lStyleEx = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
   if (lStyleEx & WS_EX_CONTEXTHELP) 
      m_nFlags |= MODAL_W_CONTEXT_HELP;
   long lStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
   if (lStyle & (WS_MAXIMIZEBOX|WS_MINIMIZEBOX))
   {
      if (lStyle & WS_MAXIMIZEBOX) m_nFlags |= MODAL_W_MAX_BTN;
      if (lStyle & WS_MINIMIZEBOX) m_nFlags |= MODAL_W_MIN_BTN;
      ChangeModalStyles(false);
   }

   if (gm_hDll_Hook) SetProp(hWnd, THIS_POINTER_CDIALOG, gm_hDll_Hook);
   if (m_nID)
   {
      hDlgInit = ::FindResource(m_hInstance, MAKEINTRESOURCE(m_nID), RT_DLGINIT);
   	if (hDlgInit != NULL) ExecuteDlgInit(hDlgInit);
   }

   return true;
}

void CEtsDialog::ExecuteDlgInit(HRSRC hDlgInit)
{
	void   *pResource = NULL;
	HGLOBAL hResource = NULL;

   ASSERT(hDlgInit != NULL);
   hResource = ::LoadResource(m_hInstance, hDlgInit);
	if (hResource)
   {
      pResource = ::LockResource(hResource);
	   ASSERT(pResource != NULL);
      if (pResource) ExecuteDlgInit(pResource);
   }

   if ((pResource != NULL) && (hResource != NULL))
	{
		UnlockResource(hResource);
		FreeResource(hResource);
	}
}

void CEtsDialog::ExecuteDlgInit(void *pResource)
{
   ASSERT(pResource != NULL);
	UNALIGNED WORD* pnRes = (WORD*)pResource;
	BOOL bSuccess = TRUE;
	while (bSuccess && *pnRes != 0)
	{
		WORD nIDC = *pnRes++;
		WORD nMsg = *pnRes++;
		DWORD dwLen = *((UNALIGNED DWORD*&)pnRes)++;

		// In Win32 the WM_ messages have changed.  They have
		// to be translated from the 32-bit values to 16-bit
		// values here.

		#define WIN16_LB_ADDSTRING  0x0401
		#define WIN16_CB_ADDSTRING  0x0403
		#define AFX_CB_ADDSTRING    0x1234

		// unfortunately, WIN16_CB_ADDSTRING == CBEM_INSERTITEM
		if (nMsg == AFX_CB_ADDSTRING)
			nMsg = CBEM_INSERTITEM;
		else if (nMsg == WIN16_LB_ADDSTRING)
			nMsg = LB_ADDSTRING;
		else if (nMsg == WIN16_CB_ADDSTRING)
			nMsg = CB_ADDSTRING;

		ASSERT(nMsg == LB_ADDSTRING || nMsg == CB_ADDSTRING ||
			nMsg == CBEM_INSERTITEM);

#ifdef _DEBUG
		if (nMsg == LB_ADDSTRING || nMsg == CB_ADDSTRING || nMsg == CBEM_INSERTITEM)
			ASSERT(*((LPBYTE)pnRes + (UINT)dwLen - 1) == 0);
#endif

		if (nMsg == CBEM_INSERTITEM)
		{
			COMBOBOXEXITEM item;
			item.mask = CBEIF_TEXT;
			item.iItem   = -1;
			item.pszText = LPSTR(pnRes);

			if (::SendDlgItemMessage(m_hWnd, nIDC, nMsg, 0, (LPARAM) &item) == -1)
				bSuccess = FALSE;
		}
		else if (nMsg == LB_ADDSTRING || nMsg == CB_ADDSTRING)
		{
			// List/Combobox returns -1 for error
         char *pszText = (char*)pnRes;
			if (::SendDlgItemMessageA(m_hWnd, nIDC, nMsg, 0, (LPARAM) pnRes) == -1)
				bSuccess = FALSE;
		}

		// skip past data
		pnRes = (WORD*)((LPBYTE)pnRes + (UINT)dwLen);
	}
}

bool CEtsDialog::AutoInitBtnBmp(int cx, int cy)
{
   long plParam[5] = {(long)this, 1, (long)cx, (long)cy};
   ::EnumChildWindows(m_hWnd, CEtsDialog::EnumBmpButtons, (LPARAM)plParam);
   return (plParam[1] ==2) ? true : false;
}

void CEtsDialog::DeleteAutoBtnBmp()
{
   long plParam[3] = {(long)this, 0};
   ::EnumChildWindows(m_hWnd, CEtsDialog::EnumBmpButtons, (LPARAM)plParam);
}

/******************************************************************************
* Name       : OnEndDialog                                                    *
* Zweck      : Aufräumen der Dialogboxparameter und Variablen.                *
*              Wird diese Funktion überladen, muß sie am Ende der überladenen *
*              Funktion folgendermaßen aufgerufen werden :                    *
*              CEtsDialog::OnEndDialog(nResult);                              *
* Definition : virtual void OnEndDialog(int);                                 *
* Aufruf     : OnEndDialog(nResult);                                          *
* Parameter  :                                                                *
* nResult (E): Rückgabewert der Dialogbox                               (int) *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CEtsDialog::OnEndDialog(int nResult)
{
   if (m_hWnd)
   {
      ASSERT(::IsWindow(m_hWnd));
      RemoveDockProc();
      if (::GetProp(m_hWnd, THIS_POINTER_CDIALOG))
      {
         ::RemoveProp(m_hWnd, THIS_POINTER_CDIALOG);
      }
      if (m_bModal)
      {
         VERIFY(this == (CEtsDialog*) ::SetWindowLong(m_hWnd, DWL_USER, NULL));
         EndDialog(m_hWnd, nResult);
      }
      else
      {
         DestroyWindow(m_hWnd);
      }
      m_hWnd = NULL;
   }
}

/******************************************************************************
* Name       : OnOk                                                           *
* Zweck      : Posten einer WM_COMMAND Message an das Parentfenster mit der   *
*              ID der Dialogboxresource, einem NotificationCode und dem       *
*              Fensterhandle der Dialogbox. Ist ein Applybutton vorhanden     *
*              wird nur dann gesendet, wenn dieser Enabled war.               *
*              Wird diese Funktion überladen, kann sie am Ende der überladenen*
*              Funktion folgendermaßen aufgerufen werden :                    *
*              return CEtsDialog::OnOk(nNotify);                                 *
* Definition : virtual int OnOk(WORD);                                        *
* Aufruf     : Durch WM_COMMAND mit IDOK bzw.                                 *
*              CEtsDialog::gm_nIDAPPLY (durch die Funktion OnApply())            *
* Parameter  :                                                                *
* Funktionswert : (IDOK, ...)                                           (int) *
* Liefert diese Funktion IDOK, so wird die Dialogbox beendet.                 *
******************************************************************************/
int CEtsDialog::OnOk(WORD nNotify)
{
   bool bChanged  = true;
   HWND hwndApply = GetDlgItem(CEtsDialog::gm_nIDAPPLY);
   if (hwndApply)                                           // ist ein Apply-Button vorhanden
   {
      bChanged = (IsWindowEnabled(hwndApply)!=0) ? true : false;
      EnableWindow(hwndApply, false);                       // Apply-Button Disablen
   }
   if (bChanged)                                            // wenn sich etwas geändert hat und
   {
      HWND hwndParent = m_hWndParent;
      if (!IsWindow(hwndParent)) hwndParent = GetParent(m_hWnd);
      if (IsWindow(hwndParent))                             // ein Elternfenster vorhanden ist :
      {                                                     // Posten der Nachricht WM_COMMAND, mit der ID des Dialogfeldes, nNotify, Fensterhandle des Dialogfeldes
         WPARAM wParam = MAKELONG(m_nID, nNotify);
         LPARAM lParam = (LPARAM)m_hWnd;
         PostMessage(hwndParent, WM_COMMAND, wParam, lParam);
      }
   }
   return IDOK;
}

/******************************************************************************
* Name       : OnCommand                                                      *
* Zweck      : Verarbeitung der WM_COMMAND Nachrichten an die Dialogbox       *
*              Zum Empfangen der Nachrichten muß diese Funktion überladen     *
*              werden.                                                        *
* Definition : virtual int OnCommand(WORD, WORD, HWND);                       *
* Aufruf     : OnCommand(nID , nNotifyCode, hwndControl);                     *
*              durch die Dialogfunktion bei einer WM_COMMAND-Nachricht        *
* Parameter  :                                                                *
* nID        (E): Control ID des Absenders.                            (WORD) *
* nNotifyCode(E): Notificationcode                                     (WORD) *
* hwndControl(E): Fensterhandle des Absenders                          (HWND) *
* Funktionswert : Nachricht verarbeitet (0, 1)                         (int)  *
******************************************************************************/
int CEtsDialog::OnCommand(WORD nID , WORD nNotifyCode, HWND hwndControl)
{
   return true;
}

/******************************************************************************
* Name       : OnMessage                                                      *
* Zweck      : Verarbeitung der Nachrichten an die Dialogbox.                 *
*              Wird diese Funktion überladen, so muß sie, wenn die Nachricht  *
*              nicht verarbeitet wurde, am Ende der überladenen Funktion      *
*              folgendermaßen aufgerufen werden:                              *
*              CEtsDialog::OnMessage(nMsg, wParam, lParam);                      *
* Definition : virtual int OnMessage(UINT, WPARAM, LPARAM);                   *
* Aufruf     : OnMessage(nMsg, wParam, lParam);                               *
*              durch die Dialogfunktion zur Verarbeitung der noch nicht       *
*              verarbeiteten Nachrichten.                                     *
* Parameter  :                                                                *
* nMsg   (E) : Message ID                                            (UINT)   *
* wParam (E) : erster Parameter der Nachricht                        (LPARAM) *
* lParam (E) : zweiter Parameter der Nachricht                       (WPARAM) *
* Funktionswert : (0, 1) Nachricht verarbeitet                       (int)    *
******************************************************************************/
int CEtsDialog::OnMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
//   TRACE("Msg:%x\n", nMsg);
   switch (nMsg)
   {
      case WM_ENTERMENULOOP:
      {
         m_nFlags |= IS_IN_MENU_LOOP;
      }break;
      case WM_EXITMENULOOP:
         m_nFlags &= ~IS_IN_MENU_LOOP;
         break;
      case WM_SYSCOMMAND:
      {
         if      (wParam == SC_MINIMIZE) ChangeModalStyles(true);
         else if (wParam == SC_RESTORE)  ChangeModalStyles(false);
      }break;
      case WM_DESTROY:
      if (!m_bModal)                                           // ein nichtmodaler Dialog
      {
         if (::GetProp(m_hWnd, THIS_POINTER_CDIALOG))
         {
            ::RemoveProp(m_hWnd, THIS_POINTER_CDIALOG);
         }
         gm_nNonModalDlg--;                                    // und der Instanzenzähler verringert
         if ((gm_nNonModalDlg == 0) && (gm_hDll_Hook != NULL)) // ist kein nichtmodaler Dialog mehr da
         {
            ::UnhookWindowsHookEx(gm_hDll_Hook);               // Nachrichten Hook entfernen
            gm_hDll_Hook = NULL;                               // Hook Handle auf 0 setzen
         }
         VERIFY(this == (CEtsDialog*) ::SetWindowLong(m_hWnd, DWL_USER, NULL));
         ::DefDlgProc(m_hWnd, nMsg, wParam, lParam);
         m_hWnd = 0;
      } break;
      case WM_EXITSIZEMOVE: return OnExitSizeMove(wParam, (HWND)lParam);
      case WM_HELP:
      if (lParam)
      {
         HELPINFO *pHI = (HELPINFO*) lParam;
         if ((pHI->iContextType == HELPINFO_WINDOW) || (pHI->iContextType == HELPINFO_MENUITEM))
         {
#ifdef INCLUDE_ETS_HELP
            char *ptext;
            if ((pHI->iCtrlId >= IDOK) && (pHI->iCtrlId <= IDAPPLY))
               pHI->iCtrlId += ID_STRING_OFFSET;
            ptext = LoadString(pHI->iCtrlId);
            if (ptext)
            {
               CEtsHelp::CreateContextWnd(ptext, pHI->MousePos.x, pHI->MousePos.y);
               free(ptext);
               if (wParam) *((int*)wParam) = 1;
            }
            else if ((pHI->dwContextId !=0) && ((ptext = LoadString(pHI->dwContextId)) !=NULL))
            {
               CEtsHelp::CreateContextWnd(ptext, pHI->MousePos.x, pHI->MousePos.y);
               free(ptext);
               if (wParam) *((int*)wParam) = 1;
            }
#else
            if (gm_pszHelp)
            {
               if (pHI->iContextType == HELPINFO_WINDOW)                   // Hilfe für ein Control oder window
               {
                  WinHelp(m_hWnd, gm_pszHelp, pHI->dwContextId, HELP_CONTEXTPOPUP);
               }
               else                                                        // Hilfe für einen Menüpunkt
               {
                  WinHelp(m_hWnd, gm_pszHelp, pHI->dwContextId, HELP_FINDER);
               }
            }
#endif
         }
         return true;
      }  break;
      default:
         break;
   }
   return 0;
}

/******************************************************************************
* Name       : SetComboBoxStrings                                             *
* Zweck      : Setzen der Combobox Strings durch einen Resourcenstring, der   *
*              mehrere durch "\n" getrennte Strings enthalten kann.           *
* Aufruf     : SetComboBoxStrings(nIDListBox, nIDString);                     *
* Parameter  :                                                                *
* nIDListBox(E): Resourcen-ID der Listbox                               (int) *
* nIDString (E): Resourcen-ID des Strings                               (int) *
* Funktionswert : (true, false)                                         (bool)*
******************************************************************************/
bool CEtsDialog::SetComboBoxStrings(int nIDListBox, int nIDString)
{
   char *ptext = LoadString(nIDString);
   if (ptext)
   {
      char *ptr1 = ptext;
      char *ptr2 = NULL;
      while (ptr1)
      {
         ptr2    = strstr(ptr1, "\n");
         if (ptr2)
         {
            ptr2[0] = 0;
            ptr2++;
         }
         SendDlgItemMessage(nIDListBox, CB_ADDSTRING, 0, (LPARAM)ptr1);
         ptr1 = ptr2;
      }
      free(ptext);
      return true;
   }
   return false;
}

/******************************************************************************
* Name       : SetDlgItemDouble                                               *
* Zweck      : Konvertiert eine double-Zahl in einen String und setzt diesen  *
*              als Windowtext des Dialogfeldes.                               *
* Aufruf     : SetDlgItemDouble(nIDDlgItem, dValue, nDec);                    *
* Parameter  :                                                                *
* nIDDlgItem(E): ID des Dialog Items                                 (int)    *
* dValue  (E): zu konvertierende Zahl                                (double) *
* nDec    (E): Anzahl der Stellen hinter dem Komma (0,..,9)          (int)    *
* Funktionswert : (true, false)                                      (bool)   *
******************************************************************************/
bool CEtsDialog::SetDlgItemDouble(int nIDDlgItem, double dValue, int nDec)
{
   char text[STRINGSIZE];              
   bool bOK = false;
#ifdef INCLUDE_ETS_HELP
   ETSDIV_NUTOTXT ntx = {dValue, gm_nmode, abs(nDec), STRINGSIZE, 0};
   bOK = (CEtsDiv::NumberToText(&ntx, text)!=0) ? true : false;
#endif
   if (!bOK)
   {
      if (((dValue >  2147483647.0) || (dValue < -2147483648.0)) && (nDec > 0)) nDec = -nDec;
#ifndef INCLUDE_ETS_HELP
      if (nDec <= 0)
      {
         char  format[16];
         wsprintf(format, "%%.%df", -nDec);
         sprintf(text, format, dValue);
      }
      else if (nDec <= 9)
#else
      if (nDec <= 0) nDec = -nDec;
#endif
      {
         char szDecimal[2];
         if (GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, szDecimal, 2)==0)
            szDecimal[0] = ',';
         char  format[] = "%d%c%0xd";
         format[6] = '0' + (char) nDec;
         int nNum = (int) dValue;
         nDec = (int) pow(10.0, (double)nDec);
         double dRound = 0.5 / (double) nDec;
         if (nNum < 0) dRound = -dRound;
         dValue = dValue + dRound - (double)nNum;
         nDec   = (int)(dValue * nDec);
         if (nDec != 0)
            wsprintf(text, format, nNum, szDecimal[0], abs(nDec));
         else
            wsprintf(text, "%d", nNum);
      }
   }
//   nDec = (int) ((dValue - (double)nNum) * nDec);
   return (SetDlgItemText(nIDDlgItem, text) != 0) ? true : false;
}

/******************************************************************************
* Name       : GetDlgItemDouble                                               *
* Zweck      : Konvertiert den Windowtext eines Dialogfeldes in eine          *
*              double-Zahl.                                                   *
* Aufruf     : GetDlgItemDouble(nIDDlgItem, dValue);                          *
* Parameter  :                                                                *
* nIDDlgItem(E): ID des Dialog Items                                 (int)    *
* dValue  (A): Konvertierte Zahl                                     (doubel&)*
* Funktionswert : (true, false)                                      (bool)   *
******************************************************************************/
bool CEtsDialog::GetDlgItemDouble(int nIDDlgItem, double& dValue, double dDefault, bool *pbChanged)
{
   char text[STRINGSIZE];

   if (GetDlgItemText(nIDDlgItem, text, pbChanged, STRINGSIZE)!=0)
   {
      if ((pbChanged != 0) && (*pbChanged == false)) return false;
      int i, c, nComma = 0, nPoint = 0;

      char szDecimal[4];
      if (GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, szDecimal, 4)==0)
         szDecimal[0] = ',';

      for (i=0; ((c=text[i])!=0) && (i<STRINGSIZE); i++)
      {
         if (c == (int)'.') {text[i] = szDecimal[0]; continue;}
//         if (c == (int)',') {text[i] = szDecimal[0]; continue;}
         if (!isdigit(c) &&                                    // Nummern
             (c != (int)szDecimal[0]) &&                       // Dezimalzeichen
             (c != (int)'-') && (c != (int)'+') &&             // Vorzeichen
             (c != (int)' '))
         {
            if (::LoadString(m_hInstance, gm_nIDFmtNoNumber, text, STRINGSIZE)==0)
               strcpy(text, "Format Error");
            CreateErrorWnd(nIDDlgItem, text, true);

            if (dValue != dDefault)
            {
               dValue = dDefault;
               *pbChanged = true;
            }
            return false;
         }
      }
      double dNew = atof(text);
      if (dValue != dNew)
      {
         dValue = dNew;
         if (pbChanged) *pbChanged = true;
      }
      return true;
   }  
   else
   {
      return false;
   }
}

/******************************************************************************
* Name       : CheckMinMaxInt                                                 *
* Zweck      : Überprüft den Wertebereich einer Integer-Zahl, gibt eine       *
*              Fehlermeldung aus, setzt die Zahl auf einen gültigen Wert im   *
*              vorgegebenen Wertebereich und setzt den geänderten Wert im     *
*              Dialogfeld.                                                    *
* Aufruf     : CheckMinMaxInt(nIDDlgItem, nMin, nMax, bSigned, nValue);       *
* Parameter  :                                                                *
* nIDDlgItem(E): ID des Dialog Items                                 (int)    *
* nMin, nMax(E): Minimaler und Maximaler Wert des Bereichs           (int)    *
* bSigned   (E): Vorzeichen (true, false)                            (bool)   *
* nValue   (EA): zu überprüfende Zahl                                (int&)   *
* Funktionswert : (true, false)                                      (bool)   *
******************************************************************************/
bool CEtsDialog::CheckMinMaxInt(int nIDDlgItem, int nMin, int nMax, bool bSigned, int &nValue, bool bMsg)
{
   ASSERT(nMin <= nMax);
   bool bReturn = true;
   if (nValue > nMax)
   {
      nValue  = nMax;
      bReturn = false;
   }
   if (nValue < nMin)
   {
      nValue  = nMin;
      bReturn = false;
   }
   if (bReturn == false)
   {
      SetDlgItemInt(nIDDlgItem, nValue, bSigned);
      if (bMsg)
      {
         char format[STRINGSIZE];
         char text[STRINGSIZE];
         if (::LoadString(m_hInstance, gm_nIDFmtMinMaxInt, format, STRINGSIZE))
            wsprintf(text, format, nMin, nMax);
         else
            strcpy(text, "Range Error");
         CreateErrorWnd(nIDDlgItem, text, true);
      }
   }
   return bReturn;
}

/******************************************************************************
* Name       : CheckMinMaxDouble                                              *
* Zweck      : Überprüft den Wertebereich einer Double-Zahl, gibt eine        *
*              Fehlermeldung aus, setzt die Zahl auf einen gültigen Wert im   *
*              vorgegebenen Wertebereich und setzt den geänderten Wert im     *
*              Dialogfeld.                                                    *
* Aufruf     : CheckMinMaxInt(nIDDlgItem, dMin, dMax, nDec, nValue);          *
* Parameter  :                                                                *
* nIDDlgItem(E): ID des Dialog Items                                 (int)    *
* nMin, nMax(E): Minimaler und Maximaler Wert des Bereichs           (int)    *
* nDec      (E): Anzahl der Kommastellen im Dialogfeldtext           (int)    *
* dValue   (EA): zu überprüfende Zahl                                (double&)*
* Funktionswert : (true, false)                                      (bool)   *
******************************************************************************/
bool CEtsDialog::CheckMinMaxDouble(int nIDDlgItem, double dMin, double dMax, int nDec, double &dValue, bool bMsg)
{
   ASSERT(dMin <= dMax);
   bool bReturn = true;
   if (dValue > dMax)
   {
      dValue  = dMax;
      bReturn = false;
   }
   if (dValue < dMin)
   {
      dValue  = dMin;
      bReturn = false;
   }
   if (bReturn == false)
   {
      SetDlgItemDouble(nIDDlgItem, dValue, nDec);
      if (bMsg)
      {
         char format[STRINGSIZE];
         char text[STRINGSIZE];
         if (::LoadString(m_hInstance, gm_nIDFmtMinMaxDouble, format, STRINGSIZE))
         {
#ifdef INCLUDE_ETS_HELP
            char szMin[64] ="", szMax[64]= "";
            ETSDIV_NUTOTXT ntx = {dMin, gm_nmode, abs(nDec), STRINGSIZE, 0};
            bool bOK = (CEtsDiv::NumberToText(&ntx, szMin)!=0) ? true : false;
            if (bOK)
            {
               ntx.fvalue = dMax;
               CEtsDiv::NumberToText(&ntx, szMax);
            }
            else return false;
            wsprintf(text, format, szMin, szMax);
#else
            if (nDec <= 0) return false;
            int nFact   = (int) pow(10.0, (double)nDec);
            int nMin    = (int) dMin;
            int nMinDec = (int) ((dMin - (double)nMin) * nFact);
            int nMax    = (int) dMax;
            int nMaxDec = (int) ((dMax - (double)nMax) * nFact);
            char * ptr = strstr(format, "%0xd");
            if (ptr)
            {
               ptr[2] = '0' + (char) nDec;
               ptr+=4;
               ptr = strstr(format, "%0xd");
               if (ptr) ptr[2] = '0' + (char) nDec;
            }
            wsprintf(text, format, nMin, nMinDec, nMax, nMaxDec);
#endif
         }
         else
            strcpy(text, "Range Error");
         CreateErrorWnd(nIDDlgItem, text, true);
      }
   }
   return bReturn;
}

/******************************************************************************
* Name       : CreateErrorWnd                                                 *
* Definition : void CreateErrorWnd(int, int, bool);                           *
* Zweck      : Erzeugen eines Fensters zur Fehlerausgabe speziell für ein     *
*              Dialogelement.                                                 *
* Aufruf     : CreateErrorWnd(nIDDlgItem, nIdString, bSetFocus);              *
*              CreateErrorWnd(nIDDlgItem, text     , bSetFocus);              *
* Parameter  :                                                                *
* nIDDlgItem (E) : ID des Dialogelementes zur Positionierung des        (int) *
*                  Fensters. Ist dieses ungültig, so wird das Fenster in der  *
*                  Dialogbox zentriert.                                       *
* nIdString  (E) : ID eines Resourcentextes für die Fehlermeldung.      (int) *
* text       (E) : Fehlermeldung als String                           (char*) *
* bSetFocus  (E) : Setzen des Eingabefocus auf dieses Dialogelement    (bool) *
* Funktionswert keiner                                                        *
******************************************************************************/
void CEtsDialog::CreateErrorWnd(int nIDDlgItem, int nIdString, bool bSetFocus)
{
   if (m_nFlags & NOERRORWINDOW) return;

   char szText[256];
   if (::LoadString(m_hInstance, nIdString, szText, 256))
   {
      CreateErrorWnd(nIDDlgItem, szText, bSetFocus);
   }
}

void CEtsDialog::CreateErrorWnd(int nIDDlgItem, char *text, bool bSetFocus)
{
   if (m_nFlags & NOERRORWINDOW) return;
   HWND hWnd = GetDlgItem(nIDDlgItem);
   RECT rcWnd;
   if (hWnd)
   {
      if (bSetFocus)
      {
         m_nFlags |= NOKILLFOCUSNOTIFICATION;
         ::SetFocus(hWnd);
         m_nFlags &= ~NOKILLFOCUSNOTIFICATION;
      }
      ::GetWindowRect(hWnd, &rcWnd);
   }
   else 
   {
      ::GetWindowRect(m_hWnd, &rcWnd);
      rcWnd.left = (rcWnd.left + rcWnd.right ) >> 1;
      rcWnd.top  = (rcWnd.top  + rcWnd.bottom) >> 1;
   }
   m_nFlags |= NOKILLFOCUSNOTIFICATION;
#ifdef INCLUDE_ETS_HELP
   CEtsHelp::CreateErrorWnd(text, rcWnd.left, rcWnd.top);
#else
   ::MessageBox(m_hWnd, text, "Error", MB_OK|MB_ICONWARNING);
#endif
   m_hWndForeground = GetForegroundWindow();
   if (m_hWndForeground == NULL)
   {
      m_nFlags &= ~NOKILLFOCUSNOTIFICATION;
   }
}

RECT CEtsDialog::AdaptMonitorPoints(POINT *ppt, int npt)
{
   ASSERT(ppt != NULL);
   HMONITOR hMon = MonitorFromPoint(ppt[0], MONITOR_DEFAULTTONEAREST);
   MONITORINFO sMI = {sizeof(MONITORINFO),{0,0,0,0},{0,0,0,0},0};
   if (hMon)
   {
      if (GetMonitorInfo(hMon, &sMI))
      {
         for (int i=0; i<npt; i++)
         {
            ppt[i].x += sMI.rcMonitor.left;
            ppt[i].y += sMI.rcMonitor.top;
         }
         return sMI.rcMonitor;
      }
   }
   return sMI.rcMonitor;
}

/******************************************************************************
* Name       : MessageBox                                                     *
* Definition : int MessageBox(HWND, int, int, UINT);                          *
* Zweck      : Aufruf des Windows API-MessageBox(..) Funktion mit ID´s für    *
*              Resourcenstrings statt Strings.                                *
* Aufruf     : int CEtsDialog::MessageBox(hwnd, nIDMsg, nIDCapiton, uType);   *
* Parameter  :                                                                *
* hwnd       (E) : Elternfenster                                     (HWND)   *
* nIDMsg     (E) : Ausgabetext ID                                    (long)   *
* nIDCapiton (E) : Überschrift ID                                    (long)   *
* uType      (E) : Typ der MessageBox (Flags)                        (UINT)   *
*    MBU_TEXTSTRING    : nIDMsg     = Zeichenkette                   (char*)  *
*    MBU_CAPTIONSTRING : nIDcaption = Zeichenkette                   (char*)  *
* Funktionswert : Rückgabewert der MessageBox                        (int)    *
******************************************************************************/
int CEtsDialog::MessageBox(long nIDtext, long nIDcaption, UINT uType)
{
   char * pszText    = NULL;
   char * pszCaption = NULL;
   if (uType & MBU_TEXTSTRING)    pszText    = (char*)nIDtext;
   else                           pszText    = LoadString(nIDtext);
   if (uType & MBU_CAPTIONSTRING) pszCaption = (char*)nIDcaption;
   else                           pszCaption = LoadString(nIDcaption);
   int nReturn = ::MessageBox(m_hWnd, pszText, pszCaption, uType & ~(MBU_TEXTSTRING|MBU_CAPTIONSTRING));
   if ((pszText    != NULL) && !(uType & MBU_TEXTSTRING   )) free(pszText);
   if ((pszCaption != NULL) && !(uType & MBU_CAPTIONSTRING)) free(pszCaption);
   return nReturn;
}

/******************************************************************************
* Name       : GetDlgSpinInt                                                  *
* Zweck      : liefert die Position eines Spinbuttons                         *
* Aufruf     : GetDlgSpinInt(nIDDlgItem);                                     *
* Parameter  :                                                                *
* nIDDlgItem(E): Resourcen-ID des Spincontrols                          (int) *
* Funktionswert : Position des Spincontrols                                   *
******************************************************************************/
int CEtsDialog::GetDlgSpinInt(int nIDDlgItem)
{
   int index = SendDlgItemMessage(nIDDlgItem, UDM_GETPOS, 0, 0);
   if (index == 65635)                                      // Wert größer als die obere Grenze
   {
      index = SendDlgItemMessage(nIDDlgItem, UDM_GETRANGE, 0, 0);
      index = LOWORD(index);
      SendDlgItemMessage(nIDDlgItem, UDM_SETPOS, 0, index);
   }                                                        // Wert kleiner als die Untere Grenze oder undefiniert
   if (index == 65636)
   {
      index = SendDlgItemMessage(nIDDlgItem, UDM_GETRANGE, 0, 0);
      index = HIWORD(index);
      SendDlgItemMessage(nIDDlgItem, UDM_SETPOS, 0, index);
   }
   return index;
}

/******************************************************************************
* Name       : ChooseColor                                                    *
* Zweck      : Aufruf des Common-Dialogs zur Farbauswahl                      *
* Aufruf     : ChooseColor(pszHeading, color);                                *
* Parameter  :                                                                *
* pszHeading(E): Überschrift des Dialogfeldes                     (char*)     *
* color  (EA): RGB-Farbwert                                       (COLORREF&) *
* Funktionswert : (true, false)                                   (bool)      *
******************************************************************************/
bool CEtsDialog::ChooseColorT(char *pszHeading, COLORREF &color, HWND hWndParent, HANDLE hInstance)
{
   CHOOSECOLOR cc = 
   {
      sizeof(CHOOSECOLOR),                // Größe
      hWndParent, (HWND)hInstance,        // Window, Instancehandle
      color, gm_CustomColors,             // Farbe, Farbtabelle
      CC_ANYCOLOR|CC_SHOWHELP|CC_RGBINIT, // Flags
      (long) pszHeading,                  // pCustomData
      NULL, NULL                          // pfnHook, pszTemplate
   };
   if (pszHeading != NULL)
   {
       cc.lpfnHook = CCHookProc;
       cc.Flags   |= CC_ENABLEHOOK;
   }
   if (::ChooseColor(&cc))
   {
      color = cc.rgbResult;
      return true;
   }
   return false;
}

void CEtsDialog::SaveCustomColors(HANDLE hFile)
{
   DWORD dwBytesRead;
   WriteFile(hFile, &gm_CustomColors, sizeof(COLORREF)*16, &dwBytesRead, NULL);// Dateityp,
}

void CEtsDialog::LoadCustomColors(HANDLE hFile)
{
   DWORD dwBytesRead;
   ReadFile(hFile, &gm_CustomColors, sizeof(COLORREF)*16, &dwBytesRead, NULL);// Dateityp,
}

/******************************************************************************
* Name       : CCHookProc                                                     *
* Zweck      : Setzen des Titels der Farbdialogbox                            *
* Aufruf     : CCHookProc(hdlg, uiMsg, wParam, lParam);                       *
* Parameter  :                                                                *
* Funktionswert : (0)                                             (UINT)      *
******************************************************************************/
UINT CALLBACK CEtsDialog::CCHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
   if (uiMsg == WM_INITDIALOG)
   {
      CHOOSECOLOR *pcc = (CHOOSECOLOR*) lParam;
      if (hdlg && pcc && pcc->lCustData)
      {
         char *text = (char*) pcc->lCustData;
         ::SetWindowText(hdlg, text);
         return 0;
      }
   }
   return 0;
}

struct MessageFilter
{
   int    code;
   WPARAM wParam;
   LPARAM lParam;
   BOOL   bResult;
};

BOOL CALLBACK CEtsDialog::MsgFilterEnum(HWND hwnd, LPARAM lParam)
{
   char szName[32];
   ::GetClassName(hwnd, szName, 32);
   if (strcmp(szName, "#32770") == 0)
   {
      MessageFilter *pmsf = (MessageFilter*)lParam;
      MSG *pmsg = (MSG*)pmsf->lParam;
      if (::GetProp(hwnd, THIS_POINTER_CDIALOG) == gm_hDll_Hook)
      {
         CEtsDialog* pDlg = (CEtsDialog*)::GetWindowLong(hwnd, DWL_USER);
         if (pDlg)
         {
            if ((pDlg->m_nFlags & IS_IN_MENU_LOOP) && 
                ((pmsg->message >= WM_MOUSEFIRST) && (pmsg->message <= WM_MOUSELAST))||
                ((pmsg->message >= WM_KEYFIRST  ) && (pmsg->message <= WM_KEYLAST)))
            {
               return ::CallNextHookEx(gm_hDll_Hook, pmsf->code, pmsf->wParam, pmsf->lParam);
            }
         }
      }
      pmsg->time = 0;
      pmsf->bResult = ::IsDialogMessage(hwnd, pmsg);
      if (pmsf->bResult)   // Message für diesen Dialog ? 
      {
#ifdef _DEBUG
         ::GetWindowText(hwnd, szName, 32);
         TRACE("%x, %s\n", hwnd, szName);
#endif // _DEBUG
         return 0;
      }
   }
   return 1;
}

/******************************************************************************
* Name       : MsgFilterProc                                                  *
* Zweck      : Filtern der Nachrichten für eine nicht modale Dialogbox aus der*
*              Nachrichtenschleife. Wenn die Nachricht von der Dialogbox      *
*              verarbeitet wird, so muß sie nicht weiter verarbeitet werden.  *
* Aufruf     : als HOOK Funktion in der Nachrichtenschleife, in der sie       *
*              installiert wurde.                                             *
* Parameter  :                                                                *
* code    (E): Code der Nachrichtverarbeitung                          (int)  *
* wParam (E) : erster Parameter der Nachricht                        (WPARAM) *
* lParam (E) : zweiter Parameter der Nachricht                       (LPARAM) *
* Funktionswert : ungleich 0, wenn die Nachricht nicht weiter        (LRESULT)*
*                 verarbeitet werden soll:                                    *
******************************************************************************/
LRESULT CALLBACK CEtsDialog::MsgFilterProc(int code, WPARAM wParam,  LPARAM lParam)
{
   if (code >= 0) // wenn code größer gleich 0 ist dürfen die Nachrichten untersucht werden
   {
      if (lParam)
      {
         if (gm_hWndTopLevelParent)
         {
            MessageFilter msf ={code, wParam, lParam, 0};
            MSG *pmsg = (MSG*)lParam;
            if (pmsg->time != 0)
            {
               ::EnumChildWindows(gm_hWndTopLevelParent, MsgFilterEnum, (LPARAM)&msf);
            }
            if (msf.bResult)
            {
               MSG *pmsg = (MSG*)lParam;
               MSG msg;
               _asm CLD;
               PeekMessage(&msg, pmsg->hwnd, pmsg->message, pmsg->message, PM_REMOVE|PM_NOYIELD);
               return msf.bResult;
            }
         }
      }
   }
   return CallNextHookEx(gm_hDll_Hook, code, wParam, lParam);
}

/******************************************************************************
* Name       : DialogProc                                                     *
* Zweck      : Verarbeitung der Nachrichten der DialogBox                     *
* Aufruf     : als Callbackfunktion der Windowklasse                          *
* Parameter  :                                                                *
* hwndDlg (E): Windowhandle des Dialogfeldes                         (HWND)   *
* uMsg    (E): ID der Nachricht                                      (UINT)   *
* wParam (E) : erster Parameter der Nachricht                        (LPARAM) *
* lParam (E) : zweiter Parameter der Nachricht                       (WPARAM) *
* Funktionswert : abhängig von der Nachricht                         (BOOL)   *
******************************************************************************/
BOOL CALLBACK CEtsDialog::DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   switch(uMsg)
   {
      case WM_INITDIALOG:
      if (lParam)
      {
         CEtsDialog * pDlg = NULL;
         if (*((DWORD*)lParam) == sizeof(PROPSHEETPAGE))
         {
            ::SetWindowLong(hwndDlg, DWL_USER, (LONG)((PROPSHEETPAGE*)lParam)->lParam);
            pDlg = (CEtsDialog*)((PROPSHEETPAGE*)lParam)->lParam;
            pDlg->m_hInstance = ((PROPSHEETPAGE*)lParam)->hInstance;
            ((CEtsPropertyPage*)pDlg)->m_pPropSheetPage = (PROPSHEETPAGE*)lParam;
         }
         else
         {
            ::SetWindowLong(hwndDlg, DWL_USER, (LONG)lParam);
            pDlg = (CEtsDialog*)lParam;
            if (pDlg && pDlg->m_bModal && !IsWindowVisible(hwndDlg))
            {
               ShowWindow(hwndDlg, SW_SHOW);
            }
         }
         if (pDlg!=NULL)
         {
            if (pDlg->OnInitDialog(hwndDlg)) return 1;
            else
            {
               pDlg->OnEndDialog(-1);
            }
         }
         return 0;
      }
      break;
      case WM_COMMAND:
      {
         CEtsDialog* pDlg = (CEtsDialog*)::GetWindowLong(hwndDlg, DWL_USER);
         if (pDlg)
         {
            if (CEtsDialog::gm_nIDAPPLY == LOWORD(wParam))
            {
               pDlg->OnApply();
               return 1;
            }
            switch(LOWORD(wParam))
            {
               case IDCANCEL:
               {
                  int nResult = pDlg->OnCancel();
                  pDlg->OnEndDialog(nResult);
                  return 1;
               }
               case IDOK:
               {
                  if (pDlg->OnOk(HIWORD(wParam)) == IDOK)
                  {
                     pDlg->OnEndDialog(IDOK);
                  }
                  return 1;
               }
               default:
               if ((pDlg->m_hWndForeground!=NULL) && 
                   !::IsWindow(pDlg->m_hWndForeground))
               {
                  pDlg->m_nFlags &= ~NOKILLFOCUSNOTIFICATION;
                  pDlg->m_hWndForeground           = NULL;
               }
               if (pDlg->m_nFlags & NOKILLFOCUSNOTIFICATION)
               {
                  switch (HIWORD(wParam))
                  {
                     case EN_KILLFOCUS:
                     case BN_KILLFOCUS:
                     case LBN_KILLFOCUS:
                     case CBN_KILLFOCUS:
                        TRACE("Invalid Killfocus\n");
                        return 1;
                     default: break;
                  }
               }
               return pDlg->OnCommand(LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
            }
         }
      }
      default:
      {
         CEtsDialog* pDlg = (CEtsDialog*)::GetWindowLong(hwndDlg, DWL_USER);
         if (pDlg) return pDlg->OnMessage(uMsg, wParam, lParam);
      } break;
   }
   return 0;
}

/******************************************************************************
* Name       : KillTimer                                                      *
* Zweck      : Beenden des Timers und Löschen der Timer Nachrichten.          *
* Aufruf     : KillTimer(nID);                                                *
* Parameter  :                                                                *
* nID     (E): ID des Timers                                         (UINT)   *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CEtsDialog::KillTimer(UINT nID)
{
   ASSERT(IsWindow(m_hWnd));
   MSG msg;
   ZeroMemory(&msg, sizeof(MSG));
   ::KillTimer(m_hWnd, nID);
   _asm CLD;
   PeekMessage(&msg, m_hWnd, WM_TIMER, WM_TIMER, PM_REMOVE|PM_NOYIELD);
}


HWND CEtsDialog::GetTopLevelParent(HWND hwndChild)
{
   HWND hwndParent;
   while(hwndChild)
   {
      hwndParent = ::GetParent(hwndChild);
      if (hwndParent == NULL) return hwndChild;
      hwndChild = hwndParent;
   };
   return NULL;
}

LRESULT CEtsDialog::OnExitSizeMove(WPARAM wParam, HWND hwndFrom)
{
   if (wParam & EXIT_SIZE_MOVE_GET)
   {
      wParam = 0;
      switch(m_nWhere)
      {
         case 0: wParam |= EXIT_SIZE_MOVE_BOTTOM; break;
         case 1: wParam |= EXIT_SIZE_MOVE_TOP;    break;
         case 2: wParam |= EXIT_SIZE_MOVE_LEFT;   break;
         case 3: wParam |= EXIT_SIZE_MOVE_RIGHT;  break;
      }
      if (wParam) wParam |= EXIT_SIZE_MOVE_FORCE;
      if (m_nFlags & DLG_DOCK_ABLE) wParam |= EXIT_SIZE_MOVE_DOCK;
      *((DWORD*)hwndFrom) = wParam;
      return 1;
   }
   if (wParam & EXIT_SIZE_MOVE_SET) // Parameterwerden geändert
   {
      if (wParam & EXIT_SIZE_MOVE_DOCK) m_nFlags |= DLG_DOCK_ABLE;
      else                              m_nFlags &= DLG_DOCK_ABLE;
   }
   if (m_nFlags & DLG_DOCK_ABLE)
   {
      RECT rcDlg;
      RECT rcParent;
      POINT ptDock, ptDiff;
      DWORD dwProcessID, dwId2;
      ::GetWindowRect(m_hWnd, &rcDlg);
      ::GetWindowThreadProcessId(m_hWnd, &dwProcessID);
      HWND hwndDock;

      if (wParam & EXIT_SIZE_MOVE_FORCE)
      {
         hwndDock = hwndFrom;
         if      (wParam & EXIT_SIZE_MOVE_LEFT)   m_nWhere = 2;
         else if (wParam & EXIT_SIZE_MOVE_TOP)    m_nWhere = 1;
         else if (wParam & EXIT_SIZE_MOVE_RIGHT)  m_nWhere = 3;
         else if (wParam & EXIT_SIZE_MOVE_BOTTOM) m_nWhere = 0;
         else
         {
            m_nWhere = 4;
            hwndDock = NULL;
         }
      }
      else
      {
         for (m_nWhere=0; m_nWhere<4; m_nWhere++)
         {
            switch (m_nWhere)
            {
               case 0:                                               // Unten links
               ptDock.x = rcDlg.left + 5;
               ptDock.y = rcDlg.top  - 5;
               break;
               case 1:                                               // Oben links
               ptDock.x = rcDlg.left   + 5;
               ptDock.y = rcDlg.bottom + 5;
               break;
               case 2:                                               // Seite oben links
               ptDock.x = rcDlg.right  + 5;
               ptDock.y = rcDlg.top    + 5;
               break;
               case 3:                                               // Seite oben rechts
               ptDock.x = rcDlg.left   - 5;
               ptDock.y = rcDlg.top    + 5;
               break;
            }
            hwndDock = ::WindowFromPoint(ptDock);
            if (hwndDock)
            {
               ::GetWindowThreadProcessId(hwndDock, &dwId2);
               HANDLE hProp = ::GetProp(hwndDock, "DockChild");
               if (hProp) continue;
               if (dwId2 == dwProcessID)
               {
                  while (!(::GetWindowLong(hwndDock, GWL_STYLE) & WS_CAPTION))
                  {
                     hwndDock = ::GetParent(hwndDock);
                     if (hwndDock == NULL) break;
                  }
                  if (hwndDock == NULL) continue;
                  ::GetWindowRect(hwndDock, &rcParent);
                  switch (m_nWhere)
                  {
                     case 0:                                               // Unten links
                     ptDiff.x = rcParent.left - rcDlg.left;
                     ptDiff.y = rcParent.bottom - rcDlg.top;
                     break;
                     case 1:                                               // Oben links
                     ptDiff.x = rcParent.left - rcDlg.left;
                     ptDiff.y = rcParent.top - rcDlg.bottom;
                     break;
                     case 2:                                               // Seite oben links
                     ptDiff.x = rcParent.left - rcDlg.right;
                     ptDiff.y = rcParent.top - rcDlg.top;
                     break;
                     case 3:                                               // Seite oben rechts
                     ptDiff.x = rcParent.right - rcDlg.left;
                     ptDiff.y = rcParent.top - rcDlg.top;
                     break;
                  }
                  if (((ptDiff.x * ptDiff.x) + (ptDiff.y * ptDiff.y)) < 500)
                     break;
               }            
               else continue;
            }
         }
      }
      RemoveDockProc();
      if (m_nWhere < 4) 
      {
         m_hwndDock = hwndDock;
         ::SetProp(m_hwndDock, "DockChild", (HANDLE) this);
         DockingParentSC(m_hwndDock, WM_MOVE, 0, 0);
         m_lOlpParentSubClass = ::SetWindowLong(m_hwndDock, GWL_WNDPROC, (long)DockingParentSC);
      }
   }
   return 0;
}

LRESULT CALLBACK CEtsDialog::DockingParentSC(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   CEtsDialog *pThis = (CEtsDialog*)::GetProp(hwnd, "DockChild");
   if (pThis)
   {
      if ((uMsg == WM_MOVE) || (uMsg == WM_SIZE))
      {
         RECT rcDlg;
         RECT rcParent;
         POINT ptOffset;
         ::GetWindowRect(hwnd, &rcParent);
         ::GetWindowRect(pThis->m_hWnd, &rcDlg);
         switch (pThis->m_nWhere)
         {
            case 0:                                               // Unten links
            ptOffset.x = 0;
            ptOffset.y = 0;
            break;
            case 1:                                               // Oben links
            ptOffset.x = 0;
            ptOffset.y = -(abs(rcParent.top - rcParent.bottom) + abs(rcDlg.top - rcDlg.bottom));
            break;
            case 2:                                               // Seite oben links
            ptOffset.x = -abs(rcDlg.right - rcDlg.left);
            ptOffset.y = -abs(rcParent.top - rcParent.bottom);
            break;
            case 3:                                               // Seite oben rechts
            ptOffset.x = abs(rcParent.right - rcParent.left);
            ptOffset.y = -abs(rcParent.top - rcParent.bottom);
            break;
         }
         ::SetWindowPos(pThis->m_hWnd, NULL, rcParent.left+ptOffset.x, rcParent.bottom+ptOffset.y, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOSENDCHANGING|SWP_ASYNCWINDOWPOS|SWP_DEFERERASE);
      }
      if (pThis->m_lOlpParentSubClass)
         return ::CallWindowProc((WNDPROC)pThis->m_lOlpParentSubClass, hwnd, uMsg, wParam, lParam);
   }
   return 0;
}

void CEtsDialog::RemoveDockProc()
{
   if (m_lOlpParentSubClass)
   {
      ::SetWindowLong(m_hwndDock, GWL_WNDPROC, (long)m_lOlpParentSubClass);
      ::RemoveProp(m_hwndDock, "DockChild");
      m_lOlpParentSubClass = 0;
   }
}

LRESULT CALLBACK EditFloatSC(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   if (uMsg == WM_CHAR)
   {
      bool bOk = false;
      if      ('-' == (char)wParam)  bOk = true;   // Minus ist erlaubt
      else if (   ::isdigit(wParam)) bOk = true;   // Zahlen
      else if (wParam         < 32)  bOk = true;   // Steuerzeichen
//      else if (',' == (char)wParam)  bOk = true;   // Komma erlaubt
      else if ('.' == (char)wParam)  bOk = true;   // Punkt erlaubt
      else                                         // Dezimalzeichen
      {
         char szSign[4];
         GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, szSign, 4);
         if (szSign[0] == (char)wParam) bOk = true;

//         GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, szSign, 4);
//         if (szSign[0] == (char)wParam) bOk = true;
      }
      if (!bOk)
      {
         ::MessageBeep(0xffffffff);
         return 0;
      }
   }
   return ::CallWindowProc((WNDPROC)::GetClassLong(hwnd, GCL_WNDPROC), hwnd, uMsg, wParam, lParam);
}

bool CEtsDialog::SetNumeric(int nID, bool bNumeric, bool bFloatingPoint)
{
   HWND hwnd = GetDlgItem(nID);
   if (hwnd)
   {
      long lStyle = ::GetWindowLong(hwnd, GWL_STYLE);
      if (bNumeric && !bFloatingPoint)
      {
         lStyle |= ES_NUMBER;
      }
      else
      {
         lStyle &= ~ES_NUMBER;
      }
      ::SetWindowLong(hwnd, GWL_STYLE, lStyle);
      if (bNumeric && bFloatingPoint)
      {
         ::SetWindowLong(hwnd, GWL_WNDPROC, (long)EditFloatSC);
      }
      else
      {
         ::SetWindowLong(hwnd, GWL_WNDPROC, (long)::GetClassLong(hwnd, GCL_WNDPROC));
      }
      return true;
   }
   return false;
}

void CEtsDialog::ChangeModalStyles(bool bMin)
{
   long lStyle;
   HMENU hMenu = NULL;
   if (m_hWnd) hMenu = ::GetSystemMenu(m_hWnd, false);
   if (hMenu)
   {
      if (bMin)
      {
         if ((m_nFlags & MODAL_W_CONTEXT_HELP) && (m_nFlags & (MODAL_W_MIN_BTN|MODAL_W_MAX_BTN)))
         {
            lStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
            if (m_nFlags & MODAL_W_MIN_BTN) lStyle |= WS_MINIMIZEBOX;
            if (m_nFlags & MODAL_W_MAX_BTN) lStyle |= WS_MAXIMIZEBOX;
            else EnableMenuItem(hMenu, SC_MAXIMIZE, MF_BYCOMMAND | MF_GRAYED);

            EnableMenuItem(hMenu, SC_RESTORE, MF_BYCOMMAND | MF_ENABLED);
            EnableMenuItem(hMenu, SC_MINIMIZE, MF_BYCOMMAND | MF_GRAYED);

            ::SetWindowLong(m_hWnd, GWL_STYLE, lStyle);

            lStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
            lStyle &= ~WS_EX_CONTEXTHELP;
            ::SetWindowLong(m_hWnd, GWL_EXSTYLE, lStyle);
         }
      }
      else
      {
         if ((m_nFlags & MODAL_W_CONTEXT_HELP) && (m_nFlags & (MODAL_W_MIN_BTN|MODAL_W_MAX_BTN)))
         {
            lStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
            if (m_nFlags & MODAL_W_MIN_BTN) lStyle &= ~WS_MINIMIZEBOX;
            if (m_nFlags & MODAL_W_MAX_BTN) lStyle &= ~WS_MAXIMIZEBOX;
            else EnableMenuItem(hMenu, SC_MAXIMIZE, MF_BYCOMMAND | MF_GRAYED);

            EnableMenuItem(hMenu, SC_RESTORE, MF_BYCOMMAND | MF_GRAYED);
            EnableMenuItem(hMenu, SC_MINIMIZE, MF_BYCOMMAND | MF_ENABLED);

            ::SetWindowLong(m_hWnd, GWL_STYLE, lStyle);

            lStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
            lStyle |= WS_EX_CONTEXTHELP;
            ::SetWindowLong(m_hWnd, GWL_EXSTYLE, lStyle);
         }
      }
   }
}

