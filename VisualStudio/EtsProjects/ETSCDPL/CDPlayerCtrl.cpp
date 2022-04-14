// CDPlayerCtrl.cpp: Implementierung der Klasse CCDPlayerCtrl.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CDPlayerCtrl.h"
#include "ETS3DGLRegKeys.h"
#include "EtsRegistry.h"
#include "resource.h"
#include "CEtsDlg.h"

#define  ETSDEBUG_INCLUDE 
#include "CEtsDebug.h"

extern HINSTANCE g_hInstance;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CCDPlayerCtrl::CCDPlayerCtrl()
{
   ZeroMemory(&m_OpenParams, sizeof(MCI_OPEN_PARMS));
   m_mcierror           = 0;
   ZeroMemory(&m_DeviceStatus, sizeof(CDStatus));
   m_DeviceStatus.Eject   = true;

   m_mmCurrentTrackTime.dwTime =0;
   m_mmCDTime.dwTime    = 0;
   m_mmTrackTime.dwTime = 0;
   m_hFontCDtime        = NULL;
   m_hOwner             = NULL;
   m_dwWaveTrack        = 0;
   m_Formats.ReadFormats();
}

CCDPlayerCtrl::~CCDPlayerCtrl()
{
   CloseDevice();
   DeleteCDFont();
}

void CCDPlayerCtrl::DeleteCDFont()
{
   if (m_hFontCDtime)
   {
      ::DeleteObject(m_hFontCDtime);
      m_hFontCDtime = NULL;
   }
}

bool CCDPlayerCtrl::PlayDevice(DWORD dwFrom, DWORD dwTo)
{
   if (m_OpenParams.wDeviceID)
   {
      MCI_PLAY_PARMS pp;
      long lUpdate  = 0;
      pp.dwCallback = (DWORD) m_hOwner;
      pp.dwFrom     = dwFrom;
      pp.dwTo       = dwTo;
      DWORD dwFlags = MCI_NOTIFY;
      bool bReturn  = false;
      if (dwFrom) dwFlags |= MCI_FROM;
      if (dwTo  ) dwFlags |= MCI_TO;
      if (m_DeviceStatus.WaveFiles) dwFlags |= MCI_FROM;
      if (m_DeviceStatus.RecordingDev)
      {
         bReturn = RecordDevice(dwFrom);
      }
      else
      {
         m_mcierror = mciSendCommand(m_OpenParams.wDeviceID, MCI_PLAY, dwFlags, (DWORD)&pp);
         bReturn    = ReportDeviceError();                      // Fehler aufgetreten ?
      }
      if (bReturn)
      {
         DWORD dwTrack = GetCurrentTrack();
         if ((dwTrack < 1) || (dwTrack > (DWORD)GetTracks())) dwTrack = 1;
         DWORD dwReturn = 0;
         if (GetDeviceStatus(MCI_STATUS_ITEM, MCI_STATUS_CURRENT_TRACK, dwReturn, dwTrack) &&
             GetCurrentPosition() && GetTrackLength(dwTrack))
         {
            if ((m_hOwner) && !m_DeviceStatus.WaveFiles && (dwTrack != dwReturn)) ::SendMessage(m_hOwner, WM_UPDATE_CD_CTRLS, SET_CURRENT_TRACK, dwReturn);
         }
         if (!m_DeviceStatus.Playes)
         {
            StartPollCDPosition();
            m_DeviceStatus.Playes = true;                      // Wenn nicht, ist Play gedrückt und Pause Enabled
            lUpdate |= (UPDATE_START_BTN|UPDATE_STOP_BTN);
         }
      }
      else
      {
         m_DeviceStatus.Playes = false;                       // Wenn ja, ist Play nicht gedrückt
         lUpdate |= (UPDATE_START_BTN|UPDATE_STOP_BTN);
      }
      if (m_DeviceStatus.Paused) lUpdate |= UPDATE_PAUSE_BTN;
      m_DeviceStatus.Paused = false;

      lUpdate |= UPDATE_CURR_TIME;
      if (dwTo||dwFrom) lUpdate |= UPDATE_DIRECT_BTN;
      UpdateCDCtrls(lUpdate);
      return bReturn;
   }
   return false;
}

bool CCDPlayerCtrl::RecordDevice(DWORD dwFrom, DWORD dwTo)
{
   bool bReturn = false;
   if (m_OpenParams.wDeviceID)
   {
      MCI_RECORD_PARMS pp;
      pp.dwCallback = (DWORD) m_hOwner;
      pp.dwFrom     = dwFrom;
      pp.dwTo       = dwTo;
      DWORD dwFlags = MCI_NOTIFY|MCI_RECORD_OVERWRITE;
      if (dwFrom) dwFlags |= MCI_FROM;
      if (dwTo  ) dwFlags |= MCI_TO;
      m_mcierror    = mciSendCommand(m_OpenParams.wDeviceID, MCI_RECORD, dwFlags, (DWORD)&pp);
      bReturn  = ReportDeviceError();                      // Fehler aufgetreten ?
      if (bReturn)
      {
         m_DeviceStatus.Records = true;
      }
   }
   return bReturn;
}

bool CCDPlayerCtrl::SaveFile(char *pszFilename)
{
  if (IsOpen())
  {
     MCI_SAVE_PARMS sp;
     sp.dwCallback = (DWORD) m_hOwner;
     if (pszFilename) sp.lpfilename = pszFilename;
     else if (m_OpenParams.lpstrElementName)
     {
         sp.lpfilename = m_OpenParams.lpstrElementName;
     }
     else return false;
     m_mcierror = mciSendCommand(m_OpenParams.wDeviceID, MCI_SAVE, MCI_SAVE_FILE, (DWORD)&sp);
     return ReportDeviceError();
  }
  return false;
}

bool CCDPlayerCtrl::ReportDeviceError()
{
   if (m_mcierror)
   {
      char lpszErrorText[MAX_PATH];
      mciGetErrorString(m_mcierror, lpszErrorText, MAX_PATH);
      if (m_hOwner) ::SendMessage(m_hOwner, WM_UPDATE_CD_CTRLS, REPORT_ERROR, (LPARAM)lpszErrorText);
      REPORT("%s\n", lpszErrorText);
      m_mcierror = 0;
      return false;
   }
   return true;
}

void CCDPlayerCtrl::CloseDevice()
{
   if (m_OpenParams.wDeviceID)
   {
      MCI_GENERIC_PARMS gp;
      gp.dwCallback = 0;//(DWORD)m_hOwner;
      m_mcierror = mciSendCommand(m_OpenParams.wDeviceID, MCI_CLOSE, MCI_WAIT, (DWORD) &gp);
      ReportDeviceError();
      m_OpenParams.wDeviceID = 0;
      if (m_hOwner)
      {
         ::SendMessage(m_hOwner, WM_UPDATE_CD_CTRLS, STOP_POLLCDSTATUS, 0);
         ::SendMessage(m_hOwner, WM_UPDATE_CD_CTRLS, DISABLE_CTRLS, 0);
      }
   }
   if (m_OpenParams.lpstrElementName)
   {
      free((void*)m_OpenParams.lpstrElementName);
      m_OpenParams.lpstrElementName = NULL;
   }
   if (m_OpenParams.lpstrDeviceType)
   {
      free((void*)m_OpenParams.lpstrDeviceType);
      m_OpenParams.lpstrDeviceType = NULL;
   }
   m_mmCDTime.dwTime           = 0;
   m_mmCurrentTrackTime.dwTime = 0;
   m_mmTrackTime.dwTime        = 0;
   m_dwWaveTrack               = 0;
}

bool CCDPlayerCtrl::GetDeviceInfo(DWORD dwFlags, char *pszString, DWORD&dwSize)
{
   if (m_OpenParams.wDeviceID == 0) return false;
   MCI_INFO_PARMS ip;
   ip.dwCallback  = 0;
   ip.lpstrReturn = pszString; 
   ip.dwRetSize   = dwSize;
   m_mcierror     = mciSendCommand(m_OpenParams.wDeviceID, MCI_INFO, MCI_WAIT|dwFlags, (DWORD)&ip);
   dwSize         = ip.dwRetSize;
   return ReportDeviceError();
}

bool CCDPlayerCtrl::GetDeviceStatus(DWORD dwFlags, DWORD dwItem, DWORD &dwReturn, DWORD &dwTrack)
{
   if (m_OpenParams.wDeviceID == 0) return false;
   MCI_STATUS_PARMS sp;
   sp.dwCallback = 0;
   sp.dwReturn   = dwReturn;
   sp.dwItem     = dwItem;
   sp.dwTrack    = dwTrack;
   if (m_DeviceStatus.WaveFiles) dwFlags &= ~MCI_TRACK;
   m_mcierror    = mciSendCommand(m_OpenParams.wDeviceID, MCI_STATUS, MCI_WAIT|dwFlags, (DWORD)&sp);
   dwReturn      = sp.dwReturn;
   dwTrack       = sp.dwTrack;
   return ReportDeviceError();
}

void CCDPlayerCtrl::UpdateCDDisplay(long lUpdate)
{
   if (m_hOwner) ::SendMessage(m_hOwner, WM_UPDATE_CD_CTRLS, UPDATE_DISPLAY, lUpdate);
}

void CCDPlayerCtrl::GetCDStatus()
{
   DWORD dwTrack = 0, dwReturn = 0, dwTracks, dwCurrentTrack;
   CDStatus dwOldDeviceStatus = m_DeviceStatus;
/*                                                               // Flags, die hier gesetzt werden erstmal löschen
   if (GetDeviceStatusMode() && (m_DeviceStatus.IsOpen != dwOldDeviceStatus.IsOpen))
   {
      m_DeviceStatus.MediaPresent = false;
      m_DeviceStatus.MediaAudio   = false;
      m_DeviceStatus.MediaOther   = false;
      m_DeviceStatus.Playes       = false;
      m_DeviceStatus.Reading      = true;
      UpdateCDDisplay(UPDATE_IMEDIATE);
   }
*/
   m_DeviceStatus.Playes       = false;
   GetDeviceStatusMode();
   m_DeviceStatus.MediaPresent = false;
   m_DeviceStatus.MediaAudio   = false;
   m_DeviceStatus.MediaOther   = false;

   if (GetDeviceStatus(MCI_STATUS_ITEM, MCI_STATUS_MEDIA_PRESENT, dwReturn, dwTrack) && 
       (dwReturn != 0))                                        // Liegt ein Medium im Player
   {
      m_DeviceStatus.MediaPresent = true;                      // Medium vorhanden heißt immer
      m_DeviceStatus.Eject        = true;                      // beim nächsten Click öffnen
      dwTrack = 1;
      if (GetDeviceStatus(MCI_STATUS_ITEM|MCI_TRACK, MCI_CDA_STATUS_TYPE_TRACK, dwReturn, dwTrack))
      {
         if (dwReturn == MCI_CDA_TRACK_AUDIO) m_DeviceStatus.MediaAudio = true;
         if (dwReturn == MCI_CDA_TRACK_OTHER) m_DeviceStatus.MediaOther = true;

         if (GetDeviceStatus(MCI_STATUS_ITEM, MCI_STATUS_NUMBER_OF_TRACKS, dwTracks, dwTrack))
         {
            if (dwTracks != (DWORD)GetTracks())
            {
               GetDeviceStatus(MCI_STATUS_ITEM, MCI_STATUS_LENGTH, m_mmCDTime.dwTime, dwTrack);
               m_mmCDTime.msutTime.Track  = dwTracks;
               m_mmCDTime.MSUT2TMSF();

               GetDeviceStatus(MCI_STATUS_ITEM, MCI_STATUS_CURRENT_TRACK, dwCurrentTrack, dwTrack);
               GetTrackLength(dwTrack);
               m_mmTrackTime.msutTime.Track = dwCurrentTrack;
               m_mmTrackTime.MSUT2TMSF();

               m_mmCurrentTrackTime.dwTime = MCI_MAKE_TMSF(dwCurrentTrack, 0, 0, 0);
               if (m_hOwner) 
               {
                  ::SendMessage(m_hOwner, WM_UPDATE_CD_CTRLS, NEW_CD_MEDIA, 1);
                  ::SendMessage(m_hOwner, WM_UPDATE_CD_CTRLS, NEW_CD_TRACK, dwCurrentTrack);
               }
            }
         }
      }
   }
   else
   {
      m_mmCDTime.dwTime = 0;
      ::SendMessage(m_hOwner, WM_UPDATE_CD_CTRLS, DISABLE_CTRLS, 0);
   }

   m_DeviceStatus.Reading = false;

   long lUpdate = 0;
   if (m_DeviceStatus.Reading      != dwOldDeviceStatus.Reading)      lUpdate |= UPDATE_DISPLAY_F;
   if (m_DeviceStatus.Playes       != dwOldDeviceStatus.Playes)       lUpdate |= (UPDATE_START_BTN|UPDATE_STOP_BTN);
   if (m_DeviceStatus.Paused       != dwOldDeviceStatus.Paused)       lUpdate |= UPDATE_PAUSE_BTN;
   if (m_DeviceStatus.Eject        != dwOldDeviceStatus.Eject)        lUpdate |= UPDATE_EJECT_BTN;
   if (m_DeviceStatus.MediaPresent != dwOldDeviceStatus.MediaPresent) lUpdate |= UPDATE_ALL;

   UpdateCDCtrls(lUpdate);
}

void CCDPlayerCtrl::StartPollCDPosition()
{
   if (!m_DeviceStatus.PollPosition)
   {
      if (m_hOwner) ::SendMessage(m_hOwner, WM_UPDATE_CD_CTRLS, START_POLLCDPOS, 0);
      m_DeviceStatus.PollPosition = true;
   }
}

void CCDPlayerCtrl::StopPollCDPosition()
{
   if (m_DeviceStatus.PollPosition)
   {
      if (m_hOwner) ::SendMessage(m_hOwner, WM_UPDATE_CD_CTRLS, STOP_POLLCDPOS, 0);
      m_DeviceStatus.PollPosition = false;
   }
}

void CCDPlayerCtrl::UpdateCDCtrls(long lUpdate)
{
   if (m_hOwner)
   {
      ::SendMessage(m_hOwner, WM_UPDATE_CD_CTRLS, UPDATE_CTRLS, lUpdate);
      if (lUpdate&UPDATE_DISPLAY_ALL) UpdateCDDisplay(lUpdate);
   }
}

bool CCDPlayerCtrl::OpenCDPlayer()
{
   DWORD dwFlags, dwSize = MAX_PATH;
   char  szInfoString[MAX_PATH];
   ResetDeviceStatus();
   dwFlags          = MCI_WAIT|MCI_OPEN_TYPE|MCI_OPEN_SHAREABLE;
   m_OpenParams.dwCallback = (DWORD) m_hOwner;
   m_OpenParams.lpstrDeviceType = _strdup("cdaudio");
   m_OpenParams.lpstrAlias      = (LPCSTR)"Rollo";

   if (m_OpenParams.lpstrElementName) dwFlags |= MCI_OPEN_ELEMENT;
   if (m_OpenParams.lpstrAlias)       dwFlags |= MCI_OPEN_ALIAS;

   m_mcierror = mciSendCommand(0, MCI_OPEN, dwFlags, (DWORD) &m_OpenParams);

   if ((m_hOwner != NULL) && (m_mcierror != 0))
   {
      m_OpenParams.wDeviceID = 0;
      ::SendMessage(m_hOwner, WM_UPDATE_CD_CTRLS, SET_FILE_FOLDER, (LPARAM)&dwFlags);
      if (dwFlags == 0)
      {
         CloseDevice();
         return false;
      }
      return true;
   }

   if (!ReportDeviceError())
   {
      CloseDevice();
      return false;
   }
   else if (SetDevice(MCI_SET_TIME_FORMAT, MCI_FORMAT_TMSF))
   {
      m_DeviceStatus.Open         = true;
      m_DeviceStatus.RecordingDev = false;
      GetDeviceCaps();

      if (GetDeviceInfo(MCI_INFO_PRODUCT, szInfoString, dwSize) && (dwSize > 0))
      {
         char szText[MAX_PATH];
         ::LoadString(g_hInstance, IDC_AC_CD_GRP_TEXT, szText, _MAX_PATH);
         if (m_OpenParams.lpstrElementName)
         {
            strcat(szText, m_OpenParams.lpstrElementName);
         }
         else strcat(szText, szInfoString);

         if (m_hOwner) ::SendMessage(m_hOwner, WM_UPDATE_CD_CTRLS, SET_INFO_STRING, (LPARAM)szText);
      }

      GetCDStatus();
      UpdateCDCtrls();
      if (!m_DeviceStatus.PollStatus)
      {
         if (m_hOwner) ::SendMessage(m_hOwner, WM_UPDATE_CD_CTRLS, START_POLLCDSTATUS, 0);
         m_DeviceStatus.PollStatus = true;
      }

      return ReportDeviceError();
   }
   return false;
}

bool CCDPlayerCtrl::OpenWaveDevice(LPCTSTR pszFilename, DWORD dwBufferSeconds)
{
   if (IsOpen()) return false;
   DWORD dwFlags, dwSize = MAX_PATH;
   char  szInfoString[MAX_PATH];
   MCI_WAVE_OPEN_PARMS wop;

   ResetDeviceStatus();
   dwFlags = MCI_WAIT|MCI_OPEN_TYPE;

   wop.dwCallback       = (DWORD) m_hOwner;
   wop.wDeviceID        = 0;

   wop.lpstrDeviceType  = NULL;
   wop.lpstrAlias       = NULL;
   wop.dwBufferSeconds  = dwBufferSeconds; 

   if (pszFilename)
   {
      wop.lpstrElementName = _strdup(pszFilename);
      _strupr((char*)wop.lpstrElementName);
      int n = strlen(wop.lpstrElementName);
      n = m_Formats.m_Formats.FindElement(CFormatDlg::FindString, (void*)&wop.lpstrElementName[n-4]);
      if (n!=-1)
      {
         FormatStruct *pFS = (FormatStruct *)m_Formats.m_Formats.GetAt(n);
         if (pFS)
         {
            char *psz = strstr(pFS->szFormat, ", ");
            if (psz) wop.lpstrDeviceType = &psz[2];
         }
      }
   }
   else wop.lpstrElementName = NULL;

   if (wop.lpstrElementName) dwFlags |= MCI_OPEN_ELEMENT;
   if (wop.lpstrAlias)       dwFlags |= MCI_OPEN_ALIAS;
   if (wop.dwBufferSeconds)  dwFlags |= MCI_WAVE_OPEN_BUFFER;

   m_mcierror = mciSendCommand(0, MCI_OPEN, dwFlags, (DWORD) &wop);

   m_OpenParams.dwCallback       = wop.dwCallback;
   m_OpenParams.wDeviceID        = wop.wDeviceID;
   m_OpenParams.lpstrElementName = wop.lpstrElementName;
   m_OpenParams.lpstrDeviceType  = _strdup(wop.lpstrDeviceType);
   m_OpenParams.lpstrAlias       = wop.lpstrAlias;

   if (!ReportDeviceError())
   {
      CloseDevice();
      return false;
   }
   m_DeviceStatus.Open = true;
 
   GetDeviceCaps();
   GetDeviceInfo(MCI_INFO_PRODUCT, szInfoString, dwSize);
   dwSize = 0;

   m_DeviceStatus.MediaPresent = true;
   m_DeviceStatus.MediaAudio   = true;
   m_DeviceStatus.WaveFiles    = true;
   m_DeviceStatus.CanEject     = false;
   m_DeviceStatus.Eject        = true;

   SetDevice(MCI_SET_TIME_FORMAT, MCI_FORMAT_MILLISECONDS);
   GetTrackLength(1);

   if (m_hOwner) ::SendMessage(m_hOwner, WM_UPDATE_CD_CTRLS, SET_INFO_STRING, (LPARAM)szInfoString);

   return ReportDeviceError();
}

bool CCDPlayerCtrl::SetDevice(DWORD dwFlag, DWORD dwTimeformat, DWORD dwAudio)
{
   if (!m_OpenParams.wDeviceID) return false;
   if (m_mcierror == 0)
   {
      MCI_SET_PARMS sp;
      sp.dwCallback   = 0;// (DWORD)m_hOwner;
      sp.dwTimeFormat = dwTimeformat; 
      sp.dwAudio      = dwAudio;
      m_mcierror      = mciSendCommand(m_OpenParams.wDeviceID, MCI_SET, MCI_WAIT|dwFlag, (DWORD)&sp);
   }
   return ReportDeviceError();
}

bool CCDPlayerCtrl::SetAudio(DWORD dwTimeFormat, DWORD dwAudio, WORD wFormatTag, DWORD nSamplesPerSec, DWORD nAvgBytesPerSec, WORD nBlockAlign, WORD wBitsPerSample, WORD wInput, WORD wOutput, WORD nChannels)
{
   if (!m_OpenParams.wDeviceID) return false;
   if (m_mcierror == 0)
   {
      MCI_WAVE_SET_PARMS swp;
      DWORD dwFlags = 0;
      swp.dwCallback      = 0; 
      swp.dwTimeFormat    = dwTimeFormat;
      swp.dwAudio         = dwAudio;
      swp.wInput          = wInput;
      swp.wOutput         = wOutput;
      swp.wFormatTag      = wFormatTag;
      swp.nChannels       = nChannels;
      swp.nSamplesPerSec  = nSamplesPerSec;
      swp.nAvgBytesPerSec = nAvgBytesPerSec;
      swp.nBlockAlign     = nBlockAlign;
      swp.wBitsPerSample  = wBitsPerSample;
      if      (wInput  == INVALID_VALUE) dwFlags |= MCI_WAVE_SET_ANYINPUT;
      else if (wInput  !=             0) dwFlags |= MCI_WAVE_INPUT;
      if      (wOutput == INVALID_VALUE) dwFlags |= MCI_WAVE_SET_ANYOUTPUT;
      else if (wOutput !=             0) dwFlags |= MCI_WAVE_OUTPUT;
      if (dwTimeFormat)       dwFlags |= MCI_SET_TIME_FORMAT; 
      if (nAvgBytesPerSec)    dwFlags |= MCI_WAVE_SET_AVGBYTESPERSEC;
      if (wBitsPerSample)     dwFlags |= MCI_WAVE_SET_BITSPERSAMPLE;
      if (nBlockAlign)        dwFlags |= MCI_WAVE_SET_BLOCKALIGN;
      if (nChannels)          dwFlags |= MCI_WAVE_SET_CHANNELS;
      if (wFormatTag)         dwFlags |= MCI_WAVE_SET_FORMATTAG;
      if (nSamplesPerSec)     dwFlags |= MCI_WAVE_SET_SAMPLESPERSEC;
      swp.wReserved2      = 0;
      swp.wReserved3      = 0;
      swp.wReserved4      = 0;
      swp.wReserved5      = 0;
      m_mcierror      = mciSendCommand(m_OpenParams.wDeviceID, MCI_SET, dwFlags, (DWORD)&swp);
   }
   return ReportDeviceError();
}

BOOL CCDPlayerCtrl::GetDeviceCapability(DWORD dwFlags, DWORD dwItem, DWORD &dwReturn)
{
   if (m_OpenParams.wDeviceID)
   {
      MCI_GETDEVCAPS_PARMS gdcp;
      gdcp.dwCallback = 0;// (DWORD)m_hOwner;
      gdcp.dwReturn   = 0;
      gdcp.dwItem     = dwItem;
      m_mcierror      = mciSendCommand(m_OpenParams.wDeviceID, MCI_GETDEVCAPS, MCI_WAIT|dwFlags, (DWORD)&gdcp);
      dwReturn        = gdcp.dwReturn;
      return ReportDeviceError();
   }
   return false;
}

void CCDPlayerCtrl::GetDeviceCaps()
{
   DWORD dwReturn;
   if (GetDeviceCapability(MCI_GETDEVCAPS_ITEM, MCI_GETDEVCAPS_CAN_PLAY, dwReturn) && (dwReturn  != 0))
      m_DeviceStatus.CanPlay = true;
   if (GetDeviceCapability(MCI_GETDEVCAPS_ITEM, MCI_GETDEVCAPS_CAN_RECORD, dwReturn) && (dwReturn  != 0))
      m_DeviceStatus.CanRecord = true;
   if (GetDeviceCapability(MCI_GETDEVCAPS_ITEM, MCI_GETDEVCAPS_CAN_EJECT, dwReturn) && (dwReturn  != 0))
      m_DeviceStatus.CanEject = true;
   if (GetDeviceCapability(MCI_GETDEVCAPS_ITEM, MCI_GETDEVCAPS_CAN_SAVE, dwReturn) && (dwReturn  != 0))
      m_DeviceStatus.CanSave  = true;
   if (GetDeviceCapability(MCI_GETDEVCAPS_ITEM, MCI_GETDEVCAPS_USES_FILES, dwReturn) && (dwReturn  != 0))
      m_DeviceStatus.UsesFiles = true;
}

bool CCDPlayerCtrl::IsOpen()
{
   return (m_OpenParams.wDeviceID != 0) ? true : false;
}

void CCDPlayerCtrl::ClickPause()
{
   if (IsOpen())
   {
      if (m_DeviceStatus.Paused)
         ClickPlay();
      else
      {
         MCI_GENERIC_PARMS gp;
         gp.dwCallback = 0;// (DWORD)m_hOwner;
         m_mcierror = mciSendCommand(m_OpenParams.wDeviceID, MCI_PAUSE, MCI_WAIT, (DWORD) &gp);
         if (ReportDeviceError())
         {
            m_DeviceStatus.Paused = true;
            m_DeviceStatus.Playes = false;
         }
      }
   }
   UpdateCDCtrls(UPDATE_START_BTN|UPDATE_PAUSE_BTN);
}

void CCDPlayerCtrl::ClickPlay()
{
   if (m_DeviceStatus.Paused)
   {
      if (m_DeviceStatus.WaveFiles)
      {
         PlayDevice(m_mmCurrentTrackTime.GetWaveTime());
      }
      else
      {
         PlayDevice();
      }
   }
   else 
   {
      PlayDevice(MCI_MAKE_TMSF(GetCurrentTrack(), 0, 0, 0));
   }
}

void CCDPlayerCtrl::ClickStop()
{
   if (IsOpen())
   {
      if (m_DeviceStatus.Playes || m_DeviceStatus.Paused)
      {
         StopPollCDPosition();
         if (!m_DeviceStatus.WaveFiles)
         {
            m_mmCurrentTrackTime.dwTime = MCI_MAKE_TMSF(GetCurrentTrack(), 0, 0, 0);
         }
      }
      else
      {
         if (!m_DeviceStatus.WaveFiles)
            m_mmCurrentTrackTime.dwTime = MCI_MAKE_TMSF(1, 0, 0, 0);
         if (m_hOwner)
         {
            ::SendMessage(m_hOwner, WM_UPDATE_CD_CTRLS, NEW_CD_TRACK, (LPARAM)1);
            UpdateCDDisplay();
         }
         return;
      }
      MCI_GENERIC_PARMS gp;
      gp.dwCallback = 0;// m_hOwner;
      m_mcierror = mciSendCommand(m_OpenParams.wDeviceID, MCI_STOP, MCI_WAIT, (DWORD) &gp);
      ReportDeviceError();
      m_DeviceStatus.Playes = false;
      m_DeviceStatus.Paused = false;
      UpdateCDDisplay();
      UpdateCDCtrls(UPDATE_START_BTN|UPDATE_PAUSE_BTN|UPDATE_STOP_BTN);
   }
}

void CCDPlayerCtrl::ClickEject()
{
   m_DeviceStatus.Reading = true;
   UpdateCDDisplay(UPDATE_IMEDIATE);

   if (m_DeviceStatus.Eject)
   {
      if (SetDevice(MCI_SET_DOOR_OPEN))
      {
         m_DeviceStatus.Eject   = false;
      }
   }
   else                                                        // Schublade schließen
   {
      if (SetDevice(MCI_SET_DOOR_CLOSED))
      {
         m_DeviceStatus.Eject   = true;
      }
   }

   GetCDStatus();
}

void CCDPlayerCtrl::SelectDisplayColor()
{
   if (CEtsDialog::ChooseColorT("Diplay color", m_dwCDDiplayText, m_hOwner, g_hInstance))
   {
      UpdateCDDisplay();
      HKEY hKey = NULL;
      if (OpenRegistry(g_hInstance, KEY_WRITE, &hKey, REGKEY_CD_DISPLAY))
      {
         SetRegDWord(hKey, CD_DISPLAY_TEXT, m_dwCDDiplayText);
         EtsRegCloseKey(hKey);
      }
   }
}

void CCDPlayerCtrl::SelectDisplayBkColor()
{
   if (CEtsDialog::ChooseColorT("Diplay background color", m_dwCDDiplayBkGnd, m_hOwner, g_hInstance))
   {
      UpdateCDDisplay();
      HKEY hKey = NULL;
      if (OpenRegistry(g_hInstance, KEY_WRITE, &hKey, REGKEY_CD_DISPLAY))
      {
         SetRegDWord(hKey, CD_DISPLAY_BKGND, m_dwCDDiplayBkGnd);
         EtsRegCloseKey(hKey);
      }
   }
}

void CCDPlayerCtrl::SelectDisplayFont()
{
   CHOOSEFONT cf;
   LOGFONT lf;
   char  szStyle[_MAX_PATH] = "";
   ZeroMemory(szStyle, _MAX_PATH);
   ZeroMemory(&cf, sizeof(CHOOSEFONT));
   if (m_hFontCDtime) GetObject(m_hFontCDtime, sizeof(LOGFONT), &lf);

   cf.lStructSize = sizeof(CHOOSEFONT);
   cf.lpLogFont = &lf;
   cf.rgbColors = m_dwCDDiplayText;
   cf.Flags     = CF_INITTOLOGFONTSTRUCT|CF_USESTYLE|CF_EFFECTS|CF_SCREENFONTS;
   cf.lpszStyle = szStyle;
   cf.hwndOwner = m_hOwner;

   if (::ChooseFont(&cf))
   {
      if (m_hFontCDtime) ::DeleteObject(m_hFontCDtime);
      m_hFontCDtime = CreateFontIndirect(&lf);
      m_dwCDDiplayText = cf.rgbColors;

      HKEY hKey = NULL;
      if (OpenRegistry(g_hInstance, KEY_WRITE, &hKey, REGKEY_CD_DISPLAY))
      {
         SetRegBinary(hKey, CD_DISPLAY_FONT, (BYTE*)&lf, sizeof(LOGFONT));
         EtsRegCloseKey(hKey);
      }
      UpdateCDDisplay();
   }

   if (cf.hDC) ::ReleaseDC(NULL, cf.hDC);
}

void CCDPlayerCtrl::SelectDrive()
{
   char szPath[_MAX_PATH];
   char szTitle[_MAX_PATH];
   if (m_OpenParams.lpstrElementName) strcpy(szPath, m_OpenParams.lpstrElementName);

   ::LoadString(g_hInstance, IDS_SELECT_CD_DRIVE, szTitle, _MAX_PATH);
   if (GetFolderPath(szPath, NULL, 0, szTitle, m_hOwner))
   {
      if ((m_OpenParams.lpstrElementName != NULL) &&
          (strcmp(szPath, m_OpenParams.lpstrElementName) == 0))
          return;
      CloseDevice();
      m_OpenParams.lpstrElementName = _strdup(szPath);
      if (!OpenCDPlayer())
      {
         OpenCDPlayer();
      }
   }
}

void CCDPlayerCtrl::EditFormats()
{
   m_Formats.Init(g_hInstance, IDD_FORMATS, m_hOwner);
   m_Formats.DoModal();
}

void CCDPlayerCtrl::ChangeCurrentTrack(DWORD dwTrack)
{
   if (m_OpenParams.wDeviceID != 0)
   {
      DWORD nOldTrack = GetCurrentTrack();
      if (m_DeviceStatus.WaveFiles)
      {
         if (m_hOwner) ::SendMessage(m_hOwner, WM_UPDATE_CD_CTRLS, NEW_CD_TRACK, dwTrack);
         return;
      }
      if (m_DeviceStatus.Playes || m_DeviceStatus.Paused)
      {
         if (!m_DeviceStatus.CompareTitle)                     // wenn nicht verglichen wird
            m_mmCurrentTrackTime.dwTime = 0;                   // zeit auf 0 setzen

         m_mmCurrentTrackTime.tmsfTime.Track = dwTrack;        // Track übergeben

         PlayDevice(m_mmCurrentTrackTime.dwTime);
         GetTrackLength(dwTrack);
         UpdateCDCtrls(nOldTrack|UPDATE_DIRECT_BTN|UPDATE_DISPLAY_F);
      }
      else if (nOldTrack != dwTrack)
      {
         if (GetTrackLength(dwTrack))
         {
            UpdateCDCtrls(nOldTrack|UPDATE_DIRECT_BTN|UPDATE_DISPLAY_F);
            return;
         }
      }
      if (m_hOwner) ::SendMessage(m_hOwner, WM_UPDATE_CD_CTRLS, NEW_CD_TRACK, dwTrack);
   }
}

void CCDPlayerCtrl::ClickPrev()
{
   if (m_OpenParams.wDeviceID != 0)
   {
      DWORD nTrack  = GetCurrentTrack();
      if (nTrack > 1)
      {
         nTrack--;
         ChangeCurrentTrack(nTrack);
      }
   }
}

void CCDPlayerCtrl::ClickNext()
{
   if (m_OpenParams.wDeviceID != 0)
   {
      DWORD nTrack  = GetCurrentTrack();
      if (nTrack < (DWORD)GetTracks())
      {
         nTrack++;
         ChangeCurrentTrack(nTrack);
      }
   }
}

void CCDPlayerCtrl::PollCDPosition()
{
   if (m_OpenParams.wDeviceID != 0)
   {
      MMTime mmT(0, 0, 10, 0);                                 // 10 Sekunden
      bool   bPressed      = false;
      bool   bTrackChanged = false;
      DWORD  nTrack        = GetCurrentTrack();
      if (m_DeviceStatus.ButtonPrev)
      {
         bPressed = true;
         m_mmCurrentTrackTime.Sub(m_mmCurrentTrackTime, mmT);
         if (m_mmCurrentTrackTime.tmsfTime.Minute == 255)
         {
            nTrack--;
            bTrackChanged = true;
         }
      }
      else if (m_DeviceStatus.ButtonNext)
      {
         bPressed = true;
         m_mmCurrentTrackTime.Add(m_mmCurrentTrackTime, mmT);
         if (m_mmCurrentTrackTime > m_mmTrackTime)
         {
            nTrack++;
            bTrackChanged = true;
         }
      }
      if (bPressed)
      {
         if (m_DeviceStatus.WaveFiles)
         {
            if (bTrackChanged) return;
            DWORD dwTime = m_mmCurrentTrackTime.GetWaveTime();
            PlayDevice(dwTime);
         }
         else
         {
            if (bTrackChanged)
            {
               if (m_DeviceStatus.StopAtTrackEnd) ClickStop();
               if ((nTrack > (DWORD)GetTracks()) || (nTrack < 1)) return;
               if (GetTrackLength(nTrack))
               {
                  if (m_DeviceStatus.ButtonPrev)               // Previous
                  {
                     MMTime mm(0,0,2,0);
                     m_mmCurrentTrackTime.Sub(m_mmTrackTime, mm);
                  }
                  else                                         // Next
                  {
                     m_mmCurrentTrackTime.tmsfTime.Minute = 0;
                     m_mmCurrentTrackTime.tmsfTime.Second = 0;
                  }
               }
               if (m_hOwner) ::SendMessage(m_hOwner, WM_UPDATE_CD_CTRLS, NEW_CD_TRACK, nTrack);
            }
            m_mmCurrentTrackTime.tmsfTime.Track = nTrack;
            PlayDevice(m_mmCurrentTrackTime.dwTime);
            return;
         }
      }
      if (m_DeviceStatus.WaveFiles)
      {
         if (GetCurrentPosition())
         {
//            m_mmCurrentTrackTime.Wave2TMSF();
            if (!(m_mmCurrentTrackTime < m_mmTrackTime))
            {
               if (!bTrackChanged) nTrack++;
               if (m_DeviceStatus.StopAtTrackEnd) ClickStop();
               else
               {
                  if (m_hOwner) ::SendMessage(m_hOwner, WM_UPDATE_CD_CTRLS, NEW_CD_TRACK, nTrack);
               }
            }
            long lUpdate = UPDATE_CURR_TIME;
            UpdateCDCtrls(lUpdate);
         }
      }
      else
      {
         DWORD dwCurrentTrack, dwTrack = 1;
         GetDeviceStatus(MCI_STATUS_ITEM, MCI_STATUS_CURRENT_TRACK, dwCurrentTrack, dwTrack);
         if (dwCurrentTrack != (DWORD)GetCurrentTrack())
         {
            GetTrackLength(dwCurrentTrack);
            bTrackChanged = true;
            if (m_DeviceStatus.StopAtTrackEnd) ClickStop();
         }
         if (GetCurrentPosition())
//         if (GetDeviceStatus(MCI_STATUS_ITEM, MCI_STATUS_POSITION, m_mmCurrentTrackTime.dwTime, nTrack))
         {
            long lUpdate = UPDATE_CURR_TIME;
            if (bTrackChanged)
            {
               if (m_hOwner) ::SendMessage(m_hOwner, WM_UPDATE_CD_CTRLS, SET_CURRENT_TRACK, dwCurrentTrack);
               lUpdate |= UPDATE_DIRECT_BTN;
            }
            UpdateCDCtrls(lUpdate);
         }
      }

   }
}

void CCDPlayerCtrl::PressNextBtn()
{
   m_DeviceStatus.ButtonNext = true;
}

void CCDPlayerCtrl::PressPrevBtn()
{
   m_DeviceStatus.ButtonPrev = true;
}

void CCDPlayerCtrl::ReleasePrevNextBtns()
{
   m_DeviceStatus.ButtonPrev = false;
   m_DeviceStatus.ButtonNext = false;
}


int CCDPlayerCtrl::GetTrackMinutes()
{
   return (int)m_mmTrackTime.tmsfTime.Minute;
}

int CCDPlayerCtrl::GetTrackSeconds()
{
   return (int)m_mmTrackTime.tmsfTime.Second;
}

int CCDPlayerCtrl::GetCurrentTrack()
{
   if (m_DeviceStatus.WaveFiles)
      return m_dwWaveTrack;
   else return (int)m_mmTrackTime.tmsfTime.Track;
}

int CCDPlayerCtrl::GetCurrentTrackMinute()
{
   return (int)m_mmCurrentTrackTime.tmsfTime.Minute;
}

int CCDPlayerCtrl::GetCurrentTrackSecond()
{
   return (int)m_mmCurrentTrackTime.tmsfTime.Second;
}

int CCDPlayerCtrl::GetCurrentTrackFrame()
{
   if (m_DeviceStatus.WaveFiles) return 0;
   else return (int)m_mmCurrentTrackTime.tmsfTime.Frame;
}

bool CCDPlayerCtrl::GetDeviceStatusMode()
{
   DWORD dwTrack = 1, dwReturn = 0;
   if (GetDeviceStatus(MCI_STATUS_ITEM|MCI_TRACK, MCI_STATUS_MODE, dwReturn, dwTrack))
   {
      m_DeviceStatus.IsOpen = false;
      switch (dwReturn)
      {
         case MCI_MODE_PAUSE:
            m_DeviceStatus.Paused = true;
            StopPollCDPosition();
            break;
         case MCI_MODE_PLAY:
            m_DeviceStatus.Playes = true;
            StartPollCDPosition();
            break;
         case MCI_MODE_STOP:
            if (!m_DeviceStatus.Paused)
               m_DeviceStatus.Playes = false;
            StopPollCDPosition();
            break;
         case MCI_MODE_OPEN:
            StopPollCDPosition();
            m_DeviceStatus.IsOpen = true;
            if (m_hOwner) ::SendMessage(m_hOwner, WM_UPDATE_CD_CTRLS, DISABLE_CTRLS, 0);
            break;
         case MCI_MODE_NOT_READY:
            return false;
      }
      return true;
   }
   return false;
}

bool CCDPlayerCtrl::GetTrackLength(DWORD dwTrack)
{
   bool bReturn = false;
   if (GetDeviceStatus(MCI_STATUS_ITEM|MCI_TRACK, MCI_STATUS_LENGTH, m_mmTrackTime.dwTime, dwTrack))
   {
      if (m_DeviceStatus.WaveFiles) m_mmTrackTime.Wave2TMSF();
      else                          m_mmTrackTime.MSUT2TMSF();
      m_mmTrackTime.tmsfTime.Track = dwTrack;

      bReturn = true;
   }

   return bReturn;
}

bool CCDPlayerCtrl::GetCurrentPosition()
{
   bool bReturn = false;
   DWORD dwReturn = 1;
   if (GetDeviceStatus(MCI_STATUS_ITEM, MCI_STATUS_POSITION, m_mmCurrentTrackTime.dwTime, dwReturn))
   {
      if (m_DeviceStatus.WaveFiles) m_mmCurrentTrackTime.Wave2TMSF();
      bReturn = true;
   }

   return bReturn;
}

void MMTime::Wave2TMSF()
{
   DWORD dwWaveTime = dwTime;                   // speichern
   dwTime = 0;                                  // auf 0 setzen
   int nSeconds = dwWaveTime / 1000;            // Wavedateizeit in 1/1000 sekunden
   if (nSeconds > 59)                           // mehr als 1 Minute
   {
      tmsfTime.Minute = nSeconds / 60;          // durch 60 teilen
      tmsfTime.Second = nSeconds % 60;
   }
   else                                         // weniger als eine Minute
   {
      tmsfTime.Minute = 0;
      tmsfTime.Second = nSeconds;
   }
}

void MMTime::MSUT2TMSF()
{
   MMTime mmT;
   mmT.dwTime = dwTime;
   dwTime = 0;
   tmsfTime.Track  = mmT.msutTime.Track;
   tmsfTime.Minute = mmT.msutTime.Minute;
   tmsfTime.Second = mmT.msutTime.Second;
}

void MMTime::TMSF2HMS()
{
   MMTime mmT;
   mmT.dwTime = dwTime;
   dwTime = 0;
   if (mmT.tmsfTime.Minute >= 60)
   {
      hmsTime.Hour   = mmT.tmsfTime.Minute / 60;
      hmsTime.Minute = mmT.tmsfTime.Minute % 60;
   }
   else
   {
      hmsTime.Minute = mmT.tmsfTime.Minute;
   }
   hmsTime.Second = mmT.tmsfTime.Second;
   hmsTime.Track  = mmT.tmsfTime.Track;
}

void MMTime::Add(MMTime& mmT1, MMTime& mmT2)
{
   MMTime mmT;
   mmT.dwTime = 0;
   mmT.tmsfTime.Second = mmT1.tmsfTime.Second + mmT2.tmsfTime.Second;
   if (mmT.tmsfTime.Second>59)
   {
      mmT.tmsfTime.Second -= 60;
      mmT.tmsfTime.Minute++;
   }
   mmT.tmsfTime.Minute += mmT1.tmsfTime.Minute + mmT2.tmsfTime.Minute;
   dwTime = mmT.dwTime;
}

void MMTime::Sub(MMTime& mmT1, MMTime& mmT2)
{
   MMTime mmT;
   mmT.dwTime = 0;
   int nSecond = mmT1.tmsfTime.Second - mmT2.tmsfTime.Second;
   mmT.tmsfTime.Minute = mmT1.tmsfTime.Minute - mmT2.tmsfTime.Minute;

   if (nSecond<0)
   {
      mmT.tmsfTime.Second = nSecond + 60;
      mmT.tmsfTime.Minute--;
   }
   else mmT.tmsfTime.Second = nSecond;

   dwTime = mmT.dwTime;
}

DWORD MMTime::GetWaveTime()
{
   return (tmsfTime.Minute * 60 + tmsfTime.Second) * 1000;
}

void MMTime::FormatTHMS(char*pszFormat, char*pszText)
{
   wsprintf(pszText, pszFormat, hmsTime.Track, hmsTime.Hour, hmsTime.Minute, hmsTime.Second);
}

void MMTime::FormatMS(char*pszFormat, char*pszText)
{
   wsprintf(pszText, pszFormat, tmsfTime.Minute, tmsfTime.Second);
}

void CCDPlayerCtrl::ResetDeviceStatus()
{
   m_DeviceStatus.Open         = 0;
   m_DeviceStatus.MediaPresent = 0;
   m_DeviceStatus.MediaAudio   = 0;
   m_DeviceStatus.MediaOther   = 0;
   m_DeviceStatus.CanPlay      = 0;
   m_DeviceStatus.CanRecord    = 0;
   m_DeviceStatus.CanEject     = 0;
   m_DeviceStatus.CanSave      = 0;
   m_DeviceStatus.UsesFiles    = 0;
   m_DeviceStatus.Records      = 0;
   m_DeviceStatus.WaveFiles    = 0;
   m_DeviceStatus.Paused       = 0;
   m_DeviceStatus.Playes       = 0;
   m_DeviceStatus.PollPosition = 0;
   m_DeviceStatus.PollStatus   = 0;
}
