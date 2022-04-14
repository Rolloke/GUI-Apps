/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: sec3.cpp $
// ARCHIVE:		$Archive: /Project/SecurityServer/sec3.cpp $
// CHECKIN:		$Date: 14.08.06 16:06 $
// VERSION:		$Revision: 434 $
// LAST CHANGE:	$Modtime: 14.08.06 16:04 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "sec3.h"

#include "InputList.h"
#include "OutputList.h"
#include "ProcessMacro.h"

#include "mainfrm.h"
#include "CIPCInputServer.h"

#include "CipcServerControlServerSide.h"
#include "CipcDatabaseClientServer.h"
#include "ProcessSchedulerVideo.h"
#include "ProcessSchedulerAudio.h"
#include "CipcServerControlClientSide.h"

#include "CipcInputServer.h"
#include "CipcOutputServer.h"

#include "ProcessListDoc.h"
#include "ProcessListView.h"
#include "RequestCollectors.h"
#include "Quad.h"
#include ".\sec3.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//#include <wk_new.inl>

#define SERVERDEBUG_CFG  _T("ServerDebug.cfg")
#define TRACE_H263_DATA _T("TraceH263Data")
#define TRACE_JPEG_DATA _T("TraceJpegData")
#define TRACE_VIRTUAL_SECONDS _T("TraceVirtualSeconds")
#define TRACE_SYNC _T("TraceSync")
#define TRACE_DROPPED_REQUEST _T("TraceDroppedRequest")
#define TRACE_ENCODE_REQUESTS _T("TraceEncodeRequests")
#define TRACE_ENCODE_UNIT_REQUESTS _T("TraceEncodeUnitRequests")
#define TRACE_INACTIVE_BY_TIMER _T("TraceInactiveByTimer")
#define TRACE_INACTIVE_BY_LOCK _T("TraceInactiveByLock")
#define TRACE_PROCESSES _T("TraceProcesses")
#define TRACE_PROCESS_TERMINATION _T("TraceProcessTermination")
#define TRACE_PROCESS_RUN _T("TraceProcessRun")
#define TRACE_PROCESS _T("TraceProcess")
#define ADJUST_START_TIME _T("AdjustStartTime")
#define ALLOW_ANY_OVERLAY_CLIENT _T("AllowAnyOverlayClient")
#define TRACE_CLIENT_JPEG_REQUESTS _T("TraceClientJpegRequests")
#define TRACE_REQUEST_OPTIMIZATION _T("TraceRequestOptimization")
#define DISABLE_REQUEST_OPTIMIZATION _T("DisableRequestOptimization")
#define DISABLE_REQUEST_MULTIPLEXING _T("DisableRequestMultiplexing")
#define TRACE_REQUEST_MULTIPLEXING _T("TraceRequestMultiplexing")
#define COCO_CLIENT_PICS_PER_SECOND _T("CoCoClientPicsPerSecond")
#define TRACE_ALARM _T("TraceAlarm")
#define TRACE_MILI _T("TraceMili")
#define ALLOW_GET_REGISTRY _T("AllowGetRegistry")
#define TRACE_CONFIRM_VALUES _T("TraceConfirmValues")
#define TRACE_INDICATE_MEDIA_DATA _T("TraceIndicateMediaData")
#define TRACE_DO_CLIENT_ENCODING _T("TraceDoClientEncoding")
#define TRACE_ON_REQUEST_START_MEDIA_ENCODING _T("TraceOnRequestStartMediaEncoding")
#define TRACE_ON_REQUEST_STOP_MEDIA_ENCODING _T("TraceOnRequestStopMediaEncoding")
#define TRACE_REQUEST_MEDIA_DECODING _T("TraceRequestMediaDecoding")
#define TRACE_ON_REQUEST_STOP_MEDIA_DECODING _T("TraceOnRequestStopMediaDecoding")
#define TRACE_ON_REQUEST_VALUES _T("TraceOnRequestValues")

LPCTSTR NameOfEnum(ApplicationState as)
{
	switch (as)
	{
		NAME_OF_ENUM(AS_STARTED);
		NAME_OF_ENUM(AS_START_RESET);
		NAME_OF_ENUM(AS_START_DATABASE);
		NAME_OF_ENUM(AS_START_ENCODER);
		NAME_OF_ENUM(AS_START_UPLOAD);
		NAME_OF_ENUM(AS_START_OUTPUT);
		NAME_OF_ENUM(AS_SET_MD);
		NAME_OF_ENUM(AS_START_AUDIO);
		NAME_OF_ENUM(AS_START_TIMER);
		NAME_OF_ENUM(AS_START_INPUT);
		NAME_OF_ENUM(AS_RESET_COMPLETE);
		NAME_OF_ENUM(AS_UP_AND_RUNNING);
	default:
		return _T("");
	}
}

/////////////////////////////////////////////////////////////////////////////
static TCHAR BASED_CODE szSection[] = _T("SecurityServer");
static TCHAR BASED_CODE szSectionDebug[] = _T("SecurityServer\\Debug");
static TCHAR BASED_CODE szSectionQuad[] = _T("Quad");

/////////////////////////////////////////////////////////////////////////////
CWK_Timer theTimer;	// the one and only instance


					/*@TOPIC{CSec3App Overview|CSec3App,Overview}
					@RELATED @LINK{members|CSec3App},
*/
/*
@MLIST @RELATED NYD
*/

/////////////////////////////////////////////////////////////////////////////
// CSec3App
BEGIN_MESSAGE_MAP(CSec3App, CWinApp)
	//{{AFX_MSG_MAP(CSec3App)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_EINSTELLUNGEN_SUPERVISOR, OnEinstellungenSupervisor)
	ON_UPDATE_COMMAND_UI(IDS_PANE_NO_UNIT,OnUpdatePaneUnits)
	ON_UPDATE_COMMAND_UI(IDS_PANE_NO_CLIENT,OnUpdatePaneClients)
	ON_UPDATE_COMMAND_UI(IDS_PANE_NO_DB,OnUpdatePaneDb)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
	ON_COMMAND(ID_LOG_PROZESSE, OnLogProcesses)
	ON_COMMAND(ID_LOG_REQUESTS, OnLogRequests)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// The one and only CSec3App object

CSec3App theApp;

/////////////////////////////////////////////////////////////////////////////
// CSec3App construction
/*@MHEAD{constructor:}*/
/*@MD NYD */
CSec3App::CSec3App()
{
	SetState(AS_STARTED);
	XTIB::SetThreadName(_T("MainThread"));

	m_bTraceLocks = FALSE;
#ifdef TRACE_LOCKS
	CWK_Profile wkp;
	m_bTraceLocks = wkp.GetInt(szSectionDebug, _T("TraceLocks"), m_bTraceLocks);
	WK_TRACE(_T("TraceLocks=%d\n"), m_bTraceLocks);

	int nCurrentThread = m_bTraceLocks ? 0 : -1;
	m_Timers.m_nCurrentThread = nCurrentThread;
	m_inputClientsDummy.m_nCurrentThread = nCurrentThread;
	m_outputClientsDummy.m_nCurrentThread = nCurrentThread;

	m_ProcessMacros.m_nCurrentThread = nCurrentThread;
	m_Timers.m_nCurrentThread = nCurrentThread;
	
	// Input Units
	m_InputGroups.m_nCurrentThread = nCurrentThread;
	m_OutputGroups.m_nCurrentThread = nCurrentThread;
	m_AudioGroups.m_nCurrentThread = nCurrentThread;
#endif

	m_bResetPosted = FALSE;
	m_bTraceVeryOld = FALSE;
	m_bTraceTimer = FALSE;
	m_bMpeg4Transmission = TRUE;
	m_bMpeg4Recording = FALSE;
	m_iMaxNrOfClients = 6;

	m_pDatabase = NULL;
	m_pProcessDoc = NULL;
	m_bStoring = FALSE;
	m_bHasAlarmOffSpanFeature = FALSE;

	m_pServerControl	= NULL;
	m_pOutputDummy		= NULL;

	m_tmIamAlive.SetDelta(1000*60*60);

	m_lpSmallTestbild = NULL;
	m_dwSmallTestbildLen = 0;

	m_lpLargeTestbild = NULL;
	m_dwLargeTestbildLen = 0;
	// Kleines Testbild aus der Resource laden
	HRSRC hRc = FindResource(NULL, MAKEINTRESOURCE(IDR_TEST_SMALL), _T("BINARY"));
	if (hRc)
	{
		HGLOBAL hJpegResource	= LoadResource(NULL, hRc);
		if (hJpegResource)
		{
			m_dwSmallTestbildLen	= SizeofResource(NULL, hRc);
			m_lpSmallTestbild		= (char*)LockResource(hJpegResource);
		}
	}


	// Großes Testbild aus der Resource laden
	hRc = FindResource(NULL, MAKEINTRESOURCE(IDR_TEST_LARGE), _T("BINARY"));
	if (hRc)
	{
		HGLOBAL hJpegResource	= LoadResource(NULL, hRc);
		if (hJpegResource)
		{
			m_dwLargeTestbildLen	= SizeofResource(NULL, hRc);
			m_lpLargeTestbild		= (char*)LockResource(hJpegResource);
		}
	}

	m_csLastTimerThreadRunTick.Lock();
	m_dwLastTimerThreadRunTick = WK_GetTickCount();
	m_csLastTimerThreadRunTick.Unlock();

	m_stTime.GetLocalTime();
}
CSec3App::~CSec3App()
{
}

void CSec3App::RegisterWindowClass()
{
	WNDCLASS  wndclass ;
	
	// register window class
	wndclass.style =         0;
	wndclass.lpfnWndProc =   DefWindowProc;
	wndclass.cbClsExtra =    0;
	wndclass.cbWndExtra =    0;
	wndclass.hInstance =     m_hInstance;
	wndclass.hIcon =         LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME));
	wndclass.hCursor =       LoadCursor(IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1) ;
	wndclass.lpszMenuName =  0L;
	wndclass.lpszClassName = WK_APP_NAME_SERVER;
	
	AfxRegisterClass(&wndclass);
}
/////////////////////////////////////////////////////////////////////////////
// CSec3App initialization

/*@MD NYD */
BOOL CSec3App::InitInstance()
{
	if (!WK_ALONE(WK_APP_NAME_SERVER)) 
	{
		return FALSE;
	}

	CString sApp(m_pszHelpFilePath);
	int nFind = sApp.ReverseFind(_T('\\'));
	if (nFind != -1)
	{
		sApp = sApp.Left(nFind+1);
		CString sFile = sApp + REMOTE_REG_IMPORT_BACKUP;
		if (DoesFileExist(sFile))
		{
			CString sExecute = _T("wait:regedt32.exe /s");	// mit Regedt32 /s = Silent
			sExecute += sFile;
			ExecuteProgram(sExecute, sApp);// importieren
			DeleteFile(sFile);
		}
	}

#if _MFC_VER >= 0x0710
	SetLanguageParameters(0, 0);
#endif

	CString sTitle = COemGuiApi::GetApplicationName(WAI_SECURITY_SERVER);
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
	
	InitDebugger(_T("Server.log"), WAI_SECURITY_SERVER);

	// dongle check via applicationId
	CWK_Dongle dongle(WAI_SECURITY_SERVER);	// already sends RunTimeError 
	if (!dongle.IsExeOkay()) 
	{	
		return FALSE;
	}
	m_bHasAlarmOffSpanFeature = dongle.AllowAlarmOffSpans();
	
	RegisterWindowClass();

#if _MFC_VER < 0x700
	Enable3dControls();
#endif

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CProcessListDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CProcessListView));
	AddDocTemplate(pDocTemplate);
	
	// do not touch
	if (m_nCmdShow!=0) // do not touch if already hidden
	{	
		m_nCmdShow = SW_HIDE;
	}
#ifdef _DEBUG
	m_nCmdShow = SW_SHOW;
#endif

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	
	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	
	// make sure status bar is visible
	AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_VIEW_STATUS_BAR);
	
	m_pProcessDoc = (CProcessListDoc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();
	m_pServerControl = new CIPCServerControlServerSide();
	// gf Never set the CodePage to UNICODE, Request will always be coded in ANSI!
	
	// After getting all data, reset System ////////////////////////////////////////
	WK_TRACE(_T("First initial reset...\n"));
	Reset();
	
	return TRUE;
}

/*@MHEAD{destructor:}*/
/*@MD NYD */
int CSec3App::ExitInstance() 
{
	// order is important, client process still need the server control
	if (m_pServerControl) 
	{
		m_pServerControl->Shutdown();
	}

	DeleteAll();

	WK_DELETE(m_pServerControl);
	
	WriteDebugConfiguration();

	WK_TRACE(_T("*** server exit ***\n"));

	CloseDebugger();
	
	return 0L;
}

/////////////////////////////////////////////////////////////////////////////
// CSec3App Functions

/*@MD NYD */
void CSec3App::DeleteAll()
{
	m_State = AS_STARTED;

	m_UploadThread.StopThread();
	m_EncoderThread.StopThread();
	m_TimerThread.StopThread();
	
	ModifyStoringFlag(FALSE);
	WK_STAT_LOG(_T("Reset"),0,_T("ServerActive"));
	WK_STAT_LOG(_T("Reset"),0,_T("ServerAlive"));
	
	// indicate shutdown in all IOgroups
	int i;
	for (i=0;i<m_InputGroups.GetSize();i++) 
	{
		m_InputGroups.GetGroupAt(i)->SetShutdownInProgress();
	}
	Sleep(100);
	for (i=0;i<m_OutputGroups.GetSize();i++) 
	{
		m_OutputGroups.GetGroupAt(i)->SetShutdownInProgress();
	}
	Sleep(100);
	for (i=0;i<m_AudioGroups.GetSize();i++) 
	{
		m_AudioGroups.GetGroupAt(i)->SetShutdownInProgress();
	}
	Sleep(100);
	if (m_pOutputDummy) 
	{
		m_pOutputDummy->SetShutdownInProgress();
	}
	Sleep(100);
	// order is important
	m_AudioGroups.SafeDeleteAll();
	Sleep(100);
	CWK_Profile wkp;
	m_OutputGroups.Save(wkp);
	m_OutputGroups.SafeDeleteAll();
	Sleep(100);
	WK_DELETE(m_pOutputDummy);
	Sleep(100);
	// still some pointers from schedulers to input
	m_InputGroups.SafeDeleteAll();
	Sleep(100);

	WK_DELETE(m_pDatabase);
	WK_STAT_LOG(_T("Reset"),0,_T("DatabaseActive"));

	m_Timers.SafeDeleteAll();
	m_ProcessMacros.DeleteAll();
	m_Hosts.DeleteAll();
	m_Users.DeleteAll();
	m_Permissions.DeleteAll();
}

/*@TOPIC{Server DebugOptions|DebugOptions,Server}
@RELATED @LINK{ConfigOptions|Server,ConfigOptions}

@BW{Protokollwert} ist das was im .log steht.
@LINE@BW{Wert} ist das was im im .cfg eingestellt werden kann.

@LISTHEAD{TraceH263Data}
Protokolliert erhaltene H263Daten pro Kamera,
Protokollwert 1: P-Bild, 
Protokollwert 2: I-Bild.
@LISTHEAD{TraceH263Encode}
Protokolliert, die H263-Anforderungen des Server. Der
Protokollwert entspricht der Anzahl der Bilder.
@LISTHEAD{TraceProcesses}
Protokolliert die Prozesse.
Wert 1 (default), nur An/Aus Wechsel
Wert 2: mit vielen Details. NYD
Wert 99: gar nix.

@LISTHEAD{TraceSync}
Protokolliert die Sync-Zeiten (Dauer einer JobBuendels bei CoCo/MiCo).

@LISTHEAD{AdjustStartTime}
Passt die StartZeit eines Prozesses an, wenn das erste Bild empfangen wird.

@LISTHEAD{TraceProcessTermination}
Protokolliert beim Beenden eines Prozesses wieviele virtuelle (und echte) Sekunden der Prozess erhalten hat 
und wieviele Bilder empfangen wurden.
@LISTHEAD{TraceVirtualSeconds}
Protokolliert die Dauer der virtuellen Sekunden erhalten.

@LISTHEAD{TraceInactiveByTimer}
Protokolliert, wenn ein Prozess nicht gestartet wird,
weil er ausshalb der AktivZeit ist.
@LISTHEAD{TraceInactiveByLock}
Protokolliert, wenn ein Prozess nicht gestartet wird,
weil durch ein Blockschloss blockiert wird.

@REMARK
@CW{TraceMili} wird fuer folgende Faelle automatisch aktiviert:
@ITEM	TraceH263Data,
TraceJpegData,
TraceH263Encode,
TraceEncodeRequests,,

@ALSO @LINK{ServerMain}
*/

/*
@TOPIC{Server ConfigOptions|Server,ConfigOptions}
@LISTHEAD{CoCoClientPicsPerSecond}
Anzahl Bilder pro Sekunde fuer CoCo client Prozesse (default 12).
*/

/*@MHEAD{config:}*/
/*@MD 
@RELATED @LINK{Sever Debug Options|DebugOptions,Server}

Liest die DebugOptions aus @CW{ServerDebug.cfg}. Es wird
erst das aktuelle Verzeichnis versucht, danach @CW{C:\}

Eine Erklaerung der Optionen steht unter
@LINK{Server DebugOptions|DebugOptions,Server}.

*/
void CSec3App::ReadDebugConfiguration()
{
	CWK_Profile wkp;
	CWKDebugOptions debugOptions;
	CString sDebugFile = _T("c:\\ServerDebug.cfg");
	
	// check for C:\ServerDebug first, then in current directory
	BOOL bFoundFile = debugOptions.ReadFromFile(sDebugFile);

	if (bFoundFile)
	{
		DeleteFile(_T("c:\\ServerDebug.cfg"));
	}
	else
	{
		sDebugFile = SERVERDEBUG_CFG;
		bFoundFile = debugOptions.ReadFromFile(SERVERDEBUG_CFG);
		DeleteFile(SERVERDEBUG_CFG);
	}


	if (bFoundFile) 
	{
		// old fashioned config file
		WK_TRACE(_T("ConfigFile %s found\n"),(LPCTSTR)sDebugFile);
		CProcessSchedulerVideo::m_iTraceH263Data = debugOptions.GetValue(TRACE_H263_DATA);	// documented
		CProcessSchedulerVideo::m_bTraceJpegData = debugOptions.GetValue(TRACE_JPEG_DATA);
		CProcessSchedulerVideo::m_bTraceSync = debugOptions.GetValue(TRACE_SYNC);

		CProcessScheduler::m_bTraceVirtualSeconds = debugOptions.GetValue(TRACE_VIRTUAL_SECONDS);	// documented
		CProcessScheduler::m_bTraceEncodeRequests= debugOptions.GetValue(TRACE_ENCODE_REQUESTS);
		CProcessScheduler::m_bTraceEncodeUnitRequests= debugOptions.GetValue(TRACE_ENCODE_UNIT_REQUESTS);
		CProcessScheduler::m_bTraceInactiveByTimer = debugOptions.GetValue(TRACE_INACTIVE_BY_TIMER,0);
		CProcessScheduler::m_bTraceInactiveByLock= debugOptions.GetValue(TRACE_INACTIVE_BY_LOCK,0);
		
		
		CProcess::m_iTraceLevel = debugOptions.GetValue(TRACE_PROCESSES);	// documented
		CProcess::m_bTraceProcessTermination = debugOptions.GetValue(TRACE_PROCESS_TERMINATION);	// documented
		CProcess::m_bTraceProcessRun= debugOptions.GetValue(TRACE_PROCESS_RUN,0);
		CProcess::m_bAdjustStartTime = debugOptions.GetValue(ADJUST_START_TIME);	// documented
		
		CIPCOutputServerClient::m_bAllowAnyOverlayClient=debugOptions.GetValue(ALLOW_ANY_OVERLAY_CLIENT);
		CIPCOutputServerClient::m_bTraceClientJpegRequests = debugOptions.GetValue(TRACE_CLIENT_JPEG_REQUESTS);
		
		CRequestCollectors::ms_bTraceRequestOptimization = debugOptions.GetValue(TRACE_REQUEST_OPTIMIZATION,CRequestCollectors::ms_bTraceRequestOptimization);
		CRequestCollectors::ms_bDisableRequestOptimization=debugOptions.GetValue(DISABLE_REQUEST_OPTIMIZATION,0);
		CRequestCollectors::ms_bDisableRequestMultiplexing=debugOptions.GetValue(DISABLE_REQUEST_MULTIPLEXING,0);
		CRequestCollectors::ms_bTraceRequestMultiplexing=debugOptions.GetValue(TRACE_REQUEST_MULTIPLEXING,0);
		
		CIPCOutputServerClient::m_iCoCoClientPicsPerSecond=debugOptions.GetValue(COCO_CLIENT_PICS_PER_SECOND,1000);
		CInput::m_bTraceAlarms = debugOptions.GetValue(TRACE_ALARM,CInput::m_bTraceAlarms);
		CProcess::m_bTraceProcesses = debugOptions.GetValue(TRACE_PROCESS,CProcess::m_bTraceProcesses);
	#ifdef _DEBUG
		CIPCOutputServerClient::m_iCoCoClientPicsPerSecond = 12;
	#endif
		m_iTraceMili = debugOptions.GetValue(TRACE_MILI,0);

		CIPCOutputServer::m_iMicoFPS = debugOptions.GetValue(_T("MiCoFPS"),0);
		CIPCOutputServer::m_iSaVicFPS = debugOptions.GetValue(_T("SaVicFPS"),0);
		CIPCOutputServer::m_iCocoFPS = debugOptions.GetValue(_T("CoCoFPS"),12);
		CIPCInputFileUpdate::m_bAllowGetRegistry = debugOptions.GetValue(ALLOW_GET_REGISTRY,TRUE);
		
		CIPCAudioServer::gm_bTraceConfirmValues     = debugOptions.GetValue(TRACE_CONFIRM_VALUES);
		CIPCAudioServer::gm_bTraceIndicateMediaData = debugOptions.GetValue(TRACE_INDICATE_MEDIA_DATA);

		CIPCAudioServerClient::gm_bTraceDoClientEncoding           = debugOptions.GetValue(TRACE_DO_CLIENT_ENCODING);
		CIPCAudioServerClient::gm_bTraceOnRequestStartMediaEncoding= debugOptions.GetValue(TRACE_ON_REQUEST_START_MEDIA_ENCODING);
		CIPCAudioServerClient::gm_bTraceOnRequestStopMediaEncoding = debugOptions.GetValue(TRACE_ON_REQUEST_STOP_MEDIA_ENCODING);
		CIPCAudioServerClient::gm_bTraceRequestMediaDecoding       = debugOptions.GetValue(TRACE_REQUEST_MEDIA_DECODING);
		CIPCAudioServerClient::gm_bTraceOnRequestStopMediaDecoding = debugOptions.GetValue(TRACE_ON_REQUEST_STOP_MEDIA_DECODING);
		CIPCAudioServerClient::gm_bTraceOnRequestValues            = debugOptions.GetValue(TRACE_ON_REQUEST_VALUES);
	}
	else
	{
		// new modern registry
		CProcessSchedulerVideo::m_iTraceH263Data = wkp.GetInt(szSectionDebug,TRACE_H263_DATA,CProcessSchedulerVideo::m_iTraceH263Data);
		CProcessSchedulerVideo::m_bTraceJpegData = wkp.GetInt(szSectionDebug,TRACE_JPEG_DATA,CProcessSchedulerVideo::m_bTraceJpegData);
		CProcessSchedulerVideo::m_bTraceSync= wkp.GetInt(szSectionDebug,TRACE_SYNC,CProcessSchedulerVideo::m_bTraceSync);
		CProcessSchedulerVideo::ms_bTraceDroppedRequest = wkp.GetInt(szSectionDebug,TRACE_DROPPED_REQUEST,CProcessSchedulerVideo::ms_bTraceDroppedRequest);
		
		CProcessScheduler::m_bTraceVirtualSeconds= wkp.GetInt(szSectionDebug,TRACE_VIRTUAL_SECONDS,CProcessSchedulerVideo::m_bTraceVirtualSeconds);
		CProcessScheduler::m_bTraceEncodeRequests= wkp.GetInt(szSectionDebug,TRACE_ENCODE_REQUESTS,CProcessScheduler::m_bTraceEncodeRequests);
		CProcessScheduler::m_bTraceEncodeUnitRequests= wkp.GetInt(szSectionDebug,TRACE_ENCODE_UNIT_REQUESTS,CProcessScheduler::m_bTraceEncodeUnitRequests);
		CProcessScheduler::m_bTraceInactiveByTimer= wkp.GetInt(szSectionDebug,TRACE_INACTIVE_BY_TIMER,CProcessScheduler::m_bTraceInactiveByTimer);
		CProcessScheduler::m_bTraceInactiveByLock= wkp.GetInt(szSectionDebug,TRACE_INACTIVE_BY_LOCK,CProcessScheduler::m_bTraceInactiveByLock);
		CProcessScheduler::m_bTraceVideoout= wkp.GetInt(szSectionDebug,_T("TraceVideoout"),CProcessScheduler::m_bTraceVideoout);

		CProcess::m_bTraceProcesses= wkp.GetInt(szSectionDebug,TRACE_PROCESS,CProcess::m_bTraceProcesses);
		CProcess::m_iTraceLevel= wkp.GetInt(szSectionDebug,TRACE_PROCESSES,CProcess::m_iTraceLevel);
		CProcess::m_bTraceProcessTermination= wkp.GetInt(szSectionDebug,TRACE_PROCESS_TERMINATION,CProcess::m_bTraceProcessTermination);
		CProcess::m_bTraceProcessRun= wkp.GetInt(szSectionDebug,TRACE_PROCESS_RUN,CProcess::m_bTraceProcessRun);
		CProcess::m_bAdjustStartTime= wkp.GetInt(szSectionDebug,ADJUST_START_TIME,CProcess::m_bAdjustStartTime);
		
		CIPCOutputServerClient::m_bAllowAnyOverlayClient   = wkp.GetInt(szSectionDebug,ALLOW_ANY_OVERLAY_CLIENT,CIPCOutputServerClient::m_bAllowAnyOverlayClient);
		CIPCOutputServerClient::m_bTraceClientJpegRequests = wkp.GetInt(szSectionDebug,TRACE_CLIENT_JPEG_REQUESTS,CIPCOutputServerClient::m_bTraceClientJpegRequests);
		
		CRequestCollectors::ms_bTraceRequestOptimization   = wkp.GetInt(szSectionDebug,TRACE_REQUEST_OPTIMIZATION,CRequestCollectors::ms_bTraceRequestOptimization);
		CRequestCollectors::ms_bDisableRequestOptimization = wkp.GetInt(szSectionDebug,DISABLE_REQUEST_OPTIMIZATION,CRequestCollectors::ms_bDisableRequestOptimization);
		CRequestCollectors::ms_bDisableRequestMultiplexing = wkp.GetInt(szSectionDebug,DISABLE_REQUEST_MULTIPLEXING,CRequestCollectors::ms_bDisableRequestMultiplexing);
		CRequestCollectors::ms_bTraceRequestMultiplexing   = wkp.GetInt(szSectionDebug,TRACE_REQUEST_MULTIPLEXING,CRequestCollectors::ms_bTraceRequestMultiplexing);

		CIPCOutputServerClient::m_iCoCoClientPicsPerSecond = wkp.GetInt(szSectionDebug,COCO_CLIENT_PICS_PER_SECOND,CIPCOutputServerClient::m_iCoCoClientPicsPerSecond);
		CInput::m_bTraceAlarms = wkp.GetInt(szSectionDebug,TRACE_ALARM,CInput::m_bTraceAlarms);
		m_iTraceMili = wkp.GetInt(szSectionDebug,TRACE_MILI,0);

		CIPCOutputServer::m_iMicoFPS= wkp.GetInt(szSectionDebug,_T("MiCoFPS"),CIPCOutputServer::m_iMicoFPS);
		CIPCOutputServer::m_iSaVicFPS= wkp.GetInt(szSectionDebug,_T("SaVicFPS"),CIPCOutputServer::m_iSaVicFPS);
		CIPCOutputServer::m_iCocoFPS= wkp.GetInt(szSectionDebug,_T("CoCoFPS"),CIPCOutputServer::m_iCocoFPS);
		CIPCInputFileUpdate::m_bAllowGetRegistry= wkp.GetInt(szSectionDebug,ALLOW_GET_REGISTRY,CIPCInputFileUpdate::m_bAllowGetRegistry);
		m_bTraceTimer = wkp.GetInt(szSectionDebug, _T("TraceTimer"), m_bTraceTimer);

		CIPCAudioServer::gm_bTraceConfirmValues     = wkp.GetInt(szSectionDebug, TRACE_CONFIRM_VALUES, CIPCAudioServer::gm_bTraceConfirmValues);
		CIPCAudioServer::gm_bTraceIndicateMediaData = wkp.GetInt(szSectionDebug, TRACE_INDICATE_MEDIA_DATA, CIPCAudioServer::gm_bTraceIndicateMediaData);

		CIPCAudioServerClient::gm_bTraceDoClientEncoding           = wkp.GetInt(szSectionDebug, TRACE_DO_CLIENT_ENCODING          , CIPCAudioServerClient::gm_bTraceDoClientEncoding);
		CIPCAudioServerClient::gm_bTraceOnRequestStartMediaEncoding= wkp.GetInt(szSectionDebug, TRACE_ON_REQUEST_START_MEDIA_ENCODING, CIPCAudioServerClient::gm_bTraceOnRequestStartMediaEncoding);
		CIPCAudioServerClient::gm_bTraceOnRequestStopMediaEncoding = wkp.GetInt(szSectionDebug, TRACE_ON_REQUEST_STOP_MEDIA_ENCODING, CIPCAudioServerClient::gm_bTraceOnRequestStopMediaEncoding);
		CIPCAudioServerClient::gm_bTraceRequestMediaDecoding       = wkp.GetInt(szSectionDebug, TRACE_REQUEST_MEDIA_DECODING      , CIPCAudioServerClient::gm_bTraceRequestMediaDecoding);
		CIPCAudioServerClient::gm_bTraceOnRequestStopMediaDecoding = wkp.GetInt(szSectionDebug, TRACE_ON_REQUEST_STOP_MEDIA_DECODING, CIPCAudioServerClient::gm_bTraceOnRequestStopMediaDecoding);
		CIPCAudioServerClient::gm_bTraceOnRequestValues            = wkp.GetInt(szSectionDebug, TRACE_ON_REQUEST_VALUES           , CIPCAudioServerClient::gm_bTraceOnRequestValues);

		CQuad::m_bTraceQuad = wkp.GetInt(szSectionDebug, _T("TraceQuad"), CQuad::m_bTraceQuad);
		CUploadThread::m_bTraceFTP = wkp.GetInt(szSectionDebug, _T("TraceFTP"), CUploadThread::m_bTraceFTP);

		m_bMpeg4Transmission = wkp.GetInt(szSectionDebug,_T("Mpeg4Transmission"), m_bMpeg4Transmission);
		m_bMpeg4Recording = wkp.GetInt(szSectionDebug,_T("Mpeg4Recording"), m_bMpeg4Recording);
		m_iMaxNrOfClients = wkp.GetInt(szSection,_T("MaxNrOfClients"),m_iMaxNrOfClients);

		CAVCodec enc;
		BOOL bAVCodec = enc.IsLibAVCodecAvailable();
		BOOL bUseAVCodec = wkp.GetInt(szSectionDebug,_T("UseAVCodec"),CEncoderThread::m_bUseAVCodec);
		CEncoderThread::m_bUseAVCodec = bAVCodec && bUseAVCodec;
	}

	// trace the settings
	WK_TRACE(_T("TraceH263Data=%d\n"),CProcessSchedulerVideo::m_iTraceH263Data);
	WK_TRACE(_T("TraceJpegData=%d\n"),CProcessSchedulerVideo::m_bTraceJpegData);
	WK_TRACE(_T("TraceEncodeRequests=%d\n"),CProcessScheduler::m_bTraceEncodeRequests);
	WK_TRACE(_T("TraceEncodeUnitRequests=%d\n"),CProcessScheduler::m_bTraceEncodeUnitRequests);
	WK_TRACE(_T("TraceInactiveByTimer=%d\n"),CProcessScheduler::m_bTraceInactiveByTimer);
	WK_TRACE(_T("TraceInactiveByLock=%d\n"),CProcessScheduler::m_bTraceInactiveByLock);
	WK_TRACE(_T("TraceVirtualSeconds=%d\n"),CProcessSchedulerVideo::m_bTraceVirtualSeconds);
	WK_TRACE(_T("TraceSync=%d\n"),CProcessSchedulerVideo::m_bTraceSync);
	WK_TRACE(_T("TraceDroppedRequest=%d\n"),CProcessSchedulerVideo::ms_bTraceDroppedRequest);
	
	WK_TRACE(_T("TraceProcess=%d\n"),CProcess::m_bTraceProcesses);
	WK_TRACE(_T("TraceProcesses=%d\n"),CProcess::m_iTraceLevel);
	WK_TRACE(_T("TraceProcessTermination=%d\n"),CProcess::m_bTraceProcessTermination);
	WK_TRACE(_T("TraceProcessRun=%d\n"),CProcess::m_bTraceProcessRun);
	WK_TRACE(_T("AdjustStartTime=%d\n"),CProcess::m_bAdjustStartTime);
	
	WK_TRACE(_T("AllowAnyOverlayClient=%d\n"),CIPCOutputServerClient::m_bAllowAnyOverlayClient);
	WK_TRACE(_T("TraceClientJpegRequests=%d\n"),CIPCOutputServerClient::m_bTraceClientJpegRequests);
	
	WK_TRACE(_T("TraceRequestOptimization=%d\n"),CRequestCollectors::ms_bTraceRequestOptimization);
	WK_TRACE(_T("DisableRequestOptimization=%d\n"),CRequestCollectors::ms_bDisableRequestOptimization);
	WK_TRACE(_T("DisableRequestMultiplexing=%d\n"),CRequestCollectors::ms_bDisableRequestMultiplexing);
	WK_TRACE(_T("TraceRequestMultiplexing=%d\n"),CRequestCollectors::ms_bTraceRequestMultiplexing);


	WK_TRACE(_T("CoCoClientPicsPerSecond=%d\n"),CIPCOutputServerClient::m_iCoCoClientPicsPerSecond);
	WK_TRACE(_T("TraceAlarm=%d\n"),CInput::m_bTraceAlarms);
	WK_TRACE(_T("TraceMilli=%d\n"),0);

	WK_TRACE(_T("JaCobFPS=%d\n"),CIPCOutputServer::m_iMicoFPS);
	WK_TRACE(_T("SaVicFPS=%d\n"),CIPCOutputServer::m_iSaVicFPS);
	WK_TRACE(_T("CoCoFPS=%d\n"),CIPCOutputServer::m_iCocoFPS);
	WK_TRACE(_T("AllowGetRegistry=%d\n"),CIPCInputFileUpdate::m_bAllowGetRegistry);
	WK_TRACE(_T("TraceTimer=%d\n"), m_bTraceTimer);
	WK_TRACE(_T("TraceQuad=%d\n"), CQuad::m_bTraceQuad);
	WK_TRACE(_T("TraceVideoOut=%d\n"), CProcessScheduler::m_bTraceVideoout);
	WK_TRACE(_T("TraceFTP=%d\n"), CUploadThread::m_bTraceFTP);


	WK_TRACE(_T("Mpeg4Recording=%d\n"), m_bMpeg4Recording);
	WK_TRACE(_T("Mpeg4Transmission=%d\n"), m_bMpeg4Transmission);
	WK_TRACE(_T("MaxNrOfClients=%d\n"), m_iMaxNrOfClients);
	WK_TRACE(_T("UseAVCodec=%d\n"), CEncoderThread::m_bUseAVCodec);

	if (CIPCOutputServerClient::m_iCoCoClientPicsPerSecond<=0) 
	{
		CIPCOutputServerClient::m_iCoCoClientPicsPerSecond=1000;
	}
	if (m_iTraceMili==0) 
	{
		m_iTraceMili = debugOptions.GetValue(_T("TraceMilli"),0);
	}
	
	// FPS options, new 14.21.99, 0 as default
	if (CIPCOutputServer::m_iMicoFPS) {	// not 0 is for testing, else the registry is read
		WK_TRACE(_T("MiCoFPS=%d\n"),CIPCOutputServer::m_iMicoFPS);
	}
	if (CIPCOutputServer::m_iSaVicFPS) {	// not 0 is for testing, else the registry is read
		WK_TRACE(_T("SaVicFPS=%d\n"),CIPCOutputServer::m_iSaVicFPS);
	}
	if (CIPCOutputServer::m_iCocoFPS!=12) {
		WK_TRACE(_T("CoCoFPS=%d\n"),CIPCOutputServer::m_iCocoFPS);
	}
	
	if (!CIPCInputFileUpdate::m_bAllowGetRegistry) 
	{
		WK_TRACE(_T("AllowGetRegistry=%d\n"),CIPCInputFileUpdate::m_bAllowGetRegistry);
	}

		  
		  
	// NEW 120397 automatic TraceMili for certain debug options
	// NOT YET all reasonable options hit ?
	if ( 
		   CProcessSchedulerVideo::m_iTraceH263Data
		|| CProcessSchedulerVideo::m_bTraceJpegData
		|| CProcessScheduler::m_bTraceEncodeRequests
		|| CProcessScheduler::m_bTraceEncodeUnitRequests
		|| CProcessSchedulerVideo::m_bTraceSync
		) 
	{
		m_iTraceMili=1;
	}

}
void CSec3App::WriteDebugConfiguration()
{
	CWK_Profile wkp;
	
	wkp.WriteInt(szSectionDebug,TRACE_H263_DATA,CProcessSchedulerVideo::m_iTraceH263Data);
	wkp.WriteInt(szSectionDebug,TRACE_JPEG_DATA,CProcessSchedulerVideo::m_bTraceJpegData);
	wkp.WriteInt(szSectionDebug,TRACE_ENCODE_REQUESTS,CProcessScheduler::m_bTraceEncodeRequests);
	wkp.WriteInt(szSectionDebug,TRACE_ENCODE_UNIT_REQUESTS,CProcessScheduler::m_bTraceEncodeUnitRequests);
	
	wkp.WriteInt(szSectionDebug,TRACE_INACTIVE_BY_TIMER,CProcessScheduler::m_bTraceInactiveByTimer);
	wkp.WriteInt(szSectionDebug,TRACE_INACTIVE_BY_LOCK,CProcessScheduler::m_bTraceInactiveByLock);
	wkp.WriteInt(szSectionDebug,TRACE_DROPPED_REQUEST,CProcessSchedulerVideo::ms_bTraceDroppedRequest);
	wkp.WriteInt(szSectionDebug,TRACE_VIRTUAL_SECONDS,CProcessSchedulerVideo::m_bTraceVirtualSeconds);
	wkp.WriteInt(szSectionDebug,_T("TraceVideoout"),CProcessScheduler::m_bTraceVideoout);
	
	wkp.WriteInt(szSectionDebug,TRACE_SYNC,CProcessSchedulerVideo::m_bTraceSync);
	
	wkp.WriteInt(szSectionDebug,TRACE_PROCESS,CProcess::m_bTraceProcesses);
	wkp.WriteInt(szSectionDebug,TRACE_PROCESSES,CProcess::m_iTraceLevel);
	wkp.WriteInt(szSectionDebug,TRACE_PROCESS_TERMINATION,CProcess::m_bTraceProcessTermination);
	wkp.WriteInt(szSectionDebug,TRACE_PROCESS_RUN,CProcess::m_bTraceProcessRun);
	wkp.WriteInt(szSectionDebug,ADJUST_START_TIME,CProcess::m_bAdjustStartTime);
	
	wkp.WriteInt(szSectionDebug,ALLOW_ANY_OVERLAY_CLIENT,CIPCOutputServerClient::m_bAllowAnyOverlayClient);
	wkp.WriteInt(szSectionDebug,TRACE_CLIENT_JPEG_REQUESTS,CIPCOutputServerClient::m_bTraceClientJpegRequests);
	
	wkp.WriteInt(szSectionDebug,TRACE_REQUEST_OPTIMIZATION,CRequestCollectors::ms_bTraceRequestOptimization);
	wkp.WriteInt(szSectionDebug,DISABLE_REQUEST_OPTIMIZATION,CRequestCollectors::ms_bDisableRequestOptimization);
	wkp.WriteInt(szSectionDebug,DISABLE_REQUEST_MULTIPLEXING,CRequestCollectors::ms_bDisableRequestMultiplexing);
	wkp.WriteInt(szSectionDebug,TRACE_REQUEST_MULTIPLEXING,CRequestCollectors::ms_bTraceRequestMultiplexing);

	wkp.WriteInt(szSectionDebug,COCO_CLIENT_PICS_PER_SECOND,CIPCOutputServerClient::m_iCoCoClientPicsPerSecond);

	wkp.WriteInt(szSectionDebug,TRACE_ALARM,CInput::m_bTraceAlarms);
	wkp.WriteInt(szSectionDebug,TRACE_MILI,m_iTraceMili);


	wkp.WriteInt(szSectionDebug,_T("MiCoFPS"),CIPCOutputServer::m_iMicoFPS);
	wkp.WriteInt(szSectionDebug,_T("SaVicFPS"),CIPCOutputServer::m_iSaVicFPS);
	wkp.WriteInt(szSectionDebug,_T("CoCoFPS"),CIPCOutputServer::m_iCocoFPS);
	wkp.WriteInt(szSectionDebug,ALLOW_GET_REGISTRY,CIPCInputFileUpdate::m_bAllowGetRegistry);
	wkp.WriteInt(szSectionDebug, _T("TraceTimer"), m_bTraceTimer);
	wkp.WriteInt(szSectionDebug, _T("TraceLocks"), m_bTraceLocks);

	wkp.WriteInt(szSectionDebug, TRACE_CONFIRM_VALUES, CIPCAudioServer::gm_bTraceConfirmValues);
	wkp.WriteInt(szSectionDebug, TRACE_INDICATE_MEDIA_DATA, CIPCAudioServer::gm_bTraceIndicateMediaData);

	wkp.WriteInt(szSectionDebug, TRACE_DO_CLIENT_ENCODING           , CIPCAudioServerClient::gm_bTraceDoClientEncoding);
	wkp.WriteInt(szSectionDebug, TRACE_ON_REQUEST_START_MEDIA_ENCODING, CIPCAudioServerClient::gm_bTraceOnRequestStartMediaEncoding);
	wkp.WriteInt(szSectionDebug, TRACE_ON_REQUEST_STOP_MEDIA_ENCODING , CIPCAudioServerClient::gm_bTraceOnRequestStopMediaEncoding);
	wkp.WriteInt(szSectionDebug, TRACE_REQUEST_MEDIA_DECODING       , CIPCAudioServerClient::gm_bTraceRequestMediaDecoding);
	wkp.WriteInt(szSectionDebug, TRACE_ON_REQUEST_STOP_MEDIA_DECODING , CIPCAudioServerClient::gm_bTraceOnRequestStopMediaDecoding);
	wkp.WriteInt(szSectionDebug, TRACE_ON_REQUEST_VALUES            , CIPCAudioServerClient::gm_bTraceOnRequestValues);

	wkp.WriteInt(szSectionDebug, _T("TraceQuad"), CQuad::m_bTraceQuad);
	wkp.WriteInt(szSectionDebug, _T("TraceFTP"), CUploadThread::m_bTraceFTP);
	wkp.WriteInt(szSection,_T("MaxNrOfClients"),m_iMaxNrOfClients);

	wkp.WriteInt(szSectionDebug,_T("Mpeg4Transmission"), m_bMpeg4Transmission);
	wkp.WriteInt(szSectionDebug,_T("Mpeg4Recording"), m_bMpeg4Recording);
	wkp.WriteInt(szSectionDebug,_T("UseAVCodec"), CEncoderThread::m_bUseAVCodec);
}


// util
static void DoDisplay(LPCTSTR sText)
{
	  CString sMsg;
	  sMsg.Format(_T("%s"),sText);
	  WK_TRACE(_T("%s\n"),(LPCTSTR)sMsg);
	  UpdateProcessListView(sMsg);
}
/*@MD NYD */
void CSec3App::DisplayDebugOptions()
{
	  CString sExe;
	  GetModuleFileName(m_hInstance,sExe.GetBufferSetLength(_MAX_PATH),_MAX_PATH);
	  sExe.ReleaseBuffer();
	  UpdateProcessListView(_T("Version: ")+WK_GetFileVersion(sExe));
	  
	  CString sMsg;
	  
	  if (m_iTraceMili) {
		  DoDisplay(_T("TraceMilli"));
	  }
	  
	  if (CProcess::m_bTraceProcessTermination) {
		  DoDisplay(TRACE_PROCESS_TERMINATION);
	  }
	  if (CProcess::m_bTraceProcessRun) {
		  DoDisplay(TRACE_PROCESS_RUN);
	  }
	  
	  if (CProcess::m_bAdjustStartTime) {
		  DoDisplay(ADJUST_START_TIME);
	  }
	  if (CProcess::m_bTraceProcesses) {
		  DoDisplay(TRACE_PROCESS);
	  }
	  
	  if (CProcessSchedulerVideo::m_bTraceSync ) {
		  DoDisplay(TRACE_SYNC);
	  }
	  
	  if (CProcessScheduler::m_bTraceEncodeRequests) {
		  DoDisplay(TRACE_ENCODE_REQUESTS);
	  }
	  if (CProcessScheduler::m_bTraceEncodeUnitRequests){
		  DoDisplay(TRACE_ENCODE_UNIT_REQUESTS);
	  }
	  if (CRequestCollectors::ms_bTraceRequestOptimization) {
		  DoDisplay(TRACE_REQUEST_OPTIMIZATION);
	  }
	  
	  if (CProcessSchedulerVideo::m_bTraceJpegData) {
		  DoDisplay(TRACE_JPEG_DATA);	// NOT YET detail
	  }
	  if (CProcessSchedulerVideo::m_iTraceH263Data) {
		  DoDisplay(TRACE_H263_DATA);	// NOT YET detail
	  }
	  if (CProcessScheduler::m_bTraceInactiveByTimer) {
		  DoDisplay(TRACE_INACTIVE_BY_TIMER);
	  }
	  if (CProcessScheduler::m_bTraceInactiveByLock) {
		  DoDisplay(TRACE_INACTIVE_BY_LOCK);
	  }
	  if (CProcessScheduler::m_bTraceVirtualSeconds) {
		  DoDisplay(TRACE_VIRTUAL_SECONDS);
	  }
	  if (CProcessSchedulerVideo::m_bTracePictDataWithoutProcess) {
		  DoDisplay(_T("PictDataWithoutProcess"));
	  }
	  if (CProcessSchedulerVideo::ms_bTraceDroppedRequest==TRUE) {
		  DoDisplay(TRACE_DROPPED_REQUEST);
	  }
	  if (CInput::m_bTraceAlarms) {
		  DoDisplay(TRACE_ALARM);
	  }
	  
	  if (CIPCOutputServerClient::m_bAllowAnyOverlayClient) {
		  DoDisplay(ALLOW_ANY_OVERLAY_CLIENT);	// NOT YET detail
	  }
	  
	  if (CIPCOutputServerClient::m_bTraceClientJpegRequests) {
		  DoDisplay(TRACE_CLIENT_JPEG_REQUESTS);
	  }
	  
	  if (CIPCOutputServerClient::m_iCoCoClientPicsPerSecond!=100) {
		  sMsg.Format(_T("CoCoClientPicsPerSecond=%d"),CIPCOutputServerClient::m_iCoCoClientPicsPerSecond);
		  DoDisplay(sMsg);
	  }
	  
	  if (CRequestCollectors::ms_bDisableRequestOptimization==TRUE) {
		  DoDisplay(DISABLE_REQUEST_OPTIMIZATION);
	  }
	  
	  if (CRequestCollectors::ms_bDisableRequestMultiplexing==TRUE) {
		  DoDisplay(DISABLE_REQUEST_MULTIPLEXING);
	  }
	  if (CRequestCollectors::ms_bTraceRequestMultiplexing==TRUE) {
		  DoDisplay(TRACE_REQUEST_MULTIPLEXING);
	  }
	  
	  
	  if (CIPCOutputServer::m_iMicoFPS) { // not 0 is for testing, else the registry is read
		  sMsg.Format(_T("MiCoFPS=%d"),CIPCOutputServer::m_iMicoFPS);
		  DoDisplay(sMsg);
	  }
	  if (CIPCOutputServer::m_iSaVicFPS) {	// not 0 is for testing, else the registry is read
		  sMsg.Format(_T("SaVicFPS=%d"),CIPCOutputServer::m_iSaVicFPS);
		  DoDisplay(sMsg);
	  }
	  if (CIPCOutputServer::m_iCocoFPS!=12) {
		  sMsg.Format(_T("CoCoFPS=%d"),CIPCOutputServer::m_iCocoFPS);
		  DoDisplay(sMsg);
	  }
	  if (theApp.HasAlarmOffSpanFeature())
	  {
		  DoDisplay(_T("Alarm Off Span"));
	  }
		  
}

//////////////////////////////////////////////////////////////////////////////////
void CSec3App::ResetHosts(CWK_Profile &wkp)
{
	CHostArray newHosts;
	CHost* pOldHost = NULL;
	CHost* pNewHost = NULL;
	
	newHosts.Load(wkp);
	
	for (int i=0;i<m_Hosts.GetSize();i++)
	{
		pOldHost = m_Hosts.GetAtFast(i);
		pNewHost = newHosts.GetHost(pOldHost->GetID());
		if (pNewHost == NULL)
		{
			WK_TRACE(_T("deleting temp www dir for host %s\n"),pOldHost->GetName());
			// a deleted host
			CString sDir;
			sDir.Format(_T("%s\\%08lx"),
				CNotificationMessage::GetWWWRoot(),
				pOldHost->GetID().GetID());
			DeleteDirRecursiv(sDir);
		}
	}
	
	m_Hosts.Load(wkp);
}
//////////////////////////////////////////////////////////////
void CSec3App::InitTickCount()
{
	if (m_iTraceMili) 
	{
		if (m_iTraceMili<0) 
		{
			SET_WK_STAT_TICK_COUNT(WK_GetTickCount());
		} 
		else 
		{
			SET_WK_STAT_TICK_COUNT(m_iTraceMili);
		}
	} 
	else 
	{
		SET_WK_STAT_TICK_COUNT(0);
	}
}
//////////////////////////////////////////////////////////////
void CSec3App::DoReset()
{
	// called from FetchServerReset
	if (WK_IS_WINDOW(m_pMainWnd))
	{
		if (IsUpAndRunning())
		{
			WK_TRACE(_T("RESET posted...\n"));
			m_pMainWnd->PostMessage(WM_COMMAND,ID_EINSTELLUNGEN_SUPERVISOR);
		}
	}
}
//////////////////////////////////////////////////////////////
void CSec3App::EmergencyReset()
{
	// called by myself
	if (!m_bResetPosted)
	{
		if (WK_IS_WINDOW(m_pMainWnd))
		{
			if (IsUpAndRunning())
			{
				WK_TRACE(_T("RESET posted...\n"));
				m_bResetPosted = TRUE;
				m_pMainWnd->PostMessage(WM_COMMAND,ID_EINSTELLUNGEN_SUPERVISOR);
			}
		}
	}
}
//////////////////////////////////////////////////////////////
void CSec3App::AddProcessListViewItem(const CString& sLine)
{
	CListCtrl &list = m_pProcessDoc->GetView()->GetListCtrl();
	list.InsertItem(list.GetItemCount(),sLine);
	list.Update(list.GetItemCount()-1);
}
//////////////////////////////////////////////////////////////
void CSec3App::ClearProcessListView()
{
	CListCtrl &list = m_pProcessDoc->GetView()->GetListCtrl();
	list.DeleteAllItems();
}
//////////////////////////////////////////////////////////////
void CSec3App::Reset()
{
	WK_TRACE(_T("RESETTING...\n"));

	// Erstmal alles löschen
	DeleteAll();
	m_tmIamAlive.Reset();
	m_tmIamAlive.StatMessage(_T("Reset"),0,_T("ServerAlive"));
	WK_STAT_LOG(_T("Reset"),1,_T("ServerActive"));

	ClearProcessListView();

	SetState(AS_START_RESET);

	AddProcessListViewItem(_T("loading data..."));

	ReadDebugConfiguration();
	
	// CAVEAT order is important
	// _T('static') data first
	CWK_Profile wkp;
	m_Permissions.Load(wkp);
	m_Users.Load(wkp);
	ResetHosts(wkp);
	m_ProcessMacros.Load(m_Permissions);
	m_ProcessMacros.AddDefaultMacros();
	
	// CAVEAT this time is for Load, after the reset delay another
	// UpdateTimers is made
	CTime ct = CTime::GetCurrentTime();
	m_Timers.SetAutoDelete(TRUE);
	m_Timers.Load(ct,wkp);
	
	// now the inputs, including activations, which make use of
	// ProcessMacros, Timers ...
	// the references to outputs are plain CSecIDs
	m_InputGroups.Load(wkp);
	if (HasAlarmOffSpanFeature())
	{
		m_InputGroups.InformClientsAlarmOffSpans();
	}
	
	MarkLockInputs();
	
	m_OutputGroups.Load(wkp);
	m_OutputGroups.DeleteUnnecessaryReferenzImages();
	m_OutputGroups.WriteNamesIni();
	
	// create a dummy outputGroup to have a processScheduler
	// for ISDN processes, these might even run without an output
	// the CIPC itself is never activated
	// NOT YET only create if there is need (saves a scheduler thread)
	m_pOutputDummy = new CIPCOutputServer(_T("OutputDummy"),0,_T("OutputDummyShm"));
	m_pOutputDummy->Load(wkp,0);
	m_AudioGroups.Load(wkp);

	m_InputGroups.CreateMDOKActivations();
	
//	SetState(AS_START_DATABASE);
	SetState(AS_START_RESET); 
	AddProcessListViewItem(_T("activating threads..."));
	((CMainFrame*)m_pMainWnd)->SetTimer(100);
}
////////////////////////////////////////////////////////////////////
void CSec3App::EnableMDOnOutputs()
{
	m_InputGroups.Lock(_T(__FUNCTION__));
	for (int i=0;i<m_InputGroups.GetSize();i++)
	{
		CIPCInputServer* pInputGroup = m_InputGroups.GetAtFast(i);
		CString sShmName(pInputGroup->GetShmName());
		if (0==sShmName.Find(SM_MICO_MD_INPUT))
		{
			// it's a MD Group
			sShmName.Replace(_T("MDInput"),_T("OutputCamera"));
			CIPCOutputServer* pOutputGroup = m_OutputGroups.GetGroupShmName(sShmName);
			if (pOutputGroup)
			{
				for (WORD j=0;j<pInputGroup->GetSize();j++)
				{
					const CInput* pInput = pInputGroup->GetInputAt(j);
					CSecID id(pOutputGroup->GetGroupID(),j);
					CString s(CSD_OFF);
					if (   pInput 
						&& (pInput->GetIDActivate() != SECID_ACTIVE_OFF)
					   )
					{
						s = CSD_ON;
					}
					pOutputGroup->DoRequestSetValue(id,CSD_MD,s);
				}
			}
		}
	}
	m_InputGroups.Unlock();
}

/*@MHEAD{interface actions:}*/
/*@MD NYD */
void CSec3App::OnEinstellungenSupervisor() 
{
	Sleep(100);	// allow answer for FetchServerReset
	WK_TRACE(_T("Reset because of SuperVisor change......\n"));
	Reset();
}

/*@MD NYD */
void CSec3App::UpdateStateInfo()
{
	if (GetMainWnd()) 
	{
		GetMainWnd()->PostMessage(WM_COMMAND,ID_ANSICHT_AKTUALISIEREN);
	}
}

// App command to run the dialog
/*@MD NYD */
void CSec3App::OnAppAbout()
{
	COemGuiApi::AboutDialog(m_pMainWnd);
/*
	CString s(_T("c:\\out\\test.bmp"));
	CDib d(s);

	int iWidth, iHeight;
	BYTE* pBuffer;
	CSecID id(SECID_GROUP_OUTPUT,0);

	iWidth = d.GetBMinfoHeader()->biWidth;
	iHeight = d.GetBMinfoHeader()->biHeight;
	if (iWidth != 0 && iHeight != 0)
	{
		pBuffer = new BYTE[iWidth*iHeight*3];

		ZeroMemory(pBuffer,iWidth*iHeight*3);
		CopyMemory(pBuffer,d.GetBMbits(),iWidth*iHeight*3);

		CMPEG4Encoder encoder;
		encoder.Init(iWidth,iHeight,16000,1,1);
		encoder.SetBGRData(id,iWidth,iHeight,pBuffer);
		encoder.Encode();

		if (   m_pDatabase
			&& m_pDatabase->GetIPCState() == CIPC_STATE_CONNECTED)
		{
			CSystemTime st;
			st.GetLocalTime();
			CIPCPictureRecord* pPict = encoder.GetEncodedPicture(m_pDatabase,id,st);
			if (pPict)
			{
				TRACE(_T("encoded image %d Bytes"),pPict->GetDataLength());
				
				CFile file;

				if (file.Open(_T("c:\\out\\test.mp4"),CFile::modeCreate|CFile::modeReadWrite))
				{
					file.Write(pPict->GetData(),pPict->GetDataLength());
				}

				CIPCFields fields;
				m_pDatabase->DoRequestNewSavePicture(5,*pPict,fields);
				delete pPict;
			}
		}
		delete [] pBuffer;
	}
*/
}


/*@MD NYD */
void CSec3App::StopClientProcesses(CIPCOutputServerClient* pClient)
{
	if (IsUpAndRunning()) 
	{	
		for (int i=0;i<m_OutputGroups.GetSize();i++) 
		{
			m_OutputGroups[i]->GetProcessScheduler()->TerminateClientProcesses(pClient->GetID());
		}
		if (m_pOutputDummy)
		{
			m_pOutputDummy->GetProcessScheduler()->TerminateClientProcesses(pClient->GetID());
			m_pOutputDummy->GetProcessScheduler()->TerminateCallProcesses(pClient);
		}
	}
}
/*@MD NYD */
void CSec3App::StopClientProcesses(CIPCAudioServerClient* pClient)
{
	if (IsUpAndRunning()) 
	{	
		for (int i=0;i<m_AudioGroups.GetSize();i++) 
		{
			m_AudioGroups[i]->GetProcessScheduler()->TerminateClientProcesses(pClient->GetID());
		}
	}
}


/*@MD NYD */
void CSec3App::OnUpdatePaneUnits(CCmdUI* pCmdUI)
{
	int i;
	int numInputUnits=0;
	int numOutputUnits=0;
	int numAudioUnits=0;
	
	for (i=0;i<m_InputGroups.GetSize();i++) 
	{
		if (m_InputGroups.GetGroupAt(i)->GetIPCState()==CIPC_STATE_CONNECTED) 
		{
			numInputUnits++;
		}
	}
	for (i=0;i<m_OutputGroups.GetSize();i++) 
	{
		if (m_OutputGroups.GetGroupAt(i)->GetIPCState()==CIPC_STATE_CONNECTED) 
		{
			numOutputUnits++;
		}
	}
	for (i=0;i<m_AudioGroups.GetSize();i++) 
	{
		if (m_AudioGroups.GetAtFast(i)->GetIPCState()==CIPC_STATE_CONNECTED) 
		{
			numAudioUnits++;
		}
	}
	CString s;
	s.Format(_T("%02d,%02d,%02d"),numInputUnits,numOutputUnits,numAudioUnits);
	pCmdUI->SetText(s);
}

/*@MD NYD */
void CSec3App::OnUpdatePaneClients(CCmdUI* pCmdUI)
{
	int numInputClients=0;
	int numOutputClients=0;
	int numAudioClients=0;
	CString s;
	
	if (m_pServerControl) 
	{
		numInputClients  = GetInputClients().GetSize();
		numOutputClients = GetOutputClients().GetSize();
		numAudioClients = GetAudioClients()->GetSize();
	}
	s.Format(_T("%02d,%02d,%02d"),numInputClients,numOutputClients,numAudioClients);
	pCmdUI->SetText(s);
	
}

/*@MD NYD */
void CSec3App::OnUpdatePaneDb(CCmdUI* pCmdUI)
{
	CString s;
	if (m_pDatabase && m_pDatabase->GetIPCState()==CIPC_STATE_CONNECTED) 
	{
		s=_T("available");
		if (m_bStoring) 
		{
			s=_T("storing");
		}
	} 
	else 
	{
		if (m_bStoring) 
		{
			s=_T("can't store");
		} 
		else 
		{
			s=_T("not available");
		}
	}
	pCmdUI->SetText(s);
}
///////////////////////////////////////////////////////////////////////////
void CSec3App::SaveMedia(const CIPCMediaSampleRecord& rec,								
						 CIPCFields& fields,
						 WORD wArchiveNr)
{
	if (    m_pDatabase
		&& (m_pDatabase->GetIPCState() == CIPC_STATE_CONNECTED))
	{
		fields.Lock(_T(__FUNCTION__));
		m_pDatabase->DoRequestSaveMedia(wArchiveNr,SECID_NO_ID,FALSE,FALSE,rec,fields);
		fields.Unlock();
	}
}
///////////////////////////////////////////////////////////////////////////
void CSec3App::SavePicture(Compression comp,
						   const CIPCPictureRecord& pictRef,								
						   CIPCFields& fields,
						   WORD wArchiveNr)
{
	ModifyStoringFlag(TRUE);
	if (    m_pDatabase
		&& (m_pDatabase->GetIPCState() == CIPC_STATE_CONNECTED))
	{
		if (pictRef.GetDataLength()>0)
		{
			fields.Lock(_T(__FUNCTION__));
			CIPCFields fieldscopy(fields);
			fields.Unlock();

			if (   pictRef.GetCompressionType() == COMPRESSION_YUV_422
				|| pictRef.GetCompressionType() == COMPRESSION_RGB_24)
			{
				CompressionType ct = COMPRESSION_JPEG;
				if (m_bMpeg4Recording)
				{
					ct = COMPRESSION_MPEG4;
				}
				m_EncoderThread.AddPictureRecord(comp,pictRef,fieldscopy,wArchiveNr,0,ct);
			}
			else if (  pictRef.GetCompressionType() == COMPRESSION_JPEG
					|| pictRef.GetCompressionType() == COMPRESSION_H263
					|| pictRef.GetCompressionType() == COMPRESSION_MPEG4)
			{
				CIPCPictureRecord* pTemp = NULL;

				if (pictRef.GetBubble())
				{
					pTemp = new CIPCPictureRecord(pictRef);
				}
				else
				{
					pTemp = new CIPCPictureRecord(m_pDatabase,
											pictRef.GetData(),
											pictRef.GetDataLength(),
											pictRef.GetCamID(),
											pictRef.GetResolution(),
											pictRef.GetCompression(),
											pictRef.GetCompressionType(),
											pictRef.GetTimeStamp(),
											pictRef.GetJobTime(),
											NULL,
											pictRef.GetBitrate(),
											pictRef.GetPictureType(),
											pictRef.GetBlockNr());
				}
				/*
				TRACE(_T("saving %08lx %s,%d\n"),pict.GetCamID().GetID(),
				pict.GetTimeStamp().GetTime(),pict.GetTimeStamp().GetMilliseconds());
				*/
				if (   pTemp
					&& pTemp->GetDataLength())
				{
					m_pDatabase->DoRequestNewSavePicture(wArchiveNr,0,*pTemp,fieldscopy);
				}
				WK_DELETE(pTemp);
			}
		}
		else
		{
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSec3App::SaveAlarmData(CIPCFields& fields,
							 WORD wArchiveNr)
{
	if (	m_pDatabase
		&& (m_pDatabase->GetIPCState() == CIPC_STATE_CONNECTED))
	{
		fields.Lock();
		m_pDatabase->DoRequestSaveData(wArchiveNr,fields);
		fields.Unlock();
	}
}

/*@MD NYD */
void CSec3App::ModifyStoringFlag(BOOL bNewState)
{
	m_csStoreFlag.Lock();
	if (m_bStoring!=bNewState) {
		m_bStoring=bNewState;
		if (GetMainWnd()) {
			GetMainWnd()->PostMessage(WM_COMMAND,ID_UPDATE_DATABASE_PANE);
		}
	}
	m_csStoreFlag.Unlock();
}

/*@MD NYD */
void CSec3App::DenyClientAccess(CIPC *pCipc, DWORD dwCmd)
{
	if (pCipc) 
	{
		WK_TRACE_ERROR(_T("Client denied %s, %s\n"),pCipc->GetShmName(),CIPC::NameOfCmd(dwCmd));
		pCipc->DoIndicateError(dwCmd, SECID_NO_ID,CIPC_ERROR_UNABLE_TO_CONNECT, 3, _T("Client denied due to server reset"));
	}
}


/*@MD NYD 
Called from TimerThread AND if an CInput has changed,
which is used as a lock.
*/
void CSec3App::StartProcessesByTimer(const CSecTimer *pTimer)
{
	for (int i=0;i<m_InputGroups.GetSize();i++) 
	{
		CIPCInputServer *pInputGroup = m_InputGroups.GetGroupAt(i);
		pInputGroup->StartMissingProcesses(SECID_NO_GROUPID, pTimer);
	}
}
/*@MD NYD
*/
void CSec3App::KillProcessesByTimer(const CSecTimer *pTimer)
{
	for (int i=0;i<m_OutputGroups.GetSize();i++) 
	{
		m_OutputGroups.GetGroupAt(i)->GetProcessScheduler()->KillProcessByTimer(pTimer);
	}
	for (i=0;i<m_AudioGroups.GetSize();i++) 
	{
		m_AudioGroups.GetGroupAt(i)->GetProcessScheduler()->KillProcessByTimer(pTimer);
	}
	if (m_pOutputDummy) 
	{
		m_pOutputDummy->GetProcessScheduler()->KillProcessByTimer(pTimer);
	}
}

/*@MD NYD
posted in TimerThread
@PAR if reset delay still active, print _T("synchronizing")
@PAR else inital start of alarms

  Uses @CW{ID_START_ALARMS}, which is posted from NOT YET
*/

/*@MD
Mark all inputs, which are used as locks. Even
call pInput->AddLockTimer(pTimer), so the input knowns its
associated timers.
*/
void CSec3App::MarkLockInputs()
{
	int iNumLocks=0;
	
	for (int t=0;t<m_Timers.GetSize();t++) 
	{
		CSecTimer *pTimer = m_Timers.GetAtFast(t);
		const CDWordArray &lockIDs = pTimer->GetLockIDs();
		for (int lx=0;lx<lockIDs.GetSize();lx++) 
		{
			CInput* pInput = m_InputGroups.GetInputByID(lockIDs[lx]);
			
			if (pInput) 
			{
				if (!pInput->IsUsedAsLockForTimer()) 
				{
					iNumLocks++;
				} 
				else 
				{
					// already done
				}
				// alwasy add the lockID
				pInput->AddLockTimer(pTimer);
			} 
			else 
			{
				WK_TRACE_ERROR(_T("Lock %x not found\n"),lockIDs[lx]);
			}
		} // end of lx loop over lockIDs
	} // end of t loop over timers
	WK_TRACE(_T("%d inputs are used as locks\n"),iNumLocks);
}
////////////////////////////////////////////////////////////////////////
void CSec3App::OnTimerThreadRun()
{
	m_csLastTimerThreadRunTick.Lock();
	m_dwLastTimerThreadRunTick = WK_GetTickCount();
	m_csLastTimerThreadRunTick.Unlock();
	m_tmIamAlive.StatMessage(_T("Reset"),-2,_T("ServerAlive"));
}
////////////////////////////////////////////////////////////////////////
void CSec3App::SetState(ApplicationState as)
{
	m_State = as;
	m_dwCounter = 0;
}
////////////////////////////////////////////////////////////////////////
void CSec3App::OnTimer()
{
	m_dwCounter++;
	switch (m_State)
	{
	case AS_STARTED:
		OnStarted();
		break;
	case AS_START_RESET:
		OnStartReset();
		break;
	case AS_START_DATABASE:
		OnStartDatabase();
		break;
	case AS_START_ENCODER:
		OnStartEncoder();
		break;
	case AS_START_UPLOAD:
		OnStartUpload();
		break;
	case AS_START_OUTPUT:
		OnStartOutput();
		break;
	case AS_SET_MD:
		OnSetMD();
		break;
	case AS_START_AUDIO:
		OnStartAudio();
		break;
	case AS_START_TIMER:
		OnStartTimer();
		break;
	case AS_START_INPUT:
		OnStartInput();
		break;
	case AS_RESET_COMPLETE:
		OnResetComplete();
		break;
	case AS_UP_AND_RUNNING:
//#ifndef _DEBUG
		OnIsUpAndRunning();
//#endif
		break;
	default:
		break;
	}
}
////////////////////////////////////////////////////////////////////////
void CSec3App::OnStarted()
{
}
////////////////////////////////////////////////////////////////////////
void CSec3App::OnStartReset()
{
	for (int oc=0;oc<GetOutputClients().GetSize();oc++) 
	{
		GetOutputClients().GetAtFast(oc)->DoConfirmGetValue(SECID_NO_ID, CSD_RESET, CSD_ON, 0);
	}
	for (int ac=0;ac<GetAudioClients()->GetSize();ac++) 
	{
		GetAudioClients()->GetAtFast(ac)->DoRequestSetValue(SECID_NO_ID, CSD_RESET, CSD_ON, 0);
	}
	for (int ic=0;ic<GetInputClients().GetSize();ic++) 
	{
		GetInputClients().GetAtFast(ic)->DoConfirmGetValue(SECID_NO_ID, CSD_RESET, CSD_ON, 0);
	}
	SetState(AS_START_DATABASE);
}
////////////////////////////////////////////////////////////////////////
BOOL CSec3App::ConnectToDatabase()
{
	BOOL bRet = FALSE;
	// hard coded shared memory name, to keep permanent DB connection
	if (m_pDatabase == NULL)
	{
		AddProcessListViewItem(_T("connecting to database"));
		m_pDatabase = new CIPCDatabaseClientServer(_T("DataBaseToServer"));
		ModifyStoringFlag(FALSE);
	}
	else
	{
		if (m_pDatabase->GetIPCState() == CIPC_STATE_CONNECTED)
		{
			if (!IsUpAndRunning())
			{
				AddProcessListViewItem(_T("connected to database"));
			}
			bRet = TRUE;
		}
		else if (m_pDatabase->GetIsMarkedForDelete())
		{
			WK_DELETE(m_pDatabase);
		}
	}
	return bRet;
}
////////////////////////////////////////////////////////////////////////
void CSec3App::OnStartDatabase()
{
	if (ConnectToDatabase())
	{
		SetState(AS_START_ENCODER);
	}
	else
	{
		// timeout 2 seconds
		if (m_dwCounter>20)
		{
			SetState(AS_START_ENCODER);
		}
	}
}
////////////////////////////////////////////////////////////////////////
void CSec3App::OnStartEncoder()
{
	if (m_dwCounter == 1)
	{
		AddProcessListViewItem(_T("starting JPEG/Mpeg4 Encoder"));
		m_EncoderThread.StartThread();
	}
	else
	{
		if (m_EncoderThread.IsRunning())
		{
			AddProcessListViewItem(_T("JPEG/Mpeg4 Encoder started"));
			SetState(AS_START_UPLOAD);
		}
		// timeout 2 seconds
		if (m_dwCounter>20)
		{
			SetState(AS_START_UPLOAD);
		}
	}
}
////////////////////////////////////////////////////////////////////////
void CSec3App::OnStartUpload()
{
	if (m_dwCounter == 1)
	{
		AddProcessListViewItem(_T("starting FTP Uploader"));
		m_UploadThread.StartThread();
	}
	else
	{
		if (m_UploadThread.IsRunning())
		{
			AddProcessListViewItem(_T("FTP Uploader started"));
			SetState(AS_START_OUTPUT);
		}
		// timeout 2 seconds
		if (m_dwCounter>20)
		{
			SetState(AS_START_OUTPUT);
		}
	}
}
////////////////////////////////////////////////////////////////////////
void CSec3App::OnStartOutput()
{
	if (m_dwCounter == 1)
	{
		AddProcessListViewItem(_T("connecting outputs"));
		m_OutputGroups.StartThreads();
		m_pOutputDummy->Start();
	}
	else
	{
		BOOL bOK = TRUE;
		CIPCOutputServer* pGroup = NULL;
		for (int i=0;i<m_OutputGroups.GetSize() && bOK;i++)
		{
			pGroup = m_OutputGroups.GetAtFast(i);
			bOK	   =    (pGroup->GetIPCState() == CIPC_STATE_CONNECTED)
				     && (pGroup->GetHardwareState() != 1);
		}
		if (bOK)
		{
			AddProcessListViewItem(_T("outputs connected"));
		}
		// timeout 30 seconds
		if (   bOK
			|| m_dwCounter>300)
		{
			SetState(AS_SET_MD);
		}
	}
}
////////////////////////////////////////////////////////////////////////
void CSec3App::OnSetMD()
{
	if (m_dwCounter == 1)
	{
		AddProcessListViewItem(_T("enable motion detection"));
		EnableMDOnOutputs();
	}
	else if (m_dwCounter == 10)
	{
		AddProcessListViewItem(_T("motion detection enabled"));
		SetState(AS_START_AUDIO);
	}
}
////////////////////////////////////////////////////////////////////////
void CSec3App::OnStartAudio()
{
	if (m_dwCounter == 1)
	{
		AddProcessListViewItem(_T("connecting audio"));
		m_AudioGroups.StartThreads();
	}
	else
	{
		BOOL bOK = TRUE;
		CIPCAudioServer* pGroup = NULL;
		for (int i=0;i<m_AudioGroups.GetSize() && bOK;i++)
		{
			pGroup = m_AudioGroups.GetAtFast(i);
			bOK	   =    (pGroup->GetIPCState() == CIPC_STATE_CONNECTED)
				     && (pGroup->GetHardwareState() != 1);
		}
		if (bOK)
		{
			AddProcessListViewItem(_T("audio connected"));
		}
		if (   bOK
			|| m_dwCounter>100)
		{
			SetState(AS_START_TIMER);
		}
	}
}
////////////////////////////////////////////////////////////////////////
void CSec3App::OnStartTimer()
{
	if (m_dwCounter == 1)
	{
		AddProcessListViewItem(_T("starting timer thread"));

		// print inital timer states in log file
		{
			CTime ct = CTime::GetCurrentTime();
			
			for (int i=0;i<m_Timers.GetSize();i++) 
			{
				CSecTimer *pTimer = m_Timers.GetAtFast(i);
				int iValue;
				if (pTimer->IsIncluded(ct)) 
				{
					iValue=1;
				} 
				else 
				{
					iValue=0;
				}
				CString sMsg;
				sMsg.Format(_T("Timer|%s"),pTimer->GetName());
				WK_STAT_LOG(_T("Timer"),iValue,sMsg);
				
				// OOPS strange behaviour if time is wrong
				// for example if ct is 9.4.2034 12:00
				// the next update is in the far future
				pTimer->UpdateNextModeChange(ct+CTimeSpan(0,0,0,1));	// plus one second
			}
		}
		
		m_TimerThread.StartThread();
		m_csLastTimerThreadRunTick.Lock();
		m_dwLastTimerThreadRunTick = WK_GetTickCount();
		m_csLastTimerThreadRunTick.Unlock();
	}
	else
	{
		if (m_dwCounter>5 || m_TimerThread.IsRunning())
		{
			AddProcessListViewItem(_T("timer thread started"));
			SetState(AS_START_INPUT);
		}
	}
}
////////////////////////////////////////////////////////////////////////
void CSec3App::OnStartInput()
{
	if (m_dwCounter == 1)
	{
		AddProcessListViewItem(_T("connecting input"));
		m_InputGroups.StartThreads();
	}
	else
	{
		BOOL bOK = TRUE;
		CIPCInputServer* pGroup = NULL;
		for (int i=0;i<m_InputGroups.GetSize() && bOK;i++)
		{
			pGroup = m_InputGroups.GetAtFast(i);
			bOK	   =    (pGroup->GetIPCState() == CIPC_STATE_CONNECTED)
				     && (pGroup->IsInitDone());
		}
		if (bOK)
		{
			AddProcessListViewItem(_T("input connected"));
		}
		if (   bOK
			|| m_dwCounter>50)
		{
			SetState(AS_RESET_COMPLETE);
		}
	}
}
////////////////////////////////////////////////////////////////////////
void CSec3App::OnResetComplete()
{
	// clear list
	ClearProcessListView();
	//
	DisplayDebugOptions();
	
	// NOW things are ready
	m_bResetPosted = FALSE;
	SetState(AS_UP_AND_RUNNING);

	m_InputGroups.StartInitialProcesses();

	// force an update of the clients, since info requests are
	// denied during reset
	for (int oc=0;oc<GetOutputClients().GetSize();oc++) 
	{
		GetOutputClients()[oc]->OnRequestInfoOutputs(SECID_NO_GROUPID);
	}
	for (int ac=0;ac<GetAudioClients()->GetSize();ac++) 
	{
		GetAudioClients()->GetAtFast(ac)->OnRequestInfoMedia(SECID_NO_GROUPID);
	}
	for (int ic=0;ic<GetInputClients().GetSize();ic++) 
	{
		GetInputClients()[ic]->OnRequestInfoInputs(SECID_NO_GROUPID);
	}
	
	// and do the actions!
	WK_STAT_LOG(_T("Reset"),-1,_T("ServerActive"));
	if (HasAlarmOffSpanFeature())
	{
		m_InputGroups.InformClientsAlarmOffSpans();
	}

	((CMainFrame*)m_pMainWnd)->SetTimer(5000);
	// add clients manager and start the server control listener
	WK_TRACE(_T("RESET ... COMPLETE\n"));
}
////////////////////////////////////////////////////////////////////////
void CSec3App::OnIsUpAndRunning()
{
#ifndef _DEBUG
	if (IsUpAndRunning())
	{
		// check my threads
		BOOL bExit = FALSE;
		BOOL bReset = FALSE;
		if (m_TimerThread.IsRunning())
		{
			m_csLastTimerThreadRunTick.Lock();
			if (WK_GetTimeSpan(m_dwLastTimerThreadRunTick) > 10*1000)
			{
				WK_TRACE_ERROR(_T("timer thread does not answer %d %d\n"),WK_GetTickCount(),m_dwLastTimerThreadRunTick);
				bExit = TRUE;
			}
			m_csLastTimerThreadRunTick.Unlock();
		}

		if (m_pServerControl)
		{
			if (m_pServerControl->IsTimeout())
			{
				WK_TRACE_ERROR(_T("server control does not answer\n"));
				bExit = TRUE;
			}
		}
		
		for (int i=0;i<m_OutputGroups.GetSize();i++) 
		{
			CIPCOutputServer* pOutputServer = m_OutputGroups.GetGroupAt(i);
			if (pOutputServer)
			{
				if (pOutputServer->GetIPCState() != CIPC_STATE_CONNECTED)
				{
					CString sAppname = pOutputServer->GetAppname(); 
					TRACE(_T("%s not connected\n"),sAppname);
					if (WK_IS_RUNNING(sAppname))
					{
						// running and not connected
						WK_TRACE_ERROR(_T("%s,%s unit not connected but running RESET\n"),sAppname,pOutputServer->GetShmName());
						bReset = TRUE;
					}
					else
					{
						TRACE(_T("%s not connected, not running\n"),sAppname);
					}
				}
				if 	(  pOutputServer->GetProcessScheduler()
					&& pOutputServer->GetProcessScheduler()->IsRunning())
				{
					if (pOutputServer->GetProcessScheduler()->IsTimeout())
					{
						WK_TRACE_ERROR(_T("sheduler thread does not answer %s\n"),m_OutputGroups.GetGroupAt(i)->GetShmName());
						bExit = TRUE;
					}
					if (pOutputServer->GetProcessScheduler()->IsImageTimeout())
					{
						WK_TRACE_ERROR(_T("image timeout %s\n"),m_OutputGroups.GetGroupAt(i)->GetShmName());
						bReset = TRUE;
					}
				}
			}
		}
		if (   m_pOutputDummy
			&& m_pOutputDummy->GetProcessScheduler()
			&& m_pOutputDummy->GetProcessScheduler()->IsRunning()) 
		{
			if (m_pOutputDummy->GetProcessScheduler()->IsTimeout())
			{
				WK_TRACE_ERROR(_T("sheduler thread does not answer %s\n"),m_pOutputDummy->GetShmName());
				bExit = TRUE;
			}
		}
		
		if (bExit)
		{
			m_pMainWnd->DestroyWindow();
			CloseDebugger();
			Sleep(100);
			ExitProcess(0);
		}
		else if (bReset)
		{
			EmergencyReset();
		}
		else
		{
			// everything OK
			// TRACE(_T("checking threads OK\n"));
			ConnectToDatabase();

			CSystemTime now;
			now.GetLocalTime();
			if (now.wDay != m_stTime.wDay)
			{
				m_stTime = now;
				OnNewDay();
			}
		}
	}
#else
	ConnectToDatabase();
#endif
}
/////////////////////////////////////////////////////////////////////////////////
void CSec3App::OnNewDay()
{
	//show NAG screen with maintenance and RTEs on all clients on a new day
	//if the NAG screen is still on screen it will be updated
	int ic = 0;
	for (ic=0;ic<GetOutputClients().GetSize();ic++) 
	{
		CIPCOutputServerClient* pOC = GetOutputClients().GetAtFast(ic);
		pOC->CheckNagScreen();
	}
}
/////////////////////////////////////////////////////////////////////////////////
BOOL SeparatSAToWA(CStringArray& sa, CWordArray& waCams, char cSeparator)
{
	BOOL bRet = TRUE;
	BOOL bSplit = FALSE;
	CString sToCheck;
	int iFound =0;
	DWORD dwFrom, dwTo;
	dwFrom = dwTo = 0;


	//Separator darf nicht leer sein
	CString sSeparator(cSeparator);
	if(sSeparator.IsEmpty())
	{
		return FALSE;
	}

	//prüfe StringArray auf korrekte Einträge
	for (int i=0; i < sa.GetSize(); i++)
	{
		sToCheck = sa.GetAt(i);

		//nur Zahlen oder _T("-") dürfen enthalten sein
		CString sAcceptChar;
		sAcceptChar = sToCheck;
		CString s;
		for(int i=0; i<=9; i++)
		{
			s.Format(_T("%i"), i); 
			LPCTSTR c = (LPCTSTR)s;
			sAcceptChar.Remove(c[0]);
		}
		sAcceptChar.Remove(cSeparator);


		//Prüfe ArrayElement auf zugelassene Characters
		if(!sAcceptChar.IsEmpty())
		{
			//falscher Character im String gefunden
			bRet = FALSE;
			break;
		}

		//prüfe, ob Separator (mehrfach) vorhanden ist
		if(bRet)
		{
			CString sCheckSeparator;
			sCheckSeparator = sToCheck;
			iFound = sCheckSeparator.Find(sSeparator);

			if(iFound != -1)
			{
				sCheckSeparator.Delete(iFound);
				iFound = sCheckSeparator.Find(sSeparator);
				if(iFound != -1)
				{
					//Separator ist mehrmals vorhanden
					TRACE(_T("mehr als 1 Separator\n"));
					bRet = FALSE;
					break;
				}
				else
				{
					//nur ein Separator gefunden
					if(2 == _stscanf(sToCheck,_T("%d-%d"), &dwFrom, &dwTo))
					{
//						TRACE(_T("OK %d-%d\n"), dwFrom, dwTo);
						bSplit = TRUE;
					}
					else
					{
						TRACE(_T("Fehler bei scanf\n"));
						bRet = FALSE;
						break;
					}
				}
			}
			//kein Separator gefunden, Wert kann gespeichert werden
			else
			{
				DWORD dwNewArrayElement = 0;
				if(1 == _stscanf(sToCheck,_T("%d"),&dwNewArrayElement))
				{
					waCams.Add((WORD)dwNewArrayElement);
				}				
			}

			//Einträge "von - bis" (z.B. 5-8) prüfen
			if(bRet && bSplit)
			{
				if(dwFrom < dwTo)
				{
					for(DWORD dw=(int)dwFrom; dw<=(int)dwTo; dw++)
					{
						waCams.Add((WORD)dw);
					}
				}
				else if(dwFrom == dwTo)
				{
					waCams.Add((WORD)dwFrom);
				}
				else
				{
					for(DWORD dw=(int)dwTo; dw<=(int)dwFrom; dw++)
					{
						waCams.Add((WORD)dw);
					}
				}
				bSplit = FALSE;
			}
		}
	}

	return bRet;

}
/////////////////////////////////////////////////////////////////////////////////
CString CSec3App::GetHTMLCodePage(UINT nCP)
{
	CString str;
#ifdef _UNICODE
	CoInitialize(NULL);
	CStringArray ar;
	if (GetCodePageInfo(nCP, ar))
	{
/*
		for (int i=OEM_CP_FLAGS; i<=OEM_CP_GDICHARSET; i++)
		{
			WK_TRACE(_T("%s|"), ar.GetAt(i));
		}
*/
		str = ar.GetAt(OEM_CP_WEBCHARSET);
	}
	CoUninitialize();
#endif
	if (str.IsEmpty())
	{
		str.Format(_T("windows-%d"), nCP);
	}
	return str;
}
/////////////////////////////////////////////////////////////////////////////////
#if _MFC_VER >= 0x0710
int CSec3App::SetLanguageParameters(UINT uCodePage, DWORD dwCPbits)
{
	// TODO! RKE: extract resource for multilang
	// m_hLangResourceDLL may be initialized in CWinApp::InitInstance()
	// Do not call InitInstance() of the base class
//	CLoadResourceDll ResourceDll;
//	m_hLangResourceDLL = ResourceDll.DetachResourceDllHandle();
//	if (m_hLangResourceDLL == NULL)
//	{
//		WK_TRACE(_T("Did not find any ResourceDLL\n"));
//		ASSERT(FALSE);
//		return FALSE;
//	}
//	AfxSetResourceHandle(m_hLangResourceDLL);

	COemGuiApi::ChangeLanguageResource();
	if (uCodePage == 0)
	{
		uCodePage = _ttoi(COemGuiApi::GetCodePage());
	}
	// Set the CodePage for MBCS conversion (Registry Import Export)
	CWK_String::SetCodePage(uCodePage);
	CWK_Profile wkp;
	wkp.SetCodePage(CWK_String::GetCodePage());
	if (dwCPbits == 0)
	{
		dwCPbits = COemGuiApi::GetCodePageBits();
	}
	if (dwCPbits)
	{
		SetFontFaceNamesFromCodePageBits(dwCPbits);
	}
	else
	{
		CSkinDialog::DoUseGlobalFonts(FALSE);
	}
#ifdef _UNICODE
	if (m_pServerControl)
	{
		m_pServerControl->SetCodePage((WORD)uCodePage);
	}
#endif
	return 0;
}
#endif
/////////////////////////////////////////////////////////////////////////
void CSec3App::OnLogProcesses()
{
	for (int i=0;i<m_OutputGroups.GetSize();i++) 
	{
		CIPCOutputServer* pOutputServer = m_OutputGroups.GetGroupAt(i);
		if (pOutputServer)
		{
			if 	(  pOutputServer->GetProcessScheduler()
				&& pOutputServer->GetProcessScheduler()->IsRunning())
			{
				pOutputServer->GetProcessScheduler()->TraceProcesses(TRUE);
			}
		}
	}
	if (   m_pOutputDummy
		&& m_pOutputDummy->GetProcessScheduler()
		&& m_pOutputDummy->GetProcessScheduler()->IsRunning()) 
	{
		m_pOutputDummy->GetProcessScheduler()->TraceProcesses(TRUE);
	}
}
/////////////////////////////////////////////////////////////////////////
void CSec3App::OnLogRequests()
{
	for (int i=0;i<m_OutputGroups.GetSize();i++) 
	{
		CIPCOutputServer* pOutputServer = m_OutputGroups.GetGroupAt(i);
		if (pOutputServer)
		{
			if 	(  pOutputServer->GetProcessScheduler()
				&& pOutputServer->GetProcessScheduler()->IsRunning())
			{
				pOutputServer->GetProcessScheduler()->TraceRequests();
			}
		}
	}
	if (   m_pOutputDummy
		&& m_pOutputDummy->GetProcessScheduler()
		&& m_pOutputDummy->GetProcessScheduler()->IsRunning()) 
	{
		m_pOutputDummy->GetProcessScheduler()->TraceRequests();
	}
}
