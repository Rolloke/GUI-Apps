// Recherche.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Recherche.h"

#include "MainFrm.h"
#include "RechercheDoc.h"
#include "RechercheView.h"
#include "StationNameDialog.h"
#include "CIPCServerControlRecherche.h"

#include "IPCControlAudioUnit.h"


#include <cderr.h>      // Commdlg Error definitions
#include <winspool.h>

// NOTE: THis line is a hardcoded reference to an MFC header file
//  this path may need to be changed to refer to the location of VC5 install
//  for successful compilation.
// #include <..\src\occimpl.h>
// GF moved to custsite.h, workaround for VS_70, but still errors
// GF moving to custsite.h results in unresolved external 8-(
#if _MSC_VER >= 1300
	#include <ocdb.h>
	#include <occimpl.h>
#else
	#include <..\src\occimpl.h>
#endif

#include "custsite.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define BASE_DVRT_KEY            _T("software\\DVRT\\")
#define REGKEY_BASEDVRT_DEFINED  2
#define REGKEY_RECHERCHE_DEFINED 4

#define USE_MONITOR           _T("UseMonitor")
#define MAX_SENT_SAMPLES      _T("MaxSentSamples")
#define MAX_REQUESTED_RECORDS _T("MaxRequestedRecords")
#define REALTIME              _T("Realtime")
#define TARGET_DISPLAY        _T("TargetDisplay")
#define SEARCH_MASK           _T("SearchMask")
#define DEV_MODE              _T("DevMode")
#define DEV_NAMES             _T("DevNames")
#define STATION_NAME          _T("StationName")
#define SYNC_TIME             _T("SyncTime")
#define DATA_BASE_CLIENT      _T("DataBaseClient")
/////////////////////////////////////////////////////////////////////////////
// CRechercheApp

#define CASE_ENUM_RETURN_NAME(E)	case E: return _T(#E)

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


BEGIN_MESSAGE_MAP(CRechercheApp, CWinApp)
	//{{AFX_MSG_MAP(CRechercheApp)
	ON_COMMAND(ID_NEW_LOGIN, OnNewLogin)
	ON_COMMAND(ID_EDIT_STATION, OnEditStation)
	ON_COMMAND(ID_FILE_PRINT_SETUP, OnFilePrintSetup)
	ON_COMMAND(ID_SEARCH_AT_START, OnSearchAtStart)
	ON_UPDATE_COMMAND_UI(ID_SEARCH_AT_START, OnUpdateSearchAtStart)
	ON_COMMAND(ID_VIEW_TARGET_DISPLAY, OnViewTargetDisplay)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TARGET_DISPLAY, OnUpdateViewTargetDisplay)
	ON_COMMAND(ID_SEQUENCE_REALTIME, OnSequenceRealtime)
	ON_UPDATE_COMMAND_UI(ID_SEQUENCE_REALTIME, OnUpdateSequenceRealtime)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRechercheApp construction

CRechercheApp::CRechercheApp()
{
	m_bRunAnyLinkUnit = FALSE;
	m_pUser = NULL;
	m_pPermission = NULL;
	m_pCIPCServerControlRecherche = NULL;
	m_OemCode = CWK_Dongle::OEM_INVALID;
	m_bReadOnlyModus = FALSE;
	m_bBackupModus = FALSE;
	m_bAllowMiCo = TRUE;
	m_bAllowCoCo = TRUE;
	m_bAllowPT = TRUE;
	m_bAllowBackup = FALSE;
	m_pDispOM = NULL;
	m_bSearchMaskAtStart = FALSE;
	m_bIsReceiver = FALSE;
	m_bIsDemo = FALSE;
	m_nMaxRequestedRecords = 5;
	m_nMaxSentSamples      = 3;
	m_bTargetDisplay = TRUE;
	CWK_PerfMon perfMon(_T("Dummy"));
	m_dwCPUClock = perfMon.GetClockFrequency();

	CWK_Profile wkp;
	m_dwMonitorFlags   = wkp.GetInt(SECTION_RECHERCHE, USE_MONITOR, USE_MONITOR_1);

	//init member for realtime playback
//TODO TKR noch in Abhängigkeit einer noch auszudenkenden Einstellmöglichkeit setzen
	//incl. Registry Speicherung ?
	m_bCanPlayRealtime = TRUE;
}

CRechercheApp::~CRechercheApp()
{
	WK_DELETE(m_pDispOM);
}
/////////////////////////////////////////////////////////////////////////////
// The one and only CRechercheApp object

CRechercheApp theApp;

static TCHAR BASED_CODE szFieldMap[]	= _T("FieldMap");
static TCHAR BASED_CODE szDontUse[]	= _T("dontuse");

/////////////////////////////////////////////////////////////////////////////
void CRechercheApp::InitField(CWK_Profile& wkp,const CString& sName, UINT nID)
{
	CString s;
	if (m_bReadOnlyModus)
	{
		GetPrivateProfileString(_T("Fields"),
			sName,szDontUse,s.GetBuffer(64),64,m_sOemIni);
		s.ReleaseBuffer();
	}
	else
	{
		s = wkp.GetString(szFieldMap,sName,_T(""));
	}
	
	if (s.IsEmpty())
		s.LoadString(nID);

	if (s != szDontUse)
	{
		m_FieldMap.SetAt(sName,s);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRechercheApp::InitFields()
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
	for (int i=0 ; i<fields.GetSize() ; i++)
	{
		pField = fields.GetAt(i);
		sValue = pField->GetValue();
		if (sValue != szDontUse)
		{
			m_FieldMap.SetAt(pField->GetName(), sValue);
		}
	}
	return (fields.GetSize() > 0);
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheApp::InitFieldMap()
{
	CWK_Profile wkp;

	if (m_bReadOnlyModus)
	{
		CString sPath;
		GetModuleFileName(m_hInstance,sPath.GetBuffer(_MAX_PATH),_MAX_PATH);
		sPath.ReleaseBuffer();
		m_sOemIni = sPath.Left(3) + _T("oem.ini");
//		InitIniFile();
	}
	else
	{
		BOOL bFields = FALSE;
		CWK_Dongle dongle;
		InternalProductCode ipc = dongle.GetProductCode();

		switch (ipc)
		{
		case IPC_STORAGE:				// FALLTHROUGH
		case IPC_STORAGE_PLUS:			// FALLTHROUGH
		case IPC_STORAGE_TRANSMITTER:	// FALLTHROUGH
		case IPC_STORAGE_RECEIVER:		// FALLTHROUGH
		case IPC_PROFESSIONAL_RECEIVER:	// FALLTHROUGH
		case IPC_UNIVERSAL_RECEIVER:	// FALLTHROUGH
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
CString	CRechercheApp::GetMappedString(const CString& sKey)
{
	CString s;
	m_FieldMap.Lookup(sKey,s);
	return s;
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheApp::LoadSettings()
{
	CWK_Profile wkp;
	CString sSection;
	UINT nRead = 0;
	LPBYTE pBuffer=NULL;

	sSection = SECTION_RECHERCHE;
	if (wkp.GetBinary(sSection, DEV_MODE,&pBuffer,&nRead))
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
	if (wkp.GetBinary(sSection, DEV_NAMES,&pBuffer,&nRead))
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

	m_sStationName = wkp.GetString(sSection, STATION_NAME,_T(""));
	if (m_sStationName.IsEmpty())
	{
		m_sStationName = wkp.GetString(DATA_BASE_CLIENT, STATION_NAME,_T(""));
		wkp.DeleteSection(DATA_BASE_CLIENT);
	}

	m_QueryParameter.Load(sSection + _T("\\") + m_pUser->GetName(),wkp);
	m_sSyncTime = wkp.GetString(sSection + _T("\\") + m_pUser->GetName(), SYNC_TIME, _T(""));

	m_sVersion = wkp.GetString(_T("Version"),_T("Number"),_T(""));

	m_bSearchMaskAtStart = FALSE;
	m_bSearchMaskAtStart = wkp.GetInt(sSection, SEARCH_MASK,m_bSearchMaskAtStart);
	m_bTargetDisplay = wkp.GetInt(sSection, TARGET_DISPLAY,m_bTargetDisplay);
	m_nMaxRequestedRecords = wkp.GetInt(sSection, MAX_REQUESTED_RECORDS,m_nMaxRequestedRecords);
	m_nMaxSentSamples =  wkp.GetInt(sSection, MAX_SENT_SAMPLES,m_nMaxSentSamples);

	if (m_bIsDemo)
	{
		m_AutoLogout.SetValues(TRUE,TRUE,10,0,10,0);
	}
	else
	{
		m_AutoLogout.Load(wkp);
	}

	//get realtime option
	m_bCanPlayRealtime = wkp.GetInt(sSection, REALTIME,m_bCanPlayRealtime);
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheApp::SaveSettings()
{
	CWK_Profile wkp;
	CString sSection;
	
	sSection = SECTION_RECHERCHE;
	if (m_hDevMode)
	{
		DWORD  dwSize = GlobalSize(m_hDevMode);
		LPVOID lpVoid = GlobalLock(m_hDevMode);
		wkp.WriteBinary(sSection, DEV_MODE,(LPBYTE)lpVoid,dwSize);
		GlobalUnlock(m_hDevMode);
	}
	if (m_hDevNames)
	{
		DWORD  dwSize = GlobalSize(m_hDevNames);
		LPVOID lpVoid = GlobalLock(m_hDevNames);
		wkp.WriteBinary(sSection, DEV_NAMES,(LPBYTE)lpVoid,dwSize);
		GlobalUnlock(m_hDevNames);
	}
	if (m_pUser)
	{
		wkp.WriteString(sSection, STATION_NAME,m_sStationName);
		m_QueryParameter.Save(sSection+_T("\\") + m_pUser->GetName(),wkp);
		wkp.WriteString(sSection + _T("\\") + m_pUser->GetName(), SYNC_TIME,m_sSyncTime);
	}
	wkp.WriteInt(sSection, SEARCH_MASK,m_bSearchMaskAtStart);
	wkp.WriteInt(sSection, TARGET_DISPLAY,m_bTargetDisplay);

	//save current realtime option
	wkp.WriteInt(sSection, REALTIME,m_bCanPlayRealtime);
	wkp.WriteInt(sSection, MAX_REQUESTED_RECORDS,m_nMaxRequestedRecords);
	wkp.WriteInt(sSection, MAX_SENT_SAMPLES,m_nMaxSentSamples);
}
/////////////////////////////////////////////////////////////////////////////
// CRechercheApp initialization

BOOL CRechercheApp::InitInstance()
{
	// nur eine Instanz starten
	// auch mit Vision 3.5
	if ( WK_ALONE(WK_APP_NAME_DB_CLIENT)==FALSE )
	{
		HWND hWndFirstInstance = FindWindow(WK_APP_NAME_DB_CLIENT,NULL);
		if (hWndFirstInstance)
		{
			SetWindowPos(hWndFirstInstance,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
			SetWindowPos(hWndFirstInstance,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
		}
		return FALSE;
	}

	COemGuiApi::SplashDialog(2, m_dwMonitorFlags);

	WK_CreateDirectory(_T("c:\\out"));

	if (WK_IS_RUNNING(WK_DB_SERVER_READ_ONLY))
	{
		int p;
		CString sAutoRun;
		GetModuleFileName(AfxGetApp()->m_hInstance, sAutoRun.GetBuffer(MAX_PATH), MAX_PATH);
		sAutoRun.ReleaseBuffer();
		p = sAutoRun.ReverseFind(_T('\\'));
		sAutoRun = sAutoRun.Left(p+1);
		sAutoRun += _T("oem.ini");

		if (DoesFileExist(sAutoRun))
		{
			int oem = GetPrivateProfileInt(_T("oem"),_T("oem"),0,sAutoRun);
			if (oem>0)
			{
				m_OemCode = (CWK_Dongle::OemCode)oem;
			}
		}
		InitDebugger(_T("RechercheRO.log"),WAI_DATABASE_CLIENT);
		m_bReadOnlyModus = TRUE;

		HKEY hKey = NULL;
		CString str = BASE_DVRT_KEY;
		LONG res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, str, 0, KEY_READ, &hKey);
		if (res == ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			m_bReadOnlyModus |= REGKEY_BASEDVRT_DEFINED;
		}
		str += SECTION_RECHERCHE;
		res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, str, 0, KEY_READ, &hKey);
		if (res == ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			m_bReadOnlyModus |= REGKEY_RECHERCHE_DEFINED;
		}
	}
	else
	{
		InitDebugger(_T("Recherche.log"),WAI_DATABASE_CLIENT);
		CWK_Dongle dongle(WAI_DATABASE_CLIENT);
		
		if (dongle.RunDatabaseClient()==FALSE)
		{
			COemGuiApi::DongleFailed();
			return FALSE;
		}
		m_OemCode = dongle.GetOemCode();
		m_bAllowBackup = dongle.RunCDRWriter();
		m_bAllowMiCo = dongle.AllowSoftDecodeMiCo();
		m_bAllowCoCo = dongle.AllowSoftDecodeCoCo();
		m_bAllowPT = dongle.AllowSoftDecodePT();
		m_bRunAnyLinkUnit = dongle.RunAnyLinkUnit();
		m_bIsReceiver = dongle.IsReceiver();
		m_bIsDemo = dongle.IsDemo();

		CString sWWW;
		sWWW = CNotificationMessage::GetWWWRoot();
		m_sMapURL = sWWW + _T("\\map.htm");
		m_sDefaultURL = _T("res://recherche.exe/nostatement.htm");

		if (!DoesFileExist(m_sMapURL))
		{
			m_sMapURL.Empty();
		}

	}

	CString sTitle;
	sTitle = COemGuiApi::GetApplicationName(WAI_DATABASE_CLIENT);
	if (IsDemo())
	{
		sTitle += _T(" DEMO");
	}
   if (m_pszAppName) free((void*)m_pszAppName);
	m_pszAppName = _tcsdup(sTitle);    // human readable title
	WK_TRACE(_T("%s gestarted\n"),m_pszAppName);


	// Create a custom control manager class so we can overide the site
	CCustomOccManager *pMgr = new CCustomOccManager;
	// Create an IDispatch class for extending the Dynamic HTML Object Model 
	m_pDispOM = new CImpIDispatch;
	// Set our control containment up but using our control container 
	// management class instead of MFC's default
	AfxEnableControlContainer(pMgr);

	AfxInitRichEdit();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#if _MFC_VER < 0x0700
	#ifdef _AFXDLL
		Enable3dControls();			// Call this when using MFC in a shared DLL
	#else
		Enable3dControlsStatic();	// Call this when linking to MFC statically
	#endif
#endif

	WK_STAT_LOG(_T("Reset"),1,_T("ServerActive"));

#ifndef _DEBUG
	m_nCmdShow = SW_SHOWMAXIMIZED;
#endif

	RegisterWindowClass();
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CRechercheDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CRechercheView));
	AddDocTemplate(pDocTemplate);

	CString sCommand = m_lpCmdLine;
	m_bBackupModus = (-1!=sCommand.Find(_T("-backup")));
	if (m_bBackupModus)
	{
		WK_TRACE(_T("starting in backup mode\n"));
	}

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	if (!Login())
	{
		return FALSE;
	}

	LoadSettings();

	InitFieldMap();

	m_pCIPCServerControlRecherche = new CIPCServerControlRecherche();

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
int CRechercheApp::ExitInstance() 
{
	SaveSettings();

	WK_DELETE(m_pCIPCServerControlRecherche);

	WK_STAT_LOG(_T("Reset"),0,_T("ServerActive"));

	if (m_pUser)
	{
		WK_TRACE_USER(_T("%s hat sich ausgeloggt. Programmende\n"),m_pUser->GetName());
	}
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
		else if (!(m_bReadOnlyModus & REGKEY_RECHERCHE_DEFINED))
		{
			str += SECTION_RECHERCHE;
			wkpAbs.DeleteSection(str);
		}
	}

	CloseDebugger();

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheApp::RegisterWindowClass()
{
	WNDCLASS  wndclass ;

	// register window class
	wndclass.style =         CS_DBLCLKS|CS_HREDRAW;
	wndclass.lpfnWndProc =   DefWindowProc;
	wndclass.cbClsExtra =    0;
	wndclass.cbWndExtra =    0;
	wndclass.hInstance =     m_hInstance;
	wndclass.hIcon =         LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME));
	wndclass.hCursor =       LoadCursor(IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1) ;
	wndclass.lpszMenuName =  0L;
	wndclass.lpszClassName = WK_APP_NAME_DB_CLIENT;

	// main app window
	AfxRegisterClass(&wndclass);

	wndclass.hIcon = NULL;
	wndclass.hCursor = NULL;
	wndclass.lpszClassName = SMALL_WINDOW_CLASS;

	// CSmallWindow
	AfxRegisterClass(&wndclass);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRechercheApp::LoginAsSuperVisor()
{
	CWK_Profile wkp;
	WK_DELETE(m_pUser);
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
BOOL CRechercheApp::Login()
{
#ifdef _DEBUG
//	return LoginAsSuperVisor();
#endif

	if (m_bReadOnlyModus)
	{
		return LoginAsSuperVisor();
	}

	CWK_Profile wkp;
	CPermissionArray pa;
	CPermission* pPermission = pa.GetSpecialReceiver();
	if (pPermission)
	{
		WK_DELETE(m_pUser);
		m_pUser = new CUser();
		m_pUser->SetName(pPermission->GetName());
		WK_DELETE(m_pPermission);
		m_pPermission = new CPermission(*pPermission);
		WK_TRACE_USER(_T("%s hat sich eingeloggt.\n"),m_pUser->GetName());
		return TRUE;
	}
	else
	{
		COEMLoginDialog dlg(AfxGetMainWnd());

		if (IDOK==dlg.DoModal())
		{
			if (m_pUser)
			{
				WK_TRACE_USER(_T("%s hat sich ausgeloggt.\n"),m_pUser->GetName());
			}
			WK_DELETE(m_pUser);
			m_pUser = new CUser(dlg.GetUser());
			WK_DELETE(m_pPermission);
			m_pPermission = new CPermission(dlg.GetPermission());
			if (m_pUser)
			{
				WK_TRACE_USER(_T("%s hat sich eingeloggt.\n"),m_pUser->GetName());
			}

			return (m_pUser && m_pUser->GetID()!=SECID_NO_ID) &&
				   (m_pPermission && m_pPermission->GetID()!=SECID_NO_ID);
		}
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
// CRechercheApp commands
void CRechercheApp::OnEditStation() 
{
	CStationNameDialog dlg(AfxGetMainWnd());

	dlg.m_sStationName = m_sStationName;
	if (IDOK==dlg.DoModal())
	{
		m_sStationName = dlg.m_sStationName;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheApp::OnFilePrintSetup() 
{
	CPrintDialog pd(TRUE);
	int nResponse = DoPrintDialog(&pd);
	if (IDOK==nResponse)
	{
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRechercheApp::PreTranslateMessage(MSG* pMsg) 
{
	if ( (pMsg->message == WM_KEYDOWN) || 
		 (pMsg->message == WM_LBUTTONDOWN) ||
		 (pMsg->message == WM_RBUTTONDOWN))
	{
		if (!m_bIsDemo)
		{
			m_AutoLogout.ResetApplication();
			CRechercheDoc* pDoc = (CRechercheDoc*)((CMainFrame*)m_pMainWnd)->GetActiveDocument();
			pDoc->ResetConnectionAutoLogout();
		}
	}
	return CWinApp::PreTranslateMessage(pMsg);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRechercheApp::OnIdle(LONG lCount) 
{
	if (WK_IS_WINDOW(m_pMainWnd))
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
	
	return CWinApp::OnIdle(lCount);
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheApp::OnSearchAtStart() 
{
	m_bSearchMaskAtStart = !m_bSearchMaskAtStart;
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheApp::OnUpdateSearchAtStart(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bSearchMaskAtStart);
}
////////////////////////////////////////////////////////////////////////////
CHostArray& CRechercheApp::GetHosts()
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
		pHost = dvhosts.GetAt(i);
		BOOL bFound = FALSE;

		for (int j=0;j<m_Hosts.GetSize();j++)
		{
			if (m_Hosts.GetAt(j)->GetNumber() == pHost->GetNumber())
			{
				// Nummer gleich
				bFound = TRUE;
				if (m_Hosts.GetAt(j)->GetName() != pHost->GetName())
				{
					// Name verschieden
					m_Hosts.GetAt(j)->SetName(pHost->GetName());
					bChanged = TRUE;
				}
				break;
			}
			else if (    m_Hosts.GetAt(j)->GetName() == pHost->GetName()
				     && pHost->GetName() != _T("Standard"))
			{
				// Name gleich
				bFound = TRUE;
				if (m_Hosts.GetAt(j)->GetNumber() != pHost->GetNumber())
				{
					// Nummer verschieden
					m_Hosts.GetAt(j)->SetNumber(pHost->GetNumber());
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
/////////////////////////////////////////////////////////////////////////////
void CRechercheApp::OnNewLogin() 
{
	CUser* pOldUser;
	CPermission* pOldPermission;

	pOldUser = new CUser(*m_pUser);
	pOldPermission = new CPermission(*m_pPermission);

	if (Login())
	{
		CRechercheDoc* pDoc = (CRechercheDoc*)((CMainFrame*)m_pMainWnd)->GetActiveDocument();
		pDoc->DisconnectAll();
	}
	else
	{
		if (m_pUser)
		{
			WK_TRACE_USER(_T("%s hat sich ausgeloggt\n"),m_pUser->GetName());
		}
		WK_DELETE(m_pUser);
		WK_DELETE(m_pPermission);
		m_pUser = new CUser(*pOldUser);
		m_pPermission = new CPermission(*pOldPermission);
	}
	WK_DELETE(pOldUser);
	WK_DELETE(pOldPermission);
}
///////////////////////////////////////////////////////////////////////////
void CRechercheApp::OnViewTargetDisplay() 
{
	m_bTargetDisplay = !m_bTargetDisplay;
	if (WK_IS_WINDOW(m_pMainWnd))
	{
		m_pMainWnd->RedrawWindow();
	}
}
///////////////////////////////////////////////////////////////////////////
void CRechercheApp::OnUpdateViewTargetDisplay(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bTargetDisplay);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CRechercheApp::CanPlayRealtime()
{
	return m_bCanPlayRealtime;
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheApp::OnSequenceRealtime() 
{
	m_bCanPlayRealtime = !m_bCanPlayRealtime;
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheApp::OnUpdateSequenceRealtime(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bCanPlayRealtime);
}
