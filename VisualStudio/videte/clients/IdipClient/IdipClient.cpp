// IdipClient.cpp : Definiert das Klassenverhalten für die Anwendung.
//

#include "stdafx.h"
#include ".\idipclient.h"
#include "MainFrm.h"

#include "WndMpeg4.h"

#include "IdipClientDoc.h"
#include "ViewIdipClient.h"
#include "ViewDialogBar.h"
#include "ObjectTree.h"
#include "ViewControllItems.h"
#include "DlgBackup.h"
#include "DlgBackupNew.h"

#include "custsite.h"
#include "DlgPTZVideo.h"
#include "DlgOptions.h"
#include "DlgStationName.h"
#include "DlgUserInterface.h"
#include "DlgBarIdipClient.h"
#include "ViewDlg.h"
#include "DlgExternalFrame.h"
#include "wkclasses\wk_utilities.h"
#include "oemgui\autorun.h"

#include <locale.h>

//#include <WK_New.inl>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////////////
#define VISION_SETTINGS	_T("Vision\\Settings")

#define	SHOWTITLE_OF_SW		_T("ShowSmallTitles")
#define	OBJECTVIEWSLEFT		_T("ObjectViewsLeft")
#define	DIALOGBARTOP		_T("DialogBarTop")
#define	KEEPPICTPROPORTION	_T("KeepPictProportion")
#define	SHOWMAINMENU		_T("ShowMainMenu")
#define	SHOWMENUTOOLTIPS	_T("ShowMenuToolTips")
#define	ANIMATEALARMCONNECTIONS	_T("AnimateAlarmConnections")
#define MPEG4DECODEMT		_T("Mpeg4DecodeMT")
#define MAPIMAGEPATH		_T("MapImagePath")
#define OPENDIRECTORY		_T("OpenDirectory")
#define SHOW_FIRSTCAM		_T("FirstCam")
#define SHOW_1PLUSALARM		_T("1PlusAlarm")
#define SHOW_FULLSCREENALARM _T("FullscreenAlarm")

#define TF_TRACE_BITRATE	_T("TraceBitrate") 
#define TF_TRACE_DELAY		_T("TraceDelay")
#define TF_TRACE_SWITCH		_T("TraceSwitch")
#define TF_TRACE_IMAGE		_T("TraceImage")
#define TF_STAT_IMAGE		_T("StatImage")
#define TF_TRACESEQUENCE	_T("TraceSequence")
#define TF_TRACEGOPTIMER	_T("TraceGOPTimer")

#define TF_CORRECT_BITRATE	_T("CorrectBitrate")
#define TF_INITIAL_TUNING	_T("InitialTuning")
#define TF_COCO_QUALITY		_T("CoCoQuality")
#define TF_COCO_FIT			_T("CoCoFit")
#define TF_COCO_SOFT		_T("CoCoSoft")
#define TF_SHOWCOCOFPS		_T("ShowCocoFPS")

#define UPPER_CPU_USAGE_THRESHOLD	_T("UpperCPUusageThreshold")
#define LOWER_CPU_USAGE_THRESHOLD	_T("LowerCPUusageThreshold")
#define MIN_PERFORMANCE_LEVEL		_T("MinPerformanceLevel")

#define WARN_IF_AUDIO_IS_BLOCKED_AT_NO _T("WarnIfAudioIsBlockedAtNo")
// Recherche
#define RECHERCHE		_T("Recherche")

#define MAX_SENT_SAMPLES      _T("MaxSentSamples")
#define MAX_REQUESTED_RECORDS _T("MaxRequestedRecords")
#define REALTIME              _T("Realtime")
#define TARGET_DISPLAY        _T("TargetDisplay")
#define SEARCH_MASK           _T("SearchMask")
#define DEV_MODE              _T("DevMode")
#define DEV_NAMES             _T("DevNames")
#define STATION_NAME          _T("StationName")
#define PRINTER_MARGINS       _T("PrinterMargins")
#define SYNC_TIME             _T("SyncTime")
#define DATA_BASE_CLIENT      _T("DataBaseClient")
#define USE_MM_TIMER		  _T("UseMMTimer")
#define USE_USE_GT_ALWAYS	  _T("UseGOPThreadAlways")
#define REGARDNETWORKFRAMERATE _T("UseNetworkFrameRate")
#define USE_AV_CODEC		  _T("UseAVCodec")
#define RUN_LIVE_WND_THREAD   _T("RunLiveWindowThread")
#define CLIENT_VERSION		  _T("ClientVersion")
#define BASE_DVRT_KEY         _T("software\\DVRT\\")
#define REGKEY_BASEDVRT_DEFINED  2
#define REGKEY_IDIPCLIENT_DEFINED 4

typedef enum _SYSTEM_INFORMATION_CLASS {
    SystemBasicInformation = 0,
    SystemPerformanceInformation = 2,
    SystemTimeInformation = 3,
} SYSTEM_INFORMATION_CLASS;

typedef struct
{
    LARGE_INTEGER liKeBootTime;
    LARGE_INTEGER liKeSystemTime;
    LARGE_INTEGER liExpTimeZoneBias;
    ULONG         uCurrentTimeZoneId;
    DWORD         dwReserved;
} SYSTEM_TIME_INFORMATION;

typedef struct
{
    LARGE_INTEGER   liIdleTime;
    DWORD           dwSpare[76];
} SYSTEM_PERFORMANCE_INFORMATION;

typedef struct
{
    DWORD   dwUnknown1;
    ULONG   uKeMaximumIncrement;
    ULONG   uPageSize;
    ULONG   uMmNumberOfPhysicalPages;
    ULONG   uMmLowestPhysicalPage;
    ULONG   uMmHighestPhysicalPage;
    ULONG   uAllocationGranularity;
    PVOID   pLowestUserAddress;
    PVOID   pMmHighestUserAddress;
    ULONG   uKeActiveProcessors;
    BYTE    bKeNumberProcessors;
    BYTE    bUnknown2;
    WORD    wUnknown3;
} SYSTEM_BASIC_INFORMATION;

#define Li2Double(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))

#define CASE_ENUM_RETURN_NAME(E)	case E: return _T(#E)

/////////////////////////////////////////////////////////////////////////////
CString NameOfEnum(PlayStatus ps)
{
	switch (ps)
	{
		CASE_ENUM_RETURN_NAME(PS_PLAY_BACK);
		CASE_ENUM_RETURN_NAME(PS_PLAY_FORWARD);
		CASE_ENUM_RETURN_NAME(PS_STOP);
		CASE_ENUM_RETURN_NAME(PS_FAST_BACK);
		CASE_ENUM_RETURN_NAME(PS_FAST_FORWARD);
		CASE_ENUM_RETURN_NAME(PS_SKIP_BACK);
		CASE_ENUM_RETURN_NAME(PS_SKIP_FORWARD);
	}
	return _T("");
}

/////////////////////////////////////////////////////////////////////////////
static _TCHAR szLoginTimeoutSec[] = _T("LoginTimeoutSec");
static _TCHAR szFile[] = _T("file://");
static _TCHAR szHttp[] = _T("http://");
// Recherche
static TCHAR BASED_CODE szFieldMap[]	= _T("FieldMap");
static TCHAR BASED_CODE szDontUse[]	= _T("dontuse");

// CIdipClientApp
/////////////////////////////////////////////////////////////////////////////
// Das einzige CIdipClientApp-Objekt
CIdipClientApp theApp;
CWK_Timer theTimer;
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CIdipClientApp, CWinApp)
	ON_COMMAND_RANGE(ID_NEW_LOGIN, ID_NEW_LOGIN, OnNewLogin)
	ON_COMMAND_RANGE(ID_FORCE_NEW_LOGIN, ID_FORCE_NEW_LOGIN, OnNewLogin)
	ON_COMMAND(ID_OPTIONS, OnOptions)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS, OnUpdateOptions)
	ON_COMMAND(ID_VIEW_TARGET_DISPLAY, OnViewTargetDisplay)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TARGET_DISPLAY, OnUpdateViewTargetDisplay)
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_LOG_VIEW, OnLogview)
	ON_UPDATE_COMMAND_UI(ID_LOG_VIEW, OnUpdateLogview)
	ON_COMMAND(ID_PRODUCT_VIEW, OnProductview)
	ON_UPDATE_COMMAND_UI(ID_PRODUCT_VIEW, OnUpdateProductview)
	ON_COMMAND(ID_EXPLORER , OnExplorer)
	ON_UPDATE_COMMAND_UI(ID_EXPLORER , OnUpdateExplorer)
	ON_COMMAND(ID_SHELL , OnShell)
	ON_COMMAND(ID_NON_SHELL , OnShell)
	ON_UPDATE_COMMAND_UI(ID_SHELL , OnUpdateShell)
	ON_UPDATE_COMMAND_UI(ID_NON_SHELL , OnUpdateShell)
	ON_COMMAND(ID_ERROR , OnRuntimeError)
	ON_UPDATE_COMMAND_UI(ID_ERROR , OnUpdateRuntimeError)
// Recherche
	ON_COMMAND(ID_EDIT_STATION, OnEditStation)
	ON_COMMAND(ID_FILE_PRINT_SETUP, OnFilePrintSetup)
	ON_COMMAND(ID_SEARCH_AT_START, OnSearchAtStart)
	ON_UPDATE_COMMAND_UI(ID_SEARCH_AT_START, OnUpdateSearchAtStart)
	ON_COMMAND(ID_BUTTON_SYSTEM, OnButtonSystem)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_SYSTEM, OnUpdateButtonSystem)
	ON_UPDATE_COMMAND_UI(ID_NEW_LOGIN, OnUpdateNewLogin)
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_UPDATE_COMMAND_UI(ID_APP_EXIT, OnUpdateAppExit)
	ON_COMMAND(ID_EXIT_CLIENT, OnExitClient)
	ON_UPDATE_COMMAND_UI(ID_EXIT_CLIENT, OnUpdateAppExit)
	ON_COMMAND(ID_SEQUENCE_REALTIME, OnSequenceRealtime)
	ON_UPDATE_COMMAND_UI(ID_SEQUENCE_REALTIME, OnUpdateSequenceRealtime)
	ON_COMMAND(ID_USER_INTERFACE, OnUserInterface)
	ON_UPDATE_COMMAND_UI(ID_USER_INTERFACE, OnUpdateUserInterface)
	ON_COMMAND(ID_FILE_PAGE_SETUP, OnFilePageDlg)
	ON_UPDATE_COMMAND_UI(ID_FILE_PAGE_SETUP, OnUpdateFilePageDlg)
	ON_COMMAND(ID_SOFTWARE_UPDATE, OnSoftwareUpdate)
	ON_UPDATE_COMMAND_UI(ID_SOFTWARE_UPDATE, OnUpdateSoftwareUpdate)
	ON_COMMAND(ID_EXTENSION_CODE, OnExtensionCode)
	ON_UPDATE_COMMAND_UI(ID_EXTENSION_CODE, OnUpdateExtensionCode)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE

#define HKCU_INTERNATIONAL_SECTION	_T("Control Panel\\International")
#define INTERNATIONAL_LOCALE	_T("Locale")
#define INTERNATIONAL_NUMSHAPE	_T("NumShape")
BOOL CALLBACK EnumLocalesProc(LPTSTR lpLocaleString)
{
	CString sLangAbb = COemGuiApi::GetLanguageAbbreviation();
	_TCHAR szLangAbb[32];
	LCID lcID;
	_stscanf(lpLocaleString, _T("%x"), &lcID);
	GetLocaleInfo(lcID, LOCALE_SABBREVLANGNAME, szLangAbb, 32);
	if (sLangAbb.CompareNoCase(szLangAbb) == 0)
	{
		struct {
			LPCTSTR sRegKey;
			LCTYPE  lcType;
		} International[] = 
		{
			{ _T("sCountry"), LOCALE_SCOUNTRY },
			{ _T("iTime"), LOCALE_ITIME},
			{ _T("iCountry"), LOCALE_ICOUNTRY },
			{ _T("iCurrDigits"), LOCALE_ICURRDIGITS },
			{ _T("iCurrency"), LOCALE_ICURRENCY },
			{ _T("iDate"), LOCALE_IDATE },
			{ _T("iDigits"), LOCALE_IDIGITS },
			{ _T("iLZero"), LOCALE_ILZERO },
			{ _T("iMeasure"), LOCALE_IMEASURE },
			{ _T("iNegCurr"), LOCALE_INEGCURR },
			{ _T("iTLZero"), LOCALE_ITLZERO },
			{ _T("iTimePrefix"), LOCALE_ITIMEMARKPOSN },
			{ _T("iNegNumber"), LOCALE_INEGNUMBER },
			{ _T("iCalendarType"), LOCALE_ICALENDARTYPE },
			{ _T("iFirstDayOfWeek"), LOCALE_IFIRSTDAYOFWEEK },
			{ _T("iFirstWeekOfYear"), LOCALE_IFIRSTWEEKOFYEAR },
			{ _T("sTime"), LOCALE_STIME },
			{ _T("s1159"), LOCALE_S1159},
			{ _T("s2359"), LOCALE_S2359},
			{ _T("sLanguage"), LOCALE_SABBREVLANGNAME},
			{ _T("sCountry"), LOCALE_SCOUNTRY},
			{ _T("sCurrency"), LOCALE_SCURRENCY},
			{ _T("sDate"), LOCALE_SDATE },
			{ _T("sDecimal"), LOCALE_SDECIMAL },
			{ _T("sList"), LOCALE_SLIST },
			{ _T("sLongDate"), LOCALE_SLONGDATE},
			{ _T("sShortDate"), LOCALE_SSHORTDATE}, 
			{ _T("sThousand"), LOCALE_STHOUSAND },
			{ _T("sTimeFormat"), LOCALE_STIMEFORMAT },
			{ _T("sMonDecimalSep"), LOCALE_SMONDECIMALSEP },
			{ _T("sMonThousandSep"), LOCALE_SMONTHOUSANDSEP },
			{ _T("sNativeDigits"), LOCALE_SNATIVEDIGITS },
			{ _T("sGrouping"), LOCALE_SGROUPING },
			{ _T("sMonGrouping"), LOCALE_SMONGROUPING },
			{ _T("sPositiveSign"), LOCALE_SPOSITIVESIGN },
			{ _T("sNegativeSign"), LOCALE_SNEGATIVESIGN },
			{ NULL, 0}
		};
		DWORD   dw;
		HKEY    hSecKey = NULL;
		LONG    lResult = 0;
		int		i;
		CString str;
		lResult = RegCreateKeyEx(HKEY_CURRENT_USER, HKCU_INTERNATIONAL_SECTION, 0, REG_NONE,
										REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hSecKey, &dw);
		if (lResult == ERROR_SUCCESS)
		{
			_TCHAR szLCdata[128];
			for (i=0; International[i].sRegKey != NULL; i++)
			{
				GetLocaleInfo(lcID, International[i].lcType, szLCdata, 128);
				lResult = RegSetValueEx(hSecKey, International[i].sRegKey, NULL, REG_SZ, (BYTE*)szLCdata, (_tcslen(szLCdata)+1)*2);
				ASSERT(lResult == ERROR_SUCCESS);
				TRACE(_T("%s:%s\n"), International[i].sRegKey, szLCdata);
			}
			lResult = RegSetValueEx(hSecKey, INTERNATIONAL_LOCALE, NULL, REG_SZ, (BYTE*)lpLocaleString, (_tcslen(lpLocaleString)+1)*2);
			ASSERT(lResult == ERROR_SUCCESS);
		}
		if (hSecKey)
		{
			RegCloseKey(hSecKey);
		}
		return FALSE;
	}
	return TRUE;
}
#endif
/////////////////////////////////////////////////////////////////////////////
// CIdipClientApp-Erstellung
CIdipClientApp::CIdipClientApp() :
	  m_MaxMegraRect(0,0,710,560)
	, m_ComDSDll(COMMON_DIRECT_SHOW_MODULE
	)
{
	XTIB::SetThreadName(_T("MainThread"));

	// User / Permission
	m_pDongle = NULL;
	m_pUser = NULL;
	m_pPermission = NULL;
	m_pDlgLogin = NULL;

	// Misc.
	// m_AutoLogout
//	m_MaxMegraRect = 0;
	// msVersion
	//m_ErrorMessages
	m_dwMonitorFlags = USE_MONITOR_1;
	m_dwInitialTuning = 1000;
	m_dwSequenceIntervall = 5;
	m_dwClosing			= 0;
	m_dwIdleTickCount   = GetTickCount();

	// Recherche
	// m_sSyncTime;		// Recherche
	m_OemCode = CWK_Dongle::OEM_INVALID;
	m_bIsDemo = FALSE;
//	m_bAllowMiCo = TRUE;
//	m_bAllowCoCo = TRUE;
//	m_bAllowPT = TRUE;
	m_bAllowBackup = FALSE;
	m_bReadOnlyModus = FALSE;
	m_bShellModus = FALSE;
//	m_bBackupModus = FALSE;
	m_bSearchMaskAtStart = FALSE;
	m_nMaxRequestedRecords	= 5;
	m_nMaxSentSamples		= 3;
	// m_FieldMap;
	// m_QueryParameter;
	CWK_PerfMon perfMon(_T("Dummy"));
	m_dwCPUClock = perfMon.GetClockFrequency();

	m_wNoOfProcessors = 1;
	NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(GetModuleHandle(_T("ntdll")),"NtQuerySystemInformation");
	if (NtQuerySystemInformation)
	{
		SYSTEM_BASIC_INFORMATION       SysBaseInfo;
		// get number of processors in the system
		LONG status = NtQuerySystemInformation(SystemBasicInformation,&SysBaseInfo,sizeof(SysBaseInfo),NULL);
		if (status == NO_ERROR)
		{
			m_wNoOfProcessors = (WORD)SysBaseInfo.bKeNumberProcessors;
		}
	}

	m_wCPUusage = 0;
	m_wUpperThreshold = 85;
	m_wLowerThreshold = 70;
	m_nMinPerformanceLevel = 0;

	m_nPerformanceLevel	= m_nMinPerformanceLevel;
	m_AvgCPUusage.SetNoOfAvgValues(5);
	m_liOldIdleTime.LowPart = 0;
	m_liOldIdleTime.HighPart = 0;
	m_liOldSystemTime.LowPart = 0;
	m_liOldSystemTime.HighPart = 0;
	m_bShowCPUusage = FALSE;
	m_bRegardNetworkFrameRate = FALSE;
	m_bUseAvCodec			 = TRUE;
	m_dwPictureBufferThreadFlags = 0x00000011;

	m_bCanPlayRealtime = FALSE;

	// HTML Pages
	m_pDispOM = NULL;
	GetCurrentDirectory(MAX_PATH, m_sMapImagePath.GetBuffer(MAX_PATH));
	m_sMapImagePath.ReleaseBuffer();

	m_sOpenDirectory = _T("c:\\out");
	m_nRemoveableDriveLetter = 0;
	// m_sApplicationDir
	// m_sDefaultURL
	// m_sMapURL
	m_wHtmlWindowID = 0;

	// m_sStationName
	m_rcMargin = CRect(2000,2000,2000,2000);
	// m_sOemIni
	// m_Hosts

	// Flags
	m_bRunAnyLinkUnit = FALSE;
#ifdef _DTS
	m_bCorrectBitrate = FALSE;
#else
	m_bCorrectBitrate = TRUE;
#endif
	m_bCoCoQuality = FALSE;
//	m_bCoCoFit = FALSE;
//	m_bCoCoSoft = TRUE;
	m_bConnectToLocalServer = DO_CONNECT_LOCAL;
	m_bRS232Transparent = FALSE;
	m_bDoReset = FALSE;

	m_bAnimateAlarmConnections = FALSE;
	m_bAlarmAnimation = FALSE;
	m_bFirstCam = TRUE;
	m_b1PlusAlarm = FALSE;
	m_bFullscreenAlarm = FALSE;
	m_bTargetDisplay = TRUE;
	m_bDisableZoom = FALSE;
	m_bDisableSmall = FALSE;
	m_bCloseAlarmWindow = FALSE;
	m_bDisableRightClick =FALSE;
	m_bDisableDblClick =FALSE;
	m_bWarnIfAudioIsBlockedAtNo = 16;
	m_bShowFPS = FALSE;
	m_bUseMMtimer = TRUE;
	m_bUseGOPThreadAlways = TRUE;
	m_bShowHostMapAtLoggin = TRUE;
	m_bShowCamereaMapAtConnection = TRUE;
	m_bKeepProportion = TRUE;
	m_bConfirmDisconnect = FALSE;
	m_bVerifyUserToCloseNote = FALSE;


	// Trace Flags
	m_bTraceBitrate = FALSE;
	m_bTraceDelay = FALSE;
	m_bTraceSwitch = FALSE;
	m_bTraceImage = FALSE;
	m_bStatImage = FALSE;
	m_bTraceSequence = FALSE;
	m_bTraceGOPTimer = FALSE;
	m_dwTraceFlags = 0;
//	if (GetIntValue(_T("TraceCpuUsage")) m_dwTraceFlags |= FLAG_TRACE_CPU_USAGE;
//	if (GetIntValue(_T("TraceAvgCpuUsage")) m_dwTraceFlags |= FLAG_TRACE_AVG_CPU_USAGE;
//	if (GetIntValue(_T("TracePerformanceLevel")) m_dwTraceFlags |= FLAG_TRACE_PERFORMANCE_LEVEL;


	// global Cursors
	m_hPanLeft		= NULL;
	m_hPanRight		= NULL;
	m_hTiltUp		= NULL;
	m_hTiltDown		= NULL;
	m_hZoomIn		= NULL;
	m_hZoomOut		= NULL;
	m_hArrow		= NULL;
	m_hLeftTop		= NULL;
	m_hRightTop		= NULL;
	m_hRightBottom	= NULL;
	m_hLeftBottom	= NULL;
	
	m_pKBinput		= NULL;

	m_hMainWndBrush = NULL;

	m_bDialogBarTop    = FALSE;
	m_bObjectViewsLeft = FALSE;
	m_bShowMainMenu    = TRUE;
	m_bShowMenuToolTips = TRUE;
	m_bShowAlarmMenu	= TRUE;
	m_bShowTitleOfSmallWindows = TRUE;
	m_nNoOfHighPerformanceWnd = 0;

	IsDataBasePresent(TRUE);	// determine presence of database
}
/////////////////////////////////////////////////////////////////////////////
CIdipClientApp::~CIdipClientApp()
{
// do not delete m_pDlgLogin here, it destructs itself 
//	WK_DELETE(m_pDlgLogin);
	WK_DELETE(m_pUser);
	WK_DELETE(m_pPermission);
	WK_DELETE(m_pDispOM);

	WK_DELETE_ARRAY(m_pKBinput);
	if (m_hMainWndBrush)
	{
		::DeleteObject(m_hMainWndBrush);
	}
}
int CIdipClientApp::GetInt(CWK_Profile&wkp, CWKDebugOptions*pDO, LPCTSTR sSection, LPCTSTR sKey, int nDefault)
{
	int nValue = 0;
	if (pDO)
	{
		nValue = pDO->GetValue(sKey, nDefault);
		if (nValue == nDefault)
		{
			nValue = wkp.GetInt(sSection, sKey, nDefault);
		}
	}
	else
	{
		nValue = wkp.GetInt(sSection, sKey, nDefault);
	}
	WK_TRACE(_T("%s = %d\n"), sKey, nValue);
	return nValue;
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::LoadSettings()
{
	CWKDebugOptions debugOptions, *pDO = NULL;
	CWK_Profile wkp;
	UINT nRead			= 0;
	LPBYTE pBuffer		= NULL;
	LPCTSTR sSection	= IDIP_CLIENT_SETTINGS;
	CString sDebugFile	= _T("c:\\IdipClient.cfg");
	BOOL bOldROmode		= CWK_Profile::SetReadOnlyMode(TRUE),
		 bSaveSettings	= FALSE,
		 bFoundFile		= debugOptions.ReadFromFile(sDebugFile);
	HKEY hKey			= wkp.GetSectionKey(VISION_SETTINGS);
	if (hKey)			// Nach Update Vision Einstellungen übernehmen
	{
		sSection = VISION_SETTINGS;
		::RegCloseKey(hKey);
		hKey = NULL;
		bSaveSettings = TRUE;
	}
	CWK_Profile::SetReadOnlyMode(bOldROmode);

	if (bFoundFile)
	{
		pDO = &debugOptions;
		bSaveSettings = TRUE;
	}
	else
	{
		sDebugFile.Empty();
	}

	// obsolete Registry Keys
	wkp.DeleteEntry(IDIP_CLIENT_SETTINGS, MPEG4DECODEMT);
	wkp.DeleteEntry(IDIP_CLIENT_SETTINGS, _T("DrawInDecodeThread"));
	wkp.DeleteEntry(IDIP_CLIENT_SETTINGS, _T("RegardAvgVideoTimeout"));

	// Traces
	m_bTraceBitrate		= GetInt(wkp, pDO, sSection, TF_TRACE_BITRATE, m_bTraceBitrate);
	m_bTraceDelay		= GetInt(wkp, pDO, sSection, TF_TRACE_DELAY, m_bTraceDelay);
	m_bTraceSwitch		= GetInt(wkp, pDO, sSection, TF_TRACE_SWITCH, m_bTraceSwitch);
	m_bTraceImage		= GetInt(wkp, pDO, sSection, TF_TRACE_IMAGE, m_bTraceImage);
	m_bStatImage		= GetInt(wkp, pDO, sSection, TF_STAT_IMAGE, m_bStatImage);
	m_bTraceSequence	= GetInt(wkp, pDO, sSection, TF_TRACESEQUENCE, m_bTraceSequence);
	m_bTraceGOPTimer	= GetInt(wkp, pDO, sSection, TF_TRACEGOPTIMER, m_bTraceGOPTimer);
	m_bShowFPS			= GetInt(wkp, pDO, sSection, TF_SHOWCOCOFPS, m_bShowFPS); 

	// Old coco stuff
	m_bCorrectBitrate	= GetInt(wkp, pDO, sSection,TF_CORRECT_BITRATE, m_bCorrectBitrate); 
	m_bCoCoQuality		= GetInt(wkp, pDO, sSection,TF_COCO_QUALITY, m_bCoCoQuality); 
//	m_bCoCoFit			= GetInt(wkp, pDO, sSection,TF_COCO_FIT, m_bCoCoFit); 
//	m_bCoCoSoft			= GetInt(wkp, pDO, sSection,TF_COCO_SOFT, m_bCoCoSoft); 

	// test and bug prevents
	m_bUseMMtimer				= GetInt(wkp, pDO, sSection, USE_MM_TIMER     , m_bUseMMtimer); 
	m_bUseGOPThreadAlways		= GetInt(wkp, pDO, sSection, USE_USE_GT_ALWAYS, m_bUseGOPThreadAlways); 
	m_bWarnIfAudioIsBlockedAtNo = GetInt(wkp, pDO, sSection, WARN_IF_AUDIO_IS_BLOCKED_AT_NO, m_bWarnIfAudioIsBlockedAtNo);
	m_bRegardNetworkFrameRate	= GetInt(wkp, pDO, sSection, REGARDNETWORKFRAMERATE, m_bRegardNetworkFrameRate);
	m_bUseAvCodec				= GetInt(wkp, pDO, sSection, USE_AV_CODEC, m_bUseAvCodec);
	m_dwPictureBufferThreadFlags= GetInt(wkp, pDO, sSection, RUN_LIVE_WND_THREAD, m_dwPictureBufferThreadFlags);

	// touch screen
	m_bDisableSmall				= GetInt(wkp, pDO, sSection, DISABLESMALL     , m_bDisableSmall);
	m_bDisableZoom				= GetInt(wkp, pDO, sSection, DISABLEZOOM      , m_bDisableZoom);
	m_bDisableRightClick		= GetInt(wkp, pDO, sSection, DISABLERIGHTCLICK, m_bDisableRightClick);
	m_bDisableDblClick			= GetInt(wkp, pDO, sSection, DISABLEDBLCLICK  , m_bDisableDblClick);

	// Alarm
	m_bCloseAlarmWindow			= GetInt(wkp, pDO, sSection, CLOSEALARMWINDOW ,m_bCloseAlarmWindow);
	m_bFirstCam					= GetInt(wkp, pDO, sSection, SHOW_FIRSTCAM,m_bFirstCam);
	m_b1PlusAlarm				= GetInt(wkp, pDO, sSection, SHOW_1PLUSALARM,m_b1PlusAlarm);
	m_bFullscreenAlarm			= GetInt(wkp, pDO, sSection, SHOW_FULLSCREENALARM,m_bFullscreenAlarm);
	m_bAnimateAlarmConnections	= GetInt(wkp, pDO, sSection, ANIMATEALARMCONNECTIONS, m_bAnimateAlarmConnections);
	m_bShowAlarmMenu			= GetInt(wkp, pDO, sSection, SHOWALARMMENU, m_bShowAlarmMenu);

	m_sMapImagePath				= wkp.GetString(IDIP_CLIENT_SETTINGS, MAPIMAGEPATH, m_sMapImagePath);

	// probably User specific
	m_bShowTitleOfSmallWindows	= GetInt(wkp, pDO, sSection, SHOWTITLE_OF_SW, m_bShowTitleOfSmallWindows);
	m_bObjectViewsLeft			= GetInt(wkp, pDO, sSection, OBJECTVIEWSLEFT, m_bObjectViewsLeft);
	m_bDialogBarTop				= GetInt(wkp, pDO, sSection, DIALOGBARTOP, m_bDialogBarTop);
	m_bKeepProportion			= GetInt(wkp, pDO, sSection, KEEPPICTPROPORTION, m_bKeepProportion);
	m_bShowMainMenu				= GetInt(wkp, pDO, sSection, SHOWMAINMENU, m_bShowMainMenu);
	m_bShowMenuToolTips			= GetInt(wkp, pDO, sSection, SHOWMENUTOOLTIPS, m_bShowMenuToolTips);
	BOOL bTemp = GetInt(wkp, pDO, sSection, CONNECT_TO_LOCAL_SERVER, -1);
	if (bTemp == -1)
	{
		wkp.WriteInt(IDIP_CLIENT_SETTINGS, CONNECT_TO_LOCAL_SERVER, m_bConnectToLocalServer);
		bTemp = TRUE;
	}
	m_bConnectToLocalServer = bTemp ? DO_CONNECT_LOCAL : FALSE;
	ReadOpenDirPath();
	m_bShowHostMapAtLoggin		= GetInt(wkp, pDO, sSection, SHOW_HOSTMAPATLOGGIN, m_bShowHostMapAtLoggin);
	m_bShowCamereaMapAtConnection= GetInt(wkp, pDO, sSection, SHOW_CAMEREAMAPATCONNECTION, m_bShowCamereaMapAtConnection);
	m_bConfirmDisconnect = GetInt(wkp, pDO, sSection, CONFIRMDISCONNECT, m_bConfirmDisconnect);
	m_bVerifyUserToCloseNote = GetInt(wkp, pDO, sSection, VERIFYUSERTOCLOSENOTE, m_bVerifyUserToCloseNote);

	LoadUserSpecificSettings();

	m_sVersion					= wkp.GetString(_T("Version"),_T("Number"),_T(""));
	m_bRS232Transparent			= wkp.GetInt(_T("Communit"),_T("COM"),0)>0;
	m_bShellModus				= wkp.GetInt(_T("SecurityLauncher"), _T("Shell"), m_bShellModus);

	if (m_pDongle->IsDemo())
	{
		m_AutoLogout.SetValues(TRUE,TRUE,10,0,10,0);
	}
	else
	{
		m_AutoLogout.Load(wkp);
	}

	sSection = IDIP_CLIENT_SETTINGS;// normalfall ist IdipClient
	// Recherche
	bOldROmode = CWK_Profile::SetReadOnlyMode(TRUE);
	hKey = wkp.GetSectionKey(RECHERCHE);
	if (hKey)						// Nach Update Recherche Einstellungen übernehmen
	{
		sSection = RECHERCHE;
		::RegCloseKey(hKey);
		hKey = NULL;
		bSaveSettings = TRUE;
	}
	CWK_Profile::SetReadOnlyMode(bOldROmode);

	if (   wkp.GetBinary(SECTION_IDIP_CLIENT, DEV_MODE,&pBuffer,&nRead)
		|| wkp.GetBinary(RECHERCHE, DEV_MODE,&pBuffer,&nRead))
	{
		if (nRead>0)
		{
			m_hDevMode = GlobalAlloc(GHND,nRead);
			if (m_hDevMode)
			{
				LPBYTE lpDevMode = (LPBYTE)GlobalLock(m_hDevMode);
				if (lpDevMode)
				{
					CopyMemory(lpDevMode,pBuffer,nRead);
				}
				GlobalUnlock(m_hDevMode);
			}
		}
	}
	WK_DELETE(pBuffer);

	if (   wkp.GetBinary(SECTION_IDIP_CLIENT, DEV_NAMES,&pBuffer,&nRead)
		|| wkp.GetBinary(RECHERCHE, DEV_NAMES,&pBuffer,&nRead))
	{
		if (nRead>0)
		{
			m_hDevNames = GlobalAlloc(GHND,nRead);
			if (m_hDevNames)
			{
				LPBYTE lpDevNames = (LPBYTE)GlobalLock(m_hDevNames);
				if (lpDevNames)
				{
					CopyMemory(lpDevNames,pBuffer,nRead);
				}
				GlobalUnlock(m_hDevNames);
			}
		}
	}
	WK_DELETE(pBuffer);

	m_sStationName = wkp.GetString(sSection, STATION_NAME, _T(""));
	CString sMargin = wkp.GetString(sSection, PRINTER_MARGINS, RectToString(m_rcMargin));
	m_rcMargin = StringToRect(sMargin);

	m_bSearchMaskAtStart	= GetInt(wkp, pDO, sSection, SEARCH_MASK, m_bSearchMaskAtStart);
	m_nMaxRequestedRecords	= GetInt(wkp, pDO, sSection, MAX_REQUESTED_RECORDS, m_nMaxRequestedRecords);
	m_nMaxSentSamples		= GetInt(wkp, pDO, sSection, MAX_SENT_SAMPLES, m_nMaxSentSamples);
	m_bCanPlayRealtime		= GetInt(wkp, pDO, sSection, REALTIME, m_bCanPlayRealtime);

	m_wUpperThreshold = (WORD)GetInt(wkp, pDO, sSection, UPPER_CPU_USAGE_THRESHOLD, m_wUpperThreshold);
	m_wLowerThreshold = (WORD)GetInt(wkp, pDO, sSection, LOWER_CPU_USAGE_THRESHOLD, m_wLowerThreshold);
	m_nMinPerformanceLevel = (signed short)GetInt(wkp, pDO, sSection, MIN_PERFORMANCE_LEVEL, m_nMinPerformanceLevel);

	if (bSaveSettings)
	{
		CString sRegFile;
		SaveSettings();
		wkp.WriteInt(IDIP_CLIENT_SETTINGS, USE_MONITOR, m_dwMonitorFlags);
		
		hKey = wkp.GetSectionKey(VISION_SETTINGS);
		if (hKey)	// Copy the Vision monitor settings to the IdipClient key ?
		{
			sSection = HKEY_LOCAL_MACHINE_NAME _T("\\") SOFTWARE_KEY _T("\\") WK_PROFILE_ROOT IDIP_CLIENT_SETTINGS;
			sRegFile = m_sApplicationDir + _T("VisionOld.reg");
			wkp.SaveAs(sRegFile, sSection, FALSE, hKey);	// Vision Setting unter IdipClient RegKey speichern
			RegCloseKey(hKey);
			CString sExecute = _T("wait:regedt32.exe /s ");	// mit Regedt32 /s = Silent
			sExecute += sRegFile;
			ExecuteProgram(sExecute, m_sApplicationDir);	// importieren
		}

		wkp.DeleteSection(_T("Vision"));					// alte Registry einträge löschen
		wkp.DeleteSection(RECHERCHE);

		TRY													// Dateien löschen
		{
			if (!sDebugFile.IsEmpty())
			{
				CFile::Remove(sDebugFile);
			}
			if (!sRegFile.IsEmpty())
			{
				CFile::Remove(sRegFile);
			}
		}
		WK_CATCH(_T("cannot delete IdipClient.cfg"))
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::ReadOpenDirPath()
{
	CWK_Profile wkp;
	m_sOpenDirectory = wkp.GetString(IDIP_CLIENT_SETTINGS, OPENDIRECTORY, m_sOpenDirectory);
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::LoadUserSpecificSettings()
{
	if (m_pUser)
	{
		CWK_Profile wkp;
		CString sSection;
		sSection.Format(_T("%s\\%08lx"), IDIP_CLIENT_SETTINGS, m_pUser->GetID().GetID());
		m_dwSequenceIntervall = wkp.GetInt(sSection,_T("Sequence"),m_dwSequenceIntervall);
		m_bTargetDisplay = wkp.GetInt(sSection,_T("TargetDisplay"),m_bTargetDisplay);
		CIdipClientDoc* pDoc = GetDocument();
		if (pDoc)
		{
			pDoc->SetSequenceTime(m_dwSequenceIntervall);
		}
		// Recherche
		//ML 25.10.04 sSection = SECTION_IDIP_CLIENT;
		m_sStationName = wkp.GetString(sSection, STATION_NAME, NULL);
		CString sMargin = wkp.GetString(sSection, PRINTER_MARGINS, RectToString(m_rcMargin));
		m_rcMargin = StringToRect(sMargin);

		CString sQuerySection = sSection + _T("\\") + m_pUser->GetName();
		m_QueryParameter.DeleteFields();
		// RKE: Achtung! Bugfix mit Spätfolgen.
		// Die ungültigen Einträge müssen gelöscht werden
		int nCount = wkp.GetInt(sQuerySection, _T("NrOfFields"), 0);
		if (nCount > 150)
		{
			wkp.DeleteSection(sQuerySection);
			WK_TRACE(_T("Deleting Section %s: No of Query Fiels %d"), sQuerySection, nCount);
		}
		else
		{
			m_QueryParameter.Load(sQuerySection, wkp);
			m_sSyncTime = wkp.GetString(sQuerySection, SYNC_TIME, _T(""));
		}
		// probably userspecific
		m_bShowTitleOfSmallWindows	= wkp.GetInt(sSection, SHOWTITLE_OF_SW, m_bShowTitleOfSmallWindows);
		m_bShowMainMenu				= wkp.GetInt(sSection, SHOWMAINMENU, m_bShowMainMenu);
		m_sOpenDirectory			= wkp.GetString(sSection, OPENDIRECTORY, m_sOpenDirectory);
/*
		// TODO! RKE: Implement, if these settings are modified in the settings dialog
		m_bShowMenuToolTips			= wkp.GetInt(sSection, SHOWMENUTOOLTIPS, m_bShowMenuToolTips);
		m_bObjectViewsLeft			= wkp.GetInt(sSection, OBJECTVIEWSLEFT, m_bObjectViewsLeft);
		m_bDialogBarTop				= wkp.GetInt(sSection, DIALOGBARTOP, m_bDialogBarTop);
		m_bConnectToLocalServer		= wkp.GetInt(sSection, CONNECT_TO_LOCAL_SERVER, m_bConnectToLocalServer);
		m_bShowHostMapAtLoggin		= wkp.GetInt(sSection, SHOW_HOSTMAPATLOGGIN, m_bShowHostMapAtLoggin);
		m_bShowCamereaMapAtConnection= wkp.GetInt(sSection, SHOW_CAMEREAMAPATCONNECTION, m_bShowCamereaMapAtConnection);
*/
	}
	COemGuiApi::ToggleKeyboardInput(SELECT_USER_DEFINED); 
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::SaveSettings()
{
	CWK_Profile wkp;
	wkp.WriteInt(IDIP_CLIENT_SETTINGS,ANIMATEALARMCONNECTIONS,m_bAnimateAlarmConnections);
	wkp.WriteInt(IDIP_CLIENT_SETTINGS,TF_TRACE_BITRATE,m_bTraceBitrate);
	wkp.WriteInt(IDIP_CLIENT_SETTINGS,TF_TRACE_DELAY,m_bTraceDelay);
	wkp.WriteInt(IDIP_CLIENT_SETTINGS,TF_TRACE_SWITCH,m_bTraceSwitch);
	wkp.WriteInt(IDIP_CLIENT_SETTINGS,TF_TRACE_IMAGE,m_bTraceImage);
	wkp.WriteInt(IDIP_CLIENT_SETTINGS,TF_STAT_IMAGE,m_bStatImage);
	wkp.WriteInt(IDIP_CLIENT_SETTINGS,TF_TRACESEQUENCE,m_bTraceSequence);
	wkp.WriteInt(IDIP_CLIENT_SETTINGS,TF_TRACEGOPTIMER,m_bTraceGOPTimer);
	wkp.WriteInt(IDIP_CLIENT_SETTINGS,WARN_IF_AUDIO_IS_BLOCKED_AT_NO, m_bWarnIfAudioIsBlockedAtNo);
	wkp.WriteInt(IDIP_CLIENT_SETTINGS,TF_CORRECT_BITRATE,m_bCorrectBitrate);
	wkp.WriteInt(IDIP_CLIENT_SETTINGS,TF_COCO_QUALITY,m_bCoCoQuality);
//	wkp.WriteInt(IDIP_CLIENT_SETTINGS,TF_COCO_FIT,m_bCoCoFit);
//	wkp.WriteInt(IDIP_CLIENT_SETTINGS,TF_COCO_SOFT,m_bCoCoSoft);
//	wkp.WriteInt(IDIP_CLIENT_SETTINGS,_T("Settings"),1);
	wkp.WriteInt(IDIP_CLIENT_SETTINGS,SHOW_FIRSTCAM,m_bFirstCam);
	wkp.WriteInt(IDIP_CLIENT_SETTINGS,SHOW_1PLUSALARM,m_b1PlusAlarm);
	wkp.WriteInt(IDIP_CLIENT_SETTINGS,SHOW_FULLSCREENALARM,m_bFullscreenAlarm);
	wkp.WriteInt(IDIP_CLIENT_SETTINGS,TF_SHOWCOCOFPS, m_bShowFPS);
	wkp.WriteInt(IDIP_CLIENT_SETTINGS, USE_MM_TIMER, m_bUseMMtimer);
	wkp.WriteInt(IDIP_CLIENT_SETTINGS, USE_USE_GT_ALWAYS, m_bUseGOPThreadAlways); 
	wkp.WriteInt(IDIP_CLIENT_SETTINGS, REGARDNETWORKFRAMERATE, m_bRegardNetworkFrameRate);
	wkp.WriteInt(IDIP_CLIENT_SETTINGS, USE_AV_CODEC, m_bUseAvCodec); 
	wkp.WriteInt(IDIP_CLIENT_SETTINGS, RUN_LIVE_WND_THREAD, m_dwPictureBufferThreadFlags); 

	wkp.WriteInt(IDIP_CLIENT_SETTINGS, SHOW_HOSTMAPATLOGGIN, m_bShowHostMapAtLoggin);
	wkp.WriteInt(IDIP_CLIENT_SETTINGS, SHOW_CAMEREAMAPATCONNECTION, m_bShowCamereaMapAtConnection);
	wkp.WriteInt(IDIP_CLIENT_SETTINGS, CONFIRMDISCONNECT, m_bConfirmDisconnect);
	wkp.WriteInt(IDIP_CLIENT_SETTINGS, VERIFYUSERTOCLOSENOTE, m_bVerifyUserToCloseNote);
	wkp.WriteString(IDIP_CLIENT_SETTINGS, MAPIMAGEPATH, m_sMapImagePath);
	SaveOpenDirPath();

	wkp.WriteInt(IDIP_CLIENT_SETTINGS, DISABLESMALL     , m_bDisableSmall);
	wkp.WriteInt(IDIP_CLIENT_SETTINGS, DISABLEZOOM      , m_bDisableZoom);
	wkp.WriteInt(IDIP_CLIENT_SETTINGS, CLOSEALARMWINDOW , m_bCloseAlarmWindow);
	wkp.WriteInt(IDIP_CLIENT_SETTINGS, DISABLERIGHTCLICK, m_bDisableRightClick);
	wkp.WriteInt(IDIP_CLIENT_SETTINGS, DISABLEDBLCLICK  , m_bDisableDblClick);
	wkp.WriteInt(IDIP_CLIENT_SETTINGS, SHOWTITLE_OF_SW  , m_bShowTitleOfSmallWindows);
	wkp.WriteInt(IDIP_CLIENT_SETTINGS, OBJECTVIEWSLEFT, m_bObjectViewsLeft);
	wkp.WriteInt(IDIP_CLIENT_SETTINGS, DIALOGBARTOP, m_bDialogBarTop);
	wkp.WriteInt(IDIP_CLIENT_SETTINGS, KEEPPICTPROPORTION, m_bKeepProportion);
	wkp.WriteInt(IDIP_CLIENT_SETTINGS, SHOWMAINMENU, m_bShowMainMenu);
	wkp.WriteInt(IDIP_CLIENT_SETTINGS, SHOWMENUTOOLTIPS, m_bShowMenuToolTips);
	wkp.WriteInt(IDIP_CLIENT_SETTINGS, SHOWALARMMENU, m_bShowAlarmMenu);

	// is changed in system management
//	wkp.WriteInt(IDIP_CLIENT_SETTINGS, USE_MONITOR  , m_dwMonitorFlags);

	SaveUserSettings();

	// Recherche
	if (m_hDevMode)
	{
		DWORD  dwSize = GlobalSize(m_hDevMode);
		LPVOID lpVoid = GlobalLock(m_hDevMode);
		wkp.WriteBinary(SECTION_IDIP_CLIENT, DEV_MODE,(LPBYTE)lpVoid,dwSize);
		GlobalUnlock(m_hDevMode);
	}
	if (m_hDevNames)
	{
		DWORD  dwSize = GlobalSize(m_hDevNames);
		LPVOID lpVoid = GlobalLock(m_hDevNames);
		wkp.WriteBinary(SECTION_IDIP_CLIENT, DEV_NAMES,(LPBYTE)lpVoid,dwSize);
		GlobalUnlock(m_hDevNames);
	}

	wkp.WriteInt(SECTION_IDIP_CLIENT, SEARCH_MASK,m_bSearchMaskAtStart);
	wkp.WriteInt(SECTION_IDIP_CLIENT, REALTIME, m_bCanPlayRealtime);
	wkp.WriteInt(SECTION_IDIP_CLIENT, MAX_REQUESTED_RECORDS, m_nMaxRequestedRecords);
	wkp.WriteInt(SECTION_IDIP_CLIENT, MAX_SENT_SAMPLES, m_nMaxSentSamples);

	wkp.WriteInt(SECTION_IDIP_CLIENT, UPPER_CPU_USAGE_THRESHOLD, m_wUpperThreshold);
	wkp.WriteInt(SECTION_IDIP_CLIENT, LOWER_CPU_USAGE_THRESHOLD, m_wLowerThreshold);
	wkp.WriteInt(SECTION_IDIP_CLIENT, MIN_PERFORMANCE_LEVEL, m_nMinPerformanceLevel);
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::SaveOpenDirPath()
{
	CWK_Profile wkp;
	wkp.WriteString(IDIP_CLIENT_SETTINGS, OPENDIRECTORY, m_sOpenDirectory);
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::SaveUserSettings()
{
	if (m_pUser)
	{
		CWK_Profile wkp;
		CString sSection;
		sSection.Format(_T("%s\\%08lx"),IDIP_CLIENT_SETTINGS, m_pUser->GetID().GetID());
		wkp.WriteInt(sSection,_T("Sequence"),m_dwSequenceIntervall);
		wkp.WriteInt(sSection,_T("TargetDisplay"),m_bTargetDisplay);
		// Recherche
		wkp.WriteString(sSection, STATION_NAME,m_sStationName);
		wkp.WriteString(sSection, PRINTER_MARGINS, RectToString(m_rcMargin));

		m_QueryParameter.Save(sSection+_T("\\") + m_pUser->GetName(),wkp);
		wkp.WriteString(sSection + _T("\\") + m_pUser->GetName(), SYNC_TIME,m_sSyncTime);

		wkp.WriteInt(sSection, SHOWTITLE_OF_SW, m_bShowTitleOfSmallWindows);
		wkp.WriteInt(sSection, OBJECTVIEWSLEFT, m_bObjectViewsLeft);
		wkp.WriteInt(sSection, DIALOGBARTOP, m_bDialogBarTop);
		wkp.WriteInt(sSection, KEEPPICTPROPORTION, m_bKeepProportion);
		wkp.WriteInt(sSection, SHOWMAINMENU, m_bShowMainMenu);
		wkp.WriteInt(sSection, SHOWMENUTOOLTIPS, m_bShowMenuToolTips);
		wkp.WriteInt(sSection, CONNECT_TO_LOCAL_SERVER, m_bConnectToLocalServer != 0 ? TRUE : FALSE);
		wkp.WriteString(sSection, OPENDIRECTORY, m_sOpenDirectory);
		wkp.WriteInt(sSection, SHOW_HOSTMAPATLOGGIN, m_bShowHostMapAtLoggin);
		wkp.WriteInt(sSection, SHOW_CAMEREAMAPATCONNECTION, m_bShowCamereaMapAtConnection);
		wkp.WriteInt(sSection, CONFIRMDISCONNECT, m_bConfirmDisconnect);
		wkp.WriteInt(sSection, VERIFYUSERTOCLOSENOTE, m_bVerifyUserToCloseNote);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIdipClientApp::SplitCommandLine(const CString sCommandLine, CStringArray& saParameters, LPCTSTR szParam)
{
	CString sCL = _T(" ") + sCommandLine;
	int nFind1, nFind2;
	nFind1 = sCL.Find(szParam);
	if (nFind1 != -1)
	{
		nFind1 += _tcslen(szParam);
		nFind2 = sCL.Find(_T(" -"), nFind1);
		if (nFind2 != -1 && nFind2 >= nFind1)
		{
			saParameters.Add(sCL.Mid(nFind1, nFind2-nFind1));
		}
		else
		{
			saParameters.Add(sCL.Mid(nFind1));
		}
		return TRUE;
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIdipClientApp::InitInstance()
{
	CWK_Profile wkp;
	CString sLangAbb;
	LPCTSTR pszLangAbb = NULL;

	if (WK_ALONE(WK_APP_NAME_IDIP_CLIENT) == FALSE)
	{
		HWND hWndFirstInstance = FindWindow(WK_APP_NAME_IDIP_CLIENT,NULL);
		if (hWndFirstInstance)
		{
			SetWindowPos(hWndFirstInstance,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
			SetWindowPos(hWndFirstInstance,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
		}
		return FALSE;
	}

	// Load OEM settings
	CString sIniFile = m_pszHelpFilePath;
	int nSlash = sIniFile.ReverseFind(_T('\\'));
	if (nSlash != -1)
	{
		m_sApplicationDir = sIniFile.Left(nSlash+1);
		sIniFile = m_sApplicationDir + OEM_FILE;
	}
#ifdef _DEBUG
	m_sApplicationDir = _T("C:\\Security\\");
	CString sDebugInifile = m_sApplicationDir + OEM_FILE;
	if (::GetFileAttributes(sDebugInifile) != INVALID_FILE_ATTRIBUTES)
	{
		sIniFile = sDebugInifile;
	}
#endif

	CWK_Profile wkpOEM(CWK_Profile::WKP_INIFILE_ABSOLUT, NULL, _T(""), sIniFile); 
	COemGuiApi::LoadOEMSkinOptions(wkpOEM);
	CSkinDialog::SetButtonImages(SC_CLOSE    , (UINT)-1, 0, 0);
	CSkinDialog::SetButtonImages(SC_MAXIMIZE , (UINT)-1, 0, 0);
	CSkinDialog::SetButtonImages(SC_MINIMIZE , (UINT)-1, 0, 0);
	CSkinDialog::SetButtonImages(SC_MOUSEMENU, (UINT)-1, 0, 0, LoadIcon(IDR_MAINFRAME));
	CSkinDialog::LoadButtonImages();

	CString sLogfileName = _T("IdipClient.log");

	if (WK_IS_RUNNING(WK_DB_SERVER_READ_ONLY))
	{
		m_bReadOnlyModus = TRUE;
		sLogfileName = _T("IdipClientRO.log");

		CAutoRun ar;
		if (ar.IsValid())
		{
			m_sOemIni = ar.GetPath();
			m_OemCode = ar.GetOemCode();
		}

		HKEY hKey = NULL;
		CString str = BASE_DVRT_KEY;
		LONG res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, str, 0, KEY_READ, &hKey);
		if (res == ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			m_bReadOnlyModus |= REGKEY_BASEDVRT_DEFINED;
		}
		str += SECTION_IDIP_CLIENT;
		res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, str, 0, KEY_READ, &hKey);
		if (res == ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			m_bReadOnlyModus |= REGKEY_IDIPCLIENT_DEFINED;
		}
		m_pDongle = new CWK_Dongle();
		
		CString sTitle = COemGuiApi::GetApplicationNameOem(WAI_IDIP_CLIENT, m_OemCode);
		int nFind = sTitle.Find(_T(" "));
		if (nFind != -1)
		{
			sTitle = sTitle.Left(nFind);
		}
		
		if (m_pszAppName)
		{
			free((void*)m_pszAppName);
		}
		m_pszAppName = _tcsdup(sTitle);    // human readable title

		CDlgLogin dlg(NULL, 0, DLGLOGIN_ONLY_LANGUAGE);
		dlg.DoModal();
		sLangAbb = dlg.GetLanguageAbbreviation();
		pszLangAbb = sLangAbb;
	}
	else
	{
		CWK_Dongle dongle(WAI_DATABASE_CLIENT);
		
		m_OemCode = dongle.GetOemCode();
		m_bAllowBackup = dongle.RunCDRWriter();
//		m_bAllowMiCo = dongle.AllowSoftDecodeMiCo();
//		m_bAllowCoCo = dongle.AllowSoftDecodeCoCo();
//		m_bAllowPT = dongle.AllowSoftDecodePT();
//		m_bIsReceiver = dongle.IsReceiver(); //variable fällt weg, da DongleInfo in IsReceiver() 
		m_bRunAnyLinkUnit = dongle.RunAnyLinkUnit();
											 //direkt abgefragt wird
		m_bIsDemo = dongle.IsDemo();

		m_pDongle = new CWK_Dongle();
		if (!m_pDongle->IsValid())
		{
			AfxMessageBox(IDP_INVALID_DONGLE);
			return FALSE;
		}
		CheckHostMap();
	}

	InitDebugger(sLogfileName, WAI_IDIP_CLIENT);

#if _MFC_VER == 0x0710
	CLoadResourceDll ResourceDll(NULL, pszLangAbb);
	m_hLangResourceDLL = ResourceDll.DetachResourceDllHandle();
	if (m_hLangResourceDLL == NULL)
	{
		WK_TRACE(_T("Did not find any ResourceDLL\n"));
		ASSERT(FALSE);
		return FALSE;
	}

	AfxSetResourceHandle(m_hLangResourceDLL);
	// Set the CodePage for MBCS conversion (HTML Pages)

//#ifndef _DEBUG
	// RKE: version upgrade check
	CString sClientVersion = wkp.GetString(IDIP_CLIENT_SETTINGS, CLIENT_VERSION, NULL);
	CString sCurrentVersion = COemGuiApi::GetSoftwareVersion();
	if (!sCurrentVersion.IsEmpty())
	{
		int nLastSpace = sCurrentVersion.ReverseFind(_T(' '));
		if (nLastSpace != -1)
		{
			sCurrentVersion = sCurrentVersion.Mid(nLastSpace+1);
		}
	}
	if (   sClientVersion.IsEmpty()
		|| sCurrentVersion > sClientVersion)
	{
		// delete these keys to take the new optimal values for the new version
		wkp.DeleteEntry(IDIP_CLIENT_SETTINGS, USE_MM_TIMER);		// for accuracy
		wkp.DeleteEntry(IDIP_CLIENT_SETTINGS, USE_USE_GT_ALWAYS);	// for correct replay
		wkp.DeleteEntry(IDIP_CLIENT_SETTINGS, REGARDNETWORKFRAMERATE);// for timeout caused by network
		wkp.DeleteEntry(IDIP_CLIENT_SETTINGS, RUN_LIVE_WND_THREAD);	// to decouple CIPC Threads from working
		wkp.DeleteEntry(IDIP_CLIENT_SETTINGS, USE_AV_CODEC);		// for speed
		// write the new version, so that the keys are not deleted next time
		wkp.WriteString(IDIP_CLIENT_SETTINGS, CLIENT_VERSION, sCurrentVersion);
	}
//#endif
	m_AvgCPUusage.SetNoOfAvgValues(GetIntValue(_T("CPUusageAvgValues"), 5));
	m_bShowCPUusage = GetIntValue(_T("Show_CPU_Usage"));

	CString sCodePage = COemGuiApi::GetCodePage();
	CWK_String::SetCodePage(_ttoi(sCodePage));
	wkp.SetCodePage(CWK_String::GetCodePage());

	if (GetIntValue(_T("SetLocale"), m_pDongle->IsReceiver() ? 0 : 1))
	{
		EnumSystemLocales(EnumLocalesProc, LCID_SUPPORTED);
	}
// RKE: InitInstance() loads the languageDll depending on the system language settings
// Do not call!
//	CWinApp::InitInstance();
#endif


	CString sTitle;
	if (m_bReadOnlyModus)
	{
		CString sBackup;
		sBackup.LoadString(IDS_BACKUP);
		sTitle = m_pszAppName;
		sTitle += _T(" ") + sBackup;
	}
	else
	{
		sTitle = COemGuiApi::GetApplicationName(WAI_IDIP_CLIENT);
		int nFind = sTitle.Find(_T(" "));
		if (nFind != -1)
		{
			sTitle = sTitle.Left(nFind);
		}
		if (m_pDongle->IsDemo())
		{
			sTitle += _T(" DEMO");
		}
	}

	if (m_pszAppName)
	{
		free((void*)m_pszAppName);
	}
	m_pszAppName = _tcsdup(sTitle);    // human readable title
	AFX_MODULE_STATE*pState = AfxGetModuleState();
	if (pState)
	{
		if (pState->m_lpszCurrentAppName)
		{
			pState->m_lpszCurrentAppName = m_pszAppName;
		}
	}

#ifndef _DTS
	m_dwMonitorFlags   = GetInt(wkp, NULL, VISION_SETTINGS, USE_MONITOR, m_dwMonitorFlags);
	m_dwMonitorFlags   = GetInt(wkp, NULL, IDIP_CLIENT_SETTINGS, USE_MONITOR, m_dwMonitorFlags);
#endif
	
	// Create a custom control manager class so we can overide the site
	CCustomOccManager *pMgr = new CCustomOccManager;
	// Create an IDispatch class for extending the Dynamic HTML Object Model 
	m_pDispOM = new CImpIDispatch;
	// Set our control containment up but using our control container 
	// management class instead of MFC's default
	AfxEnableControlContainer(pMgr);

	AfxInitRichEdit();	// Recherche

	m_bRunAnyLinkUnit = m_pDongle->RunAnyLinkUnit() || m_pDongle->RunAnyBoxUnit();

	WK_STAT_LOG(_T("Reset"),0,_T("ServerActive"));
	WK_STAT_LOG(_T("Reset"),1,_T("ServerActive"));
	LoadSettings();

#ifdef _DEBUG
	m_nCmdShow = SW_SHOWNORMAL;
#else
	m_nCmdShow = SW_SHOWMAXIMIZED;
#endif

	RegisterWindowClass();

	InitCommonControls();

	InitFieldMap();	// Recherche

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CIdipClientDoc),
		RUNTIME_CLASS(CMainFrame),       // Haupt-SDI-Rahmenfenster
		RUNTIME_CLASS(CViewIdipClient));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

// RKE: OnFileNew() is called in stead of parsing the command line
//	CCommandLineInfo cmdInfo;
//	ParseCommandLine(cmdInfo);
//	if (!ProcessShellCommand(cmdInfo))
//		return FALSE;

	OnFileNew();

	if (m_pMainWnd == NULL)
	{
		return FALSE;
	}
	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->SetWindowText(m_pszAppName);
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	m_pMainWnd->SetForegroundWindow();

	CString s = m_lpCmdLine;
	CStringArray saParameters;
	SplitCommandLine(m_lpCmdLine, saParameters, CMD_PARAM_USER);
	SplitCommandLine(m_lpCmdLine, saParameters, CMD_PARAM_PASSWORD);

//	RKE: BackupMode obsolete
//	m_bBackupModus = (-1!=s.Find(CMD_PARAM_BACKUP));
//	if (m_bBackupModus)
//	{
//		WK_TRACE(_T("starting in backup mode\n"));
//	}

	s.TrimLeft();
	s.TrimRight();

	if (   (saParameters.GetSize()==0)
		|| (s.IsEmpty())
		)
	{
		if (Login())
		{
			// if logged in be sure the menu is visible if enabled
			CMainFrame* pMainFrame = GetMainFrame();
			pMainFrame->ShowMenu(TRUE);
		}
	}
	else if (saParameters.GetSize()==1)
	{
		if (!Login((LPCTSTR)saParameters[0]))
		{
			return FALSE;
		}
	}
	else if (saParameters.GetSize()==2)
	{
		if (!Login((LPCTSTR)saParameters[0],(LPCTSTR)saParameters[1]))
		{
			return FALSE;
		}
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
int CIdipClientApp::ExitInstance()
{
	SaveSettings();

	WK_STAT_LOG(_T("Reset"),0,_T("ServerActive"));
	if (m_pUser)
	{
		WK_TRACE_USER(_T("%s hat sich ausgeloggt, Programmende\n"),m_pUser->GetName());
	}

//	WK_DESTROY_WINDOW(m_pDlgLogin);
	WK_DELETE(m_pDongle);
	WK_DELETE(m_pUser);
	WK_DELETE(m_pPermission);

	if (m_bReadOnlyModus) // WK_IS_RUNNING(WK_DB_SERVER_READ_ONLY))
	{
		// close database server
		HWND hWnd = FindWindow(WK_APP_NAME_DB_SERVER,NULL);
		if (hWnd && IsWindow(hWnd))
		{
			PostMessage(hWnd,WM_COMMAND,ID_APP_EXIT,0L);
			WK_TRACE(_T("Exit and close DBS\n"));
		}

		//close audiounit
		//the audiounit actually was started in readonly modus from dbs.exe
		HWND hWndAudio = FindWindow(NULL, WK_APP_NAME_AUDIOUNIT1);

		if (hWndAudio && IsWindow(hWndAudio))
		{
			PostMessage(hWndAudio,WM_COMMAND,ID_APP_EXIT,0L);
			WK_TRACE(_T("Exit and close AudioUnit\n"));
		}
		CWK_Profile wkpAbs(CWK_Profile::WKP_ABSOLUT, HKEY_LOCAL_MACHINE, _T(""), _T(""));
		CString str = BASE_DVRT_KEY;
		if (!(m_bReadOnlyModus & REGKEY_BASEDVRT_DEFINED))
		{
			wkpAbs.DeleteSection(str);
		}
		else if (!(m_bReadOnlyModus & REGKEY_IDIPCLIENT_DEFINED))
		{
			str += SECTION_IDIP_CLIENT;
			wkpAbs.DeleteSection(str);
		}
	}
	
	CWndMpeg4::ReleaseMMTimer();

	CloseDebugger();

	return CWinApp::ExitInstance();
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::RegisterWindowClass()
{
	WNDCLASS  wndclass;
	
	m_hMainWndBrush = ::CreateSolidBrush(CSkinTree::GetOEMBkColor());
	// register window class
	wndclass.style			= CS_DBLCLKS|CS_HREDRAW;
	wndclass.lpfnWndProc	= DefWindowProc;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= 0;
	wndclass.hInstance		= m_hInstance;
	wndclass.hCursor		= LoadCursor(IDC_ARROW);
	wndclass.hIcon			= LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME));
	wndclass.hbrBackground	= m_hMainWndBrush; //(HBRUSH) (COLOR_WINDOW + 1); //afxData.hbrWindowFrame;
	wndclass.lpszMenuName	= 0L;
	wndclass.lpszClassName	= WK_APP_NAME_IDIP_CLIENT;

	// main app window
	AfxRegisterClass(&wndclass);

//	wndclass.style |= CS_OWNDC;
	wndclass.hIcon = NULL;
	wndclass.hCursor = NULL;
	wndclass.lpszClassName = SMALL_WINDOW_CLASS;

	// CWndSmall
	AfxRegisterClass(&wndclass);

	m_hPanLeft		= LoadCursor(IDC_PANLEFT);
	m_hPanRight		= LoadCursor(IDC_PANRIGHT);
	m_hTiltUp		= LoadCursor(IDC_TILTUP);
	m_hTiltDown		= LoadCursor(IDC_TILTDOWN);
	m_hZoomIn		= LoadCursor(IDC_ZOOMIN);
	m_hZoomOut		= LoadCursor(IDC_ZOOMOUT);
	m_hArrow		= LoadStandardCursor(IDC_ARROW);
	m_hLeftTop		= LoadCursor(IDC_LEFTTOP);
	m_hRightTop		= LoadCursor(IDC_RIGHTTOP);
	m_hRightBottom	= LoadCursor(IDC_RIGHTBOTTOM);
	m_hLeftBottom	= LoadCursor(IDC_LEFTBOTTOM);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIdipClientApp::LoginAsSuperVisor()
{
	SaveUserSettings();
	WK_DELETE(m_pUser);

	CWK_Profile wkp;
	CPermissionArray PermissionArray;
	CPermission* pPermission;

	PermissionArray.Load(wkp);
	pPermission = PermissionArray.GetSuperVisor();
	WK_DELETE(m_pPermission);
	m_pPermission = new CPermission(*pPermission);
	
	m_pUser = new CUser();
	m_pUser->SetName(_T("SuperVisor"));
	m_pUser->SetPermissionID(m_pPermission->GetID());
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIdipClientApp::Login(LPCTSTR pszUser /*= NULL*/,LPCTSTR pszPassword /*= NULL*/)
{
#ifdef _DEBUG
//	return LoginAsSuperVisor();
#endif
	if (m_bReadOnlyModus)
	{
		return LoginAsSuperVisor();
	}

	CWK_Profile wkp;
	if (m_pUser)
	{
		CString sSection;
		sSection.Format(_T("%s\\%08lx"),IDIP_CLIENT_SETTINGS, m_pUser->GetID().GetID());
		wkp.WriteInt(sSection,_T("Sequence"),m_dwSequenceIntervall);
	}

	CString sUser(pszUser);
	CString sPassword(pszPassword);
	BOOL bRet = FALSE;

	if (pszPassword == NULL) // sPassword.IsEmpty())
	{
		// check user and password
		// only if one of them is not empty
#ifndef _DTS
		if (pszUser != NULL) // !sUser.IsEmpty())
		{
			bRet = CheckUserPassword(sUser,sPassword);
		}
#endif

		if (!bRet)
		{
			CPermissionArray pa;
			CPermission* pPermission = pa.GetSpecialReceiver();
			WK_TRACE(_T("Permission %08lx\n"),pPermission);
			if (pPermission)
			{
				SaveUserSettings();
				WK_DELETE(m_pUser);
				m_pUser = new CUser();
				m_pUser->SetName(pPermission->GetName());
				WK_DELETE(m_pPermission);
				m_pPermission = new CPermission(*pPermission);
				// Remove some menu entries for SpecialReceiver
				CMenu* pMenu = theApp.m_pMainWnd->GetMenu();
				if (pMenu != NULL)
				{
					pMenu = pMenu->GetSubMenu(MAIN_MENU_CONNECTION);
					if (pMenu != NULL)
					{
						pMenu->DeleteMenu(ID_NEW_LOGIN, MF_BYCOMMAND);
						pMenu->DeleteMenu(ID_CONNECT_LOCAL, MF_BYCOMMAND);
					}
					// force a redraw of the menu bar NEEDED
					theApp.m_pMainWnd->DrawMenuBar();
				}
				bRet = TRUE;
			}
			else
			{
				CMainFrame* pMainFrame = GetMainFrame();
				if (pMainFrame)
				{
					// Avoid drawing of the window
					// pMainFrame->LockWindowUpdate();
					// Use the Menu on/off workaround for correct sizing of the view
					pMainFrame->ShowMenu(TRUE);
					CViewIdipClient* pViewClient = pMainFrame->GetViewIdipClient();
					if (pViewClient)
					{
						CWnd* pParent = pViewClient;
						DWORD dwFlags = DLGLOGIN_CENTER;		// normaly shown inside the view
						dwFlags      |= DLGLOGIN_AUTO_DELETE;	// shall delete himself

						if (pViewClient->GetSmallWindows()->GetSize())
						{										// not, if there are small windows
							dwFlags &= ~DLGLOGIN_CENTER;
						}
						if (IsUserAlarm())						// Alarm User
						{										// still an alarm connection
							dwFlags |= DLGLOGIN_NO_LANGUAGE;	// do not select other language
						}
						if (m_pDlgLogin)
						{
							WK_DESTROY_WINDOW(m_pDlgLogin);
						}
						m_pDlgLogin = new CDlgLogin(pParent, 0, dwFlags);
						m_pDlgLogin->Create(pParent);

						pMainFrame->ShowMenu(FALSE);
						pMainFrame->SetActiveView(pViewClient);
						m_pDlgLogin->SetFocus();
					}
				}
				// FALSE is a valid return, as no one is logged in yet
				bRet = FALSE;
			}
		}
	}
	else
	{
		bRet = CheckUserPassword(sUser,sPassword);
		if (bRet && IsValidUserLoggedIn())
		{
			CMainFrame* pMainFrame = GetMainFrame();
			if (pMainFrame)
			{
				pMainFrame->ShowMenu(TRUE);
				CViewIdipClient* pViewClient = pMainFrame->GetViewIdipClient();
				if (pViewClient)
				{
					pViewClient->ShowMapWindow();
				}
			}
			LoadUserSpecificSettings();
			m_AutoLogout.ResetApplication();
		}
		else
		{
			WK_TRACE_USER(_T("Login Fehler! Nutzer:%s\n"),(LPCTSTR)sUser);
		}
	}

	if (bRet)
	{
		WK_TRACE_USER(_T("Login Nutzer:%s, Berechtigungsstufe:%s\n"),m_pUser->GetName(),m_pPermission->GetName());
		LoadUserSpecificSettings();
		m_AutoLogout.ResetApplication();
	}

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIdipClientApp::CheckUserPassword(CString& sUser, CString& sPassword)
{
	CUserArray UserArray;
	CSecID  idUser;
	BOOL bRet = FALSE;
	CPermissionArray PermissionArray;
	CPermission* pPermission;
	CWK_Profile wkp;

	if (sUser==_T("SuperVisor"))
	{
		SaveUserSettings();
		WK_DELETE(m_pUser);

		PermissionArray.Load(wkp);
		pPermission = PermissionArray.GetSuperVisor();
		
		if (sPassword==pPermission->GetPassword())
		{
			WK_DELETE(m_pPermission);
			m_pPermission = new CPermission(*pPermission);
			
			m_pUser = new CUser();
			m_pUser->SetName(_T("SuperVisor"));
			bRet = TRUE;
		}
	}
	else
	{
		UserArray.Load(wkp);
		idUser = UserArray.IsValid(sUser,sPassword);

		if (idUser.IsUserID())
		{
			CUser* pUser;
			pUser = UserArray.GetUser(idUser);
			if (pUser)
			{
				SaveUserSettings();
				WK_DELETE(m_pUser);
				m_pUser = new CUser(*pUser);

				PermissionArray.Load(wkp);
				pPermission = PermissionArray.GetPermission(m_pUser->GetPermissionID());
				if (pPermission)
				{
					WK_DELETE(m_pPermission);
					m_pPermission = new CPermission(*pPermission);
					bRet = TRUE;
				}
			}
		}
	}// sUser==_T("SuperVisor")
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
#define NO_ACTION			0
#define DIRECT_TO_DLG_BAR	1
#define DIRECT_TO_DLG_PTZ	2
#define ANY_ACTION			3
#define ACTION_HANDLED		4
#define DIALOG_MESSAGE		5
BOOL CIdipClientApp::PreTranslateMessage(MSG* pMsg) 
{
	int nAction = NO_ACTION;
	CWnd*pWnd = NULL, *pWndOrg = NULL;
	switch (pMsg->message)
	{
		case WM_KEYUP:
			switch(pMsg->wParam)
			{
				case VK_F11:
					COemGuiApi::ToggleKeyboardInput();
					return TRUE;
					break;
				case VK_F1:
				if (GetMainFrame()->m_hAccelTable)
				{
					int i, nAcc = CopyAcceleratorTable(GetMainFrame()->m_hAccelTable, NULL, 0);
					if (nAcc)
					{
						struct MyAccel
						{
							WORD fVirt;
							WORD key;
							WORD cmd;
						};
						MyAccel *pAcc = new MyAccel[nAcc];
						CString s, sCmd, sTxt, sMsg;
						CSkinMenu *pMenu = (CSkinMenu*)GetMainFrame()->GetMenu();
						CSkinMenu MenuLive;
						CSkinMenu MenuPlay;
						MenuLive.LoadMenu(IDR_MENU_LIVE);
						MenuPlay.LoadMenu(IDR_MENU_ARCHIVE);
						nAcc = CopyAcceleratorTable(GetMainFrame()->m_hAccelTable, (ACCEL*)pAcc, nAcc);
						for (i=0; i<nAcc; i++)
						{
							UINT uID = pAcc[i].cmd;
							sCmd.Empty();
							if (pAcc[i].fVirt & FALT) sCmd += _T("ALT+");
							if (pAcc[i].fVirt & FCONTROL) sCmd += _T("Ctrl+");
							if (pAcc[i].fVirt & FSHIFT) sCmd += _T("Shift+");
							if (isalpha(pAcc[i].key)&& isupper(pAcc[i].key))
							{
								_TCHAR sKey[2] = _T(" ");
								sKey[0] = pAcc[i].key;
								sCmd += sKey;
							}
							else
							{
								CString sKey;
								switch(pAcc[i].key)
								{
									case VK_ESCAPE: sKey = _T("ESC"); break;
									case VK_RETURN: sKey = _T("Enter"); break;
									case VK_DELETE: sKey = _T("Del"); break;
									case VK_NEXT: sKey = _T("Next"); break;
									case VK_PRIOR: sKey = _T("Prior"); break;
									case VK_END: sKey = _T("End"); break;
									case VK_HOME: sKey = _T("Home"); break;
									case VK_RIGHT: sKey = _T(">"); break;
									case VK_LEFT: sKey = _T("<"); break;
									case VK_PAUSE: sKey = _T("|-|"); break;
									case VK_SPACE: sKey = _T("Space"); break;
									default:
										if (IsBetween(pAcc[i].key, VK_F1, VK_F24))
										{
											sKey.Format(_T("F%d"), pAcc[i].key - VK_F1 + 1);
										}
										else
										{
											sKey.Format(_T("VK (%d)"), pAcc[i].key);
										}
										break;
								}
								sCmd += sKey;
							}
							if (  s.LoadString(uID)
								|| MenuLive.FindMenuString(uID, s)
								|| MenuPlay.FindMenuString(uID, s)
								|| pMenu && pMenu->FindMenuString(uID, s))
							{
								s.Replace(_T("\n"), _T(" / "));
								sTxt.Format(_T("%s:\t%s\n"), sCmd, s);
							}
							else
							{
								sTxt.Format(_T("%s:\t%d\n"), sCmd, uID);
//								continue;
							}
							TRACE(_T("%s"), sTxt);
							sMsg += sTxt;
						}
						AfxMessageBox(sMsg, MB_OK);
						delete pAcc;
					}
					return TRUE;
				}break;
				case VK_TAB:
					pWnd = pWndOrg = CWnd::FromHandle(GetFocus());
					if (pWnd) pWnd = pWnd->GetParent();
					if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CDialog)))
					{
						// allow tab control in dialogs
					}
					else
					{
						GetMainFrame()->OnTab();
						nAction = ACTION_HANDLED;
					}break;
			}	// FALLTHROUGH
				// RKE: do not insert break here !!
		case WM_KEYDOWN:
		{
			if (pMsg->wParam == VK_F12)
			{
				return TRUE;
			}
			pWnd = pWndOrg = CWnd::FromHandle(GetFocus());
			if (pWnd)
			{
//				CString str(pWnd->GetRuntimeClass()->m_lpszClassName);
//				TRACE(_T("%s\n"), str);
				if (pWnd->IsKindOf(RUNTIME_CLASS(CDialog)))
				{
					nAction = DIALOG_MESSAGE;	// Dialogs
				}
				else if (pWnd->IsKindOf(RUNTIME_CLASS(CViewObjectTree)))
				{
					nAction = ANY_ACTION;		// to TreeViews
				}
				else							// what else
				{
					nAction = DIRECT_TO_DLG_BAR;// assume to dialogbar
					pWnd = pWnd->GetParent();	// is it a control
					if (pWnd)
					{
						if (pWnd->IsKindOf(RUNTIME_CLASS(CDialog)))
						{
							nAction = DIALOG_MESSAGE;
						}
						else if (pWnd->IsKindOf(RUNTIME_CLASS(CWndSmall)))
						{
							if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_UP || pMsg->wParam == VK_DOWN))
							{
								nAction = ANY_ACTION;		// to Edit field
							}
							else
							{
								pWndOrg->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
								nAction = ACTION_HANDLED;
							}
						}
						else
						{
							pWnd = pWnd->GetParent();	// was it a control in a toobar
							if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CSyncCoolBar)))
							{
								if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_LEFT || pMsg->wParam == VK_RIGHT))
								{
									pWndOrg->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
									nAction = ACTION_HANDLED;
								}
								else
								{
									nAction = ANY_ACTION;
								}
							}
						}
					}
				}
				if (nAction == DIALOG_MESSAGE)	// is it a DialogMessage
				{	
					if (   pWnd->IsKindOf(RUNTIME_CLASS(CDlgBarIdipClient))
						|| pWnd->IsKindOf(RUNTIME_CLASS(CDlgExternalFrame)))
					{
						nAction = DIRECT_TO_DLG_BAR;// handle directly
					}
					else if (pWnd->IsKindOf(RUNTIME_CLASS(CDlgPTZVideo)))
					{
						nAction = DIRECT_TO_DLG_PTZ;// handle directly
					}
					else
					{
						nAction = ANY_ACTION;		// to Any Dialog
					}
				}
			}
		} break;
		case WM_LBUTTONDOWN: case WM_RBUTTONDOWN:
			nAction = ANY_ACTION;
			break;
		case WM_COMMAND:
			nAction = ANY_ACTION;
			// RKE: Messages from SV to avoid autologout during administration.
			// SV is not closed by logout anymore
			// in future applications should use WM_RESET_AUTO_LOGOUT
			// maybe we have to make a difference between
			// autologout and alarm or manual logout.
			break;
	}

	if (nAction == DIRECT_TO_DLG_BAR)
	{
		if (   pMsg->message == WM_KEYDOWN
			&& GetMainFrame()->GetDlgBarIdipClient()->OnKeyDown(pMsg->wParam, LOWORD(pMsg->lParam), HIWORD(pMsg->lParam)))
		{
			nAction = ACTION_HANDLED;
		}
		else if (   pMsg->message == WM_KEYUP
			     && GetMainFrame()->GetDlgBarIdipClient()->OnKeyUp(pMsg->wParam, LOWORD(pMsg->lParam), HIWORD(pMsg->lParam)))
		{
			nAction = ACTION_HANDLED;
		}
	}

	if (nAction)			// ANY
	{
		ResetAutoLogout();
	}
	if (nAction == ACTION_HANDLED)
	{
		return TRUE;
	}
	return CWinApp::PreTranslateMessage(pMsg);
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::ResetAutoLogout()
{
	if (!IsDemo())
	{
		m_AutoLogout.ResetApplication();
		CIdipClientDoc* pDoc = GetDocument();
		if (pDoc) 
		{
			pDoc->ResetConnectionAutoLogout();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIdipClientApp::OnIdle(LONG lCount) 
{
	m_dwIdleTickCount = GetTickCount();
	if (m_pMainWnd)
	{
		((CMainFrame*)m_pMainWnd)->OnIdle();
	}

	CErrorMessage* pErrorMessage = m_ErrorMessages.SafeGetAndRemoveHead();
	if (pErrorMessage)
	{
		CString s;
		s.Format(IDP_ERROR,pErrorMessage->GetServer(),
			pErrorMessage->GetMessage());
		COemGuiApi::MessageBox(s,10,MB_ICONINFORMATION|MB_OK);
		WK_DELETE(pErrorMessage);
	}

	if (m_bDoReset == TRUE)	// ask whether it is equal to TRUE
	{
		m_bDoReset = 2;
		Reset();			
	}

	return CWinApp::OnIdle(lCount);
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::OnNewLogin(UINT nID) 
{
	CMainFrame* pMainfame = GetMainFrame();
	if (pMainfame)
	{
		SaveUserSettings();
		BOOL bKeepUserAlarm = FALSE;
		BOOL bAllDisconnected = FALSE;
		// mehrsprachige Programme werden hier nicht mehr geschlossen
		// die Sprache kann sich aendern, muss aber nicht.
		pMainfame->FreeJoystick();
		CIdipClientDoc* pDoc = pMainfame->GetDocument();
		if (pDoc)
		{
			bAllDisconnected = pDoc->DisconnectAll(TRUE);
			if (!bAllDisconnected)
			{
				if (IsValidUserLoggedIn())
				{
					// Ask for disconnecting alarm connection
					if (AfxMessageBox(IDP_DISCONNECT_ALARM_CONNECTION, MB_ICONSTOP|MB_YESNO|MB_DEFBUTTON2) == IDYES)
					{
						bAllDisconnected = pDoc->DisconnectAll(FALSE);
					}
					else
					{ // keep current user online, do not a new login
					}
				}
				else
				{ // it is already user alarm, keep it, but allow new login
					bKeepUserAlarm = TRUE;
				}
			}
			if (bAllDisconnected || bKeepUserAlarm)
			{
				if (bAllDisconnected)
				{
					// Close all non server related small windows (Connection map a.o)
					CViewIdipClient* pViewIdipClient = pMainfame->GetViewIdipClient();
					if (pViewIdipClient)
					{
						pViewIdipClient->CloseWndAll();
					}
					WK_DELETE(m_pUser);
					WK_DELETE(m_pPermission);
					if (theApp.m_bConnectToLocalServer != FALSE)
					{
						theApp.m_bConnectToLocalServer = DO_CONNECT_LOCAL;
					}
				}
				Login();
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::OnUpdateNewLogin(CCmdUI *pCmdUI)
{
	BOOL bEnable = (  (m_pDlgLogin == NULL)
					&& (theApp.IsReadOnlyModus() == FALSE)
					);
	pCmdUI->Enable(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::OnEditStation() 
{
	CDlgStationName dlg(theApp.m_pMainWnd);

	dlg.m_sStationName = m_sStationName;
	if (IDOK==dlg.DoModal())
	{
		m_sStationName = dlg.m_sStationName;
		SaveSettings();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::OnFilePrintSetup() 
{
	CPrintDialog pd(TRUE);
	int nResponse = DoPrintDialog(&pd);
	if (IDOK==nResponse)
	{
		SaveSettings();
	}
}
#define CALC_PRINTER_MARGIN 1
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::OnFilePageDlg()
{
	CPageSetupDialog psd;
#ifdef CALC_PRINTER_MARGIN
	psd.m_psd.rtMargin = m_rcMargin;
	psd.m_psd.Flags |= PSD_DISABLEORIENTATION|PSD_DISABLEPAPER|PSD_DISABLEPRINTER|PSD_INHUNDREDTHSOFMILLIMETERS;//|PSD_DISABLEPAGEPAINTING;
#else
	psd.m_psd.rtMargin.left = m_rcMargin.left*10;
	psd.m_psd.rtMargin.top = m_rcMargin.top*10;
	psd.m_psd.rtMargin.right = m_rcMargin.right*10;
	psd.m_psd.rtMargin.bottom = m_rcMargin.bottom*10;
	psd.m_psd.Flags |= PSD_DISABLEORIENTATION|PSD_DISABLEPAPER|PSD_DISABLEPRINTER|PSD_INHUNDREDTHSOFMILLIMETERS;//|PSD_DISABLEPAGEPAINTING;
#endif
	if (psd.DoModal() == IDOK)
	{
#ifdef CALC_PRINTER_MARGIN
		m_rcMargin = psd.m_psd.rtMargin;
#else
		m_rcMargin.left   = MulDiv(psd.m_psd.rtMargin.left, 1, 10);
		m_rcMargin.top    = MulDiv(psd.m_psd.rtMargin.top, 1, 10);
		m_rcMargin.right  = MulDiv(psd.m_psd.rtMargin.right, 1, 10);
		m_rcMargin.bottom = MulDiv(psd.m_psd.rtMargin.bottom, 1, 10);
#endif
	}
}
/////////////////////////////////////////////////////////////////////////////
CRect CIdipClientApp::GetPrintMargin(CDC *pDC) const
{
#ifdef CALC_PRINTER_MARGIN
	CRect rc;
	int nHr   = pDC->GetDeviceCaps(HORZRES);
	int nVr   = pDC->GetDeviceCaps(VERTRES);
	int nHs   = pDC->GetDeviceCaps(HORZSIZE)*100;
	int nVs   = pDC->GetDeviceCaps(VERTSIZE)*100;
	rc.left   = MulDiv(m_rcMargin.left, nHr, nHs);
	rc.top    = MulDiv(m_rcMargin.top, nVr, nVs);
	rc.right  = MulDiv(m_rcMargin.right, nHr, nHs);
	rc.bottom = MulDiv(m_rcMargin.bottom, nVr, nVs);
	return rc;
#else
	return m_rcMargin;
#endif
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::OnUpdateFilePageDlg(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::OnSearchAtStart() 
{
	m_bSearchMaskAtStart = !m_bSearchMaskAtStart;
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::OnUpdateSearchAtStart(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bSearchMaskAtStart);
}
/////////////////////////////////////////////////////////////////////////////
CString	CIdipClientApp::GetMappedString(const CString& sKey)
{
	CString s;
	if (sKey == FIELD_DBD_STNM)
	{
		s.LoadString(IDS_STATION);
	}
	else if (sKey == FIELD_DBD_CANM)
	{
		s.LoadString(IDS_CAM_NAME);
	}
	else if (sKey == FIELD_DBD_INNM)
	{
		s.LoadString(IDS_INPUT);
	}
	else
	{
		m_FieldMap.Lookup(sKey,s);
	}
	return s;
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::AlarmConnection()
{
	if (m_bAnimateAlarmConnections)
	{
		m_bAlarmAnimation = TRUE;
	}
	if (m_pMainWnd)
	{
		if (m_pMainWnd->IsIconic())
		{
			m_pMainWnd->SendMessage(WM_SYSCOMMAND, m_pMainWnd->GetStyle() & WS_MAXIMIZE ? SC_MAXIMIZE : SC_RESTORE);
		}
		SetWindowPos(m_pMainWnd->m_hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
		SetWindowPos(m_pMainWnd->m_hWnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
		m_pMainWnd->PostMessage(WM_USER, ALARM_CONNECTION);
	}
}
////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::OnErrorMessage()
{
	CErrorMessage* pErrorMessage = m_ErrorMessages.SafeGetAndRemoveHead();

	if (pErrorMessage)
	{
		CString s;
		s.Format(IDP_ERROR,pErrorMessage->GetServer(),
			pErrorMessage->GetMessage());
		COemGuiApi::MessageBox(s,10,MB_ICONINFORMATION|MB_OK);
		WK_DELETE(pErrorMessage);
	}
}
////////////////////////////////////////////////////////////////////////////
CHostArray& CIdipClientApp::GetHosts()
{
#ifdef _DTS
	CHostArray dvhosts;
	CWK_Profile wkp;
	CWK_Profile prof(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));
	CHost* pHost;
	CHost* pNewHost;
	BOOL bChanged = FALSE;

	m_Hosts.Load(wkp);
	dvhosts.Load(prof);

	for (int i=0;i<dvhosts.GetSize();i++)
	{
		pHost = dvhosts.GetAtFast(i);
		BOOL bFound = FALSE;

		for (int j=0;j<m_Hosts.GetSize();j++)
		{
			if (m_Hosts.GetAtFast(j)->GetNumber() == pHost->GetNumber())
			{
				// Nummer gleich
				bFound = TRUE;
				if (m_Hosts.GetAtFast(j)->GetName() != pHost->GetName())
				{
					// Name verschieden
					m_Hosts.GetAtFast(j)->SetName(pHost->GetName());
					bChanged = TRUE;
				}
				break;
			}
			else if (    m_Hosts.GetAtFast(j)->GetName() == pHost->GetName()
				     && pHost->GetName() != _T("Standard"))
			{
				// Name gleich
				bFound = TRUE;
				if (m_Hosts.GetAtFast(j)->GetNumber() != pHost->GetNumber())
				{
					// Nummer verschieden
					m_Hosts.GetAtFast(j)->SetNumber(pHost->GetNumber());
					bChanged = TRUE;
				}
			}
		}

        if (!bFound)
		{
			pNewHost = m_Hosts.AddHost();
			pNewHost->SetName(pHost->GetName());
			pNewHost->SetNumber(pHost->GetNumber());
			bChanged = TRUE;
		}
	}
	if (bChanged)
	{
		m_Hosts.Save(wkp);
	}
#else
	CWK_Profile wkp;
	m_Hosts.Load(wkp);
#endif

	return m_Hosts;
}
////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::OnOptions() 
{
	CDlgOptions dlg;

	dlg.m_b1PlusAlarm = m_b1PlusAlarm;
	dlg.m_bAudibleAlarm = m_bAnimateAlarmConnections;
	dlg.m_bFullScreenAlarm = m_bFullscreenAlarm;
	dlg.m_bFirstCam = m_bFirstCam;

	if (IDOK==dlg.DoModal())
	{
		m_b1PlusAlarm = dlg.m_b1PlusAlarm;
		m_bAnimateAlarmConnections = dlg.m_bAudibleAlarm;
		m_bFullscreenAlarm = dlg.m_bFullScreenAlarm;
		m_bFirstCam = dlg.m_bFirstCam;
		SaveSettings();
	}
}
////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::OnUpdateOptions(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(   theApp.IsValidUserLoggedIn()
				   && m_pPermission 
				   && (   m_pPermission->IsSuperVisor()
				       || m_pPermission->IsSpecialReceiver()
					   )
				   && (theApp.IsReadOnlyModus() == FALSE)
				  );
}
////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::OnUserInterface()
{
	CDlgUserInterface dlg;
	if (dlg.DoModal() == IDOK)
	{
		GetMainFrame()->GetSplitterObjectView()->PostMessage(WM_USER, WM_UPDATE_ALL_VIEWS, 1);
	}
}
////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::OnUpdateUserInterface(CCmdUI *pCmdUI)
{
	OnUpdateOptions(pCmdUI);	// RKE: it probably has the same permissions
}
////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::OnViewTargetDisplay() 
{
	m_bTargetDisplay = !m_bTargetDisplay;
	if (m_pMainWnd)
	{
//		m_pMainWnd->RedrawWindow();
		CViewIdipClient*pView = ((CMainFrame*)m_pMainWnd)->GetViewIdipClient();
		if (pView)
		{
			pView->RedrawSmallWindows();
		}
	}
}
////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::OnUpdateViewTargetDisplay(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bTargetDisplay);
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::OnSequenceRealtime() 
{
	m_bCanPlayRealtime = !m_bCanPlayRealtime;
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::OnUpdateSequenceRealtime(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bCanPlayRealtime);
}
////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::CheckHostMap()
{
	CString sWWW;

	sWWW = CNotificationMessage::GetWWWRoot();
	m_sDefaultURL = _T("res://IdipClient.exe/nostatement.htm");

	if (AllowHTMLmaps())
	{
		m_sMapURL = sWWW + _T("\\map.htm");
		if (!DoesFileExist(m_sMapURL))
		{
			m_sMapURL.Empty();
		}
	}
}
////////////////////////////////////////////////////////////////////////////
CSecID CIdipClientApp::GetNextHtmlWindowID()
{
	CSecID id(SECID_HTML_WINDOWS, ++m_wHtmlWindowID);
	return id;
}
////////////////////////////////////////////////////////////////////////////
BOOL CIdipClientApp::DeleteMap()
{
	BOOL bReturn = FALSE;
	if (DoesFileExist(m_sMapURL))
	{
		TRY
		{
			CFile::Remove(m_sMapURL);
			m_sMapURL.Empty();
			bReturn = TRUE;
		}
		CATCH( CFileException, e )
		{
			WK_TRACE_ERROR(_T("File %s cannot be removed\n"), m_sMapURL);
		}
		END_CATCH
	}
	else
	{
		bReturn = TRUE;
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::InitField(CWK_Profile& wkp,const CString& sName, UINT nID)
{
	CString s;
	if (m_bReadOnlyModus)
	{
		GetPrivateProfileString(_T("Fields"),
			sName, szDontUse, s.GetBuffer(64),64,m_sOemIni);
		s.ReleaseBuffer();
	}
	else
	{
		s = wkp.GetString(szFieldMap, sName,_T(""));
	}
	
	if (s.IsEmpty())
		s.LoadString(nID);

	if (s != szDontUse)
	{
		m_FieldMap.SetAt(sName,s);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIdipClientApp::InitFields()
{
	// CAVEAT: Also called in CIPC-Thread from CServer::OnConfirmFieldInfo()
	CIPCFields fields;
	CIPCField* pField = NULL;
	if (m_bReadOnlyModus)
	{
		const DWORD dwSizeKeys = 1000;
		LPTSTR szKeysBuffer = new TCHAR[dwSizeKeys];
		DWORD dw = GetPrivateProfileString(_T("Fields"), NULL, NULL, szKeysBuffer, dwSizeKeys, m_sOemIni);
		if (dw == dwSizeKeys-1)
		{
			WK_TRACE_WARNING(_T("InitFields Buffer for keys maybe too small\n"));
		}
		LPTSTR pszKeys = szKeysBuffer;
		const DWORD dwSizeKey = 100;
		CString sKey, sValue;
		sKey = pszKeys;
		while (sKey.GetLength() > 0)
		{
			dw = GetPrivateProfileString(_T("Fields"), sKey, szDontUse, sValue.GetBuffer(dwSizeKey), dwSizeKey, m_sOemIni);
			sValue.ReleaseBuffer();
			if (dw == dwSizeKey-1)
			{
				WK_TRACE_WARNING(_T("InitFields Buffer for key maybe too small\n"));
			}
			pField = new CIPCField(sKey, sValue, 'C');
			fields.Add(pField);
			pszKeys += sKey.GetLength() + 1;
			sKey = pszKeys;
		}
		WK_DELETE(szKeysBuffer);
	}
	else
	{
		CWK_Profile wkp;
		fields.Load(SECTION_DATABASE_FIELDS, wkp);
	}

	CString sValue;
	m_FieldMap.RemoveAll();						// remove all Map Entries

	CIdipClientDoc*pDoc = GetDocument();		// insert all non local fields
	if (pDoc)
	{
		CServer *pServer;
		const CServers &Servers = pDoc->GetServers();
		int i, nServers = Servers.GetSize();
		for (i=0; i<nServers; i++)
		{
			pServer = Servers.GetAtFast(i);
			if (!pServer->IsLocal())
			{
				const CIPCFields&fieldsNL = pServer->GetFields();
				int j, nFields = fieldsNL.GetSize();
				for (j=0; j<nFields; j++)
				{
					pField = fieldsNL.GetAtFast(j);
					sValue = pField->GetValue();
					if (sValue != szDontUse)
					{
						m_FieldMap.SetAt(pField->GetName(), sValue);
					}
				}
			}
		}
	}
	//changes for VisualStudio 2005
	int i = 0;
	for (i=0 ; i<fields.GetSize() ; i++)	// insert all local fields
	{
		pField = fields.GetAtFast(i);
		sValue = pField->GetValue();			// The names are in the local defined language
		if (sValue != szDontUse)
		{
			m_FieldMap.SetAt(pField->GetName(), sValue);
		}
	}
	return (fields.GetSize() > 0);
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::InitFieldMap()
{
	CWK_Profile wkp;

	if (!m_bReadOnlyModus)
	{
		BOOL bFields = FALSE;
		CWK_Dongle dongle;
		InternalProductCode ipc = dongle.GetProductCode();

		switch (ipc)
		{
		case IPC_STORAGE:				// FALLTHROUGH
		case IPC_STORAGE_PLUS:			// FALLTHROUGH
		case IPC_STORAGE_TRANSMITTER:	// FALLTHROUGH
		case IPC_INSPICIO:				// FALLTHROUGH
		case IPC_INSPECTUS:				// FALLTHROUGH
		case IPC_VIDEO_MASTER:			// FALLTHROUGH
		case IPC_ATM_SURVEILENCE:
			bFields = TRUE;
			break;
		}
		if (dongle.RunSDIUnit())
		{
			bFields = TRUE;
		}

		if (!bFields)
		{
			return;
		}
	}
	////////////////////
	// variable fields
	////////////////////

	// first load names from
	if (InitFields())
	{
	}
	else
	{
		// Kontonummer
		InitField(wkp,CIPCField::m_sfKtNr,IDS_KTO_NR);
		// Geldautomaten Zeit
		InitField(wkp,CIPCField::m_sfTime,IDS_DTP_TIME);
		// Geldautomaten Datum
		InitField(wkp,CIPCField::m_sfDate,IDS_DTP_DATE);
		// Geldautomaten Nummer
		InitField(wkp,CIPCField::m_sfGaNr,IDS_GA_NR);
		// Transaktion Nummer
		InitField(wkp,CIPCField::m_sfTaNr,IDS_TA_NR);
		// Bankleitzahl
		InitField(wkp,CIPCField::m_sfBcNr,IDS_BANKLEITZAHL);
		// Betrag
		InitField(wkp,CIPCField::m_sfAmnt,IDS_VALUE);
		// Waehrung
		InitField(wkp,CIPCField::m_sfCurr,IDS_CURRENCY);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::OnButtonSystem()
{
	StartApplication(WAI_SUPERVISOR, 0);
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::OnLogview()
{
	theApp.StartApplication(WAI_LOG_VIEW, 0);
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::OnProductview()
{
	theApp.StartApplication(WAI_PRODUCT_VIEW, 0);
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::OnExplorer()
{
	theApp.StartApplication(WAI_LAUNCHER, LAUNCHER_START_EXPLORER);
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::OnShell()
{
	if (m_bShellModus)
	{
		theApp.StartApplication(WAI_LAUNCHER, LAUNCHER_START_NOT_AS_SHELL);
	}
	else if (AfxMessageBox(IDS_START_IN_SHELL_MODE, MB_ICONWARNING|MB_YESNO) == IDYES)
	{
		theApp.StartApplication(WAI_LAUNCHER, LAUNCHER_START_AS_SHELL);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::OnRuntimeError()
{
	theApp.StartApplication(WAI_LAUNCHER, LAUNCHER_RUNTIME_ERROR_DLG);
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::OnSoftwareUpdate()
{
	StartApplication(WAI_LAUNCHER , LAUNCHER_SOFTWARE_UPDATE);
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::OnExtensionCode()
{
	StartApplication(WAI_LAUNCHER, LAUNCHER_EXTENSION_CODE);
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::OnUpdateButtonSystem(CCmdUI *pCmdUI)
{
	BOOL bEnable = (   IsValidUserLoggedIn()
					&& (WK_IS_RUNNING(WK_APP_NAME_LAUNCHER)));
	pCmdUI->Enable(bEnable);
}

void CIdipClientApp::OnUpdateSoftwareUpdate(CCmdUI *pCmdUI)
{
	OnUpdateExtensionCode(pCmdUI);
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::OnUpdateExtensionCode(CCmdUI *pCmdUI)
{

	BOOL bEnable = 	IsValidUserLoggedIn()
					&& (WK_IS_RUNNING(WK_APP_NAME_LAUNCHER))
					&& theApp.m_pPermission 
        			&& theApp.m_pPermission->IsSuperVisor();

	pCmdUI->Enable(bEnable);
}
////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::OnUpdateLogview(CCmdUI* pCmdUI) 
{
	OnUpdateButtonSystem(pCmdUI);
}
////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::OnUpdateProductview(CCmdUI* pCmdUI) 
{
	OnUpdateButtonSystem(pCmdUI);
}
////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::OnUpdateExplorer(CCmdUI* pCmdUI) 
{
	OnUpdateButtonSystem(pCmdUI);
}
////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::OnUpdateShell(CCmdUI* pCmdUI) 
{
	OnUpdateButtonSystem(pCmdUI);
}
////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::OnUpdateRuntimeError(CCmdUI* pCmdUI) 
{
	OnUpdateButtonSystem(pCmdUI);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIdipClientApp::StartApplication(WK_ApplicationId wai, WORD wStartParam)
{
	HWND hWnd = NULL;
	if (WK_IS_RUNNING(WK_APP_NAME_LAUNCHER))
	{
		hWnd = FindWindow(WK_APP_NAME_LAUNCHER, NULL);
	}
	if (hWnd)
	{
		WORD wUserID=SECID_NO_SUBID;
		if ( m_pUser)						// User vorhanden
		{
			wUserID = m_pUser->GetID().GetSubID();
			switch (wai)
			{
				case WAI_IDIP_CLIENT:						// for language change no login
					break;
				case WAI_LOG_VIEW: case WAI_SUPERVISOR:		// login necessary
//				if (m_pPermission == NULL || !m_pPermission->IsSuperVisor())
				{
					wUserID = SECID_NO_SUBID;				// only Supervisor starts directly
				}break;
				case WAI_LAUNCHER:
					switch(wStartParam)
					{
						case LAUNCHER_DATE_TIME_DLG:		// setting dat and time
						case LAUNCHER_RUNTIME_ERROR_DLG:	// start runtime error dialog
						if (m_pPermission == NULL || !m_pPermission->IsSuperVisor())
						{
							wUserID = SECID_NO_SUBID;		// only Supervisor calls directly
						}break;
						case 0:								// stopp the Launcher
						case LAUNCHER_START_EXPLORER:		// start explorer
						case LAUNCHER_START_NOT_AS_SHELL:	// start in explorer mode
						case LAUNCHER_START_AS_SHELL:		// start in shell mode
							wUserID = SECID_NO_SUBID;		// login necessary
							break;
					}
					break;
			}
		}

		LPARAM lParam = MAKELONG(wStartParam, wUserID);
		WK_TRACE(_T("StartApplication %s\n"), NameOfEnum(wai));
		if (   wStartParam == LAUNCHER_APPLICATION_RUNNING
			|| wStartParam == LAUNCHER_IS_SHELL)
		{
			DWORD dwResult = 0;
			if (::SendMessageTimeout(hWnd, LAUNCHER_APPLICATION, wai, lParam, SMTO_NORMAL, 5000, &dwResult))
			{
				return (BOOL)dwResult;
			}
			return FALSE;
		}
		else
		{
			::PostMessage(hWnd, LAUNCHER_APPLICATION, wai, lParam);
			::SetForegroundWindow(hWnd);
		}
		return TRUE;
	}
	else
	{
		WK_TRACE_WARNING(_T("StartApplication %s, Launcher not found\n"), NameOfEnum(wai));
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
int CIdipClientApp::DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt)
{
	CString sMsg;
	if (lpszPrompt)
	{
		sMsg = lpszPrompt;
		if (sMsg.IsEmpty())
		{
/*			_AFX_THREAD_STATE* pTS = _afxThreadState.GetData();
			WK_TRACE_ERROR(_T("DoMessageBox: empty message:\n LastError:%d\nLastMessage: %d, %08x, %08x\n"),
				GetLastError(),
				pTS->m_lastSentMsg.message,
				pTS->m_lastSentMsg.wParam,
				pTS->m_lastSentMsg.lParam);
*/
			WK_TRACE_ERROR(_T("DoMessageBox: empty message, LastError:%d\n"), GetLastError());
			CUnhandledException::TraceCallStack(NULL);
			return IDCANCEL;
		}
	}
	else if (nIDPrompt)
	{
		sMsg.LoadString(nIDPrompt);
	}
	return COemGuiApi::MessageBox(sMsg, 0, nType);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIdipClientApp::IsValidUserLoggedIn() 
{
	return (m_pUser && !m_pUser->IsUserAlarm()) ? TRUE : FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIdipClientApp::IsUserAlarm()
{
	return (m_pUser && m_pUser->IsUserAlarm()) ? TRUE : FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::OnAppExit()
{
	// RKE: This message comes from the launcher window!!
	// the function max be called directly to close the program immediatly
	// in debug or read only mode
	m_dwClosing |= LAUNCHER_IS_CLOSING;

	CWinApp::OnAppExit();					// exit	
}
void CIdipClientApp::OnExitClient()
{
	// RKE: this message comes from menu, toolbar or buttons

#ifdef _DEBUG								// in debug mode
	OnAppExit();							// simply exit	
#else										// in release
	if (m_bReadOnlyModus)					// ReadOnly
	{
		OnAppExit();						// simply exit	
		return;
	}

	CMainFrame*pMF = GetMainFrame();		// Request Reference Picture completion
	if (pMF)
	{
		CDocument*pDoc = pMF->GetActiveDocument();
		if (pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CIdipClientDoc)))
		{
			CServer *pLocal = ((CIdipClientDoc*)pDoc)->GetLocalServer();
			if (pLocal)
			{
				CIPCOutputIdipClient*pOut = pLocal->GetOutput();
				if (pOut && !pOut->ReferenceComplete())
				{
					if (IDNO==AfxMessageBox(IDP_NOT_ALL_REFERENCE,MB_YESNO))
					{
						return;				// Do nothing if the answer is no
					}
				}
			}
		}
	}

	if (AfxMessageBox(IDS_CLOSE_APP_REQUEST, MB_YESNO|MB_ICONQUESTION) == IDNO)
	{										
		return;								// Do nothing if the answer is no
	}
											// stop the launcher (password request !!)
	BOOL bStopSent = StartApplication(WAI_LAUNCHER, 0);	
	if (!bStopSent)							// no launcher
	{
		OnAppExit();						// simply exit	
	}
#endif
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::OnUpdateAppExit(CCmdUI *pCmdUI)
{	
	BOOL bEnable = TRUE;
	CViewIdipClient* pView = GetMainFrame()->GetViewIdipClient();
	if(pView)
	{
		CDlgBackup* pBackupDlg = pView->GetDlgBackup();
		if(pBackupDlg)
		{
			if(pBackupDlg->IsKindOf(RUNTIME_CLASS(CDlgBackupNew)))
			{
				bEnable = FALSE;
			}
		}
	}

	pCmdUI->Enable(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::LoginOK()
{
	WK_DELETE(m_pUser);
	WK_DELETE(m_pPermission);
	m_pUser       = new CUser(m_pDlgLogin->GetUser());
	m_pPermission = new CPermission(m_pDlgLogin->GetPermission());
	if (m_pUser->GetName().IsEmpty())
	{
		m_pUser->SetName(_T("EmptyUserName"));
	}

	BOOL bLanguageChanged = m_pDlgLogin->GetLanguageChanged();
	WK_DESTROY_WINDOW(m_pDlgLogin);

	if (bLanguageChanged)	// wenn sich die Sprache geändert hat:
	{	// werden alle mehrsprachigen Programme geschlossen
		if (StartApplication(WAI_SUPERVISOR, LAUNCHER_APPLICATION_RUNNING))
		{
			StartApplication(WAI_SUPERVISOR, LAUNCHER_TERMINATE_APPLICATION);
		}
		if (StartApplication(WAI_LOG_VIEW, LAUNCHER_APPLICATION_RUNNING))
		{
			StartApplication(WAI_LOG_VIEW, LAUNCHER_TERMINATE_APPLICATION);
		}
		if (StartApplication(WAI_PRODUCT_VIEW, LAUNCHER_APPLICATION_RUNNING))
		{
			StartApplication(WAI_PRODUCT_VIEW, LAUNCHER_TERMINATE_APPLICATION);
		}
		if (StartApplication(WAI_PRODUCT_VERSION, LAUNCHER_APPLICATION_RUNNING))
		{
			StartApplication(WAI_PRODUCT_VERSION, LAUNCHER_TERMINATE_APPLICATION);
		}
		// auch der Client selbst
		if (StartApplication(WAI_IDIP_CLIENT, 0))
		{
			return;
		}
	}

	if (IsValidUserLoggedIn())
	{
		CMainFrame* pMainFrame = GetMainFrame();
		if (pMainFrame)
		{
			pMainFrame->ShowMenu(TRUE);
			CViewIdipClient* pViewClient = pMainFrame->GetViewIdipClient();
			if (pViewClient && m_bShowHostMapAtLoggin)
			{
				pViewClient->ShowMapWindow();
			}
			
			IsDataBasePresent(TRUE); // determine data base presence

			if	(m_bConnectToLocalServer == DO_CONNECT_LOCAL)
			{	// nach maximal 2 Sekunden wird die Verbindung hergestellt.
				pMainFrame->m_nTimerCounter = 3;
			}
			pMainFrame->InitJoystick();
		}
	}
	LoadUserSpecificSettings();
	m_AutoLogout.ResetApplication();
	if (GetIntValue(_T("AddDebugWindow")))
	{
		GetMainFrame()->GetViewIdipClient()->PostMessage(WM_USER, WPARAM_INSERT_DEBUGGER_WINDOW);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::DoReset()
{
	m_bDoReset = TRUE;
	GetMainFrame()->SetPanetext(7, _T("Reset"));
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::SetResetting()
{
	if (m_bDoReset != TRUE) // RKE: state TRUE is to change in OnIdle
	{
		GetMainFrame()->SetPanetext(7, _T("Reset"));
		m_bDoReset = 2;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::EndResetting()
{
	GetMainFrame()->SetPanetext(7, _T(""));
	m_bDoReset = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::Reset()
{
	WK_TRACE(_T("reset idip client\n"));
	CWK_Profile wkp;

	CMainFrame *pMF = GetMainFrame();
	m_AutoLogout.Load(wkp);
	m_AutoLogout.ResetApplication();

	CIdipClientDoc*pDoc = pMF->GetDocument();
	pDoc->SetAutoLogout(m_AutoLogout.GetAutoApplication());
	((CServers*)&pDoc->GetServers())->Reset();

	LoadSettings();

	DWORD dwFlags = wkp.GetInt(IDIP_CLIENT_SETTINGS, USE_MONITOR, m_dwMonitorFlags);
	if (m_dwMonitorFlags != dwFlags)
	{
		if (  (dwFlags == USE_MONITOR_1 && m_dwMonitorFlags == USE_MONITOR_2)
			||(dwFlags == USE_MONITOR_2 && m_dwMonitorFlags == USE_MONITOR_1))
		{
			pMF->PostMessage(WM_DISPLAYCHANGE, dwFlags);
		}
		else if (   (dwFlags == USE_MONITOR_1   && m_dwMonitorFlags == USE_MONITOR_1_2)
				 || (dwFlags == USE_MONITOR_1_2 && m_dwMonitorFlags == USE_MONITOR_1))
		{
			pMF->PostCommand(ID_VIEW_MONITOR2);
		}
		else 
		{
			pMF->PostCommand(ID_VIEW_MONITOR2);
			pMF->PostMessage(WM_DISPLAYCHANGE, dwFlags);
		}
	}

	pMF->InitJoystick();

	IsDataBasePresent(TRUE);	// determine presence of database
	InitFieldMap();				// database fields
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::AutoLogout()
{
	// Attention called from OnIdle
	if (  !IsReadOnlyModus()
		&& IsValidUserLoggedIn()
		&& m_AutoLogout.IsApplicationOver())
	{
		if (m_pDongle->IsDemo())
		{
			m_pMainWnd->PostMessage(WM_COMMAND, ID_FORCE_NEW_LOGIN, 0L);
		}
		else
		{
			if (theApp.m_pMainWnd->IsIconic())
			{
				theApp.m_pMainWnd->PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
			}
			SetForegroundWindow(theApp.m_pMainWnd->m_hWnd);
			COemGuiApi::AutoExitDialog(theApp.m_pMainWnd, &m_AutoLogout, WM_COMMAND, ID_FORCE_NEW_LOGIN, 0L);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
int CIdipClientApp::GetIntValue(LPCTSTR sKey, int nDefault/*=0*/)
{
	CWK_Profile wkp;
	int bIntValue = wkp.GetInt(IDIP_CLIENT_SETTINGS, sKey, -1);
	if (bIntValue == -1)
	{
		bIntValue = nDefault;
		wkp.WriteInt(IDIP_CLIENT_SETTINGS, sKey, bIntValue);
	}
	return bIntValue;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIdipClientApp::IsDataBasePresent(BOOL bDetermine/*=FALSE*/) 
{
	if (bDetermine)
	{
		CIPCDrives drives;
		CWK_Profile wkp;
		drives.Init(wkp);
		m_bIsDatabasePresent = drives.GetMB() > 0 ? TRUE : FALSE;
	}
	return m_bIsDatabasePresent;
}
/////////////////////////////////////////////////////////////////////////////
CIdipClientDoc*	CIdipClientApp::GetDocument()
{
	CMainFrame *pMF = GetMainFrame();
	if (pMF)
	{
		return pMF->GetDocument();
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CServer* CIdipClientApp::GetServer(WORD wID)
{
	CIdipClientDoc*pDoc = GetDocument();
	if (pDoc)
	{
		return pDoc->GetServer(wID);
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::CalcCPUusage()
{
	if (NtQuerySystemInformation)
	{
		SYSTEM_PERFORMANCE_INFORMATION SysPerfInfo;
		SYSTEM_TIME_INFORMATION        SysTimeInfo;
		double                         dbIdleTime;
		double                         dbSystemTime;
		LONG                           status;
		static FILETIME ftLastKernel, ftLastUser;
		FILETIME ftCreate, ftExit, ftKernel, ftUser;
		GetProcessTimes(GetCurrentProcess(), &ftCreate, &ftExit, &ftKernel, &ftUser);

        // get new system time
	    status = NtQuerySystemInformation(SystemTimeInformation,&SysTimeInfo,sizeof(SysTimeInfo),0);
        if (status!=NO_ERROR)
            return ;

        // get new CPU's idle time
        status = NtQuerySystemInformation(SystemPerformanceInformation,&SysPerfInfo,sizeof(SysPerfInfo),NULL);
        if (status != NO_ERROR)
            return ;

			// if it's a first call - skip it
		if (m_liOldIdleTime.QuadPart != 0)
		{
			// CurrentValue = NewValue - OldValue
			dbIdleTime = Li2Double(SysPerfInfo.liIdleTime) - Li2Double(m_liOldIdleTime);
			dbSystemTime = Li2Double(SysTimeInfo.liKeSystemTime) - Li2Double(m_liOldSystemTime);

			if (GetMainFrame()->HasStatusBar())
			{
				DWORD dwKernel = ftKernel.dwLowDateTime - ftLastKernel.dwLowDateTime;
				DWORD dwUser = ftUser.dwLowDateTime - ftLastUser.dwLowDateTime;
				double dProcess = (dwKernel + dwUser) / dbSystemTime * 100.0 / (double)m_wNoOfProcessors;
				GetMainFrame()->PostMessage(WM_USER, MAKELONG(WPARAM_PANE_INT, 8), (LPARAM)(dProcess+0.5));
			}

			// CurrentCpuIdle = IdleTime / SystemTime
			dbIdleTime = dbIdleTime / dbSystemTime;

			// CurrentCpuUsage% = 100 - (CurrentCpuIdle * 100) / NumberOfProcessors
			dbIdleTime = 100.0 - dbIdleTime * 100.0 / (double)m_wNoOfProcessors;

			m_wCPUusage = (WORD)(dbIdleTime + 0.5);
			if (m_AvgCPUusage.GetNoOfAvgValues())
			{
				m_AvgCPUusage.AddValue(m_wCPUusage);
			}
			if (m_bShowCPUusage & CPU_USAGE_LOG_VALUE)
			{
				WK_STAT_LOG(_T("PM"), m_wCPUusage, _T("CPUUsage"));
			}
		}

		// store new CPU's idle and system time
		m_liOldIdleTime = SysPerfInfo.liIdleTime;
		m_liOldSystemTime = SysTimeInfo.liKeSystemTime;

		ftLastKernel = ftKernel;
		ftLastUser = ftUser;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::CheckPerformanceLevel()
{
	int nAvgCUPusage = (int)GetAvgCPUusage();
	if (nAvgCUPusage == -1)
	{
		return;
	}
	int nPerformanceBias = 0;
	if (nAvgCUPusage > (int)m_wUpperThreshold)
	{
		nPerformanceBias = nAvgCUPusage - m_wUpperThreshold;
	}
	else if (nAvgCUPusage < (int)m_wLowerThreshold)
	{
		nPerformanceBias = nAvgCUPusage - m_wLowerThreshold;
	}

	if (nPerformanceBias > 0 && m_nPerformanceLevel < MAX_PERFORMANCE_LEVEL)
	{
		m_nPerformanceLevel++;
		if (m_bShowCPUusage & CPU_USAGE_TRACE_CHANGE)
		{
			WK_TRACE(_T("increased PerformanceLevel:%d\n"), m_nPerformanceLevel);
		}
	}
	else if (nPerformanceBias < 0 && m_nPerformanceLevel > m_nMinPerformanceLevel)
	{
		m_nPerformanceLevel--;
		if (m_bShowCPUusage & CPU_USAGE_TRACE_CHANGE)
		{
			WK_TRACE(_T("decreased PerformanceLevel:%d\n"), m_nPerformanceLevel);
		}
	}
	else
	{
		nPerformanceBias = 0;
	}

	int nLevelCorrVal = CWndMpeg4::CheckPerformanceLevel(nPerformanceBias);
	if (nLevelCorrVal < 0)
	{
		m_nPerformanceLevel = (signed short)(m_nPerformanceLevel - nLevelCorrVal);
		if (m_bShowCPUusage & CPU_USAGE_TRACE_CHANGE)
		{
			WK_TRACE(_T("Corrected PerformanceLevel by %d\n"), nLevelCorrVal);
		}
		nPerformanceBias = 1;
	}
	if (nPerformanceBias != 0)
	{
		GetMainFrame()->GetViewIdipClient()->CalculateRequestedFPS(NULL);
	}

	if (m_bShowCPUusage & CPU_USAGE_LOG_AVG)
	{
		WK_STAT_LOG(_T("PM"), nAvgCUPusage, _T("AvgCPUUsage"));
	}
	ASSERT(IsBetween(m_nPerformanceLevel, m_nMinPerformanceLevel, MAX_PERFORMANCE_LEVEL));
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientApp::SetPerformanceLevel(short nLevel)
{
	if (IsBetween(nLevel, m_nMinPerformanceLevel, MAX_PERFORMANCE_LEVEL))
	{
		if (m_bShowCPUusage & CPU_USAGE_TRACE_CHANGE)
		{
			WK_TRACE(_T("Corrected PerformanceLevel to %d, %d\n"), nLevel, m_nPerformanceLevel);
		}
		if (nLevel != m_nPerformanceLevel)
		{
			m_nPerformanceLevel = nLevel;
			CMainFrame*pMF = theApp.GetMainFrame();
			pMF->GetViewDlgBar()->GetDlgInView()->PostMessage(WM_TIMER, pMF->GetOneSecTimerID(), 0);
		}
	}
	else if (nLevel < m_nMinPerformanceLevel)
	{
		m_nPerformanceLevel = m_nMinPerformanceLevel;
		CMainFrame*pMF = theApp.GetMainFrame();
		pMF->GetViewDlgBar()->GetDlgInView()->PostMessage(WM_TIMER, pMF->GetOneSecTimerID(), 0);
	}
}
