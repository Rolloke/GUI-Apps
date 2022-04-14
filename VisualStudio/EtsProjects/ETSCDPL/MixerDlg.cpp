/********************************************************************/
/* Funktionen der Klasse CMixerDlg                                  */
/********************************************************************/
#include "stdafx.h"
#include "MixerDlg.h"
#include "EtsRegistry.h"
#include "ETS3DGLRegKeys.h"
#include "resource.h"
#include "CCDPLDlg.h"
#include "CDPlayerCtrl.h"
#include "CEtsHelp.h"

#define  ETSDEBUG_INCLUDE 
#include "CEtsDebug.h"

#define GET_CONTROLS       2003

#define SET_SLIDERPOSITION true
#define SET_VOLUME         false


CMixerDlg::CMixerDlg()
{
   Constructor();
}

CMixerDlg::CMixerDlg(HINSTANCE hInstance, int nID, HWND hwndParent) : CEtsDialog(hInstance, nID, hwndParent)
{
   Constructor();
}

CMixerDlg::~CMixerDlg()
{
   CountMixerCtrInst(NULL, false);
   if (m_pWaveCtrl) DeleteMCL(m_pWaveCtrl);

   CloseMixerDev();
}

void CMixerDlg::Constructor()
{
   m_nMixerDev       = 0;
   m_hMixer          = NULL;
   m_hWaveMixer      = NULL;
   m_szWindowText[0] = 0;
   m_Controls.SetDeleteFunction(DeleteMCL);
   m_nForWhat        = 0;
   m_nUpdateControl  = CONTROL_UPDATE;
   m_pWaveCtrl       = NULL;
} // Constructor

bool CMixerDlg::OnInitDialog(HWND hWnd) 
{
   BEGIN("OnInitDialog");
   if (CEtsDialog::OnInitDialog(hWnd))
   {
      ::GetWindowText(hWnd, m_szWindowText, 64);
      if (!OpenMixerDev()) return false;

      return true;
   }
   return false;
} // OnInitDialog

void CMixerDlg::OnEndDialog(int nResult)
{
   if (m_hMixer)
   {
      mixerClose(m_hMixer);
      m_hMixer = NULL;
   }
   m_Controls.Destroy();
   CEtsDialog::OnEndDialog(nResult);                              // Funktion der Basisklasse aufrufen
} // OnEndDialog

int CMixerDlg::OnOk(WORD nNotifyCode)
{
   if (nNotifyCode == 0)
   {
      return IDOK;
   }
   return 0;
}

int CMixerDlg::OnCommand(WORD nID , WORD nNotifyCode, HWND hwndControl)
{
   if (nNotifyCode == BN_CLICKED)
   {
      m_nUpdateControl = CONTROL_NO_UPDATE_ONCE;
      HWND hwnd = GetDlgItem(nID);
      if (hwnd)
      {
         MMRESULT mmr;
         MixerControlLine *pml = (MixerControlLine*)::GetWindowLong(hwnd, GWL_USERDATA);
         if (pml)
         {
//            if (pml->mxctl.dwControlType == MIXERCONTROL_CONTROLTYPE_VOLUME)
            if ((pml->mxctl.dwControlType&MIXERCONTROL_CT_CLASS_MASK) == MIXERCONTROL_CT_CLASS_FADER)
            {
               pml->bSingle = (::SendMessage(hwnd, BM_GETCHECK, 0, 0) == BST_CHECKED) ? true : false;
               if (!pml->bSingle)
               {
                  if (pml->cd.unSigned[0].dwValue < pml->cd.unSigned[1].dwValue)
                  {
                     pml->cd.unSigned[0].dwValue = pml->cd.unSigned[1].dwValue;
                     VolumeControl(pml->hwndSliderRight, SET_VOLUME);
                  }
                  else
                  {
                     pml->cd.unSigned[1].dwValue = pml->cd.unSigned[0].dwValue;
                     VolumeControl(pml->hwndSliderLeft, SET_VOLUME);
                  }
               }
            }
            else if (pml->mxctl.dwControlType == MIXERCONTROL_CONTROLTYPE_MUTE)
            {
               pml->cd.boolean[0].fValue = (::SendMessage(hwnd, BM_GETCHECK, 0, 0) == BST_CHECKED) ? 0 : 1;
               mmr = mixerSetControlDetails((HMIXEROBJ)m_hMixer, &pml->mxcd, 0L);
               ReportMMError(mmr, false);
            }
            else
            {
               switch (pml->mxctl.dwControlType)
               {
                  case MIXERCONTROL_CONTROLTYPE_SINGLESELECT:
                  case MIXERCONTROL_CONTROLTYPE_MUX:
                  case MIXERCONTROL_CONTROLTYPE_MULTIPLESELECT:
                  case MIXERCONTROL_CONTROLTYPE_MIXER:
                  {
                     UINT cChannels      = pml->mxcd.cChannels;
                     UINT cMultipleItems = pml->mxcd.cMultipleItems;
                     MIXERCONTROLDETAILS_BOOLEAN *pBool = (MIXERCONTROLDETAILS_BOOLEAN*) pml->mxcd.paDetails;
                     for (UINT u = 0; u < cChannels; u++)
                     {
                        for (UINT v = 0; v < cMultipleItems; v++)
                        {
                           UINT uIndex = (u * cMultipleItems) + v;
                           MixerControlLine *pmlVolume = (MixerControlLine*) pml->cd.listext[uIndex].dwParam2;
                           if (pmlVolume == NULL) break;
                           if (pmlVolume->hwndMute == hwnd)
                           {
                              if (pml->bSingle) pBool[uIndex].fValue = 1;
                              else              pBool[uIndex].fValue = !pBool[uIndex].fValue;
                           }
                           else
                           {
                              if (pml->bSingle) pBool[uIndex].fValue = 0;
                           }
                           ::SendMessage(pmlVolume->hwndMute, BM_SETCHECK, (pBool[uIndex].fValue != 0) ? BST_CHECKED : BST_UNCHECKED, 0);
                        }
                     }
                     mmr = mixerSetControlDetails((HMIXEROBJ)m_hMixer, &pml->mxcd, 0L);
                     ReportMMError(mmr, false);
                     pml->mxcd.cChannels      = cChannels;
                     pml->mxcd.cMultipleItems = cMultipleItems;
                  } break;
               }
            }
         }
         else if ((GetWindowLong(hwnd, GWL_STYLE) & (BS_AUTOCHECKBOX|BS_PUSHLIKE)) == (BS_AUTOCHECKBOX|BS_PUSHLIKE))
         {
            ::SendMessage(hwnd, BM_SETCHECK, BST_UNCHECKED, 0);
         }
      }
   }

   return CEtsDialog::OnCommand(nID , nNotifyCode, hwndControl);
} // OnCommand

int CMixerDlg::OnMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
   switch(nMsg)
   {
      case MM_MIXM_LINE_CHANGE:
      if (wParam == (WPARAM)m_hMixer)
      {
      }break;
      case WM_SYSCOLORCHANGE:
      {
         MSG msg = {m_hWnd, nMsg, wParam, lParam, 0, {0,0}};
         ::EnumChildWindows(m_hWnd, EnumSliderSysColorChange, (LPARAM)&msg);
      } break;
      case WM_HELP:
      if (lParam)
      {
         HELPINFO *pHI = (HELPINFO*) lParam;
         MixerControlLine *pml = (MixerControlLine*)::GetWindowLong((HWND)pHI->hItemHandle, GWL_USERDATA);
         if ((pml != NULL) && (*(DWORD*)pml == sizeof(MIXERCONTROLDETAILS)) &&
             ((pHI->iContextType == HELPINFO_WINDOW) || (pHI->iContextType == HELPINFO_MENUITEM)))
         {
            if ((pHI->dwContextId == IDC_MIXER_LINE_FADER) || (pHI->dwContextId == IDC_MIXER_LINE_MUTE))
            {
               char format[128], text[512], szComponent[128];
               if (::LoadString(m_hInstance, pHI->dwContextId, format,  128))
               {
                  MIXERLINE         mxl;
                  mxl.fdwLine         = pml->dwLineFlags;
                  mxl.dwComponentType = pml->dwComponentType;
                  MixAppGetComponentName(&mxl, szComponent);
                  wsprintf(text, format, pml->mxctl.szShortName, pml->mxctl.szName, szComponent, pml->dwLineID, pml->mxctl.dwControlID);
                  CEtsHelp::CreateContextWnd(text, pHI->MousePos.x, pHI->MousePos.y);
                  return 1;
               }
            }
         }
         else if (pHI->dwContextId == IDS_MIXER_DEST0)
         {
            pHI->iCtrlId = IDS_MIXER_DEST0;
         }
      }break;
      case WM_INITMENUPOPUP:
      {
         HMENU hMenu = (HMENU) wParam;
         ::EnableMenuItem(hMenu, SC_SIZE, MF_BYCOMMAND|MF_GRAYED);
      } break;
      case MM_MIXM_CONTROL_CHANGE:
      if (m_nUpdateControl == CONTROL_UPDATE)
      {
         if (wParam == (WPARAM)m_hMixer)
         {
            void *pElement = NULL;
            int   nElement = m_Controls.FindElement(FindControlID, &lParam);
            if (nElement != -1)
            {
               pElement = m_Controls.GetAt(nElement);
               if (pElement)
               {
                  GetControlValues((MixerControlLine*)pElement);
               }
            }
            REPORT("Control Change ID:%d, Pos:%d, Ptr:%x", lParam, nElement, pElement);
         }
      }
      if (m_nUpdateControl == CONTROL_NO_UPDATE_ONCE)
      {
         m_nUpdateControl = CONTROL_UPDATE;
      } break;
      case WM_NOTIFY:
      {
         NMHDR *phdr = (NMHDR*) lParam;
         MixerControlLine *pml = (MixerControlLine*)::GetWindowLong(phdr->hwndFrom, GWL_USERDATA);
         if (phdr->code == TTN_GETDISPINFO)
         { 
            NMTTDISPINFO*pnmtdi = (NMTTDISPINFO*)lParam;
            wsprintf(pnmtdi->szText, "%d", 100-atoi(pnmtdi->szText));
         }
         else if ((pml != NULL) && 
             (*(DWORD*)pml == sizeof(MIXERCONTROLDETAILS)) && 
               ((pml->mxctl.dwControlType&MIXERCONTROL_CT_CLASS_MASK) == MIXERCONTROL_CT_CLASS_FADER))
//             (pml->mxctl.dwControlType == MIXERCONTROL_CONTROLTYPE_VOLUME))
         {
            if (phdr->code == NM_RELEASEDCAPTURE)
            {
               m_nUpdateControl = CONTROL_NO_UPDATE_ONCE;
               VolumeControl(phdr->hwndFrom, SET_VOLUME);
            }
            else if (phdr->code == NM_CUSTOMDRAW)
            {
               if (GetCapture() == phdr->hwndFrom) m_nUpdateControl = CONTROL_NO_UPDATE;
               else                                m_nUpdateControl = CONTROL_UPDATE;
               VolumeControl(phdr->hwndFrom, SET_VOLUME);
            }
         }
      } break;
   }
   return CEtsDialog::OnMessage(nMsg, wParam, lParam);
} // OnMessage

bool CMixerDlg::OpenMixerDev(HWND hwndParent, bool bRestore)
{
   BEGIN("OpenMixerDev");
   MMRESULT          mmr = 0;                                  // Fehlervariable
   MIXERCAPS         mxcaps;                                   // Eigenschaften des Mixers
   MIXERLINE         mxl;                                      // Eigenschaften einer Mixer Line
   MIXERLINECONTROLS mxlc;                                     // Eigenschaften eines Mixer Line Controls
   MixerControlLine  *pmclVolume = NULL;                       // Volume Control
   MixerControlLine  *pmclMute   = NULL;                       // Muting Control
   MixerControlLine  *pmclMeter  = NULL;                       // VU-Meter Control
   MixerControlLine  *pmclMux    = NULL;                       // Umschalter
   MixerControlLine  *pmclTemp   = NULL;                       // Temporär
   CEtsList           listSoundEqu;

   UINT        i, j, k;
   UINT        cConnections;
   int         nControl = 0, nCreated = 0, nDestinations = 0, nDefault;
   char        szComponentName[128];
   char        szWndText[128];
   char        szKeyName[MAX_PATH];
   DWORD       dwFlags     = 0,
               dwInfo, dwInitLines, dwValue, dwDestinationType;
   HKEY        hKey = NULL;
   BOOL        bInitValues = 0;;

   int    nBorderx = 0;
   RECT   rcSlider   = {0, 0, 0, 0};
   RECT   rcCKMute   = {0, 0, 0, 0};
   RECT   rcCKSingle = {0, 0, 0, 0};
   RECT   rcWindow   = {0, 0, 0, 0};
   SIZE   szMax      = {0, 0};
   bool   bFoundWaveSrc = false;
   char   szSliderClass[64] = "";
   char   szChkBtnClass[64] = "";
   char   szSingleText[64]  = "";
   HWND   hwndSliderL     = NULL,
          hwndSliderR     = NULL,
          hwndCKMute      = NULL,
          hwndCKSingle    = NULL,
          hwndTemp        = NULL;
   HFONT  hChkBtnFont     = NULL;
   DWORD  dwChkBtnStyle   = 0,
          dwChkBtnStyleEx = 0,
          dwSliderLStyle  = 0,
          dwSliderRStyle  = 0,
          dwSliderEx      = 0;

   if (m_hWnd && bRestore)
   {
      hwndTemp = m_hWnd;
      m_hWnd = NULL;
   }

   if (m_hWnd)
   {
      hwndSliderL    = GetDlgItem(IDC_MIXER_SLIDER_L);
      hwndSliderR    = GetDlgItem(IDC_MIXER_SLIDER_R);
      hwndCKMute     = GetDlgItem(IDC_MIXER_LINE_MUTE);
      hwndCKSingle   = GetDlgItem(IDC_MIXER_LINE_SINGLE);
      hChkBtnFont    = (HFONT) ::SendMessage(hwndCKMute, WM_GETFONT, 0, 0);
	   dwChkBtnStyle  = ::GetWindowLong(hwndCKMute, GWL_STYLE);;
	   dwChkBtnStyleEx= ::GetWindowLong(hwndCKMute, GWL_EXSTYLE);
	   dwSliderLStyle = ::GetWindowLong(hwndSliderL, GWL_STYLE);
	   dwSliderRStyle = ::GetWindowLong(hwndSliderR, GWL_STYLE);
	   dwSliderEx     = ::GetWindowLong(hwndSliderL, GWL_EXSTYLE);
      ::SetWindowContextHelpId(hwndCKMute, IDS_MIXER_DEST0);
                                                                  // Erzeugung von weiteren Controls
      ::GetClassName(hwndSliderL, szSliderClass, 64);             // Klassennamen
      ::GetClassName(hwndCKMute,  szChkBtnClass, 64);
      ::GetWindowRect(m_hWnd, &rcWindow);
      rcWindow.right  = rcWindow.right  - rcWindow.left;
      rcWindow.bottom = rcWindow.bottom - rcWindow.top;
      szMax.cx = 0;
      szMax.cy = rcWindow.bottom;
   }

   try
   {
      UINT nDev = mixerGetNumDevs();                           // Anzahl der Mixergeräte ermitteln
      if (nDev == 0) throw mmr;                                // Keine : Fehler
      UINT nOldDev = m_nMixerDev;
      if (OpenRegistry(m_hInstance, KEY_WRITE|KEY_READ, &hKey, MIXER)) // Anzahl speichern
      {
         SetRegDWord(hKey, MIXER_DEVICES, nDev);
         m_nMixerDev = GetRegDWord(hKey, MIXER_DEVICE, INVALID_VALUE);
         if (m_nMixerDev == INVALID_VALUE)
         {
            m_nMixerDev = 0;
            SetRegDWord(hKey, MIXER_DEVICE, m_nMixerDev);
         }
      }

      if (bRestore)                                            // Initalisierungsflags auswerten
      {
         dwInitLines = ModifyInitFlag();      // Decrementieren
         if (HIWORD(dwInitLines) == 0)
            bInitValues = RESTORE_LINES|LOWORD(dwInitLines);
         else throw (MMRESULT)0;
      }
      else if (m_hWnd == NULL)                                 // Incrementieren
      {
         dwInitLines = ModifyInitFlag();
         if (HIWORD(dwInitLines) == 1)
            bInitValues = STORE_LINES|LOWORD(dwInitLines);
         else bInitValues = LOWORD(dwInitLines);
      }
      else
      {
         bInitValues = LOWORD(ModifyInitFlag());
      }

      if (hKey) EtsRegCloseKey(hKey);

      if (m_nMixerDev >= nDev) m_nMixerDev = nDev-1;           // Mixernummer größer als die Anzahl der Geräte geht nicht

      mmr = mixerGetDevCaps(m_nMixerDev, &mxcaps, sizeof(mxcaps));
      if (MMSYSERR_NOERROR != mmr) throw mmr;                  // Mixereigenschaften

      if ((m_hMixer != NULL) && (nOldDev != m_nMixerDev))      // Mixer Ändern ?
      {
         CloseMixerDev();                                      // vorher schließen
      }

      if (m_hMixer == NULL)                                    // Öffnen des Mixers
      {
         if (hwndParent == NULL) hwndParent = m_hWnd;
         mmr = mixerOpen(&m_hMixer, m_nMixerDev, (DWORD)hwndParent, 0L, CALLBACK_WINDOW);
         if (MMSYSERR_NOERROR != mmr) throw mmr;               
      }

      wsprintf(szWndText, "%s\\M%02d", MIXER, m_nMixerDev);    // Mixer Nr.: als RegKey
      OpenRegistry(m_hInstance, KEY_WRITE, &hKey, szWndText);  // Name des Mixers als Defaultkey
      if (hKey) SetRegBinary(hKey, NULL, (BYTE*)mxcaps.szPname, 0, true);

      mmr = mixerGetDevCaps((UINT)m_hMixer, &mxcaps, sizeof(mxcaps));
      if (MMSYSERR_NOERROR != mmr) throw mmr;                  // Mixer Geräte ermitteln

      for (i=0; i < mxcaps.cDestinations; i++)                 // Alle Audio Lines ermitteln
      {
         mxl.cbStruct      = sizeof(mxl);
         mxl.dwDestination = i;                                // Info über die Line i
         mmr = mixerGetLineInfo((HMIXEROBJ)m_hMixer, &mxl, MIXER_GETLINEINFOF_DESTINATION);
         ReportMMError(mmr, false);
         dwDestinationType = mxl.dwComponentType;

         if (MMSYSERR_NOERROR != mmr) continue;                // Fehler ? weitersuchen
         if (m_nForWhat)                                       // eine bestimmte Line 
         {
            if (mxl.dwComponentType != m_nForWhat)
               continue;
         }
         MixAppGetComponentName(&mxl, szComponentName);        // Name der Audio Line
         REPORT("Destination(%d):%s, %s, %s", i+1, szComponentName, mxl.szName, mxl.szShortName);
         if (hKey) EtsRegCloseKey(hKey);
         wsprintf(szKeyName, "%s\\M%02d\\%s", MIXER, m_nMixerDev, mxl.szShortName);
         OpenRegistry(m_hInstance, KEY_READ|KEY_WRITE, &hKey, szKeyName);
         if (bInitValues & DEST_CTRL_FLAG)                     // in der Registry aktiviert
         {
            dwFlags = GetRegDWord(hKey, MIXER_CTRL_FLAG, INVALID_VALUE);
            if (dwFlags != INVALID_VALUE)
            {
               if (dwFlags & 0x00000001) bInitValues |=  DEST_ACTIVE;
               else                      bInitValues &= ~DEST_ACTIVE;
            }
         }

         if (m_hWnd)
         {
            ::GetWindowRect(hwndSliderL, &rcSlider);           // Größe und Position
            ::ScreenToClient(m_hWnd, &((POINT*)&rcSlider)[0]); // der zu erzeugenden Controls
            ::ScreenToClient(m_hWnd, &((POINT*)&rcSlider)[1]); 
            ::GetWindowRect(hwndCKMute,      &rcCKMute);
            ::ScreenToClient(m_hWnd, &((POINT*)&rcCKMute)[0]);
            ::ScreenToClient(m_hWnd, &((POINT*)&rcCKMute)[1]);
            ::GetWindowText(hwndCKSingle, szSingleText, 64);
            ::GetWindowRect(hwndCKSingle,      &rcCKSingle);
            ::ScreenToClient(m_hWnd, &((POINT*)&rcCKSingle)[0]);
            ::ScreenToClient(m_hWnd, &((POINT*)&rcCKSingle)[1]);
            nBorderx = rcCKSingle.left;
            nCreated = 0;
            if (nDestinations==0)                              // Anzahl der schon erzeugten Reihen == 0
            {                                                  // in der ersten stehen schon ein Controls
               ::OffsetRect(&rcSlider, (rcSlider.right - rcSlider.left), 0);
               strcpy(szWndText, m_szWindowText);              // Dialogfeldüberschrift setzen
               strncat(szWndText, (LPSTR)mxcaps.szPname, 128); // Mixername
               strncat(szWndText, ", ", 128);                  // und
               strncat(szWndText, szComponentName, 128);       // Destinationname
               ::SetWindowText(m_hWnd, szWndText);             // als Windowtext
            }
            else
            {                                                  // sonst vorne anfangen
               ::OffsetRect(&rcSlider  , (rcSlider.left   - rcSlider.right  ), rcWindow.bottom*nDestinations);
               ::OffsetRect(&rcCKMute  , (rcCKMute.left   - rcCKMute.right  ), rcWindow.bottom*nDestinations);
               ::OffsetRect(&rcCKSingle, (rcCKSingle.left - rcCKSingle.right), rcWindow.bottom*nDestinations);
               HWND hwnd = CreateWindow("Static", szComponentName, WS_VISIBLE|WS_CHILD, 
                      5, rcCKMute.top-22, rcWindow.right, 12,
                      m_hWnd, NULL,, m_hInstance, NULL);
               ::SendMessage(hwnd, WM_SETFONT, (WPARAM)hChkBtnFont, 0);
            }
         }
         dwInfo = MIXER_GETLINEINFOF_SOURCE;
//         dwInfo = MIXER_GETLINEINFOF_COMPONENTTYPE;
//         dwInfo = MIXER_GETLINEINFOF_DESTINATION;
//         dwInfo = MIXER_GETLINEINFOF_LINEID;
//         dwInfo = MIXER_GETLINEINFOF_TARGETTYPE;

         cConnections = (UINT)mxl.cConnections;
         for (j=0; j <= cConnections; j++)                     // Alle Anschlüsse der Audio Line prüfen
         {
            if (j > 0)                                         // Basisanschluß : j = 0;
            {                                                  // Alle weiteren : j > 0 <= cConnections
               mxl.cbStruct      = sizeof(mxl);
               mxl.dwDestination = i;
               mxl.dwSource      = j-1;
               mmr = mixerGetLineInfo((HMIXEROBJ)m_hMixer, &mxl, dwInfo);
               ReportMMError(mmr, false);
               if (MMSYSERR_NOERROR != mmr) continue;
               MixAppGetComponentName(&mxl, szWndText);
               REPORT("   Connection(%d):%s, %s, %s", j, szWndText, mxl.szName, mxl.szShortName);
            }
            if (mxl.cControls)                                 // Controls vorhanden
            {
               mxlc.cbStruct    = sizeof(mxlc);
               mxlc.dwLineID    = mxl.dwLineID;
               mxlc.dwControlID = 0;
               mxlc.cControls   = mxl.cControls;               // alle Controls ermitteln
               mxlc.cbmxctrl    = sizeof(MIXERCONTROL);
               mxlc.pamxctrl    = new MIXERCONTROL[mxl.cControls];

               mmr = mixerGetLineControls((HMIXEROBJ)m_hMixer, &mxlc, MIXER_GETLINECONTROLSF_ALL);
               ReportMMError(mmr, false);
               if (MMSYSERR_NOERROR == mmr)                    // OK
               {      
                  int nlen = strlen(szKeyName);
                  strcat(szKeyName, "\\");
                  strcat(szKeyName, mxl.szShortName);
                  EtsRegCloseKey(hKey);
                  OpenRegistry(m_hInstance, KEY_READ|KEY_WRITE, &hKey, szKeyName);
                  szKeyName[nlen] = 0;
                  nDefault = 0;//IF_CONTROL_ACTIVE;            // Standardmäßig nur Öffnen, wenn aktiv
                  if (hKey)                                    // RegKey offen
                  {
                     dwFlags = GetRegDWord(hKey, MIXER_CTRL_FLAG, INVALID_VALUE);
                     if (dwFlags == INVALID_VALUE)                // Ist der Key noch nicht da
                     {
                        dwFlags = 0;
                        SetRegDWord(hKey, MIXER_CTRL_FLAG, dwFlags);
                     }
                     if (dwDestinationType == MIXERLINE_COMPONENTTYPE_DST_SPEAKERS)
                     {
                        if (bInitValues & RESTORE_LINES)
                        {
                           dwFlags = VOLUME_ACTIVE|MUTE_ACTIVE;
                        }
                        else if (bInitValues & INIT_LINES)
                        {
                           if ((bInitValues & WAVE_LINES) &&   // Wave
                               (bInitValues & CD_LINES))       // und CD
                           {
                              switch (mxl.dwComponentType)     // Komponenten setzen
                              {
                                 case MIXERLINE_COMPONENTTYPE_DST_SPEAKERS:// Mastervolume
                                    nDefault = VOLUME_ACTIVE|MUTE_ACTIVE|SET_VOLUME_FULL;
                                    break;
                                 case MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT: // Wave
                                    if (!bFoundWaveSrc)
                                    {
                                       bFoundWaveSrc = true;
                                       nDefault = VOLUME_ACTIVE|MUTE_ACTIVE|SET_VOLUME_FULL;
                                    }
                                    else
                                    {
                                       nDefault = SET_MUTED;
                                    }break;
                                 case MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC:// CD
                                 case MIXERLINE_COMPONENTTYPE_SRC_AUXILIARY:
                                    nDefault = VOLUME_ACTIVE|MUTE_ACTIVE;
                                    break;
                                 default:
                                    nDefault = SET_MUTED;
                                    break;
                              }
                              nDefault |= SET_EQU_LINEAR;
                           }
                           else if (bInitValues & WAVE_LINES)  // nur Wave
                           {
                              switch (mxl.dwComponentType)     // Komponenten setzen
                              {
                                 case MIXERLINE_COMPONENTTYPE_DST_SPEAKERS:// Mastervolume
                                    nDefault = VOLUME_ACTIVE|MUTE_ACTIVE|SET_VOLUME_FULL;
                                    break;
                                 case MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT: // Wave
                                    if (!bFoundWaveSrc)
                                    {
                                       bFoundWaveSrc = true;
                                       nDefault = VOLUME_ACTIVE|MUTE_ACTIVE|SET_VOLUME_FULL;
                                    }
                                    else
                                    {
                                       nDefault = SET_MUTED;
                                    }break;
                                 default:
                                    nDefault = SET_MUTED;
                                    break;
                              }
                              nDefault |= SET_EQU_LINEAR;
                           }
                           else if (bInitValues & CD_LINES)    // nur CD
                           {
                              switch (mxl.dwComponentType)     // Komponenten setzen
                              {
                                 case MIXERLINE_COMPONENTTYPE_DST_SPEAKERS:// Mastervolume
                                 case MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC:// CD
                                 case MIXERLINE_COMPONENTTYPE_SRC_AUXILIARY:
                                    nDefault = VOLUME_ACTIVE|MUTE_ACTIVE;
                                    break;
                                 default:
                                    nDefault = SET_MUTED;
                                    break;
                              }
                              nDefault |= SET_EQU_LINEAR;
                           }
                           if (m_hWnd) nDefault &= ~(SET_MUTED|SET_VOLUME_FULL|SET_EQU_LINEAR);
                           dwFlags |= nDefault;
                        }
                        else
                        {
                           if ((bInitValues & WAVE_LINES) &&   // Wave
                               (bInitValues & CD_LINES))       // und CD
                           {
                              switch (mxl.dwComponentType)     // Komponenten setzen
                              {
                                 case MIXERLINE_COMPONENTTYPE_DST_SPEAKERS:// Mastervolume
                                 case MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT: // Wave
                                 case MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC:// CD
                                 case MIXERLINE_COMPONENTTYPE_SRC_AUXILIARY:
                                    nDefault = VOLUME_ACTIVE|MUTE_ACTIVE;
                                    break;
                                 default:
                                    break;
                              }
                           }
                           else if (bInitValues & WAVE_LINES)  // nur Wave
                           {
                              switch (mxl.dwComponentType)     // Komponenten setzen
                              {
                                 case MIXERLINE_COMPONENTTYPE_DST_SPEAKERS:// Mastervolume
                                 case MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT: // Wave
                                    nDefault = VOLUME_ACTIVE|MUTE_ACTIVE;
                                    break;
                              }
                           }
                           else if (bInitValues & CD_LINES)    // nur CD
                           {
                              switch (mxl.dwComponentType)     // Komponenten setzen
                              {
                                 case MIXERLINE_COMPONENTTYPE_DST_SPEAKERS:// Mastervolume
                                 case MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC:// CD
                                 case MIXERLINE_COMPONENTTYPE_SRC_AUXILIARY:
                                    nDefault = VOLUME_ACTIVE|MUTE_ACTIVE;
                                    break;
                              }
                           }
                           if (m_hWnd) nDefault &= ~(SET_MUTED|SET_VOLUME_FULL|SET_EQU_LINEAR);
                           dwFlags |= nDefault;
                        }
                     }
                     else if ((m_nForWhat == MIXERLINE_COMPONENTTYPE_DST_WAVEIN) && (dwDestinationType == m_nForWhat))
                     {
                        dwFlags = IF_CONTROL_ACTIVE|VOLUME_ACTIVE|MUX_ACTIVE;
                     }
                  }
                  for (k=0; k<mxl.cControls; k++)              // Controls prüfen
                  {
                     REPORT("Controls(%d):%s, %s", k, mxlc.pamxctrl[k].szName, mxlc.pamxctrl[k].szShortName);
                     if (dwFlags == 0) break;          // Nichts aktiv 
                     switch (mxlc.pamxctrl[k].dwControlType)
                     {
                        case MIXERCONTROL_CONTROLTYPE_VOLUME:  // Volume Control
                        if (bInitValues || (LOWORD(dwFlags) & VOLUME_ACTIVE)) // wenn es aktiv sein soll
                        {
                           ASSERT(pmclVolume == NULL);
                           pmclVolume = (MixerControlLine*)CreateMCL(mxlc.pamxctrl[k], mxl.cChannels*sizeof(MIXERCONTROLDETAILS_UNSIGNED));
                           pmclVolume->mxcd.cChannels      = mxl.cChannels;
                           pmclVolume->mxcd.cMultipleItems = 0;
                           pmclVolume->mxcd.cbDetails      = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
                           pmclVolume->mxcd.paDetails      = &pmclVolume->cd.unSigned[0];
                           if (bInitValues & INIT_LINES)
                           {
                              MMRESULT mmr = mixerGetControlDetails((HMIXEROBJ)m_hMixer, &pmclVolume->mxcd, 0L);
                              if (mmr == 0)
                              {
                                 if (bInitValues & STORE_LINES)
                                 {
                                    if (pmclVolume->mxcd.cChannels>1)
                                       dwValue = MAKELONG(LOWORD(pmclVolume->cd.unSigned[0].dwValue), LOWORD(pmclVolume->cd.unSigned[1].dwValue));
                                    else
                                       dwValue = LOWORD(pmclVolume->cd.unSigned[0].dwValue);
                                    SetRegDWord(hKey, CONTROL_VOLUME, dwValue);
                                    REPORT("Save %s %s : %x", mxl.szName, CONTROL_VOLUME, dwValue);
                                 }
                              }

                              if ((dwFlags & SET_VOLUME_FULL) && !(bInitValues & RESTORE_LINES))
                              {
                                 DWORD dwVolume = 0x0000ffff;
                                 pmclVolume->cd.unSigned[0].dwValue = dwVolume;
                                 if (pmclVolume->mxcd.cChannels>1)
                                    pmclVolume->cd.unSigned[1].dwValue = dwVolume;
                                 mixerSetControlDetails((HMIXEROBJ)m_hMixer, &pmclVolume->mxcd, 0L);
                                 REPORT("Set full Volume for %s", mxl.szName);
                              }
                           }
                           if (bInitValues & RESTORE_LINES)
                           {
                              dwValue = GetRegDWord(hKey, CONTROL_VOLUME, 0);
                              pmclVolume->cd.unSigned[0].dwValue = LOWORD(dwValue);
                              if (pmclVolume->mxcd.cChannels>1)
                                 pmclVolume->cd.unSigned[1].dwValue = HIWORD(dwValue);
                              mixerSetControlDetails((HMIXEROBJ)m_hMixer, &pmclVolume->mxcd, 0L);
                              REPORT("Restore %s %s : %x", mxl.szName, CONTROL_VOLUME, dwValue);
                           }

                           if ((m_hWnd == NULL) &&             // Beim Initialisieren ohne Fenster
                               (dwDestinationType == MIXERLINE_COMPONENTTYPE_DST_SPEAKERS) &&
                               (mxl.dwComponentType == MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT) &&
                               (m_pWaveCtrl == NULL))          // Den WaveControl belegen
                           {
                              mixerSetControlDetails((HMIXEROBJ)m_hMixer, &pmclVolume->mxcd, 0L);
                              m_pWaveCtrl = pmclVolume;
                              m_Controls.Remove(pmclVolume);
                              pmclVolume = NULL;
                           }
                        }
                        else
                        {
                           dwValue = 0;
                           SetRegDWord(hKey, mxlc.pamxctrl[k].szShortName, dwValue);
                        }break;
                        case MIXERCONTROL_CONTROLTYPE_MUTE:    // Muting Control
                        if (bInitValues || ((LOWORD(dwFlags) & MUTE_ACTIVE)))   // oder wenn es aktiv sein soll
                        {
                           ASSERT(pmclMute == NULL);
                           pmclMute = (MixerControlLine*)CreateMCL(mxlc.pamxctrl[k], sizeof(MIXERCONTROLDETAILS_BOOLEAN));
                           pmclMute->mxcd.cChannels      = 1;
                           pmclMute->mxcd.cMultipleItems = 0;
                           pmclMute->mxcd.cbDetails      = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
                           pmclMute->mxcd.paDetails      = pmclMute->cd.boolean;
                           pmclMute->dwLineFlags         = mxl.fdwLine;
                           pmclMute->dwLineID            = mxl.dwLineID;
                           pmclMute->dwComponentType     = mxl.dwComponentType;
                           mmr = mixerGetControlDetails((HMIXEROBJ)m_hMixer, &pmclMute->mxcd, 0L);
                           ReportMMError(mmr, false);
                           if (bInitValues & INIT_LINES)
                           {
                              if (bInitValues & STORE_LINES)
                              {
                                 SetRegDWord(hKey, CONTROL_MUTE, pmclMute->cd.boolean[0].fValue);
                                 REPORT("Save %s of %s : %x", CONTROL_MUTE, mxl.szName, pmclMute->cd.boolean[0].fValue);
                              }
                              if (!(bInitValues & RESTORE_LINES))
                              {
                                 pmclMute->cd.boolean[0].fValue = (dwFlags & SET_MUTED) ? 1 : 0;
                                 mmr = mixerSetControlDetails((HMIXEROBJ)m_hMixer, &pmclMute->mxcd, 0L);
                                 ReportMMError(mmr, false);
                                 REPORT("Set %s of %s = %d", CONTROL_MUTE, mxl.szName, pmclMute->cd.boolean[0].fValue);
                              }
                           }
                           if (bInitValues & RESTORE_LINES)
                           {
                              pmclMute->cd.boolean[0].fValue = GetRegDWord(hKey, CONTROL_MUTE, 0);
                              mmr = mixerSetControlDetails((HMIXEROBJ)m_hMixer, &pmclMute->mxcd, 0L);
                              ReportMMError(mmr, false);
                              REPORT("Restore %s %s : %d", mxl.szName, CONTROL_MUTE, pmclMute->cd.boolean[0].fValue);
                           }
                        }
                        else
                        {
                           dwValue = 0;
                           SetRegDWord(hKey, mxlc.pamxctrl[k].szShortName, dwValue);
                        }break;
                        case MIXERCONTROL_CONTROLTYPE_SINGLESELECT:
                        case MIXERCONTROL_CONTROLTYPE_MULTIPLESELECT:
                        case MIXERCONTROL_CONTROLTYPE_MUX:
                        case MIXERCONTROL_CONTROLTYPE_MIXER:
                        if ((j == 0) &&                        // Dieser Mux ist nur für den Basisanschluß !
                            //((mxl.fdwLine & MIXERLINE_LINEF_ACTIVE)||// Wenn aktiv
                             (LOWORD(dwFlags) & MUX_ACTIVE))   // oder wenn es aktiv sein soll
                        {
                           UINT fSingleSelect = (MIXERCONTROL_CT_SC_LIST_SINGLE == (MIXERCONTROL_CT_SUBCLASS_MASK & mxlc.pamxctrl[k].dwControlType));
                           UINT cChannels = (UINT)mxl.cChannels;
                           if (MIXERCONTROL_CONTROLF_UNIFORM &  mxlc.pamxctrl[k].fdwControl)
                              cChannels = 1;

                           UINT cMultipleItems = 1;
                           if (MIXERCONTROL_CONTROLF_MULTIPLE &  mxlc.pamxctrl[k].fdwControl)
                             cMultipleItems = (UINT) mxlc.pamxctrl[k].cMultipleItems;

                           pmclMux = (MixerControlLine*)CreateMCL(mxlc.pamxctrl[k], cChannels * cMultipleItems * sizeof(MIXERCONTROLDETAILS_LISTTEXT));
                           pmclMux->bSingle = (fSingleSelect != 0) ? true : false;

                           pmclMux->mxcd.cbStruct       = sizeof(MIXERCONTROLDETAILS);
                           pmclMux->mxcd.dwControlID    = mxlc.pamxctrl[k].dwControlID;
                           pmclMux->mxcd.cChannels      = cChannels;
                           pmclMux->mxcd.cMultipleItems = cMultipleItems;
                           pmclMux->mxcd.cbDetails      = sizeof(MIXERCONTROLDETAILS_LISTTEXT);
                           pmclMux->mxcd.paDetails      = pmclMux->cd.listext;

                           mmr = mixerGetControlDetails((HMIXEROBJ)m_hMixer, &pmclMux->mxcd, MIXER_GETCONTROLDETAILSF_LISTTEXT);
                           ReportMMError(mmr, false);
                           pmclMux->mxcd.cChannels      = cChannels;
                           pmclMux->mxcd.cMultipleItems = cMultipleItems;
                           pmclMux->mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
                           pmclMux->mxcd.paDetails = new MIXERCONTROLDETAILS_BOOLEAN[cChannels*cMultipleItems];
                        } break;
                        case MIXERCONTROL_CONTROLTYPE_EQUALIZER:
                        case MIXERCONTROL_CONTROLTYPE_BASS:
                        case MIXERCONTROL_CONTROLTYPE_TREBLE:
                        if (bInitValues || (LOWORD(dwFlags) & SET_EQU_LINEAR)) // wenn es aktiv sein soll
                        {
                           char szRegName[64];
                           pmclTemp = (MixerControlLine*)CreateMCL(mxlc.pamxctrl[k], mxl.cChannels*sizeof(MIXERCONTROLDETAILS_UNSIGNED));
                           wsprintf(szRegName, CONTROL_SOUND, pmclTemp->mxctl.szShortName);
                           listSoundEqu.ADDHead(pmclTemp);
                           pmclTemp->mxcd.cChannels      = mxl.cChannels;
                           pmclTemp->mxcd.cMultipleItems = 0;
                           pmclTemp->mxcd.cbDetails      = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
                           pmclTemp->mxcd.paDetails      = &pmclTemp->cd.unSigned[0];
                           pmclTemp->dwLineFlags         = mxl.fdwLine;
                           pmclTemp->dwLineID            = mxl.dwLineID;
                           pmclTemp->dwComponentType     = mxl.dwComponentType;
                           if (bInitValues & INIT_LINES)
                           {
                              MMRESULT mmr = mixerGetControlDetails((HMIXEROBJ)m_hMixer, &pmclTemp->mxcd, 0L);
                              REPORT("%s : %d", pmclTemp->mxctl.szName, mmr);
                              if (mmr == 0)
                              {
                                 if (bInitValues & STORE_LINES)
                                 {
                                    if (pmclTemp->mxcd.cChannels>1)
                                       dwValue = MAKELONG(LOWORD(pmclTemp->cd.unSigned[0].dwValue), LOWORD(pmclTemp->cd.unSigned[1].dwValue));
                                    else
                                       dwValue = LOWORD(pmclTemp->cd.unSigned[0].dwValue);
                                    SetRegDWord(hKey, szRegName, dwValue);
                                    REPORT("Save %s : %x", szRegName, dwValue);
                                 }
                              }
                              if ((dwFlags & SET_EQU_LINEAR) && !(bInitValues & RESTORE_LINES))
                              {
                                 DWORD dwVolume = 0x00007fff;
                                 pmclTemp->cd.unSigned[0].dwValue = dwVolume;
                                 if (pmclTemp->mxcd.cChannels>1)
                                    pmclTemp->cd.unSigned[1].dwValue = dwVolume;
                                 mixerSetControlDetails((HMIXEROBJ)m_hMixer, &pmclTemp->mxcd, 0L);
                                 REPORT("Set %s linear : %x", szRegName, dwVolume);
                              }
                           }
                           if (bInitValues & RESTORE_LINES)
                           {
                              dwValue = GetRegDWord(hKey, szRegName, 0);
                              pmclTemp->cd.unSigned[0].dwValue = LOWORD(dwValue);
                              if (pmclTemp->mxcd.cChannels>1)
                                 pmclTemp->cd.unSigned[1].dwValue = HIWORD(dwValue);
                              mixerSetControlDetails((HMIXEROBJ)m_hMixer, &pmclTemp->mxcd, 0L);
                              REPORT("Restore %s : %x", szRegName, dwValue);
                           }
                        } break;
                        case MIXERCONTROL_CONTROLTYPE_ONOFF:                  // ?
                        case MIXERCONTROL_CONTROLTYPE_MONO:                   // wird ausgeschaltet
                        case MIXERCONTROL_CONTROLTYPE_LOUDNESS:               // wird ausgeschaltet
                        case MIXERCONTROL_CONTROLTYPE_STEREOENH:              // wird ausgeschaltet
                        if (bInitValues || (LOWORD(dwFlags) & SET_EQU_LINEAR))// wenn es linear sein soll
                        {
                           pmclTemp = (MixerControlLine*)CreateMCL(mxlc.pamxctrl[k], sizeof(MIXERCONTROLDETAILS_BOOLEAN));
                           pmclTemp->mxcd.cChannels      = 1;
                           pmclTemp->mxcd.cMultipleItems = 0;
                           pmclTemp->mxcd.cbDetails      = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
                           pmclTemp->mxcd.paDetails      = pmclTemp->cd.boolean;
                           pmclTemp->dwLineFlags         = mxl.fdwLine;
                           pmclTemp->dwLineID            = mxl.dwLineID;
                           pmclTemp->dwComponentType     = mxl.dwComponentType;
                           mmr = mixerGetControlDetails((HMIXEROBJ)m_hMixer, &pmclTemp->mxcd, 0L);
                           ReportMMError(mmr, false);
                           if (bInitValues & INIT_LINES)
                           {
                              if (bInitValues & STORE_LINES)
                              {
                                 SetRegDWord(hKey, pmclTemp->mxctl.szShortName, pmclTemp->cd.boolean[0].fValue);
                                 REPORT("Save %s : %x", pmclTemp->mxctl.szShortName, pmclTemp->cd.boolean[0].fValue);
                              }
                              if (!(bInitValues & RESTORE_LINES))
                              {
                                 pmclTemp->cd.boolean[0].fValue = 0;
                                 mmr = mixerSetControlDetails((HMIXEROBJ)m_hMixer, &pmclTemp->mxcd, 0L);
                                 ReportMMError(mmr, false);
                                 REPORT("Set %s = %d", pmclTemp->mxctl.szShortName, pmclTemp->cd.boolean[0].fValue);
                              }
                           }
                           if (bInitValues & RESTORE_LINES)
                           {
                              pmclTemp->cd.boolean[0].fValue = GetRegDWord(hKey, pmclTemp->mxctl.szShortName, 0);
                              mmr = mixerSetControlDetails((HMIXEROBJ)m_hMixer, &pmclTemp->mxcd, 0L);
                              ReportMMError(mmr, false);
                              REPORT("Restore %s : %x", pmclTemp->mxctl.szShortName, pmclTemp->cd.boolean[0].fValue);
                           }
                           m_Controls.Delete(pmclTemp);
                        } break;
                        case MIXERCONTROL_CONTROLTYPE_FADER:
                        case MIXERCONTROL_CONTROLTYPE_PEAKMETER:
                        default:
                           dwValue = 0;
                           SetRegDWord(hKey, mxlc.pamxctrl[k].szShortName, dwValue);
                           break;;
                     }
                  }
               }
               delete mxlc.pamxctrl;
            }

            morecontrols:                                      // (goto) weitere Controls einfügen

            if ((m_hWnd != NULL) && (pmclVolume != NULL) &&    // Volumecontrol vorhanden
                (LOWORD(dwFlags) & VOLUME_ACTIVE))             // und Aktiv
            {
               nCreated++;
               if (nControl == 0)                              // vorhandene Windos setzen
               {
                  pmclVolume->hwndMute        = hwndCKMute;
                  pmclVolume->hwndSingle      = hwndCKSingle;
                  pmclVolume->hwndSliderLeft  = hwndSliderL;
                  pmclVolume->hwndSliderRight = hwndSliderR;
                  ::SetWindowText(hwndCKMute, mxl.szShortName);
                  if (pmclMute)                                // Muting vorhanden 
                  {  
                     pmclMute->hwndMute        = hwndCKMute;
                     pmclMute->hwndSingle      = hwndCKSingle;
                     pmclMute->hwndSliderLeft  = hwndSliderL;
                     pmclMute->hwndSliderRight = hwndSliderR;
                     strcpy(pmclMute->mxctl.szName, mxl.szName);
                     ::EnableWindow(hwndCKMute, true);
                     ::SetWindowLong(hwndCKMute, GWL_USERDATA, (long)pmclMute);
                     ::SendMessage(hwndCKMute, BM_SETCHECK, (pmclMute->cd.boolean[0].fValue) ? BST_UNCHECKED : BST_CHECKED, 0);
                     ::SetWindowContextHelpId(hwndCKMute, IDC_MIXER_LINE_MUTE);
                  }
                  else if (pmclMux)                            // MuxControl vorhanden
                  {
                     UINT i;
                     for (i=0; i<pmclMux->mxcd.cMultipleItems; i++)
                     {
                        if (pmclMux->cd.listext[i].dwParam1 == mxl.dwLineID)
                        {
                           pmclMux->cd.listext[i].dwParam2 = (DWORD)pmclVolume;
                           break;
                        }
                     }
                     ::SetWindowLong(hwndCKMute, GWL_USERDATA, (long)pmclMux);
                     ::SetWindowContextHelpId(hwndCKMute, IDC_MIXER_LINE_MUX);
                  }
                  else
                  {
                     ::EnableWindow(hwndCKMute, false);
                     ::SetWindowContextHelpId(hwndCKMute, IDC_MIXER_LINE_FADER);
                     ::SetWindowText(hwndCKMute, pmclVolume->mxctl.szShortName);
                     ::SetWindowLong(hwndCKMute, GWL_USERDATA, (long)pmclVolume);
                  }

                  ::SetWindowText(hwndSliderL , "Left");       // Lautstärke rechts
                  ::SetWindowLong(hwndSliderL, GWL_USERDATA, (long)pmclVolume);
                  ::SendMessage(hwndSliderL, TBM_SETRANGEMIN, 0,   0);
                  ::SendMessage(hwndSliderL, TBM_SETRANGEMAX, 0, 100);
                  VolumeControl(hwndSliderL, SET_SLIDERPOSITION);

                  if (mxl.cChannels == 2)                      // Lautstärke rechts, wenn vorhanden
                  {
                     ::EnableWindow(hwndCKSingle, true);
                     ::ShowWindow(hwndCKSingle, SW_SHOW);
                     ::EnableWindow(hwndSliderR, true);
                     ::ShowWindow(hwndSliderR, SW_SHOW);
                     ::SetWindowText(hwndSliderR , "Right");
                     ::SetWindowLong(hwndSliderR, GWL_USERDATA, (long)pmclVolume);
                     ::SendMessage(hwndSliderR, TBM_SETRANGEMIN, 0,   0);
                     ::SendMessage(hwndSliderR, TBM_SETRANGEMAX, 0, 100);
                     VolumeControl(hwndSliderR, SET_SLIDERPOSITION);
                     ::SetWindowLong(hwndCKSingle, GWL_USERDATA, (long)pmclVolume);
                     pmclVolume->bSingle = (::SendMessage(hwndCKSingle, BM_GETCHECK, 0, 0) == BST_CHECKED) ? true : false;
                  }
                  else                                         // sonst disablen
                  {
                     ::EnableWindow(hwndCKSingle, false);
                     ::ShowWindow(hwndCKSingle, SW_HIDE);
                     ::EnableWindow(hwndSliderR, false);
                     ::ShowWindow(hwndSliderR, SW_HIDE);
                  }
               }
               else
               {
                  HWND hwnd;
                  dwChkBtnStyle  &= ~WS_DISABLED;
                  dwChkBtnStyle  |= WS_VISIBLE;
                  dwSliderLStyle &= ~WS_DISABLED;
                  dwSliderLStyle |= WS_VISIBLE;
                  dwSliderRStyle &= ~WS_DISABLED;
                  dwSliderRStyle |= WS_VISIBLE;
                                                               // Volume links
                  ASSERT(GetDlgItem(IDC_MIXER_SLIDER_L+nControl) == NULL);
                  OffsetRect(&rcSlider, rcSlider.right - rcSlider.left+5, 0);
                  hwnd = CreateWindowEx(dwSliderEx, szSliderClass, "Left", dwSliderLStyle,
                         rcSlider.left, rcSlider.top,
                         rcSlider.right - rcSlider.left, rcSlider.bottom - rcSlider.top,
                         m_hWnd, NULL, m_hInstance, NULL);
                  ::SetWindowLong(hwnd, GWL_ID, IDC_MIXER_SLIDER_L+nControl);
                  ::SetWindowLong(hwnd, GWL_USERDATA, (long)pmclVolume);
                  ::SetWindowContextHelpId(hwnd, IDC_MIXER_SLIDER_L);
                  ::SendMessage(hwnd, TBM_SETRANGEMIN, 0,   0);
                  ::SendMessage(hwnd, TBM_SETRANGEMAX, 0, 100);
                  pmclVolume->hwndSliderLeft = hwnd;
                                                               // Volume rechts
                  OffsetRect(&rcSlider, rcSlider.right - rcSlider.left+1, 0);
                  if (mxl.cChannels == 2)
                  {
                     hwnd = CreateWindowEx(dwSliderEx, szSliderClass, "Right", dwSliderRStyle,
                            rcSlider.left, rcSlider.top,
                            rcSlider.right - rcSlider.left, rcSlider.bottom - rcSlider.top,
                            m_hWnd, NULL, m_hInstance, NULL);
                     ::SetWindowLong(hwnd, GWL_ID, IDC_MIXER_SLIDER_L+nControl+1);
                     ::SetWindowLong(hwnd, GWL_USERDATA, (long)pmclVolume);
                     ::SetWindowContextHelpId(hwnd, IDC_MIXER_SLIDER_R);
                     ::SendMessage(hwnd, TBM_SETRANGEMIN, 0,   0);
                     ::SendMessage(hwnd, TBM_SETRANGEMAX, 0, 100);
                     pmclVolume->hwndSliderRight = hwnd;
                  }
                                                               // Single control
                  OffsetRect(&rcCKSingle, rcCKSingle.right - rcCKSingle.left+5, 0);
                  if (mxl.cChannels == 2)
                  {
                     hwnd = CreateWindowEx(dwChkBtnStyleEx, szChkBtnClass, szSingleText, dwChkBtnStyle, 
                            rcCKSingle.left, rcCKSingle.top,
                            rcCKSingle.right - rcCKSingle.left, rcCKSingle.bottom - rcCKSingle.top,
                            m_hWnd, NULL, m_hInstance, NULL);
                     ::SetWindowLong(hwnd, GWL_ID, IDC_MIXER_SLIDER_L+nControl+3);
                     ::SendMessage(hwnd, WM_SETFONT, (WPARAM)hChkBtnFont, 0);
                     ::SetWindowLong(hwnd, GWL_USERDATA, (long)pmclVolume);
                     ::SetWindowContextHelpId(hwnd, IDC_MIXER_LINE_SINGLE);
                     pmclVolume->bSingle = (::SendMessage(hwnd, BM_GETCHECK, 0, 0) == BST_CHECKED) ? true : false;
                     pmclVolume->hwndSingle = hwnd;
                  }

                  VolumeControl(pmclVolume->hwndSliderLeft, SET_SLIDERPOSITION);
                  if (mxl.cChannels == 2)
                     VolumeControl(pmclVolume->hwndSliderRight, SET_SLIDERPOSITION);
                                                               // Mute Control
                  OffsetRect(&rcCKMute, rcCKMute.right - rcCKMute.left+5, 0);
                  hwnd = CreateWindowEx(dwChkBtnStyleEx, szChkBtnClass, mxl.szShortName, dwChkBtnStyle,
                         rcCKMute.left, rcCKMute.top,
                         rcCKMute.right - rcCKMute.left, rcCKMute.bottom - rcCKMute.top,
                         m_hWnd, NULL, m_hInstance, NULL);
                  ::SetWindowLong(hwnd, GWL_ID, IDC_MIXER_SLIDER_L+nControl+2);
                  ::SetWindowContextHelpId(hwnd, IDC_MIXER_LINE_MUTE);
                  ::SendMessage(hwnd, WM_SETFONT, (WPARAM)hChkBtnFont, 0);
                  if (pmclMute)
                  {
                     ::SetWindowLong(hwnd, GWL_USERDATA, (long)pmclMute);
                     ::SendMessage(hwnd, BM_SETCHECK, (pmclMute->cd.boolean[0].fValue) ? BST_UNCHECKED : BST_CHECKED, 0);
                     pmclMute->hwndMute        = pmclVolume->hwndMute = hwnd;
                     pmclMute->hwndSliderLeft  = pmclVolume->hwndSliderLeft;
                     pmclMute->hwndSliderRight = pmclVolume->hwndSliderRight;
                     pmclMute->hwndSingle      = pmclVolume->hwndSingle;
                     strcpy(pmclMute->mxctl.szName, mxl.szName);
                  }
                  else if (pmclMux)                            // MuxControl vorhanden
                  {
                     UINT i;
                     pmclVolume->hwndMute = hwnd;
                     for (i=0; i<pmclMux->mxcd.cMultipleItems; i++)
                     {
                        if (pmclMux->cd.listext[i].dwParam1 == mxl.dwLineID)
                        {
                           pmclMux->cd.listext[i].dwParam2 = (DWORD)pmclVolume;
                           break;
                        }
                     }
                     ::SetWindowLong(hwnd, GWL_USERDATA, (long)pmclMux);
                     ::SetWindowContextHelpId(hwnd, IDC_MIXER_LINE_MUX);
                  }
                  else
                  {
                     ::SetWindowContextHelpId(hwnd, IDC_MIXER_LINE_FADER);
                     ::SetWindowText(hwnd, pmclVolume->mxctl.szShortName);
                     DWORD dwStyle = ::GetWindowLong(hwnd, GWL_STYLE);
                     dwStyle &= ~BS_CHECKBOX;
                     ::SetWindowLong(hwnd, GWL_STYLE, dwStyle);
                     ::SetWindowLong(hwnd, GWL_USERDATA, (long)pmclVolume);
                  }
               }
               nControl+=4;                                 // es kommen immer 4 Controlnummern dazu
               pmclVolume = NULL;
               pmclMute   = NULL;
            }
/*
            if ((m_hWnd != NULL) && (pmclMeter != NULL))
            {
               HWND hwnd;
               OffsetRect(&rcSlider, rcSlider.right - rcSlider.left+5, 0);
               hwnd = CreateWindowEx(dwSliderEx, szSliderClass, "Left", dwSliderLStyle,
                      rcSlider.left, rcSlider.top,
                      rcSlider.right - rcSlider.left, rcSlider.bottom - rcSlider.top,
                      m_hWnd, NULL, m_hInstance, NULL);
               ::SetWindowLong(hwnd, GWL_ID, IDC_MIXER_SLIDER_L+nControl);
               ::SetWindowLong(hwnd, GWL_USERDATA, (long)pmclMeter);
               ::SendMessage(hwnd, TBM_SETRANGEMIN, 0,   0);
               ::SendMessage(hwnd, TBM_SETRANGEMAX, 0, 100);
               pmclMeter->hwndSliderLeft = hwnd;
                                                            // VU rechts
               OffsetRect(&rcSlider, rcSlider.right - rcSlider.left+1, 0);
               if (mxl.cChannels == 2)
               {
                  hwnd = CreateWindowEx(dwSliderEx, szSliderClass, "Right", dwSliderRStyle,
                         rcSlider.left, rcSlider.top,
                         rcSlider.right - rcSlider.left, rcSlider.bottom - rcSlider.top,
                         m_hWnd, NULL, m_hInstance, NULL);
                  ::SetWindowLong(hwnd, GWL_ID, IDC_MIXER_SLIDER_L+nControl+1);
                  ::SetWindowLong(hwnd, GWL_USERDATA, (long)pmclMeter);
                  ::SendMessage(hwnd, TBM_SETRANGEMIN, 0,   0);
                  ::SendMessage(hwnd, TBM_SETRANGEMAX, 0, 100);
                  pmclMeter->hwndSliderRight = hwnd;
               }
               OffsetRect(&rcCKSingle, rcCKSingle.right - rcCKSingle.left+5, 0);
               OffsetRect(&rcCKMute, rcCKMute.right - rcCKMute.left+5, 0);
               hwnd = CreateWindowEx(dwChkBtnStyleEx, szChkBtnClass, pmclMeter->mxctl.szShortName, dwChkBtnStyle,
                      rcCKMute.left, rcCKMute.top,
                      rcCKMute.right - rcCKMute.left, rcCKMute.bottom - rcCKMute.top,
                      m_hWnd, NULL, m_hInstance, NULL);
               ::SetWindowLong(hwnd, GWL_ID, IDC_MIXER_SLIDER_L+nControl+2);
               ::SendMessage(hwnd, WM_SETFONT, (WPARAM)hChkBtnFont, 0);
               pmclMeter = NULL;
               nControl+=3;                                 // es kommen hier 3 Controlnummern dazu
            }
*/
            if (pmclVolume)
            {
               m_Controls.Delete(pmclVolume);
               pmclVolume = NULL;
            }
            if (listSoundEqu.GetCounter())
            {
               if (dwFlags & EQU_ACTIVE)
               {
                  pmclVolume = (MixerControlLine*)listSoundEqu.GetLast();
                  listSoundEqu.Remove(pmclVolume);
                  goto morecontrols;
               }
               else
               {
                  while (listSoundEqu.GetCounter())
                  {
                     void *p = listSoundEqu.GetLast();
                     m_Controls.Delete(p);
                     listSoundEqu.Remove(p);
                  }
               }
            }
            if (pmclMute)
            {
               m_Controls.Delete(pmclMute);
               pmclMute = NULL;
            }
            if (pmclMeter)
            {
               m_Controls.Delete(pmclMeter);
               pmclMeter = NULL;
            }
         }
         if (nCreated)
         {
            pmclMux = NULL;
            nDestinations++;
            if (rcCKSingle.right  > szMax.cx) szMax.cx = rcCKSingle.right;
            if (rcCKSingle.bottom > szMax.cy) szMax.cy = rcCKSingle.bottom+nBorderx+GetSystemMetrics(SM_CYDLGFRAME)+GetSystemMetrics(SM_CYBORDER)+GetSystemMetrics(SM_CYCAPTION);
         }
         if (pmclMux)
         {
            m_Controls.Delete(pmclMux);
            pmclMux = NULL;
         }
      }
   }
   catch (MMRESULT mmError)
   {
      ReportMMError(mmError, true);
      m_nMixerDev = INVALID_VALUE;
   }
   if (hKey) EtsRegCloseKey(hKey);

   if (m_hWnd)
   {
      if ((nCreated != 0) || (nDestinations != 0))
         ::SetWindowPos(m_hWnd, NULL, 0, 0, szMax.cx+nBorderx+GetSystemMetrics(SM_CXDLGFRAME)+GetSystemMetrics(SM_CXBORDER), szMax.cy, SWP_NOMOVE|SWP_NOZORDER);
   }

   if (hwndTemp) m_hWnd = hwndTemp;

   return true;
} // OpenMixerDev

void CMixerDlg::GetControlValues(MixerControlLine *pml)
{
   BEGIN("GetControlValues");
   MMRESULT mmr;
   REPORT("%s",pml->mxctl.szName);
//   if (pml->mxctl.dwControlType == MIXERCONTROL_CONTROLTYPE_VOLUME)
   if ((pml->mxctl.dwControlType&MIXERCONTROL_CT_CLASS_MASK) == MIXERCONTROL_CT_CLASS_FADER)
   {
      BOOL bSingle = pml->bSingle;                             // Timerupdate
      pml->bSingle = true;                                     // darf den Gleichlauf nicht erzwingen
      VolumeControl(pml->hwndSliderLeft , SET_SLIDERPOSITION);
      VolumeControl(pml->hwndSliderRight, SET_SLIDERPOSITION);
      pml->bSingle = bSingle;
   }
   else if (pml->mxctl.dwControlType == MIXERCONTROL_CONTROLTYPE_MUTE)
   {
      mmr = mixerGetControlDetails((HMIXEROBJ)m_hMixer, &pml->mxcd, 0L);
      ReportMMError(mmr, false);
      ::SendMessage(pml->hwndMute, BM_SETCHECK, (pml->cd.boolean[0].fValue) ? BST_UNCHECKED : BST_CHECKED, 0);
   }
   else
   {
      switch (pml->mxctl.dwControlType)
      {
         case MIXERCONTROL_CONTROLTYPE_SINGLESELECT:
         case MIXERCONTROL_CONTROLTYPE_MUX:
         case MIXERCONTROL_CONTROLTYPE_MULTIPLESELECT:
         case MIXERCONTROL_CONTROLTYPE_MIXER:
         {
            UINT cChannels      = pml->mxcd.cChannels;
            UINT cMultipleItems = pml->mxcd.cMultipleItems;
            MIXERCONTROLDETAILS_BOOLEAN *pBool = (MIXERCONTROLDETAILS_BOOLEAN*) pml->mxcd.paDetails;
            mmr = mixerGetControlDetails((HMIXEROBJ)m_hMixer, &pml->mxcd, 0L);
            ReportMMError(mmr, false);
            pml->mxcd.cChannels      = cChannels;
            pml->mxcd.cMultipleItems = cMultipleItems;
            for (UINT u = 0; u < cChannels; u++)
            {
               for (UINT v = 0; v < cMultipleItems; v++)
               {
                  UINT uIndex = (u * cMultipleItems) + v;
                  MixerControlLine *pmlVolume = (MixerControlLine*) pml->cd.listext[uIndex].dwParam2;
                  ::SendMessage(pmlVolume->hwndMute, BM_SETCHECK, (pBool[uIndex].fValue) ? BST_CHECKED : BST_UNCHECKED, 0);
               }
            }
         } break;
      }
   }
} // GetControlValues

int CMixerDlg::FindControlID(const void *p1, const void *p2)
{
   MixerControlLine *pml = (MixerControlLine*)p1;
   return pml->mxcd.dwControlID - *(LPARAM*)p2;
}

void CMixerDlg::SetWaveVolume(long lValue)
{
   if (m_pWaveCtrl)
   {
      MixerControlLine *pml = m_pWaveCtrl;
      lValue = 100 - lValue;
      pml->cd.unSigned[0].dwValue = MulDiv(lValue, pml->mxctl.Bounds.dwMaximum, 100);
      pml->cd.unSigned[1].dwValue = pml->cd.unSigned[0].dwValue;
      HMIXEROBJ hMixer = (HMIXEROBJ) ((m_hWaveMixer != NULL) ? m_hWaveMixer : m_hMixer);
      MMRESULT mmr = mixerSetControlDetails(hMixer, &pml->mxcd, 0L);
      ReportMMError(mmr, false);
   }
} // SetWaveVolume

long CMixerDlg::GetWaveVolume()
{
   if (m_pWaveCtrl)
   {
      MixerControlLine *pml = m_pWaveCtrl;
      HMIXEROBJ hMixer = (HMIXEROBJ) ((m_hWaveMixer != NULL) ? m_hWaveMixer : m_hMixer);
      MMRESULT mmr = mixerGetControlDetails(hMixer, &pml->mxcd, 0L);
      long lValue = 100 - MulDiv((pml->cd.unSigned[0].dwValue+pml->cd.unSigned[1].dwValue)/2, 100, pml->mxctl.Bounds.dwMaximum);
      ReportMMError(mmr, false);
      return lValue;
   }
   return 100;
} // GetWaveVolume

int CMixerDlg::GetWaveCtrlID()
{
   if (m_pWaveCtrl)
   {
      return m_pWaveCtrl->mxcd.dwControlID;
   }
   return -1;
}

HMIXER CMixerDlg::GetWaveMixer()
{
   if (m_pWaveCtrl)
   {
      if (m_hWaveMixer == NULL)
      {
         m_hWaveMixer = m_hMixer;
         m_hMixer = NULL;
      }
      return m_hWaveMixer;
   }
   return NULL;
}

void CMixerDlg::VolumeControl(HWND hwnd, bool bSet)
{
   if (hwnd == NULL) return;
   MixerControlLine *pml = (MixerControlLine*)::GetWindowLong(hwnd, GWL_USERDATA);

//   REPORT("%x", pml);

   if (pml == NULL) return;
   if (*(DWORD*)pml != sizeof(MIXERCONTROLDETAILS)) return;
//   if (pml->mxctl.dwControlType& != MIXERCONTROL_CONTROLTYPE_VOLUME)
   if ((pml->mxctl.dwControlType&MIXERCONTROL_CT_CLASS_MASK) != MIXERCONTROL_CT_CLASS_FADER)
      return;

   MMRESULT mmr;                                    // Fehlervariable
   char     szText[32];
   ::GetWindowText(hwnd, szText, 32);
//   REPORT("%s:%s, %d", pml->mxctl.szName, szText, pml->mxcd.cChannels);
   bool bLeft  = (strcmp(szText, "Left" ) == 0) ? true : false;
   bool bRight = (strcmp(szText, "Right") == 0) ? true : false;
   if (bLeft || bRight)
   {
      if (bSet)
      {
         mmr = mixerGetControlDetails((HMIXEROBJ)m_hMixer, &pml->mxcd, 0L);
         REPORT("GetDetails : %d", mmr);
         ReportMMError(mmr, false);

         if (!pml->bSingle && (pml->mxcd.cChannels > 1))       // Gleichschalten
         {
//            REPORT("SetEqual");
            bLeft = bRight = true;
            if (pml->cd.unSigned[0].dwValue > pml->cd.unSigned[1].dwValue) pml->cd.unSigned[1].dwValue = pml->cd.unSigned[0].dwValue;
            else                                                           pml->cd.unSigned[0].dwValue = pml->cd.unSigned[1].dwValue;
         }
         LPARAM lValue;
         if (bLeft)
         {
//            REPORT("SetLeft");
            lValue = 100 - MulDiv(pml->cd.unSigned[0].dwValue, 100, pml->mxctl.Bounds.dwMaximum);
            ::SendMessage(pml->hwndSliderLeft, TBM_SETPOS, 1, lValue);
         }
         if (bRight)
         {
//            REPORT("SetRight");
            lValue = 100 - MulDiv(pml->cd.unSigned[1].dwValue, 100, pml->mxctl.Bounds.dwMaximum);
            ::SendMessage(pml->hwndSliderRight, TBM_SETPOS, 1, lValue);
         }
      }
      else
      {
         LPARAM lValue = 0;
         if (bLeft)
         {
            lValue = 100 - ::SendMessage(hwnd, TBM_GETPOS, 0, 0);
            pml->cd.unSigned[0].dwValue = MulDiv(lValue, pml->mxctl.Bounds.dwMaximum, 100);
//            REPORT("Get Left %d", pml->cd.unSigned[0].dwValue);
         }
         if (bRight)
         {
            lValue = 100 - ::SendMessage(hwnd, TBM_GETPOS, 0, 0);
            pml->cd.unSigned[1].dwValue = MulDiv(lValue, pml->mxctl.Bounds.dwMaximum, 100);
//            REPORT("Get Right %d", pml->cd.unSigned[1].dwValue);
         }

         if (!pml->bSingle && (pml->mxcd.cChannels > 1))       // Gleichschalten
         {
            if (bLeft )                                        // links bewegt ?
            {
               pml->cd.unSigned[1].dwValue = pml->cd.unSigned[0].dwValue;// rechts setzen
               ::SendMessage(pml->hwndSliderRight, TBM_SETPOS, 1, 100-lValue);
//               REPORT("  SetRight");
            }
            if (bRight)                                        // rechts bewegt ?
            {
               pml->cd.unSigned[0].dwValue  = pml->cd.unSigned[1].dwValue;// links setzen
               ::SendMessage(pml->hwndSliderLeft , TBM_SETPOS, 1, 100-lValue);
//               REPORT("  SetLeft");
            }
            bLeft = bRight = true;
         }
         mmr = mixerSetControlDetails((HMIXEROBJ)m_hMixer, &pml->mxcd, 0L);
//         REPORT("SetDetails : %d", mmr);
         ReportMMError(mmr, false);
      }
   }
} // VolumeControl

void *CMixerDlg::CreateMCL(MIXERCONTROL &mxctl, DWORD dwSize)
{
   dwSize += (sizeof(MixerControlLine) - sizeof(MixerControlLine::ControlDetails));
   MixerControlLine *pmcl = (MixerControlLine*)new BYTE[dwSize];
   m_Controls.ADDTail(pmcl);
   pmcl->mxctl               = mxctl;
   pmcl->mxcd.cbStruct       = sizeof(MIXERCONTROLDETAILS);
   pmcl->mxcd.dwControlID    = mxctl.dwControlID;
   pmcl->mxcd.cChannels      = 1;
   pmcl->mxcd.cMultipleItems = 0;
   pmcl->mxcd.cbDetails      = 0;
   pmcl->hwndMute            = NULL;
   pmcl->hwndSingle          = NULL;
   pmcl->hwndSliderLeft      = NULL;
   pmcl->hwndSliderRight     = NULL;
   pmcl->bSingle             = true;
   return (void*)pmcl;
} // CreateMCL

void CMixerDlg::DeleteMCL(void *p)
{
   MixerControlLine *pml = (MixerControlLine*)p;
   switch (pml->mxctl.dwControlType)
   {
      case MIXERCONTROL_CONTROLTYPE_SINGLESELECT:
      case MIXERCONTROL_CONTROLTYPE_MUX:
      case MIXERCONTROL_CONTROLTYPE_MULTIPLESELECT:
      case MIXERCONTROL_CONTROLTYPE_MIXER:
      if (pml->mxcd.paDetails)
      {
         delete pml->mxcd.paDetails;
      } break;
   }
   delete pml;
} // DeleteMCL

void CMixerDlg::CloseMixerDev()
{
   if ((m_hWaveMixer != NULL) && (m_hMixer == NULL))
   {
      m_hMixer = m_hWaveMixer;
      m_hWaveMixer = NULL;
   }
   if (m_hMixer)
   {
      OpenMixerDev(NULL, true);
      mixerClose(m_hMixer);
      m_hMixer = NULL;
   }
   if (m_hWaveMixer)
   {
      mixerClose(m_hWaveMixer);
      m_hWaveMixer = NULL;
   }
}

void CMixerDlg::ReportMMError(MMRESULT mmr, bool bMsgBox)
{
   if (mmr)
   {
      char szErrorText[MAX_PATH];
      mciGetErrorString(mmr, szErrorText, MAX_PATH);
      REPORT("%s",  szErrorText);
      if (bMsgBox && m_hWnd)
      {
         char szCaption[MAX_PATH];
         ::GetWindowText(m_hWnd, szCaption, MAX_PATH);
         MessageBox((long)szErrorText, (long)szCaption, MBU_CAPTIONSTRING|MBU_TEXTSTRING|MB_OK|MB_ICONERROR);
      }
   }
}

BOOL CMixerDlg::MixAppGetComponentName(LPMIXERLINE pmxl, LPTSTR szComponent)
{
   if (0 == (MIXERLINE_LINEF_SOURCE & pmxl->fdwLine))
   {
      int nID = IDS_MIXER_DEST0 + pmxl->dwComponentType - MIXERLINE_COMPONENTTYPE_DST_FIRST;
      ::LoadString(m_hInstance, nID, szComponent, 128);
   }
   else
   {
      int nID = IDS_MIXER_SRC0 + pmxl->dwComponentType - MIXERLINE_COMPONENTTYPE_SRC_FIRST;
      ::LoadString(m_hInstance, nID, szComponent, 128);
   }

   return (TRUE);
} // MixAppGetComponentName()

DWORD CMixerDlg::ModifyInitFlag(DWORD dwSet, DWORD dwRemove)
{
   HKEY hKey = NULL;
   DWORD dwInitLines = 0, dwSize = 0;
   if (OpenRegistry(m_hInstance, KEY_WRITE|KEY_READ, &hKey, MIXER))
   {
      bool bChanged = ((dwSet!=0) || (dwRemove!=0)) ? true : false;
      dwInitLines = GetRegDWord(hKey, MIXER_INIT, INVALID_VALUE);
      if (dwInitLines == INVALID_VALUE)                        // Registrywert nicht vorhanden
      {
         bChanged = true;
         dwInitLines = INIT_LINES;                             // Initialisieren
      }

      if (LOWORD(dwSet))    dwInitLines |= LOWORD(dwSet);      // Flags setzen
      if (LOWORD(dwRemove)) dwInitLines &= ~LOWORD(dwRemove);  // Flags löschen

      if (bChanged) SetRegDWord(hKey, MIXER_INIT, dwInitLines);// RegWert schreiben, wenn geändert
      GetRegBinary(hKey, MIXER_HWND, NULL, dwSize);
      if (dwSize)
      {
         dwSize /= sizeof(HWND);
         dwInitLines |= dwSize<<16;
      }
   }
   if (hKey) EtsRegCloseKey(hKey);
   return dwInitLines;
}

void CMixerDlg::CountMixerCtrInst(HWND hwnd, bool bSet)
{
   HKEY hKey         = NULL;
   HWND *phwnd       = NULL;
   DWORD dwSize      = 0;
   if (OpenRegistry(m_hInstance, KEY_WRITE|KEY_READ, &hKey, MIXER))
   {
      GetRegBinary(hKey, MIXER_HWND, NULL, dwSize);
      int i, nWnd = 0; 
      if (dwSize > 0)
      {
         nWnd = dwSize / sizeof(HWND);
      }
      if (bSet)       nWnd++;
      if (nWnd>0)
      {
         phwnd = new HWND[nWnd];
         GetRegBinary(hKey, MIXER_HWND, phwnd, dwSize);
         for (i=0; i<nWnd; i++)
         {
            if (hwnd == phwnd[i])
            {
               if (bSet == false) phwnd[i] = NULL;
               break;
            }
         }
         if ((i == nWnd) && bSet) phwnd[nWnd-1] = hwnd;
         for (i=0; i<nWnd; i++)
         {
            if ((phwnd[i] != NULL) && !::IsWindow(phwnd[i])) 
               phwnd[i] = NULL;
         }
         if (nWnd > 1) qsort(phwnd, nWnd, sizeof(HWND), SortWnd);
         for (i=0; i<nWnd; i++)
            if (phwnd[i] == NULL)break;
         SetRegBinary(hKey, MIXER_HWND, (BYTE*)phwnd, i*sizeof(HWND), false);
//         else RegDeleteKey(hKey, MIXER_HWND);
      }
   }
   if (hKey) EtsRegCloseKey(hKey);
   if (phwnd) delete[] phwnd;
}

BOOL CALLBACK CMixerDlg::EnumSliderSysColorChange(HWND hwnd, LPARAM lParam)
{
   char text[64];
   ::GetClassName(hwnd, text, 64);
   if (strcmp(text, "msctls_trackbar32") == 0)
   {
      MSG *pmsg = (MSG*) lParam;
      ::SendMessage(hwnd, pmsg->message, pmsg->wParam, pmsg->lParam);
   }
   return 1;
}

int CMixerDlg::SortWnd(const void *p1, const void *p2)
{
   HWND *pWnd1 = (HWND*)p1;
   HWND *pWnd2 = (HWND*)p2;
   if (*pWnd1 == NULL) return  1;
   if (*pWnd2 == NULL) return -1;
   return 0;
}

/*
    switch (pmxctrl->dwControlType)
    {
        case MIXERCONTROL_CONTROLTYPE_SINGLESELECT:
        case MIXERCONTROL_CONTROLTYPE_MUX:
        case MIXERCONTROL_CONTROLTYPE_MULTIPLESELECT:
        case MIXERCONTROL_CONTROLTYPE_MIXER:
            break;

        default:
            DPF(0, "!MixAppInitDialogList: pmxctrl->dwControlType=%.08lXh not valid list type!", pmxctrl->dwControlType);
            return (FALSE);
    }

    fSingleSelect = (MIXERCONTROL_CT_SC_LIST_SINGLE == (MIXERCONTROL_CT_SUBCLASS_MASK & pmxctrl->dwControlType));

    cChannels = (UINT)pmxl->cChannels;
    if (MIXERCONTROL_CONTROLF_UNIFORM & pmxctrl->fdwControl)
        cChannels = 1;

    cMultipleItems = 1;
    if (MIXERCONTROL_CONTROLF_MULTIPLE & pmxctrl->fdwControl)
        cMultipleItems = (UINT)pmxctrl->cMultipleItems;

    cb  = sizeof(*pmaci_list);
    cb += cChannels * cMultipleItems * sizeof(pmaci_list->pmxcd_f[0]);
    pmaci_list = (PMACONTROLINSTANCE_LIST)LocalAlloc(LPTR, cb);
    if (NULL == pmaci_list)
    {
        DPF(0, "!MixAppInitDialogList: failed trying to alloc %u bytes for control instance!", cb);
        return (FALSE);
    }

    pmaci_list->pmaci         = pmaci;
    pmaci_list->fSingleSelect = fSingleSelect;


    //
    //
    //
    cb = cChannels * cMultipleItems * sizeof(*pmxcd_lt);
    pmxcd_lt = (PMIXERCONTROLDETAILS_LISTTEXT)LocalAlloc(LPTR, cb);
    if (NULL == pmaci_list)
    {
        DPF(0, "!MixAppInitDialogList: failed trying to alloc %u bytes for control list text!", cb);
        return (FALSE);
    }


    //
    //
    //
    mxcd.cbStruct       = sizeof(mxcd);
    mxcd.dwControlID    = pmxctrl->dwControlID;
    mxcd.cChannels      = cChannels;
    mxcd.cMultipleItems = cMultipleItems;
    mxcd.cbDetails      = sizeof(*pmxcd_lt);
    mxcd.paDetails      = pmxcd_lt;

    mmr = mixerGetControlDetails((HMIXEROBJ)pmaci->hmx, &mxcd, MIXER_GETCONTROLDETAILSF_LISTTEXT);
    if (MMSYSERR_NOERROR != mmr)
    {
        AppMsgBox(hwnd, MB_OK | MB_ICONEXCLAMATION,
                    "mixerGetControlDetails(ctrlid=%.08lXh) on LISTTEXT failed on hmx=%.04Xh, mmr=%u!",
                    pmxctrl->dwControlID, pmaci->hmx, mmr);
        return (FALSE);
    }


    rcM.bottom = rcM.top + cycap;

    for (u = 0; u < cChannels; u++)
    {
        for (v = 0; v < cMultipleItems; v++)
        {
            uIndex = (u * cMultipleItems) + v;

            CreateWindow(szButton,
            Testchannelnumber
        }
    }



    //
    //
    //
    mxcd.cbStruct       = sizeof(mxcd);
    mxcd.dwControlID    = pmxctrl->dwControlID;
    mxcd.cChannels      = 1;
    mxcd.cMultipleItems = cMultipleItems;
    mxcd.cbDetails      = sizeof(*pmxcd_lt);
    mxcd.paDetails      = pmxcd_lt;

    mmr = mixerGetControlDetails((HMIXEROBJ)pmaci->hmx, &mxcd, MIXER_GETCONTROLDETAILSF_LISTTEXT);
    if (MMSYSERR_NOERROR != mmr)
    {
        AppMsgBox(hwnd, MB_OK | MB_ICONEXCLAMATION,
                    "mixerGetControlDetails(ctrlid=%.08lXh) on LISTTEXT failed on hmx=%.04Xh, mmr=%u!",
                    pmxctrl->dwControlID, pmaci->hmx, mmr);
        return (FALSE);
    }


    //
    //
    //
    //
    hcheck = GetDlgItem(hwnd, IDD_MACONTROL_GRP_UNIFORM);
    GetWindowRect(hcheck, &rcU);

    InflateRect(&rcU, -10, -20);
    ScreenToClient(hwnd, (LPPOINT)&rcU.left);
    ScreenToClient(hwnd, (LPPOINT)&rcU.right);

    rcU.bottom = rcU.top + cycap;

    for (v = 0; v < cMultipleItems; v++)
    {
        hcheck = CreateWindow(szButton,
                              pmxcd_lt[v].szName,
                              FCB_DEF_STYLE,
                              rcU.left, rcU.top,
                              rcU.right - rcU.left,
                              rcU.bottom - rcU.top,
                              hwnd, (HMENU)(IDD_MACONTROL_UNIFORM_BASE + v),
                              ghinst, NULL);

        rcU.top    += cycap + 4;
        rcU.bottom += cycap + 4;
    }


    LocalFree((HLOCAL)pmxcd_lt);
*/
