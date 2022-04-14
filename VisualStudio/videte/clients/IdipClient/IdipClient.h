// IdipClient.h : Hauptheaderdatei für die IdipClient-Anwendung
//
#pragma once

#ifndef __AFXWIN_H__
	#error 'stdafx.h' muss vor dieser Datei in PCH eingeschlossen werden.
#endif

#include "resource.h"       // Hauptsymbole

// CIdipClientApp:
// Siehe IdipClient.cpp für die Implementierung dieser Klasse
//
#define COUNTDOWN_MS	500

#define ID_CONNECT_LOW	0x0E00
#define ID_CONNECT_HIGH	0x0EFF

#define ID_DISCONNECT_LOW	0x0F00
#define ID_DISCONNECT_HIGH	0x0FFF

#define ID_SEARCH_LOW	0x1000
#define ID_SEARCH_HIGH	0x10FF

//#define ID_CHANGE_VIDEO			0x7000	// WM_COMMAND replaced by WM_CHANGE_VIDEO
//#define ID_DECODE_VIDEO			0x7001	// WM_COMMAND replaced by WM_DECODE_VIDEO
#define ID_SEARCH_RESULT			0x7002
#define ID_SEARCH_CANCELED			0x7003
#define ID_SEARCH_FINISHED			0x7004
#define ID_ARCHIV_INFO				0x7005
#define ID_BACKUP_CONFIRM			0x7006	// WM_COMMAND replaced by WM_USER
#define ID_BACKUP_CANCEL_CONFIRM	0x7007	// WM_COMMAND replaced by WM_USER
#define ID_BACKUP_ERROR				0x7008	// WM_COMMAND replaced by WM_USER
#define ID_CAMERA_NAMES				0x7009
#define ID_SEARCH_RESULT_COPY		0x700A
//#define ID_REDRAW_VIDEO			0x700B	// WM_COMMAND replaced by WM_REDRAW_VIDEO
#define ID_DRIVE_INFO				0x700D	// WM_COMMAND replaced by WM_USER
//#define ID_CHANGE_AUDIO			0x700E	// WM_COMMAND replaced by WM_CHANGE_AUDIO
//#define ID_UPDATE_DIALOGS			0x700F	// WM_COMMAND replaced by WM_UPDATE_DIALOGS

/////////////////////////////////////////////////////////////////////////////
// Query defines to differentiate the queries
// Query IDs are yet increased by 8,
// so the lowest 3 Bit are available for identification
#define QUERY_SERVER		1
#define QUERY_SYNC_PLAY		2
#define QUERY_LIVE_TO_PLAY	3

#define SMALL_WINDOW_CLASS _T("SmallWindowClassIdipClient")
#define SECTION_RECHERCHE  _T("Recherche")

// Menu positions of main menu
// CAVEAT: If you change the order check for any 'DeleteMenu'
// because it may have side affects
#define MAIN_MENU_CONNECTION	0
#define MAIN_MENU_SEARCH		1
#define MAIN_MENU_FILE			2
//#define MAIN_MENU_EDIT			3 //TKR dieser Menüeintrag wurde gelöscht
#define MAIN_MENU_VIEW			3
#define MAIN_MENU_INPUT			4
#define MAIN_MENU_CAMERA		5
#define MAIN_MENU_RELAY			6
#define MAIN_MENU_AUDIO			7
#define MAIN_MENU_ARCHIVE		8
#define MAIN_MENU_SEQUENCE		9
#define MAIN_MENU_HELP			10

// Menu positions of contect menu Live
// CAVEAT: If you change the order check for any 'DeleteMenu'
// because it may have side affects
#define SUB_MENU_HOST				0
#define SUB_MENU_DETECTOR			1
#define SUB_MENU_CAMERA				2
#define SUB_MENU_RELAIS				3
#define SUB_MENU_SEQUENCER			4
#define SUB_MENU_PTZ				5
#define SUB_MENU_MICO				6
#define SUB_MENU_COCO				7
#define SUB_MENU_TASHA				8
#define SUB_MENU_PICTURES			9
#define SUB_MENU_MONITOR			10
#define SUB_MENU_HTML				11
#define SUB_MENU_AUDIO_PLAY			12
#define SUB_MENU_AUDIO_REC			13
#define SUB_MENU_CONNECT_HOST		14
#define SUB_MENU_AUDIO_NEW			15
#define SUB_MENU_LIVE_WINDOW		16
#define SUB_MENU_OBJECT_VIEWS		17
#define SUB_MENU_SETTINGS			18

// Menu positions of contect menu Archive
// CAVEAT: If you change the order check for any 'DeleteMenu'
// because it may have side affects
#define SUB_MENU_SERVER				0
#define SUB_MENU_ARCHIV				1
#define SUB_MENU_SEQUENCE			2
#define SUB_MENU_DATABASE_WND		3
#define SUB_MENU_SAFE_AS			4
#define SUB_MENU_DIB_WND			5
#define SUB_MENU_QUERY				6
#define SUB_MENU_DATE_TIME			7
#define SUB_MENU_PLAY_WINDOW		8

// m_bConnectToLocalServer States
// FALSE, DO_CONNECT_LOCAL, CONNECTED_1ST_TIME
#define DO_CONNECT_LOCAL	TRUE
#define CONNECTED_1ST_TIME	2

#define	RESET_TIME_OUT	12

#define NO_CAM -1
#define ALL_CAMS FALSE

#define CPU_USAGE_SHOW			0x00000001	// display performance level meter
#define CPU_USAGE_TRACE_CHANGE	0x00000010	// trace change of performance level
#define CPU_USAGE_LOG_AVG		0x00000100	// log average cpu usage value every 5 seconds
#define CPU_USAGE_LOG_VALUE		0x00001000	// log current cpu usage value every second
#define CPU_USAGE_LOG_AVG_VT	0x00010000	// log average video timeout every 2 seconds

#define TRACE_NETWORK_RQ_STATE	0x00000010

class CIPCServerControlIdipClient;

#include "QueryParameter.h"

typedef enum PlayStatus
{
	PS_PLAY_BACK	=	1,
	PS_PLAY_FORWARD	=	2,
	PS_STOP			=	3,
	PS_FAST_BACK	=	4,
	PS_FAST_FORWARD	=	5,
	PS_SKIP_BACK	= 	8,
	PS_SKIP_FORWARD	= 	9,
};
typedef enum RectPlayStatus
{
	PS_RECT_NAVIGATE_NEXT	=	1,
	PS_RECT_NAVIGATE_PREV	=	2,
	PS_RECT_STOP			=	3,
};

CString NameOfEnum(PlayStatus ps);

typedef LONG (WINAPI *PROCNTQSI)(UINT,PVOID,ULONG,PULONG);
#define MAX_PERFORMANCE_LEVEL		6	// defines array size
// the meaning is how much performance is needed
#define LOWEST_PERFORMANCE_LEVEL	MAX_PERFORMANCE_LEVEL
#define LOWER_PERFORMANCE_LEVEL		5
#define LOW_PERFORMANCE_LEVEL		4
#define MEDIUM_PERFORMANCE_LEVEL	3
#define INITIAL_PERFORMANCE_LEVEL	2	// initial performance for determination
#define HIGH_PERFORMANCE_LEVEL		INITIAL_PERFORMANCE_LEVEL
#define HIGH2_PERFORMANCE_LEVEL		1
#define HIGH3_PERFORMANCE_LEVEL		0

#ifdef _UNICODE
 #ifdef _DEBUG
  #define COMMON_DIRECT_SHOW_MODULE _T("CommonDirectShowDU.dll")
 #else
  #define COMMON_DIRECT_SHOW_MODULE _T("CommonDirectShowU.dll")
 #endif
 #define  GET_GUID_STRING "?GetGUIDString@@YAXAAV?$CStringT@GV?$StrTraitMFC_DLL@GV?$ChTraitsCRT@G@ATL@@@@@ATL@@PAU_GUID@@H@Z"
 #define  GET_ERROR_STRING "?GetErrorStrings@@YAXJAAV?$CStringT@GV?$StrTraitMFC_DLL@GV?$ChTraitsCRT@G@ATL@@@@@ATL@@0@Z"
#else
 #ifdef _DEBUG
  #define COMMON_DIRECT_SHOW_MODULE _T("CommonDirectShowD.dll")
 #else
  #define COMMON_DIRECT_SHOW_MODULE _T("CommonDirectShow.dll")
 #endif
 #define  GET_GUID_STRING "?GetGUIDString@@YAXAAVCString@@PAU_GUID@@H@Z"
 #define  GET_ERROR_STRING "?GetErrorStrings@@YAXJAAVCString@@0@Z"
#endif

class CMainFrame;
class CImpIDispatch;
class CDlgLogin;
class CKBInput;
class CIdipClientDoc;
class CServer;

class CIdipClientApp : 
	public CWinApp,			// Win Application Class
	public COsVersionInfo,	// OS version Info functions
	public CWinXPThemes		// XP Themes info
{
public:
	CIdipClientApp();
	virtual ~CIdipClientApp();

	// Überschreibungen
public:
	virtual BOOL InitInstance();
	virtual int  ExitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle(LONG lCount);	// Recherche
	virtual int DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt);

	// attributes
public:
	inline CString		GetDefaultURL() const;
	inline CString		GetMapURL() const;
	inline DWORD		GetCPUClockFrequency() const;
	inline DWORD		GetCPUusage() const;
	inline BOOL			ShowCPUusage() const;
	inline DWORD		GetAvgCPUusage();
	inline DWORD		GetAvgVideoTimeout();
	inline int			GetPerformanceLevel() const;
	inline int			GetMinPerformanceLevel() const;
	inline CString		GetStationName();
	inline CString		GetApplicationDir();
	inline BOOL			IsReceiver() const;
	inline BOOL			IsShellMode() const;
	inline BOOL			IsInMainThread() const;
	inline BOOL			IsDemo() const;
	inline BOOL			AllowBackup() const;
	inline BOOL			AllowHTMLmaps() const;
	inline BOOL			AllowMultiMonitor() const;
	inline BOOL			AllowTimers() const;
	inline BOOL			CanPlayRealtime() const;
	inline BOOL			IsResetting() const;
	inline CMainFrame*	GetMainFrame();
	CRect				GetPrintMargin(CDC*pDC) const;
	CIdipClientDoc*		GetDocument();
	CServer*			GetServer(WORD wID);
	inline const		CMapStringToString& GetFieldMap() const;// Recherche
	inline CQueryParameter& GetQueryParameter();				// Recherche
	CHostArray&			GetHosts();
	CSecID				GetNextHtmlWindowID();
	CString				GetMappedString(const CString& sKey);
	BOOL				IsValidUserLoggedIn();
	BOOL				IsUserAlarm();
	inline BOOL			IsReadOnlyModus() const;
	static int			GetIntValue(LPCTSTR sKey, int nDefault=0);
	BOOL				IsDataBasePresent(BOOL bDetermine=FALSE);

	// operations
public:
	void AlarmConnection();
	void OnErrorMessage();
	BOOL DeleteMap();
	void CheckHostMap();
	BOOL StartApplication(WK_ApplicationId wai, WORD wStartParam);
	BOOL InitFields();	// Recherche
	void LoginOK();
	void DoReset();
	void SetResetting();
	void Reset();
	void EndResetting();
	void AutoLogout();
	void CalcCPUusage();
	void CheckPerformanceLevel();
	void SetPerformanceLevel(short nLevel);
	void LoadSettings();
	void ReadOpenDirPath();
	void SaveSettings();
	void SaveOpenDirPath();
	void SaveUserSettings();
	void ResetAutoLogout();

protected:
	BOOL LoginAsSuperVisor();
	BOOL Login(LPCTSTR pszUser = NULL,LPCTSTR pszPassword = NULL);
	BOOL CheckUserPassword(CString& sUser, CString& sPassword);
	void RegisterWindowClass();
	void LoadUserSpecificSettings();
	BOOL SplitCommandLine(const CString sCommandLine, CStringArray& saParameters, LPCTSTR szParam);
	// Recherche
	void InitFieldMap();
	void InitField(CWK_Profile& wkp,const CString& sName, UINT nID);
	int	 GetInt(CWK_Profile&wkp, CWKDebugOptions*pDO, LPCTSTR sSection, LPCTSTR sKey, int nDefault);

protected:
// Implementierung
	// Generated message map functions
	//{{AFX_MSG(CWndLive)
	afx_msg void OnButtonSystem();
	afx_msg void OnNewLogin(UINT);
	afx_msg void OnOptions();
	afx_msg void OnUpdateOptions(CCmdUI* pCmdUI);
	afx_msg void OnViewTargetDisplay();
	afx_msg void OnUpdateViewTargetDisplay(CCmdUI* pCmdUI);
	afx_msg void OnLogview();
	afx_msg void OnUpdateLogview(CCmdUI* pCmdUI);
	afx_msg void OnProductview();
	afx_msg void OnUpdateProductview(CCmdUI* pCmdUI);
	afx_msg void OnExplorer();
	afx_msg void OnUpdateExplorer(CCmdUI* pCmdUI);
	afx_msg void OnShell();
	afx_msg void OnUpdateShell(CCmdUI* pCmdUI);
	afx_msg void OnRuntimeError();
	afx_msg void OnUpdateRuntimeError(CCmdUI* pCmdUI);
	// Recherche
	afx_msg void OnEditStation();
	afx_msg void OnFilePrintSetup();
	afx_msg void OnSearchAtStart();
	afx_msg void OnUpdateSearchAtStart(CCmdUI* pCmdUI);
	afx_msg void OnSequenceRealtime();
	afx_msg void OnUpdateSequenceRealtime(CCmdUI* pCmdUI);
	afx_msg void OnUpdateButtonSystem(CCmdUI *pCmdUI);
	afx_msg void OnUpdateNewLogin(CCmdUI *pCmdUI);
	afx_msg void OnAppExit();
	afx_msg void OnExitClient();
	afx_msg void OnUpdateAppExit(CCmdUI *pCmdUI);
	afx_msg void OnUserInterface();
	afx_msg void OnUpdateUserInterface(CCmdUI *pCmdUI);
	afx_msg void OnFilePageDlg();
	afx_msg void OnUpdateFilePageDlg(CCmdUI *pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// data member
public:
	// User / Permission
	CWK_Dongle*  m_pDongle;
	CUser*		 m_pUser;
	CPermission* m_pPermission;
	CDlgLogin*	m_pDlgLogin;
	// Misc.
	CAutoLogout		m_AutoLogout;
	CRect			m_MaxMegraRect;
	CString			m_sVersion; 
	CErrorMessages	m_ErrorMessages;
	DWORD			m_dwMonitorFlags;
	DWORD			m_dwInitialTuning;
	DWORD			m_dwSequenceIntervall;
	CWK_Dongle::OemCode m_OemCode;
	CKBInput*		m_pKBinput;
	DWORD			m_dwClosing;
	DWORD			m_dwIdleTickCount;
	BOOL			m_bIsDemo;
	BOOL			m_bShowMenuToolTips;
private:
	BOOL			m_bReadOnlyModus;
	BOOL			m_bShellModus;

	HBRUSH			m_hMainWndBrush;
	WORD			m_wHtmlWindowID;

	CString			m_sStationName;
	CRect			m_rcMargin;
	CHostArray		m_Hosts;
	BOOL			m_bDoReset;
public:
	// Recherche, WndPlay
	CString			m_sOemIni;
	CDllModule		m_ComDSDll;
	CString			m_sSyncTime;
	BOOL			m_bAllowBackup;
//	BOOL			m_bAllowMiCo;
//	BOOL			m_bAllowCoCo;
//	BOOL			m_bAllowPT;
//	BOOL			m_bBackupModus;
	BOOL			m_bSearchMaskAtStart;
	int				m_nMaxRequestedRecords;
	int				m_nMaxSentSamples;
	CMapStringToString	m_FieldMap;
	CQueryParameter	m_QueryParameter;
	BOOL			m_bCanPlayRealtime;
private:
	BOOL			m_bIsDatabasePresent;

public:
	// HTML Pages
	CImpIDispatch*	m_pDispOM;
	CString			m_sMapImagePath;
	// pathes
	CString			m_sOpenDirectory;
	int				m_nRemoveableDriveLetter;
private:
	CString			m_sApplicationDir;
	CString			m_sDefaultURL;
	CString			m_sMapURL;

	// performance
	DWORD			m_dwCPUClock;
    LARGE_INTEGER	m_liOldIdleTime;
    LARGE_INTEGER	m_liOldSystemTime;
	PROCNTQSI		NtQuerySystemInformation;
	CWK_Average<int>m_AvgCPUusage;
	WORD			m_wNoOfProcessors;
	volatile signed short m_nPerformanceLevel;
	WORD			m_wCPUusage;
	WORD			m_wUpperThreshold;
	WORD			m_wLowerThreshold;
	signed short	m_nMinPerformanceLevel;
	BOOL			m_bShowCPUusage;

public:
	// Flags
	BOOL		m_bRunAnyLinkUnit;

	BOOL		m_bCorrectBitrate;
	BOOL		m_bCoCoQuality;
//	BOOL		m_bCoCoFit;
//	BOOL		m_bCoCoSoft;
	BOOL		m_bConnectToLocalServer;
	BOOL		m_bRS232Transparent;

	BOOL		m_bAnimateAlarmConnections;
	BOOL		m_bAlarmAnimation;
	BOOL		m_bFirstCam;
	BOOL		m_b1PlusAlarm;
	BOOL		m_bFullscreenAlarm;
	BOOL		m_bTargetDisplay;
	BOOL		m_bDisableZoom;
	BOOL		m_bDisableSmall;
	BOOL		m_bCloseAlarmWindow;
	BOOL		m_bDisableRightClick;
	BOOL		m_bDisableDblClick;
	BOOL		m_bWarnIfAudioIsBlockedAtNo;
	BOOL		m_bDialogBarTop;
	BOOL		m_bObjectViewsLeft;
	BOOL		m_bShowMainMenu;
	BOOL		m_bShowTitleOfSmallWindows;
	BOOL		m_bShowFPS;
	BOOL		m_bUseMMtimer;
	BOOL		m_bUseGOPThreadAlways;
	BOOL		m_bRegardNetworkFrameRate;
	BOOL		m_bUseAvCodec;
	BOOL		m_bShowHostMapAtLoggin;
	BOOL		m_bShowCamereaMapAtConnection;
	DWORD		m_dwPictureBufferThreadFlags;
	BOOL		m_bKeepProportion;
	BOOL		m_bShowAlarmMenu;
	BOOL		m_bConfirmDisconnect;
	BOOL		m_bVerifyUserToCloseNote;
	int			m_nNoOfHighPerformanceWnd;

public:
	// Trace Flags
	BOOL		m_bTraceBitrate;
	BOOL		m_bTraceDelay;
	BOOL		m_bTraceSwitch;
	BOOL		m_bTraceImage;
	BOOL		m_bStatImage;
	BOOL		m_bTraceSequence;
	BOOL		m_bTraceGOPTimer;
	DWORD		m_dwTraceFlags;

	// global Cursors
	HCURSOR		m_hPanLeft;
	HCURSOR		m_hPanRight;
	HCURSOR		m_hTiltUp;
	HCURSOR		m_hTiltDown;
	HCURSOR		m_hZoomIn;
	HCURSOR		m_hZoomOut;
	HCURSOR		m_hArrow;
	HCURSOR		m_hLeftTop;
	HCURSOR		m_hRightTop;
	HCURSOR		m_hRightBottom;
	HCURSOR		m_hLeftBottom;
	afx_msg void OnSoftwareUpdate();
	afx_msg void OnUpdateSoftwareUpdate(CCmdUI *pCmdUI);
	afx_msg void OnExtensionCode();
	afx_msg void OnUpdateExtensionCode(CCmdUI *pCmdUI);
};
/////////////////////////////////////////////////////////////////////////////
extern CWK_Timer theTimer;
extern CIdipClientApp theApp;
extern AFX_DATA_IMPORT AUX_DATA afxData;
/////////////////////////////////////////////////////////////////////////////
inline CString CIdipClientApp::GetDefaultURL() const
{
	return m_sDefaultURL;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CIdipClientApp::IsReceiver() const
{
	return m_pDongle ? m_pDongle->IsReceiver() : FALSE;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CIdipClientApp::IsDemo() const
{
	return m_pDongle ? m_pDongle->IsDemo() : TRUE;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CIdipClientApp::AllowBackup() const
{
	return m_pDongle ? m_pDongle->RunCDRWriter() : FALSE;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CIdipClientApp::AllowHTMLmaps() const
{
	return    (IsWinXP() || IsWin2000() || IsVista()) 
		   && m_pDongle 
		   && m_pDongle->AllowHTMLmaps();
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CIdipClientApp::AllowMultiMonitor() const
{
	return m_pDongle ? m_pDongle->AllowMultiMonitor() : FALSE;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CIdipClientApp::AllowTimers() const
{
	return m_pDongle ? m_pDongle->AllowTimers() : FALSE;
}
/////////////////////////////////////////////////////////////////////////////
inline CString CIdipClientApp::GetMapURL() const
{
	return m_sMapURL;
}
/////////////////////////////////////////////////////////////////////////////
inline const CMapStringToString& CIdipClientApp::GetFieldMap() const
{
	return m_FieldMap;
}
/////////////////////////////////////////////////////////////////////////////
inline CMainFrame* CIdipClientApp::GetMainFrame()
{
	return (CMainFrame*)m_pMainWnd;
}
/////////////////////////////////////////////////////////////////////////////
inline CQueryParameter& CIdipClientApp::GetQueryParameter()
{
	return m_QueryParameter;
}
/////////////////////////////////////////////////////////////////////////////
inline DWORD CIdipClientApp::GetCPUClockFrequency() const
{
	return m_dwCPUClock;
}
/////////////////////////////////////////////////////////////////////////////
inline DWORD CIdipClientApp::GetCPUusage() const
{
	return (DWORD)m_wCPUusage;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CIdipClientApp::ShowCPUusage() const
{
	return m_bShowCPUusage;
}
/////////////////////////////////////////////////////////////////////////////
inline DWORD CIdipClientApp::GetAvgCPUusage()
{
	if (m_AvgCPUusage.GetActualValues())
	{
		return (DWORD)MulDiv(m_AvgCPUusage.GetSum(), 1, m_AvgCPUusage.GetActualValues());
	}
	return 0xffffffff;
}
/////////////////////////////////////////////////////////////////////////////
inline int CIdipClientApp::GetPerformanceLevel() const
{
	return (int)m_nPerformanceLevel;
}
/////////////////////////////////////////////////////////////////////////////
inline int CIdipClientApp::GetMinPerformanceLevel() const
{
	return (int)m_nMinPerformanceLevel;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CIdipClientApp::CanPlayRealtime() const
{
	return m_bCanPlayRealtime;
}
/////////////////////////////////////////////////////////////////////////////
// class:		CIdipClientApp
// function:	IsResetting()
// purpose:		indicates the reset period
// parameters:	none
// returns:		reset period in seconds	(BOOL)
//				zero, if the Application is not resetting
inline BOOL	CIdipClientApp::IsResetting() const
{
	return m_bDoReset;
}
/////////////////////////////////////////////////////////////////////////////
inline CString CIdipClientApp::GetStationName()
{
#ifdef _DTS
	return m_Hosts.GetLocalHost()->GetName();
#else
	return m_sStationName;
#endif
}
/////////////////////////////////////////////////////////////////////////////
inline CString CIdipClientApp::GetApplicationDir()
{
	return m_sApplicationDir;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CIdipClientApp::IsReadOnlyModus() const
{
	return m_bReadOnlyModus;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CIdipClientApp::IsShellMode() const
{
	return m_bShellModus;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CIdipClientApp::IsInMainThread() const
{
	return m_nThreadID == GetCurrentThreadId() ? TRUE : FALSE;
}
/////////////////////////////////////////////////////////////////////////////
