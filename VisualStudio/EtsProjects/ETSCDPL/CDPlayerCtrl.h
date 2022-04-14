// CDPlayerCtrl.h: Schnittstelle für die Klasse CCDPlayerCtrl.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CDPLAYERCTRL_H__B0A5DFC1_E877_11D4_87C5_0000B48B0FC3__INCLUDED_)
#define AFX_CDPLAYERCTRL_H__B0A5DFC1_E877_11D4_87C5_0000B48B0FC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FormatDlg.h"

struct TMSF_Time
{
   unsigned long Track  :8;   // 25
   unsigned long Minute :8;   // 17
   unsigned long Second :8;   // 9
   unsigned long Frame  :8;   // 1
};

struct MSUT_Time
{
   unsigned long Minute  :8;   // 25
   unsigned long Second  :8;   // 17
   unsigned long Unknown :8;   // 9
   unsigned long Track   :8;   // 1
};

struct HMS_Time
{
   unsigned long Track  :8;   // 1
   unsigned long Hour   :8;   // 9
   unsigned long Minute :8;   // 17
   unsigned long Second :8;   // 25
};

union MMTime
{
   MMTime() {dwTime = 0;};
   MMTime(int nT, int nM, int nS, int nF) 
   {
      tmsfTime.Track  = nT;
      tmsfTime.Minute = nM;
      tmsfTime.Second = nS;
      tmsfTime.Frame  = nF;
   };

   void Wave2TMSF();
   void MSUT2TMSF();
   void TMSF2HMS();
   void FormatTHMS(char*, char*);
   void FormatMS(char*, char*);
   void Add(MMTime&, MMTime&);
   void Sub(MMTime&, MMTime&);
   friend bool operator > (MMTime&, MMTime&);
   friend bool operator < (MMTime&, MMTime&);

   DWORD     dwTime;
   TMSF_Time tmsfTime;
   MSUT_Time msutTime;
   HMS_Time  hmsTime;
public:
	DWORD GetWaveTime();
};

struct CDStatus
{
   unsigned long Open         :1;   // 1
   unsigned long MediaPresent :1;   // 2
   unsigned long MediaAudio   :1;   // 3
   unsigned long MediaOther   :1;   // 4
   unsigned long CanPlay      :1;   // 5
   unsigned long CanRecord    :1;   // 6
   unsigned long CanEject     :1;   // 7
   unsigned long IsOpen       :1;   // 8
   unsigned long Paused       :1;   // 9
   unsigned long Playes       :1;   // 10
   unsigned long Eject        :1;   // 11
   unsigned long Reading      :1;   // 12
   unsigned long ErrorText    :1;   // 13
   unsigned long PollPosition :1;   // 14
   unsigned long PollStatus   :1;   // 15
   unsigned long ButtonPrev   :1;   // 16
   unsigned long ButtonNext   :1;   // 17
   unsigned long Dummy1       :4;   // 18
   unsigned long CanSave      :1;   // 22
   unsigned long UsesFiles    :1;   // 23
   unsigned long Records      :1;   // 24
   unsigned long WaveFiles    :1;   // 25
   unsigned long NoDisableOnClose:1;// 26
   unsigned long TimeOptions  :2;   // 27
   unsigned long CompareTitle :1;   // 29
   unsigned long StopAtTrackEnd:1;  // 30
   unsigned long RecordingDev :1;   // 31
   unsigned long Dummy2       :1;   // 32
};

#define CD_STATUS_CNT_TIMER      0x00F00000

#define WM_DRAW_CD_DISPLAY          WM_USER+1
#define WM_UPDATE_CD_CTRLS          WM_USER+2
   #define DISABLE_CTRLS            1
   #define UPDATE_CTRLS             2
   #define START_POLLCDPOS          3
   #define STOP_POLLCDPOS           4
   #define START_POLLCDSTATUS       5
   #define STOP_POLLCDSTATUS        6
   #define NEW_CD_MEDIA             8
   #define NEW_CD_TRACK             9
   #define UPDATE_DISPLAY           10
   #define SET_INFO_STRING          11
   #define REPORT_ERROR             12
   #define SET_CURRENT_TRACK        13
   #define SET_FILE_FOLDER          14
   #define GET_CURRENT_TRACK        15
   #define GET_MM_FILTERS           16

#define UPDATE_START_BTN            0x00000100
#define UPDATE_STOP_BTN             0x00000200
#define UPDATE_PAUSE_BTN            0x00000400
#define UPDATE_PREV_BTN             0x00000800
#define UPDATE_PREVC_BTN            0x00001000
#define UPDATE_NEXT_BTN             0x00002000
#define UPDATE_NEXTC_BTN            0x00004000
#define UPDATE_EJECT_BTN            0x00008000
#define UPDATE_DIRECT_BTN           0x00010000
#define UPDATE_DIRECT_MASK          0x000000FF
#define UPDATE_CURR_TIME            0x00100000
#define UPDATE_DISPLAY_F            0x00200000
#define UPDATE_IMEDIATE             0x00400000
#define UPDATE_DISPLAY_ALL          0x00f00000
#define UPDATE_ALL                  0xff2fffff

#define ID_WAIT_CD_TRACK_END        989
#define ID_WAIT_CD_ERROR_TIMER      992
#define ID_UPDATE_CD_CTRLS          993
#define ID_POLL_CD_POSITION         996
#define ID_POLL_CD_STATUS           998

class CCDPlayerCtrl  
{
public:
	void ResetDeviceStatus();
   CCDPlayerCtrl();
	virtual ~CCDPlayerCtrl();

	bool OpenCDPlayer();
	bool OpenWaveDevice(LPCTSTR pszFilename=NULL, DWORD dwBufferSeconds=0);
	void CloseDevice();
   void SelectDrive();
	void EditFormats();
   void DeleteCDFont();

	bool GetDeviceInfo(DWORD, char *, DWORD&);
	BOOL GetDeviceCapability(DWORD, DWORD, DWORD&);
	bool GetDeviceStatus(DWORD, DWORD, DWORD&, DWORD&);
	bool GetDeviceStatusMode();
	bool GetCurrentPosition();
	bool GetTrackLength(DWORD);
	bool SetDevice(DWORD dwFlag, DWORD dwTimeformat=0, DWORD dwAudio=0);
   bool PlayDevice(DWORD dwFrom=0, DWORD dwTo=0);
   bool RecordDevice(DWORD dwFrom=0, DWORD dwTo=0);
	bool SaveFile(char *pszFilename=NULL);
   void ChangeCurrentTrack(DWORD dwTrack);
	bool SetAudio(DWORD dwTimeFormat, DWORD dwAudio, WORD wFormatTag, DWORD nSamplesPerSec, DWORD nAvgBytesPerSec, WORD nBlockAlign, WORD wBitsPerSample, WORD wInput, WORD wOutput, WORD nChannels);

   void GetCDStatus();
	void PollCDPosition();

	void ClickPlay();
	void ClickStop();
	void ClickPause();
   void ClickEject();
	void ClickNext();
	void ClickPrev();
	bool IsOpen();

   int  GetMinutes()            {return (int)m_mmCDTime.tmsfTime.Minute;};
   int  GetSeconds()            {return (int)m_mmCDTime.tmsfTime.Second;};
   int  GetTracks()             {return (int)m_mmCDTime.tmsfTime.Track;};

   int  GetTrackMinutes();
   int  GetTrackSeconds();
   int  GetCurrentTrack();
   int  GetCurrentTrackMinute();
   int  GetCurrentTrackSecond();
   int  GetCurrentTrackFrame();

   bool ReportDeviceError();
   void UpdateCDCtrls(long lUpdate=UPDATE_ALL);
   void UpdateCDDisplay(long lUpdate=UPDATE_DISPLAY_F);

   void StartPollCDPosition();
   void StopPollCDPosition();
	void PressPrevBtn();
	void PressNextBtn();
	void ReleasePrevNextBtns();

	void SelectDisplayFont();
	void SelectDisplayBkColor();
	void SelectDisplayColor();

   MCI_OPEN_PARMS m_OpenParams;
   MCIERROR    m_mcierror;
   CDStatus    m_DeviceStatus;

   MMTime      m_mmCDTime;
   MMTime      m_mmTrackTime;
   MMTime      m_mmCurrentTrackTime;

   DWORD       m_dwWaveTrack;

   HFONT       m_hFontCDtime;
   COLORREF    m_dwCDDiplayBkGnd;
   COLORREF    m_dwCDDiplayText;

   HWND        m_hOwner;

   CFormatDlg  m_Formats;
private:
	void GetDeviceCaps();
};

inline bool operator>(MMTime& mmA, MMTime& mmB)
{
   int nA = mmA.tmsfTime.Minute*60+mmA.tmsfTime.Second;
   int nB = mmB.tmsfTime.Minute*60+mmB.tmsfTime.Second;
   return (nA > nB) ? true: false;
}
inline bool operator<(MMTime& mmA, MMTime& mmB)
{
   int nA = mmA.tmsfTime.Minute*60+mmA.tmsfTime.Second;
   int nB = mmB.tmsfTime.Minute*60+mmB.tmsfTime.Second;
   return (nA < nB) ? true: false;
}

#endif // !defined(AFX_CDPLAYERCTRL_H__B0A5DFC1_E877_11D4_87C5_0000B48B0FC3__INCLUDED_)
