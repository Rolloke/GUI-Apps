/********************************************************************/
/* Funktionen der Klasse CFormatDlg                                */
/********************************************************************/
#include "stdafx.h"
#include "FormatDlg.h"
#include "EtsRegistry.h"
#include "ETS3DGLRegKeys.h"
#include "resource.h"

extern HINSTANCE g_hInstance;

long CFormatDlg::gm_lOldEditCtrlWndProc = 0;

CFormatDlg::CFormatDlg()
{
   Constructor();
}

CFormatDlg::CFormatDlg(HINSTANCE hInstance, int nID, HWND hwndParent) : CEtsDialog(hInstance, nID, hwndParent)
{
   Constructor();
}

CFormatDlg::~CFormatDlg()
{
}

void CFormatDlg::Constructor()
{
   m_hIcon = NULL;
   m_Formats.SetDeleteFunction(DeleteFormat); 
}

bool CFormatDlg::OnInitDialog(HWND hWnd) 
{
   if (CEtsDialog::OnInitDialog(hWnd))
   {
      char      szText[64];
      LVCOLUMNA lvc;
      int nOrder[2]  = { 1, 0};
      ZeroMemory(&lvc, sizeof(LVCOLUMN));
      lvc.cchTextMax = 64;
      lvc.pszText    = szText;
      lvc.mask       = LVCF_FMT|LVCF_WIDTH;

      HWND hwndList = GetDlgItem(IDC_FORMAT_LIST);
      if (hwndList == NULL) return false;
      HIMAGELIST hList = ::ImageList_LoadBitmap(m_hInstance, MAKEINTRESOURCE(IDB_LIST_IMAGES), 13, 3, RGB(255,255,255));
      if (hList)
      {
         hList = (HIMAGELIST)::SendMessage(hwndList, LVM_SETIMAGELIST, LVSIL_SMALL, (LPARAM)hList);
         if (hList) ImageList_Destroy(hList);
      }
      ::SendMessage(hwndList, LVM_SETEXTENDEDLISTVIEWSTYLE,
         LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES,
         LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);

      lvc.cx = 155, lvc.fmt = LVCFMT_LEFT;
      ListView_InsertColumn(hwndList, 0,  &lvc);

      lvc.cx = 20, lvc.fmt = LVCFMT_RIGHT;
      ListView_InsertColumn(hwndList, 1,  &lvc);

      ::SendMessage(hwndList, LVM_SETCOLUMNORDERARRAY, 2, (LPARAM)&nOrder);

      ::SendMessage(hwndList, LVM_DELETEALLITEMS, 0, 0);
      SetListSize(m_Formats.GetCounter());

      return true;
   }
   return false;
}

void CFormatDlg::OnEndDialog(int nResult)
{
   HIMAGELIST hList = (HIMAGELIST) SendDlgItemMessage(IDC_FORMAT_LIST, LVM_SETIMAGELIST, LVSIL_SMALL, (LPARAM)NULL);
//   HIMAGELIST hList = (HIMAGELIST) SendDlgItemMessage(IDC_FORMAT_LIST, LVM_SETIMAGELIST, LVSIL_NORMAL, (LPARAM)NULL);
   if (hList) ImageList_Destroy(hList);
   CEtsDialog::OnEndDialog(nResult);                              // Funktion der Basisklasse aufrufen
}

int CFormatDlg::OnOk(WORD nNotifyCode)
{
   if (nNotifyCode == 0)
   {
      WriteFormats();
      return IDOK;
   }
   return 0;
}

int CFormatDlg::OnCommand(WORD nID , WORD nNotifyCode, HWND hwndControl)
{
//   bool bChanged = false;
   return CEtsDialog::OnCommand(nID , nNotifyCode, hwndControl);
}

int CFormatDlg::OnMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
   switch(nMsg)
   {
      case WM_NOTIFY:
      {
         NMHDR *pnmhdr = (NMHDR*) lParam;
         switch (pnmhdr->code)
         {
            case LVN_GETDISPINFO:    return OnGetdispinfoFormatList((LV_DISPINFO*)lParam);
            case LVN_BEGINLABELEDIT: return OnBeginlabeleditFormatList((LV_DISPINFO*)lParam);
            case LVN_ENDLABELEDIT:   return OnEndlabeleditFormatList((LV_DISPINFO*)lParam);
            case NM_CLICK:           return OnClickFormatList((NMHDR*)lParam);
            case LVN_KEYDOWN:        return OnKeyDownFormatList((NMLVKEYDOWN*)lParam);
               break;
         }
      }break;
      case WM_EDITCTRL_KEYUP: return OnEditCtrlKeyUp(wParam, lParam);
   }
   return CEtsDialog::OnMessage(nMsg, wParam, lParam);
}

int CFormatDlg::OnGetdispinfoFormatList(LV_DISPINFO* pDispInfo)
{
   if(pDispInfo->item.mask & LVIF_TEXT)
   {
      switch (pDispInfo->item.iSubItem)
      {
         case 1: wsprintf(pDispInfo->item.pszText, "%d", pDispInfo->item.iItem+1); break;
         case 0:
         {
            FormatStruct *pFS = (FormatStruct*) m_Formats.GetAt(pDispInfo->item.iItem);
            if (pFS)
            {
               lstrcpy(pDispInfo->item.pszText, pFS->szFormat); 
            }
         }break;
      }
   }
   if(pDispInfo->item.mask & LVIF_IMAGE)
   {
      FormatStruct *pFS = (FormatStruct*) m_Formats.GetAt(pDispInfo->item.iItem);
      if (pFS) pDispInfo->item.iImage = (pFS->bChecked) ? 1 : 0;
   }
   ::SetWindowLong(m_hWnd, DWL_MSGRESULT, 0);
   return 0;
}

int CFormatDlg::OnKeyDownFormatList(NMLVKEYDOWN*pNmhdr)
{
   if (pNmhdr->wVKey == VK_DELETE)
   {
      int nSel = ListView_GetSelectionMark(pNmhdr->hdr.hwndFrom);
      if (nSel != -1)
      {
         FormatStruct *pFS = (FormatStruct*) m_Formats.GetAt(nSel);
         if (pFS)
         {
            m_Formats.Delete(pFS);
            SetListSize(m_Formats.GetCounter());
         }
      }
   }
   return 0;
}

int CFormatDlg::OnClickFormatList(NMHDR*pNmhdr)
{
   LVHITTESTINFO lvHTI;
   ::GetCursorPos(&lvHTI.pt);
   ::ScreenToClient(pNmhdr->hwndFrom, &lvHTI.pt);

   ::SendMessage(pNmhdr->hwndFrom, LVM_SUBITEMHITTEST, 0, (LPARAM)&lvHTI);
   if ((lvHTI.iItem != -1) && (lvHTI.flags == LVHT_ONITEMICON))
   {
      FormatStruct *pFS = (FormatStruct*) m_Formats.GetAt(lvHTI.iItem);
      if (pFS)
      {
         pFS->bChecked = !pFS->bChecked;
         ::SendMessage(pNmhdr->hwndFrom, LVM_REDRAWITEMS, lvHTI.iItem, lvHTI.iItem);
      }
   }
   return 0;
}
int CFormatDlg::OnBeginlabeleditFormatList(LV_DISPINFO* pDispInfo) 
{
   long lResult = 0;
   HWND hwndEdit = (HWND) SendDlgItemMessage(IDC_FORMAT_LIST, LVM_GETEDITCONTROL, 0, 0);
   if (hwndEdit)
   {
      if (gm_lOldEditCtrlWndProc==0)
      {
         gm_lOldEditCtrlWndProc = ::SetWindowLong(hwndEdit, GWL_WNDPROC, (long)EditCtrlWndProc);
         m_nCurrentItem = pDispInfo->item.iItem;
      }
   }

   ::SetWindowLong(m_hWnd, DWL_MSGRESULT, lResult);
   return lResult;
}

LRESULT CALLBACK CFormatDlg::EditCtrlWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

int CFormatDlg::OnEndlabeleditFormatList(LV_DISPINFO* pDispInfo) 
{
	long lResult = 0;
   HWND hwndEdit = (HWND) SendDlgItemMessage(IDC_FORMAT_LIST, LVM_GETEDITCONTROL, 0, 0);
   if (hwndEdit)
   {
      m_nCurrentItem = -1;
      ::SetWindowLong(hwndEdit, GWL_WNDPROC, gm_lOldEditCtrlWndProc);
      gm_lOldEditCtrlWndProc = 0;
   }
   if ((pDispInfo->item.mask & LVIF_TEXT) &&
       (pDispInfo->item.pszText != NULL) &&
       (pDispInfo->item.iItem >= 0) && 
       (pDispInfo->item.iItem <  m_Formats.GetCounter()))
   {
      char *psz = strstr(pDispInfo->item.pszText, ", ");
      if (psz != NULL)
      {
         if (strlen(&psz[1]) > 3)
         {
            FormatStruct *pFmt = (FormatStruct*)malloc(strlen(pDispInfo->item.pszText)+2);
            if (pFmt)
            {
               FormatStruct *pFmtOld;
               strcpy(pFmt->szFormat, pDispInfo->item.pszText);
               pFmtOld = (FormatStruct*)m_Formats.ReplaceAt(pDispInfo->item.iItem, pFmt, false);
               if (pFmtOld)
               {
                  pFmt->bChecked = pFmtOld->bChecked;
                  free(pFmtOld);
               }
            }
    	      lResult = 1;
         }
      }
   }
   if (lResult == 1)
   {
      int n = m_Formats.FindElement(FindString, "#");
      if (n==-1)
      {
         AddFormat("#");
      }
   }
   ::SetWindowLong(m_hWnd, DWL_MSGRESULT, lResult);
   return 0;
}

LRESULT CFormatDlg::OnEditCtrlKeyUp(WPARAM wParam, LPARAM)
{
   int nItem = m_nCurrentItem;
   if (wParam == VK_UP)
   {
      nItem--;
   }
   if (wParam == VK_DOWN)
   {
      nItem++;
      if (nItem >= m_Formats.GetCounter()) nItem = -1;
   }

   if (nItem >= 0)
   {
      HWND hwndList = GetDlgItem(IDC_FORMAT_LIST);
      ::SetFocus(hwndList);
      ::SendMessage(hwndList, LVM_EDITLABEL, nItem, 0);
   }
   return 0;
}

void CFormatDlg::ReadFormats()
{
   HKEY hKey=NULL;
   m_Formats.Destroy();
   if (OpenRegistry(g_hInstance, KEY_READ, &hKey, REGKEY_FORMATS))
   {
      DWORD dwRead;
      int i;
      char szFormat[64];
      char szKey[64];
      for (i=0; ; i++)
      {
         dwRead = 64;
         wsprintf(szKey, "%s%d", REGKEY_FORMATS, i);
         if (GetRegBinary(hKey, szKey, (BYTE*)szFormat, dwRead, true))
         {
            AddFormat(&szFormat[1], (szFormat[0]!=0) ? true : false);
         }
         else break;
      }
   }
   if (hKey) EtsRegCloseKey(hKey);
   if (m_Formats.GetCounter() == 0)                            // Keine Formate in der registry
   {
      AddFormat("*.WAV, waveaudio", true);                     // dann wenigstens Wave
      AddFormat("*.MID, sequencer", true);                     // und Midi und...

      // cdaudio, waveaudio, sequencer, vcr, overlay, videodisc, digitalvideo
   }
   AddFormat("#");
}

void CFormatDlg::WriteFormats()
{
   HKEY hKey=NULL;

   if (OpenRegistry(g_hInstance, KEY_WRITE, &hKey, REGKEY_FORMATS))
   {
      int i, nF;
      FormatStruct *pFS;
      char *psz;
      char szKey[64];
      char szExt[_MAX_PATH] = "";
      for (i=0, nF=0; i<m_Formats.GetCounter(); i++)
      {
         pFS = (FormatStruct*) m_Formats.GetAt(i);
         if (pFS->szFormat[0] == '#') break;
         psz = strstr(pFS->szFormat, ", ");
         if ((psz != NULL) && pFS->bChecked)
         {
            psz[1] = 0;
            strcat(szExt, pFS->szFormat);
            psz[1] = ' ';
         }
         wsprintf(szKey, "%s%d", REGKEY_FORMATS, nF++);
         SetRegBinary(hKey, szKey, (BYTE*)pFS, strlen(pFS->szFormat)+1, true);
      }
      i = strlen(szExt);
      if (i>0)
      {
         szExt[i-1] = 0;
         SetRegBinary(hKey, AUDIO_EXTENSION, (BYTE*)szExt, 0, true);
      }
      EtsRegCloseKey(hKey);
   }
}

int CFormatDlg::FindString(const void *pszObj, const void *pszSearch)
{
   FormatStruct *pFmt = (FormatStruct *)pszObj;
   char *psz = strstr((const char*)pFmt->szFormat, (const char*)pszSearch);
   return (psz == NULL) ? 1: 0;
}

void CFormatDlg::SetListSize(int nCount)
{
   SendDlgItemMessage(IDC_FORMAT_LIST, LVM_SETITEMCOUNT, nCount, LVSICF_NOSCROLL);
}

void CFormatDlg::AddFormat(char *pszFormat, bool bChecked)
{
   FormatStruct *pFmt = (FormatStruct*)malloc(strlen(pszFormat)+2);
   pFmt->bChecked = bChecked;
   strcpy(pFmt->szFormat, pszFormat);
   m_Formats.ADDHead(pFmt);
   SetListSize(m_Formats.GetCounter());
}

void CFormatDlg::DeleteFormat(void *p)
{
   FormatStruct *pFmt = (FormatStruct*)p;
   free(pFmt);
}
