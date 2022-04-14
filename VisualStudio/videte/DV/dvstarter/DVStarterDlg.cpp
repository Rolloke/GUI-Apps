// DVStarterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DVStarter.h"
#include "DVStarterDlg.h"
#include "CAboutDlg.h"
#include "CRTErrorDlg.h"
#include "wkclasses\CLogFile.h"
#include <HardwareInfo\\CBoards.h>
#include <HardwareInfo\\W83627.h>

#include <dbt.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define VIRTUAL_ALARM_SET -50

CDVStarterDlg* CDVStarterDlg::m_pThis = NULL;
extern  CDVStarterApp theApp;

/////////////////////////////////////////////////////////////////////////////
CDVStarterDlg::CDVStarterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDVStarterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDVStarterDlg)
	m_sTotalUsedMemory = _T("0 KB");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon					= AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pRTErrorThread		= NULL;
	m_bRun					= FALSE;
	m_nTimerIntervall		= 20*1000;
	m_pThis					= this;
	m_bTraceMemInf			= TRUE;
	m_dwTotalUsedMemory		= 0;
	m_hPsApiDll				= NULL;
	m_EnumProcesses			= NULL;
	m_GetProcessMemoryInfo	= NULL;
	m_EnumProcessModules	= NULL;
	m_GetModuleBaseName		= NULL;
	m_bQueryEndSessionReturn= FALSE;

	StartRTErrorThread();
	m_WatchDog.Disable();

	// Die PsApi.Dll wird nur unter den NT-Betriebssystemen unterstützt. Daher muß die PsApi library
	// dynamisch geladen werden.
	if (theApp.IsNT())
	{
		m_hPsApiDll = AfxLoadLibrary(_T("PsApi.Dll"));
		if (m_hPsApiDll)
			WK_TRACE(_T("PsApi.dll loaded\n"));
		else
			WK_TRACE_ERROR(_T("PsApi.dll not loaded Error %u <%s>\n"), GetLastError(), GetLastErrorString());
	
		if (m_hPsApiDll)
		{
			m_EnumProcesses			= (ENUMPROCESS)GetProcAddress(m_hPsApiDll, "EnumProcesses");
			m_GetProcessMemoryInfo	= (GETPROCESSMEMORYINFO)GetProcAddress(m_hPsApiDll, "GetProcessMemoryInfo");
			m_EnumProcessModules	= (ENUMPROCESSMODULES)GetProcAddress(m_hPsApiDll, "EnumProcessModules");
#ifdef UNICODE
			m_GetModuleBaseName		= (GETMODULEBASENAME)GetProcAddress(m_hPsApiDll, "GetModuleBaseNameW");
#else
			m_GetModuleBaseName		= (GETMODULEBASENAME)GetProcAddress(m_hPsApiDll, "GetModuleBaseNameA");
#endif // !UNICODE
		if (!m_EnumProcesses || !m_GetProcessMemoryInfo || !m_EnumProcessModules || !m_GetModuleBaseName)
			WK_TRACE_ERROR(_T("PsApi function not found <%s>\n"), GetLastErrorString());
		}
	}

	CWK_Profile prof;

	m_bTraceMemInf = prof.GetInt(DV_DVSTARTER_DEBUG, _T("TraceMemoryInformation"), m_bTraceMemInf);
	prof.WriteInt(DV_DVSTARTER_DEBUG, _T("TraceMemoryInformation"), m_bTraceMemInf);

	// Ist dies ein DTS Sender?
	if (theApp.IsTransmitter())
	{
		// DVHook nur im Shellmodus starten
		if (theApp.AmIShell())
		{
			m_AppDVHook.Create(_T("DVHook.exe"), WAI_DV_HOOK, 1);
			m_AppDVHook.Load(SW_MINIMIZE, 10000);
		}

		CBoards Boards;

		// Hier kann das Laden der JaCob-, SaVic-, Tasha- und QUnits zu debugzwecken verhindert werden.
		int nAllowedJaCobs	= prof.GetInt(DV_DVSTARTER_DEBUG, _T("AllowedJaCobs"), 2);
		int nAllowedSaVics	= prof.GetInt(DV_DVSTARTER_DEBUG, _T("AllowedSaVics"), 2);
		int nAllowedTashas	= prof.GetInt(DV_DVSTARTER_DEBUG, _T("AllowedTashas"), 2);
		int nAllowedQs		= prof.GetInt(DV_DVSTARTER_DEBUG, _T("AllowedQs"), 1);
		prof.WriteInt(DV_DVSTARTER_DEBUG, _T("AllowedJaCobs"),   nAllowedJaCobs);
		prof.WriteInt(DV_DVSTARTER_DEBUG, _T("AllowedSaVics"),   nAllowedSaVics);
		prof.WriteInt(DV_DVSTARTER_DEBUG, _T("AllowedTashas"),   nAllowedTashas);
		prof.WriteInt(DV_DVSTARTER_DEBUG, _T("AllowedQs"),		 nAllowedQs);

		// Die maximale Anzahl der zu startenden Units evtl. begrenzen
		m_nNumbersOfJaCobs = min(nAllowedJaCobs, Boards.GetNumbersOfJaCobs());
		m_nNumbersOfSaVics = min(nAllowedSaVics, Boards.GetNumbersOfSaVics());
		m_nNumbersOfTashas = min(nAllowedTashas, Boards.GetNumbersOfTashas());
		m_nNumbersOfQs	   = min(nAllowedQs,	 Boards.GetNumbersOfQs());

		// 1. JaCob starten?
		if (m_nNumbersOfJaCobs > 0)
		{
			m_AppJaCob1.Create(_T("JaCobUnit.exe"),WAI_JACOBUNIT_1, 3);
			if (!m_AppJaCob1.IsRunning())
				m_AppJaCob1.Load(SW_MINIMIZE, 10000);
		}
		
		// 2. JaCob starten?
		if (m_nNumbersOfJaCobs > 1)
		{
			m_AppJaCob2.Create(_T("JaCobUnit.exe"),	WAI_JACOBUNIT_2, 3);
			if (!m_AppJaCob2.IsRunning())
				m_AppJaCob2.Load(SW_MINIMIZE, 20000);
		}

		// 1. SaVic starten?
		if (m_nNumbersOfSaVics > 0)
		{
			m_AppSaVic1.Create(_T("SaVicUnit.exe"),WAI_SAVICUNIT_1, 3);
			if (!m_AppSaVic1.IsRunning())
				m_AppSaVic1.Load(SW_MINIMIZE, 20000);
		}
		// 2. SaVic starten?
		if (m_nNumbersOfSaVics > 1)
		{
			m_AppSaVic2.Create(_T("SaVicUnit.exe"),WAI_SAVICUNIT_2, 3);
			if (!m_AppSaVic2.IsRunning())
				m_AppSaVic2.Load(SW_MINIMIZE, 20000);
		}
		// 3. SaVic starten?
		if (m_nNumbersOfSaVics > 2)
		{
			m_AppSaVic3.Create(_T("SaVicUnit.exe"),WAI_SAVICUNIT_3, 3);
			if (!m_AppSaVic3.IsRunning())
				m_AppSaVic3.Load(SW_MINIMIZE, 20000);
		}
		// 4. SaVic starten?
		if (m_nNumbersOfSaVics > 3)
		{
			m_AppSaVic4.Create(_T("SaVicUnit.exe"),WAI_SAVICUNIT_4, 3);
			if (!m_AppSaVic4.IsRunning())
				m_AppSaVic4.Load(SW_MINIMIZE, 20000);
		}

		// 1. Tasha starten?
		if (m_nNumbersOfTashas > 0)
		{
			m_AppTasha1.Create(_T("TashaUnit.exe"),WAI_TASHAUNIT_1, 3);
			if (!m_AppTasha1.IsRunning())
				m_AppTasha1.Load(SW_MINIMIZE, 60000);
		}
		// 2. Tasha starten?
		if (m_nNumbersOfTashas > 1)
		{
			m_AppTasha2.Create(_T("TashaUnit.exe"),WAI_TASHAUNIT_2, 3);
			if (!m_AppTasha2.IsRunning())
				m_AppTasha2.Load(SW_MINIMIZE, 60000);
		}
		// 3. Tasha starten?
		if (m_nNumbersOfTashas > 2)
		{
			m_AppTasha3.Create(_T("TashaUnit.exe"),WAI_TASHAUNIT_3, 3);
			if (!m_AppTasha3.IsRunning())
				m_AppTasha3.Load(SW_MINIMIZE, 60000);
		}
		// 4. Tasha starten?
		if (m_nNumbersOfTashas > 3)
		{
			m_AppTasha4.Create(_T("TashaUnit.exe"),WAI_TASHAUNIT_4, 3);
			if (!m_AppTasha4.IsRunning())
				m_AppTasha4.Load(SW_MINIMIZE, 60000);
		}

		// QUnit starten?
		if (m_nNumbersOfQs > 0)
		{
			m_AppQ.Create(_T("QUnit.exe"),WAI_QUNIT, 3);
			if (!m_AppQ.IsRunning())
				m_AppQ.Load(SW_MINIMIZE, 60000);
		}

		//check if ACDC.exe exists in directory C:\DV
		// DBUG mode is hardcoded in dvstarter.cpp anyway
		CString sAcdcPath = theApp.GetHomeDir() + _T("\\ACDC.exe");
		if(DoesFileExist(sAcdcPath))
		{
			//ACDC starten
			m_AppACDC.Create(_T("ACDC.exe"),	WAI_AC_DC, 3);
			if (!m_AppACDC.IsRunning())
				m_AppACDC.Load(SW_MINIMIZE, 5000);
		}
		else
		{
			WK_TRACE(_T("ACDC.exe does not exists\n"));
		}

		// CommUnit starten
		CString sCommUnitPath = theApp.GetHomeDir() + _T("\\communit.exe");
		if(DoesFileExist(sCommUnitPath))
		{
			m_AppCommUnit.Create(_T("CommUnit.exe"), WAI_COMMUNIT, 3);	
			if (!m_AppCommUnit.IsRunning())
				m_AppCommUnit.Load(SW_MINIMIZE, 5000);
		}
	
		// AudioUnit starten
		CString sAudioUnitPath = theApp.GetHomeDir() + _T("\\audiounit.exe");
		if(DoesFileExist(sAudioUnitPath))
		{
			m_AppAudioUnit.Create(_T("audiounit.exe"), WAI_AUDIOUNIT_1, 3);	
			if (!m_AppAudioUnit.IsRunning())
				m_AppAudioUnit.Load(SW_MINIMIZE, 5000);
		}

		// DVStorage starten.
		m_AppDVStorage.Create(_T("DVStorage.exe"), WAI_DATABASE_SERVER, 3);
		if (!m_AppDVStorage.IsRunning())
			m_AppDVStorage.Load(SW_MINIMIZE, 5000);

		// DVProcess starten
		m_AppDVProcess.Create(_T("DVProcess.exe"), WAI_SECURITY_SERVER, 3);	
		if (!m_AppDVProcess.IsRunning())
			m_AppDVProcess.Load(SW_MINIMIZE, 7000);

		CWK_Profile wkp;
		if (wkp.GetInt(REG_PATH_PC_HEALTH, STAND_ALONE, -1) != -1)
		{
			m_AppHealthControl.Create(WK_APP_NAME_HEALT_CONTROL _T(".exe"), WAI_HEALTH_CONTROL, 3);
			if (!m_AppHealthControl.IsRunning())
				m_AppHealthControl.Load(SW_MINIMIZE, 5000);
		}

		WK_TRACE(_T("Running as transmitter...\n"));
	}
	else
		WK_TRACE(_T("Running as receiver...\n"));

//#ifndef UNICODE 
	m_bIsNetworkInstalled = IsNetworkInstalled();
	m_bIsISDNInstalled = IsISDNInstalled();

	// ISDNUnit starten, wenn Sender und ISDN installiert und ISDN freigegeben ist
	if (m_bIsISDNInstalled) 
	{
		if (theApp.IsTransmitter())
		{
			if (IsISDNAllowed())
			{
				m_AppISDN1.Create(_T("ISDNUnit.exe"), WAI_ISDN_UNIT, 5);
				if (!m_AppISDN1.IsRunning())
				{
					m_AppISDN1.Load(SW_SHOWMINNOACTIVE, 5000);
				}
				m_AppISDN2.Create(_T("ISDNUnit.exe"), WAI_ISDN_UNIT_2, 5);
				if (!m_AppISDN2.IsRunning())
				{
					m_AppISDN2.Load(SW_SHOWMINNOACTIVE, 5000);
				}
			}
		}
		else if (theApp.IsReceiver())
		{
			m_AppISDN1.Create(_T("ISDNUnit.exe"), WAI_ISDN_UNIT, 5);
			if (!m_AppISDN1.IsRunning())
			{
				m_AppISDN1.Load(SW_SHOWMINNOACTIVE, 5000);
			}
		}
	}
//#endif
	// SocketUnit starten,
	// a) wenn Netzwerk installiert
	if (m_bIsNetworkInstalled)
	{
		// b) wenn Receiver oder Netzwerk freigegeben ist
		if (theApp.IsReceiver() || IsNetworkAllowed())
		{
			m_AppSocket.Create(_T("SocketUnit.exe"), WAI_SOCKET_UNIT, 3);
			if (!m_AppSocket.IsRunning())
				m_AppSocket.Load(SW_MINIMIZE, 5000);
		}
	}
	if (theApp.IsReceiver())
	{
		// AudioUnit starten
		CString sAudioUnitPath = theApp.GetHomeDir() + _T("\\audiounit.exe");
		if(DoesFileExist(sAudioUnitPath))
		{
			m_AppAudioUnit.Create(_T("audiounit.exe"), WAI_AUDIOUNIT_1, 3);	
			if (!m_AppAudioUnit.IsRunning())
				m_AppAudioUnit.Load(SW_MINIMIZE, 5000);
		}
	}
	// Client starten
	m_AppDVClient.Create(_T("DVClient.exe"), WAI_DV_CLIENT, 1);
	if (!m_AppDVClient.IsRunning())
		m_AppDVClient.Load(SW_SHOWMAXIMIZED, 5000);
		
	// Defaultwert schreiben, wenn keiner existiert.
	prof.WriteInt(DV_DVSTARTER, _T("AsShell"), prof.GetInt(DV_DVSTARTER, _T("AsShell"), 0));

	m_nMaxCheckedDrives = prof.GetInt(DV_DVSTARTER, _T("MaxCheckedDrives"), 4);
	m_pnLastTemperatures = new int[m_nMaxCheckedDrives];
	m_pbSMARTFailureReported = new BOOL[m_nMaxCheckedDrives];
	for (int i=0; i<m_nMaxCheckedDrives; i++)
	{
		m_pnLastTemperatures[i] = -1;
		m_pbSMARTFailureReported[i] = FALSE;
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDVStarterDlg::IsNetworkInstalled()
{
	CSocket socket;
	BOOL	bResult = FALSE;

	if (!socket.Create(0, SOCK_STREAM, _T("127.0.0.1")))
	{
		WK_TRACE(_T("No network detected\n"));
	}
	else
	{
		WK_TRACE(_T("Network detected\n"));
		bResult = TRUE;
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDVStarterDlg::IsISDNInstalled()
{
	HINSTANCE hInst = LoadLibrary(_T("Capi2032.dll"));
	BOOL	bResult = FALSE;

	if (!hInst)
	{
		WK_TRACE(_T("No ISDN detected (%s)\n"), GetLastErrorString());
	}
	else
	{
		WK_TRACE(_T("ISDN detected\n"));
		FreeLibrary(hInst);
		bResult = TRUE;
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDVStarterDlg::IsNetworkAllowed()
{
	CWK_Profile prof;
	BOOL	bResult = FALSE;
	static  BOOL	bTraceInfo = TRUE;

	if (bTraceInfo)
	{
		WK_TRACE(_T("searching for NET dongle info %d Jacob %d Savic %d Tasha\n"),
			m_nNumbersOfJaCobs,m_nNumbersOfSaVics,m_nNumbersOfTashas);
	}

	if (m_nNumbersOfJaCobs > 0)
	{
		if (prof.GetInt(_T("JaCobUnit\\Device1\\EEProm"), _T("Net"), 0) == 0)
		{
			// Only trace once
			if (bTraceInfo)
			{
				WK_TRACE(_T("Network not allowed by JaCob\n"));
				bTraceInfo = FALSE;
			}
		}
		else
		{
			// Only trace once
			if (bTraceInfo)
			{
				WK_TRACE(_T("Network allowed by JaCob\n"));
				bTraceInfo = FALSE;
			}
		bResult = TRUE;
		}
	}
	else if (m_nNumbersOfSaVics > 0)
	{
		if (prof.GetInt(_T("SaVicUnit\\Device1\\EEProm"), _T("Net"), 0) == 0)
		{
			// Only trace once
			if (bTraceInfo)
			{
				WK_TRACE(_T("Network not allowed by SaVic\n"));
				bTraceInfo = FALSE;
			}
		}
		else
		{
			// Only trace once
			if (bTraceInfo)
			{
				WK_TRACE(_T("Network allowed by SaVic\n"));
				bTraceInfo = FALSE;
			}
			bResult = TRUE;
		}
	}
	else if (m_nNumbersOfTashas > 0)
	{
		if (prof.GetInt(_T("TashaUnit\\Device1\\EEProm"), _T("Net"), 0) == 0)
		{
			// Only trace once
			if (bTraceInfo)
			{
				WK_TRACE(_T("Network not allowed by Tasha\n"));
				bTraceInfo = FALSE;
			}
		}
		else
		{
			// Only trace once
			if (bTraceInfo)
			{
				WK_TRACE(_T("Network allowed by Tasha\n"));
				bTraceInfo = FALSE;
			}
			bResult = TRUE;
		}
	}
	else  if (m_nNumbersOfQs > 0)
	{
		if (prof.GetInt(_T("QUnit\\EEProm"), _T("Net"), 0) == 0)
		{
			// Only trace once
			if (bTraceInfo)
			{
				WK_TRACE(_T("Network not allowed by QUnit\n"));
				bTraceInfo = FALSE;
			}
		}
		else
		{
			// Only trace once
			if (bTraceInfo)
			{
				WK_TRACE(_T("Network allowed by QUnit\n"));
				bTraceInfo = FALSE;
			}
			bResult = TRUE;
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("No Compression board detectet or allowed...can't read dongle information\n"));
	}

	prof.WriteInt(DV_DVSTARTER, _T("Net"), bResult);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDVStarterDlg::IsISDNAllowed()
{
	CWK_Profile prof;
	BOOL	bResult = FALSE;
	static BOOL	bTraceInfo = TRUE;

/*	WK_TRACE(_T("searching for ISDN dongle info %d Jacob %d Savic %d Tasha\n"),
		m_nNumbersOfJaCobs,m_nNumbersOfSaVics,m_nNumbersOfTashas);*/
	if (m_nNumbersOfJaCobs > 0)
	{
		if (prof.GetInt(_T("JaCobUnit\\Device1\\EEProm"), _T("ISDN"), 0) == 0)
		{
			// Only trace once
			if (bTraceInfo)
			{
				WK_TRACE(_T("ISDN not allowed by JaCob\n"));
				bTraceInfo = FALSE;
			}
		}
		else
		{
			// Only trace once
			if (bTraceInfo)
			{
				WK_TRACE(_T("ISDN allowed by JaCob\n"));
				bTraceInfo = FALSE;
			}
			bResult = TRUE;
		}
	}
	else if (m_nNumbersOfSaVics > 0)
	{
		if (prof.GetInt(_T("SaVicUnit\\Device1\\EEProm"), _T("ISDN"), 0) == 0)
		{
			// Only trace once
			if (bTraceInfo)
			{
				WK_TRACE(_T("ISDN not allowed by SaVic\n"));
				bTraceInfo = FALSE;
			}
		}
		else
		{
			// Only trace once
			if (bTraceInfo)
			{
				WK_TRACE(_T("ISDN allowed by JaCob\n"));
				bTraceInfo = FALSE;
			}
			bResult = TRUE;
		}
	}
	else if (m_nNumbersOfTashas > 0)
	{
		if (prof.GetInt(_T("TashaUnit\\Device1\\EEProm"), _T("ISDN"), 0) == 0)
		{
			// Only trace once
			if (bTraceInfo)
			{
				WK_TRACE(_T("ISDN not allowed by Tasha\n"));
				bTraceInfo = FALSE;
			}
		}
		else
		{
			// Only trace once
			if (bTraceInfo)
			{
				WK_TRACE(_T("ISDN allowed by Tasha\n"));
				bTraceInfo = FALSE;
			}
			bResult = TRUE;
		}
	}
	else  if (m_nNumbersOfQs > 0)
	{
		if (prof.GetInt(_T("QUnit\\EEProm"), _T("ISDN"), 0) == 0)
		{
			// Only trace once
			if (bTraceInfo)
			{
				WK_TRACE(_T("ISDN not allowed by QUnit\n"));
				bTraceInfo = FALSE;
			}
		}
		else
		{
			// Only trace once
			if (bTraceInfo)
			{
				WK_TRACE(_T("ISDN allowed by QUnit\n"));
				bTraceInfo = FALSE;
			}
			bResult = TRUE;
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("No Compression board detectet or allowed...can't read dongle information\n"));
	}

	prof.WriteInt(DV_DVSTARTER, _T("ISDN"), bResult);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
CDVStarterDlg::~CDVStarterDlg()
{
	StopRTErrorThread();

	if (m_hPsApiDll)
	{
		if (AfxFreeLibrary(m_hPsApiDll))
			m_hPsApiDll = NULL;
		else
			WK_TRACE_ERROR(_T("PsApi.dll NOT unloaded\n"));
	}

	WK_DELETE(m_pnLastTemperatures);
	WK_DELETE(m_pbSMARTFailureReported);
}

/////////////////////////////////////////////////////////////////////////////
void CDVStarterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDVStarterDlg)
	DDX_Text(pDX, IDC_MEMORY_USED, m_sTotalUsedMemory);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDVStarterDlg, CDialog)
	//{{AFX_MSG_MAP(CDVStarterDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_MESSAGE(WM_USER_RTERROR_ARRIVED, OnRTErrorArrived)
	ON_MESSAGE(DVSTARTER_EXIT, OnExit)
	ON_MESSAGE(WM_SELFCHECK, OnConfirmSelfcheck)
	ON_MESSAGE(WM_CHECK_RTE, OnCheckRTE)
	ON_MESSAGE(WM_PIEZO_USE_CHANGED, OnPiezoUseChanged)
	ON_MESSAGE(WM_LANGUAGE_CHANGED, OnLanguageChanged)
	ON_WM_DESTROY()
	ON_WM_DEVICECHANGE()
	ON_BN_CLICKED(IDC_EXIT, OnBnClickedExit)
	ON_WM_QUERYENDSESSION()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CDVStarterDlg::OnBnClickedExit()
{
	WK_TRACE(_T("CDVStarterDlg::OnBnClickedExit\n"));

	if (!theApp.AmIShell())
		OnExit(0, 0);
}

/////////////////////////////////////////////////////////////////////////////
void CDVStarterDlg::OnCancel()
{
}

/////////////////////////////////////////////////////////////////////////////
void CDVStarterDlg::OnOk()
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDVStarterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// Add _T("About...") menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	//Wich backup program is installed? PacketCD or DirectCD ?
	int iBackupProgram = CheckBackupProgram();
	switch(iBackupProgram) 
	{
		case 1:  //PacketCD
			break;

		case 2:  //DirectCD Version 3.xx
			InitRegForDirectCD();
			MoveDirectCDFromRunToDVRT();
			break;

		case 3:  //DirectCD Version 5.xx
			WK_TRACE(_T("DirectCD Version 5 is installed but not yet ready. Backup impossible.\n"));
			//TODO TKR beide Funktionen noch mit Code füllen
			//InitRegForDirectCDVers5();
			//MoveDirectCDVers5FromRunToDVRT();
			break;
		default:
			WK_TRACE(_T("PacketCD or DirectCD is not installed.\n"));
			break;
	}

	if (theApp.IsReceiver())
	{
		// Ab ins Tray
		NOTIFYICONDATA tnd;

		tnd.cbSize		= sizeof(NOTIFYICONDATA);
		tnd.hWnd		= m_hWnd;
		tnd.uID			= 1;

		tnd.uFlags		= NIF_MESSAGE|NIF_ICON|NIF_TIP;
		tnd.uCallbackMessage = WM_USER;

		tnd.hIcon		= theApp.LoadIcon(IDR_MAINFRAME);;
		CString sTip	= _T("Starter");

		lstrcpyn(tnd.szTip, sTip, sTip.GetLength()+1);
		Shell_NotifyIcon(NIM_ADD, &tnd);		
	}
	else
	{
		// Alles starten was unter
		// HKEY_LOCAL_MACHINE\SOFTWARE\\MicroSoft\\Windows\\CurrentVersion\\Run eingetragen ist.
		if (theApp.AmIShell())
		{
			CRunService run;
			run.RunServices();
			ShowWindow(SW_HIDE);
		}
		else
		{
			ShowWindow(SW_SHOW);
		}
		if (theApp.UsePiezo())
		{
			m_Piezo.InitPiezo();
		}

		// Stand das System wegen Stromausfall?
		CheckIfPowerLossed();
	}

	SetTimer(1, m_nTimerIntervall, NULL);
	
	SetTimer(2, 1000, NULL); // Sekunden Takt
	
	QueryMemoryInformation();

	CheckHDTemperaturs();
	CheckSMARTparameters();

	CWK_Profile wkp;
	if (wkp.GetInt(DV_DVSTARTER, VCD_RUN_MODULE, -1)) //RunVideteCheckDisc = TRUE, d.h. CF System
	{
		//TKR
		//prüfen, ob vor dem aktuellen jetzigen start der software die HDD mit dem
		//page file ausgefallen war. In diesem Fall wurde auf der nächsten HDD, die jetzt diejenige
		//mit dem page file ist, die rterror.dat mit dem protokollierten HDD Ausfall gespeichert.
		//Diesen HDD Ausfall laden und in die "richtige" rterror.dat von C:\ schreiben.
		//Anschließend die rterror.dat, die nicht auf C:\ liegt, löschen
		//Beispiel: 
		//C:\ = CF
		//D:\ = CD/DVD
		//E:\ = erste HDD (incl page file)
		//F:\ = zweite HDD (reines Speicherlaufwerk)
		//Szenario: E:\ fällt aus, RTE wird auf F:\ abgelegt, da das auf C:\ nicht mehr klappt
		//Neustart: aus F:\ wird nun E:\ und mittels VideteCheckDisc werden alle Pfade und page file "umgebogen"
		//RTE mit dem HDD Ausfall (E:\) auslesen und als "echten" RTE ind die richtige rterror.dat eintragen
		//Zur Info:
		//Im Falle eines Ausfall von F:\ (anstatt E:\), wird die Software nicht neu gestartet, sondern
		//es wird anschließend nur noch auf E:\ gespeichert, sonst ändert sich nichts

		CWK_RunTimeErrors rtErrorHDDFailed;
		CString sFirstFixedDrive = theApp.GetNextAvailableFixedDrive();
		CString sHDDFailedDir = sFirstFixedDrive.Left(2)+_T("\\")+RTE_HDDFAILED_PATH;
		CString sHDDFailedFile = sHDDFailedDir+_T("\\")+RTE_FILENAME;
		
		if(    DoesFileExist(sHDDFailedFile)
			&& rtErrorHDDFailed.LoadFromFile(sHDDFailedFile))
		{
			CWK_RunTimeError* pRTE = rtErrorHDDFailed.GetAt(0);
			if(pRTE)
			{
				CWK_RunTimeError e(WAI_DV_STARTER, REL_ERROR, RTE_HARDDISK_FALURE, pRTE->GetErrorText());			
				e.Send();
				rtErrorHDDFailed.SafeDeleteAll();
				DeleteFile(sHDDFailedDir+_T("\\")+RTE_FILENAME);
				RemoveDirectory(sHDDFailedDir);
				/*Beep(2000, 200);
				Sleep(100);
				Beep(2000, 200);
				Sleep(100);
				Beep(2000, 200);
				*/
			}
		}
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CDVStarterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
/////////////////////////////////////////////////////////////////////////////
void CDVStarterDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
/////////////////////////////////////////////////////////////////////////////
HCURSOR CDVStarterDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
/////////////////////////////////////////////////////////////////////////////
void CDVStarterDlg::ReCheckISDNLAN()
{
	if (theApp.IsTransmitter())
	{
		if (   m_bIsNetworkInstalled
			&& !m_AppSocket.WasCreated()
			&& IsNetworkAllowed())
		{
			// recreate Socket
			m_AppSocket.Create(_T("SocketUnit.exe"), WAI_SOCKET_UNIT, 3);
			if (!m_AppSocket.IsRunning())
			{
				m_AppSocket.Load(SW_MINIMIZE, 5000);
			}
		}
		if (   m_bIsISDNInstalled
			&& !m_AppISDN1.WasCreated()
			&& IsISDNAllowed())
		{
			m_AppISDN1.Create(_T("ISDNUnit.exe"), WAI_ISDN_UNIT, 5);
			if (!m_AppISDN1.IsRunning())
			{
				m_AppISDN1.Load(SW_SHOWMINNOACTIVE, 5000);
			}
		}
		if (   m_bIsISDNInstalled
			&& !m_AppISDN2.WasCreated()
			&& IsISDNAllowed())
		{
			m_AppISDN2.Create(_T("ISDNUnit.exe"), WAI_ISDN_UNIT_2, 5);
			if (!m_AppISDN2.IsRunning())
			{
				m_AppISDN2.Load(SW_SHOWMINNOACTIVE, 5000);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVStarterDlg::OnTimer(UINT nIDEvent) 
{
	static dwLastTC = GetTickCount();

	BOOL bOK = TRUE;

	// Der erste Timer ist für längere Zeitintervall (>20Sekunden)
	if (nIDEvent == 1)
	{
		if (theApp.IsTransmitter())
		{
			// In der Registierung die aktuelle Uhrzeit sichern, um später festzustellen, ob und
			// wie lange das System (Wegen Stromausfall) stand.
			CWK_Profile wkp;
			CString sTi;
			CSystemTime st;
			st.GetLocalTime();
			sTi = st.ToString();
			wkp.WriteString(DV_DVSTARTER, _T("ALiveTime"),sTi);

			ReCheckISDNLAN();

			// Alle 15 Minuten die Festplattentemperatur aller IDE-Festplatten auslesen.
			static DWORD dwHDTempLastTC = WK_GetTickCount();

			if (WK_GetTimeSpan(dwHDTempLastTC) > 15 * 60 * 1000)
			{
				dwHDTempLastTC = WK_GetTickCount();
				CheckHDTemperaturs();
			}
			// Alle 60 Minuten den S.M.A.R.T-Status aller IDE-Festplatten checken.
			static DWORD dwSMARTLastTC = WK_GetTickCount();

			if (WK_GetTimeSpan(dwSMARTLastTC) > 60 * 60 * 1000)
			{
				dwSMARTLastTC = WK_GetTickCount();
				CheckSMARTparameters();
			}
		}
		else
		{
			static DWORD dwLastCheckDiskSpaceAndCleanUp = WK_GetTickCount();

			if (WK_GetTimeSpan(dwLastCheckDiskSpaceAndCleanUp) > 15 * 60 * 1000)
			{
				dwLastCheckDiskSpaceAndCleanUp = WK_GetTickCount();
				const CIPCDebugger *pDebugger = GetDebugger();
				if (pDebugger && pDebugger->m_pFile)
				{
					pDebugger->m_pFile->CheckDiskSpaceAndCleanUp();
				}
			}
		}

		CApplication*pApp;
		int i, nc = theApp.GetApplications().GetSize();
		for (i=0; i<nc; i++)
		{
			pApp = theApp.GetApplications().GetAtFast(i);
			if (pApp->WasCreated())
			{
				if (!pApp->IsRunning())
					bOK = pApp->Load(SW_MINIMIZE, INVALID_DWORD_VALUE) && bOK;
				else
					bOK = pApp->MakeSelfCheck() && bOK;
			}
		}

/*		
		/////////////////////////////////
		if (m_AppDVHook.WasCreated())
		{
			if (!m_AppDVHook.IsRunning())
				bOK = m_AppDVHook.Load(SW_MINIMIZE, 10000) && bOK;
			else
				bOK = m_AppDVHook.MakeSelfCheck() && bOK;
		}

		/////////////////////////////////
		if (m_AppJaCob1.WasCreated())
		{
			if (!m_AppJaCob1.IsRunning())
				bOK = m_AppJaCob1.Load(SW_MINIMIZE, 10000) && bOK;
			else
				bOK = m_AppJaCob1.MakeSelfCheck() && bOK;
		}

		/////////////////////////////////
		if (m_AppJaCob2.WasCreated())
		{
			if (!m_AppJaCob2.IsRunning())
				bOK = m_AppJaCob2.Load(SW_MINIMIZE, 10000)  && bOK;
			else
				bOK = m_AppJaCob2.MakeSelfCheck() && bOK;
		}

		/////////////////////////////////
		if (m_AppJaCob3.WasCreated())
		{
			if (!m_AppJaCob3.IsRunning())
				bOK = m_AppJaCob3.Load(SW_MINIMIZE, 10000) && bOK;
			else
				bOK = m_AppJaCob3.MakeSelfCheck() && bOK;
		}

		/////////////////////////////////
		if (m_AppJaCob4.WasCreated())
		{
			if (!m_AppJaCob4.IsRunning())
				bOK = m_AppJaCob4.Load(SW_MINIMIZE, 10000)  && bOK;
			else
				bOK = m_AppJaCob4.MakeSelfCheck() && bOK;
		}

		/////////////////////////////////
		if (m_AppSaVic1.WasCreated())
		{
			if (!m_AppSaVic1.IsRunning())
				bOK = m_AppSaVic1.Load(SW_MINIMIZE, 10000) && bOK;
			else
				bOK = m_AppSaVic1.MakeSelfCheck() && bOK;
		}

		/////////////////////////////////
		if (m_AppSaVic2.WasCreated())
		{
			if (!m_AppSaVic2.IsRunning())
				bOK = m_AppSaVic2.Load(SW_MINIMIZE, 10000)  && bOK;
			else
				bOK = m_AppSaVic2.MakeSelfCheck() && bOK;
		}

			/////////////////////////////////
		if (m_AppSaVic3.WasCreated())
		{
			if (!m_AppSaVic3.IsRunning())
				bOK = m_AppSaVic3.Load(SW_MINIMIZE, 10000) && bOK;
			else
				bOK = m_AppSaVic3.MakeSelfCheck() && bOK;
		}

		/////////////////////////////////
		if (m_AppSaVic4.WasCreated())
		{
			if (!m_AppSaVic4.IsRunning())
				bOK = m_AppSaVic4.Load(SW_MINIMIZE, 10000)  && bOK;
			else
				bOK = m_AppSaVic4.MakeSelfCheck() && bOK;
		}

		/////////////////////////////////
		if (m_AppTasha1.WasCreated())
		{
			if (!m_AppTasha1.IsRunning())
				bOK = m_AppTasha1.Load(SW_MINIMIZE, 10000) && bOK;
			else
				bOK = m_AppTasha1.MakeSelfCheck() && bOK;
		}

		/////////////////////////////////
		if (m_AppTasha2.WasCreated())
		{
			if (!m_AppTasha2.IsRunning())
				bOK = m_AppTasha2.Load(SW_MINIMIZE, 10000)  && bOK;
			else
				bOK = m_AppTasha2.MakeSelfCheck() && bOK;
		}

			/////////////////////////////////
		if (m_AppTasha3.WasCreated())
		{
			if (!m_AppTasha3.IsRunning())
				bOK = m_AppTasha3.Load(SW_MINIMIZE, 10000) && bOK;
			else
				bOK = m_AppTasha3.MakeSelfCheck() && bOK;
		}

		/////////////////////////////////
		if (m_AppTasha4.WasCreated())
		{
			if (!m_AppTasha4.IsRunning())
				bOK = m_AppTasha4.Load(SW_MINIMIZE, 10000)  && bOK;
			else
				bOK = m_AppTasha4.MakeSelfCheck() && bOK;
		}

		/////////////////////////////////
		if (m_AppQ.WasCreated())
		{
			if (!m_AppQ.IsRunning())
				bOK = m_AppQ.Load(SW_MINIMIZE, 10000)  && bOK;
			else
				bOK = m_AppQ.MakeSelfCheck() && bOK;
		}

		/////////////////////////////////
		if (m_AppDVProcess.WasCreated())
		{
			if (!m_AppDVProcess.IsRunning())
				bOK = m_AppDVProcess.Load(SW_MINIMIZE, 5000) && bOK;
			else
				bOK = m_AppDVProcess.MakeSelfCheck() && bOK;
		}

		/////////////////////////////////
		if (m_AppDVStorage.WasCreated())
		{
			if (!m_AppDVStorage.IsRunning())
				bOK = m_AppDVStorage.Load(SW_MINIMIZE, 5000) && bOK;
			else
				bOK = m_AppDVStorage.MakeSelfCheck() && bOK;
		}

		/////////////////////////////////
		if (m_AppSocket.WasCreated())
		{
			if (!m_AppSocket.IsRunning())
				bOK = m_AppSocket.Load(SW_MINIMIZE, 5000) && bOK;
			else
				bOK = m_AppSocket.MakeSelfCheck() && bOK;
		}

		/////////////////////////////////
		if (m_AppISDN1.WasCreated())
		{
			if (!m_AppISDN1.IsRunning())
				bOK = m_AppISDN1.Load(SW_SHOWMINNOACTIVE,5000) && bOK;
			else
				bOK = m_AppISDN1.MakeSelfCheck() && bOK;
		}

		/////////////////////////////////
		if (m_AppISDN2.WasCreated())
		{
			if (!m_AppISDN2.IsRunning())
				bOK = m_AppISDN2.Load(SW_SHOWMINNOACTIVE,5000) && bOK;
			else
				bOK = m_AppISDN2.MakeSelfCheck() && bOK;
		}

		/////////////////////////////////
		if (m_AppDVClient.WasCreated())
		{
			if (!m_AppDVClient.IsRunning())
				bOK = m_AppDVClient.Load(SW_SHOWMAXIMIZED, 5000) && bOK;
			else
				bOK = m_AppDVClient.MakeSelfCheck() && bOK;
		}

		/////////////////////////////////
		if (m_AppACDC.WasCreated())
		{
			if (!m_AppACDC.IsRunning())
				bOK = m_AppACDC.Load(SW_SHOWMINIMIZED, 5000) && bOK;
			else
				bOK = m_AppACDC.MakeSelfCheck() && bOK;
		}

		/////////////////////////////////
		if (m_AppCommUnit.WasCreated())
		{
			if (!m_AppCommUnit.IsRunning())
				bOK = m_AppCommUnit.Load(SW_SHOWMINIMIZED, 5000) && bOK;
			else
				bOK = m_AppCommUnit.MakeSelfCheck() && bOK;
		}

		/////////////////////////////////
		if (m_AppAudioUnit.WasCreated())
		{
			if (!m_AppAudioUnit.IsRunning())
				bOK = m_AppAudioUnit.Load(SW_SHOWMINIMIZED, 5000) && bOK;
			else
				bOK = m_AppAudioUnit.MakeSelfCheck() && bOK;
		}
		/////////////////////////////////
		if (m_AppHealthControl.WasCreated())
		{
			if (!m_AppHealthControl.IsRunning())
				bOK = m_AppHealthControl.Load(SW_SHOWMINIMIZED, 5000) && bOK;
			else
				bOK = m_AppHealthControl.MakeSelfCheck() && bOK;
		}
*/
		m_WatchDog.Trigger(3*m_nTimerIntervall/1000);

		if (bOK)
		{
			m_Piezo.PiezoClearFlag(PF_APP_NOT_RUNNING);
		}
		else
		{
			m_Piezo.PiezoSetFlag(PF_APP_NOT_RUNNING);
		}
		
		if (WK_GetTimeSpan(dwLastTC) > 15000)
		{
			dwLastTC = GetTickCount();
			PostMessage(WM_CHECK_RTE);
		}

		QueryMemoryInformation();

		CTime StartTime;									 
		CTime CurrentTime = CTime::GetCurrentTime();

		CWK_Profile prof;

		// Zeitpunkt der nächsten Registrierungssicherung lesen.
		CString sTime = prof.GetString(_T("DV\\DVClient"), _T("NextRDiskTime"), _T(""));
		if (sTime.IsEmpty())
		{
			// Kein Zeitpunkt in der Registrierung eingetragen. Neuen Eintrag erstellen
			StartTime = CTime(CurrentTime.GetYear(), CurrentTime.GetMonth(), CurrentTime.GetDay(),
							CurrentTime.GetHour(), CurrentTime.GetMinute(),CurrentTime.GetSecond());
			
			// 1.5 Tage vor. Dadurch ist gewährleistet, das die nächste Sicherung erst in 12...24 Stunden erfolgt
			CTimeSpan span(1, 12, 0, 0);	
			StartTime += span;
			sTime.Format(_T("%02u.%02u.%04u/%02u:%02u:%02u"), StartTime.GetDay(), StartTime.GetMonth(), StartTime.GetYear(),0, 0, 0);
			prof.WriteString(_T("DV\\DVClient"), _T("NextRDiskTime"), sTime);
		}
		else
		{
			// Zeitpunkt der Registrierungssicherung formatieren
			int nDay, nMonth, nYear, nHour, nMinute, nSecond;
			_stscanf(sTime, _T("%d.%d.%d/%d:%d:%d"), &nDay, &nMonth, &nYear, &nHour, &nMinute, &nSecond); 
			StartTime = CTime(nYear, nMonth, nDay, nHour, nMinute, nSecond);
		}

		// Nächste Sicherung fällig?
		if (CurrentTime >= StartTime)
		{
			// Kein Zeitpunkt in der Registrierung eingetragen. Neuen Eintrag erstellen
			StartTime = CTime(CurrentTime.GetYear(), CurrentTime.GetMonth(), CurrentTime.GetDay(),
							CurrentTime.GetHour(), CurrentTime.GetMinute(),CurrentTime.GetSecond());

			// 1.5 Tage vor. Dadurch ist gewährleistet, das die nächste Sicherung erst in 12...24 Stunden erfolgt
			CTimeSpan span(1, 12, 0, 0);	
			StartTime += span;
			sTime.Format(_T("%02u.%02u.%04u/%02u:%02u:%02u"), StartTime.GetDay(), StartTime.GetMonth(), StartTime.GetYear(),0, 0, 0);
			prof.WriteString(_T("DV\\DVClient"), _T("NextRDiskTime"), sTime);

			// RDisk.exe wird nur mit WinNT4 ausgeliefert.
			if (theApp.IsNT40())
			{
				theApp.Rdisk(FALSE);
			}
		}
	}	// nIDEvent = 1

	// Der zeite Timer ist für kürzere Zeitintervall (>1Sekunde)
	if (nIDEvent == 2)
	{
		// TODO
	}

	CDialog::OnTimer(nIDEvent);

}

/////////////////////////////////////////////////////////////////////////////
void CDVStarterDlg::StartRTErrorThread()
{
	m_bRun = TRUE;

	m_pRTErrorThread = AfxBeginThread(RTErrorThreadProc,NULL);
	m_pRTErrorThread->m_bAutoDelete = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
void CDVStarterDlg::StopRTErrorThread()
{
	m_bRun = FALSE;

	if (m_pRTErrorThread)
	{
		WaitForSingleObject(m_pRTErrorThread->m_hThread,2000);
		WK_DELETE(m_pRTErrorThread);
	}
}

/////////////////////////////////////////////////////////////////////////////
UINT CDVStarterDlg::RTErrorThreadProc(LPVOID lpParam)
{
	CWK_RunTimeError RunTimeError;
	while (m_pThis->m_bRun)
	{
		if (RunTimeError.ReadGlobalError())
		{
			if (m_pThis->m_hWnd)
			{
				m_pThis->PostMessage(WM_USER_RTERROR_ARRIVED, 0, (LPARAM)new CWK_RunTimeError(RunTimeError));
			}
			RunTimeError.ConfirmGlobalError();
		}
	}

	return 0x0E;
}

/////////////////////////////////////////////////////////////////////////////
long CDVStarterDlg::OnRTErrorArrived(WPARAM wParam, LPARAM lParam)
{
	CWK_RunTimeError* pLastError = (CWK_RunTimeError*)lParam;

	if (!pLastError)
		return -1;

	BOOL bAcceptRTE = FALSE;
	int nCamOffset = 0;

	// Nur die folgenden RTE werden berücksichtigt.
	switch(pLastError->GetError())
	{
		case RTE_CAMERA_MISSING:
		{
			// Ermittel die tatsächliche Kameranummer aus UNIT und übergebener Kameranummer.
			// Sofern die Units irgendwann einmal eine andere Kameraanzahl besitzen, ist
			// der Offset natürlich anzupassen.
			switch (pLastError->GetApplicationId())
			{
				case WAI_JACOBUNIT_1:
					nCamOffset = 0;
					break;
				case WAI_JACOBUNIT_2:
					nCamOffset = 8;
					break;
				case WAI_JACOBUNIT_3:
					nCamOffset = 16;
					break;
				case WAI_JACOBUNIT_4:
					nCamOffset = 24;
					break;
				case WAI_SAVICUNIT_1:
					nCamOffset = 0;
					break;
				case WAI_SAVICUNIT_2:
					nCamOffset = 4;
					break;
				case WAI_SAVICUNIT_3:
					nCamOffset = 8;
					break;
				case WAI_SAVICUNIT_4:
					nCamOffset = 12;
					break;
				case WAI_TASHAUNIT_1:
					nCamOffset = 0;
					break;
				case WAI_TASHAUNIT_2:
					nCamOffset = 8;
					break;
				case WAI_TASHAUNIT_3:
					nCamOffset = 16;
					break;
				case WAI_TASHAUNIT_4:
					nCamOffset = 24;
					break;
				default:
					nCamOffset = 0;
				}
			}
			if (nCamOffset > 0)
			{
				CString sCam			= _T("");
				CString sNewErrorText	= _T("");
				CString sOldErrorText	= pLastError->GetErrorText();
				sCam.Format(_T("%d"), pLastError->GetParam1());
				int nI = sOldErrorText.Find(sCam, 0);
				if (nI != -1)
				{
					sNewErrorText.Format(_T("%s %d %s"), sOldErrorText.Left(nI),
														pLastError->GetParam1()+nCamOffset,
														sOldErrorText.Mid(nI+sCam.GetLength()));

					pLastError->SetErrorText(sNewErrorText);
				}
			}
			bAcceptRTE = TRUE;
			break;
		// RTE that may be on or off
		case RTE_ARCHIVE_FULL:		// archive is full
		case RTE_ARCHIVE_60PERCENT:	// archive is filled 60%
		case RTE_TEMPERATURE:		// device temperature too high
		case RTE_FANCONTROL:		// fan speed too low
			if (pLastError->GetParam2() & 1)
				bAcceptRTE = TRUE;	// only accept if switched on
			break;
		// RTE just for info
		case RTE_LC_AC_FAIL:		// power loss
		case RTE_INCOMING_CALL:		// incomming call
		case RTE_REJECT_ALARM_CONNECTION:// alarm rejected
			bAcceptRTE = TRUE;
			break;
		// RTE with alert status (warnings or errors)
		case RTE_SMART:				// HDD smart error
		case RTE_HARDWARE:			// Hardware device failure
		case RTE_CONFIGURATION:		// wrong configuration for storage
		case RTE_STORAGE:			// could not store data
		case RTE_EXTERNAL_DEVICE_FAILURE:// failure in ext device
		case RTE_COM_OPEN:			// failed to open com port
//		case RTE_VOLTAGE:			// Voltage out of range
			bAcceptRTE = TRUE;
			break;
		// RTE where reboot may be nessesary
		case RTE_EWF_RAM_OVERFLOW:	// too low EWF ram
		case RTE_HARDDISK_FALURE:	// HDD failure (with page file will need reboot)
			if (pLastError->GetLevel() == REL_REBOOT_ERROR)
			{
				theApp.Reboot();
			}
			bAcceptRTE = TRUE;
			break;
		default:
			bAcceptRTE = FALSE;
			break;
	}

	if (bAcceptRTE)
	{
		CRTErrorDlg *pRTErrorDlg = new CRTErrorDlg(pLastError);
		if (pRTErrorDlg)
			pRTErrorDlg->Create();
	}
	else
		WK_DELETE(pLastError);

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDVStarterDlg::OnExit(WPARAM wParam, LPARAM lParam)
{
	WK_TRACE(_T("CDVStarterDlg::OnExit\n"));
	
	// Das Herrunterfahren soll nicht verhindert werden.
	m_bQueryEndSessionReturn = TRUE;

	m_AppDVClient.SetAutoTerminate(TRUE);
	CDialog::OnCancel();
	return 0;
}
LRESULT CDVStarterDlg::OnLanguageChanged(WPARAM wParam, LPARAM lParam)
{
	WK_TRACE(_T("CDVStarterDlg::OnLanguageChanged(%d, %x)\n"), wParam, lParam);
	CApplication*pApp;
	int i, nc = theApp.GetApplications().GetSize();
	for (i=0; i<nc; i++)
	{
		pApp = theApp.GetApplications().GetAtFast(i);
		pApp->PostMessage(WM_LANGUAGE_CHANGED, wParam, lParam);
	}

#if _MFC_VER >= 0x0710
	theApp.SetLanguageParameters((UINT)wParam, (DWORD)lParam);
#endif

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDVStarterDlg::OnPiezoUseChanged(WPARAM wParam, LPARAM lParam)
{
	theApp.OnPiezoUseChanged();
	if (theApp.UsePiezo())
	{
		WK_TRACE(_T("system integrity contact on\n"));
		m_Piezo.InitPiezo();
	}
	else
	{
		WK_TRACE(_T("system integrity contact off\n"));
		m_Piezo.ExitPiezo();
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDVStarterDlg::OnConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	WK_ApplicationId AppId = (WK_ApplicationId)wParam;

	switch (AppId)
	{
		case WAI_JACOBUNIT_1:
			m_AppJaCob1.ConfirmSelfcheckMessage();
			break;
		case WAI_JACOBUNIT_2:
			m_AppJaCob2.ConfirmSelfcheckMessage();
			break;
		case WAI_JACOBUNIT_3:
			m_AppJaCob3.ConfirmSelfcheckMessage();
			break;
		case WAI_JACOBUNIT_4:
			m_AppJaCob4.ConfirmSelfcheckMessage();
			break;
		case WAI_SAVICUNIT_1:
			m_AppSaVic1.ConfirmSelfcheckMessage();
			break;
		case WAI_SAVICUNIT_2:
			m_AppSaVic2.ConfirmSelfcheckMessage();
			break;
		case WAI_SAVICUNIT_3:
			m_AppSaVic3.ConfirmSelfcheckMessage();
			break;
		case WAI_SAVICUNIT_4:
			m_AppSaVic4.ConfirmSelfcheckMessage();
			break;
		case WAI_TASHAUNIT_1:
			m_AppTasha1.ConfirmSelfcheckMessage();
			break;
		case WAI_TASHAUNIT_2:
			m_AppTasha2.ConfirmSelfcheckMessage();
			break;
		case WAI_TASHAUNIT_3:
			m_AppTasha3.ConfirmSelfcheckMessage();
			break;
		case WAI_TASHAUNIT_4:
			m_AppTasha4.ConfirmSelfcheckMessage();
			break;
		case WAI_QUNIT:
			m_AppQ.ConfirmSelfcheckMessage();
			break;
		case WAI_DATABASE_SERVER:
			m_AppDVStorage.ConfirmSelfcheckMessage();
			break;
		case WAI_SECURITY_SERVER:
			m_AppDVProcess.ConfirmSelfcheckMessage();
			break;
		case WAI_DV_CLIENT:
			m_AppDVClient.ConfirmSelfcheckMessage();
			break;
		case WAI_SOCKET_UNIT:
			m_AppSocket.ConfirmSelfcheckMessage();
			break;
		case WAI_ISDN_UNIT:
			m_AppISDN1.ConfirmSelfcheckMessage();
			break;
		case WAI_ISDN_UNIT_2:
			m_AppISDN2.ConfirmSelfcheckMessage();
			break;
		case WAI_DV_HOOK:
			m_AppDVHook.ConfirmSelfcheckMessage();
			break;
		case WAI_AC_DC:
			m_AppACDC.ConfirmSelfcheckMessage();
			break;
		case WAI_COMMUNIT:
			m_AppCommUnit.ConfirmSelfcheckMessage();
			break;
		case WAI_AUDIOUNIT_1:
			m_AppAudioUnit.ConfirmSelfcheckMessage();
			break;
		case WAI_HEALTH_CONTROL:
			m_AppHealthControl.ConfirmSelfcheckMessage();
			break;
		default:
			WK_TRACE_WARNING(_T("application not found for WM_SELFCHECK %d\n"),wParam);
			break;
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// DoModal muß überschrieben werden, da normalerweise Modale Dialoge nicht mit
// ShowWindow(SW_HIDE) unsichtbar gemacht werden können.
#include <Afxpriv.h>
#include ".\dvstarterdlg.h"
#define DELETE_EXCEPTION(e) do { e->Delete(); } while (0)
int CDVStarterDlg::DoModal() 
{
	// can be constructed with a resource template or InitModalIndirect
	ASSERT(m_lpszTemplateName != NULL || m_hDialogTemplate != NULL ||
		m_lpDialogTemplate != NULL);

	// load resource as necessary
	LPCDLGTEMPLATE lpDialogTemplate = m_lpDialogTemplate;
	HGLOBAL hDialogTemplate = m_hDialogTemplate;
	HINSTANCE hInst = AfxGetResourceHandle();
	if (m_lpszTemplateName != NULL)
	{
		hInst = AfxFindResourceHandle(m_lpszTemplateName, RT_DIALOG);
		HRSRC hResource = ::FindResource(hInst, m_lpszTemplateName, RT_DIALOG);
		hDialogTemplate = LoadResource(hInst, hResource);
	}
	if (hDialogTemplate != NULL)
		lpDialogTemplate = (LPCDLGTEMPLATE)LockResource(hDialogTemplate);

	// return -1 in case of failure to load the dialog template resource
	if (lpDialogTemplate == NULL)
		return -1;

	// disable parent (before creating dialog)
	HWND hWndParent = PreModal();
	AfxUnhookWindowCreate();
	BOOL bEnableParent = FALSE;
	if (hWndParent != NULL && ::IsWindowEnabled(hWndParent))
	{
		::EnableWindow(hWndParent, FALSE);
		bEnableParent = TRUE;
	}

	TRY
	{
		// create modeless dialog
		AfxHookWindowCreate(this);
		if (CreateDlgIndirect(lpDialogTemplate,
						CWnd::FromHandle(hWndParent), hInst))
		{
			if (m_nFlags & WF_CONTINUEMODAL)
			{
				// enter modal loop
				DWORD dwFlags = 0; //ML 01.03.00 MLF_SHOWONIDLE;
				if (GetStyle() & DS_NOIDLEMSG)
					dwFlags |= MLF_NOIDLEMSG;
				VERIFY(RunModalLoop(dwFlags) == m_nModalResult);
			}

			// hide the window before enabling the parent, etc.
			if (m_hWnd != NULL)
				SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW|
					SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);
		}
	}
	CATCH_ALL(e)
	{
		e->Delete();
		m_nModalResult = -1;
	}
	END_CATCH_ALL

	if (bEnableParent)
		::EnableWindow(hWndParent, TRUE);
	if (hWndParent != NULL && ::GetActiveWindow() == m_hWnd)
		::SetActiveWindow(hWndParent);

	// destroy modal window
	DestroyWindow();
	PostModal();

	// unlock/free resources as necessary
	if (m_lpszTemplateName != NULL || m_hDialogTemplate != NULL)
		UnlockResource(hDialogTemplate);
	if (m_lpszTemplateName != NULL)
		FreeResource(hDialogTemplate);

	return m_nModalResult;
}

/////////////////////////////////////////////////////////////////////////////
void CDVStarterDlg::OnDestroy() 
{								    
	if (theApp.IsTransmitter())
	{
		// System wurde ordnungsgemäß beendet. Daher Shutdowntime und Alivetime
		// auf die aktuelle Zeit setzen
		CSystemTime st;
		CString sTi;
		CWK_Profile wkp;
																		  
		st.GetLocalTime();
		sTi = st.ToString();
		wkp.WriteString(DV_DVSTARTER, _T("ShutDownTime"),sTi);
		wkp.WriteString(DV_DVSTARTER, _T("ALiveTime"),sTi);
	}

 	KillTimer(1);

	// Nur beim Receiver erscheint das Icon im SystemTray
	if (theApp.IsReceiver())
	{
		// Nur erforderlich, wenn Icon in der Systemtaskleiste
		NOTIFYICONDATA tnd;

		tnd.cbSize		= sizeof(NOTIFYICONDATA);
		tnd.hWnd		= m_hWnd;
		tnd.uID			= 1;
		tnd.uFlags		= 0;

		Shell_NotifyIcon(NIM_DELETE, &tnd);
	}


	CDialog::OnDestroy();
}

/////////////////////////////////////////////////////////////////////////////
int CDVStarterDlg::CheckDCDVersion(CWK_Profile* pwkProf)
{
	int iRet = 0;

	int iDefault = -1;
	CString sStartup = _T("StartupBanner");

	if(pwkProf->GetInt(_T("3.0"), sStartup, iDefault) == iDefault)
	{
		//Section _T("3.0") doesn`t exists
		pwkProf->DeleteSubSection(_T("3.0"), _T(""));
		if(pwkProf->GetInt(_T("2.6"), sStartup, iDefault) == iDefault)
		{
			//Section _T("2.6") doesn`t exists
			pwkProf->DeleteSubSection(_T("2.6"), _T(""));
		}
		else
		{
			//actual version of DirectCD is version 2.6
			iRet = DCD_VERSION_26;
		}
	}
	else
	{
		//actual version of DirectCD is version 3.0
		iRet = DCD_VERSION_30;			
	}

	return iRet;
}

/////////////////////////////////////////////////////////////////////////////
int CDVStarterDlg::CheckBackupProgram()
{
	int iRet = 0;

	CWK_Profile wkProfRun(CWK_Profile::WKP_NTLOGON, NULL, _T("MicroSoft\\Windows\\CurrentVersion"),_T(""));

	CString sDefault(_T("default"));
	CString sPacketCD = wkProfRun.GetString(_T("Run"), _T("PacketCD"), sDefault);
	//wurde DirectCD noch nicht aus Run-Pfad gelöscht, prüfe hier
	CString sDirectCDRun = wkProfRun.GetString(_T("Run"), _T("Adaptec DirectCD"), sDefault);
	CString sDirectCD5Run = wkProfRun.GetString(_T("Run"), _T("AdaptecDirectCD"), sDefault);


	if(sPacketCD == sDefault)
	{
		wkProfRun.DeleteSubSection(_T("PacketCD"), _T(""));
	}
	else
	{
		iRet = 1;
	}

	if(sDirectCDRun == sDefault)
	{
		CWK_Profile wkProfDV;
		wkProfDV.DeleteSubSection(_T("Adaptec DirectCD"), _T(""));

		//wurde DirectCD schon aus Run-Pfad gelöscht, prüfe hier, 
		//da Inhalt hierher kopiert wurde
		CString sDirectCDDVClient = wkProfDV.GetString(_T("DV\\DVClient"), _T("DirectCDPath"), sDefault);
		if(sDirectCDDVClient == sDefault)
		{
			HKEY hKey = NULL;
			LONG res;
			CString sSection = _T("Software\\DVRT\\DV\\DVClient");
			res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, sSection, 0, KEY_WRITE|KEY_READ, &hKey);
			if (res == ERROR_SUCCESS)
			{
				RegDeleteValue(hKey, _T("DirectCDPath"));
			}
			RegCloseKey(hKey);
		}
		else
		{
			iRet = 2;
		}
	}
	else
	{
		iRet = 2;
	}

	if(sDirectCD5Run == sDefault)
	{	
		//wurde DirectCD5 schon aus Run-Pfad gelöscht, prüfe hier, 
		//da Inhalt hierher kopiert wurde
		CWK_Profile wkProfDV;
		CString sDirectCDDVClient = wkProfDV.GetString(_T("DV\\DVClient"), _T("DirectCD5Path"), sDefault);
		if(sDirectCDDVClient == sDefault)
		{
			HKEY hKey = NULL;
			LONG res;
			CString sSection = _T("Software\\DVRT\\DV\\DVClient");
			res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, sSection, 0, KEY_WRITE|KEY_READ, &hKey);
			if (res == ERROR_SUCCESS)
			{
				RegDeleteValue(hKey, _T("DirectCD5Path"));
			}
			RegCloseKey(hKey);
		}
		else
		{
			iRet = 3;
		}
	}
	else
	{
		iRet = 3;
	}
	return iRet;

}


/////////////////////////////////////////////////////////////////////////////
void CDVStarterDlg::InitRegForDirectCD()
{
	//Registry Eintrag von DirectCD initialisieren, damit nur die Dialoge 
	//hochpoppen, die gwünscht sind. Diese Einstellungen können unter den
	//Eigenschaften des Brenner-Laufwerkes (Properties) nachgesehen werden
	CWK_Profile wkProf(CWK_Profile::WKP_NTLOGON, NULL, _T("Adaptec\\DirectCD"),_T(""));
	CWK_Profile wkProf305(CWK_Profile::WKP_REGPATH, HKEY_CURRENT_USER,_T("Adaptec\\DirectCD"),_T("")); //DirectCD 3.05
	CWK_Profile* pwkProf;
	pwkProf = &wkProf;
	int iVersion = 0;
	iVersion = CheckDCDVersion(pwkProf);
	
	switch (iVersion)
	{

	case 0:
		//no DirectCD installed on this computer
		WK_TRACE(_T("no DirectCD is installed.\n"));
		break;


	case DCD_VERSION_26:
		{
		WK_TRACE(_T("DirectCD Version 2.6 is installed\n."));
		wkProf.WriteInt(_T("2.6"), _T("StartupBanner"), 0);

		int iDefaultValue = 0;
		int iKeyValue = 0;
		CString sDrive;
		char cValue = 0;
		
		//try with every drive from _T('D') to _T('Z') if there is a key with that drivename
		//if there is a key, that`s the cd-rom drive
		//set new value to initialize DirectCD`s checkboxes
		for(int i = 0; i <= 22; i++)
		{
			cValue = (char)(i + 68);
			sDrive = cValue;
			sDrive = sDrive + _T(":\\");
			iKeyValue = wkProf.GetInt(_T("2.6"), sDrive, iDefaultValue);
			if(iKeyValue != 0)
			{
				break;
			}
		}

		CIPCDrive drive(cValue, wkProf);
		FS_Producer Producer = (FS_Producer)drive.GetFSProducer();
		WK_TRACE(_T("FSProducer: %i\n"), Producer);
		if (Producer == FSP_UDF_ADAPTEC_DIRECTCD)
		{
			wkProf.WriteInt(_T("2.6"), sDrive, 272);
		}

		}
		break;

	case DCD_VERSION_30:
		{
		WK_TRACE(_T("DirectCD Version 3.0 is installed\n."));

		int iDefaultValue = 0;
		int iKeyValue = 0;
		int iKeyValue305 = 0;  //DirectCD 3.05
		CString sDrive;
		char cValue = 0;
		
		//try with every drive from _T('D') to _T('Z') if there is a key with that drivename
		//if there is a key, that`s the cd-rom drive
		//set new value to initialize DirectCD`s checkboxes
		for(int i = 0; i <= 22; i++)
		{
			cValue = (char)(i + 68);
			sDrive = cValue;
			sDrive = sDrive + _T(":\\");
			iKeyValue		= wkProf.GetInt(_T("2.6"), sDrive, iDefaultValue);
			iKeyValue305	= wkProf305.GetInt(_T("2.6"), sDrive, iDefaultValue);  //DirectCD 3.05
			if(iKeyValue != 0)
			{
				//DirectCD Version früher als 3.05
				break;
			}
			if(iKeyValue305 != 0)
			{
				//DirectCD Version ist 3.05
				break;
			}
		}

		
		if(iKeyValue305 == 0)
		{
			CIPCDrive drive(cValue, wkProf305);
			FS_Producer Producer = (FS_Producer)drive.GetFSProducer();
			WK_TRACE(_T("FSProducer: %i\n"), Producer);
			if (Producer == FSP_UDF_ADAPTEC_DIRECTCD)
			{
				wkProf.WriteInt(_T("2.6"), sDrive, 16);
				wkProf.WriteInt(_T("2.6"), _T("StartupBanner"), 0);
				WK_TRACE(_T("DirectCD Version < 3.05\n"));
			}
		}
		else
		{
			CIPCDrive drive(cValue, wkProf);
			FS_Producer Producer = (FS_Producer)drive.GetFSProducer();
			WK_TRACE(_T("FSProducer: %i\n"), Producer);
			if (Producer == FSP_UDF_ADAPTEC_DIRECTCD)
			{
				wkProf305.WriteInt(_T("2.6"), sDrive, 16);
				wkProf305.WriteInt(_T("2.6"), _T("StartupBanner"), 0);
				WK_TRACE(_T("DirectCD Version = 3.05\n"));
			}
		}

		//fügt Schlüssel CDCapacity ein. Dieser wird für das Backup auf CD-R benötigt und ist
		//mit 600 (mb) vorbelegt
		
		CWK_Profile wkProfCDCapacity;
		iKeyValue = wkProfCDCapacity.GetInt(_T("DV\\DVClient"), _T("CDCapacity"), iDefaultValue);
		if(iKeyValue == iDefaultValue)
		{
			wkProfCDCapacity.WriteInt(_T("DV\\DVClient"),_T("CDCapacity"),600);
		}

		}
		break;


	default:
		WK_TRACE_ERROR(_T("DVStarter: Normally non reachable case!"));
		break;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVStarterDlg::InitRegForDirectCDVers5()
{
//TODO TKR Reg Einträge initialisieren, nachdem erforscht ist, welche Einträge das sind
//Pfad in Registry für Einstellungen:
	//HKEY_CURRENT_USER\Software\Adaptec\DirectCD\5.0\Settings\
	//Schlüssel:
	//Disc eject options = 1
	//Enable standard CD-ROM compatibility = 1
	//Show CD Ready Alert = 1
	//Show Ejected CD Dialog = 1
	//Show Icon in System Tray = 1

}
/////////////////////////////////////////////////////////////////////////////
void CDVStarterDlg::MoveDirectCDFromRunToDVRT()
{

	//Verschiebe DirectCD aus dem AutoRun-Verzeichnis in der Registry nach
	//dvrt/dv/directcd, da DirectCD nicht beim booten gestartet werden soll, sondern
	//immer nur dann, wenn es zum Backup gebraucht wird. Es kann nämlich passieren,
	//dass DirectCD eine in den CD-writer eingelegte CD-R nicht wieder freigibt, falls
	//von dieser ein Programm (z.B. DVStorage bei Backup CD`s) ausgeführt wird.

	CWK_Profile wkProfOld(CWK_Profile::WKP_NTLOGON, NULL, _T("Microsoft\\Windows\\CurrentVersion"),_T(""));
	CWK_Profile wkProfNew;
	CString sValue = _T("Adaptec DirectCD");
	CString sDefault(_T("default"));
	CString sDirectCDPath		= wkProfOld.GetString(_T("Run"), sValue, sDefault);

	if(sDirectCDPath != sDefault)
	{
		wkProfNew.WriteString(_T("DV\\DVClient"), _T("DirectCDPath"), sDirectCDPath);
	}

	HKEY hRunKey = NULL;
	LONG res;

	CString sSection = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
	res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, sSection, 0, KEY_WRITE|KEY_READ, &hRunKey);
	if (res == ERROR_SUCCESS)
	{
		RegDeleteValue(hRunKey, (LPCTSTR)sValue);
	}
	RegCloseKey(hRunKey);
}

/////////////////////////////////////////////////////////////////////////////
void CDVStarterDlg::MoveDirectCDVers5FromRunToDVRT()
{
//TODO TKR	Reg Einträge für DirectCD 5 verschieben usw
	//SchlüsselName im Run Zweig: _T("AdaptecDirectCD")
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDVStarterDlg::OnCheckRTE(WPARAM wParam, LPARAM lParam)
{
	CheckRuntimeErrors();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CDVStarterDlg::CheckRuntimeErrors()
{
	CWK_RunTimeErrors	rtErrors;
	CWK_RunTimeError*	pError;
	CString				sErrorText;
	DWORD				dwPiezoFlags	= 0;
	int					nCam = 0;

	rtErrors.LoadFromFile(theApp.GetHomeDir()+_T("\\")+RTE_FILENAME);
//	rtErrors.LoadFromFile(_T("c:\\dv\\rterrors.dat"));

	// Die ganze Runtime Error Liste nach nicht bestätigten Fehlern durchsuchen.
	int nSize = rtErrors.GetSize();
	int nCams[16] ={0};

	for (int nI = 0; nI < nSize; nI++)
	{
		pError = rtErrors.GetAt(nI);
		if (pError)
		{
			switch(pError->GetError())
			{
		 		case RTE_CAMERA_MISSING:
					nCam = pError->GetParam1()-1;
					if (nCam >= 0 && nCam <= 15)
					{
						if (pError->GetParam2() == 1)
						{
							sErrorText	= pError->GetErrorText();
							if (sErrorText.Find(RTE_CONFIRMED) == -1)
								nCams[nCam]++;
						}
					}	  
					break;
				case RTE_ARCHIVE_FULL:
					sErrorText	= pError->GetErrorText();
					if (sErrorText.Find(RTE_CONFIRMED) == -1)
						dwPiezoFlags |= PF_ARCHIVE_FULL;
					break;
				case RTE_TEMPERATURE:
					sErrorText	= pError->GetErrorText();
					if (sErrorText.Find(RTE_CONFIRMED) == -1)
						dwPiezoFlags |= PF_TEMPERATURE;
					break;
				case RTE_STORAGE:
					sErrorText	= pError->GetErrorText();
					if (sErrorText.Find(RTE_CONFIRMED) == -1)
						dwPiezoFlags |= PF_CANTSTORE;
					break;
				case RTE_SMART:
					sErrorText	= pError->GetErrorText();
					if (sErrorText.Find(RTE_CONFIRMED) == -1)
						dwPiezoFlags |= PF_SMART_ALARM;
					break;
				case RTE_HARDDISK_FALURE:
					sErrorText	= pError->GetErrorText();
					if (sErrorText.Find(RTE_CONFIRMED) == -1)
						dwPiezoFlags |= PF_HDD_FAIL;
					break;
				case RTE_FANCONTROL:
					sErrorText	= pError->GetErrorText();
					if (sErrorText.Find(RTE_CONFIRMED) == -1)
						dwPiezoFlags |= PF_FAN_ALARM;
					break;
				default:
					break;
			}
		}
	}

	rtErrors.SafeDeleteAll();

	// Liegt ein nicht quittierter Kameraausfall vor?
	for (nI = 0; nI < 16; nI++)
	{
		if (nCams[nI] > 0)
		{
			dwPiezoFlags|= PF_NO_VIDEOSIGNAL;	
			break;
		}
	}

	// Piezo ansteuern.
	for (nI = 0; nI < 32; nI++)
	{
		int nBit = 1<<nI;;
		switch(nBit)
		{
			case PF_NO_VIDEOSIGNAL:	
			case PF_TEMPERATURE:
			case PF_ARCHIVE_FULL:
			case PF_CANTSTORE:
			case PF_SMART_ALARM:
			case PF_FAN_ALARM:
			case PF_HDD_FAIL:
				if (dwPiezoFlags & nBit)
					m_Piezo.PiezoSetFlag(nBit);
				else
					m_Piezo.PiezoClearFlag(nBit);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CDVStarterDlg::QueryMemoryInformation()
{
	BOOL bTraceToLogFile = FALSE;

	// Die PSAPI.DLL wird nur unter WinNT4, Win2000, WinXP unterstützt. Nicht aber
	// für Win9x und WinME
	if (theApp.IsNT())
	{
		// Alle 15 Minuten Speicherinformationen im Logfile mitprotokollieren
		static DWORD dwLastTC = WK_GetTickCount();
		if (WK_GetTimeSpan(dwLastTC) > 15 * 60 * 1000)
		{
			dwLastTC = WK_GetTickCount();
			bTraceToLogFile = TRUE;
		}

		DWORD aProcesses[1024], cbNeeded;
		m_dwTotalUsedMemory = 0;
		if (m_EnumProcesses && m_EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
		{
			// Anzahl der Prozesse
			int cProcesses = cbNeeded / sizeof(DWORD);
			if (bTraceToLogFile)
				WK_TRACE(_T("------------------------------------\n"));
			for (int i = 0; i < cProcesses; i++ )
				EnumerateMemoryInfo(aProcesses[i], i, bTraceToLogFile);
		}

		if (bTraceToLogFile)
		{
			WK_TRACE(_T("====================================\n"));
			WK_STAT_LOG(_T("Reset"), m_dwTotalUsedMemory,			_T("UsedTotal"));
		}

		// Im Dialog immer aktualisieren
		m_sTotalUsedMemory.Format(_T("%d KB"), m_dwTotalUsedMemory);
		UpdateData(FALSE);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CDVStarterDlg::EnumerateMemoryInfo(DWORD processID, int nI, BOOL bTraceToLogFile)
{
    HANDLE hProcess;
    PROCESS_MEMORY_COUNTERS psmemCounters;

    // Print information about the memory usage of the process.
    hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID );

    if (m_GetProcessMemoryInfo && m_GetProcessMemoryInfo( hProcess, &psmemCounters, sizeof(psmemCounters)) )
    {
			HMODULE hModule = NULL;
			DWORD dwNeeded  = sizeof(hModule);

			if (m_EnumProcessModules && m_EnumProcessModules(hProcess, &hModule, sizeof(hModule), &dwNeeded))
			{
				_TCHAR szFileName[_MAX_PATH] = {0};
				CString sFileName, sTraceName;
				
				if (m_GetModuleBaseName && m_GetModuleBaseName(hProcess, hModule, szFileName, sizeof(szFileName)))
					sFileName = szFileName;
				
				sFileName.MakeLower();

				// Sollen alle Prozesses beobachtet werden, oder nur die, die zum DTS gehören.
				if ((sFileName == _T("dvstarter.exe"))  ||
					(sFileName == _T("jacobunit.exe"))  ||
					(sFileName == _T("savicunit.exe"))  ||
					(sFileName == _T("tashaunit.exe"))  ||
					(sFileName == _T("dvprocess.exe"))  ||
					(sFileName == _T("dvstorage.exe"))  ||
					(sFileName == _T("socketunit.exe")) ||
					(sFileName == _T("isdnunit.exe"))   ||
					(sFileName == _T("dvhook.exe"))		||
					(sFileName == _T("dvclient.exe")))
				{
//					DWORD dwTotal = (psmemCounters.PeakWorkingSetSize + psmemCounters.PagefileUsage) / 1024;
					DWORD dwTotal = psmemCounters.PagefileUsage / 1024;
					m_dwTotalUsedMemory += dwTotal;
					
					if (bTraceToLogFile)
					{
						// Die extension entfernen
						int nIndex = sFileName.Find(_T(".exe"));
						if (nIndex != - 1)
							sFileName = sFileName.Left(nIndex);
						
						sTraceName.Format(_T("%02d %s"), nI, sFileName);
						WK_STAT_LOG(_T("Reset"), dwTotal, sTraceName);

	/*					WK_TRACE(_T("Memorystatistic for %s\n"), sFileName);
						WK_TRACE(_T("----------------------------------------------------\n"));
						WK_TRACE(_T("PeakWorkingSize...........=%d [KB]\n"), psmemCounters.PeakWorkingSetSize/1024);
						WK_TRACE(_T("WorkingSize...............=%d [KB]\n"), psmemCounters.WorkingSetSize/1024);
						WK_TRACE(_T("QuotaPeakPagedPoolUsage...=%d [KB]\n"), psmemCounters.QuotaPeakPagedPoolUsage/1024);
						WK_TRACE(_T("QuotaPagedPoolUsage.......=%d [KB]\n"), psmemCounters.QuotaPagedPoolUsage/1024);
						WK_TRACE(_T("QuotaPeakNonPagedPoolUsage=%d [KB]\n"), psmemCounters.QuotaPeakNonPagedPoolUsage/1024);
						WK_TRACE(_T("QuotaNonPagedPoolUsage....=%d [KB]\n"), psmemCounters.QuotaNonPagedPoolUsage/1024);
						WK_TRACE(_T("PagefileUsage.............=%d [KB]\n"), psmemCounters.PagefileUsage/1024);
						WK_TRACE(_T("PeakPagefileUsag..........=%d [KB]\n"), psmemCounters.PeakPagefileUsage/1024);
	*/				}
				}
			}
		}

    CloseHandle( hProcess );
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDVStarterDlg::CheckIfPowerLossed()
{
	BOOL bResult = FALSE;

	CString sShutdownTime;
	CString sAliveTime;

	CWK_Profile wkp;

	sShutdownTime = wkp.GetString(DV_DVSTARTER, _T("ShutDownTime"),_T(""));
	sAliveTime = wkp.GetString(DV_DVSTARTER, _T("ALiveTime"),_T(""));

	if (!sAliveTime.IsEmpty())
	{
		if ( (sShutdownTime==sAliveTime) || (sShutdownTime.IsEmpty()))
		{
			// normal shutdown
			bResult = TRUE;
		}
		else
		{
			// watchdog oder Stromausfall
			CSystemTime stNow;
			CSystemTime stLastAliveTime;
			CSystemTime stResult;
			stNow.GetLocalTime();
			TRACE(_T("%s\n"), (LPCTSTR)stNow.ToString());

			if (stLastAliveTime.FromString(sAliveTime))
			{
				CString sResult,sF,sL;
				// calculate time diff between now and last actualize
				// CARE result is time since 1.1.1601
				stResult = stNow - stLastAliveTime;

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

				CWK_RunTimeError rte(WAI_DV_STARTER,REL_ERROR,RTE_LC_AC_FAIL,sL);
				rte.Send();
			}
		}
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDVStarterDlg::OnDeviceChange(UINT nEventType, DWORD dwData)
{
	DEV_BROADCAST_HDR* pHdr = (DEV_BROADCAST_HDR*)dwData;
	
	if (pHdr)
	{
		if (pHdr->dbch_devicetype == DBT_DEVTYP_VOLUME)
		{
			DEV_BROADCAST_VOLUME *pDBV  = (DEV_BROADCAST_VOLUME*)pHdr;
			if (!(pDBV->dbcv_flags & DBTF_MEDIA))
			{
				switch (nEventType)
				{
					case DBT_DEVICEARRIVAL:
					case DBT_DEVICEREMOVECOMPLETE:
						((CDVStarterApp*)AfxGetApp())->SetInstallationSourcePaths();
					break;
				}
			}
		}
	}
	return 1;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDVStarterDlg::CheckHDTemperaturs()
{
	BOOL bResult = FALSE;

	static nLastTemperature[4] = {-1};

	if (theApp.IsWinXP() && theApp.IsTransmitter())
	{
		// Die Temeraturen aller angeschlossenen IDE-Festplatten ausgeben, sofern möglich.
		CHDDTApi Hdd;
		CWK_Profile prof;
		int nMaxTemperature = prof.GetInt(DV_DVSTARTER, _T("MaxTemperatureHDD"), 55);
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
							CWK_RunTimeError e(WAI_LAUNCHER, REL_MESSAGE, RTE_TEMPERATURE, _T(""));
							e.Send();
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
							DWORD dwParam2 = MAKELONG(1, nMaxTemperature); // alarm on
							CWK_RunTimeError e(WAI_LAUNCHER, REL_ERROR, RTE_TEMPERATURE, sDrive, dwParam1, dwParam2);
							e.Send();							
							m_pnLastTemperatures[uDrive] = VIRTUAL_ALARM_SET;
						}
					}
				}
			}
		}
	}

	return bResult;
}
///////////////////////////////////////////////////////////////////////////////
BOOL CDVStarterDlg::CheckSMARTparameters()
{
	BOOL bResult = FALSE;

	if (theApp.IsWinXP() && theApp.IsTransmitter())
	{
		GETVERSIONOUTPARAMS VersionParams;

		CSmart smart;
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
							bResult = TRUE;
							if (bState)
							{
								m_pbSMARTFailureReported[nDrive] = FALSE;
								WK_TRACE(_T("S.M.A.R.T status: OK (Drive %d <%s>)\n\n"), nDrive, smart.GetModelNumber());
							}
							else
							{
								if (m_pbSMARTFailureReported[nDrive] == FALSE)
								{
									m_pbSMARTFailureReported[nDrive] = TRUE;
									CString sErrorMsg;
									sErrorMsg.Format(_T("S.M.A.R.T status: failed! (Drive %d <%s>)"), nDrive, smart.GetModelNumber());
									CWK_RunTimeError rte(WAI_LAUNCHER, REL_ERROR, RTE_SMART, sErrorMsg);
									rte.Send();
								}
							}
						}
					}
				}
				smart.Close();
			}
		}
	}
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDVStarterDlg::OnQueryEndSession() 
{
	WK_TRACE(_T("OnQueryEndSession\n"));

	if (!CDialog::OnQueryEndSession())
		return FALSE; 
	
	CWnd* pWnd = FindWindow(_T("DVClient"), NULL);
	if (WK_IS_WINDOW(pWnd))
   		pWnd->PostMessage(WM_POWEROFF_PRESSED, 0, 0);
	else
		WK_TRACE_WARNING(_T("Can't find DVClient window\n"));

	return m_bQueryEndSessionReturn;
}
