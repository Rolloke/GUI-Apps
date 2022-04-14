/********************************************************************/
/* Funktionen der Klasse CCDPLDlg                                   */
/********************************************************************/
#include "stdafx.h"
#include "CCDPLDlg.h"
#include "resource.h"
#include "EtsRegistry.h"
#include "ETS3DGLRegKeys.h"
#include "CEtsFileDlg.h"
#include "CEtsHelp.h"
#include "MixerOptionDlg.h"

#define  ETSDEBUG_INCLUDE 
#include "CEtsDebug.h"

#include <MMREG.H>
#include <Msacm.h>

#define TITLES_FOR_RECORDING      0x80000000

char CCDPLDlg::gm_szFormatCDTime[64]="";
char CCDPLDlg::gm_szFormatCDTitle[64]="";

char CCDPLDlg::gm_szImportPath[MAX_PATH]="";
bool CCDPLDlg::gm_bOwnerDraw            = true;


CCDPLDlg::CCDPLDlg()
{
   Constructor();
}

CCDPLDlg::CCDPLDlg(HINSTANCE hInstance, int nID, HWND hwndParent) : CEtsDialog(hInstance, nID, hwndParent)
{
   Constructor();
}

void CCDPLDlg::Constructor()
{
   m_nDisplayDivX       = 330;
   m_bUseMsgBoxForError = true;
   m_bHitDisplayDivX    = false;
   m_pCDDlgStruct       = NULL;
   m_nUpdateControl     = CONTROL_UPDATE;
   m_hMixer             = NULL;
   m_wAlternateEjectIcon= 1;

   HANDLE hModule = ::GetModuleHandle("uxtheme.dll");
   if (hModule) gm_bOwnerDraw = false;
}

CCDPLDlg::~CCDPLDlg()
{
}

UINT CCDPLDlg::SetTimerSave(UINT nIDEvent, UINT nElapse, void (CALLBACK *lpfnTimer)(HWND,UINT,UINT,DWORD))
{
   UINT nResult = ::SetTimer(m_hWnd, nIDEvent, nElapse, lpfnTimer);
   if (nResult)
   {
      m_Timers.ADDTail((void*)nIDEvent);
   }
   return nResult;
}

void CCDPLDlg::KillTimerSave(UINT nID)
{
   MSG msg;
   ZeroMemory(&msg, sizeof(MSG));
   ::KillTimer(m_hWnd, nID);
   _asm CLD;
   PeekMessage(&msg, m_hWnd, WM_TIMER, WM_TIMER, PM_REMOVE|PM_NOYIELD);

   m_Timers.Remove((void*)nID);
}

bool CCDPLDlg::OnInitDialog(HWND hWnd) 
{
   if (CEtsDialog::OnInitDialog(hWnd))
   {
      LOGFONT lf;
      DWORD  dwRead = 0;
      HKEY hKey = NULL;
      m_Timers.SetDestroyMode(false);
      m_wOldEjectIconID = 0;
      m_hMixer = NULL;
      m_MixerDlg.CountMixerCtrInst(hWnd, true);

      if (gm_szFormatCDTime[0] == 0)
      {
         ::LoadString(m_hInstance, IDS_CD_TIME, gm_szFormatCDTime, 64); 
      }
      if (gm_szFormatCDTitle[0] == 0)
      {
         ::GetWindowText(m_hWnd, gm_szFormatCDTitle, 64);
      }
      HICON hIcon = ::LoadIcon(m_hInstance, MAKEINTRESOURCE(IDR_FILE_TYPECD));
      SendMessage(m_hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

      m_CDPlayer.m_hOwner = m_hWnd;
      if (OpenRegistry(m_hInstance, KEY_READ, &hKey, REGKEY_CD_DISPLAY))
      {
         dwRead = sizeof(LOGFONT);
         if (!GetRegBinary(hKey, CD_DISPLAY_FONT, (BYTE*)&lf, dwRead)) dwRead=0;
         m_CDPlayer.m_dwCDDiplayBkGnd = GetRegDWord(hKey, CD_DISPLAY_BKGND, RGB(0, 0x60, 0));
         m_CDPlayer.m_dwCDDiplayText  = GetRegDWord(hKey, CD_DISPLAY_TEXT , RGB(0, 0xff, 0));
         m_nDisplayDivX               = (short)GetRegDWord(hKey, CD_DISPLAY_DIV, 330);
         m_nRoundRectRadius           = GetRegDWord(hKey, "RoundRect",  16);
         m_wAlternateEjectIcon        = (WORD)GetRegDWord(hKey, "EjectIcon",  1);
         if (m_wAlternateEjectIcon > 11) m_wAlternateEjectIcon = 11;
         DWORD dwOwnerDraw = (long)GetRegDWord(hKey, "OwnerDraw",  INVALID_VALUE);
         if (dwOwnerDraw != INVALID_VALUE) gm_bOwnerDraw = (dwOwnerDraw != 0) ? true : false;
      } 
      if (hKey) EtsRegCloseKey(hKey);
      if (dwRead != sizeof(LOGFONT))
      {
         ZeroMemory(&lf, sizeof(LOGFONT));
         lf.lfHeight  = -20;
         lf.lfWidth   =  8;
         lf.lfCharSet = ANSI_CHARSET;
         lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
         lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
         lf.lfQuality = PROOF_QUALITY;
         lf.lfPitchAndFamily = FIXED_PITCH|FF_MODERN;
      }
      m_CDPlayer.DeleteCDFont();
      m_CDPlayer.m_hFontCDtime = CreateFontIndirect(&lf);
      ::EnumChildWindows(m_hWnd, EnumChildWndFunc, (LPARAM) this);

      m_TitleDlg.Init(m_hInstance, IDD_CD_TITLE, m_hWnd);
      m_MixerDlg.Init(m_hInstance, IDD_MIXER, m_hWnd);

      if (m_pCDDlgStruct != NULL)
      {
         if (m_pCDDlgStruct->nTitles&TITLES_FOR_RECORDING)
         {
            REPORT("Recording Device");
            CEtsFileDlg dlg(m_hInstance, 0, m_hWnd);
            char szFilter[_MAX_PATH];
            if (gm_szImportPath[0] == 0)
            {
               if (GetRegBinary(hKey, CD_DRIVE, (BYTE*)&gm_szImportPath, dwRead, true))
               RemoveCharacter(gm_szImportPath, '\\', true);
            }
            dlg.SetInitialDir(CCDPLDlg::gm_szImportPath);
            ::SendMessage(m_hWnd, WM_UPDATE_CD_CTRLS, GET_MM_FILTERS, (LPARAM)szFilter);
            dlg.SetFilter(szFilter, strlen(szFilter));
            dlg.SetTitle(IDS_LOAD_FILE_TITLE);
            dlg.ModifyFlags(OFN_NOCHANGEDIR|OFN_HIDEREADONLY, OFN_SHOWHELP);
            if (dlg.GetOpenFileName())
            {
               strcpy(szFilter, dlg.GetPathName());
               strcat(szFilter, dlg.GetFileTitle());
               if (::GetFileAttributes(szFilter) != INVALID_VALUE)
               {
                  m_TitleDlg.AddTitle((char*)szFilter);
                  m_TitleDlg.m_nTracks = m_TitleDlg.m_Titles.GetCounter();
                  ::LoadString(m_hInstance, IDS_TITLE_LIST_WO_PATH, szFilter, _MAX_PATH);
                  m_TitleDlg.SetInterpret(szFilter);

                  m_CDPlayer.m_DeviceStatus.StopAtTrackEnd = true;
                  m_CDPlayer.m_DeviceStatus.WaveFiles      = true;
                  m_CDPlayer.m_DeviceStatus.RecordingDev   = true;
                  m_CDPlayer.m_dwWaveTrack = 1;

                  SendMessage(m_hWnd, WM_UPDATE_CD_CTRLS, NEW_CD_MEDIA, 0);
                  SendMessage(m_hWnd, WM_UPDATE_CD_CTRLS, NEW_CD_TRACK, 1);
//                  m_CDPlayer.UpdateCDCtrls();
                  ::LoadString(m_hInstance, IDC_AC_CD_RECORD, szFilter, _MAX_PATH);
                  SetWindowText(GetDlgItem(IDC_AC_CK_PLAY), szFilter);

                  m_MixerDlg.ModifyInitFlag(WAVE_LINES, 0);
                  m_MixerDlg.m_nForWhat = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
                  m_MixerDlg.Create();
               }
            }
         }
         else if (m_pCDDlgStruct->nTitles>0)
         {
            if (m_CDPlayer.OpenWaveDevice(m_pCDDlgStruct->ppszTitles[0]))
            {
               REPORT("Wave Player");
               InitTitles();

               m_CDPlayer.m_DeviceStatus.CompareTitle   = true;
               m_CDPlayer.m_DeviceStatus.StopAtTrackEnd = true;

               m_CDPlayer.m_dwWaveTrack = 1;
               SendMessage(m_hWnd, WM_UPDATE_CD_CTRLS, NEW_CD_MEDIA, 0);
               SendMessage(m_hWnd, WM_UPDATE_CD_CTRLS, NEW_CD_TRACK, 1);
//               m_CDPlayer.UpdateCDCtrls();

               m_MixerDlg.ModifyInitFlag(WAVE_LINES, 0);
/*
               DWORD dwInit = m_MixerDlg.ModifyInitFlag(WAVE_LINES, 0);
               if ((dwInit & WAVE_LINES) && (dwInit & CD_LINES))
               {
                  m_MixerDlg.OpenMixerDev(m_hWnd);
                  m_MixerDlg.Create();
               }
*/
            }
         }
      }

      if (!m_CDPlayer.m_DeviceStatus.Open)
      {
         if (OpenRegistry(m_hInstance, KEY_READ, &hKey, REGKEY_DEFAULT))
         {
            char szPath[_MAX_PATH];
            dwRead = _MAX_PATH;
            if (GetRegBinary(hKey, CD_DRIVE, (BYTE*)&szPath, dwRead, true))
            {
               m_CDPlayer.m_OpenParams.lpstrElementName = _strdup(szPath);
            }
         }
         if (hKey) EtsRegCloseKey(hKey);
         m_CDPlayer.OpenCDPlayer();
         if (m_CDPlayer.m_DeviceStatus.WaveFiles)
            m_MixerDlg.ModifyInitFlag(WAVE_LINES, 0);
         else if (m_CDPlayer.m_DeviceStatus.CanEject)
            m_MixerDlg.ModifyInitFlag(CD_LINES, 0);
      }

      if (!m_MixerDlg.IsOpened())
      {
         m_MixerDlg.OpenMixerDev(m_hWnd);
         m_hMixer = m_MixerDlg.GetWaveMixer();
      }

      if (m_CDPlayer.m_OpenParams.lpstrElementName)
         REPORT("Device : %s", m_CDPlayer.m_OpenParams.lpstrElementName);
      m_bUseMsgBoxForError = false;
      SetEjectBtnImage();
      AdaptWindowSize();
      m_TitleDlg.FindCommonPath();
      return true;
   }
   return false;
} 
 
void CCDPLDlg::OnEndDialog(int nResult)
{
   HKEY hKey = NULL;
   if (OpenRegistry(m_hInstance, KEY_WRITE, &hKey, REGKEY_DEFAULT))
   {
      if (m_CDPlayer.m_OpenParams.lpstrElementName)
      {
         if (m_CDPlayer.m_DeviceStatus.WaveFiles)
         {
            if ((m_pCDDlgStruct == NULL) || (m_pCDDlgStruct->nTitles == 0))
            {
               const char * pszInterpret = m_TitleDlg.GetInterpret();
               if (pszInterpret) SetRegBinary(hKey, CD_DRIVE, (BYTE*)pszInterpret, 0, true);
            }
         }
         else
         {
            SetRegBinary(hKey, CD_DRIVE, (BYTE*)m_CDPlayer.m_OpenParams.lpstrElementName, 0, true);
         }
      }
   }
   if (hKey) EtsRegCloseKey(hKey);

   if (m_CDPlayer.m_DeviceStatus.RecordingDev)
   {
      if (MessageBox(IDS_CD_SAVE_TEXT, IDS_CD_SAVE_TITLE, MB_YESNO|MB_ICONQUESTION) == IDYES)
      {
         m_CDPlayer.SaveFile((char*)m_TitleDlg.m_Titles.GetFirst());
      }
   }
   
   if (m_CDPlayer.m_DeviceStatus.WaveFiles)
      m_MixerDlg.ModifyInitFlag(0, WAVE_LINES);
   else if (m_CDPlayer.m_DeviceStatus.CanEject)
      m_MixerDlg.ModifyInitFlag(0, CD_LINES);

   m_CDPlayer.CloseDevice();

   m_MixerDlg.CountMixerCtrInst(m_hWnd, false);
   m_MixerDlg.CloseMixerDev();

   if (m_MixerDlg.GetWindowHandle())
   {
      ::SendMessage(m_MixerDlg.GetWindowHandle(), WM_COMMAND, MAKELONG(IDCANCEL,0), 0);
   }
   if (m_TitleDlg.GetWindowHandle())
   {
      ::SendMessage(m_TitleDlg.GetWindowHandle(), WM_COMMAND, MAKELONG(IDCANCEL,0), 0);
   }

   if (OpenRegistry(m_hInstance, KEY_WRITE, &hKey, REGKEY_CD_DISPLAY))
   {
      SetRegDWord(hKey, CD_DISPLAY_DIV, m_nDisplayDivX);
   }
   if (hKey) EtsRegCloseKey(hKey);

   UINT nID;
   while ((nID = (UINT)m_Timers.GetAt(0)) != 0)
   {
      KillTimerSave(nID);
   }

   CEtsDialog::OnEndDialog(nResult);                              // Funktion der Basisklasse aufrufen
}
 
int CCDPLDlg::OnOk(WORD)
{
   HWND hwnd = ::GetFocus();
   if (hwnd)
   {
      int nID = GetDlgCtrlID(hwnd);
      switch (nID)
      {
         case IDC_AC_BTN_TITLE1: case IDC_AC_BTN_TITLE2: case IDC_AC_BTN_TITLE3: case IDC_AC_BTN_TITLE4: case IDC_AC_BTN_TITLE5:
         case IDC_AC_BTN_TITLE6: case IDC_AC_BTN_TITLE7: case IDC_AC_BTN_TITLE8: case IDC_AC_BTN_TITLE9: case IDC_AC_BTN_TITLE10:
         case IDC_AC_CK_PLAY:    case IDC_AC_CK_STOP:    case IDC_AC_CK_PAUSE:
         case IDC_AC_BTN_PREV:   case IDC_AC_BTN_PREV_C: case IDC_AC_BTN_NEXT_C: case IDC_AC_BTN_NEXT:
         case IDC_AC_BTN_EJECT:
            SendMessage(m_hWnd, WM_COMMAND, nID, (LPARAM)hwnd);
            break;
      }
   }
   return 0;
}

int CCDPLDlg::OnCommand(WORD nID , WORD nNotifyCode, HWND hwndControl)
{
   if (nNotifyCode == CBN_SELCHANGE)
   {
      if (nID == IDC_AC_COMBO_CD_TITLE)
      {
         DWORD dwTrack = SendDlgItemMessage(IDC_AC_COMBO_CD_TITLE, CB_GETCURSEL, 0, 0)+1;
         m_CDPlayer.ChangeCurrentTrack(dwTrack);
      }
   }
   else if (nNotifyCode == BN_CLICKED)
   {
      switch (nID)
      {
         case IDC_AC_CK_PLAY:    m_CDPlayer.ClickPlay();  break;
         case IDC_AC_CK_STOP:    m_CDPlayer.ClickStop();  break;
         case IDC_AC_CK_PAUSE:   m_CDPlayer.ClickPause(); break;
         case IDC_AC_BTN_NEXT:   m_CDPlayer.ClickNext();  break;
         case IDC_AC_BTN_PREV:   m_CDPlayer.ClickPrev();  break;
         case IDC_AC_BTN_EJECT:
         if (m_CDPlayer.m_DeviceStatus.CanEject)
         {
            m_CDPlayer.ClickEject();
         }
         else
         {
            if (gm_bOwnerDraw) SendDlgItemMessage(IDC_AC_BTN_EJECT, BM_SETCHECK, BST_CHECKED, 0);
            OnOptionsCdDrive();
            if (gm_bOwnerDraw) SendDlgItemMessage(IDC_AC_BTN_EJECT, BM_SETCHECK, BST_UNCHECKED, 0);
         } break;
         case IDC_AC_BTN_TITLE1: case IDC_AC_BTN_TITLE2: case IDC_AC_BTN_TITLE3: case IDC_AC_BTN_TITLE4: case IDC_AC_BTN_TITLE5: 
         case IDC_AC_BTN_TITLE6: case IDC_AC_BTN_TITLE7: case IDC_AC_BTN_TITLE8: case IDC_AC_BTN_TITLE9: case IDC_AC_BTN_TITLE10: 
         {
            int nTrack = nID-IDC_AC_BTN_TITLE1;
            DWORD dwTrack = nTrack+1;
            if (m_CDPlayer.GetTrackLength(dwTrack))
            {
               m_CDPlayer.ChangeCurrentTrack(dwTrack);
               if (!m_CDPlayer.m_DeviceStatus.Playes)
               {
                  if (m_CDPlayer.m_DeviceStatus.CompareTitle)
                  {
                     if (m_CDPlayer.m_DeviceStatus.WaveFiles)
                     {
                        DWORD dwStart, dwEnd;
                        dwStart = m_CDPlayer.m_mmCurrentTrackTime.GetWaveTime();
                        dwEnd   = m_CDPlayer.m_mmTrackTime.GetWaveTime();
                        m_CDPlayer.PlayDevice(dwStart, dwEnd);
                     }
                     else
                     {
                        m_CDPlayer.PlayDevice();
                     }
                  }
                  else
                  {
                     DWORD dwStart, dwEnd;
                     if (m_CDPlayer.m_DeviceStatus.WaveFiles)
                     {
                        dwStart = m_CDPlayer.m_mmCurrentTrackTime.GetWaveTime();
                        dwEnd   = m_CDPlayer.m_mmTrackTime.GetWaveTime();
                     }
                     else
                     {
                        m_CDPlayer.m_mmCurrentTrackTime.dwTime = 0;
                        m_CDPlayer.m_mmCurrentTrackTime.tmsfTime.Track = dwTrack;
                        dwStart = m_CDPlayer.m_mmCurrentTrackTime.dwTime; 
                        dwEnd   = m_CDPlayer.m_mmTrackTime.dwTime;
                     }
                     m_CDPlayer.PlayDevice(dwStart, dwEnd);
                  }
               }
            }
         } break;
      }
   }
   switch (nID)
   {
      case ID_OPTIONS_CD_DRIVE: OnOptionsCdDrive(); break;
      case ID_OPTIONS_CD_DISPLAY_COLOR:   m_CDPlayer.SelectDisplayColor();   break;
      case ID_OPTIONS_CD_DISPLAY_FONT:    m_CDPlayer.SelectDisplayFont();    break;
      case ID_OPTIONS_CD_DISPLAY_COLOR_BK:m_CDPlayer.SelectDisplayBkColor(); break;
      case ID_OPTIONS_VOLUME:
      {
         CMixerOptionDlg dlg(m_hInstance, IDD_MIXER_OPTIONS, m_hWnd);
         dlg.DoModal();
      }break;
      case ID_EDIT_FORMATS:               m_CDPlayer.EditFormats();          break;
      case ID_CHANGE_VOLUMES:
      {
         if (m_MixerDlg.ModifyInitFlag() & INIT_LINES) m_MixerDlg.ModifyInitFlag(0, INIT_LINES);
         else                                          m_MixerDlg.ModifyInitFlag(INIT_LINES, 0);
      } break;
      case ID_EDIT_VOLUMES:
      {
         m_MixerDlg.m_nForWhat = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
         m_MixerDlg.Create();
         ::PostMessage(m_MixerDlg.GetWindowHandle(), WM_EXITSIZEMOVE, EXIT_SIZE_MOVE_SET|EXIT_SIZE_MOVE_DOCK, 0);
      } break;
      case ID_EDIT_CD_LIST:
      {
         m_TitleDlg.Create();              
         ::PostMessage(m_TitleDlg.GetWindowHandle(), WM_EXITSIZEMOVE, EXIT_SIZE_MOVE_SET|EXIT_SIZE_MOVE_DOCK, 0);
      }break;
      case ID_OPTIONS_CD_ELAPSED_TIME: case ID_OPTIONS_CD_REMAINING_TIME:
      case ID_OPTIONS_CD_ELAPSED_TOTAL:case ID_OPTIONS_CD_REMAINING_TOTAL:
         m_CDPlayer.m_DeviceStatus.TimeOptions = nID - ID_OPTIONS_CD_ELAPSED_TIME;
         break;
      case ID_OPTIONS_CD_COMPARE:
         m_CDPlayer.m_DeviceStatus.CompareTitle = !m_CDPlayer.m_DeviceStatus.CompareTitle;
         AdaptWindowSize();
         break;
      case ID_OPTIONS_CD_STOP_AT_TRACK_END:
         m_CDPlayer.m_DeviceStatus.StopAtTrackEnd = !m_CDPlayer.m_DeviceStatus.StopAtTrackEnd;
         break;
      case ID_EDIT_CD_LIST_PATH:
      {
         char szPath[_MAX_PATH] = "";
         char szTitle[64];
         ::LoadString(m_hInstance, IDS_SELECT_CD_INFO_PATH, szTitle, 64);
         m_TitleDlg.GetCDInfoPath(szPath, _MAX_PATH);
         if (GetFolderPath(szPath, NULL, FOLDERPATH_CONCAT_SLASH, szTitle, m_hWnd))
         {
            m_TitleDlg.SetCDInfoPath(szPath);
         }
      }
   }
   return CEtsDialog::OnCommand(nID , nNotifyCode, hwndControl);
}

int CCDPLDlg::OnMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
   switch (nMsg)
   {
      case WM_TIMER: OnTimer(wParam); break;
      case WM_UPDATE_CD_CTRLS: return OnUpdateCdCtrls(wParam, lParam);
      case WM_DRAWITEM: return OnDrawItem((DRAWITEMSTRUCT*)lParam);
      case WM_SYSCOLORCHANGE:
      if ((m_hMixer != NULL) && m_CDPlayer.m_DeviceStatus.CompareTitle)
      {
         for (int i=IDC_SLIDER_TITLE1; i<=IDC_SLIDER_TITLE10; i++)
         {
            SendDlgItemMessage(i, nMsg, wParam, lParam);
         }
      } break;
      case WM_DROPFILES:
      {
         return m_TitleDlg.OnDropFiles((HDROP) wParam, m_hWnd);
      }
      case MM_MIXM_CONTROL_CHANGE:
      if ((wParam == (WPARAM)m_hMixer) && m_CDPlayer.m_DeviceStatus.CompareTitle && (m_MixerDlg.GetWaveCtrlID() == lParam))
      {
         if (m_nUpdateControl == CONTROL_UPDATE)
         {
            long lValue = m_MixerDlg.GetWaveVolume();
            int nTrack = m_CDPlayer.GetCurrentTrack()-1;
            if ((nTrack>=0)&&(nTrack<10))
            {
               SendDlgItemMessage(IDC_SLIDER_TITLE1 + nTrack, TBM_SETPOS, 1, lValue);
            }
         }
         if (m_nUpdateControl == CONTROL_NO_UPDATE_ONCE)
         {
            m_nUpdateControl = CONTROL_UPDATE;
         }
      }break;
      case WM_NOTIFY:
      if ((m_hMixer != NULL) && m_CDPlayer.m_DeviceStatus.CompareTitle)
      {
         NMHDR *phdr = (NMHDR*) lParam;
         if (phdr->code == TTN_GETDISPINFO)
         { 
            NMTTDISPINFO*pnmtdi = (NMTTDISPINFO*)lParam;
            wsprintf(pnmtdi->szText, "%d", 100-atoi(pnmtdi->szText));
         }
         else if (m_CDPlayer.m_DeviceStatus.Playes && 
                 (m_CDPlayer.GetCurrentTrack() == (int)(phdr->idFrom - IDC_SLIDER_TITLE1+1)))
         {
            if (phdr->code == NM_RELEASEDCAPTURE)
            {
               m_nUpdateControl = CONTROL_NO_UPDATE_ONCE;
               m_MixerDlg.SetWaveVolume(::SendMessage(phdr->hwndFrom, TBM_GETPOS, 0, 0));
            }
            else if (phdr->code == NM_CUSTOMDRAW)
            {
               if (GetCapture() == phdr->hwndFrom) m_nUpdateControl = CONTROL_NO_UPDATE;
               else                                m_nUpdateControl = CONTROL_UPDATE;
               m_MixerDlg.SetWaveVolume(::SendMessage(phdr->hwndFrom, TBM_GETPOS, 0, 0));
            }
         }
      } break;
      case WM_HELP:
      if (lParam)
      {
         HELPINFO *pHI = (HELPINFO*) lParam;
         if ((pHI->iContextType == HELPINFO_WINDOW) || (pHI->iContextType == HELPINFO_MENUITEM))
         {
            char format[64], text[64];
            if ((pHI->iCtrlId >= IDC_SLIDER_TITLE1) && (pHI->iCtrlId <= IDC_SLIDER_TITLE10))
            {
               ::LoadString(m_hInstance, IDC_SLIDER_TITLE1, format,  64);
               wsprintf(text, format, 1 + pHI->iCtrlId - IDC_SLIDER_TITLE1);
               CEtsHelp::CreateContextWnd(text, pHI->MousePos.x, pHI->MousePos.y);
               return 1;
            }
            else if (pHI->iCtrlId ==IDC_AC_BTN_EJECT)
            {
               if (!m_CDPlayer.m_DeviceStatus.CanEject) pHI->iCtrlId = ID_OPTIONS_CD_DRIVE;
               CEtsDialog::OnMessage(nMsg, wParam, lParam);
            }
         }
      }break;
      case WM_MENUSELECT:
      {
         UINT uItem  = (UINT) LOWORD(wParam); // menu item or submenu index 
         UINT uFlags = (UINT) HIWORD(wParam); // menu flags 
         HMENU hmenu = (HMENU) lParam;        // handle to menu clicked 
         if ((uFlags != 0xffff) && (hmenu != NULL))
         {
            char szText[128];
            HWND hWnd = GetDlgItem(IDC_AC_CD_TIME);
            if (hWnd!= NULL)
            {
               m_CDPlayer.m_DeviceStatus.ErrorText = true;
               if (::LoadString(m_hInstance, uItem, m_szCDWndText, MAX_PATH) == 0)
               {
                  m_szCDWndText[0] = 0;
               }
               ::SendMessage(hWnd, WM_DRAW_CD_DISPLAY, 0, (LPARAM)szText);
            }
         }
      } break;
      case WM_EXITMENULOOP:
      {
         HWND hWnd = GetDlgItem(IDC_AC_CD_TIME);
         m_CDPlayer.m_DeviceStatus.ErrorText = false;
         ::InvalidateRect(hWnd, NULL, true);
      } break;
      case WM_INITMENUPOPUP:
      {
         HMENU hMenu = (HMENU) wParam;
         ::EnableMenuItem(hMenu, SC_SIZE, MF_BYCOMMAND|MF_GRAYED);
      } break;
      case WM_CONTEXTMENU:
      {
         POINT ptContext;
         RECT rcClient;
         POINTSTOPOINT(ptContext, lParam);
         ::ScreenToClient(m_hWnd, &ptContext);
         ::GetClientRect(m_hWnd, &rcClient);
         if (::PtInRect(&rcClient, ptContext))
         {
            HMENU hMenu = ::LoadMenu(m_hInstance, MAKEINTRESOURCE(IDR_CONTEXT1));
            if (hMenu)
            {
               HMENU hContext = ::GetSubMenu(hMenu, 0);
               if (hContext)
               {
                  ::CheckMenuItem(hContext, ID_OPTIONS_CD_COMPARE, MF_BYCOMMAND|(m_CDPlayer.m_DeviceStatus.CompareTitle ? MF_CHECKED:MF_UNCHECKED));
                  ::CheckMenuItem(hContext, ID_OPTIONS_CD_STOP_AT_TRACK_END, MF_BYCOMMAND|(m_CDPlayer.m_DeviceStatus.StopAtTrackEnd ? MF_CHECKED:MF_UNCHECKED));
                  ::CheckMenuItem(hContext, ID_CHANGE_VOLUMES,  MF_BYCOMMAND|((m_MixerDlg.ModifyInitFlag() & INIT_LINES) ? MF_CHECKED:MF_UNCHECKED));
                  ::EnableMenuItem(hContext,ID_EDIT_CD_LIST  , (m_CDPlayer.m_DeviceStatus.MediaPresent) ? MF_ENABLED : MF_DISABLED|MF_GRAYED);
                  ::EnableMenuItem(hContext,ID_EDIT_VOLUMES  , (m_MixerDlg.m_nMixerDev != INVALID_VALUE) ? MF_ENABLED : MF_DISABLED|MF_GRAYED);
                  ::EnableMenuItem(hContext,ID_CHANGE_VOLUMES, (m_MixerDlg.m_nMixerDev != INVALID_VALUE) ? MF_ENABLED : MF_DISABLED|MF_GRAYED);

                  HMENU hDisplay = ::GetSubMenu(hContext, 0);
                  if (hDisplay)
                  {
                     ::CheckMenuRadioItem(hDisplay, ID_OPTIONS_CD_ELAPSED_TIME, ID_OPTIONS_CD_REMAINING_TOTAL, ID_OPTIONS_CD_ELAPSED_TIME+m_CDPlayer.m_DeviceStatus.TimeOptions, MF_BYCOMMAND);
                  }
                  ::GetClientRect(GetDlgItem(IDC_AC_CD_TIME), &rcClient);
                  if (ptContext.y < rcClient.bottom) ptContext.y = rcClient.bottom;
                  ::ClientToScreen(m_hWnd, &ptContext);
                  ::TrackPopupMenu(hContext, 0, ptContext.x, ptContext.y, 0, m_hWnd, NULL);
               }
               ::DestroyMenu(hMenu);
            }
         }
      }
      break;
   }
   int nResult = CEtsDialog::OnMessage(nMsg, wParam, lParam);
   return nResult;
}


void CCDPLDlg::OnTimer(UINT nIDEvent)
{
   switch (nIDEvent)
   {
      case ID_UPDATE_CD_CTRLS:  m_CDPlayer.UpdateCDCtrls();  break;
      case ID_POLL_CD_STATUS:   m_CDPlayer.GetCDStatus();    break;
      case ID_POLL_CD_POSITION: m_CDPlayer.PollCDPosition(); break;

      case ID_WAIT_CD_ERROR_TIMER:                             // Update der Fehleranzeige in der CD-Anzeige
      {  
         KillTimerSave(ID_WAIT_CD_ERROR_TIMER);                // nach Ablauf dieser Message wieder möglich
         m_CDPlayer.m_DeviceStatus.ErrorText = false;
         m_CDPlayer.UpdateCDDisplay();
      }break;
   }
}

LRESULT CCDPLDlg::OnUpdateCdCtrls(WPARAM wParam, LPARAM lParam)
{
   switch (wParam)
   {
      case DISABLE_CTRLS:
      {
         if (m_CDPlayer.m_DeviceStatus.NoDisableOnClose)
            return 0;
         EnableWindow(GetDlgItem(IDC_AC_CK_PLAY   ), false);
         EnableWindow(GetDlgItem(IDC_AC_CK_STOP   ), false);
         EnableWindow(GetDlgItem(IDC_AC_CK_PAUSE  ), false);
         EnableWindow(GetDlgItem(IDC_AC_BTN_PREV  ), false);
         EnableWindow(GetDlgItem(IDC_AC_BTN_PREV_C), false);
         EnableWindow(GetDlgItem(IDC_AC_BTN_NEXT  ), false);
         EnableWindow(GetDlgItem(IDC_AC_BTN_NEXT_C), false);
         EnableWindow(GetDlgItem(IDC_AC_COMBO_CD_TITLE), false);

         for (int nBtn=IDC_AC_BTN_TITLE1; nBtn<=IDC_AC_BTN_TITLE10; nBtn++)
         {
            EnableWindow(GetDlgItem(nBtn), false);
         }
      } break;
      case UPDATE_CTRLS:
      {
         bool  bPlay    = m_CDPlayer.m_DeviceStatus.Playes;
         bool  bMediaOK = m_CDPlayer.m_DeviceStatus.MediaAudio;
//                     || m_CDPlayer.m_DeviceStatus.MediaPresent;
//                     || m_CDPlayer.m_DeviceStatus.MediaOther;
         bool  bNext    = bMediaOK;
         bool  bPrev    = bMediaOK;
         int   nTracks  = 0;
         int   nTrack   = 0;


         if (m_CDPlayer.m_DeviceStatus.CanPlay)
         {
            nTracks = m_CDPlayer.GetTracks();
            nTrack  = m_CDPlayer.GetCurrentTrack();
            bNext   = (nTrack < nTracks) ? true : false;
            bPrev   = (nTrack >       1) ? true : false;
            EnableWindow(GetDlgItem(IDC_AC_COMBO_CD_TITLE), (nTracks > 0) ? true : false);
            if (!bMediaOK) bPlay = false;
         }
         else bPlay = false;
         
         EnableWindow(GetDlgItem(IDC_AC_CK_STOP), bMediaOK);
         EnableWindow(GetDlgItem(IDC_AC_CK_PLAY), bMediaOK);
         EnableWindow(GetDlgItem(IDC_AC_CK_PAUSE), bMediaOK);

         EnableWindow(GetDlgItem(IDC_AC_BTN_NEXT)  , bNext);
         EnableWindow(GetDlgItem(IDC_AC_BTN_NEXT_C), bPlay);
         EnableWindow(GetDlgItem(IDC_AC_BTN_PREV)  , bPrev);
         EnableWindow(GetDlgItem(IDC_AC_BTN_PREV_C), bPlay);
         if (gm_bOwnerDraw)
         {
            if (lParam & UPDATE_START_BTN) InvalidateRect(GetDlgItem(IDC_AC_CK_PLAY), NULL, true);
            if (lParam & UPDATE_PAUSE_BTN) InvalidateRect(GetDlgItem(IDC_AC_CK_PAUSE), NULL, true);
            if (lParam & UPDATE_STOP_BTN ) InvalidateRect(GetDlgItem(IDC_AC_CK_STOP), NULL, true);
            if (lParam & UPDATE_PREV_BTN ) InvalidateRect(GetDlgItem(IDC_AC_BTN_PREV), NULL, true);
            if (lParam & UPDATE_PREVC_BTN) InvalidateRect(GetDlgItem(IDC_AC_BTN_PREV_C), NULL, true);
            if (lParam & UPDATE_NEXT_BTN ) InvalidateRect(GetDlgItem(IDC_AC_BTN_NEXT), NULL, true);
            if (lParam & UPDATE_NEXTC_BTN) InvalidateRect(GetDlgItem(IDC_AC_BTN_NEXT_C), NULL, true);
            if (lParam & UPDATE_EJECT_BTN) InvalidateRect(GetDlgItem(IDC_AC_BTN_EJECT), NULL, true);
            if (lParam & UPDATE_DIRECT_BTN)
            {
               int nOldTrack = lParam & UPDATE_DIRECT_MASK;
               int nTrack    = m_CDPlayer.GetCurrentTrack();
               if ((nOldTrack>0) && (nOldTrack<=10))
                  InvalidateRect(GetDlgItem(nOldTrack+IDC_AC_BTN_TITLE1-1), NULL, true);
               if ((nTrack>0) && (nTrack<=10))
                  InvalidateRect(GetDlgItem(nTrack+IDC_AC_BTN_TITLE1-1), NULL, true);
            }
         }
         else
         {
            SendDlgItemMessage(IDC_AC_CK_STOP, BM_SETCHECK, (!bPlay) ? BST_CHECKED : BST_UNCHECKED, 0);
            SendDlgItemMessage(IDC_AC_CK_PLAY, BM_SETCHECK, (bPlay) ? BST_CHECKED : BST_UNCHECKED, 0);
            SendDlgItemMessage(IDC_AC_CK_PAUSE, BM_SETCHECK, (m_CDPlayer.m_DeviceStatus.Paused) ? BST_CHECKED : BST_UNCHECKED, 0);
            SendDlgItemMessage(IDC_AC_BTN_EJECT, BM_SETCHECK, (m_CDPlayer.m_DeviceStatus.Eject) ? BST_CHECKED : BST_UNCHECKED, 0);
            CheckDlgRadioBtn(IDC_AC_BTN_TITLE1, m_CDPlayer.GetCurrentTrack()-1);
         }
      } break;
      case START_POLLCDSTATUS: SetTimerSave(ID_POLL_CD_STATUS, 5000, NULL);      break;
      case STOP_POLLCDSTATUS:  KillTimerSave(ID_POLL_CD_STATUS);                 break;
      case START_POLLCDPOS:    SetTimerSave(ID_POLL_CD_POSITION, 1000, NULL);    break;
      case STOP_POLLCDPOS:     KillTimerSave(ID_POLL_CD_POSITION);               break;
      case SET_INFO_STRING:
      {
         char szText[_MAX_PATH];
         strcpy(szText, gm_szFormatCDTitle);
         strcat(szText, (LPCTSTR)lParam);
         SetWindowText(m_hWnd, szText);
      }break;
      case SET_FILE_FOLDER:
      {
         WIN32_FIND_DATA w32fd;
         char szText[_MAX_PATH];
         char szExt[_MAX_PATH] = "*.wav";
         char *pszExt = szExt;
         char *pszNew = NULL;
         int  nLen;
         DWORD dwRead;
         DWORD *pdwFlag = (DWORD*)lParam;
         HKEY  hKey = NULL;

         pdwFlag[0] = 0;
         strcpy(szText, m_CDPlayer.m_OpenParams.lpstrElementName);
         nLen = ConcatCharacter(szText, '\\', true);
         m_TitleDlg.SetInterpret(szText);
         m_TitleDlg.FreeTitles();
         m_TitleDlg.m_nTracks = 0;

         if (OpenRegistry(m_hInstance, KEY_READ, &hKey, REGKEY_FORMATS))
         {
            dwRead = _MAX_PATH;
            GetRegBinary(hKey, AUDIO_EXTENSION, (BYTE*)szExt, dwRead, true);
            EtsRegCloseKey(hKey);
         }
         m_CDPlayer.m_DeviceStatus.Reading = true;
         m_CDPlayer.UpdateCDDisplay(UPDATE_IMEDIATE);

         for (; pszExt!=NULL; pszExt = pszNew)
         {
            pszNew = strstr(pszExt, ",");
            if (pszNew)
            {
               pszNew[0] = 0;
               pszNew++;
            }
            szText[nLen] = 0;
            strncat(szText, pszExt, _MAX_PATH);
            m_CDPlayer.UpdateCDDisplay();

            HANDLE hFindFile = FindFirstFile(szText, &w32fd);
            if (hFindFile == INVALID_HANDLE_VALUE) continue;
            do
            {
               m_TitleDlg.AddTitle(w32fd.cFileName);
               m_TitleDlg.m_nTracks = m_TitleDlg.m_Titles.GetCounter();
               if (m_TitleDlg.m_nTracks == 0) break;
               else if (m_TitleDlg.m_nTracks == 1)
               {
                  szText[nLen] = 0;
                  strcat(szText, w32fd.cFileName);
                  m_CDPlayer.CloseDevice();
                  if (m_CDPlayer.OpenWaveDevice(szText))
                  {

                  }
                  else
                  {
                     m_TitleDlg.m_nTracks = -1;
                     break;
                  }
               }
            }
            while (FindNextFile(hFindFile, &w32fd));
            FindClose(hFindFile);
            if (m_TitleDlg.m_nTracks == -1) break;
         };
         m_CDPlayer.m_DeviceStatus.Reading = false;

         if (m_TitleDlg.m_nTracks <= 0)
         {
            m_CDPlayer.CloseDevice();
         }
         else
         {
            m_CDPlayer.m_mmCDTime.dwTime = 0;
            m_CDPlayer.m_mmCDTime.tmsfTime.Track = m_TitleDlg.m_nTracks;
            m_CDPlayer.m_dwWaveTrack = 1;
            SendMessage(m_hWnd, WM_UPDATE_CD_CTRLS, NEW_CD_MEDIA, 0);
            SendMessage(m_hWnd, WM_UPDATE_CD_CTRLS, SET_CURRENT_TRACK, 1);
            m_CDPlayer.UpdateCDCtrls();
            *pdwFlag = 1;
            return 1;
         }
      } break;
      case NEW_CD_MEDIA:
      {
         int i, nBtn;
         char text[8];
         DWORD nLen = 32;
         char szMediaIdent[32];
         strcpy(szMediaIdent, m_TitleDlg.m_szMediaIdent);
         if (m_CDPlayer.m_DeviceStatus.WaveFiles)
         {
            m_TitleDlg.m_bEditable = false;
            m_TitleDlg.m_szMediaIdent[0] = 0;
            m_CDPlayer.m_mmCDTime.dwTime = 0; // alles auf 0;
            m_CDPlayer.m_mmCDTime.tmsfTime.Track = m_TitleDlg.m_nTracks; // außer tracks
         }
         else if (m_CDPlayer.GetDeviceInfo(MCI_INFO_MEDIA_IDENTITY, m_TitleDlg.m_szMediaIdent, nLen))
         {
            m_TitleDlg.m_bEditable = true;
            strcat(m_TitleDlg.m_szMediaIdent, ".cdi");
            m_TitleDlg.FreeInterpret();
            m_TitleDlg.FreeTitles();
            if (!m_TitleDlg.ReadMediaInfo())
               m_TitleDlg.m_nTracks = m_CDPlayer.GetTracks();

            DWORD dwTrack;
            MMTime mmTX;
            mmTX.dwTime = 0;
            for (i=0; i<m_CDPlayer.GetTracks(); i++)
            {
               MMTime mmT;
               dwTrack = i+1;
               m_CDPlayer.GetDeviceStatus(MCI_STATUS_ITEM|MCI_TRACK, MCI_STATUS_LENGTH, mmT.dwTime, dwTrack);
               mmT.MSUT2TMSF();
               m_TitleDlg.SetTime(i, mmT);
            }
         }
         SendDlgItemMessage(IDC_AC_COMBO_CD_TITLE, CB_RESETCONTENT, 0, 0);
         nBtn = IDC_AC_BTN_TITLE1;
         for (i=1; i<=m_CDPlayer.GetTracks(); i++)
         {
            wsprintf(text, "%02d", i);
            SendDlgItemMessage(IDC_AC_COMBO_CD_TITLE, CB_ADDSTRING, 0, (LPARAM)text);
            nBtn = i+IDC_AC_BTN_TITLE1-1;
            if (nBtn <= IDC_AC_BTN_TITLE10)
               EnableWindow(GetDlgItem(nBtn), true);
         }
         for (nBtn++; nBtn<=IDC_AC_BTN_TITLE10; nBtn++)
         {
            EnableWindow(GetDlgItem(nBtn), false);
         }
         m_CDPlayer.UpdateCDDisplay();
      }break;
      case NEW_CD_TRACK:
      {
         if (m_CDPlayer.m_DeviceStatus.WaveFiles)              // bei Wave-Dateien muß ein neues Gerät geöffnet werden
         {
            int nTrack = lParam - 1;
            if ((m_hMixer != NULL) &&                          // Mixer vorhanden ?
                m_CDPlayer.m_DeviceStatus.CompareTitle)        // Titelvergleich
            {
               if (m_CDPlayer.m_dwWaveTrack != (DWORD)lParam)
                  m_CDPlayer.GetCurrentPosition();

               HWND hwndSlider = GetDlgItem(IDC_SLIDER_TITLE1+nTrack);
               if (hwndSlider)                                 // Lautstärke anpassen
               {
                  m_MixerDlg.SetWaveVolume(::SendMessage(hwndSlider, TBM_GETPOS, 0, 0));
               }
            }
            if (m_CDPlayer.m_dwWaveTrack != (DWORD)lParam)        // alt und neu ungleich ?
            {
               char szFile[_MAX_PATH];
               char *pszTitle     = (char*)m_TitleDlg.m_Titles.GetAt(nTrack);// Datei holen
               DWORD dwOldTime    = 0;
               DWORD dwOldTrack   = m_CDPlayer.m_dwWaveTrack;
               bool bCompareTitle = m_CDPlayer.m_DeviceStatus.CompareTitle;
               bool bPlayes       = m_CDPlayer.m_DeviceStatus.Playes;
               if (bCompareTitle) dwOldTime = m_CDPlayer.m_mmCurrentTrackTime.dwTime;
               m_CDPlayer.m_DeviceStatus.NoDisableOnClose = true;// Controls nicht disablen beim schließen (flackern)
               m_CDPlayer.CloseDevice();
               m_CDPlayer.m_DeviceStatus.NoDisableOnClose = false;// Controls disablen ermöglichen
               if (pszTitle)                                   // Wavedateiname vorhanden
               {
                  const char *pszInterpret = m_TitleDlg.GetInterpret();
                  if (pszInterpret == NULL) return 0;
                  if (pszInterpret[0] == '#') strcpy(szFile, pszTitle);
                  else
                  {
                     strcpy(szFile, pszInterpret);             // Pfad basteln
                     strcat(szFile, pszTitle);
                  }
                  if (m_CDPlayer.OpenWaveDevice(szFile))       // Gerät öffnen
                  {
                     m_CDPlayer.m_mmCurrentTrackTime.dwTime = dwOldTime;
                     m_CDPlayer.m_DeviceStatus.CompareTitle = bCompareTitle;
                     m_CDPlayer.m_dwWaveTrack = nTrack + 1;    // aktuellen Wavtrack speichern
                     m_CDPlayer.m_mmCDTime.dwTime = 0;         // alles auf 0;
                     m_CDPlayer.m_mmCDTime.tmsfTime.Track = m_TitleDlg.m_nTracks;// außer tracks
                     if (bPlayes)
                     {
                        DWORD dwStart, dwEnd;
                        dwStart = m_CDPlayer.m_mmCurrentTrackTime.GetWaveTime();
                        dwEnd   = m_CDPlayer.m_mmTrackTime.GetWaveTime();
                        m_CDPlayer.PlayDevice(dwStart, dwEnd);
                     }
                     m_CDPlayer.UpdateCDCtrls();
                     m_CDPlayer.UpdateCDCtrls(UPDATE_DIRECT_BTN|dwOldTrack);
                  }
               }
            }
         }
         else
         {
            if (m_CDPlayer.GetCurrentTrack() != lParam)
            {
               m_CDPlayer.ChangeCurrentTrack(lParam);
            }
         }
         SendMessage(m_hWnd, WM_UPDATE_CD_CTRLS, SET_CURRENT_TRACK, lParam);
      }break;
      case UPDATE_DISPLAY:
      {
         HWND hWnd = GetDlgItem(IDC_AC_CD_TIME);
         if (hWnd)
         {
            if (lParam&UPDATE_IMEDIATE)
            {
               ::SendMessage(hWnd, WM_DRAW_CD_DISPLAY, 0, 0);
            }
            else 
            {
               if (lParam&UPDATE_CURR_TIME)
               {
                  RECT rc;
                  ::GetClientRect(hWnd, &rc);
                  rc.left   = m_nDisplayDivX;
                  rc.top    = rc.bottom>>1;
                  rc.right  = rc.right - (m_nRoundRectRadius >> 1);
                  rc.bottom = rc.bottom-1;
                  ::InvalidateRect(hWnd, &rc, true);
               }
               else
               {
                  ::InvalidateRect(hWnd, NULL, false);
               }
            }
         }
      }break;
      case REPORT_ERROR:
      if (m_bUseMsgBoxForError)
      {
         char szCaption[_MAX_PATH];
         ::GetWindowText(m_hWnd, szCaption, _MAX_PATH);
         MessageBox(lParam, (long)szCaption, MBU_CAPTIONSTRING|MBU_TEXTSTRING|MB_OK|MB_ICONERROR);
         m_bUseMsgBoxForError = false;
      }
      else
      {
         if (m_CDPlayer.m_DeviceStatus.ErrorText) break;
         HWND hWnd = GetDlgItem(IDC_AC_CD_TIME);
         if (hWnd)
         {
            m_CDPlayer.m_DeviceStatus.ErrorText = true;
            if (lParam) strncpy(m_szCDWndText, (LPCTSTR)lParam, MAX_PATH);
            ::SendMessage(hWnd, WM_DRAW_CD_DISPLAY, 0, 0);
         }
         SetTimerSave(ID_WAIT_CD_ERROR_TIMER, 4000, NULL);
      } break;
      case SET_CURRENT_TRACK:
      {
         if (m_CDPlayer.m_DeviceStatus.WaveFiles)
            m_CDPlayer.m_dwWaveTrack = lParam;
         else
            m_CDPlayer.m_mmTrackTime.tmsfTime.Track = lParam;

         int nTrack    = lParam-1;
         int nOldTrack = SendDlgItemMessage(IDC_AC_COMBO_CD_TITLE, CB_GETCURSEL, 0, 0);
         if (nOldTrack != nTrack)
         {
            SendDlgItemMessage(IDC_AC_COMBO_CD_TITLE, CB_SETCURSEL, nTrack, 0);
            nOldTrack++;
            SendMessage(m_hWnd, WM_UPDATE_CD_CTRLS, UPDATE_CTRLS, UPDATE_DIRECT_BTN|nOldTrack);
         }
      }break;
      case GET_CURRENT_TRACK:
      {
         if (lParam == NULL) return 0;
         *((int*)lParam) = m_CDPlayer.GetCurrentTrack();
         return 0;
      }break;
      case GET_MM_FILTERS:
      {
         char *pszFilters = (char*) lParam;
         pszFilters[0] = 0;
         char *pszFilter = (char*)m_CDPlayer.m_Formats.m_Formats.GetFirst();
         while (pszFilter)
         {
            char *psz = strstr(pszFilter, ", ");
            if (psz)
            {
               psz[0] = 0;
               strcat(pszFilters, &psz[2]);
               strcat(pszFilters, "(");
               strcat(pszFilters, &pszFilter[1]);
               strcat(pszFilters, ")");
               strcat(pszFilters, "\n");
               strcat(pszFilters, &pszFilter[1]);
               strcat(pszFilters, "\n");
               psz[0] = ',';
            }
            pszFilter = (char*)m_CDPlayer.m_Formats.m_Formats.GetNext();
         }
         strcat(pszFilters, "\n");
      }break;
   }
   return 0;
}

BOOL CALLBACK CCDPLDlg::EnumChildWndFunc(HWND hwnd, LPARAM)
{
   long lValue;
   lValue = GetWindowLong(hwnd, GWL_ID);
   if ((lValue >= IDC_AC_BTN_TITLE1) && (lValue <= IDC_AC_BTN_TITLE10))
   {
      SetWindowContextHelpId(hwnd, IDC_AC_COMBO_CD_TITLE);
      if (!gm_bOwnerDraw)
      {
         ModifyStyle(hwnd, BS_RADIOBUTTON|BS_3STATE, BS_OWNERDRAW);
      }
   }
   else if ((lValue >= IDC_SLIDER_TITLE1) && (lValue <=IDC_SLIDER_TITLE10))
   {
      SetWindowContextHelpId(hwnd, IDC_SLIDER_TITLE1);
      ::SendMessage(hwnd, TBM_SETRANGEMIN, 0,   0);
      ::SendMessage(hwnd, TBM_SETRANGEMAX, 0, 100);
      ::SendMessage(hwnd, TBM_SETPOS, 1, 0);
   }
   else if (!gm_bOwnerDraw)
   {
      switch (lValue)
      {
         case IDC_AC_CK_PLAY: case IDC_AC_CK_STOP: case IDC_AC_CK_PAUSE: case IDC_AC_BTN_EJECT:
            ModifyStyle(hwnd, BS_AUTOCHECKBOX|BS_PUSHLIKE, BS_OWNERDRAW);
            break;
         case IDC_AC_BTN_PREV: case IDC_AC_BTN_PREV_C: case IDC_AC_BTN_NEXT_C: case IDC_AC_BTN_NEXT:
            ModifyStyle(hwnd, 0, BS_OWNERDRAW);
            break;
      }
   }
   lValue = GetWindowLong(hwnd, GWL_USERDATA);
   if (lValue == 0)
   {
      ::SetWindowLong(hwnd, GWL_USERDATA, ::SetWindowLong(hwnd, GWL_WNDPROC, (long)CCDPLDlg::SubClassProc));
   }
   return 1;
}

LRESULT CALLBACK CCDPLDlg::SubClassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   long lValue  = ::GetWindowLong(hwnd, GWL_USERDATA);
   long lIDctrl = ::GetWindowLong(hwnd, GWL_ID);
   ASSERT(lValue != 0);
   switch (uMsg)
   {
      case WM_ERASEBKGND:                                      // Zeichnen des CD Displays
      if (lIDctrl == IDC_AC_CD_TIME)
      {
         return false;
      }
      case WM_PAINT:                                           // Zeichnen des CD Displays
      case WM_DRAW_CD_DISPLAY:
      if (lIDctrl == IDC_AC_CD_TIME)
      {
         CCDPLDlg * pThis = (CCDPLDlg*)::GetWindowLong(::GetParent(hwnd), DWL_USER);
         if (pThis)
         {
            PAINTSTRUCT ps;
            DWORD    dwFlags = DT_CENTER;
            bool     bPaint = (uMsg == WM_DRAW_CD_DISPLAY) ? false : true;
            bool     bPartial = false;
            COLORREF cBkGnd = pThis->m_CDPlayer.m_dwCDDiplayBkGnd;        // Schwarzer Hintergrund
            COLORREF cText  = pThis->m_CDPlayer.m_dwCDDiplayText;         // 
            HBRUSH   hBrush = NULL;
            HRGN     hrgn = NULL, hRoRcRgn = NULL;
            HPEN     hPenShadow = NULL, hPenHilight = NULL;
            RECT     rcClient;
            ::GetClientRect(hwnd, &rcClient);
            if (GetColorDiff(cText, cBkGnd) < 30)
            {
               cBkGnd = ::GetSysColor(COLOR_3DFACE);
               cText  = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
            }

            hBrush  = ::CreateSolidBrush(cBkGnd);
            ::GetClientRect(hwnd, &rcClient);

            if (bPaint)
            {
               ::BeginPaint(hwnd, &ps);
               if (ps.rcPaint.left == pThis->m_nDisplayDivX)
                  bPartial = true;
            }
            else
            {
               ps.hdc = ::GetDC(hwnd);
               ps.rcPaint = rcClient;
            }
//            TRACE("Partial = %d\n", bPartial);
            ::SaveDC(ps.hdc);
            if (!bPartial)
            {

               hRoRcRgn = ::CreateRoundRectRgn(rcClient.left, rcClient.top, rcClient.right, rcClient.bottom, pThis->m_nRoundRectRadius, pThis->m_nRoundRectRadius);
               POINT pt[5] = {{rcClient.left , rcClient.top     },
                              {rcClient.right, rcClient.top     },
                              {rcClient.right, rcClient.top   +2}, 
                              {rcClient.left , rcClient.bottom-2},
                              {rcClient.left , rcClient.top     }};
               hrgn = ::CreatePolygonRgn(pt, 5, ALTERNATE);

               ::FillRgn(ps.hdc, hRoRcRgn, hBrush);

               hPenShadow  = ::CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_BTNSHADOW));
               hPenHilight = ::CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_BTNHILIGHT));

               ::SelectObject(ps.hdc, ::GetStockObject(HOLLOW_BRUSH));
               ::SelectObject(ps.hdc, hPenHilight);
               ::RoundRect(ps.hdc, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom, pThis->m_nRoundRectRadius, pThis->m_nRoundRectRadius);
               ::SelectObject(ps.hdc, hPenShadow);
               ::SelectClipRgn(ps.hdc, hrgn);
               ::RoundRect(ps.hdc, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom, pThis->m_nRoundRectRadius, pThis->m_nRoundRectRadius);

               ::SelectClipRgn(ps.hdc, hRoRcRgn);
            }
            ::SetBkColor(ps.hdc, cBkGnd);
            ::SetTextColor(ps.hdc, cText);

            if (!pThis->m_CDPlayer.m_DeviceStatus.ErrorText) ::SelectObject(ps.hdc, pThis->m_CDPlayer.m_hFontCDtime);

            if (pThis->m_CDPlayer.m_DeviceStatus.ErrorText)
            {
               dwFlags = DT_CENTER|DT_WORDBREAK;
            }
            else if (pThis->m_CDPlayer.m_DeviceStatus.Reading)
            {
               ::LoadString(pThis->m_hInstance, IDS_READ_CD_STATUS, pThis->m_szCDWndText, MAX_PATH);
            }
            else if (pThis->m_CDPlayer.m_DeviceStatus.MediaAudio)
            {                                                  // Audio CD
               RECT rc;
               int nx1 = pThis->m_nRoundRectRadius >> 1,
                   nx2 = pThis->m_nDisplayDivX,
                   nx3 = rcClient.right - (pThis->m_nRoundRectRadius >> 1),
                   ny1 = rcClient.top+1,
                   ny2 = rcClient.bottom>>1,
                   ny3 = rcClient.bottom-1;

               if (bPartial)
               {
                  hrgn = ::CreateRectRgn(ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right, ps.rcPaint.bottom);
                  ::SelectClipRgn(ps.hdc, hrgn);
                  ::FillRect(ps.hdc, &rcClient, hBrush);
               }

               dwFlags = DT_LEFT;
               MMTime mmTitle, mmTotal;
               if (pThis->m_CDPlayer.m_DeviceStatus.Playes || pThis->m_CDPlayer.m_DeviceStatus.Paused) 
               {                                               // aktuelle Spielzeit ausgeben
                  switch (pThis->m_CDPlayer.m_DeviceStatus.TimeOptions)
                  {                                            
                     case 0:                                   // verstrichene
                        mmTitle = pThis->m_CDPlayer.m_mmCurrentTrackTime;
                        break;
                     case 1:                                   // verbleibende
                        mmTitle.Sub(pThis->m_CDPlayer.m_mmTrackTime, pThis->m_CDPlayer.m_mmCurrentTrackTime);
                        break;
                     case 2:                                   // verstrichene Gesamtzeit
                     case 3:                                   // verbleibende Gesamtzeit
                     {
                        int i, nTrack = pThis->m_CDPlayer.GetCurrentTrack()-1;
                        for (i=0; i<nTrack; i++)
                        {
                           mmTitle.Add(mmTitle, pThis->m_TitleDlg.GetTime(i));
                        }
                        mmTitle.Add(mmTitle, pThis->m_CDPlayer.m_mmCurrentTrackTime);
                        if (pThis->m_CDPlayer.m_DeviceStatus.TimeOptions == 3)
                           mmTitle.Sub(pThis->m_CDPlayer.m_mmCDTime, mmTitle);
                     } break;
                  }
               }
               else                                            // oder gesamte Spielzeit des Titels ausgeben
               {
                  mmTitle = pThis->m_CDPlayer.m_mmTrackTime;
               }
               mmTitle.TMSF2HMS();
               mmTitle.hmsTime.Track = pThis->m_CDPlayer.GetCurrentTrack();
               ::SetRect(&rc, nx1, ny2, nx2, ny3);
               char *pszTitle = (char*)pThis->m_TitleDlg.m_Titles.GetAt(mmTitle.hmsTime.Track-1);
               if (pszTitle==NULL)                                        // Titel
               {
                  ::LoadString(pThis->m_hInstance, IDS_CD_TRACK, pThis->m_szCDWndText, MAX_PATH);
                  ::DrawText(ps.hdc, pThis->m_szCDWndText, strlen(pThis->m_szCDWndText), &rc, dwFlags);
               }
               else
               {
                  ::DrawText(ps.hdc, pszTitle, strlen(pszTitle), &rc, dwFlags);
               }

               dwFlags = DT_RIGHT;
               mmTitle.FormatTHMS(gm_szFormatCDTime, pThis->m_szCDWndText);
               SetRect(&rc, nx2, ny2, nx3, ny3);
               ::DrawText(ps.hdc, pThis->m_szCDWndText, strlen(pThis->m_szCDWndText), &rc, dwFlags);

               mmTotal = pThis->m_CDPlayer.m_mmCDTime;
               mmTotal.TMSF2HMS();
               mmTotal.FormatTHMS(gm_szFormatCDTime, pThis->m_szCDWndText);
               SetRect(&rc, nx2, ny1, nx3, ny2);
               ::DrawText(ps.hdc, pThis->m_szCDWndText, strlen(pThis->m_szCDWndText), &rc, dwFlags);

               dwFlags = DT_LEFT;
               SetRect(&rc, nx1, ny1, nx2, ny2);
               if (pThis->m_TitleDlg.GetInterpret() == NULL)
               {
                  ::LoadString(pThis->m_hInstance, IDS_CD_TOTAL, pThis->m_szCDWndText, MAX_PATH);
                  ::DrawText(ps.hdc, pThis->m_szCDWndText, strlen(pThis->m_szCDWndText), &rc, dwFlags);
               }
               else
               {
                  ::DrawText(ps.hdc, pThis->m_TitleDlg.GetInterpret(), strlen(pThis->m_TitleDlg.GetInterpret()), &rc, dwFlags);
               }
               pThis->m_szCDWndText[0] = 0;
            }
            else if (!pThis->m_CDPlayer.m_DeviceStatus.MediaPresent)
               ::LoadString(pThis->m_hInstance, IDS_NOCD, pThis->m_szCDWndText, MAX_PATH);
            else if (pThis->m_CDPlayer.m_DeviceStatus.MediaOther)
               ::LoadString(pThis->m_hInstance, IDS_CDOTHER, pThis->m_szCDWndText, MAX_PATH);

            if (pThis->m_szCDWndText[0])
            {
               rcClient.left  += pThis->m_nRoundRectRadius/2;
               rcClient.top++;
               rcClient.right -= pThis->m_nRoundRectRadius/2;
               rcClient.bottom--;
               ::DrawText(ps.hdc, pThis->m_szCDWndText, strlen(pThis->m_szCDWndText), &rcClient, dwFlags);
            }

            ::RestoreDC(ps.hdc, -1);
            if (bPaint) ::EndPaint(hwnd, &ps);
            else        ::ReleaseDC(hwnd, ps.hdc);

            if (hBrush)      ::DeleteObject(hBrush);
            if (hrgn)        ::DeleteObject(hrgn);
            if (hRoRcRgn)    ::DeleteObject(hRoRcRgn);
            if (hPenShadow)  ::DeleteObject(hPenShadow);
            if (hPenHilight) ::DeleteObject(hPenHilight);

            return 0;
         }
      }
      break;
      
      case WM_MOUSEMOVE:
      {
         if ((lIDctrl == IDC_AC_CD_TIME) && (wParam & MK_LBUTTON))
         {
            CCDPLDlg * pThis = (CCDPLDlg*)::GetWindowLong(::GetParent(hwnd), DWL_USER);
            POINT pt;
            POINTSTOPOINT(pt, lParam);
            if (pThis->m_bHitDisplayDivX)
            {
               pThis->m_nDisplayDivX = (short)pt.x;
               pThis->m_CDPlayer.UpdateCDDisplay();
            }
         }
/*
         DWORD dwHelpID  = ::GetWindowContextHelpId(hwnd);
         HWND hwndParent = ::GetParent(hwnd);                  // Help IDs der Ctrls zur Statusanzeige weiterleiten
         ::PostMessage(hwndParent, WM_TIMER, dwHelpID, 0);
         if (hwnd != gm_hWndActualHit)
         {
            TRACKMOUSEEVENT tme;
            tme.cbSize      = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags     = TME_LEAVE;
            tme.hwndTrack   = hwnd;
            tme.dwHoverTime = HOVER_DEFAULT;
            _TrackMouseEvent(&tme);
            gm_hWndActualHit = hwnd;
         }
*/
      } break;
      case WM_SETCURSOR:
      {
         if (lIDctrl == IDC_AC_CD_TIME)
         {
            CCDPLDlg * pThis = (CCDPLDlg*)::GetWindowLong(::GetParent(hwnd), DWL_USER);
            POINT pt;
            ::GetCursorPos(&pt);
            ::ScreenToClient(hwnd, &pt);
            if (pThis->m_bHitDisplayDivX || (abs(pt.x - pThis->m_nDisplayDivX) < 4))
            {
               if (HIWORD(lParam) == WM_LBUTTONDOWN) pThis->m_bHitDisplayDivX = true;
               lParam = MAKELONG(HTLEFT, HIWORD(lParam));
            }
            if (HIWORD(lParam) == WM_LBUTTONUP) pThis->m_bHitDisplayDivX = false;
         }
      } break;
/*
      case WM_MOUSELEAVE:
      {
         gm_hWndActualHit = ::GetParent(hwnd);
         TRACKMOUSEEVENT tme;
         tme.cbSize      = sizeof(TRACKMOUSEEVENT);
         tme.dwFlags     = TME_LEAVE;
         tme.hwndTrack   = gm_hWndActualHit;
         tme.dwHoverTime = HOVER_DEFAULT;
         _TrackMouseEvent(&tme);
      } break;
*/
      case WM_LBUTTONDOWN:                                        // CD-Steuerung
      if (lIDctrl == IDC_AC_BTN_NEXT_C)
      {
         CCDPLDlg * pThis = (CCDPLDlg*)::GetWindowLong(::GetParent(hwnd), DWL_USER);
         if (pThis) pThis->m_CDPlayer.PressNextBtn();
      }
      else if (lIDctrl == IDC_AC_BTN_PREV_C)
      {
         CCDPLDlg * pThis = (CCDPLDlg*)::GetWindowLong(::GetParent(hwnd), DWL_USER);
         if (pThis) pThis->m_CDPlayer.PressPrevBtn();
      }
      break;
      case WM_KILLFOCUS:
      case WM_SETFOCUS:
         if (gm_bOwnerDraw)
            ::InvalidateRect(hwnd, NULL, true);
         break;
      case WM_LBUTTONUP:                                          // CD-Steuerung
      {
         CCDPLDlg * pThis = (CCDPLDlg*)::GetWindowLong(::GetParent(hwnd), DWL_USER);
         if (pThis) pThis->m_CDPlayer.ReleasePrevNextBtns();
      }break;
      case WM_DESTROY:
         ::SetWindowLong(hwnd, GWL_WNDPROC, lValue);
//         if (lIDctrl == IDC_AC_BTN_EJECT) return 0;
      break;
   }

   return ::CallWindowProc((WNDPROC)lValue, hwnd, uMsg, wParam, lParam);
}


int CCDPLDlg::OnDrawItem(DRAWITEMSTRUCT *pDI)
{
   if ((pDI->CtlType == ODT_BUTTON)&& !(pDI->itemAction & ODA_FOCUS))
   {
      int nHeight  = pDI->rcItem.bottom - pDI->rcItem.top;
      RECT rcText  = pDI->rcItem;
      long lStyle = ::GetWindowLong(pDI->hwndItem, GWL_STYLE);

      pDI->rcItem.top++;

      ::SaveDC(pDI->hDC);
      HPEN hPenShadow  = ::CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_BTNSHADOW));
//      HPEN hPenShadowD = ::CreatePen(PS_DOT  , 1, ::GetSysColor(COLOR_BTNSHADOW));
      HPEN hPenHilight = ::CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_BTNHILIGHT));
      HPEN hPenBkGnd   = ::CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_3DFACE));
      POINT pt[5] = {{rcText.left , rcText.top     },
                     {rcText.right, rcText.top     },
                     {rcText.right, rcText.top   +2}, 
                     {rcText.left , rcText.bottom-2},
                     {rcText.left , rcText.top     }};
      HRGN hrgn = ::CreatePolygonRgn(pt, 5, ALTERNATE);
      switch(pDI->CtlID)                                       // Zustand des Buttons
      {
         case IDC_AC_CK_PLAY:
            if (m_CDPlayer.m_DeviceStatus.Playes) pDI->itemState |= ODS_SELECTED;
            break;
         case IDC_AC_CK_STOP:
            if (!m_CDPlayer.m_DeviceStatus.Paused && !m_CDPlayer.m_DeviceStatus.Playes) pDI->itemState |= ODS_SELECTED;
            break;
         case IDC_AC_CK_PAUSE:
            if (m_CDPlayer.m_DeviceStatus.Paused) pDI->itemState |= ODS_SELECTED;
            break;
         case IDC_AC_BTN_EJECT:
            if (!m_CDPlayer.m_DeviceStatus.Eject) pDI->itemState |= ODS_SELECTED;
            break;
         case IDC_AC_BTN_TITLE1: case IDC_AC_BTN_TITLE2: case IDC_AC_BTN_TITLE3: case IDC_AC_BTN_TITLE4: case IDC_AC_BTN_TITLE5: 
         case IDC_AC_BTN_TITLE6: case IDC_AC_BTN_TITLE7: case IDC_AC_BTN_TITLE8: case IDC_AC_BTN_TITLE9: case IDC_AC_BTN_TITLE10: 
         {
            if (pDI->CtlID - IDC_AC_BTN_TITLE1 == (UINT)m_CDPlayer.GetCurrentTrack()-1)  pDI->itemState |= ODS_SELECTED;
         }
      }

      ::SelectObject(pDI->hDC, hPenBkGnd);
      ::SelectObject(pDI->hDC, ::GetSysColorBrush(COLOR_3DFACE));
      ::Rectangle(pDI->hDC, pDI->rcItem.left, pDI->rcItem.top, pDI->rcItem.right, pDI->rcItem.bottom);

      if (lStyle & (BS_BITMAP|BS_ICON))                        // Zeichnen von Images
      {
         UINT nFlags = 0;
         int nWhat   = 0;
         if (lStyle & BS_ICON  ) nFlags |= DST_ICON  , nWhat = IMAGE_ICON;
         if (lStyle & BS_BITMAP) nFlags |= DST_BITMAP, nWhat = IMAGE_BITMAP;
         if (pDI->itemState & ODS_DISABLED) nFlags |= DSS_DISABLED;
         nFlags |= DT_CENTER;
         if (pDI->itemState & ODS_FOCUS) nFlags |= DSS_MONO;
//         BS_CENTER | BS_VCENTER   kann anders formatiert sein, hier nur center
         ::DrawState(pDI->hDC, ::GetSysColorBrush(COLOR_HIGHLIGHTTEXT), NULL, ::SendMessage(pDI->hwndItem, BM_GETIMAGE, nWhat, 0), 0, pDI->rcItem.left+10, pDI->rcItem.top+8, 0, 0, nFlags);
      }
      else                                                     // Zeichnen von Text
      {
         char szText[32];
         SIZE csText;
         ::GetWindowText(pDI->hwndItem, szText, 32);           // Text holen
         ::GetTextExtentPoint32(pDI->hDC, "8", 1, &csText);    // Höhe für das Zentrieren
         rcText.top += (nHeight -  csText.cy) / 2;             
         int nCol = COLOR_BTNTEXT;
         if (pDI->itemState & ODS_DISABLED) nCol = COLOR_GRAYTEXT;
         if (pDI->itemState & ODS_FOCUS)    nCol = COLOR_HIGHLIGHTTEXT;
         ::SetTextColor(pDI->hDC, ::GetSysColor(nCol));
         ::SetBkColor(pDI->hDC, ::GetSysColor(COLOR_3DFACE));    // Farben
//         BS_CENTER | BS_VCENTER   kann anders formatiert sein, hier nur center
         ::DrawText(pDI->hDC, szText, strlen(szText), &rcText, DT_CENTER);// Ausgabe
      }

      rcText  = pDI->rcItem;
      ::InflateRect(&rcText, -2, -2);
      ::SelectObject(pDI->hDC, ::GetStockObject(HOLLOW_BRUSH));

      if (pDI->itemState & ODS_SELECTED)                       // Button gedrückt
      {
         ::SelectObject(pDI->hDC, hPenHilight);
         ::RoundRect(pDI->hDC, pDI->rcItem.left, pDI->rcItem.top, pDI->rcItem.right, pDI->rcItem.bottom, m_nRoundRectRadius, m_nRoundRectRadius);
         ::RoundRect(pDI->hDC, rcText.left, rcText.top, rcText.right, rcText.bottom, m_nRoundRectRadius, m_nRoundRectRadius);
         ::SelectObject(pDI->hDC, hPenShadow);
         ::SelectClipRgn(pDI->hDC, hrgn);
         ::RoundRect(pDI->hDC, pDI->rcItem.left, pDI->rcItem.top, pDI->rcItem.right, pDI->rcItem.bottom, m_nRoundRectRadius, m_nRoundRectRadius);
//         if (pDI->itemState & ODS_FOCUS) ::SelectObject(pDI->hDC, hPenShadowD);
         ::RoundRect(pDI->hDC, rcText.left, rcText.top, rcText.right, rcText.bottom, m_nRoundRectRadius, m_nRoundRectRadius);
      }
      else                                                     // Button nicht gedrückt
      {
         ::SelectObject(pDI->hDC, ::GetStockObject(HOLLOW_BRUSH));
         ::SelectObject(pDI->hDC, hPenShadow);
         ::RoundRect(pDI->hDC, pDI->rcItem.left, pDI->rcItem.top, pDI->rcItem.right, pDI->rcItem.bottom, m_nRoundRectRadius, m_nRoundRectRadius);
//         if (pDI->itemState & ODS_FOCUS) ::SelectObject(pDI->hDC, hPenShadowD);
         ::RoundRect(pDI->hDC, rcText.left, rcText.top, rcText.right, rcText.bottom, m_nRoundRectRadius, m_nRoundRectRadius);
         ::SelectClipRgn(pDI->hDC, hrgn);
         ::SelectObject(pDI->hDC, hPenHilight);
         ::RoundRect(pDI->hDC, pDI->rcItem.left, pDI->rcItem.top, pDI->rcItem.right, pDI->rcItem.bottom, m_nRoundRectRadius, m_nRoundRectRadius);
         ::RoundRect(pDI->hDC, rcText.left, rcText.top, rcText.right, rcText.bottom, m_nRoundRectRadius, m_nRoundRectRadius);
      }

      ::RestoreDC(pDI->hDC, -1);
      ::DeleteObject(hrgn);
      ::DeleteObject(hPenShadow);
//      ::DeleteObject(hPenShadowD);
      ::DeleteObject(hPenHilight);
      ::DeleteObject(hPenBkGnd);
   }
   return 1;
}

void CCDPLDlg::AdaptWindowSize()
{
   RECT rc, rcW;
   ::GetWindowRect(m_hWnd, &rcW);

   if (m_MixerDlg.m_nMixerDev != INVALID_VALUE)
   {
      if (m_hMixer)
      {
         if (!m_CDPlayer.m_DeviceStatus.WaveFiles)
         {
            m_hMixer = NULL;
         }
      }
      else
      {
         if (m_CDPlayer.m_DeviceStatus.WaveFiles)
         {
            m_hMixer = m_MixerDlg.GetWaveMixer();
         }
      }
   }

   if ((m_hMixer != NULL) && m_CDPlayer.m_DeviceStatus.CompareTitle)
   {
      GetWindowRect(GetDlgItem(IDC_SLIDER_TITLE1), &rc);
      ScreenToClient(m_hWnd, &((POINT*)&rc)[1]);
      rc.bottom += 5;
      for (int i=IDC_SLIDER_TITLE1; i<IDC_SLIDER_TITLE10; i++)
         ::EnableWindow(GetDlgItem(i), true);
   }
   else
   {
      GetWindowRect(GetDlgItem(IDC_VOLUME_LINE), &rc);
      ScreenToClient(m_hWnd, &((POINT*)&rc)[1]);
      rc.bottom -= 2;
      for (int i=IDC_SLIDER_TITLE1; i<IDC_SLIDER_TITLE10; i++)
         ::EnableWindow(GetDlgItem(i), false);
   }
   rc.bottom += GetSystemMetrics(SM_CYDLGFRAME)+GetSystemMetrics(SM_CYBORDER)+GetSystemMetrics(SM_CYCAPTION);
   SetWindowPos(m_hWnd, NULL, 0, 0, rcW.right-rcW.left, rc.bottom, SWP_NOMOVE|SWP_NOOWNERZORDER);
}

void CCDPLDlg::SetEjectBtnImage()
{
   HANDLE hImageLoad, hImageCtrl = NULL;
   UINT uType = IMAGE_ICON;
   int  nIcon = 0;
   if (m_CDPlayer.m_DeviceStatus.CanEject != 0)
   {
      nIcon = IDR_BTN_EJECT;
   }
   else
   {
      nIcon = IDR_FILE_TYPECD + m_wAlternateEjectIcon;
   }
   if (m_wOldEjectIconID != nIcon)
   {
      hImageLoad = ::LoadImage(m_hInstance, MAKEINTRESOURCE(nIcon), uType, 32, 32, LR_DEFAULTCOLOR);
      if (hImageLoad)
      {
         hImageCtrl = (HANDLE) SendDlgItemMessage(IDC_AC_BTN_EJECT, BM_SETIMAGE, (WPARAM)uType, (LPARAM)hImageLoad);
      }
      InvalidateRect(GetDlgItem(IDC_AC_BTN_EJECT), NULL, true);
      m_wOldEjectIconID = (WORD)nIcon;
   }
}

void CCDPLDlg::OnOptionsCdDrive()
{
   unsigned long bCanEject  = m_CDPlayer.m_DeviceStatus.CanEject;
   unsigned long bWaveFiles = m_CDPlayer.m_DeviceStatus.WaveFiles;

   m_bUseMsgBoxForError = true;
   m_CDPlayer.SelectDrive();
   m_bUseMsgBoxForError = false;

   if (bWaveFiles != m_CDPlayer.m_DeviceStatus.WaveFiles)
   {
      if (m_CDPlayer.m_DeviceStatus.WaveFiles)
         m_MixerDlg.ModifyInitFlag(WAVE_LINES, 0);
      else
         m_MixerDlg.ModifyInitFlag(0, WAVE_LINES);
   }

   if (bCanEject != m_CDPlayer.m_DeviceStatus.CanEject)
   {
      if (m_CDPlayer.m_DeviceStatus.CanEject)
         m_MixerDlg.ModifyInitFlag(CD_LINES, 0);
      else
         m_MixerDlg.ModifyInitFlag(0, CD_LINES);

      SetEjectBtnImage();
      AdaptWindowSize();
   }
}

void CCDPLDlg::ModifyStyle(HWND hwnd, DWORD dwSet, DWORD dwRemove)
{
    long lStyle = ::GetWindowLong(hwnd, GWL_STYLE);
    lStyle &=~dwRemove;
    lStyle |=  dwSet;
    ::SetWindowLong(hwnd, GWL_STYLE, lStyle);
}

void CCDPLDlg::InitTitles()
{
   char szPath[_MAX_PATH];
   int i;
   m_TitleDlg.FreeTitles();
   for (i=0; i<LOWORD(m_pCDDlgStruct->nTitles); i++)
      m_TitleDlg.AddTitle(m_pCDDlgStruct->ppszTitles[i]);
   m_TitleDlg.m_nTracks = m_TitleDlg.m_Titles.GetCounter();
   ::LoadString(m_hInstance, IDS_TITLE_LIST_WO_PATH, szPath, _MAX_PATH);
   m_TitleDlg.SetInterpret(szPath);
}
