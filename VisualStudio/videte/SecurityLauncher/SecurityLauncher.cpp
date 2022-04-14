/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SecurityLauncher.cpp $
// ARCHIVE:		$Archive: /Project/SecurityLauncher/SecurityLauncher.cpp $
// CHECKIN:		$Date: 27.07.06 11:59 $
// VERSION:		$Revision: 329 $
// LAST CHANGE:	$Modtime: 27.07.06 9:31 $
// BY AUTHOR:	$Author: Tomas.krogh $
// $Nokeywords:$

// CheckIn Test, promote to 1998

#include "stdafx.h"
#include "SecurityLauncher.h"

#include "Mainframe.h"
#include "LauncherControl.h"

#include "LoginDialog.h"

#include "BackGroundWnd.h"
#include "BeforeBackupDialog.h"
#include "AfterBackupDialog.h"
#include "ExtensionCodeDlg.h"

#include "UpdateRecord.h"

#include "ErrorDialog.h"
#include "oemgui\oemgui.h"
#include "oemgui\oemfiledialog.h"
#include "wkclasses\CLogFile.h"

#include "CIPCInputVirtualAlarm.h"
#include ".\securitylauncher.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
static TCHAR BASED_CODE szGPHour[] = _T("GPHour");
static TCHAR BASED_CODE szGPDay[] = _T("GPDay");
static TCHAR BASED_CODE szRTEHour[] = _T("RTEHour");
static TCHAR BASED_CODE szRTEDay[] = _T("RTEDay");
static TCHAR BASED_CODE szVersion[] = _T("Version");
static TCHAR BASED_CODE szModules[] = _T("SecurityLauncher\\Modules");
static TCHAR BASED_CODE szSection[] = _T("SecurityLauncher");
static TCHAR BASED_CODE szShutdown[] = _T("Shutdown");
static TCHAR BASED_CODE szActualize[] = _T("Actualize");
static TCHAR BASED_CODE szLogonNT[] = _T("Microsoft\\Windows NT\\CurrentVersion");
static TCHAR BASED_CODE szWinlogon[] = _T("Winlogon");

#define VCD_RUN_MODULE			_T("RunVideteCheckDisk")
#define REGISTERMEDIAFILETYPES	_T("RegisterMediaFileTypes")
#define SHOW_CRT_ERROR_AS_RTE	_T("ShowCrtErrorAsRte")
#define SHOW_RTE				_T("ShowRuntimeErrors")
#define POPUP_RTE				_T("PopupAt")

#define SHOW_CRITICAL	0x00000001
#define SHOW_ERRORS		0x00000010
#define SHOW_WARNINGS	0x00000100
#define SHOW_MESSAGES	0x00001000

/////////////////////////////////////////////////////////////////////////////
// Applications that are shown as buttons in shellmode on the launcher background window
WK_ApplicationId CSecurityLauncherApp::gm_AppButtons[]  = {WAI_IDIP_CLIENT, WAI_LOG_VIEW, WAI_PRODUCT_VIEW, WAI_PRODUCT_VERSION, WAI_SUPERVISOR, WAI_SIMUNIT};
// Flag whether they are shown only, if they are started
BOOL		     CSecurityLauncherApp::gm_bAppStarted[] = {FALSE          , FALSE       , FALSE           , FALSE              , FALSE         , TRUE};

/////////////////////////////////////////////////////////////////////////////
// The one and only CSecurityLauncherApp object
CSecurityLauncherApp theApp;
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSecurityLauncherApp, CWinApp)
	//{{AFX_MSG_MAP(CSecurityLauncherApp)
	ON_COMMAND(ID_DATABASE, OnDatabase)
	ON_UPDATE_COMMAND_UI(ID_DATABASE, OnUpdateDatabase)
	ON_COMMAND(ID_VISION, OnVision)
	ON_UPDATE_COMMAND_UI(ID_VISION, OnUpdateVision)
	ON_COMMAND(ID_SUPERVISOR, OnSupervisor)
	ON_UPDATE_COMMAND_UI(ID_SUPERVISOR, OnUpdateSupervisor)
	ON_COMMAND(ID_SPACE, OnSpace)
	ON_COMMAND(ID_ERROR, OnError)
	ON_COMMAND(ID_PRODUCT_VIEW, OnProductView)
	ON_COMMAND(ID_BACKUP, OnBackup)
	ON_COMMAND(ID_DISK, OnDisk)
	ON_COMMAND(ID_VERSION, OnVersion)
	ON_COMMAND(ID_SDI_CONFIG, OnSDIConfig)
	ON_UPDATE_COMMAND_UI(ID_PRODUCT_VIEW, OnUpdateProductView)
	ON_UPDATE_COMMAND_UI(ID_VERSION, OnUpdateVersion)
	ON_UPDATE_COMMAND_UI(ID_SDI_CONFIG, OnUpdateSdiConfig)
	ON_COMMAND(ID_EXPLORER, OnExplorer)
	ON_COMMAND(ID_EXPLORER_SHELL, OnExplorerShell)
	ON_COMMAND(ID_LAUNCHER_SHELL, OnLauncherShell)
	ON_COMMAND(ID_RESTART, OnRestart)
	ON_COMMAND(ID_LOG_VIEW, OnLogView)
	ON_UPDATE_COMMAND_UI(ID_LOG_VIEW, OnUpdateLogView)
	ON_COMMAND(ID_DATETIME, OnDatetime)
	ON_COMMAND(ID_EURO_CHANGE, OnEuroChange)
	ON_COMMAND(ID_IMAGE_COMPARE, OnImageCompare)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_COMPARE, OnUpdateImageCompare)
	ON_COMMAND(ID_UPDATE_HANDLER, OnUpdateHandler)
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_SOFTWARE_UPDATE, OnSoftwareUpdate)
	ON_UPDATE_COMMAND_UI(ID_SOFTWARE_UPDATE, OnUpdateSoftwareUpdate)
	ON_COMMAND(ID_EXTENSION_CODE, OnExtensionCode)
	ON_UPDATE_COMMAND_UI(ID_EXTENSION_CODE, OnUpdateExtensionCode)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CSecurityLauncherApp construction
CSecurityLauncherApp::CSecurityLauncherApp()
	: m_TemporaryUserID(SECID_NO_ID)

{
	XTIB::SetThreadName(_T("MainThread"));
	WK_CheckTempDirectory();
	m_wWatchDogTime	   = 60;
	m_bRun             = FALSE;
	m_Mode			   = LM_NORMAL;
	m_pErrorThread     = NULL;
	m_pLauncherControl = NULL;
	m_iActualize       = 0;
	m_dwGPCounterHour  = 0;
	m_dwRTECounterHour = 0;
	m_dwGPCounterDay   = 0;
	m_dwRTECounterDay  = 0;

	// shell stuff
	m_bRebootAtExit    = FALSE;
	m_bShutDownAtExit  = FALSE;
	m_bShutDownAtExitShell = FALSE;
	m_ppBackGroundWnd   = NULL;
	CMonitorInfo MonitorInfo;
	m_nMonitors        = MonitorInfo.GetNrOfMonitors();
	m_bAmIShell        = FALSE;

	m_pErrorDialog = NULL;
	m_dwShowErrors	= SHOW_CRITICAL|SHOW_ERRORS|SHOW_WARNINGS|SHOW_MESSAGES;
	m_dwShowErrors  = MAKELONG(LOWORD(m_dwShowErrors),LOWORD(m_dwShowErrors));
	m_dwShowErrors = GetIntValue(SHOW_RTE, m_dwShowErrors);
	m_pDongle	   = NULL;
	m_iAlarmPercent = 0;
	m_iISDNBChannel = 0;
	m_hHook = NULL;
	m_bShallReboot = FALSE;

	GetLocalTime(&m_SystemTime);

	m_pCIPCVirtualAlarm = NULL;
	m_bUseUpdateHandler = FALSE;
	m_bDateTimeDlgStarted = FALSE;
	m_pnLastTemperatures = NULL;

	m_nMaxCheckedDrives = GetIntValue(MAX_CHECKED_DRIVES, 7);
	m_pnLastTemperatures = new int[m_nMaxCheckedDrives];
	m_bWriteErrorFile = FALSE;

	for (int i=0; i<m_nMaxCheckedDrives; i++)
	{
		m_pnLastTemperatures[i] = -1;
	}

	m_dwRTE_TemperatureBits = 0;
	m_dwRTE_FanBits = 0;
}
/////////////////////////////////////////////////////////////////////////////
CSecurityLauncherApp::~CSecurityLauncherApp()
{
	m_newErrors.SafeDeleteAll();
	m_Errors.SafeDeleteAll();
	WK_DELETE(m_pnLastTemperatures);
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::RegisterWindowClass()
{
	WNDCLASS  wndclass;

	// register main window class
	wndclass.style =         CS_DBLCLKS|CS_HREDRAW;
	wndclass.lpfnWndProc =   DefWindowProc;
	wndclass.cbClsExtra =    0;
	wndclass.cbWndExtra =    0;
	wndclass.hInstance =     m_hInstance;
	wndclass.hIcon =         LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME));
	wndclass.hCursor =       LoadStandardCursor(IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1) ;
	wndclass.lpszMenuName =  0L;
	wndclass.lpszClassName = WK_APP_NAME_LAUNCHER;

	AfxRegisterClass(&wndclass);


	if (theApp.m_pDongle)
	{
		switch (theApp.m_pDongle->GetOemCode())
		{
		case CWK_Dongle::OEM_IDIP:
			m_BkGndBrush.CreateSolidBrush(SKIN_COLOR_VIDETE_CI);
			m_cTextColor = SKIN_COLOR_GREY;
			break;
		case CWK_Dongle::OEM_SANTEC:
		case CWK_Dongle::OEM_VDR:
		case CWK_Dongle::OEM_ALARMCOM:
		case CWK_Dongle::OEM_PROTECTION_ONE:
		case CWK_Dongle::OEM_DRESEARCH:
		case CWK_Dongle::OEM_SIEMENS:
			m_BkGndBrush.CreateSolidBrush(RGB(255,255,255));
			m_cTextColor = SKIN_COLOR_BLUE;
			break;
		default:
			m_BkGndBrush.CreateSolidBrush(RGB(0,0,0));
			m_cTextColor = SKIN_COLOR_GOLD_METALLIC;
			break;
		}
	}
	// register background window class
	wndclass.style =         CS_DBLCLKS|CS_HREDRAW;
	wndclass.lpfnWndProc =   DefWindowProc;
	wndclass.cbClsExtra =    0;
	wndclass.cbWndExtra =    0;
	wndclass.hInstance =     m_hInstance;
	wndclass.hIcon =         LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME));
	wndclass.hCursor =       LoadStandardCursor(IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) m_BkGndBrush;
	wndclass.lpszMenuName =  0L;
	wndclass.lpszClassName = BACKGROUND_WND_CLASS;

	AfxRegisterClass(&wndclass);
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::IncreaseGPCounter()
{
	WK_STAT_LOG(_T("GP"),++m_dwGPCounterDay,szGPDay);
	WK_STAT_LOG(_T("GP"),++m_dwGPCounterHour,szGPHour);

	if ((m_dwGPCounterHour>5) || (m_dwGPCounterDay>20))
	{
		ErrorReboot(); // exits windows and app
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::IncreaseRTECounter()
{
	WK_STAT_LOG(_T("RT"),++m_dwRTECounterDay,szRTEDay);
	WK_STAT_LOG(_T("RT"),++m_dwRTECounterHour,szRTEHour);
	if ((m_dwRTECounterHour>2) || (m_dwRTECounterDay>8))
	{
		ErrorReboot(); // exits windows and app
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSecurityLauncherApp::TimeCheck()
{
	CSystemTime st;
	CWK_Profile wkp;

	st.GetLocalTime();

	if (st.wYear>=2038)
	{
		// OOPS our Software doesn't work after 18.01.2038
		// let's find out the correct year
		st.wYear = (WORD)wkp.GetInt(szSection,_T("Year"),1999);
		if (st.wYear>=2038)
		{
			st.wYear = 1999;
		}
		SetLocalTime(&st);
		WK_TRACE(_T("corrected time\n"));
		PostMessage(HWND_BROADCAST,WM_TIMECHANGE,0,0);
		return FALSE;
	}
	else
	{
		wkp.WriteInt(szSection,_T("Year"),st.wYear);
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::GetPagingDrive()
{
	CString sPageFilePath;
	CString sPageFile;

	if (IsWin95())
	{
		sPageFile =  _T("win386.swp");
		sPageFilePath = m_sWindowsDirectory + _T("\\") + sPageFile;
	}
	else if (IsNT())
	{
		sPageFile = _T("pagefile.sys");
		sPageFilePath = m_sWindowsDirectory.Left(3) + sPageFile;
	}
	else
	{
		// unknown OS
	}

	if (DoesFileExist(sPageFilePath))
	{
		m_sPagingFile = sPageFilePath;
	}
	else
	{
		// RKE: sucht das PageFile zuerst im Root auf den fixed drives !
		// Dies ist bei meinem Rechner nötig, da es auf e: liegt.
		// Er sucht sonst ewig nach dem Pfad.
		DWORD dwDrives = GetLogicalDrives();
		DWORD dwDrive  = 4; // _T('C')
		CString sRoot;
		char c;

		for (dwDrive=4,c=_T('c');dwDrive!=0;dwDrive<<=1,c++)
		{
			if (dwDrive & dwDrives)
			{
				sRoot.Format(_T("%c:\\"),c);
				if (DRIVE_FIXED == GetDriveType(sRoot))
				{
					sPageFilePath = sRoot + sPageFile;
					if (DoesFileExist(sPageFilePath))
					{
						m_sPagingFile = sPageFilePath;
						break;
					}
				}
			}
		}
	}

	if (m_sPagingFile.IsEmpty())
	{
		SearchFileRecursiv(NULL,sPageFile,m_sPagingFile);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::InitPathes()
{
	CString sPath;
	GetModuleFileName(m_hInstance,sPath.GetBuffer(MAX_PATH), MAX_PATH);
	sPath.ReleaseBuffer();
	// set important dir informations only once
	int p;

	m_sWorkingDirectory = sPath;
	p = m_sWorkingDirectory.ReverseFind(_T('\\'));
	if (p!=-1)
	{
		m_sWorkingDirectory = m_sWorkingDirectory.Left(p);
	}
	else
	{
		::GetCurrentDirectory(_MAX_PATH, sPath.GetBuffer(MAX_PATH));
		sPath.ReleaseBuffer();
		m_sWorkingDirectory = sPath;
	}

#ifdef _DEBUG
	m_sWorkingDirectory = _T("c:\\security");
#endif

	m_sWorkingDirectory.MakeLower();

	::GetWindowsDirectory(sPath.GetBuffer(MAX_PATH), MAX_PATH);
	sPath.ReleaseBuffer();
	m_sWindowsDirectory = sPath;
	
	::GetSystemDirectory(sPath.GetBuffer(MAX_PATH), MAX_PATH);
	sPath.ReleaseBuffer();
	m_sSystemDirectory = sPath;

	LPTSTR pszPath = sPath.GetBuffer(MAX_PATH);
	::GetTempPath(MAX_PATH, pszPath);
	WK_GetLongPathName(sPath, pszPath, MAX_PATH);
	sPath.ReleaseBuffer();
	m_sTempDirectory = sPath;
	if (m_sTempDirectory[m_sTempDirectory.GetLength()-1]==_T('\\'))
	{
		m_sTempDirectory = m_sTempDirectory.Left(m_sTempDirectory.GetLength()-1);
	}

	m_bUseUpdateHandler = DoesFileExist(m_sWorkingDirectory+_T("\\UpdateHandler.exe"));

	WK_TRACE(_T("searching for swap file is: %s\n"),m_sPagingFile);
	GetPagingDrive();
	WK_TRACE(_T("swap file is: %s\n"),m_sPagingFile);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSecurityLauncherApp::InitWindow()
{
	RegisterWindowClass();
	int i;
#ifndef _DEBUG
	if (AmIShell())
#endif
	{
		if (m_pDongle->GetOemCode() == CWK_Dongle::OEM_IDIP)
		{
			m_videteLogo.LoadBitmap(IDB_VIDETE_LOGO);
		}
		m_ppBackGroundWnd = (CBackGroundWnd**) new BYTE[sizeof(void*)*m_nMonitors];
		for (i=0; i<m_nMonitors; i++)
		{
			m_ppBackGroundWnd[i] = new CBackGroundWnd(i); 
		}
	}

	CMainFrame* pWnd;
	pWnd = new CMainFrame();

	pWnd->LoadAccelTable(MAKEINTRESOURCE(IDR_MAINFRAME));

	pWnd->UpdateWindow();
	m_pMainWnd = pWnd;
	pWnd->UpdateWindow();
	if (m_ppBackGroundWnd)
	{
		for (i=0; i<m_nMonitors; i++)
		{
			m_ppBackGroundWnd[i]->UpdateWindow();
		}
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSecurityLauncherApp::InitApps()
{
	CWK_Profile wkp;
	CString sAppName = GetAppnameFromId(WAI_IDIP_CLIENT);
	CString str = wkp.GetString(szModules, sAppName, _T(""));
	if (str.IsEmpty())
	{
		wkp.WriteString(szModules, sAppName, sAppName + _T(".exe"));
	}	

	m_Apps.InitAppsAndDongle(m_pDongle,&m_MeszMez,m_Mode,m_bUseUpdateHandler);
	
	if (m_ppBackGroundWnd)
	{
		m_ppBackGroundWnd[0]->AddResourceButton(ID_APP_EXIT);
		m_ppBackGroundWnd[0]->AddResourceButton(ID_ERROR);
		m_ppBackGroundWnd[0]->AddResourceButton(ID_DATETIME);
		int i, nCount = sizeof(gm_AppButtons) / sizeof(WK_ApplicationId);
		CApplication*pApp;
		for (i=0; i<nCount; i++)
		{
			pApp = m_Apps.GetApplication(gm_AppButtons[i]);
			if (pApp)
			{
				if (!gm_bAppStarted[i] && !pApp->IsEmpty())
				{
					m_ppBackGroundWnd[0]->AddApplicationButton(pApp);
				}
			}
		}
	}

	WK_STAT_LOG(_T("GP"),m_dwGPCounterDay,szGPDay);
	WK_STAT_LOG(_T("GP"),m_dwGPCounterHour,szGPHour);
	WK_STAT_LOG(_T("RT"),m_dwRTECounterDay,szRTEDay);
	WK_STAT_LOG(_T("RT"),m_dwRTECounterHour,szRTEHour);


	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::InitWatchDog()
{
	// it's WIN95
	CWK_Profile wkp;
	BOOL bUsePiezo = wkp.GetInt(szSection,_T("UseRelais4"),FALSE);

	// watchdog initializing
	CString sApp;

	sApp = wkp.GetString(szModules,WK_APP_NAME_COCOUNIT,_T(""));
	if (!sApp.IsEmpty())
	{
		// coco system
		if (m_WatchDog.Init(WD_COCO,60,bUsePiezo))
		{
			WK_TRACE(_T("Coco Watchdog initialized\n"));
		}
	}
	else
	{
		sApp = wkp.GetString(szModules,WK_APP_NAME_JACOBUNIT1,_T(""));
		if (!sApp.IsEmpty())
		{
			// Jacob system
			if (m_WatchDog.Init(WD_JACOB,60,bUsePiezo))
			{
				WK_TRACE(_T("JaCob Watchdog initialized\n"));
			}
		}
		else
		{
			sApp = wkp.GetString(szModules,WK_APP_NAME_MICOUNIT,_T(""));
			if (!sApp.IsEmpty())
			{
				// Mico system
				if (m_WatchDog.Init(WD_MICO,60,bUsePiezo))
				{
					WK_TRACE(_T("MiCo Watchdog initialized\n"));
				}
			}
			else
			{
				sApp = wkp.GetString(szModules,WK_APP_NAME_SAVICUNIT1,_T(""));
				if (!sApp.IsEmpty())
				{
					// Savic system
					if (m_WatchDog.Init(WD_SAVIC,60,bUsePiezo))
					{
						WK_TRACE(_T("Savic Watchdog initialized\n"));
					}
				}
				else
				{
					sApp = wkp.GetString(szModules,WK_APP_NAME_TASHAUNIT1,_T(""));
					if (!sApp.IsEmpty())
					{
						// Tasha system
						if (m_WatchDog.Init(WD_TASHA,60,bUsePiezo))
						{
							WK_TRACE(_T("Tasha Watchdog initialized\n"));
						}
					}
					else
					{
						sApp = wkp.GetString(szModules,WK_APP_NAME_QUNIT,_T(""));
						if (!sApp.IsEmpty())
						{
							// Q system
							if (m_WatchDog.Init(WD_Q,60,bUsePiezo))
							{
								WK_TRACE(_T("Q Watchdog initialized\n"));
							}
						}
					}
				}
			}
		}
	
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::CheckOSVersion()
{
	m_sOsVersion = COemGuiApi::GetOSVersion();
	m_sOsVersionShort = COemGuiApi::GetOSVersionShort();
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::CheckSoftwareVersion()
{
/*
	CWK_Profile wkp;
	CString sExe;

	GetModuleFileName(m_hInstance, sExe.GetBuffer(MAX_PATH), MAX_PATH);
	sExe.ReleaseBuffer();

	CString sVersion = WK_GetFileVersion(sExe);
	int v1,v2,v3,v4;
	if (4 != _stscanf(sVersion,_T("%d.%d.%d.%d"),&v1,&v2,&v3,&v4))
	{
		sVersion.Empty();
	}
	m_sVersion = COemGuiApi::CalcProductName();
	if (m_pDongle->IsDemo())
	{
		m_sVersion += _T(" DEMO");
	}
	m_sVersion += _T(" ") + sVersion;
*/
	m_sVersion = COemGuiApi::GetSoftwareVersion();

}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::PiezoCheck()
{
	CString sShutdown;
	CString sActualize;
	DWORD dwPiezo = PF_OFF;
	CWK_Profile wkp;

	sShutdown = wkp.GetString(szSection,szShutdown,_T(""));
	sActualize = wkp.GetString(szSection,szActualize,_T(""));

	if (!sActualize.IsEmpty())
	{
		if ( (sShutdown==sActualize) || (sShutdown.IsEmpty()))
		{
			// normal shutdown
		}
		else
		{
			// watchdog oder Stromausfall
			CSystemTime stNow;
			CSystemTime stLastActualize;
			CSystemTime stResult;
			stNow.GetLocalTime();
			TRACE(_T("%s\n"), (LPCTSTR)stNow.ToString());

			if (stLastActualize.FromString(sActualize))
			{
				CString sResult,sF,sL;
				// calculate time diff between now and last actualize
				// CARE result is time since 1.1.1601
				stResult = stNow - stLastActualize;

				// more than 1 year?
				if (stResult.wYear > 1601)
				{
					sL.LoadString(IDS_YEAR);
					sF.Format(_T("%d"),stResult.wYear-1601);
					sResult += sF + _T(" ") + sL + _T(", ");
				}
				// more than 1 month?
				if (stResult.wMonth > 1)
				{
					sL.LoadString(IDS_MONTH);
					sF.Format(_T("%d"),stResult.wMonth-1);
					sResult += sF + _T(" ") + sL + _T(", ");
				}
				// more than 1 day
				if (stResult.wDay > 1)
				{
					sL.LoadString(IDS_DAY);
					sF.Format(_T("%d"),stResult.wDay-1);
					sResult += sF + _T(" ") + sL + _T(", ");
				}
				if (stResult.wHour)
				{
					sL.LoadString(IDS_HOUR);
					sF.Format(_T("%d"),stResult.wHour);
					sResult += sF + _T(" ") + sL + _T(", ");
				}
				if (stResult.wMinute)
				{
					sL.LoadString(IDS_MINUTE);
					sF.Format(_T("%d"),stResult.wMinute);
					sResult += sF + _T(" ") + sL + _T(", ");
				}
				if (stResult.wSecond)
				{
					sL.LoadString(IDS_SECOND);
					sF.Format(_T("%d"),stResult.wSecond);
					sResult += sF + _T(" ") + sL + _T(", ");
				}
				if (!sResult.IsEmpty())
				{
					sResult = sResult.Left(sResult.GetLength()-2);
				}
				sL.LoadString(IDS_AC_OR_WATCHDOG);
				sL += _T(" ") + sResult;
				WK_TRACE(_T("%s\n"),sL);

				m_newErrors.SafeAdd(new CWK_RunTimeError(WAI_LAUNCHER,REL_CANNOT_RUN,RTE_LC_AC_FAIL,sL));
				if (GetMainWnd())
					GetMainWnd()->PostMessage(WM_COMMAND, ID_ERROR_ARRIVED);
			}
		}
	}

	if (wkp.GetInt(szSection,_T("ResetCounter"),0)>2)
	{
		dwPiezo = PF_ERROR_REBOOT;
	}
	m_WatchDog.PiezoSetFlag(dwPiezo);
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::InitShell()
{
	CString sModulePath;
	GetModuleFileName(m_hInstance, sModulePath.GetBuffer(MAX_PATH), MAX_PATH);
	sModulePath.ReleaseBuffer();

	CWK_Profile wkp;
	m_Mode = wkp.GetInt(szSection,_T("Mode"),LM_NORMAL);

	CString sShell;
	if (IsWin95())
	{
		// Only use short file names in system.ini!
		CString sLongPath, sShortPath;
		GetPrivateProfileString(_T("boot"),_T("shell"),_T(""),sShortPath.GetBuffer(MAX_PATH),MAX_PATH,_T("system.ini"));
		sShortPath.ReleaseBuffer();
		WK_TRACE(_T("System.ini %s\n"), (LPCTSTR)sShortPath);
		
		if (WK_GetLongPathName(sShortPath, sLongPath.GetBuffer(MAX_PATH), MAX_PATH) == 0)
		{
			sShell = sShortPath;
			// we have to get the short path of the App
			GetShortPathName((LPCTSTR)sModulePath, sShortPath.GetBuffer(MAX_PATH), MAX_PATH);
		}
		sLongPath.ReleaseBuffer();
		sModulePath = sShortPath;
		WK_TRACE(_T("Shell      %s\n"), (LPCTSTR)sShell);
		WK_TRACE(_T("Module     %s\n"), (LPCTSTR)sModulePath);
	}
	else if (IsNT())
	{
		CWK_Profile wkpNT(CWK_Profile::WKP_NTLOGON,NULL,szLogonNT,_T(""));
		sShell = wkpNT.GetString(szWinlogon,_T("Shell"),_T(""));
	}

	m_bAmIShell = (0 == sModulePath.CompareNoCase(sShell));
	m_bShutDownAtExitShell = m_bAmIShell;
	wkp.WriteInt(_T("SecurityLauncher"),_T("Shell"),m_bAmIShell);

	if (m_bAmIShell)
	{
		MINIMIZEDMETRICS mm;
		mm.cbSize = sizeof(mm);
		mm.iWidth = 100;
		mm.iHorzGap = 5;
		mm.iVertGap = 5;
		mm.iArrange = ARW_BOTTOMLEFT | ARW_RIGHT;
		SystemParametersInfo(SPI_SETMINIMIZEDMETRICS,0,&mm,0);

		m_hHook = SetWindowsHookEx(WH_SHELL,ShellProc,m_hInstance,GetCurrentThreadId());
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSecurityLauncherApp::InitInstance()
{
	if (WK_IS_RUNNING(WK_APP_NAME_DVSTARTER))
	{
		AfxMessageBox(_T("DTS Software is already started!\n\nStarting both modules will cause resource conflicts!"), MB_OK|MB_ICONERROR);
		return FALSE;
	}

	InitPathes();
	CWK_Profile wkp;
	CWK_Profile wkpSystem(CWK_Profile::WKP_ABSOLUT, HKEY_LOCAL_MACHINE,_T(""),_T(""));

	int nRunVcd = wkp.GetInt(szSection, VCD_RUN_MODULE, -1);
	DWORD dwExitCodeVCD = 0;
	CString sEmbedded = wkpSystem.GetString(REGPATH_HKLM_WINDOWS_EMBEDDED, REGKEY_RUNTIME_PID, NULL);
	if (!sEmbedded.IsEmpty())
	{
		nRunVcd = 1;
	}
	if (nRunVcd == -1)
	{
		wkp.WriteInt(szSection, VCD_RUN_MODULE, 0);
		nRunVcd = 0;
	}
	if (nRunVcd)
	{
		CApplication app;
		CString sCheckDisk = m_sWorkingDirectory + _T("\\VideteCheckDisk.exe");
		app.Create(sCheckDisk, TRUE, WK_APP_NAME_VCD);
		app.SetWaitUntilProcessEnd(true, 5000);
		app.SetTemporaryCmdLine(_T(" /a"));
		BOOL bResult = app.Execute();
		if (bResult)
		{
			do
			{
				app.IsRunning(0, 1000);
                dwExitCodeVCD = app.GetExitCode();
				if (dwExitCodeVCD == STILL_ACTIVE)
				{
					DWORD dwResult;
					if (app.SendMessageTimeOut(WM_SELFCHECK, 0, 0, 5000, &dwResult) && dwResult == 0)
					{
						app.Terminate();
						break;
					}
				}
				else if (dwExitCodeVCD == 2)
				{
					m_bRebootAtExit = TRUE;
					return FALSE;
				}
			}while (dwExitCodeVCD == STILL_ACTIVE);
		}
	}

	//check if CRT (C Runtime) error should be shown as RTE
	int nCrtAsRte = wkp.GetInt(szSection, SHOW_CRT_ERROR_AS_RTE, -1);
	if(nCrtAsRte == -1)
	{
		wkp.WriteInt(szSection, SHOW_CRT_ERROR_AS_RTE, 0);
	}


	InitDebugger(_T("launcher.log"),WAI_LAUNCHER);

	wkp.DeleteEntry(SECTION_COMMON, COMMON_SHUTDOWN_SETTING);

#if _MFC_VER == 0x0710
	CLoadResourceDll ResourceDll(NULL, NULL);
	m_hLangResourceDLL = ResourceDll.DetachResourceDllHandle();
	if (m_hLangResourceDLL == NULL)
	{
		WK_TRACE(_T("Did not find any ResourceDLL\n"));
		ASSERT(FALSE);
		return FALSE;
	}
	AfxSetResourceHandle(m_hLangResourceDLL);
	COemGuiApi::SetSystemUIlanguage(TRUE);

// RKE: InitInstance() loads the languageDll depending on the system language settings
// Do not call!
//	CWinApp::InitInstance();
#endif // _MFC_VER == 0x0710

	WK_TRACE(_T("init pathes\n"));
	WK_TRACE(_T("Windows is %s\n"),m_sWindowsDirectory);
	WK_TRACE(_T("System is %s\n"),m_sSystemDirectory);
	WK_TRACE(_T("Working is %s\n"),m_sWorkingDirectory);
	WK_TRACE(_T("Temp is %s\n"),m_sTempDirectory);
	
	m_sErrorDat = wkp.GetString(SECTION_COMMON, COMMON_RUNTIME_ERRORS, DEFAULT_ERRORFILE);

	WK_TRACE(_T("check for second instance\n"));
	if (WK_IS_RUNNING(WK_APP_NAME_LAUNCHER))
	{
		CString cl = m_lpCmdLine;
		if (m_lpCmdLine[0]!=_T('\0'))
		{
			UpdateMySelfInSecondInstance();
			return FALSE;
		}
	}
	WK_TRACE(_T("running in first instance\n"));

	if ( WK_ALONE(WK_APP_NAME_LAUNCHER)==FALSE ) 
	{
		return FALSE;
	}

	WK_TRACE(_T("check os version\n"));
	CheckOSVersion();
	WK_TRACE(_T("os version is %s\n"),GetOsVersion());

	WK_TRACE(_T("init shell\n"));
	InitShell();
	WK_TRACE(_T("shell is %s\n"),AmIShell() ? _T("LAUNCHER") : _T("EXPLORER"));

	CString sCodePage = COemGuiApi::GetCodePage();
	CWK_String::SetCodePage(_ttoi(sCodePage));
	wkp.SetCodePage(CWK_String::GetCodePage());
	DWORD dwCPBits = COemGuiApi::GetCodePageBits();
	if (dwCPBits)
	{
		SetFontFaceNamesFromCodePageBits(dwCPBits);
	}

	CString sIniFile = m_sWorkingDirectory + _T("\\") + OEM_FILE;
	CWK_Profile wkpOEM(CWK_Profile::WKP_INIFILE_ABSOLUT, NULL, _T(""), sIniFile); 
	COemGuiApi::LoadOEMSkinOptions(wkpOEM);
	CSkinDialog::SetButtonImages(SC_CLOSE    , (UINT)-1, 0, 0);
	CSkinDialog::SetButtonImages(SC_MAXIMIZE , (UINT)-1, 0, 0);
	CSkinDialog::SetButtonImages(SC_MINIMIZE , (UINT)-1, 0, 0);
	CSkinDialog::SetButtonImages(SC_MOUSEMENU, (UINT)-1, 0, 0, LoadIcon(IDR_MAINFRAME));
	CSkinDialog::LoadButtonImages();

	if (AmIShell())
	{
		WK_TRACE(_T(" +++++++++++ Start as Shell ++++\n"));
	}
	else
	{
		WK_TRACE(_T(" +++++++++++ Start ++++++++++++++\n"));

		CRunService rs;
		rs.ClearRun(_T("Launcher"));
	}

	// first init the dongle
	WK_TRACE(_T("create dongle\n"));
	m_pDongle = new CWK_Dongle();

	if (   (m_pDongle==NULL)
		|| (!m_pDongle->IsValid())
		)
	{
		// no dongle available 
		WK_TRACE_ERROR(_T("INVALID DONGLE\n"));
		AfxMessageBox(IDP_INVALID_DONGLE);

		if (AmIShell()) 
		{
			// I'm shell and the dongle is not valid reboot
			// with standard shell
			RebootWinExplorer();
		}
		return FALSE;
	}

	if (IsWinXP())
	{
		// Die Temeraturen aller angeschlossenen IDE-Festplatten ausgeben, sofern möglich.
		CHDDTApi Hdd;
		DRIVE_INFO drive_info;
		int nTemperatur = 0;

		for (UCHAR i=0;i<4;i++)
		{
			if (Hdd.GetDriveInfo(i, drive_info)!=0)  // get HDD information
			{
				WK_TRACE(_T("Hard Drive (%d)\n"), i);
				WK_TRACE(_T("===========================\n"));
				WK_TRACE(_T("Model       : %s\n"), CString(drive_info.ModelNumber));
				WK_TRACE(_T("Serial      : %s\n"), CString(drive_info.FirmWareRev));
				WK_TRACE(_T("Firmware    : %s\n"), CString(drive_info.SerialNumber));

				if (Hdd.GetDriveTemperatur(i, nTemperatur))
				{
					if (m_pDongle->IsTransmitter())
					{
						//safe the highest temperature in registry for maintenance function in system management
						CWK_Profile wkp;
						CString sSection(_T("Maintenance"));
						CString sKey, sTemperature;
						sKey.Format(_T("Temp%d"), i);
						sTemperature = wkp.GetString(sSection, sKey, _T(""));
						if(    sTemperature.IsEmpty() 
							|| _wtoi(sTemperature) < nTemperatur)
						{
							sTemperature.Format(_T("%d"), nTemperatur);
							wkp.WriteString(sSection, sKey, sTemperature);
						}
					}
					WK_TRACE(_T("Temperature : %d C\n"), nTemperatur);
				}
				else
				{
					WK_TRACE(_T("Temperature : not available\n"));
				}
				WK_TRACE(_T("\n"));
			}
		}
	}
	
	/*
	if (!m_pDongle->IsReceiver())
	{
		int nRegisterMediaFileTypes = wkp.GetInt(szSection, REGISTERMEDIAFILETYPES, -1);
		if (nRegisterMediaFileTypes == -1)
		{
			wkp.WriteInt(szSection, REGISTERMEDIAFILETYPES, 1);
			nRegisterMediaFileTypes = 1;
		}
		if (nRegisterMediaFileTypes)
		{
			ExecuteProgram(_T("c:\\Security\\AudioUnit.exe /r"), NULL);
		}
	}
*/
	WK_TRACE(_T("check software version\n"));
	CheckSoftwareVersion();

	if (m_pDongle->GetOemCode() == CWK_Dongle::OEM_AKUBIS)
	{
		// wrong OEM
		WK_TRACE_ERROR(_T("INVALID DONGLE, wrong OEM-Code: %i\n"), m_pDongle->GetOemCode());
		AfxMessageBox(IDP_INVALID_DONGLE);
		return FALSE;
	}

	m_sName = COemGuiApi::GetApplicationName(WAI_LAUNCHER);
	if (m_pszAppName)
	{
		free((void*)m_pszAppName);
	}
	m_pszAppName = _tcsdup(m_sName);    // human readable title
	AFX_MODULE_STATE*pState = AfxGetModuleState();
	if (pState)
	{
		if (pState->m_lpszCurrentAppName)
		{
			pState->m_lpszCurrentAppName = m_pszAppName;
		}
	}

	// creates c:\log if not existing
	WK_CreateDirectory(GetLogPath());
	WK_CreateDirectory(wkp.GetString(SECTION_COMMON, COMMON_OPENDIR, _T("c:\\out")));	

#if _MFC_VER < 0x0700
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

	WK_TRACE(_T("Time Corruption Detection extended\n"));
	WK_TRACE(_T("Software Version is %s\n"),m_sVersion);
	WK_TRACE(_T("OS Version is %s\n"),m_sOsVersion);
	
	//trace serial number (cumputername)
	CString sSerial(_T("no serial found"));
	CWK_Profile prof(CWK_Profile::WKP_ABSOLUT,HKEY_LOCAL_MACHINE,_T(""), _T(""));
	sSerial = prof.GetString(REGPATH_HKLM_COMPUTERNAME, REGKEY_COMPUTERNAME, _T(""));
	WK_TRACE(_T("Serial number is %s\n"),sSerial);

	TraceSystemInformation();

	WK_STAT_LOG(_T("Run"),0,m_sName);
	WK_STAT_LOG(_T("Run"),1,m_sName);

	// delete all *.chk files from scandisk
	ScanChk();

	m_SystemTime.GetLocalTime();
	TimeCheck();

	if (AmIShell())
	{
		CRunService rs;
		WK_TRACE(_T("running services\n"));
		rs.RunServices();
	}

	if (!InitWindow())
	{
		return FALSE;
	}

	// S.M.A.R.T Status ermitteln
	if (IsWinXP())
	{
		if (m_pDongle && m_pDongle->IsTransmitter())
			CheckSMARTparameters();
	}
	m_MeszMez.Init();

	// Sockets initialisieren.
	AfxSocketInit();
	
	// den Festplattenschreibcache abschalten
	if (m_pDongle->IsTransmitter())
	{
		DisableWriteCache();
	}

	if (!InitApps())
	{
		// no dongle available 
		AfxMessageBox(IDP_INVALID_DONGLE);
		WK_TRACE_ERROR(_T("INVALID DONGLE, Apps not initialised\n"));
		return FALSE;
	}

	if (IsRegistryEmpty() && (m_Mode == LM_NORMAL))
	{
		CApplication* pApp = m_Apps.GetApplication(WAI_SUPERVISOR);
		if (pApp==NULL)
		{
			WK_TRACE_ERROR(_T("registry is empty, cannot launch Systemverwaltung\n"));
		}
		else
		{
			WK_TRACE(_T("registry is empty\n"));
			m_pMainWnd->UpdateWindow();
			if (IDYES == AfxMessageBox(IDP_EMPTY_REGISTRY,MB_YESNO))
			{
				pApp->InitialStart();
			}
			m_pMainWnd->UpdateWindow();
		}
#if _MFC_VER < 0x0700
		return FALSE;
#endif
	}

	// load our errors

	m_nMaxErrors = wkp.GetInt(szSection, _T("MaxErrors"), 1000);
	
	CString sErrorDir = m_sErrorDat.Left(m_sErrorDat.ReverseFind(_T('\\')));
	if (GetFileAttributes(sErrorDir) == 0xffffffff)
	{
		m_sErrorDat = DEFAULT_ERRORFILE;
	}
	if (m_sErrorDat.CompareNoCase(DEFAULT_ERRORFILE) != 0)
	{
		m_Errors.LoadFromFile(DEFAULT_ERRORFILE);
		DeleteFile(DEFAULT_ERRORFILE);
	}
	m_Errors.LoadFromFile(theApp.m_sErrorDat);
	// delete own update exe file if exists
	DeleteNewExeInThirdInstance();

	// WatchDog and Stoerungsmelder
	// watchdog only for Win95, not Win NT
	InitWatchDog();
	PiezoCheck();

	m_pCIPCVirtualAlarm = new CIPCInputVirtualAlarm();
	StartErrorThread();

	if (m_Mode == LM_NORMAL)
	{
		m_pMainWnd->SetForegroundWindow();
	}
	m_pMainWnd->UpdateWindow();

	StartAll();

	if (m_Mode == LM_BACKUP)
	{
		m_WatchDog.PiezoSetFlag(PF_OFF);
		m_WatchDog.Disable();
		WK_TRACE(_T("watchdog disabled by backup mode\n"));
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::StartAll()
{
	int i;
	m_WatchDog.Reset(300);
	if (m_ppBackGroundWnd)
	{
		for (i=0; i<m_nMonitors; i++)
		{
			m_ppBackGroundWnd[i]->SetStatus(0);
		}
	}
	m_Apps.StartAll();


	m_pMainWnd->UpdateWindow();

	if (m_ppBackGroundWnd)
	{
		for (i=0; i<m_nMonitors; i++)
		{
			m_ppBackGroundWnd[i]->UpdateWindow();
			m_ppBackGroundWnd[i]->SetStatus((m_Mode==LM_BACKUP) ? 3 : 1);
		}
	}
	// set windows time for watchdog (coco/mico)
	m_pMainWnd->SetTimer(1,((m_Mode==LM_BACKUP) ? 5 : TIMER_SECONDS) * 1000,NULL);	
	// CIPCServerControl connection to server for updates
	ReconnectToServer();
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::RestartAll()
{
	TerminateAll();
	StartAll();
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::CleanUp()
{
/*	m_pCIPCVirtualAlarm->StopThread();
	while (!m_pCIPCVirtualAlarm->GetIsMarkedForDelete())
	{
		Sleep(10);
	}
*/	WK_DELETE(m_pCIPCVirtualAlarm);

	CWK_Profile wkp;

	if (m_pErrorDialog)
	{
		OnError();	// schließt den ErrorDialog, wenn m_pErrorDialog != NULL
	}

	StopErrorThread();

	m_bWriteErrorFile = TRUE;
	WriteErrorFile();

	CString s;
	m_Errors.Lock();
	s.Format(_T("we had %d Software Error(s)\n"),m_Errors.GetSize());
	m_Errors.DeleteAll();
	m_newErrors.DeleteAll();
	m_Errors.Unlock();

	WK_DELETE(m_pLauncherControl);

	WK_TRACE(_T("we had %d GP(s) and %d Runtime Error(s)\n"),m_dwGPCounterDay,m_dwRTECounterDay);
	WK_STAT_LOG(_T("Run"),0,m_sName);

	CSystemTime st;
	CString sTime;
	st.GetLocalTime();
	sTime = st.ToString();
	wkp.WriteString(szSection,szShutdown,sTime);
	wkp.WriteString(szSection,szActualize,sTime);

	if (AmIShell())
	{
		if (m_hHook)
		{
			UnhookWindowsHookEx(m_hHook);
			m_hHook = NULL;
		}

		if (m_bRebootAtExit)
		{
			WK_TRACE(_T(" ++++ End Restart Windows ++++\n"));
		}
		else
		{
			WK_TRACE(_T(" ++++ End Shutdown Windows ++++++++\n"));
		}
	}
	else
	{
		WK_TRACE(_T(" +++++++++++ End   ++++++++++++++\n"));
	}
	WK_DELETE(m_pDongle);

	m_Apps.DeleteAll();

	if (m_ppBackGroundWnd)
	{
		for (int i=0; i<m_nMonitors; i++)
		{
			if (WK_IS_WINDOW(m_ppBackGroundWnd[i]))
			{
				m_ppBackGroundWnd[i]->DestroyWindow();
			}
		}
		delete m_ppBackGroundWnd;
		m_ppBackGroundWnd = NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::WriteErrorFile()
{
	if(m_bWriteErrorFile)
	{
		if (!m_Errors.WriteToFile(theApp.m_sErrorDat))
		{
			m_Errors.WriteToFile(DEFAULT_ERRORFILE);
		}
		m_bWriteErrorFile = FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
int CSecurityLauncherApp::SafeExitInstance() 
{
	// Commits all current level data in the overlay to the protected volume 
	
	if (!m_sExecuteAtExit.IsEmpty())
	{
		WK_TRACE(_T("start %s at exit\n"),m_sExecuteAtExit);
	}

	SysDown();

	if (m_bShutDownAtExit)
	{
		WK_TRACE(_T("shutting down at exit\n"));
	}
	if (m_bRebootAtExit)
	{
		WK_TRACE(_T("reboot at exit\n"));
	}

	CleanUp();

	m_WatchDog.Disable();
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
int CSecurityLauncherApp::ExitInstance() 
{
	SafeExitInstance();
	CloseDebugger();
	m_BkGndBrush.DeleteObject();
	COemGuiApi::SetSystemUIlanguage(FALSE);

#ifdef _DEBUG
	if (m_bRebootAtExit)
	{
		AfxMessageBox(_T("In Release erfolgt jetzt ein Reboot"));
	}
	else if (m_bShutDownAtExit)
	{
		AfxMessageBox(_T("In Release erfolgt jetzt ein ShutDown"));
	}
	if (!m_sExecuteAtExit.IsEmpty())
	{
		AfxMessageBox(_T("In Release wird jetzt\n") + m_sExecuteAtExit+_T("\ngestartet."));
	}
#else
	DWORD dwReason = 0; // bisheriger Wert
	// XP unterstützt im EventLog verschiedene Gründe, hübsch fände ich z.B.
//	dwReason = SHTDN_REASON_MAJOR_APPLICATION|SHTDN_REASON_MINOR_RECONFIG|SHTDN_REASON_FLAG_PLANNED;

	if (m_bRebootAtExit)
	{
		if (IsNT())
		{
			AdjustPrivileges(SE_SHUTDOWN_NAME);
		}
		
		Sleep(1000);
		if (!ExitWindowsEx(EWX_REBOOT, dwReason))
		{
			Sleep(1000);
			if (!ExitWindowsEx(EWX_REBOOT, dwReason))
			{
				// GF Laut Doku ist EWX_FORCE ein *_zusätzlicher_* Parameter, nicht eigenständig
//				ExitWindowsEx(EWX_FORCE, dwReason);
//#if(_WIN32_WINNT >= 0x0500)
//				ExitWindowsEx(EWX_REBOOT|EWX_FORCE|EWX_FORCEIFHUNG, dwReason);
//#else
				ExitWindowsEx(EWX_REBOOT|EWX_FORCE, dwReason);
//#endif /* _WIN32_WINNT >= 0x0500 */
			}
		}
	}
	else if (m_bShutDownAtExit)
	{
		CWK_Profile wkp;
		if (wkp.GetInt(SECTION_COMMON, COMMON_DISPLAYEWFDIALOG, 0))
		{
			wkp.WriteInt(SECTION_COMMON, COMMON_SHUTDOWN_SETTING, RD_ES_SHUTDOWN);
		}
		if (IsNT())
		{
			AdjustPrivileges(SE_SHUTDOWN_NAME);
		}
		ExitWindowsEx(EWX_SHUTDOWN, dwReason);
	}

	if (!m_sExecuteAtExit.IsEmpty())
	{
		// WInExec requires LPCSTR
		CWK_String sExecuteAtExit(m_sExecuteAtExit);
		WinExec(sExecuteAtExit, SW_SHOWNORMAL);
	}
#endif

	return CWinApp::ExitInstance();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSecurityLauncherApp::AdjustPrivileges(LPCTSTR privilege, 
											DWORD dwAccess /*= SE_PRIVILEGE_ENABLED*/)
{
	BOOL fResult;                  // system shutdown flag 
	HANDLE hToken = INVALID_HANDLE_VALUE;// handle to process token 
	TOKEN_PRIVILEGES tkp;			// ptr. to token structure 

	try
	{
		// Get the current process token handle 
		// so we can get debug privilege. 
		fResult = OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken) ;
		if (!fResult) throw 1;

		// Get the LUID for debug privilege. 
		fResult = LookupPrivilegeValue(NULL, privilege, &tkp.Privileges[0].Luid); 
		if (!fResult) throw 2;

		tkp.PrivilegeCount = 1;  // one privilege to set    
		tkp.Privileges[0].Attributes = dwAccess; 

		// Get shutdown privilege for this process. 
		fResult = AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES) NULL, 0); 
		if (!fResult) throw 3;
	}
	catch (int nPos)
	{
	    DWORD dwError = GetLastError();
		WK_TRACE(_T("Error DoExitWindows(), %d, last error: %d, %x\n"),nPos, dwError, dwError);
	}
	if (hToken != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hToken);
	}
	
	return fResult;
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::TerminateAll(BOOL bRestartTimer /*= FALSE*/)
{
	int i, co = 10;

	if (m_pErrorDialog)
	{
		OnError(); // schließt den ErrorDialog, wenn m_pErrorDialog != NULL
	}

	m_WatchDog.Reset(300);
	m_pMainWnd->KillTimer(1);
	WK_TRACE(_T("shutting down Software\n"));

	if (m_ppBackGroundWnd)
	{
		for (i=0; i<m_nMonitors; i++)
		{
			m_ppBackGroundWnd[i]->SetStatus(2);
			m_ppBackGroundWnd[i]->UpdateWindow();
		}
	}
	// no more updates
	WK_DELETE(m_pLauncherControl);

	m_Apps.TerminateAll();

	WK_TRACE(_T("Software is down\n"));


	if (bRestartTimer)
	{
		if (m_ppBackGroundWnd)
		{
			while (co)
			{
				Sleep(50);
				co--;
				for (i=0; i<m_nMonitors; i++)
				{
					m_ppBackGroundWnd[i]->UpdateWindow();
				}
			}
		}

		m_pMainWnd->SetTimer(1,((m_Mode==LM_BACKUP) ? 5 : TIMER_SECONDS) * 1000,NULL);	
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::StartClientApp(WK_ApplicationId Id, WORD wUserID)
{
	CApplication* pApp = m_Apps.GetApplication(Id);
	if (pApp)
	{
		pApp->SetUserID(wUserID);
		pApp->Actualize(FALSE);
		if (!pApp->GetExePathName().IsEmpty())
		{
			if (pApp->IsRunning(TRUE))
			{
				pApp->BringToTop();
			}
			else
			{
				pApp->Execute();
			}
		}
		else
		{
			CString sL;
			sL.Format(_T("%s kann nicht gestartet werden. Der Pfad stimmt nicht,%s"),
				pApp->GetName(),pApp->GetExePathName());
			m_newErrors.SafeAdd(new CWK_RunTimeError(WAI_LAUNCHER,REL_ERROR,RTE_CONFIGURATION,sL));
			if (GetMainWnd())
				GetMainWnd()->PostMessage(WM_COMMAND, ID_ERROR_ARRIVED);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnDatabase() 
{
	StartClientApp(WAI_DATABASE_CLIENT);
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnUpdateDatabase(CCmdUI* pCmdUI) 
{
	CApplication* pApp = m_Apps.GetApplication(WAI_DATABASE_CLIENT);
	pCmdUI->Enable(pApp && !pApp->IsEmpty());
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnVision() 
{
	StartClientApp(WAI_IDIP_CLIENT);
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnUpdateVision(CCmdUI* pCmdUI) 
{
	CApplication* pApp = m_Apps.GetApplication(WAI_IDIP_CLIENT);
	pCmdUI->Enable(pApp && !pApp->IsEmpty());
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnSupervisor() 
{
	StartClientApp(WAI_SUPERVISOR);
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnUpdateSupervisor(CCmdUI* pCmdUI) 
{
	CApplication* pApp = m_Apps.GetApplication(WAI_SUPERVISOR);
	pCmdUI->Enable(pApp && !pApp->IsEmpty());
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnSDIConfig() 
{
	StartClientApp(WAI_SDICONFIG);
}
/////////////////////////////////////////////////////////////////////////////
WORD GetDiffererenz(WORD wOld, WORD wNew, WORD wFirst, WORD wOverflow)
{
	WORD wDifferenz = 0;
	
	if (wOld > wNew)
	{
		// old > new
		if (  (wOld == wOverflow) 
			&&(wNew == wFirst)
			)
		{
			wDifferenz = 1;
		}
		else
		{
			wDifferenz = (WORD)(wOld - wNew);
		}
	}
	else if (wOld < wNew)
	{
		// old < new
		wDifferenz = (WORD)(wNew - wOld);
	}
	return wDifferenz;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSecurityLauncherApp::TimeChangeCheck(const CSystemTime& st)
{
	WORD wYearDifference = 0;

	// the time may be changed by DCF (Funkuhr BUG)
	if (m_SystemTime.wYear > st.wYear)
	{
		wYearDifference = (WORD)(m_SystemTime.wYear - st.wYear);
	}
	else if (m_SystemTime.wYear < st.wYear)
	{
		wYearDifference = (WORD)(st.wYear - m_SystemTime.wYear);
	}

	if (wYearDifference>1)
	{
		WK_TRACE(_T("year has changed from %d to %d\n"),m_SystemTime.wYear,st.wYear);
		return TRUE;
	}

	if (m_MeszMez.IsDCF())
	{
		WORD wMonthDifferenz = GetDiffererenz(m_SystemTime.wMonth,st.wMonth,1,12);
		if (wMonthDifferenz>1)
		{
			WK_TRACE(_T("month has changed from %d to %d\n"),m_SystemTime.wMonth,st.wMonth);
			return TRUE;
		}

		WORD wDayDifferenz = GetDiffererenz(m_SystemTime.wDay,st.wDay,1,31);
		if (wDayDifferenz>1)
		{
			if ( (st.wDay == 1) &&
				 (   (m_SystemTime.wDay==31) 
				  || (m_SystemTime.wDay==30)
				  || (m_SystemTime.wDay==29)
				  || (m_SystemTime.wDay==28)
				  )
			   )
			{
			}
			else
			{
				WK_TRACE(_T("day has changed from %d to %d\n"),m_SystemTime.wDay,st.wDay);
				return TRUE;
			}
		}

		WORD wHourDifferenz = GetDiffererenz(m_SystemTime.wHour,st.wHour,0,23);
		if (wHourDifferenz>1)
		{
			WK_TRACE(_T("hour has changed from %d to %d\n"),m_SystemTime.wHour,st.wHour);
			return TRUE;
		}
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnTimeChangedBySoftware()
{
	m_SystemTime.GetLocalTime();
}
/////////////////////////////////////////////////////////////////////////////
// wird alle 20 s zur Kontrolle des Systems aufgerufen
void CSecurityLauncherApp::Actualize20s()
{
	BOOL bOK = TRUE;
	DWORD dwGPC = m_dwGPCounterHour;
	CSystemTime st;

	st.GetLocalTime();
#ifdef _DEBUG
//	CheckHDDTemperatures();
//	CheckSMARTparameters();
#endif 

/*
	if (WK_IS_WINDOW(m_pMainWnd))
	{
		((CMainFrame*)m_pMainWnd)->SetTime(st);
	}
	*/
	// Fals sich an der Modulliste was geändert hat, wird der Watchdog neu initialisiert.
	InitWatchDog();

	BOOL bCrtError = CApplication::HandleCRTMessageBox();
	if (bCrtError)
	{
		IncreaseRTECounter();
		if (m_WatchDog.PiezoSetFlag(PF_CRT_ERROR))
		{
			WK_TRACE(_T("Störungsmelder an CRT Fehler\n"));
		}
	}

	int i,c;
	BOOL bRet = TRUE;
	CApplication* pApp;

	c = m_Apps.GetSize();
	for (i=0;i<c;i++)
	{
		pApp = m_Apps.GetAt(i);
		if (pApp)
		{
			bRet = pApp->Actualize(pApp->KeepAlive());
			bOK &= bRet;
			if (bRet)
			{
				if (m_ppBackGroundWnd)	// check the background window application buttons
				{
					int j, nCount = sizeof(gm_AppButtons) / sizeof(WK_ApplicationId);
					for (j=0; j<nCount; j++)
					{
						if (gm_bAppStarted[j])
						{
							if (pApp->GetId() == gm_AppButtons[j])
							{
								if (pApp->IsEmpty())
								{
									CWnd *pWnd = m_ppBackGroundWnd[0]->GetDlgItem((UINT)pApp->GetId());
									if (pWnd)
									{
										pWnd->ShowWindow(SW_HIDE);
									}
								}
								else
								{
									m_ppBackGroundWnd[0]->AddApplicationButton(pApp);
								}
							}
						}
					}
				}
			}
			else
			{
				if(bCrtError)
				{
					//check if CRT (C Runtime) error should be shown as RTE
					CWK_Profile wkp;
					int iCrtAsRte = wkp.GetInt(szSection, SHOW_CRT_ERROR_AS_RTE, 0);
					if(iCrtAsRte)
					{
						CString sL,sF;
						sF.LoadString(IDS_NOT_ALL_RUNNING);
						sL.Format(sF,pApp->GetName());
						m_newErrors.SafeAdd(new CWK_RunTimeError(WAI_LAUNCHER,REL_ERROR,RTE_CONFIGURATION,sL));
						if (GetMainWnd())
							GetMainWnd()->PostMessage(WM_COMMAND, ID_ERROR_ARRIVED);
					}
				}
				else
				{
					CString sL,sF;
					sF.LoadString(IDS_NOT_ALL_RUNNING);
					sL.Format(sF,pApp->GetName());
					m_newErrors.SafeAdd(new CWK_RunTimeError(WAI_LAUNCHER,REL_ERROR,RTE_CONFIGURATION,sL));
					if (GetMainWnd())
						GetMainWnd()->PostMessage(WM_COMMAND, ID_ERROR_ARRIVED);
				}

			}
		}
	}

	if (m_Mode == LM_NORMAL)
	{
		if (bOK)
		{
			CWK_Profile wkp;

			CString sTime;
			sTime = st.ToString();
			wkp.WriteString(szSection,szActualize,sTime);
			m_WatchDog.Reset(m_wWatchDogTime);
			m_WatchDog.PiezoClearFlag(PF_APP_NOT_RUNNING);
		}
		else
		{
			if (m_WatchDog.PiezoSetFlag(PF_APP_NOT_RUNNING))
			{
				WK_TRACE(_T("Störungsmelder an (nicht alle Anwendungen laufen)\n"));
			}
		}

		if (m_pCIPCVirtualAlarm)
		{
			if (m_pCIPCVirtualAlarm->GetIsMarkedForDelete())
			{
				WK_DELETE(m_pCIPCVirtualAlarm);
			}
		}
		else
		{
			m_pCIPCVirtualAlarm = new CIPCInputVirtualAlarm();
		}
		if (m_pLauncherControl)
		{
			if (m_pLauncherControl->IsTimedOut())
			{
				WK_TRACE_ERROR(_T("LauncherControl time out\n"));
				WK_DELETE(m_pLauncherControl);
				ReconnectToServer();
			}
		}

		m_iActualize++;
		if (m_iActualize == 1)
		{
		}
		if (m_iActualize>=TIMER_15MIN) // alle viertel stunde
		{
			Actualize15min();
			m_iActualize = 0;
		}

		if (m_dwGPCounterHour>dwGPC)
		{
			TimeCheck();
		}

		if (m_SystemTime.wHour != st.wHour)
		{
			ActualizeHour();
		}

		if (m_SystemTime.wDay != st.wDay)
		{
			ActualizeDay();
		}

		if (TimeChangeCheck(st))
		{

			WK_TRACE_WARNING(_T("RESTART SOFTWARE by TIME CORRUPTION\n"));
			
			m_WatchDog.Reset(300); // 300 Sekunden = 5 Min
			m_Apps.TerminateAll();
		
			if (IsNT())
			{
				AdjustPrivileges(SE_SYSTEMTIME_NAME,SE_PRIVILEGE_USED_FOR_ACCESS);
			}

			if (SetLocalTime(&m_SystemTime))
			{
				WK_TRACE(_T("TIME CORRECTED by TIME CORRUPTION\n"));
				PostMessage(HWND_BROADCAST,WM_TIMECHANGE,0,0);
			}
			else
			{
				WK_TRACE_ERROR(_T("time change failed %s"),GetLastErrorString());
			}
			if (IsNT())
			{
				AdjustPrivileges(SE_SYSTEMTIME_NAME,0);
			}

			m_Apps.StartAll();

			GetLocalTime(&st);
		}

		// Automatische Umstellung von Sommerzeit auf Winterzeit und umgekehrt.
		m_MeszMez.Check();
	}
	else if (m_Mode == LM_BACKUP)
	{
		pApp = m_Apps.GetApplication(WAI_DATABASE_CLIENT);
		if (pApp && pApp->IsRunning())
		{
			// alles ok 
		}
		else
		{
			OnBackupFinished();
		}
	}
	m_SystemTime = st;

	WriteErrorFile();
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::Actualize15min()
{
	if (!TimeCheck())
	{
		WK_TRACE_WARNING(_T("restarting by time check (2038)\n"));
		RestartAll();
	}
	else
	{
		CheckHDDTemperatures();

		DWORD dwPaging;
		GlobalMemoryStatus(&m_Memory);
		WK_STAT_LOG(_T("Memory"),m_Memory.dwMemoryLoad,_T("Percent in use")); 
		WK_STAT_LOG(_T("Memory"),m_Memory.dwAvailPhys/(1024*1024),_T("MBytes Free Physical"));
		dwPaging = m_Memory.dwTotalPageFile-m_Memory.dwAvailPageFile;
		WK_STAT_LOG(_T("Memory"),(dwPaging)/(1024*1024),_T("MBytes Used PagingFile"));

		if ((m_Memory.dwMemoryLoad==100) && !m_sPagingFile.IsEmpty())
		{
			CString sDrive = m_sPagingFile.Left(3);
			// 100% of memory (phys. and swap) is used
			// check wether there is enough free space
			// for a growing swap file
			DWORD SectorsPerCluster;
			DWORD BytesPerSector;
			DWORD NumberOfFreeClusters;
			DWORD TotalNumberOfClusters;
			double fTotalBytes;
			double fFreeBytes;

			if (GetDiskFreeSpace(sDrive,
							&SectorsPerCluster,
							&BytesPerSector,
							&NumberOfFreeClusters,
							&TotalNumberOfClusters))
			{

				fTotalBytes = TotalNumberOfClusters * SectorsPerCluster * BytesPerSector;
				fFreeBytes = NumberOfFreeClusters * SectorsPerCluster * BytesPerSector;

				if (((fFreeBytes/dwPaging)<0.1) || (fFreeBytes<10*1024*1024))
				{
					WK_TRACE_WARNING(_T("restarting by swap file\n"));
					WK_TRACE_WARNING(_T("Not enough space for Win95 Paging File on %s\n"), sDrive);
					WK_TRACE_WARNING(_T("shutting down and restart software\n"));
					// swap file may only grow by 10%
					// we shut down the software and restart
					RestartAll();
				}
			}
		}
		const CIPCDebugger *pDebugger = GetDebugger();
		if (pDebugger && pDebugger->m_pFile)
		{
			pDebugger->m_pFile->CheckDiskSpaceAndCleanUp();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::ActualizeHour()
{
	WK_TRACE(_T("last hour we had %d GP(s) and %d Runtime Error(s)\n"),m_dwGPCounterHour,m_dwRTECounterHour);
	m_dwGPCounterHour = 0;
	m_dwRTECounterHour = 0;
	WK_STAT_LOG(_T("GP"),m_dwGPCounterHour,szGPHour);
	WK_STAT_LOG(_T("RT"),m_dwRTECounterHour,szRTEHour);

	// Jede Stunde den S.M.A.R.T-Status ausgeben
	if (IsWinXP())
	{
		if (m_pDongle && m_pDongle->IsTransmitter())
			CheckSMARTparameters();
	}

}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::ActualizeDay()
{
	WK_TRACE(_T("yesterday we had %d GP(s) and %d Runtime Error(s)\n"),m_dwGPCounterDay,m_dwRTECounterDay);
	m_dwGPCounterDay = 0;
	m_dwRTECounterDay = 0;
	WK_STAT_LOG(_T("GP"),m_dwGPCounterDay,szGPDay);
	WK_STAT_LOG(_T("RT"),m_dwRTECounterDay,szRTEDay);

	WK_TRACE(_T("yesterday we had %d Software Error(s)\n"),m_Errors.GetSize());

	CWK_Profile wkp;
	wkp.WriteInt(_T("SecurityLauncher"),_T("ResetCounter"),0);
	WK_STAT_LOG(_T("Restart"),0,_T("ResetCounter"));

}
/////////////////////////////////////////////////////////////////////////////
BOOL CSecurityLauncherApp::IsRegistryEmpty()
{
	CWK_Dongle dongle;
	CWK_Profile wkp;

	if (dongle.GetProductCode() == IPC_DTS_RECEIVER)
	{
		CIPCDrives drives;
		drives.Init(wkp);

		return drives.GetAvailableMB()==0;
	}
	else
	{
		// check for number of users
		// should be nonzero
		int iUsers;

		iUsers = wkp.GetInt(_T("User"),_T("NumberOfUsers"),0);

		if (iUsers>0)
		{
			return FALSE;
		}
		return TRUE;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnSpace()
{
	OnDatabase();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSecurityLauncherApp::Login(BOOL bForceSuperVisor /*= TRUE*/)
{
	BOOL bRet = FALSE;
	InternalProductCode ipc = IPC_MIN_CODE;
	if (m_pDongle)
	{
		ipc = m_pDongle->GetProductCode();
	}
	if (   (ipc == IPC_TOBS_RECEIVER)
		|| (ipc == IPC_DTS_RECEIVER)
		)
	{
		bRet = TRUE;
	}
	else
	{
		CUserArray ua;
		CPermissionArray pa;
		CUser*		pUser = NULL;
		CPermission*pPermission = NULL;
		CWK_Profile wkp;
		COEMLoginDialog dlg;

		dlg.SetShowTopMost(TRUE);
#if _MFC_VER >= 0x0700
		dlg.SetMonitorFlags(LOGIN_HIDE_LOGO);
#endif
		if (m_TemporaryUserID != SECID_NO_ID)
		{
			ua.Load(wkp);
			pUser = ua.GetUser(m_TemporaryUserID);
			if (pUser)
			{
				pa.Load(wkp);
				pPermission = pa.GetPermission(pUser->GetPermissionID());
			}
			m_TemporaryUserID = SECID_NO_ID;
		}

		bRet = FALSE;
		if (   (pUser && pPermission)
			|| IDOK==dlg.DoModal())
		{
			CUser user;
			CPermission perm(dlg.GetPermission());
			CPermission *pPerm;
			if (pUser && pPermission)
			{
				user  = *pUser;
				pPerm = pPermission;
			}
			else
			{
				user  = dlg.GetUser();
				pPerm = &perm;
			}
			WK_TRACE(_T("Legimitation: %s\n"),user.GetName());
			if (bForceSuperVisor)
			{
				bRet = (WK_ALLOW_SHUTDOWN & pPerm->GetFlags()) ? TRUE : FALSE;
			}
			else
			{
				bRet = (WK_ALLOW_BACKUP & pPerm->GetFlags()) ? TRUE : FALSE;
			}
			if (!bRet)
			{
				CString s;
				s.LoadString(IDS_NO_PERMISSION);
				COemGuiApi::MessageBox(s,10);
			}
		}
	}

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::SysDown()
{
	if (m_Mode == LM_BACKUP)
	{
		m_bShutDownAtExit = TRUE;
	}
	else
	{
		m_bShutDownAtExit = m_bShutDownAtExitShell && AmIShell();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::QuiteExit()
{
	TerminateAll();
	((CMainFrame*)m_pMainWnd)->ClearAskAtExit();
	m_pMainWnd->DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::SysReboot()
{
	if (AmIShell())
	{
		m_bRebootAtExit = TRUE;
		TerminateAll();
		((CMainFrame*)m_pMainWnd)->ClearAskAtExit();
		m_pMainWnd->DestroyWindow();
	}
	else
	{
		TerminateAll();
		StartAll();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::RebootWinExplorer()
{
	m_bRebootAtExit = TRUE;
	WK_TRACE(_T("Windows Explorer wird Shell\n"));
	MakeExplorerShell();
	((CMainFrame*)m_pMainWnd)->ClearAskAtExit();
	m_pMainWnd->DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSecurityLauncherApp::RebootBackup()
{
	CBeforeBackupDialog dlg(m_pMainWnd);

	if (IDOK==dlg.DoModal())
	{
		CWK_Profile wkp;
		wkp.WriteInt(szSection,_T("Mode"),LM_BACKUP);
		m_bRebootAtExit = FALSE;
		m_bShutDownAtExit = TRUE;
		m_bShutDownAtExitShell = TRUE;
		TerminateAll();
		((CMainFrame*)m_pMainWnd)->ClearAskAtExit();
		m_pMainWnd->DestroyWindow();
		return TRUE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::StartExplorer()
{
	WK_TRACE(_T("starte Windows Explorer\n"));
	WinExec("Explorer.exe",SW_SHOW);
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::RebootWinLauncher()
{
	m_bRebootAtExit = TRUE;
	WK_TRACE(_T("%s wird Shell\n"),m_sName);
	MakeMeShell();
	((CMainFrame*)m_pMainWnd)->ClearAskAtExit();
	m_pMainWnd->DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSecurityLauncherApp::GetMode()
{
	return m_Mode;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSecurityLauncherApp::IsBackupMode()
{
	return m_Mode == LM_BACKUP;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSecurityLauncherApp::IsNormalMode()
{
	return m_Mode == LM_NORMAL;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSecurityLauncherApp::AmIShell()
{
	return m_bAmIShell;
}
static TCHAR BASED_CODE szMsdos[] = _T("C:\\msdos.sys");
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::MakeMeShell()
{
	CString sPath;

	GetModuleFileName(m_hInstance, sPath.GetBuffer(MAX_PATH), MAX_PATH);
	sPath.ReleaseBuffer();
	if (IsWin95())
	{
		// Only use short file names in system.ini!
		CString sShortPath;
		GetShortPathName(sPath, sShortPath.GetBuffer(MAX_PATH), MAX_PATH);
		sShortPath.ReleaseBuffer();
		WritePrivateProfileString(_T("boot"), _T("shell"), sShortPath, _T("system.ini"));
	}
	else if (IsNT())
	{
		CWK_Profile wkpSystem(CWK_Profile::WKP_ABSOLUT, HKEY_LOCAL_MACHINE,_T(""),_T(""));
		CWK_Profile wkp(CWK_Profile::WKP_NTLOGON, NULL, szLogonNT, _T(""));

		wkp.WriteString(szWinlogon, _T("Shell"), sPath);

//		RKE: Does not work properly
//		wkpSystem.WriteInt(HKLM_CONTROL_WINDOWS, CONTW_ERROR_MODE, ERROR_MODE_NO_SYSTEM);

		wkpSystem.WriteInt(HKLM_ERROR_MESSAGE_INSTRUMENT, EMI_ENABLEDEFAULTREPLY, 1);
		wkpSystem.WriteInt(HKLM_ERROR_MESSAGE_INSTRUMENT, EMI_ENABLELOGGING, 1);
		wkpSystem.WriteInt(HKLM_ERROR_MESSAGE_INSTRUMENT, EMI_LOGSEVERITY, 0);
	}

	if (IsWin95())
	{
		DWORD dwAt = GetFileAttributes(szMsdos);
		SetFileAttributes(szMsdos,FILE_ATTRIBUTE_NORMAL);
		Sleep(100);
		WritePrivateProfileString(_T("Options"),_T("Logo"),_T("0"),szMsdos);
		WritePrivateProfileString(_T("Options"),_T("BootKeys"),_T("0"),szMsdos);
		WritePrivateProfileString(NULL,NULL,NULL,szMsdos);
		Sleep(100);
		SetFileAttributes(szMsdos,dwAt);
	}

	CString sOldName,sNewName;
	 
	if (IsWin95())
	{
		sOldName = GetWindowsDirectory() + _T('\\') + _T("taskman.exe");
		sNewName = GetWindowsDirectory() + _T('\\') + _T("taskman.000");
	}
	else if (IsNT())
	{
		sOldName = GetSystemDirectory() + _T('\\') + _T("taskmgr.exe");
		sNewName = GetSystemDirectory() + _T('\\') + _T("taskmgr.000");
	}

	TRY
	{
		CFile::Rename( sOldName, sNewName );
	}
	CATCH( CFileException, e )
	{
		WK_TRACE_ERROR(_T("MakeMeShell: %s  was not renamed!\n"), sOldName);
	}
	END_CATCH
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::MakeExplorerShell()
{
	if (IsWin95())
	{
		WritePrivateProfileString(_T("boot"),_T("shell"),_T("Explorer.exe"),_T("system.ini"));
	}
	else if (IsNT())
	{
		CWK_Profile wkpSystem(CWK_Profile::WKP_ABSOLUT, HKEY_LOCAL_MACHINE,_T(""),_T(""));
		CWK_Profile wkp(CWK_Profile::WKP_NTLOGON,NULL,szLogonNT,_T(""));

		wkp.WriteString(szWinlogon,_T("Shell"),_T("Explorer.exe"));

//		wkpSystem.WriteInt(HKLM_CONTROL_WINDOWS, CONTW_ERROR_MODE, ERROR_MODE_ALL);

		wkpSystem.DeleteEntry(HKLM_ERROR_MESSAGE_INSTRUMENT, EMI_ENABLEDEFAULTREPLY);
		wkpSystem.DeleteEntry(HKLM_ERROR_MESSAGE_INSTRUMENT, EMI_ENABLELOGGING);
		wkpSystem.DeleteEntry(HKLM_ERROR_MESSAGE_INSTRUMENT, EMI_LOGSEVERITY);
	}

	if (IsWin95())
	{
		DWORD dwAt = GetFileAttributes(szMsdos);
		SetFileAttributes(szMsdos,FILE_ATTRIBUTE_NORMAL);
		Sleep(100);
		WritePrivateProfileString(_T("Options"),_T("Logo"),_T("1"),szMsdos);
		WritePrivateProfileString(_T("Options"),_T("BootKeys"),_T("1"),szMsdos);
		WritePrivateProfileString(NULL,NULL,NULL,szMsdos);
		Sleep(100);
		SetFileAttributes(szMsdos,dwAt);
	}

	CString sOldName,sNewName;
	 
	if (IsWin95())
	{
		sOldName = GetWindowsDirectory() + _T('\\') + _T("taskman.000");
		sNewName = GetWindowsDirectory() + _T('\\') + _T("taskman.exe");
	}
	else if (IsNT())
	{
		sOldName = GetSystemDirectory() + _T('\\') + _T("taskmgr.000");
		sNewName = GetSystemDirectory() + _T('\\') + _T("taskmgr.exe");
	}

	TRY
	{
		CFile::Rename( sOldName, sNewName );
	}
	CATCH( CFileException, e )
	{
	}
	END_CATCH
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnBackup()
{
	if (m_pDongle->RunCDRWriter())
	{
		if (m_Mode != LM_BACKUP)
		{
			if (Login(FALSE))
			{
				RebootBackup();
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnBackupFinished()
{
	m_pMainWnd->KillTimer(1);

	CAfterBackupDialog dlg(m_pMainWnd);

	if (IDOK==dlg.DoModal())
	{
		WK_TRACE(_T("CD Sicherung beendet\n"));
		CWK_Profile wkp;
		wkp.WriteInt(szSection,_T("Mode"),LM_NORMAL);
		// end session any way
		TerminateAll();

		m_pMainWnd->DestroyWindow();
	}
	else
	{
		// restart DBBackup
		CApplication* pApp;
		pApp = m_Apps.GetApplication(WAI_DATABASE_SERVER);
		if (pApp)
		{
			pApp->InitialStart();
		}
		pApp = m_Apps.GetApplication(WAI_DATABASE_CLIENT);
		if (pApp)
		{
			pApp->InitialStart();
		}
		m_pMainWnd->SetTimer(1,(IsBackupMode() ? 5 : TIMER_SECONDS) * 1000,NULL);	
	}
}
///////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnUpdateProductView(CCmdUI* pCmdUI) 
{
	CApplication* pApp = m_Apps.GetApplication(WAI_PRODUCT_VIEW);
	pCmdUI->Enable(pApp && !pApp->IsEmpty());
}
///////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnUpdateVersion(CCmdUI* pCmdUI) 
{
	CApplication* pApp = m_Apps.GetApplication(WAI_PRODUCT_VERSION);
	pCmdUI->Enable(pApp && !pApp->IsEmpty());
}
///////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnUpdateSdiConfig(CCmdUI* pCmdUI) 
{
	CApplication* pApp = m_Apps.GetApplication(WAI_SDICONFIG);
	pCmdUI->Enable(pApp && !pApp->IsEmpty());
}
///////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnExplorer() 
{
	if (Login())
	{
		StartExplorer();
	}
}
///////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnExplorerShell() 
{
	if (Login())
	{
		TerminateAll();
		RebootWinExplorer();
	}
}
///////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnLauncherShell() 
{
	if (Login())
	{
		TerminateAll();
		RebootWinLauncher();
	}
}
///////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnRestart() 
{
	if (Login())
	{
		SysReboot();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::TraceSystemInformation()
{
	CWK_Profile wkp;
	DWORD dwRet;
	TIME_ZONE_INFORMATION ti;
	CString s;
	CString sn,dn;

	dwRet = GetTimeZoneInformation(&ti);
	sn = ti.StandardName;
	dn = ti.DaylightName;
	CSystemTime st,dt;
	CopyMemory(&st,&ti.StandardDate,sizeof(SYSTEMTIME));
	CopyMemory(&dt,&ti.DaylightDate,sizeof(SYSTEMTIME));
	if (dwRet == TIME_ZONE_ID_STANDARD)
	{
		WK_TRACE(_T("current time zone is STANDARD\n")); 
		wkp.WriteString(szSection,_T("Time"),_T("MEZ"));
	}
	else if (dwRet == TIME_ZONE_ID_DAYLIGHT)
	{
		WK_TRACE(_T("current time zone is DAYLIGHT\n")); 
		wkp.WriteString(szSection,_T("Time"),_T("MESZ"));
	}
	WK_TRACE(_T("Bias = %d\n"),ti.Bias);
	WK_TRACE(_T("StandardName = %s, Standard Date = %s\n"),sn,st.ToString());
	WK_TRACE(_T("DaylightName = %s, Daylight Date = %s\n"),dn,dt.ToString());
	WK_TRACE(_T("DaylightBias = %d\n"),ti.DaylightBias);

	HDC hDC = GetDC(NULL);
	WK_TRACE(_T("Screen Resolution is %dx%dx%dbpp\n"),
			GetSystemMetrics(SM_CXSCREEN),
			GetSystemMetrics(SM_CYSCREEN),
			GetDeviceCaps(hDC,BITSPIXEL));
	ReleaseDC(NULL,hDC);
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnLogView() 
{
	StartClientApp(WAI_LOG_VIEW);					 
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnUpdateLogView(CCmdUI* pCmdUI) 
{
	CApplication* pApp = m_Apps.GetApplication(WAI_LOG_VIEW);
	pCmdUI->Enable(pApp && !pApp->IsEmpty());
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnDatetime() 
{
	m_bDateTimeDlgStarted = TRUE;
	if (IsNT())
	{
		CSystemTime st;
		st.GetLocalTime();

		AdjustPrivileges(SE_SYSTEMTIME_NAME,SE_PRIVILEGE_USED_FOR_ACCESS);
		if (!SetLocalTime(&st))
		{
			CString sMessage;
			sMessage.LoadString(IDS_NOTIME_PERMISSION_NT);
			COemGuiApi::MessageBox(sMessage,20);
			m_bDateTimeDlgStarted = FALSE;
			return;
		}
	}


	CUser*		pUser = NULL;
	CUserArray ua;
	CWK_Profile wkp;
	COEMLoginDialog logindlg(m_pMainWnd,WK_ALLOW_DATE_TIME);

	logindlg.SetShowTopMost(TRUE);
#if _MFC_VER >= 0x0700
	logindlg.SetMonitorFlags(LOGIN_HIDE_LOGO);
#endif
	BOOL bRet = FALSE;

	bRet = FALSE;
	ua.Load(wkp);
	if (m_TemporaryUserID != SECID_NO_ID)
	{
		pUser = ua.GetUser(m_TemporaryUserID);
		m_TemporaryUserID = SECID_NO_ID;
	}

	if (   pUser
		|| IDOK==logindlg.DoModal())
	{
		CUser user;
		if (pUser)
		{
			user = *pUser;
		}
		else
		{
			user = logindlg.GetUser();
		}

		WK_TRACE(_T("%s calls date time change\n"),user.GetName());
		
		COEMDateTimeDialog datetimedlg(m_pMainWnd);
		datetimedlg.SetShowTopMost(TRUE);

		if (IDOK==datetimedlg.DoModal())
		{
			CSystemTime stTime = datetimedlg.GetSystemTime();

			WK_TRACE(_T("%s wants to change date time to %s\n"),user.GetName(),stTime.GetDateTime());

			if (IsNT())
			{
				AdjustPrivileges(SE_SYSTEMTIME_NAME,SE_PRIVILEGE_USED_FOR_ACCESS);
			}

			if (SetLocalTime(&stTime))
			{
				WK_TRACE(_T("%s changes date time to %s\n"),user.GetName(),stTime.GetDateTime());
				PostMessage(HWND_BROADCAST,WM_TIMECHANGE,0,0);
				OnTimeChangedBySoftware();
			}
			else
			{
				WK_TRACE_ERROR(_T("time change failed %s"),GetLastErrorString());
				if (IsNT())
				{
					CString sMessage;
					sMessage.LoadString(IDS_NOTIME_PERMISSION_NT);
					COemGuiApi::MessageBox(sMessage,20);
				}
			}
			if (IsNT())
			{
				AdjustPrivileges(SE_SYSTEMTIME_NAME,0);
			}
		}
	}
	m_bDateTimeDlgStarted = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnEuroChange() 
{
	// Nur mit Password
	if (Login(FALSE)) 
	{
		// Und noch eine Sicherheitsabfrage
		if (IDYES == AfxMessageBox(IDP_EURO_CHANGE_REALLY, MB_ICONSTOP|MB_YESNO|MB_DEFBUTTON2) ) {
			CWK_Profile wkp;
			CString sSection;
			CString sEntry = _T("Currency");
			CString sEuro = _T("EUR");
			CString sEntryFound;
			CString sEntryNotFound = _T("###");
			// EUR in alle vorhandenen Unit-Sections eintragen
			// und Units ggf. neu starten
			if (m_pDongle->RunGAUnit()) {
				sSection = _T("GAA");
				sEntryFound = wkp.GetString(sSection, sEntry, sEntryNotFound);
				if (sEntryFound != sEntryNotFound) {
					wkp.WriteString(sSection,sEntry,sEuro);
					CApplication* pApp = m_Apps.GetApplication(WAI_GAUNIT);
					if (pApp && pApp->IsRunning()) {
						pApp->Terminate();
					}
					StartClientApp(WAI_GAUNIT);
				}
			}
			if (m_pDongle->RunSDIUnit()) {
				BOOL bChanged = FALSE;
				DWORD dwComMask=0, dwBit;
				int i;
				dwComMask = wkp.GetInt(_T("Common"),_T("CommPortInfo"),dwComMask);
				for (dwBit = 1,i=1; dwBit && dwBit<dwComMask; dwBit<<=1,i++) {
					if (dwComMask & dwBit) {
						sSection.Format(_T("SDIUnit\\COM%i"), i);
						sEntryFound = wkp.GetString(sSection, sEntry, sEntryNotFound);
						if (sEntryFound != sEntryNotFound) {
							wkp.WriteString(sSection,sEntry,sEuro);
							bChanged = TRUE;
						}
					}
				}
				if (bChanged) {
					CApplication* pApp = m_Apps.GetApplication(WAI_SDIUNIT);
					if (pApp && pApp->IsRunning()) {
						pApp->Terminate();
					}
					StartClientApp(WAI_SDIUNIT);
				}
			}
			wkp.WriteInt(_T("Common"),_T("EuroChangeDone"),1);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnImageCompare() 
{
	StartClientApp(WAI_IMAGE_COMPARE);
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnUpdateImageCompare(CCmdUI* pCmdUI) 
{
	CApplication* pApp = m_Apps.GetApplication(WAI_IMAGE_COMPARE);
	pCmdUI->Enable(pApp && !pApp->IsEmpty());
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnConfirmSelfcheck(WK_ApplicationId AppId)
{
	for (int nI = 0; nI < m_Apps.GetSize(); nI++)
	{
		if (m_Apps.GetAt(nI)->GetId() == AppId)
			m_Apps.GetAt(nI)->ConfirmSelfcheckMessage();
	}
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK ShellProc(int nCode,WPARAM wParam,LPARAM lParam)
{
	WK_TRACE(_T("ShellProc %d,%d,%d\n"),nCode,wParam,lParam);
	switch (nCode)
	{
	case HSHELL_WINDOWCREATED:
		break;
	case HSHELL_WINDOWDESTROYED:
		break;
	case HSHELL_ACTIVATESHELLWINDOW:
		break;
	case HSHELL_WINDOWACTIVATED:
		break;
	case HSHELL_GETMINRECT:
		break;
	case HSHELL_REDRAW:
		break;
	case HSHELL_TASKMAN:
		break;
	case HSHELL_LANGUAGE:
		break;
	}

	return 0L;
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::DisableWriteCache()
{
	CWK_Profile prof;

	// Festplattencache abschalten gewünscht?
	BOOL bDisableHDCache = prof.GetInt(szSection, _T("DisableHDWriteCache"), 0);
	prof.WriteInt(szSection, _T("DisableHDWriteCache"), bDisableHDCache);
	if (bDisableHDCache)
	{	
		UINT uDeviceNum = 0;

		CWinAta* pAta = new CWinAta(uDeviceNum);
		if (pAta)
		{
			if (pAta->GetAtaErrorCode())
			{
				if (pAta->GetAtaErrorCode() == CWinAta::aec_OSNotSupported)
					WK_TRACE_WARNING(_T("Wrong OS...can't activate/deactivate write cache with ATA-cmds\n"));
				else
					WK_TRACE_WARNING(_T("Disk%u communication error (%u)\n"), uDeviceNum, pAta->GetAtaErrorCode());
			}
			else
			{
				if (pAta->DisableWriteCache())
					WK_TRACE(_T("Write cache is now deactivated.\n"));
				else
					WK_TRACE_WARNING(_T("Write cache deactivating failed.\n"));
			}
			WK_DELETE(pAta);
		}
	}
	else
	{
		UINT uDeviceNum = 0;

		CWinAta* pAta = new CWinAta(uDeviceNum);
		if (pAta)
		{
			if (pAta->GetAtaErrorCode())
			{
				if (pAta->GetAtaErrorCode() == CWinAta::aec_OSNotSupported)
					WK_TRACE_WARNING(_T("Wrong OS...can't activate/deactivate write cache with ATA-cmds\n"));
				else
					WK_TRACE_WARNING(_T("Disk%u communication error (%u)\n"), uDeviceNum, pAta->GetAtaErrorCode());
			}
			else
			{
				if (pAta->EnableWriteCache())
					WK_TRACE(_T("Write cache is now activated.\n"));
				else
					WK_TRACE_WARNING(_T("Write cache activating failed.\n"));
			}
			WK_DELETE(pAta);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnUpdateHandler() 
{
	StartClientApp(WAI_UPDATE_HANDLER);
}
/////////////////////////////////////////////////////////////////////////////
LPCTSTR CSecurityLauncherApp::GetErrorDat()
{
	return m_sErrorDat;
}
/////////////////////////////////////////////////////////////////////////////
LPCTSTR CSecurityLauncherApp::GetSection()
{
	return szSection;
}
/////////////////////////////////////////////////////////////////////////////
CBackGroundWnd* CSecurityLauncherApp::GetBackgroundWnd(int n)
{
	if (n>=0 && n<m_nMonitors && m_ppBackGroundWnd)
	{
		return m_ppBackGroundWnd[n];
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CSecurityLauncherApp::OnApplication(WK_ApplicationId wai, LPARAM lParam)
{
	WORD wStartParam = LOWORD((DWORD)lParam);
	WORD wUserID     = HIWORD((DWORD)lParam);
	CWnd *pMainWnd = AfxGetMainWnd();
	if (wStartParam)	
	{
		if (wai == WAI_LAUNCHER)
		{
			switch (wStartParam)
			{
				case LAUNCHER_DATE_TIME_DLG:
					if (!m_bDateTimeDlgStarted)
					{
						m_TemporaryUserID.Set(SECID_GROUP_USER, wUserID);
						pMainWnd->PostMessage(WM_COMMAND, ID_DATETIME, 0);
					}
					break;
				case LAUNCHER_RUNTIME_ERROR_DLG:
					pMainWnd->PostMessage(WM_COMMAND, ID_ERROR, 0);
					break;
				case LAUNCHER_START_AS_SHELL:
				case LAUNCHER_START_NOT_AS_SHELL:
					m_TemporaryUserID.Set(SECID_GROUP_USER, wUserID);
					if (AmIShell())
					{
						pMainWnd->PostMessage(WM_COMMAND, ID_EXPLORER_SHELL, 0);
					}
					else
					{
						pMainWnd->PostMessage(WM_COMMAND, ID_LAUNCHER_SHELL, 0);
					}
					break;
				case LAUNCHER_START_EXPLORER:
					m_TemporaryUserID.Set(SECID_GROUP_USER, wUserID);
					pMainWnd->PostMessage(WM_COMMAND, ID_EXPLORER, 0);
					break;
				case LAUNCHER_START_BACKUP:
					pMainWnd->PostMessage(WM_COMMAND, ID_BACKUP, 0);
					break;
				case LAUNCHER_IS_SHELL:
					return AmIShell();
				case LAUNCHER_SOFTWARE_UPDATE:
					pMainWnd->PostMessage(WM_COMMAND, ID_SOFTWARE_UPDATE, 0);
					break;
				case LAUNCHER_EXTENSION_CODE:
					pMainWnd->PostMessage(WM_COMMAND, ID_EXTENSION_CODE, 0);
					break;
			}
		}
		else
		{
			if (wai == WAI_ISDN_UNIT)
			{
				WORD wMsg      = LOWORD((DWORD)lParam);
				WORD wIncrease = HIWORD((DWORD)lParam);
				if (wMsg == WM_ISDN_B_CHANNELS)
				{
					CApplication* pApp, *pApp2;
					pApp = m_Apps.GetApplication(wai);
					pApp2 = m_Apps.GetApplication(WAI_ISDN_UNIT_2);
					if (pApp && pApp2)
					{
						pApp->PostMessage(wMsg, wIncrease, 0);
						pApp2->PostMessage(wMsg, wIncrease, 0);
					}
					return 0;
				}
			}
			switch (wStartParam)
			{
				case LAUNCHER_TERMINATE_APPLICATION:
				{
					CApplication* pApp;
					pApp = m_Apps.GetApplication(wai);
					if (pApp)
					{
						pApp->Terminate();
						return TRUE;
					}
				} break;
				case LAUNCHER_CLOSE_APPLICATION:
				{
					CApplication* pApp;
					pApp = m_Apps.GetApplication(wai);
					BOOL bIsRunning = TRUE;
					if (pApp)
					{
						bIsRunning = pApp->IsRunning();
						if (bIsRunning)
						{
							int nOld = pApp->SetCloseTry(100);	// 5000 ms
							pApp->BringToTop();
							bIsRunning = !pApp->Close();
							pApp->SetCloseTry(nOld);
						}
					}
					pApp = m_Apps.GetApplication(WAI_IDIP_CLIENT);
					if (pApp)
					{
						pApp->PostMessage(LAUNCHER_APPLICATION, wai, MAKELONG(wStartParam, bIsRunning));
					}
				} break;
				case LAUNCHER_APPLICATION_RUNNING:
				{
					CApplication* pApp;
					pApp = m_Apps.GetApplication(wai);
					if (pApp)
					{
						return pApp->IsRunning();
					}
				}break;
				case LAUNCHER_UPDATE_LANG_DEPENDS:
				{
					CApplication* pApp;
					pApp = m_Apps.GetApplication(WAI_SUPERVISOR);
					if (pApp)
					{
						BOOL bIsRunning = pApp->IsRunning();
						if (bIsRunning)
						{
							int nOld = pApp->SetCloseTry(100);	// 5000 ms
							pApp->BringToTop();
							bIsRunning = !pApp->Close();
							pApp->SetCloseTry(nOld);
						}
						if (bIsRunning)
						{
							WK_TRACE(_T("Application %s is running: cannot update language dependencies\n"), pApp->GetName());
							return FALSE;
						}
						else
						{
							pApp->SetTemporaryCmdLine(CMD_PARAM_UPDATE_LD);
							pApp->Execute();
						}
					}
				}break;
				case LAUNCHER_RELOAD_SETTINGS:
				{
					CApplication* pApp;
					pApp = m_Apps.GetApplication(wai);
					if (pApp)
					{
						pApp->PostMessage(WM_COMMAND, ID_FILE_NEW, 0);
					}
				}break;
				case LAUNCHER_UPDATE_LANG_SETTINGS:
				{
					CApplication* pApp;
					int i, nApps = m_Apps.GetCount();
					for  (i=0; i<nApps; i++)
					{
						pApp = m_Apps.GetAt(i);
						pApp->PostMessage(WM_LANGUAGE_CHANGED, 0, 0);
					}
				}break;
			}
		}
	}
	else
	{
		switch (wai)
		{
			case WAI_LAUNCHER:		// extit Idip
				CWinApp::OnAppExit();
			break;
			case WAI_IDIP_CLIENT:
			{	// restart programms with multilingual user interfaces
				CApplication* pApp;
				pApp = m_Apps.GetApplication(wai);
				DWORD dwCPBits = 0;
				UINT uCodePage = 0;
				if (pApp)
				{
					CWK_Profile wkp;
					pApp->Close();
					pApp->SetUserID(wUserID);
					pApp->Execute();
					COemGuiApi::ChangeLanguageResource();
					COemGuiApi::SetSystemUIlanguage(TRUE);
					CString sCodePage = COemGuiApi::GetCodePage();
					uCodePage = _ttoi(sCodePage);
					dwCPBits  = COemGuiApi::GetCodePageBits();
					CWK_String::SetCodePage(uCodePage);
					wkp.SetCodePage(uCodePage);
					CLoadResourceDll ResourceDll(NULL, NULL);
					HINSTANCE hLangResourceDLL = ResourceDll.DetachResourceDllHandle();
					if (hLangResourceDLL && hLangResourceDLL != m_hLangResourceDLL)
					{
						swap(hLangResourceDLL, m_hLangResourceDLL);
						AfxSetResourceHandle(m_hLangResourceDLL);
						FreeLibrary(hLangResourceDLL);
					}
				}
				int i, nApps = m_Apps.GetCount();
				for  (i=0; i<nApps; i++)
				{
					pApp = m_Apps.GetAt(i);
					if (pApp && pApp->GetId() != WAI_IDIP_CLIENT)
					{
						pApp->PostMessage(WM_LANGUAGE_CHANGED, uCodePage, dwCPBits);
					}
				}
				pApp = m_Apps.GetApplication(WAI_SUPERVISOR);
				if (pApp)
				{
					if (pApp->IsRunning())
					{
						WK_TRACE(_T("Application %s is running: cannot update language dependencies\n"), pApp->GetName());
					}
					else
					{
						pApp->SetTemporaryCmdLine(CMD_PARAM_UPDATE_LD);
						pApp->Execute();
					}
				}

			} break;
			default:
				StartClientApp(wai, wUserID);
			break;
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSecurityLauncherApp::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYUP && pMsg->wParam == VK_F11 && COemGuiApi::GetKeyboardInput(0).IsValid())
	{
		COemGuiApi::ToggleKeyboardInput();
		return TRUE;
	}

	return CWinApp::PreTranslateMessage(pMsg);
}
void CSecurityLauncherApp::CheckHDDTemperatures()
{
	// Die Temeraturen aller angeschlossenen IDE-Festplatten ausgeben, sofern möglich.
	CHDDTApi Hdd;
	int nMaxTemperature = GetIntValue(_T("MaxTemperatureHDD"), 55);
	for (UCHAR uDrive = 0; uDrive < m_nMaxCheckedDrives; uDrive++)
	{
		int nTemperature = 0;
		if (Hdd.GetDriveTemperatur(uDrive, nTemperature))
		{
			// Nur protokolieren, wenn sich die Temperatur verändert hat.
			if (m_pnLastTemperatures[uDrive] != nTemperature)
			{
				WK_STAT_LOG(_T("Reset"), nTemperature, _T("Temperature"));
				if (m_pnLastTemperatures[uDrive] == VIRTUAL_ALARM_SET)
				{
					if (nTemperature < nMaxTemperature-5)
					{
						DWORD dwParam1 = MAKELONG(nTemperature, DEVICE_HARDDISK);
						m_newErrors.SafeAdd(new CWK_RunTimeError(WAI_LAUNCHER, REL_WARNING, RTE_TEMPERATURE, _T(""), dwParam1));
						if (GetMainWnd())
							GetMainWnd()->PostMessage(WM_COMMAND, ID_ERROR_ARRIVED);
						m_pnLastTemperatures[uDrive] = nTemperature;
					}
				}
				else
				{
					m_pnLastTemperatures[uDrive] = nTemperature;
					if (nTemperature > nMaxTemperature)
					{
						CString sDrive;
						DRIVE_INFO drive_info;
						if (Hdd.GetDriveInfo(uDrive, drive_info))
						{
							CString sMN(drive_info.ModelNumber);
							CString sSN(drive_info.SerialNumber);
							CString sFWR(drive_info.FirmWareRev);
							sDrive.Format(_T("%c:, Model:%s, Serial:%s, Rev:%s"), uDrive, sMN, sSN, sFWR);
						}
						else
						{
							sDrive.Format(_T("%c:"), uDrive);
						}
						DWORD dwParam1 = MAKELONG(nTemperature, DEVICE_HARDDISK);
						DWORD dwParam2 = MAKELONG(3, nMaxTemperature); // alarm on
						m_newErrors.SafeAdd(new CWK_RunTimeError(WAI_LAUNCHER, REL_WARNING, RTE_TEMPERATURE, sDrive, dwParam1, dwParam2));
						if (GetMainWnd())
							GetMainWnd()->PostMessage(WM_COMMAND, ID_ERROR_ARRIVED);
						m_pnLastTemperatures[uDrive] = VIRTUAL_ALARM_SET;
					}
				}
			}

			if (   m_pDongle
				&& m_pDongle->IsTransmitter())
			{
				//safe the highest temperature in registry for maintenance function in system management
				CWK_Profile wkp;
				CString sSection(_T("Maintenance"));
				CString sKey, sTemperatureOld, sTemperatureNew, sTrace;
				sKey.Format(_T("Temp%d"), uDrive);
				sTemperatureOld = wkp.GetString(sSection, sKey, _T(""));

				if(    sTemperatureOld.IsEmpty() 
					|| _wtoi(sTemperatureOld) < nTemperature)
				{
					sTemperatureNew.Format(_T("%d"), nTemperature);
					wkp.WriteString(sSection, sKey, sTemperatureNew);
					sTrace.Format(_T("Temperature HDD #%d rised from %s °C to %s °C"), 
									uDrive, sTemperatureOld, sTemperatureNew);
					WK_TRACE(sTrace);
				}
				WK_TRACE(_T("\n"));
			}
		}
		else
		{
			TRACE(_T("Could not read Temperature of HDD %d\n"), uDrive);
		}
	}


}
///////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::CheckSMARTparameters()
{
	if (IsWinXP() && m_pDongle && m_pDongle->IsTransmitter())
	{
		GETVERSIONOUTPARAMS VersionParams;

		CSmart smart;
		BOOL bAllOK = TRUE;
		
		for (BYTE nDrive = 0; nDrive < m_nMaxCheckedDrives; nDrive++)
		{
			if (smart.Open(nDrive))
			{
				if (smart.GetVersionInfo(VersionParams))
				{
					BOOL bState = FALSE;
					if (smart.EnableSmart())
					{
						smart.Identify();
						if (smart.CheckDrive(bState, FALSE))
						{
							if (bState)
							{
								WK_TRACE(_T("S.M.A.R.T status: OK (Drive %d <%s>)\n"), nDrive, smart.GetModelNumber());
							}
							else
							{
								CString sErrorMsg;
								sErrorMsg.Format(_T("S.M.A.R.T status failed!\nDrive %d <%s>)\n"), nDrive, smart.GetModelNumber());
								WK_TRACE_WARNING(sErrorMsg);				
								
								DWORD dwParam1 = MAKELONG(nDrive, 0);
								DWORD dwParam2 = 2|1; // alarm on
								m_newErrors.SafeAdd(new CWK_RunTimeError(WAI_LAUNCHER, REL_ERROR, RTE_SMART, sErrorMsg, dwParam1, dwParam2));
								if (GetMainWnd())
								{
									GetMainWnd()->PostMessage(WM_COMMAND, ID_ERROR_ARRIVED);
								}
								bAllOK = FALSE;
							}
						}
					}
				}
			}
		}

		if (bAllOK)
		{
			m_newErrors.SafeAdd(new CWK_RunTimeError(WAI_LAUNCHER, REL_MESSAGE, RTE_SMART, _T("")));
			if (GetMainWnd())
			{
				GetMainWnd()->PostMessage(WM_COMMAND, ID_ERROR_ARRIVED);
			}
		}
	}
}
///////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnAppExit()
{
	// same as double-clicking on main window close box
	ASSERT(m_pMainWnd != NULL);
	m_pMainWnd->PostMessage(WM_CLOSE);
}
///////////////////////////////////////////////////////////////////////////////
LPCTSTR CSecurityLauncherApp::GetModuleSection()
{
	return szModules;
}
///////////////////////////////////////////////////////////////////////////////
int	CSecurityLauncherApp::GetIntValue(LPCTSTR sValue, int nDefault/*=0*/)
{
	CWK_Profile wkp;
	int nValue = wkp.GetInt(szSection, sValue, -1);
	if (nValue == -1)
	{
		nValue = nDefault;
		wkp.WriteInt(szSection, sValue, nDefault);
	}
	return nValue;
}
///////////////////////////////////////////////////////////////////////////////
BOOL CSecurityLauncherApp::DoShowError(CWK_RunTimeError*pRTE)
{
	WORD wShow = LOWORD(m_dwShowErrors);
	switch(pRTE->GetLevel())
	{
	case REL_REBOOT_ERROR: case REL_RESTART_ME: case REL_CANNOT_RUN: 
		if (wShow & SHOW_CRITICAL) return TRUE;
		break;
	case REL_ERROR:
		if (wShow & SHOW_ERRORS) return TRUE;
		break;
	case REL_WARNING:
		if (wShow & SHOW_WARNINGS) return TRUE;
		break;
	case REL_MESSAGE:
		if (wShow & SHOW_MESSAGES) return TRUE;
		break;
	default:
		break;
	}
	return FALSE;
}
///////////////////////////////////////////////////////////////////////////////
BOOL CSecurityLauncherApp::DoPopupErrorDlg(CWK_RunTimeError*pRTE)
{
	WORD wShow = HIWORD(m_dwShowErrors);
	CString sName = NameOfEnum(pRTE->GetError());
	sName = POPUP_RTE + sName;
	CWK_Profile wkp;
	int nSize = m_Errors.GetSize();
	if ((nSize % 100) == 0)
	{
		return TRUE;
	}
	if (wkp.GetInt(SECTION_LAUNCHER, sName, 1) == 0)
	{
		return FALSE;
	}
	switch(pRTE->GetLevel())
	{
	case REL_REBOOT_ERROR: case REL_RESTART_ME: case REL_CANNOT_RUN: 
		if (wShow & SHOW_CRITICAL) return TRUE;
		break;
	case REL_ERROR:
		if (wShow & SHOW_ERRORS) return TRUE;
		break;
	case REL_WARNING:
		if (wShow & SHOW_WARNINGS) return TRUE;
		break;
	case REL_MESSAGE:
		if (wShow & SHOW_MESSAGES) return TRUE;
		break;
	default:
		break;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnProductView()
{
	StartClientApp(WAI_PRODUCT_VIEW);
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnVersion()
{
	StartClientApp(WAI_PRODUCT_VERSION);
}
/////////////////////////////////////////////////////////////////////////////
int CSecurityLauncherApp::DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt)
{
	CString sMsg;
	if (lpszPrompt)
	{
		sMsg = lpszPrompt;
	}
	else if (nIDPrompt)
	{
		sMsg.LoadString(nIDPrompt);
	}
	return COemGuiApi::MessageBox(sMsg, 0, nType|MB_SETFOREGROUND);
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnSoftwareUpdate()
{
	DWORD dwMask=1, dwLogical = GetLogicalDrives();
	int i;				// list the available disks
	CString sPath, sTitle, sString;
	CStringArray ar;

 	for (i=0,dwMask=1; i<26; i++,dwMask<<=1)
	{
		if (dwMask & dwLogical)
		{
			sPath.Format(_T("%c:\\"), i+'a');
			int nType = GetDriveType(sPath);
			switch (nType)
			{
			case DRIVE_CDROM:
			case DRIVE_REMOVABLE:
				WK_SearchFilesRecursiv(ar, sPath, _T("setup.exe"));
				break;
			}
		}
	}

	WK_SearchFilesRecursiv(ar, m_sTempDirectory, _T("setup.exe"));

	int n = ar.GetCount();
	if (n)
	{
		for (i=0; i<n; i++)
		{
			sString = ar[i];
			sString.MakeLower();
			sString.Replace(_T(".exe"), _T(".W02"));
			if (DoesFileExist(sString) == FALSE)
			{
				ar.RemoveAt(i);
				i--;
				n--;
			}
		}
	}
	BOOL bInstall = FALSE;
	n = ar.GetCount();
	sTitle.LoadString(ID_SOFTWARE_UPDATE);
	if (n == 1)
	{
		sPath = ar[0];
		sTitle.Replace(_T("..."), _T(": "));
		sString = sTitle + _T("\n") + sPath + _T("?");
		bInstall = AfxMessageBox(sString, MB_YESNO|MB_ICONINFORMATION) == IDYES;
	}
	else
	{
		CWnd *pWnd = GetBackgroundWnd(0);
		if (pWnd == NULL)
		{
			pWnd = m_pActiveWnd;
		}
		COemFileDialog dlg(pWnd);
		CString sOpen,sDir,sFile;
		sOpen.LoadString(AFX_IDS_OPENFILE);
		dlg.SetProperties(TRUE, sTitle, sOpen);
		if (n)
		{
			WK_SplitPath(ar[0], sDir, sFile);
			dlg.SetInitialDirectory(sDir);
		}
		dlg.AddFilter(_T("Setup"), _T("exe"));
		bInstall = dlg.DoModal() == IDOK;
		sPath = dlg.GetPathname();
	}
	if (bInstall)
	{
		sPath += _T(" -auto");
		CUpdateRecord udr(RFU_STRING_INTERFACE, sPath, NULL, 0, FALSE);
		udr.Setup(sPath);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnUpdateSoftwareUpdate(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnExtensionCode()
{
	CWnd *pWnd = GetBackgroundWnd(0);
	if (pWnd == NULL)
	{
		pWnd = m_pActiveWnd;
	}
	CExtensionCodeDlg dlg(pWnd);
	dlg.DoModal();
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnUpdateExtensionCode(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::AddNewError(CWK_RunTimeError* pNewRTE)
{
	m_newErrors.SafeAdd(pNewRTE);
	if (GetMainWnd())
		GetMainWnd()->PostMessage(WM_COMMAND, ID_ERROR_ARRIVED);
}
/////////////////////////////////////////////////////////////////////////////
/*
///////////////////////////////////////////////////////////////////////////////
BOOL CSecurityLauncherApp::InitializeDrive(int nDrive)
{
	BOOL bResult = FALSE;

	CDrive Drive(nDrive);

	// Diskgeometrie informationejn holen und ausgeben.
	DISK_GEOMETRY_EX DiskGeometry;
	if (Drive.GetDriveGeometry(DiskGeometry))
		Drive.TraceDriveGeometry(DiskGeometry);

	// Partitionsinformationen holen und ausgeben
	Drive.TraceAllPartitionInfos();

	// Partitioniere laufwerk mit einer Partition.
	bResult = Drive.PartitionDrive();
	if (bResult)
		Sleep(5000); // Ein wenig warten, bis Windows die neue Partition erkannt hat

	return bResult;
}
///////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::FormatAllUnformatedFixedVolumes()
{
	TCHAR Volumes[100];

	DWORD dwSysFlags;            // flags that describe the file system
	TCHAR FileSysNameBuf[FILESYSNAMEBUFSIZE];
	GetLogicalDriveStrings(sizeof(Volumes), Volumes);
	
	int nI		= 0;
	int nVolume = 0;

	while (Volumes[nI] != '\0')
	{
		CString sVolume(&Volumes[nI+=4]);
		if (!sVolume.IsEmpty())
		{
			if (GetDriveType(sVolume) == DRIVE_FIXED)
			{
				// Hat diese Volume ein gültiges Filesystem?
				if (GetVolumeInformation(sVolume, NULL, 0, NULL, NULL, &dwSysFlags, FileSysNameBuf, FILESYSNAMEBUFSIZE))
					WK_TRACE(_T("Volume:%d Volumename:%s Filesystem=%s\n"), nVolume++, sVolume, FileSysNameBuf);
				else
				{
					if (GetLastError() == ERROR_UNRECOGNIZED_VOLUME)
					{
						WK_TRACE(_T("Volume: %s has no valid filesystem\n"), sVolume);
						CDrive::Format(sVolume);
					}
				}
			}
		}
	}
}
*/

