// DVClient.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DVClient.h"

#include "MainFrame.h"
#include "CPanel.h"
#include "Server.h"
#include "IPCServerControlDVClient.h"
#include "CIPCAudioDVClient.h"
#include "LiveWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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

/////////////////////////////////////////////////////////////////////////////
// CDVClientApp

BEGIN_MESSAGE_MAP(CDVClientApp, CWinApp)
	//{{AFX_MSG_MAP(CDVClientApp)
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDVClientApp construction

CDVClientApp::CDVClientApp()
{
	m_hArrow		= NULL;
	m_hNoCursor		= NULL;
	m_hGlass		= NULL;
	m_hRect			= NULL;
	m_hPanLeft		= NULL;
	m_hPanRight		= NULL;
	m_hTiltUp		= NULL;
	m_hTiltDown		= NULL;
	m_hZoomIn		= NULL;
	m_hZoomOut		= NULL;
	m_hFocusFar		= NULL;
	m_hFocusNear	= NULL;
	m_hFocusAuto	= NULL;
	m_hLeftTop		= NULL;
	m_hRightTop		= NULL;
	m_hRightBottom	= NULL;
	m_hLeftBottom	= NULL;
	m_bUseMMtimer	= TRUE;
	
	m_nMaxSentSamples = 3;
	m_nMaxRequestedRecords = 8;
	m_nTimeOutPlay = 20;

	m_pServer		= NULL;
	m_bReadOnly = FALSE;
	m_bShowMilliseconds = FALSE;
	m_bShowFPS = FALSE;
	m_bShowSequenceInfo = FALSE;
	m_bTKR_Trace = FALSE;
	m_bTKR_TraceBackup = FALSE;
	m_bAudibleAlarm = FALSE;
	m_bRealtime = FALSE;
	m_bChangeCodePage = FALSE;
	m_bReceiver = FALSE;
	m_bTransmitter = TRUE;
	m_bUseSpeaker = FALSE;
	m_pCIPCServerControlDVClient = NULL;

	m_iMajorVersion = 0;
	m_iMinorVersion = 0;
	m_iSubVersion = 0; 
	m_iBuildNr = 0;

	m_bAudioAllowed = FALSE;
	m_pCIPCLocalAudio = NULL;

	m_bComputerNameValid = !GetIntValue(CAN_SET_COMPUTERNAME, FALSE, FALSE);

	CString sExe;
	GetModuleFileName(NULL,sExe.GetBufferSetLength(_MAX_PATH),_MAX_PATH);
	sExe.ReleaseBuffer();
	CString sVersion;
	sVersion = WK_GetFileVersion(sExe);
	int i1,i2,i3,i4;
	i1 = i2 = i3 = i4 = 0;
	if (4==_stscanf(sVersion,_T("%d.%d.%d.%d"),&i1,&i2,&i3,&i4))
	{
		m_iMajorVersion = i1;
		m_iMinorVersion = i2;
		m_iSubVersion = i3;
		m_iBuildNr = i4;
	}

	CWK_Profile system(CWK_Profile::WKP_ABSOLUT,HKEY_LOCAL_MACHINE,_T(""),_T(""));
	m_sComputerName = system.GetString(REGPATH_HKLM_COMPUTERNAME, REGKEY_SERIALNUMBER,_T(""));
	if (m_sComputerName.IsEmpty())	// if the computername has not been changed
	{								// read from the system location
		m_sComputerName = system.GetString(REGPATH_HKLM_COMPUTERNAME, REGKEY_COMPUTERNAME,_T(""));
	}
	else							// it is saved to the alternative location
	{								// if a valid computername has been set.
		m_bComputerNameValid = TRUE;
	}

	// Performance Management
	m_nNoOfProcessors = 1;
	NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(GetModuleHandle(_T("ntdll")),"NtQuerySystemInformation");
	if (NtQuerySystemInformation)
	{
		SYSTEM_BASIC_INFORMATION       SysBaseInfo;
		// get number of processors in the system
		LONG status = NtQuerySystemInformation(SystemBasicInformation,&SysBaseInfo,sizeof(SysBaseInfo),NULL);
		if (status == NO_ERROR)
		{
			m_nNoOfProcessors = SysBaseInfo.bKeNumberProcessors;
		}
	}
	m_dwCPUusage = 0;
	m_wUpperThreshold = (WORD)GetIntValue(_T("UpperCPUusageThreshold"), 85);
	m_wLowerThreshold = (WORD)GetIntValue(_T("LowerCPUusageThreshold"), 70);
	m_nMinPerfLevel	  = (signed short)GetIntValue(_T("MinPerformanceLevel"), 0);
	m_nPerformanceLevel	= m_nMinPerfLevel;
	m_AvgCPUusage.SetNoOfAvgValues(GetIntValue(_T("CPUusageAvgValues"), 5));
	m_liOldIdleTime.LowPart = 0;
	m_liOldIdleTime.HighPart = 0;
	m_liOldSystemTime.LowPart = 0;
	m_liOldSystemTime.HighPart = 0;
}

CDVClientApp::~CDVClientApp()
{
	WK_DELETE(m_pCIPCLocalAudio);
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDVClientApp object

CDVClientApp theApp;
CWK_Timer theTimer;

/////////////////////////////////////////////////////////////////////////////
void CDVClientApp::RegisterWindowClass()
{
    WNDCLASS  wndclass;

    // register window class
    wndclass.style =         0;
    wndclass.lpfnWndProc =   DefWindowProc;
    wndclass.cbClsExtra =    0;
    wndclass.cbWndExtra =    0;
    wndclass.hInstance =     m_hInstance;
    wndclass.hIcon =         LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME));
    wndclass.hCursor =       LoadStandardCursor(IDC_NO);
    wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1) ;
    wndclass.lpszMenuName =  0L;
    wndclass.lpszClassName = WK_APP_NAME_DVCLIENT;

    AfxRegisterClass(&wndclass);

    // register window class
    wndclass.style =         0;
    wndclass.lpfnWndProc =   DefWindowProc;
    wndclass.cbClsExtra =    0;
    wndclass.cbWndExtra =    0;
    wndclass.hInstance =     m_hInstance;
    wndclass.hIcon =         LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME));
    wndclass.hCursor =       NULL; // to avoid flickering, will be set in OnMouseMove
    wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1) ;
    wndclass.lpszMenuName =  0L;
    wndclass.lpszClassName = _T("DVDisplay");

    AfxRegisterClass(&wndclass);

    // register window class
    wndclass.style =         0;
    wndclass.lpfnWndProc =   DefWindowProc;
    wndclass.cbClsExtra =    0;
    wndclass.cbWndExtra =    0;
    wndclass.hInstance =     m_hInstance;
    wndclass.hIcon =         LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME));
    wndclass.hCursor =       m_hArrow;
    wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1) ;
    wndclass.lpszMenuName =  0L;
    wndclass.lpszClassName = _T("DVPanel");

    AfxRegisterClass(&wndclass);
}

/////////////////////////////////////////////////////////////////////////////
// CDVClientApp initialization

BOOL CDVClientApp::InitInstance()
{
	if (!WK_ALONE(WK_APP_NAME_DVCLIENT)) 
	{
#ifndef _DEBUG
		return FALSE;
#endif
	}
	if (WK_IS_RUNNING(WK_APP_NAME_LAUNCHER)) 
	{
		// don't run if idip(r) is running
		return FALSE;
	}

	XTIB::SetThreadName(_T("MainThread"));

	// TODO! ML: Use CSkinDialog in stead of CTransparentDialog
	CSkinDialog::SetOEMSettings(StyleBackGroundColorChangeSimple, SKIN_COLOR_BLUE, SKIN_COLOR_GOLD_METALLIC, TRUE);
	//RKE only the BaseColorButtons is set, rest stays as it was
	CSkinButton::SetOEMColors(CPanel::m_BaseColorButtons, CSkinButton::GetTextColorNormalG(), CSkinButton::GetTextColorHighlightedG(), CSkinButton::GetTextColorDisabledG(), SKIN_COLOR_KEY);
	CSkinDialog::LoadButtonImages();

	//CSkinGroupBox only used in Maintenance Dialog
	COLORREF cBaseColor = CPanel::m_BaseColorBackGrounds;
	COLORREF cLineLight = CSkinButton::ChangeBrightness(cBaseColor, 20, TRUE);
	COLORREF cLineDark  = CSkinButton::ChangeBrightness(cBaseColor, -30, TRUE);
	CSkinGroupBox::SetOEMColors(CSkinButton::GetTextColorNormalG(), CSkinButton::GetTextColorNormalG(), 
								cBaseColor, cLineLight, cLineDark);

	//CSkinEdit only used in Maintenance Dialog
	cBaseColor = CPanel::m_BaseColorBackGrounds;
	CSkinEdit::SetOEMColors(cBaseColor, CSkinButton::GetTextColorNormalG());

	CWK_Dongle	 Dongle;

	// Bin ich Receiver, oder Transmitter
	m_bReceiver		 = (Dongle.GetProductCode() == IPC_DTS_IP_RECEIVER);
	m_bTransmitter	 = (Dongle.GetProductCode() == IPC_DTS);

	if (!m_bReceiver && !m_bTransmitter)
	{
		m_bTransmitter = TRUE;
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#if _MFC_VER < 0x700
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

	m_hArrow		= LoadCursor(IDC_ARROW4);
	m_hNoCursor		= LoadCursor(IDC_NO);
	m_hGlass		= LoadCursor(IDC_GLASS);
	m_hRect			= LoadCursor(IDC_RECT);
	m_hPanLeft		= LoadCursor(IDC_PANLEFT);
	m_hPanRight		= LoadCursor(IDC_PANRIGHT);
	m_hTiltUp		= LoadCursor(IDC_TILTUP);
	m_hTiltDown		= LoadCursor(IDC_TILTDOWN);
	m_hZoomIn		= LoadCursor(IDC_ZOOMIN);
	m_hZoomOut		= LoadCursor(IDC_ZOOMOUT);
	m_hFocusFar		= LoadCursor(IDC_FOCUS_FAR);
	m_hFocusNear	= LoadCursor(IDC_FOCUS_NEAR);
	m_hFocusAuto	= LoadCursor(IDC_FOCUS_AUTO);
	m_hLeftTop		= LoadCursor(IDC_LEFTTOP);
	m_hRightTop		= LoadCursor(IDC_RIGHTTOP);
	m_hRightBottom	= LoadCursor(IDC_RIGHTBOTTOM);
	m_hLeftBottom	= LoadCursor(IDC_LEFTBOTTOM);

	RegisterWindowClass();


	// Audio, check for NOT the real NT!!
	// every OS can Audio except NT 4.0
	m_bAudioAllowed = Dongle.RunAudioUnit() && !theApp.IsNT40();

	
	if (WK_IS_RUNNING(WK_DB_SERVER_READ_ONLY))
	{
		InitDebugger(_T("dvcro.log"),WAI_DV_CLIENT);
		m_bReadOnly = TRUE;
		
		//in ReadOnly is audio allways allowed
		m_bAudioAllowed = TRUE;
		// Registry einträge sind nicht erlaubt
		CWK_Profile::SetReadOnlyMode(TRUE);
	}
	else
	{
		InitDebugger(_T("dvc.log"),WAI_DV_CLIENT);
//		CRunService rs;
//		rs.ClearRun(_T("PacketCD"));
	}

	CWK_PerfMon perfMon(_T("Dummy"));
	m_dwCPUClock = perfMon.GetClockFrequency();

	LoadConfiguration();

	_TCHAR szPath[_MAX_PATH];
	GetModuleFileName(m_hInstance, szPath, _MAX_PATH);
	m_sHomePath = szPath;
	int nI = m_sHomePath.ReverseFind(_T('\\'));
	if (nI != -1)
	{
		m_sHomePath = m_sHomePath.Left(nI);

#ifdef _DEBUG
#ifdef _UNICODE
//		m_sHomePath = _T("C:\\DVX");
#else
	//TKR muss oben angegebener Pfad bleiben im Debugmode ! 
	//	m_sHomePath = _T("C:\\DV");
#endif
#endif

	}

	::GetWindowsDirectory(m_sWindowsDir.GetBuffer(_MAX_PATH),
					  _MAX_PATH);
	m_sWindowsDir.ReleaseBuffer();
	
	// Ohne Sprach-Dll kein Panel
	CStringArray result;
	CString sDll = szLangPrefix;
	sDll += _T("???");	// Do NOT translate, wildcard for chars
#ifdef _DEBUG
	sDll += szLangDebug;
#endif
	sDll += szLangSuffix;

	if (0 == WK_SearchFiles(result, m_sHomePath, sDll, FALSE))
	{
		CWK_RunTimeError rte(REL_CANNOT_RUN, RTE_CONFIGURATION, _T("No language library installed"));
		rte.Send();
		WK_TRACE_ERROR(_T("Program terminated\n"));
		return FALSE;
	}

	// Läuft der Client im Readonlymodus?
	InitReadOnlyModus();

	// Im Readonly-modus die Datenbank nachstarten, sofern diese noch nicht läuft.
	if (IsReadOnly() && !WK_IS_RUNNING(WK_DB_SERVER_READ_ONLY))
		WinExec("dvstorage.exe",SW_SHOWNORMAL);

	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object.

	CMainFrame* pWnd = new CMainFrame;

	// create and load the frame with its resources
	int a,b, x, y, w, h;
	if (   IsTransmitter()
		&& !IsReadOnly())
	{	
		a = GetSystemMetrics(SM_CXEDGE);
		b = GetSystemMetrics(SM_CYEDGE);
		x = 0;
		y = 0;
		w = GetSystemMetrics(SM_CXSCREEN);
		h = GetSystemMetrics(SM_CYSCREEN);
	}
	else
	{
		RECT rcDesktop;
		SystemParametersInfo(
					  SPI_GETWORKAREA,  // system parameter to retrieve or set
					  0,   // depends on action to be taken
					  (PVOID)&rcDesktop,  // depends on action to be taken
					  0);    // user profile update option

		a = 0;
		b = 0;
		x = rcDesktop.left;
		y = rcDesktop.top;
		w = rcDesktop.right - rcDesktop.left;
		h = rcDesktop.bottom- rcDesktop.top;
	}
#ifdef _DEBUG
 	w = (w*2)/3;
	h = (h*2)/3;
#else
	CWK_Profile wkp;
	BOOL bWindowMode = wkp.GetInt(REGKEY_DVCLIENT, _T("WindowMode"), 0);
	wkp.WriteInt(REGKEY_DVCLIENT, _T("WindowMode"), bWindowMode);
	if (bWindowMode)
	{
 		w = (w*2)/3;
		h = (h*2)/3;
	}
#endif

	int nWndStyle = 0;
#ifdef _DEBUG
	// In der Debugversion immer im Fenster laufen lassen
	nWndStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_EX_TOPMOST;
#else
	// Beim Empfänger läuft der DTS im Fenster, dadurch kann er bei Bedarf minimiert werden.
	if (   theApp.IsTransmitter()
		&& !IsReadOnly())
	{
		if (bWindowMode)
			nWndStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_EX_TOPMOST;
		else
			nWndStyle = WS_POPUP | WS_VISIBLE;
	}
	else
	{
		nWndStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
	}
#endif

	if (pWnd->CreateEx(0,_T("DVClient"),_T("DVClient"),nWndStyle, x-a,y-b,w+2*a,h+2*b,NULL,NULL,NULL))
	{
		m_pMainWnd = pWnd;
		EndWaitCursor();
		m_DVUIThread.CreateThread();
		m_RequestThread.StartThread();
	}
	else
	{
		return FALSE;
	}

	WK_TRACE(_T("CDVClientApp Thread ID is %08lx\n"),GetCurrentThreadId());

	WK_TRACE(_T("I am a %s%s\n"), (IsTransmitter()?_T("Transmitter"):_T("")), (IsReceiver()?_T("Receiver"):_T("")));
	WK_TRACE(_T("RunAudioUnit=%d\n"), m_bAudioAllowed);

	if (IsReceiver())
	{
		m_pCIPCServerControlDVClient = new CIPCServerControlDVClient(WK_SMClientControl);
	}

	m_bCanBackupWithACDC = FALSE;
	m_bIsACDCBackupDrive = TRUE;	//wird auf FALSE gesetzt, falls kein ACDC Backup Drive
									//(installierter Brenner) vorhanden ist
	
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
int CDVClientApp::ExitInstance() 
{
	WK_DELETE(m_pCIPCServerControlDVClient);
	SaveConfiguration();

	//	TRACE(_T("CDVClientApp::ExitInstance begin\n"));
	// Stoppe Requesthread
	m_RequestThread.StopThread();
	// Zerstöre Panel -> dadurch wird auch der DVUIThread beendet
	CPanel* pPanel = GetPanel();
	if (pPanel)
	{
//		TRACE(_T("CDVClientApp::ExitInstance destroy panel\n"));
		pPanel->PostMessage(WM_EXIT);
	}

	// Warte bis der DVUIThread beendet ist
	// gf 30052000 Aber nur bei gueltigen Handle, d.h. Thread wurde ueberhaupt gestartet
	if (m_DVUIThread.m_hThread)
	{
		DWORD dwTimeout = 5000;
		CWK_StopWatch w;
		DWORD dwReturn = WaitForSingleObject(m_DVUIThread.m_hThread, dwTimeout);
		switch (dwReturn)
		{
		case WAIT_OBJECT_0:
			w.StopWatch(_T("DVUIThread closed"));
			break;
		default:
			w.StopWatch(_T("DVUIThread not closed TIMEOUT"));
			break;
		}
	}
	else
	{
		WK_TRACE(_T("m_DVUIThread.m_hThread = NULL\n"));
	}
	WK_TRACE(_T("CDVClientApp::ExitInstance()\n"));

	CLiveWindow::ReleaseMMTimer();

	CloseDebugger();

	if (   IsReadOnly() 
		&& WK_IS_RUNNING(WK_DB_SERVER_READ_ONLY)
		&& WK_IS_RUNNING(WK_APP_NAME_DB_SERVER))
	{
		// close database server
		HWND hWnd = FindWindow(WK_APP_NAME_DB_SERVER,NULL);
		if (hWnd && IsWindow(hWnd))
		{
			PostMessage(hWnd,WM_COMMAND,ID_APP_EXIT,0L);
		}
		
		//close auduio unit
		HWND hAudio = FindWindow(NULL, WK_APP_NAME_AUDIOUNIT1);
		if (hAudio && IsWindow(hAudio))
		{
			PostMessage(hAudio, WM_COMMAND,ID_APP_EXIT,0L);	
		}
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CDVClientApp::LoadConfiguration()
{
	CWK_Profile prof;

	if ((BOOL)prof.GetInt(_T("Log"),_T("LogMilliSeconds"),0))
	{
		SET_WK_STAT_TICK_COUNT(GetTickCount());
	}

	if (!IsReadOnly() && IsTransmitter())
	{
        m_bShowMilliseconds = (BOOL)prof.GetInt(REGKEY_DVCLIENT,_T("ShowMilliseconds"),m_bShowMilliseconds);
        m_bShowSequenceInfo = (BOOL)prof.GetInt(REGKEY_DVCLIENT,_T("ShowSequenceInfo"),m_bShowSequenceInfo);
		m_bTKR_Trace = (BOOL)prof.GetInt(REGKEY_DVCLIENT,_T("TKR_Trace"),m_bTKR_Trace);
		m_bTKR_TraceBackup = (BOOL)prof.GetInt(REGKEY_DVCLIENT,_T("TKR_TraceBackup"),m_bTKR_TraceBackup);
	}
	m_bShowFPS = (BOOL)prof.GetInt(REGKEY_DVCLIENT,_T("ShowFPS"),m_bShowFPS);
	m_bShowCPUusage = GetIntValue(_T("Show_CPU_Usage"));
	m_bUseMMtimer   = GetIntValue(_T("UseMMTimer"), m_bUseMMtimer);

	prof.DeleteEntry(REGKEY_DVCLIENT, _T("DrawInDecodeThread"));
	prof.DeleteEntry(REGKEY_DVCLIENT, _T("Mpeg4DecodeMT"));

	if (!IsReadOnly())
	{
		m_bAudibleAlarm = (BOOL)prof.GetInt(REGKEY_DVCLIENT, _T("AcousticAlarmAtBackground"), m_bAudibleAlarm);
		m_bChangeCodePage = (BOOL)prof.GetInt(REGKEY_DVCLIENT, _T("ChangeCodePage"), m_bChangeCodePage);
		m_bRealtime = prof.GetInt(REGKEY_DVCLIENT, _T("Realtime"),  m_bRealtime);
		m_bUseSpeaker = prof.GetInt(REGKEY_DVCLIENT, _T("UseSpeaker"),  -1);
		if (m_bUseSpeaker == -1)
		{
			m_bUseSpeaker = 1;
			prof.WriteInt(REGKEY_DVCLIENT, _T("UseSpeaker"),  m_bUseSpeaker);
		}
		int nTemp = prof.GetInt(REGKEY_DVCLIENT, REGKEY_MAX_SENT_SAMPLES, -1);
		if (nTemp == -1)
		{
			prof.WriteInt(REGKEY_DVCLIENT, REGKEY_MAX_SENT_SAMPLES, m_nMaxSentSamples);
		}
		else
		{
			m_nMaxSentSamples = nTemp;
		}
		nTemp = prof.GetInt(REGKEY_DVCLIENT, REGKEY_MAX_REQUESTED_RECORDS, -1);
		if (nTemp == -1)
		{
			prof.WriteInt(REGKEY_DVCLIENT, REGKEY_MAX_REQUESTED_RECORDS, m_nMaxRequestedRecords);
		}
		else
		{
			m_nMaxRequestedRecords = nTemp;
		}
		nTemp = prof.GetInt(REGKEY_DVCLIENT, REGKEY_TIME_OUT_PLAY, -1);
		if (nTemp == -1)
		{
			prof.WriteInt(REGKEY_DVCLIENT, REGKEY_TIME_OUT_PLAY, m_nTimeOutPlay);
		}
		else
		{
			 m_nTimeOutPlay = nTemp;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVClientApp::SaveConfiguration()
{
	CWK_Profile prof;

	if (!IsReadOnly() && IsTransmitter())
	{
        prof.WriteInt(REGKEY_DVCLIENT,_T("ShowMilliseconds"),m_bShowMilliseconds);
        prof.WriteInt(REGKEY_DVCLIENT,_T("ShowSequenceInfo"),m_bShowSequenceInfo);
		prof.WriteInt(REGKEY_DVCLIENT,_T("ShowFPS"),m_bShowFPS);
		prof.WriteInt(REGKEY_DVCLIENT,_T("TKR_Trace"),m_bTKR_Trace);
		prof.WriteInt(REGKEY_DVCLIENT,_T("TKR_TraceBackup"),m_bTKR_TraceBackup);
	}

	if (!IsReadOnly())
	{
		prof.WriteInt(REGKEY_DVCLIENT, _T("AcousticAlarmAtBackground"), m_bAudibleAlarm);
		prof.WriteInt(REGKEY_DVCLIENT, _T("ChangeCodePage"), m_bChangeCodePage);
        prof.WriteInt(REGKEY_DVCLIENT,_T("Realtime"),m_bRealtime);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVClientApp::ForceOnIdle()
{
	if (m_pMainWnd)
	{
		m_pMainWnd->PostMessage(WM_USER);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDVClientApp::OnIdle(LONG lCount) 
{
	if (m_pServer)
	{
		m_pServer->OnIdle();
	}

	Sleep(0);

	if (m_pMainWnd && (lCount>0))
	{
		m_IdleEvent.SetEvent();
	}
	
	if (m_bReceiver || IsReadOnly())
	{
		ConnectToLocalAudio();
		if (m_pCIPCLocalAudio!=NULL && (m_pCIPCLocalAudio->GetIPCState() == CIPC_STATE_DISCONNECTED))
		{
			WK_DELETE(m_pCIPCLocalAudio);
		}
	}

	_TCHAR szBubbles[16];
	_itot(CIPCExtraMemory::GetCreatedBubbles(), szBubbles, 10);
	((CMainFrame*)m_pMainWnd)->SetPanetext(2, szBubbles);

	return CWinApp::OnIdle(lCount);
}
/////////////////////////////////////////////////////////////////////////////
void CDVClientApp::Connect(const CString& sHostNumber, const CString& sHostName, const CString& sPin)
{
	if (m_pServer == NULL)
	{
		m_pServer = new CServer();
		if (GetIntValue(_T("AddDebugWindow")))
		{
			m_pMainWnd->PostMessage(WM_USER, WPARAM_ADD_DEBUG);
		}
	}

	if (!m_pServer->IsConnected())
	{
		CString s(m_lpCmdLine);
		if (s.IsEmpty())
			s = sHostNumber;

		if (s.IsEmpty())
			m_pServer->Connect(LOCAL_LOOP_BACK, sHostName, sPin);
		else
		{
			m_pServer->Connect(s,sHostName,sPin);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVClientApp::DoDisconnect()
{
	CMainFrame* pMF = GetMainFrame();
	if (pMF)
	{
		m_LockEvent.ResetEvent();
		pMF->PostMessage(WM_USER,MAKELONG(WPARAM_DISCONNECT,0),0);
		// goes directly thread independent to Disconnect.
		DWORD dwTick = GetTickCount();
		DWORD dwWait = WaitForSingleObject(m_LockEvent,2000);
		if (dwWait == WAIT_TIMEOUT)
		{
			WK_TRACE(_T("wait for lock display in %d ms\n"),GetTimeSpan(dwTick));
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVClientApp::Disconnect()
{
	if (m_pServer)
	{
		m_pServer->Disconnect();
		WK_DELETE(m_pServer);

		m_LockEvent.SetEvent();

		CPanel* pPanel = GetPanel();
		if (pPanel)
		{
			pPanel->NotifyDisconnect();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
CServer* CDVClientApp::GetServer()
{
	return m_pServer;
}

//////////////////////////////////////////////////////////////////////
void CDVClientApp::InitReadOnlyModus()
{
	_TCHAR szBuffer[_MAX_PATH];
	CString sFile;
	CString sTest1,sTest2;
	CString sStartDrive;
	int p;

	GetModuleFileName(m_hInstance,szBuffer,sizeof(szBuffer));
	sFile = szBuffer;
	sStartDrive = sFile.Left(3);

	p = sFile.ReverseFind(_T('\\'));
	if (p!=-1)
	{
		sTest1 = sStartDrive + _T("autorun.inf");
		sTest2 = sStartDrive + _T("dvb0001");
		if (DoesFileExist(sTest1) && DoesFileExist(sTest2))
		{
			m_bReadOnly = TRUE;
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CDVClientApp::OnAppExit()
{
	// same as double-clicking on main window close box
	if (m_pMainWnd)
	{
		CPanel* pPanel = GetPanel();
		if (pPanel)
		{
			pPanel->PostMessage(WM_EXIT);
		}
	}
}
			 
/////////////////////////////////////////////////////////////////////////////
BOOL CDVClientApp::SetPrinterOriantation(CDC &dcPrint, const CString &sDeviceName, short dmOrientation)
{
	HANDLE			hPrinter	= NULL;
	LPDEVMODE		pDevMode	= NULL;
	DWORD			dwNeeded	= 0;
	DWORD			dwRet		= 0;
	_TCHAR* pDeviceName	= (_TCHAR*)LPCTSTR(sDeviceName);
	
	if (!pDeviceName)
		return FALSE;

	// Start by opening the printer
	if (!OpenPrinter((_TCHAR*)pDeviceName , &hPrinter, NULL))
	   return FALSE;

	// Allocate a buffer of the correct size.
	dwNeeded = DocumentProperties(NULL, hPrinter, (_TCHAR*)pDeviceName, NULL, NULL, 0);             
	pDevMode = (LPDEVMODE)malloc(dwNeeded);

	// Get the default DevMode for the printer and
	dwRet = DocumentProperties(NULL,  hPrinter, (_TCHAR*)pDeviceName, pDevMode, NULL, DM_OUT_BUFFER); 
	if (dwRet != IDOK)
	{
	   // if failure, cleanup and return failure
	   free(pDevMode);
	   ClosePrinter(hPrinter);
	   return FALSE;
	}

	// Make changes to the DevMode which are supported.
	if (pDevMode->dmFields & DM_ORIENTATION)
	{
		// if the printer supports paper orientation, set it
		pDevMode->dmOrientation = dmOrientation;
	}

	if (pDevMode->dmFields & DM_DUPLEX)
	{
		// if it supports duplex printing, use it
		pDevMode->dmDuplex = DMDUP_HORIZONTAL;
	}

	// Merge the new settings with the old.
	// This gives the driver a chance to update any private portions of the DevMode structure.
	dwRet = DocumentProperties(NULL, hPrinter, (_TCHAR*)pDeviceName, pDevMode, pDevMode, DM_IN_BUFFER | DM_OUT_BUFFER); 

	// Done with the printer
	ClosePrinter(hPrinter);

	if (dwRet != IDOK)
	{
	   // if failure, cleanup and return failure
	   free(pDevMode);
	   return FALSE;
	}

	dcPrint.ResetDC(pDevMode);
	free(pDevMode);

	// return the modified DevMode structure
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// See: HOWTO: Get the Status of a Printer and a Print Job (Q160129)
BOOL CDVClientApp::GetPrinterStatus(DWORD& dwPrinterStatus, CString& sPrinterStatus)
{
	BOOL	bResult		= FALSE;
	DWORD	dwNeeded	= 0;
	DWORD	dwUsed		= 0;
	DWORD	dwReturned	= 0;
	
	CPrintDialog dlg(FALSE);
	dlg.GetDefaults();

	HANDLE hPrinter = NULL;
	Sleep(1000); // Der Drucker muß
	if (OpenPrinter((_TCHAR*)LPCTSTR(dlg.GetDeviceName()), &hPrinter, NULL))
	{
		// Get size of PrinterInfo
		GetPrinter(hPrinter, 2, (LPBYTE)NULL, 0, &dwNeeded);
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			PRINTER_INFO_2* pPrinterInfo = (PRINTER_INFO_2*)(new BYTE[dwNeeded]);
			if (pPrinterInfo)
			{
				ZeroMemory(pPrinterInfo, dwNeeded);
				if (GetPrinter(hPrinter, 2, (LPBYTE)pPrinterInfo, dwNeeded, &dwNeeded))
				{
					if (pPrinterInfo->Status != 0)
					{
						// If the Printer reports an error, believe it.						
						dwPrinterStatus = pPrinterInfo->Status;
						bResult = TRUE;
					}
					else
					{
						// Get job storage space.
						EnumJobs(hPrinter, 0, pPrinterInfo->cJobs, 2, NULL, 0, &dwNeeded, &dwUsed);
						if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
						{
							JOB_INFO_2* pJobStorage = (JOB_INFO_2 *)(new BYTE[dwNeeded]);
							if (pJobStorage)
							{
								ZeroMemory(pJobStorage, dwNeeded);

								// Get the list of jobs.
								if (EnumJobs(hPrinter, 0, pPrinterInfo->cJobs, 2, (LPBYTE)pJobStorage, dwNeeded, &dwUsed, &dwReturned))
								{						
									// Find the Job in the Queue that is printing.
									for (DWORD dwI=0; dwI < pPrinterInfo->cJobs; dwI++)
									{
										if (pJobStorage[dwI].Status & JOB_STATUS_PRINTING)
										{
											// If the job is in an error state, report an error for the printer.
											// Code could be inserted here to attempt an interpretation of the
											// pStatus member as well.
											if (pJobStorage[dwI].Status & JOB_STATUS_ERROR)
												dwPrinterStatus |= PRINTER_STATUS_ERROR;
											if (pJobStorage[dwI].Status & JOB_STATUS_OFFLINE)
												dwPrinterStatus |= PRINTER_STATUS_OFFLINE;
											if (pJobStorage[dwI].Status & JOB_STATUS_PAPEROUT)
												dwPrinterStatus |= PRINTER_STATUS_PAPER_OUT;
											if (pJobStorage[dwI].Status & JOB_STATUS_BLOCKED_DEVQ) // ?
												dwPrinterStatus |= PRINTER_STATUS_ERROR;
											if (pJobStorage[dwI].Status & JOB_STATUS_USER_INTERVENTION)
												dwPrinterStatus |= PRINTER_STATUS_USER_INTERVENTION;
											if (pJobStorage[dwI].Status & JOB_STATUS_DELETING)
												dwPrinterStatus |= PRINTER_STATUS_PENDING_DELETION;
											if (pJobStorage[dwI].Status & JOB_STATUS_PAUSED)
												dwPrinterStatus |= PRINTER_STATUS_PAUSED;
										
											bResult = TRUE;
											break;
										}
									}
								}
							}
							WK_DELETE(pJobStorage);
						}
					}
				}
			}
			WK_DELETE(pPrinterInfo);
		}
		ClosePrinter(hPrinter);
	}

	sPrinterStatus = _T("");

	if ((bResult) && (dwPrinterStatus != 0))
	{
		if (dwPrinterStatus & PRINTER_STATUS_BUSY)				// The printer is busy.
			sPrinterStatus += _T("..The printer is busy..");
		if (dwPrinterStatus & PRINTER_STATUS_DOOR_OPEN)			// The printer door is open. 
			sPrinterStatus += _T("..The printer door is open..");
		if (dwPrinterStatus & PRINTER_STATUS_ERROR)				// The printer is in an error state. 
			sPrinterStatus += _T("..The printer is in an error state..");
		if (dwPrinterStatus & PRINTER_STATUS_INITIALIZING)		// The printer is initializing.  
			sPrinterStatus += _T("..The printer is initializing..");
		if (dwPrinterStatus & PRINTER_STATUS_IO_ACTIVE)			// The printer is in an active input/output state 
			sPrinterStatus += _T("..The printer is in an active input/output state..");
		if (dwPrinterStatus & PRINTER_STATUS_MANUAL_FEED)		// The printer is in a manual feed state. 
			sPrinterStatus += _T("..The printer is in a manual feed state..");
		if (dwPrinterStatus & PRINTER_STATUS_NO_TONER)			// The printer is out of toner. 
			sPrinterStatus += _T("..The printer is out of toner..");
		if (dwPrinterStatus & PRINTER_STATUS_NOT_AVAILABLE)		// The printer is not available for printing.  
			sPrinterStatus += _T("..The printer is not available for printing..");
		if (dwPrinterStatus & PRINTER_STATUS_OFFLINE)			// The printer is offline.  
			sPrinterStatus += _T("..The printer is offline..");
		if (dwPrinterStatus & PRINTER_STATUS_OUT_OF_MEMORY)		// The printer has run out of memory.  
			sPrinterStatus += _T("..The printer has run out of memory..");
		if (dwPrinterStatus & PRINTER_STATUS_OUTPUT_BIN_FULL)	// The printer's output bin is full.  
			sPrinterStatus += _T("..The printer's output bin is full..");
		if (dwPrinterStatus & PRINTER_STATUS_PAGE_PUNT)			// The printer cannot print the current page. 
			sPrinterStatus += _T("..The printer cannot print the current page..");
		if (dwPrinterStatus & PRINTER_STATUS_PAPER_JAM)			// Paper is jammed in the printer  
			sPrinterStatus += _T("..Paper is jammed in the printer..");
		if (dwPrinterStatus & PRINTER_STATUS_PAPER_OUT)			// The printer is out of paper. 
			sPrinterStatus += _T("..The printer is out of paper..");
		if (dwPrinterStatus & PRINTER_STATUS_PAPER_PROBLEM)		// The printer has a paper problem.  
			sPrinterStatus += _T("..The printer has a paper problem..");
		if (dwPrinterStatus & PRINTER_STATUS_PAUSED)			// The printer is paused. 
			sPrinterStatus += _T("..The printer is paused..");
		if (dwPrinterStatus & PRINTER_STATUS_PENDING_DELETION)	// The printer is deleting a print job.  
			sPrinterStatus += _T("..The printer is deleting a print job..");
		if (dwPrinterStatus & PRINTER_STATUS_POWER_SAVE)		// The printer is in power save mode.  
			sPrinterStatus += _T("..The printer is in power save mode..");
		if (dwPrinterStatus & PRINTER_STATUS_PRINTING)			// The printer is printing. 
			sPrinterStatus += _T("..The printer is printing..");
		if (dwPrinterStatus & PRINTER_STATUS_PROCESSING)		// The printer is processing a print job.  
			sPrinterStatus += _T("..The printer is processing a print job..");
		if (dwPrinterStatus & PRINTER_STATUS_SERVER_UNKNOWN)	// The printer status is unknown.  
			sPrinterStatus += _T("..The printer status is unknown..");
		if (dwPrinterStatus & PRINTER_STATUS_TONER_LOW)			// The printer is low on toner. 
			sPrinterStatus += _T("..The printer is low on toner..");
		if (dwPrinterStatus & PRINTER_STATUS_USER_INTERVENTION)	// The printer has an error that requires the user to do something. 
			sPrinterStatus += _T("..The printer has an error that requires the user to do something..");
		if (dwPrinterStatus & PRINTER_STATUS_WAITING)			// The printer is waiting.  
			sPrinterStatus += _T("..The printer is waiting..");
		if (dwPrinterStatus & PRINTER_STATUS_WARMING_UP)		// The printer is warming up. 
			sPrinterStatus += _T("..The printer is warming up..");
	}

	return bResult;
}		 
/////////////////////////////////////////////////////////////////////////////
BOOL CDVClientApp::CopyFile(const CString &sFrom, const CString &sTo)
{
	BOOL bResult = FALSE;
	DWORD dwAttribute = GetFileAttributes(sTo);
	if (dwAttribute != -1)
		SetFileAttributes(sTo, FILE_ATTRIBUTE_NORMAL);
	bResult = ::CopyFile(sFrom, sTo, FALSE);
	if (dwAttribute != -1)
		SetFileAttributes(sTo, dwAttribute);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDVClientApp::IsReceiver()
{
	return m_bReceiver; 
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDVClientApp::IsTransmitter()
{
	return m_bTransmitter;
}
/////////////////////////////////////////////////////////////////////////////
void CDVClientApp::SetAudibleAlarm(BOOL bAlarm)
{
	m_bAudibleAlarm = bAlarm;
	SaveConfiguration();
}
/////////////////////////////////////////////////////////////////////////////
void CDVClientApp::DoAlarmConnection(const CConnectionRecord& c)
{
	if (m_pServer == NULL)
	{
		if (m_pMainWnd)
		{
			WK_TRACE(_T("CDVClientApp::AlarmConnection from <%s>\n"),c.GetSourceHost());
			m_pServer = new CServer(c);
			if (m_pMainWnd->IsIconic())
			{
				m_pMainWnd->ShowWindow(SW_SHOWNORMAL);
			}
			m_pMainWnd->PostMessage(WM_USER,WPARAM_ALARM_CONNECTION);
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("Alarmconnection during established connection\n"));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVClientApp::OnAlarmConnection()
{
	if (m_pCIPCServerControlDVClient)
	{
		for (int i=0;i<100 && (m_pCIPCServerControlDVClient->GetIPCState()==CIPC_STATE_CONNECTED);i++)
		{
			Sleep(10);
		}
	}

	if (GetServer())
	{
		GetServer()->AlarmConnection();
	}
}

/////////////////////////////////////////////////////////////////////////////
//wird gesetzt (aus OnConfirmGetValue() in CIPCDataCarrierClient),
// wenn ACDC.exe gemeldet hat, dass eine Brennsoftware installiert ist
void CDVClientApp::SetBackupWithACDC(BOOL bCanBackup)
{
	m_bCanBackupWithACDC = bCanBackup;

	//Panel aktualisieren
	CPanel* pPanel = theApp.GetPanel();
	if(pPanel)
	{
		pPanel->UpdateMenu();
	}
}


/////////////////////////////////////////////////////////////////////////////
//Liefert TRUE, wenn ein Backup mit ACDC (Nero, XP) möglich ist
BOOL CDVClientApp::CanBackupWithACDC()
{
	return m_bCanBackupWithACDC && (m_pServer->GetDataCarrier() != NULL);
}
/////////////////////////////////////////////////////////////////////////////
//wird vom CDataCarrierClient auf FALSE gesetzt, nachdem festgestellt dass  kein
//Brenner installiert ist
void CDVClientApp::SetACDCBackupDrive(BOOL bDriveAvailable)
{
	m_bIsACDCBackupDrive = bDriveAvailable;

	CMainFrame *pMF = GetMainFrame();
	if(pMF && !m_bIsACDCBackupDrive)
	{
		//falls während des Backups (mittels USB-Brenner) die USB Verbindung unterbrochen wird
		//hier das Backup Fenster schließen
		CBackupWindow *pBW = pMF->GetBackupWindow();
		if(pBW)
		{
			pBW->SetCloseBackupWindow(TRUE, TRUE);
			WK_TRACE(_T("SetACDCBackupDrive(): Backup Drive disappeared, cancel backup\n"));
		}
	}

	//neuen Zustrand an das panel weiterleiten, um das Menu zu aktualisieren (LoadConfigMenu)
	CPanel *pPanel = theApp.GetPanel();
	if(pPanel)
	{
		pPanel->UpdateMenu();
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDVClientApp::CanRealtime()
{
	return m_bRealtime;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDVClientApp::SetRealtime(BOOL bRealtime)
{
	m_bRealtime = bRealtime;
	SaveConfiguration();

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
 CPanel* CDVClientApp::GetPanel()
{
	CWnd *pWnd = m_DVUIThread.GetPanel();
	if (   pWnd									// nessesary
		&& pWnd->m_hWnd							// also but may be too early
		&& pWnd->m_nFlags & WF_CONTINUEMODAL)	// flag is set if wnd is created
	{											// and is removed before destruction
		ASSERT_KINDOF(CPanel, pWnd);			
		return (CPanel*)pWnd;
	}
	WK_TRACE(_T("CDVClientApp::GetPanel() returns NULL\n"));
    return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CMainFrame* CDVClientApp::GetMainFrame()
{
	if (m_pMainWnd && m_pMainWnd->m_hWnd)
	{
		ASSERT_KINDOF(CMainFrame, m_pMainWnd);
		return (CMainFrame*)m_pMainWnd;
	}
	WK_TRACE(_T("CDVClientApp::GetMainFrame() returns NULL\n"));
    return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CIPCAudioDVClient* CDVClientApp::GetAudio()
{
	if (m_bAudioAllowed)
	{
		CServer *pServer = GetServer();

		if (   pServer 
			&& (    pServer->IsLocal() 
			     || m_bReceiver 
			   ) 
			&& pServer->IsAudioConnected())
		{
			return pServer->GetAudio();
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CIPCAudioDVClient* CDVClientApp::GetLocalAudio()
{
	if (m_bAudioAllowed)
	{
		if (m_bReceiver || IsReadOnly())
		{
			if (IsLocalAudioConnected())
			{
				return m_pCIPCLocalAudio;
			}
		}
		else
		{
			return GetAudio();
		}
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
BOOL CDVClientApp::IsLocalAudioConnected()
{
	BOOL bRet = FALSE;
	if (m_pCIPCLocalAudio)
	{
		bRet = (m_pCIPCLocalAudio->GetIPCState() == CIPC_STATE_CONNECTED);
	}
	else
	{
		bRet = FALSE;
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CDVClientApp::ConnectToLocalAudio()
{
	if (m_pCIPCLocalAudio)
	{
		return;	// already connected or created
	}

	if (WK_IS_RUNNING(WK_APP_NAME_AUDIOUNIT1)==FALSE) 
	{
		return; // no chance without the AudioUnit
	}
	CString str;
	str.Format(SM_AudioUnitMedia, 1);
	m_pCIPCLocalAudio = new CIPCAudioDVClient(str, NULL, TRUE);
	Sleep(10);
}
//////////////////////////////////////////////////////////////////////////
bool CDVClientApp::SetTimeOutValue(int nValue /*=-1*/)
{
	return m_RequestThread.SetTimeOutValue(nValue);
}

BOOL CDVClientApp::MessageBeep(UINT nID)
{
	if (m_bUseSpeaker)
	{
		return Beep(440, 100);
	}
	else
	{
		return ::MessageBeep(nID);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Performance Management
void CDVClientApp::CalcCPUusage()
{
	if (NtQuerySystemInformation)
	{
		SYSTEM_PERFORMANCE_INFORMATION SysPerfInfo;
		SYSTEM_TIME_INFORMATION        SysTimeInfo;
		double                         dbIdleTime;
		double                         dbSystemTime;
		LONG                           status;

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

				// CurrentCpuIdle = IdleTime / SystemTime
				dbIdleTime = dbIdleTime / dbSystemTime;

				// CurrentCpuUsage% = 100 - (CurrentCpuIdle * 100) / NumberOfProcessors
				dbIdleTime = 100.0 - dbIdleTime * 100.0 / (double)m_nNoOfProcessors;

				m_dwCPUusage = (DWORD)(dbIdleTime + 0.5);
				m_AvgCPUusage.AddValue(m_dwCPUusage);
				if (m_bShowCPUusage & 8)
				{
					WK_STAT_LOG(_T("PM"), m_dwCPUusage, _T("CPUUsage"));
				}
		}

		// store new CPU's idle and system time
		m_liOldIdleTime = SysPerfInfo.liIdleTime;
		m_liOldSystemTime = SysTimeInfo.liKeSystemTime;
	}
}
/////////////////////////////////////////////////////////////////////////////
// Performance Management
void CDVClientApp::CheckPerformanceLevel()
{
	int nAvgCUPusage = (int)GetAvgCPUusage();
	int nLen = GetMainFrame()->GetLiveWindowQueueLength();
	int nWindows = GetMainFrame()->GetNrOfWindows(DISPLAYWINDOW_LIVE, SHOWABLE);
	if      (nAvgCUPusage > (int)m_wUpperThreshold || nLen > nWindows)
	{
		IncreasePerformanceLevel();
	}
	else if (nAvgCUPusage < (int)m_wLowerThreshold) 
	{
		DecreasePerformanceLevel();
	}
	if (m_bShowCPUusage & 4)
	{
		WK_STAT_LOG(_T("PM"), nAvgCUPusage, _T("AvgCPUUsage"));
	}
	ASSERT(IsBetween(m_nPerformanceLevel, m_nMinPerfLevel, MAX_PERFORMANCE_LEVEL));
}
/////////////////////////////////////////////////////////////////////////////
void CDVClientApp::IncreasePerformanceLevel()
{
	if (m_nPerformanceLevel < MAX_PERFORMANCE_LEVEL)
	{
		m_nPerformanceLevel++;
		GetMainFrame()->PostMessage(WM_USER,MAKELONG(WPARAM_REQUEST,0),RM_PERFORMANCE);
		if (m_bShowCPUusage & 2)
		{
			WK_STAT_LOG(_T("PM"), m_nPerformanceLevel, _T("Level"));
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVClientApp::DecreasePerformanceLevel()
{
	if (m_nPerformanceLevel > m_nMinPerfLevel)
	{
		m_nPerformanceLevel--;
		GetMainFrame()->PostMessage(WM_USER,MAKELONG(WPARAM_REQUEST,0),RM_PERFORMANCE);
		if (m_bShowCPUusage & 2)
		{
			WK_STAT_LOG(_T("PM"), m_nPerformanceLevel, _T("Level"));
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
int CDVClientApp::GetIntValue(LPCTSTR sKey, int nDefault/*=0*/, BOOL bCreate/*=TRUE*/)
{
	CWK_Profile wkp;
	int bIntValue = wkp.GetInt(REGKEY_DVCLIENT, sKey, -1);
	if (bIntValue == -1)
	{
		bIntValue = nDefault;
		if (bCreate)
		{
			wkp.WriteInt(REGKEY_DVCLIENT, sKey, bIntValue);
		}
	}
	return bIntValue;
}
