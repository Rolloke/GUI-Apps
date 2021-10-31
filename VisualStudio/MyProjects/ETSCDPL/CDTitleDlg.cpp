/********************************************************************/
/* Funktionen der Klasse CDTitleDlg                                */
/********************************************************************/
#include "stdafx.h"
#include "CDTitleDlg.h"
#include "EtsRegistry.h"
#include "ETS3DGLRegKeys.h"
#include "resource.h"
#include "CDPlayerCtrl.h"
#include "CCDPLDlg.h"
#include "CEtsFileDlg.h"

#define ACCD_TYPE          0x44434341     // Identifikation der CD-Info-Dateien

long CDTitleDlg::gm_lOldEditCtrlWndProc = 0;
char CDTitleDlg::gm_szFormatCDTime[64]="";

CDTitleDlg::CDTitleDlg()
{
   Constructor();
}

CDTitleDlg::CDTitleDlg(HINSTANCE hInstance, int nID, HWND hwndParent) : CEtsDialog(hInstance, nID, hwndParent)
{
   Constructor();
}

CDTitleDlg::~CDTitleDlg()
{
   FreeInterpret();
   FreeTitles();
}

void CDTitleDlg::Constructor()
{
   m_Titles.SetDeleteFunction(DeleteTitle);
   m_pszInterpret    = NULL;
   m_szMediaIdent[0] = 0;
   m_nTracks         = 0;
   m_bEditable       = false;
   m_hIcon           = (HICON)INVALID_HANDLE_VALUE;
   m_nCurrentItem    = 0;
   m_pTimes          = NULL;
}

bool CDTitleDlg::OnInitDialog(HWND hWnd) 
{
   if (CEtsDialog::OnInitDialog(hWnd))
   {
      char      szText[64];
      LVCOLUMNA lvc;
      int nOrder[3]  = { 1, 0, 2};
      int i, nCount  = m_Titles.GetCounter(),
          nCol       = 0;

      ZeroMemory(&lvc, sizeof(LVCOLUMN));
      lvc.cchTextMax = 64;
      lvc.pszText    = szText;
      lvc.mask       = LVCF_FMT|LVCF_TEXT|LVCF_WIDTH;
      m_nIDList      = IDC_CD_TITLE_LIST;
      m_nIDInterpret = IDC_CD_INTERPRET;

      if (gm_szFormatCDTime[0] == 0)
      {
         ::LoadString(m_hInstance, IDS_CD_TIME2, gm_szFormatCDTime, 64); 
      }
      HWND hwndList = GetDlgItem(IDC_CD_TITLE_LIST);
      if (hwndList == NULL) return false;

      if (nCount != m_nTracks)
      {
         for (i=nCount; i<m_nTracks; i++)
            m_Titles.ADDTail(_strdup(" "));
      }

      ::SendMessage(hwndList, LVM_SETEXTENDEDLISTVIEWSTYLE,
         LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT,
         LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

      ::LoadString(m_hInstance, IDS_CD_TITLE_TEXT, szText, 64);
      lvc.fmt = LVCFMT_RIGHT;
      lvc.cx = (m_pTimes) ? 350 : 400;
      ListView_InsertColumn(hwndList, nCol++,  &lvc);

      ::LoadString(m_hInstance, IDS_CD_TITLE_NO, szText, 64);
      lvc.cx = 30, lvc.fmt = LVCFMT_LEFT;
      ListView_InsertColumn(hwndList, nCol++,  &lvc);

      if (m_pTimes)
      {
         ::LoadString(m_hInstance, IDS_CD_TITLE_TIME, szText, 64);
         lvc.cx = 50 , lvc.fmt = LVCFMT_RIGHT;
         ListView_InsertColumn(hwndList, nCol++,  &lvc);
      }

      ::SendMessage(hwndList, LVM_SETCOLUMNORDERARRAY, nCol, (LPARAM)&nOrder);

      ::SendMessage(hwndList, LVM_DELETEALLITEMS, 0, 0);
      ::SendMessage(hwndList, LVM_SETITEMCOUNT, m_nTracks, LVSICF_NOSCROLL);

      if (m_hIcon != INVALID_HANDLE_VALUE)
         SendMessage(m_hWnd, WM_SETICON, ICON_BIG, (LPARAM)m_hIcon);

      if (m_bEditable)
      {
         ::SendMessage(GetDlgItem(IDC_CD_INTERPRET), EM_SETREADONLY, false, 0);
         ::ShowWindow(GetDlgItem(IDC_CD_BTN_INSERT_TITELS), SW_HIDE);
         ::ShowWindow(GetDlgItem(IDC_WAVE_PATH), SW_HIDE);
         ::ShowWindow(GetDlgItem(IDC_CD_AUTHOR), SW_SHOW);
      }
      else
      {
         ::SendMessage(GetDlgItem(IDC_CD_INTERPRET), EM_SETREADONLY, true, 0);
         ::ShowWindow(GetDlgItem(IDC_CD_BTN_INSERT_TITELS), SW_SHOW);
         ::ShowWindow(GetDlgItem(IDC_WAVE_PATH), SW_SHOW);
         ::ShowWindow(GetDlgItem(IDC_CD_AUTHOR), SW_HIDE);
         SetWindowLong(hwndList, GWL_ID, IDC_CD_TITLE_LIST_B);
         SetWindowLong(GetDlgItem(IDC_CD_INTERPRET), GWL_ID, IDC_CD_INTERPRET_B);
         m_nIDList      = IDC_CD_TITLE_LIST_B;
         m_nIDInterpret = IDC_CD_INTERPRET_B;
      }

      m_nCurrentItem = -1;
      SetDlgItemText(m_nIDInterpret, m_pszInterpret);

      if (m_bModal)
      {
         HICON hIcon = ::LoadIcon(m_hInstance, MAKEINTRESOURCE(IDR_FILE_TYPECD));
         SendMessage(m_hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
         SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED|SWP_NOMOVE|SWP_NOSIZE|SWP_NOOWNERZORDER|SWP_NOZORDER|SWP_SHOWWINDOW);
      }

      return true;
   }
   return false;
}

void CDTitleDlg::OnEndDialog(int nResult)
{
   CEtsDialog::OnEndDialog(nResult);                              // Funktion der Basisklasse aufrufen
}

int CDTitleDlg::OnOk(WORD nNotifyCode)
{
   if (nNotifyCode == 0)
   {
      WriteMediaInfo();
      return IDOK;
   }
   return 0;
}

int CDTitleDlg::OnCommand(WORD nID , WORD nNotifyCode, HWND hwndControl)
{
   switch(nID)
   {
      case IDC_CD_INTERPRET:
      case IDC_CD_INTERPRET_B:
      if (nNotifyCode == EN_KILLFOCUS)
      {
         m_pszInterpret = GetDlgItemText(GetDlgItem(nID), m_pszInterpret);
      }break;
      case IDC_CD_BTN_INSERT_TITELS:
      if (nNotifyCode == BN_CLICKED)
      {
         CEtsFileDlg dlg(m_hInstance, 0, m_hWnd);
         char szFilter[_MAX_PATH];
         if (m_pszInterpret[0] != '#')
         {
            strcpy(CCDPLDlg::gm_szImportPath, m_pszInterpret);
            RemoveCharacter(CCDPLDlg::gm_szImportPath, '\\', true);
         }
         else
         {
            char *pszTitle = (char*) m_Titles.GetFirst();
            if ((pszTitle != NULL) && (CCDPLDlg::gm_szImportPath[0] == 0))
            {
               strcpy(CCDPLDlg::gm_szImportPath, pszTitle);
               RemoveCharacter(CCDPLDlg::gm_szImportPath, '\\', true);
            }
         }
         dlg.SetInitialDir(CCDPLDlg::gm_szImportPath);
         ::SendMessage(m_hWndParent, WM_UPDATE_CD_CTRLS, GET_MM_FILTERS, (LPARAM)szFilter);
         dlg.SetFilter(szFilter, strlen(szFilter));
         dlg.SetTitle(IDS_LOAD_FILE_TITLE);
         dlg.ModifyFlags(OFN_FILEMUSTEXIST|OFN_ALLOWMULTISELECT|OFN_NOCHANGEDIR|OFN_HIDEREADONLY, OFN_SHOWHELP);
         dlg.SetMaxPathLength(4096);

         if (dlg.GetOpenFileName())                               // Ok ?
         {
            char *pszPathName  = dlg.GetPathName();
            if (pszPathName)                                      // Pfad
            {
               RemoveCharacter(pszPathName, '\\', false);
               strcpy(CCDPLDlg::gm_szImportPath, pszPathName);

               char  szFilePath[_MAX_PATH];
               const char * pszFileName  = dlg.GetFileTitle();    // ersten Dateinamen holen
               if (pszFileName && (pszFileName[0] != 0))
               {
                  InitInterpret();                                // Titel können jetzt in unterschiedlichen Pfaden sein

                  while (pszFileName)                             // vorhanden ?
                  {
                     wsprintf(szFilePath, "%s\\%s", pszPathName, pszFileName);
                     AddTitle(szFilePath);
                     pszFileName = dlg.GetFileTitle();            // nächsten Dateinamen holen
                  }
                  int nTrack = 0;
                  m_nTracks = m_Titles.GetCounter();
                  SendDlgItemMessage(m_nIDList, LVM_SETITEMCOUNT, m_nTracks, LVSICF_NOSCROLL);
                  ::SendMessage(m_hWndParent, WM_UPDATE_CD_CTRLS, GET_CURRENT_TRACK, (LPARAM)&nTrack);
                  ::SendMessage(m_hWndParent, WM_UPDATE_CD_CTRLS, NEW_CD_MEDIA, 0);
                  ::SendMessage(m_hWndParent, WM_UPDATE_CD_CTRLS, SET_CURRENT_TRACK, nTrack);
               }
               free(pszPathName);
               FindCommonPath();
            }
         }
      }break;
   }
   return CEtsDialog::OnCommand(nID , nNotifyCode, hwndControl);
}

int CDTitleDlg::OnMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
   switch(nMsg)
   {
      case WM_NOTIFY:
      if (wParam == (WPARAM)m_nIDList)
      {
         NMHDR *pnmhdr = (NMHDR*) lParam;
         switch (pnmhdr->code)
         {
            case LVN_GETDISPINFO:    return OnGetdispinfoCdTitleList((LV_DISPINFO*)lParam);
            case LVN_BEGINLABELEDIT: return OnBeginlabeleditCdTitleList((LV_DISPINFO*)lParam);
            case LVN_ENDLABELEDIT:   return OnEndlabeleditCdTitleList((LV_DISPINFO*)lParam);
            case LVN_KEYDOWN:        return OnKeyDownFormatList((NMLVKEYDOWN*)lParam);
            case NM_DBLCLK:          return OnDblClkCdTitleList((NMITEMACTIVATE*)lParam);
         }
      }break;
      case WM_INITMENUPOPUP:
      {
         HMENU hMenu = (HMENU) wParam;
         ::EnableMenuItem(hMenu, SC_SIZE, MF_BYCOMMAND|MF_GRAYED);
      } break;
      case WM_DROPFILES: return OnDropFiles((HDROP) wParam);
      case WM_EDITCTRL_KEYUP: return OnEditCtrlKeyUp(wParam, lParam);
   }
   return CEtsDialog::OnMessage(nMsg, wParam, lParam);
}

int CDTitleDlg::OnGetdispinfoCdTitleList(LV_DISPINFO* pDispInfo)
{
   if(pDispInfo->item.mask & LVIF_TEXT)
   {
      switch (pDispInfo->item.iSubItem)
      {
         case 1: wsprintf(pDispInfo->item.pszText, "%d", pDispInfo->item.iItem+1); break;
         case 0:
         {
            char *pszText = (char*) m_Titles.GetAt(pDispInfo->item.iItem);
            if (pszText)
            {
               lstrcpy(pDispInfo->item.pszText, pszText); 
            }
         }break;
         case 2: m_pTimes[pDispInfo->item.iItem].FormatMS(gm_szFormatCDTime, pDispInfo->item.pszText);
            break;
      }
   }
   ::SetWindowLong(m_hWnd, DWL_MSGRESULT, 0);
   return 0;
}

int CDTitleDlg::OnBeginlabeleditCdTitleList(LV_DISPINFO* pDispInfo) 
{
   long lResult = 1;
   if (m_bEditable)
   {
      HWND hwndEdit = (HWND) SendDlgItemMessage(m_nIDList, LVM_GETEDITCONTROL, 0, 0);
      if (hwndEdit)
      {
         if (gm_lOldEditCtrlWndProc==0)
         {
            gm_lOldEditCtrlWndProc = ::SetWindowLong(hwndEdit, GWL_WNDPROC, (long)EditCtrlWndProc);
            m_nCurrentItem = pDispInfo->item.iItem;
         }
      }
      lResult = 0;
   }

   ::SetWindowLong(m_hWnd, DWL_MSGRESULT, lResult);
   return lResult;
}

LRESULT CALLBACK CDTitleDlg::EditCtrlWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   if (gm_lOldEditCtrlWndProc == 0) return 0;

   if (uMsg == WM_KEYUP)
   {
      if ((wParam == VK_UP)||(wParam == VK_DOWN))
      {
         HWND hwndP = ::GetParent(::GetParent(hwnd));
         ::PostMessage(hwndP, WM_EDITCTRL_KEYUP, wParam, lParam);
         return 0;
      }
   }  
   return CallWindowProc((WNDPROC)gm_lOldEditCtrlWndProc, hwnd, uMsg, wParam, lParam);
}

int CDTitleDlg::OnEndlabeleditCdTitleList(LV_DISPINFO* pDispInfo) 
{
	long lResult = 0;
   HWND hwndEdit = (HWND) SendDlgItemMessage(m_nIDList, LVM_GETEDITCONTROL, 0, 0);
   if (hwndEdit)
   {
      m_nCurrentItem = -1;
      ::SetWindowLong(hwndEdit, GWL_WNDPROC, gm_lOldEditCtrlWndProc);
      gm_lOldEditCtrlWndProc = 0;
   }
   if ((pDispInfo->item.mask & LVIF_TEXT) &&
       (pDispInfo->item.pszText != NULL) &&
       (pDispInfo->item.iItem >= 0) && 
       (pDispInfo->item.iItem <  m_Titles.GetCounter()))
   {
      m_Titles.ReplaceAt(pDispInfo->item.iItem, _strdup(pDispInfo->item.pszText), true);
    	lResult = 1;
   }
   ::SetWindowLong(m_hWnd, DWL_MSGRESULT, lResult);
   return 0;
}

LRESULT CDTitleDlg::OnEditCtrlKeyUp(WPARAM wParam, LPARAM)
{
   int nItem = m_nCurrentItem;
   if (wParam == VK_UP)
   {
      nItem--;
   }
   if (wParam == VK_DOWN)
   {
      nItem++;
      if (nItem >= m_nTracks) nItem = -1;
   }

   if (nItem >= 0)
   {
      HWND hwndList = GetDlgItem(m_nIDList);
      ::SetFocus(hwndList);
      ::SendMessage(hwndList, LVM_EDITLABEL, nItem, 0);
   }
   return 0;
}

void CDTitleDlg::SetInterpret(char *pszText)
{
   FreeInterpret();
   if (pszText) m_pszInterpret = _strdup(pszText);
}

void CDTitleDlg::FreeInterpret()
{
   if (m_pszInterpret) free(m_pszInterpret);
   m_pszInterpret = NULL;
}

void CDTitleDlg::FreeTitles()
{
   m_Titles.Destroy();
   if (m_pTimes)
   {
      free(m_pTimes);
      m_pTimes = NULL;
   }
}

void CDTitleDlg::DeleteTitle(void *pszText)
{
   free(pszText);
}

void CDTitleDlg::AddTitle(char *pszText)
{
   char *psz = _strdup(pszText);
   strupr(psz);
   m_Titles.ADDHead(psz);
}

bool CDTitleDlg::ReadMediaInfo(char *pszPath)
{
   char szPath[_MAX_PATH] = "";
   HANDLE hFile;
   DWORD  dwRead;
   bool   bReturn = false;
   bool   bTimes  = false;
   if (pszPath == NULL)
   {
      if (m_szMediaIdent[0] == 0) return false;
      if (GetCDInfoPath(szPath, _MAX_PATH)==0) return false;
      strncat(szPath, m_szMediaIdent, _MAX_PATH);
      pszPath = szPath;
      bTimes  = true;
   }
   FreeInterpret();
   FreeTitles();
   
   hFile = CreateFile(pszPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
   if (hFile != INVALID_HANDLE_VALUE)
   {
      DWORD dwIdent[2];
      WORD  wCount= 0;
      ::ReadFile(hFile, &dwIdent, sizeof(DWORD)*2, &dwRead, NULL);
      if ((dwIdent[0] == ACCD_TYPE) && (dwIdent[1] == 200))
      {
         m_pszInterpret = ReadString(hFile);
         ::ReadFile(hFile, &wCount, sizeof(WORD), &dwRead, NULL);
         char *pszTitle;
         while (wCount-- > 0)
         {
            pszTitle=ReadString(hFile);
            m_Titles.ADDHead(pszTitle);
         }
         m_nTracks = m_Titles.GetCounter();
         if (bTimes) m_pTimes  = (MMTime*)realloc(m_pTimes, m_nTracks*sizeof(MMTime));
         bReturn = true;
      }
      ::CloseHandle(hFile);
   }
   if (bReturn) UpdateContent();

   return bReturn;
}

bool CDTitleDlg::WriteMediaInfo(char *pszPath)
{
   char szPath[_MAX_PATH];
   HANDLE hFile;
   DWORD  dwWritten;
   if (pszPath == NULL)
   {
      if (m_szMediaIdent[0] == 0) return false;
      if (GetCDInfoPath(szPath, _MAX_PATH)==0) return false;
      strncat(szPath, m_szMediaIdent, _MAX_PATH);
      pszPath = szPath;
   }
   
   hFile = CreateFile(pszPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
   if (hFile != INVALID_HANDLE_VALUE)
   {
      DWORD dwIdent[2] = {ACCD_TYPE, 200};
      ::WriteFile(hFile, &dwIdent, sizeof(DWORD)*2, &dwWritten, NULL);

      WriteString(hFile, m_pszInterpret);

      WORD  wCount = (WORD)m_Titles.GetCounter();
      ::WriteFile(hFile, &wCount, sizeof(WORD), &dwWritten, NULL);
      char *pszTitle = (char*) m_Titles.GetFirst();
      while (pszTitle)
      {
         WriteString(hFile, pszTitle);
         pszTitle = (char*) m_Titles.GetNext();
      }
      ::CloseHandle(hFile);
      return true;
   }
   return false;
}

const char* CDTitleDlg::GetInterpret()
{
   return (const char*) m_pszInterpret;
}

bool CDTitleDlg::WriteString(HANDLE hFile, char *pszString)
{
   DWORD nNewLen = 0, dwWritten;
   if (pszString != NULL) nNewLen = strlen(pszString);
   WORD wLen = 0xffff;
   BYTE bLen = 0xff;
   if (nNewLen < 0xff)
   {
      bLen = (BYTE)nNewLen;
      ::WriteFile(hFile, &bLen, sizeof(BYTE), &dwWritten, NULL);
   }
   else if (nNewLen < 0xfffe)
   {
	   WORD wLen = (WORD)nNewLen;
      ::WriteFile(hFile, &bLen, sizeof(BYTE), &dwWritten, NULL);
      ::WriteFile(hFile, &wLen, sizeof(WORD), &dwWritten, NULL);
   }
   else
   {
      ::WriteFile(hFile, &bLen   , sizeof(BYTE), &dwWritten, NULL);
      ::WriteFile(hFile, &wLen   , sizeof(WORD), &dwWritten, NULL);
      ::WriteFile(hFile, &nNewLen, sizeof(DWORD), &dwWritten, NULL);
   }
   ::WriteFile(hFile, pszString, nNewLen, &dwWritten, NULL);
   return true;
}

char * CDTitleDlg::ReadString(HANDLE hFile)
{
	DWORD nNewLen = 0, dwRead;
   char *pszString = NULL;
   BYTE bLen;
   ::ReadFile(hFile, &bLen, sizeof(BYTE), &dwRead, NULL);
   if (dwRead != sizeof(BYTE)) return NULL;

	if (bLen < 0xff)
   {
		nNewLen = bLen;
   }
   else
   {
	   WORD wLen;
      ::ReadFile(hFile, &wLen, sizeof(WORD), &dwRead, NULL);
      if (dwRead != sizeof(WORD)) return NULL;
	   if (wLen == 0xfffe)
	   {
		   return pszString; // UNICODE string prefix (length will follow)
	   }
	   else if (wLen == 0xffff)
	   {
         ::ReadFile(hFile, &nNewLen, sizeof(DWORD), &dwRead, NULL);
         if (dwRead != sizeof(DWORD)) return NULL;
	   }
	   else
      {
		   nNewLen = wLen;
      }
   }
   pszString = (char*)malloc(nNewLen+1);
   if (pszString)
   {
      ::ReadFile(hFile, pszString, nNewLen, &dwRead, NULL);
      if (dwRead != nNewLen)
      {
         free(pszString);
         return NULL;
      }
      pszString[nNewLen] = 0;
   }
   return pszString;
}

int CDTitleDlg::GetCDInfoPath(char *pszPath, int nMaxLen)
{
   int nReturn = 0;
   HKEY hKey = NULL;
   if (OpenRegistry(m_hInstance, KEY_READ|KEY_WRITE, &hKey, REGKEY_DEFAULT))
   {
      DWORD dwRead = nMaxLen;
      if (GetRegBinary(hKey, CD_INFO_PATH, (BYTE*)pszPath, dwRead, true))
      {
         nReturn = dwRead;
      }
      else
      {
         nReturn = ::GetCurrentDirectory(nMaxLen, pszPath);
         if (pszPath[lstrlen(pszPath)-1] != '\\') strncat(pszPath, "\\", nMaxLen);
         strncat(pszPath, "CD_Info\\", nMaxLen);
         bool bSave = true;
         if (::CreateDirectory(pszPath, NULL) == 0)
         {
            if (GetLastError() != ERROR_ALREADY_EXISTS) bSave = false;
         }
         if (bSave) SetRegBinary(hKey, CD_INFO_PATH, (BYTE*)pszPath, 0, true);
      }
      EtsRegCloseKey(hKey);
   }
   return nReturn;
}

void CDTitleDlg::SetCDInfoPath(char *pszPath)
{
   HKEY hKey = NULL;
   if (OpenRegistry(m_hInstance, KEY_WRITE, &hKey, REGKEY_DEFAULT))
   {
      SetRegBinary(hKey, CD_INFO_PATH, (BYTE*)pszPath, 0, true);
      EtsRegCloseKey(hKey);
   }
}

MMTime CDTitleDlg::GetTime(int n)
{
   if ((n >= 0) && (n<m_nTracks) && m_pTimes)
      return m_pTimes[n];
   else
   {
      MMTime mmT;
      mmT.dwTime = 0;
      return mmT;
   }
}

void CDTitleDlg::SetTime(int n, MMTime mmT)
{
   if ((n >= 0) && (n<m_nTracks) && m_pTimes)
      m_pTimes[n] = mmT;
}

LRESULT CDTitleDlg::OnDropFiles(HDROP hDrop, HWND hwndParent)
{
   if (m_bEditable == false)
   {
      char szPath[_MAX_PATH];
      int i, nDropped = DragQueryFile(hDrop, (UINT)-1, szPath, _MAX_PATH);
      if (nDropped > 0)
      {
         InitInterpret();
         for (i=0; i<nDropped; i++)
         {
            DragQueryFile(hDrop, i, szPath, _MAX_PATH);
            AddTitle(szPath);
         }
         m_nTracks = m_Titles.GetCounter();
         if (m_hWnd)
         {
            hwndParent = GetParent(m_hWnd);
            SendDlgItemMessage(m_nIDList, LVM_SETITEMCOUNT, m_nTracks, LVSICF_NOSCROLL);
         }
         int nTrack = 0;
         ::SendMessage(hwndParent, WM_UPDATE_CD_CTRLS, GET_CURRENT_TRACK, (LPARAM)&nTrack);
         ::SendMessage(hwndParent, WM_UPDATE_CD_CTRLS, NEW_CD_MEDIA, 0);
         ::SendMessage(hwndParent, WM_UPDATE_CD_CTRLS, SET_CURRENT_TRACK, nTrack);
         FindCommonPath();
      }
      DragFinish(hDrop);
   }
   return 0;
}

int CDTitleDlg::OnKeyDownFormatList(NMLVKEYDOWN*pNmhdr)
{
   if (pNmhdr->wVKey == VK_DELETE)
   {
      int nSel = ListView_GetSelectionMark(pNmhdr->hdr.hwndFrom);
      if (nSel != -1)
      {
         int nTrack = 0;
         ::SendMessage(m_hWndParent, WM_UPDATE_CD_CTRLS, GET_CURRENT_TRACK, (LPARAM)&nTrack);
         if (nSel < nTrack-1) nTrack--;
         char *pszTitle = (char*)m_Titles.GetAt(nSel);
         if (pszTitle)
         {
            m_Titles.Delete(pszTitle);
            m_nTracks = m_Titles.GetCounter();
            SendDlgItemMessage(m_nIDList, LVM_SETITEMCOUNT, m_nTracks, LVSICF_NOSCROLL);
            SendMessage(m_hWndParent, WM_UPDATE_CD_CTRLS, NEW_CD_MEDIA, 0);
            if (m_nTracks > 0)
               SendMessage(m_hWndParent, WM_UPDATE_CD_CTRLS, SET_CURRENT_TRACK, nTrack);
         }
      }
   }
   return 0;
}
int CDTitleDlg::OnDblClkCdTitleList(NMITEMACTIVATE*pnmA)
{
   if (pnmA->iItem!=-1)
   {
      int nTrack = 0, nNewTrack = pnmA->iItem+1;
      ::SendMessage(m_hWndParent, WM_UPDATE_CD_CTRLS, GET_CURRENT_TRACK, (LPARAM)&nTrack);
      if (nTrack != nNewTrack)
         ::SendMessage(m_hWndParent, WM_UPDATE_CD_CTRLS, NEW_CD_TRACK, nNewTrack);
   }
   return 0;
}

void CDTitleDlg::InitInterpret()
{
   bool bInitInterpret = false;
   if (m_pszInterpret != NULL)
   {
      if (m_pszInterpret[0] != '#')
      {
         int n, nCount = m_Titles.GetCounter();
         int nLen = strlen(m_pszInterpret);
         char *pszTitle, *pszNew;
         for (n=0; n<nCount; n++)
         {
            pszTitle = (char*)m_Titles.GetAt(n);
            if (pszTitle)
            {
               pszNew = (char*)malloc(nLen+strlen(pszTitle)+1);
               strcpy(pszNew, m_pszInterpret);
               strcat(pszNew, pszTitle);
               m_Titles.ReplaceAt(n, pszNew, true);
            }
         }
         bInitInterpret = true;
      }
   }
   else
   {
      bInitInterpret = true;
   }
   if (bInitInterpret)
   {
      char szPath[_MAX_PATH];
      ::LoadString(m_hInstance, IDS_TITLE_LIST_WO_PATH, szPath, _MAX_PATH);
      SetInterpret(szPath);
      UpdateContent();
   }
}

void CDTitleDlg::FindCommonPath()
{
   if ((m_pszInterpret != NULL) && (m_pszInterpret[0] == '#'))
   {
      int n, nEqu, nEqual=0, nCount = m_Titles.GetCounter();
      char *pszTitle, *pszEqual = NULL;
      for (n=0; n<nCount; n++)
      {
         pszTitle = (char*)m_Titles.GetAt(n);
         if (pszTitle)
         {
            if (!pszEqual)
            {
               pszEqual = pszTitle;
               nEqual = strlen(pszEqual);
               for (; nEqual > 1; nEqual--)
                  if (pszEqual[nEqual-1] == '\\')
                     break;
            }
            else
            {
               for (nEqu=0; (pszEqual[nEqu] != 0) && (pszTitle[nEqu] !=0); nEqu++)
                  if (pszEqual[nEqu] != pszTitle[nEqu]) break;
               if (nEqu < nEqual) nEqual = nEqu;
            }
         }
      }
      if (nEqual > 0)
      {
         char cOld = pszEqual[nEqual];
         pszEqual[nEqual] = 0;
         SetInterpret(pszEqual);
         pszEqual[nEqual] = cOld;
         char *pszTitle, *pszNew;
         for (n=0; n<nCount; n++)
         {
            pszTitle = (char*)m_Titles.GetAt(n);
            if (pszTitle)
            {
               pszNew = (char*)_strdup(&pszTitle[nEqual]);
               m_Titles.ReplaceAt(n, pszNew, true);
            }
         }
         UpdateContent();
      }
   }
}

void CDTitleDlg::UpdateContent()
{
   if (m_hWnd)
   {
      HWND hwndList = GetDlgItem(m_nIDList);
      SetDlgItemText(m_nIDInterpret, m_pszInterpret);
      InvalidateRect(hwndList, NULL, true);
      m_nTracks = m_Titles.GetCounter();
      ::SendMessage(hwndList, LVM_SETITEMCOUNT, m_nTracks, LVSICF_NOSCROLL);
   }
}
