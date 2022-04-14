// Vision.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"

#include "Vision.h"

#include "MainFrm.h"
#include "VisionDoc.h"
#include "VisionView.h"
#include "CocoWindow.h"
#include "OptionsDialog.h"

// NOTE: THis line is a hardcoded reference to an MFC header file
//  this path may need to be changed to refer to the location of VC5 install
//  for successful compilation.
// #include <..\src\occimpl.h>
// GF moved to custsite.h, workaround for VS_70, but still errors
// GF moving to custsite.h results in unresolved external 8-(
#if _MSC_VER >= 1300
	#include <occimpl.h>
#else
	#include <..\src\occimpl.h>
#endif

#include "custsite.h"

/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
#define	DISABLESMALL		_T("DisableSmall")
#define	DISABLEZOOM			_T("DisableZoom")
#define	CLOSEALARMWINDOW	_T("CloseAlarmWindow")
#define	DISABLERIGHTCLICK	_T("DisableRightClick")
#define	DISABLEDBLCLICK	_T("DisableDblClick")
#define	SHOWTITLE_OF_SW   _T("ShowSmallTitles")

/////////////////////////////////////////////////////////////////////////////
static _TCHAR szLoginTimeoutSec[] = _T("LoginTimeoutSec");
static _TCHAR szFile[] = _T("file://");
static _TCHAR szHttp[] = _T("http://");

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CVisionApp, CWinApp)
	//{{AFX_MSG_MAP(CVisionApp)
	ON_COMMAND(ID_NEW_LOGIN, OnNewLogin)
	ON_COMMAND(ID_OPTIONS, OnOptions)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS, OnUpdateOptions)
	ON_COMMAND(ID_VIEW_TARGET_DISPLAY, OnViewTargetDisplay)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TARGET_DISPLAY, OnUpdateViewTargetDisplay)
	//}}AFX_MSG_MAP
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
CVisionApp::CVisionApp()
	: m_MaxMegraRect(0,0,710,560)
{
	// public:
	// m_AutoLogout;
	m_bRunAnyLinkUnit = FALSE;

	m_pDongle = NULL;
	m_pUser = NULL;
	m_pPermission = NULL;

	m_bTraceBitrate = FALSE;
	m_bTraceDelay = FALSE;
	m_bTraceSwitch = FALSE;
	m_bTraceImage = FALSE;
	m_bTraceSequence = FALSE;
	m_bWarnIfAudioIsBlockedAtNo = 16;

	m_dwInitialTuning = 1000;
	m_dwSequenceIntervall = 5;

#ifdef _DTS
	m_bCorrectBitrate = FALSE;
#else
	m_bCorrectBitrate = TRUE;
#endif
	m_bCoCoQuality = FALSE;
	m_bCoCoFit = FALSE;
	m_bCoCoSoft = TRUE;
	m_bConnectToLocalServer = TRUE;
	m_bRS232Transparent = FALSE;

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

	m_dwMonitorFlags = USE_MONITOR_1;
	m_bShowTitleOfSmallWindows = TRUE;

	m_hPanLeft = NULL;
	m_hPanRight = NULL;
	m_hTiltUp = NULL;
	m_hTiltDown = NULL;
	m_hZoomIn = NULL;
	m_hZoomOut = NULL;
	m_hArrow = NULL;

	m_MaxMegraRect = 0;
	// m_sVersion

	m_pDispOM = NULL;
	GetCurrentDirectory(MAX_PATH, m_sMapImagePath.GetBuffer(MAX_PATH));
	m_sMapImagePath.ReleaseBuffer();

	// m_ErrorMessages

	// private:
	m_bIsReceiver = FALSE;
	// m_sDefaultURL
	// m_sMapURL
	// m_Hosts

	m_wHtmlWindowID = 0;
}
/////////////////////////////////////////////////////////////////////////////
CVisionApp::~CVisionApp()
{
	WK_DELETE(m_pDispOM);
}
/////////////////////////////////////////////////////////////////////////////
// The one and only CVisionApp object
CVisionApp theApp;
CWK_Timer theTimer;

/////////////////////////////////////////////////////////////////////////////
// CVisionApp initialization
void CVisionApp::LoadSettings()
{
	CWKDebugOptions debugOptions;
	CWK_Profile wkp;

	CString sDebugFile = _T("c:\\Vision.cfg");
	BOOL bLocal = TRUE;
	
	// check for C:\ExplorerDebug.cfg first, then in current directory
	BOOL bFoundFile = debugOptions.ReadFromFile(sDebugFile);
	
	if (bFoundFile)
	{
		m_bTraceBitrate = debugOptions.GetValue(_T("TraceBitrate"), m_bTraceBitrate);
		m_bTraceDelay = debugOptions.GetValue(_T("TraceDelay"), m_bTraceDelay);
		m_bTraceSwitch = debugOptions.GetValue(_T("TraceSwitch"), m_bTraceSwitch);
		m_bTraceImage = debugOptions.GetValue(_T("TraceImage"), m_bTraceImage);
		m_bCorrectBitrate = debugOptions.GetValue(_T("CorrectBitrate"), m_bCorrectBitrate); 
		m_dwInitialTuning = debugOptions.GetValue(_T("InitialTuning"), m_dwInitialTuning); 
		m_bCoCoQuality = debugOptions.GetValue(_T("CoCoQuality"), m_bCoCoQuality); 
		m_bCoCoFit = debugOptions.GetValue(_T("CoCoFit"), m_bCoCoFit); 
		m_bCoCoSoft = debugOptions.GetValue(_T("CoCoSoft"), m_bCoCoSoft); 
		bLocal = debugOptions.GetValue(_T("ConnectLocal"), bLocal); 
		SaveSettings();
		TRY
		{
			CFile::Remove(sDebugFile);
		}
		WK_CATCH(_T("cannot delete vision.cfg"))
	}
	else
	{
		m_bTraceBitrate = wkp.GetInt(VISION_SETTINGS, _T("TraceBitrate"), m_bTraceBitrate);
		m_bTraceDelay = wkp.GetInt(VISION_SETTINGS,_T("TraceDelay"), m_bTraceDelay);
		m_bTraceSwitch = wkp.GetInt(VISION_SETTINGS,_T("TraceSwitch"), m_bTraceSwitch);
		m_bTraceImage = wkp.GetInt(VISION_SETTINGS,_T("TraceImage"), m_bTraceImage);
		m_bStatImage = wkp.GetInt(VISION_SETTINGS,_T("StatImage"), m_bStatImage);
		m_bTraceSequence = wkp.GetInt(VISION_SETTINGS,_T("TraceSequence"), m_bTraceSequence);
		m_bWarnIfAudioIsBlockedAtNo = wkp.GetInt(VISION_SETTINGS,_T("WarnIfAudioIsBlockedAtNo"), m_bWarnIfAudioIsBlockedAtNo);

		m_bCorrectBitrate = wkp.GetInt(VISION_SETTINGS,_T("CorrectBitrate"), m_bCorrectBitrate); 
		m_bCoCoQuality = wkp.GetInt(VISION_SETTINGS,_T("CoCoQuality"), m_bCoCoQuality); 
		m_bCoCoFit = wkp.GetInt(VISION_SETTINGS,_T("CoCoFit"), m_bCoCoFit); 
		m_bCoCoSoft = wkp.GetInt(VISION_SETTINGS,_T("CoCoSoft"), m_bCoCoSoft); 
		bLocal = wkp.GetInt(VISION_SETTINGS,_T("ConnectLocal"), bLocal); 
		CCocoWindow::m_bShowFPS = wkp.GetInt(VISION_SETTINGS,_T("ShowCocoFPS"), CCocoWindow::m_bShowFPS); 
		bLocal = wkp.GetInt(VISION_SETTINGS,_T("ConnectToLocalServer"), m_bConnectToLocalServer);

		m_bDisableSmall      = wkp.GetInt(VISION_SETTINGS, DISABLESMALL     ,m_bDisableSmall);
		m_bDisableZoom       = wkp.GetInt(VISION_SETTINGS, DISABLEZOOM      ,m_bDisableZoom);
		m_bCloseAlarmWindow  = wkp.GetInt(VISION_SETTINGS, CLOSEALARMWINDOW ,m_bCloseAlarmWindow);
		m_bDisableRightClick = wkp.GetInt(VISION_SETTINGS, DISABLERIGHTCLICK,m_bDisableRightClick);
		m_bDisableDblClick   = wkp.GetInt(VISION_SETTINGS, DISABLEDBLCLICK,  m_bDisableDblClick);
		m_bShowTitleOfSmallWindows = wkp.GetInt(VISION_SETTINGS, SHOWTITLE_OF_SW,m_bShowTitleOfSmallWindows);
	}

	m_bFirstCam = wkp.GetInt(VISION_SETTINGS,_T("FirstCam"),m_bFirstCam);
	m_b1PlusAlarm = wkp.GetInt(VISION_SETTINGS,_T("1PlusAlarm"),m_b1PlusAlarm);
	m_bFullscreenAlarm = wkp.GetInt(VISION_SETTINGS,_T("FullscreenAlarm"),m_bFullscreenAlarm);
	m_bRS232Transparent = wkp.GetInt(_T("Communit"),_T("COM"),0)>0;
	m_bAnimateAlarmConnections = wkp.GetInt(VISION_SETTINGS,_T("AnimateAlarmConnections"), m_bAnimateAlarmConnections);
	m_sVersion = wkp.GetString(_T("Version"),_T("Number"),_T(""));
	m_sMapImagePath = wkp.GetString(VISION_SETTINGS, _T("MapImagePath"), m_sMapImagePath);
	
	WK_TRACE(_T("InitialTuning = %d\n"),m_dwInitialTuning);
	WK_TRACE(_T("CorrectBitrate = %d\n"),m_bCorrectBitrate);
	WK_TRACE(_T("TraceDelay = %d\n"),m_bTraceDelay);
	WK_TRACE(_T("TraceBitrate = %d\n"),m_bTraceBitrate);
	WK_TRACE(_T("TraceSwitch = %d\n"),m_bTraceSwitch);
	WK_TRACE(_T("TraceImage = %d\n"),m_bTraceImage);
	WK_TRACE(_T("StatImage = %d\n"),m_bStatImage);
	WK_TRACE(_T("TraceSequence = %d\n"),m_bTraceSequence);
	WK_TRACE(_T("CoCoQuality = %d\n"),m_bCoCoQuality);
	WK_TRACE(_T("CoCoFit = %d\n"),m_bCoCoFit);
	WK_TRACE(_T("CoCoSoft = %d\n"),m_bCoCoSoft);
	WK_TRACE(_T("FirstCam = %d\n"),m_bFirstCam);
	WK_TRACE(_T("1PlusAlarm = %d\n"),m_b1PlusAlarm);
	WK_TRACE(_T("FullcreenAlarm = %d\n"),m_bFullscreenAlarm);
	WK_TRACE(_T("RS232Transparent = %d\n"),m_bRS232Transparent);
	WK_TRACE(_T("AnimateAlarmConnections = %d\n"),m_bAnimateAlarmConnections);

	if (m_pDongle->IsTransmitter() || m_pDongle->IsLocal())
	{
		m_bConnectToLocalServer = bLocal;
	}
	else if (m_pDongle->IsReceiver())
	{
		m_bConnectToLocalServer = FALSE;
	}
	WK_TRACE(_T("ConnectToLocalServer = %d\n"), m_bConnectToLocalServer);

	if (m_pDongle->IsDemo())
	{
		m_AutoLogout.SetValues(TRUE,TRUE,10,0,10,0);
	}
	else
	{
		m_AutoLogout.Load(wkp);
	}
	CheckHostMap();
}
/////////////////////////////////////////////////////////////////////////////
// CVisionApp initialization
void CVisionApp::LoadUserSpecificSettings()
{
	if (m_pUser)
	{
		CWK_Profile wkp;
		CString sSection;
		sSection.Format(_T("%s\\%08lx"), VISION_SETTINGS, m_pUser->GetID().GetID());
		m_dwSequenceIntervall = wkp.GetInt(sSection,_T("Sequence"),m_dwSequenceIntervall);
		m_bTargetDisplay = wkp.GetInt(sSection,_T("TargetDisplay"),m_bTargetDisplay);
		CVisionDoc* pDoc = ((CMainFrame*)m_pMainWnd)->GetDocument();
		if (pDoc)
		{
			pDoc->SetSequenceTime(m_dwSequenceIntervall);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVisionApp::SaveSettings()
{
	CWK_Profile wkp;
	wkp.WriteInt(VISION_SETTINGS,_T("AnimateAlarmConnections"),m_bAnimateAlarmConnections);
	wkp.WriteInt(VISION_SETTINGS,_T("TraceBitrate"),m_bTraceBitrate);
	wkp.WriteInt(VISION_SETTINGS,_T("TraceDelay"),m_bTraceDelay);
	wkp.WriteInt(VISION_SETTINGS,_T("TraceSwitch"),m_bTraceSwitch);
	wkp.WriteInt(VISION_SETTINGS,_T("TraceImage"),m_bTraceImage);
	wkp.WriteInt(VISION_SETTINGS,_T("StatImage"),m_bStatImage);
	wkp.WriteInt(VISION_SETTINGS,_T("TraceSequence"),m_bTraceSequence);
	wkp.WriteInt(VISION_SETTINGS,_T("WarnIfAudioIsBlockedAtNo"), m_bWarnIfAudioIsBlockedAtNo);
	wkp.WriteInt(VISION_SETTINGS,_T("CorrectBitrate"),m_bCorrectBitrate);
	wkp.WriteInt(VISION_SETTINGS,_T("CoCoQuality"),m_bCoCoQuality);
	wkp.WriteInt(VISION_SETTINGS,_T("CoCoFit"),m_bCoCoFit);
	wkp.WriteInt(VISION_SETTINGS,_T("CoCoSoft"),m_bCoCoSoft);
	wkp.WriteInt(VISION_SETTINGS,_T("ConnectToLocalServer"),m_bConnectToLocalServer);
	wkp.WriteInt(VISION_SETTINGS,_T("Settings"),1);
	wkp.WriteInt(VISION_SETTINGS,_T("FirstCam"),m_bFirstCam);
	wkp.WriteInt(VISION_SETTINGS,_T("1PlusAlarm"),m_b1PlusAlarm);
	wkp.WriteInt(VISION_SETTINGS,_T("FullscreenAlarm"),m_bFullscreenAlarm);
	wkp.WriteInt(VISION_SETTINGS,_T("ShowCocoFPS"),CCocoWindow::m_bShowFPS);
	wkp.WriteString(VISION_SETTINGS, _T("MapImagePath"), m_sMapImagePath);

	wkp.WriteInt(VISION_SETTINGS, DISABLESMALL     , m_bDisableSmall);
	wkp.WriteInt(VISION_SETTINGS, DISABLEZOOM      , m_bDisableZoom);
	wkp.WriteInt(VISION_SETTINGS, CLOSEALARMWINDOW , m_bCloseAlarmWindow);
	wkp.WriteInt(VISION_SETTINGS, DISABLERIGHTCLICK, m_bDisableRightClick);
	wkp.WriteInt(VISION_SETTINGS, DISABLEDBLCLICK  , m_bDisableDblClick);
	wkp.WriteInt(VISION_SETTINGS, SHOWTITLE_OF_SW  , m_bShowTitleOfSmallWindows);

//	wkp.WriteInt(VISION_SETTINGS, USE_MONITOR  , m_dwMonitorFlags);

	if (m_pUser)
	{
		CString sSection;
		sSection.Format(_T("%s\\%08lx"),VISION_SETTINGS, m_pUser->GetID().GetID());
		wkp.WriteInt(sSection,_T("Sequence"),m_dwSequenceIntervall);
		wkp.WriteInt(sSection,_T("TargetDisplay"),m_bTargetDisplay);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CVisionApp::SplitCommandLine(const CString sCommandLine, CStringArray& saParameters)
{
	CString sCL = sCommandLine;
	CString sOne;
	int p;

	sCL.TrimLeft();
	p = sCL.Find(' ');
	while (p!=-1)
	{
		sOne = sCL.Left(p);
		sCL = sCL.Mid(p+1);
		sCL.TrimLeft();
		saParameters.Add(sOne);
		p = sCL.Find(' ');
	}
	if (!sCL.IsEmpty())
	{
		saParameters.Add(sCL);
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CVisionApp::InitInstance()
{
	// nur eine Instanz starten
	// auch mit Vision 3.5
	if ( WK_ALONE(WK_APP_NAME_EXPLORER)==FALSE )
	{
		HWND hWndFirstInstance = FindWindow(WK_APP_NAME_EXPLORER,NULL);
		if (hWndFirstInstance)
		{
			SetWindowPos(hWndFirstInstance,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
			SetWindowPos(hWndFirstInstance,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
		}
		return FALSE;
	}
	
	InitDebugger(_T("Vision.log"),WAI_EXPLORER);

	m_pDongle = new CWK_Dongle();
	if (!m_pDongle->IsValid())
	{
		AfxMessageBox(IDP_INVALID_DONGLE);
		return FALSE;
	}

	CString sTitle;
	sTitle = COemGuiApi::GetApplicationName(WAI_EXPLORER);
	if (m_pDongle->IsDemo())
	{
		sTitle += _T(" DEMO");
	}

	if (m_pszAppName)
	{
		free((void*)m_pszAppName);
	}
	m_pszAppName = _tcsdup(sTitle);    // human readable title
#ifndef _DTS
	CWK_Profile wkp; 
	m_dwMonitorFlags   = wkp.GetInt(VISION_SETTINGS, USE_MONITOR  , m_dwMonitorFlags);
#endif

	COemGuiApi::SplashDialog(2,m_dwMonitorFlags);
	
	// Create a custom control manager class so we can overide the site
	CCustomOccManager *pMgr = new CCustomOccManager;
	// Create an IDispatch class for extending the Dynamic HTML Object Model 
	m_pDispOM = new CImpIDispatch;
	// Set our control containment up but using our control container 
	// management class instead of MFC's default
	AfxEnableControlContainer(pMgr);

#if _MFC_VER < 0x0700
	#ifdef _AFXDLL
		Enable3dControls();			// Call this when using MFC in a shared DLL
	#else
		Enable3dControlsStatic();	// Call this when linking to MFC statically
	#endif
#endif

	m_bRunAnyLinkUnit = m_pDongle->RunAnyLinkUnit() || m_pDongle->RunAnyBoxUnit();
	m_bIsReceiver = m_pDongle->IsReceiver();

	WK_STAT_LOG(_T("Reset"),0,_T("ServerActive"));
	WK_STAT_LOG(_T("Reset"),1,_T("ServerActive"));
	LoadSettings();

#ifndef _DEBUG
	m_nCmdShow = SW_SHOWMAXIMIZED;
#endif

	RegisterWindowClass();

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CVisionDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CVisionView));
	AddDocTemplate(pDocTemplate);

	OnFileNew();

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->SetWindowText(m_pszAppName);
	m_pMainWnd->UpdateWindow();

	CString s = m_lpCmdLine;
	CStringArray saParameters;
	SplitCommandLine(m_lpCmdLine, saParameters);

	s.TrimLeft();
	s.TrimRight();

	if (   (saParameters.GetSize()==0)
		|| (s.IsEmpty())
		)
	{
		if (!Login())
		{
			return FALSE;
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
		m_bConnectToLocalServer = FALSE;
	}

	if (m_bConnectToLocalServer)
	{
		m_pMainWnd->PostMessage(WM_COMMAND,ID_CONNECT_LOCAL);
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
int CVisionApp::ExitInstance() 
{
	SaveSettings();

	WK_STAT_LOG(_T("Reset"),0,_T("ServerActive"));
	if (m_pUser)
	{
		WK_TRACE_USER(_T("%s hat sich ausgeloggt, Programmende\n"),m_pUser->GetName());
	}

	WK_DELETE(m_pDongle);
	WK_DELETE(m_pUser);
	WK_DELETE(m_pPermission);

	CloseDebugger();

	return 0L;
}
/////////////////////////////////////////////////////////////////////////////
void CVisionApp::RegisterWindowClass()
{
	WNDCLASS  wndclass;

	// register window class
	wndclass.style =         CS_DBLCLKS|CS_HREDRAW;
	wndclass.lpfnWndProc =   DefWindowProc;
	wndclass.cbClsExtra =    0;
	wndclass.cbWndExtra =    0;
	wndclass.hInstance =     m_hInstance;
	wndclass.hCursor =       LoadCursor(IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1) ;
	wndclass.lpszMenuName =  0L;
	wndclass.lpszClassName = WK_APP_NAME_EXPLORER;

	wndclass.hIcon = LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME_IST));

	// main app window
	AfxRegisterClass(&wndclass);

	wndclass.hIcon = NULL;
	wndclass.hCursor = NULL;
	wndclass.lpszClassName = DISPLAY_WINDOW_CLASS;

	// CSmallWindow
	AfxRegisterClass(&wndclass);

	m_hPanLeft  = LoadCursor(IDC_PANLEFT);
	m_hPanRight = LoadCursor(IDC_PANRIGHT);
	m_hTiltUp   = LoadCursor(IDC_TILTUP);
	m_hTiltDown = LoadCursor(IDC_TILTDOWN);
	m_hZoomIn   = LoadCursor(IDC_ZOOMIN);
	m_hZoomOut  = LoadCursor(IDC_ZOOMOUT);
	m_hArrow	= LoadStandardCursor(IDC_ARROW);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CVisionApp::LoginAsSuperVisor()
{
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
BOOL CVisionApp::Login(LPCTSTR pszUser /*= NULL*/,LPCTSTR pszPassword /*= NULL*/)
{
	CWK_Profile wkp;
	if (m_pUser)
	{
		CString sSection;
		sSection.Format(_T("%s\\%08lx"),VISION_SETTINGS, m_pUser->GetID().GetID());
		wkp.WriteInt(sSection,_T("Sequence"),m_dwSequenceIntervall);
	}
#ifdef _DEBUG
//	return LoginAsSuperVisor();
#endif

	CString sUser(pszUser);
	CString sPassword(pszPassword);
	BOOL bRet = FALSE;

	if (sPassword.IsEmpty())
	{
		// check user and password
		// only if one of them is not empty
#ifndef _DTS
		if (!sUser.IsEmpty())
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
				WK_DELETE(m_pUser);
				m_pUser = new CUser();
				m_pUser->SetName(pPermission->GetName());
				WK_DELETE(m_pPermission);
				m_pPermission = new CPermission(*pPermission);
				// Remove some menu entries for SpecialReceiver
				CMenu* pMenu = AfxGetMainWnd()->GetMenu();
				if (pMenu != NULL)
				{
					pMenu = pMenu->GetSubMenu(0);
					if (pMenu != NULL)
					{
						pMenu->DeleteMenu(ID_NEW_LOGIN, MF_BYCOMMAND);
						pMenu->DeleteMenu(ID_CONNECT_LOCAL, MF_BYCOMMAND);
					}
					// force a redraw of the menu bar NEEDED
					AfxGetMainWnd()->DrawMenuBar();
				}

				bRet = TRUE;
			}
			else
			{
				CWK_Profile wkp;
				WORD wTimeoutSec = 10;
				wTimeoutSec = (WORD)wkp.GetInt(VISION_SETTINGS, szLoginTimeoutSec, wTimeoutSec);
				wkp.WriteInt(VISION_SETTINGS, szLoginTimeoutSec, wTimeoutSec);
				COEMLoginDialog dlg(m_pMainWnd, 0, LOGIN_ICON_NO, wTimeoutSec);
				if (IDOK==dlg.DoModal())
				{
					WK_DELETE(m_pUser);
					m_pUser = new CUser(dlg.GetUser());
					WK_DELETE(m_pPermission);
					m_pPermission = new CPermission(dlg.GetPermission());

					bRet = (m_pUser && m_pUser->GetID()!=SECID_NO_ID) &&
						   (m_pPermission && m_pPermission->GetID()!=SECID_NO_ID);
				}
				else
				{
					bRet = FALSE;
				}
			}
		}
	}
	else
	{
		bRet = CheckUserPassword(sUser,sPassword);
		if (!bRet)
		{
			WK_TRACE_USER(_T("Login Fehler! Nutzer:%s\n"),(LPCTSTR)sUser);
		}
	}

	if (bRet)
	{
		WK_TRACE_USER(_T("Login Nutzer:%s, Berechtigungsstufe:%s\n"),m_pUser->GetName(),m_pPermission->GetName());
		LoadUserSpecificSettings();
	}

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CVisionApp::CheckUserPassword(CString& sUser, CString& sPassword)
{
	CUserArray UserArray;
	CSecID  idUser;
	BOOL bRet = FALSE;
	CPermissionArray PermissionArray;
	CPermission* pPermission;
	CWK_Profile wkp;

	if (sUser==_T("SuperVisor"))
	{
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
BOOL CVisionApp::ProcessMessageFilter(int code, LPMSG lpMsg) 
{
	CWnd *pWnd = CWnd::FromHandle(lpMsg->hwnd);
	CVideoSNDialog* pVideoSNDialog = NULL;

	if (WK_IS_WINDOW(pWnd))
	{
		if (pWnd->IsKindOf(RUNTIME_CLASS(CVideoSNDialog)))
		{
			pVideoSNDialog = (CVideoSNDialog*)pWnd;
		}
		else
		{
			// Use ::IsChild to get messages that may be going
			// to the dialog's controls.  In the case of
			// WM_KEYDOWN this is required.
			CWnd* pParent = pWnd->GetParent();
			if (pParent && pParent->IsKindOf(RUNTIME_CLASS(CVideoSNDialog)))
			{
				pVideoSNDialog = (CVideoSNDialog*)pParent;
			}
		}
	}

	if (pVideoSNDialog)
	{
		if (lpMsg->message == WM_KEYDOWN)
		{
			if (((VK_PRIOR<=lpMsg->wParam) && (lpMsg->wParam<=VK_DOWN)) ||
				((VK_NUMPAD1<=lpMsg->wParam) && (lpMsg->wParam<=VK_NUMPAD9)) ||
				(VK_INSERT==lpMsg->wParam) || (VK_DELETE==lpMsg->wParam))
			{
				pVideoSNDialog->OnKeyDown(lpMsg->wParam,
											LOWORD(lpMsg->lParam),
											HIWORD(lpMsg->lParam));
				return TRUE;
			}
		}
		if (lpMsg->message == WM_KEYUP)
		{
			if (((VK_PRIOR<=lpMsg->wParam) && (lpMsg->wParam<=VK_DOWN)) ||
				((VK_NUMPAD1<=lpMsg->wParam) && (lpMsg->wParam<=VK_NUMPAD9)) ||
				(VK_INSERT==lpMsg->wParam) || (VK_DELETE==lpMsg->wParam))
			{
				pVideoSNDialog->OnKeyUp(lpMsg->wParam,
											LOWORD(lpMsg->lParam),
											HIWORD(lpMsg->lParam));
				return TRUE;
			}
		}
	}

	return CWinApp::ProcessMessageFilter(code, lpMsg);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CVisionApp::PreTranslateMessage(MSG* pMsg) 
{
	if ( (pMsg->message == WM_KEYDOWN) || 
		 (pMsg->message == WM_LBUTTONDOWN) ||
		 (pMsg->message == WM_RBUTTONDOWN))
	{
		m_bAlarmAnimation = FALSE;
		if (m_pDongle
			&& !m_pDongle->IsDemo())
		{
			m_AutoLogout.ResetApplication();
			CVisionDoc* pDoc = (CVisionDoc*)((CMainFrame*)m_pMainWnd)->GetActiveDocument();
			if (pDoc) 
			{
				pDoc->ResetConnectionAutoLogout();
			}
		}
	}
	return CWinApp::PreTranslateMessage(pMsg);
}
/////////////////////////////////////////////////////////////////////////////
void CVisionApp::OnNewLogin() 
{
	CUser* pOldUser;
	CPermission* pOldPermission;

	pOldUser = new CUser(*m_pUser);
	pOldPermission = new CPermission(*m_pPermission);

	if (Login())
	{
		CVisionDoc* pDoc = (CVisionDoc*)((CMainFrame*)m_pMainWnd)->GetActiveDocument();
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
/////////////////////////////////////////////////////////////////////////////
void CVisionApp::AlarmConnection()
{
	if (m_bAnimateAlarmConnections)
	{
		m_bAlarmAnimation = TRUE;
	}
	if (WK_IS_WINDOW(m_pMainWnd))
	{
		if (m_pMainWnd->IsIconic())
		{
			m_pMainWnd->ShowWindow(SW_SHOWNORMAL);
		}
		SetWindowPos(m_pMainWnd->m_hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
		SetWindowPos(m_pMainWnd->m_hWnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
	}
}
////////////////////////////////////////////////////////////////////////////
void CVisionApp::OnErrorMessage()
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
CHostArray& CVisionApp::GetHosts()
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
////////////////////////////////////////////////////////////////////////////
void CVisionApp::OnOptions() 
{
	COptionsDialog dlg;

	dlg.m_b1PlusAlarm = m_b1PlusAlarm;
	dlg.m_bAudibleAlarm = m_bAnimateAlarmConnections;
	dlg.m_bFullScreenAlarm = m_bFullscreenAlarm;
	dlg.m_iFirstCam = m_bFirstCam ? 0 : 1;

	if (IDOK==dlg.DoModal())
	{
		m_b1PlusAlarm = dlg.m_b1PlusAlarm;
		m_bAnimateAlarmConnections = dlg.m_bAudibleAlarm;
		m_bFullscreenAlarm = dlg.m_bFullScreenAlarm;
		m_bFirstCam = dlg.m_iFirstCam == 0;
	}
}
////////////////////////////////////////////////////////////////////////////
void CVisionApp::OnUpdateOptions(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(   m_pPermission 
				   && (   m_pPermission->IsSuperVisor()
				       || m_pPermission->IsSpecialReceiver())
					   );
}
////////////////////////////////////////////////////////////////////////////
void CVisionApp::OnViewTargetDisplay() 
{
	m_bTargetDisplay = !m_bTargetDisplay;
}
////////////////////////////////////////////////////////////////////////////
void CVisionApp::OnUpdateViewTargetDisplay(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bTargetDisplay);
}
////////////////////////////////////////////////////////////////////////////
void CVisionApp::CheckHostMap()
{
	CString sWWW;

	sWWW = CNotificationMessage::GetWWWRoot();
	m_sMapURL = sWWW + _T("\\map.htm");
	m_sDefaultURL = _T("res://vision.exe/nostatement.htm");

	if (!DoesFileExist(m_sMapURL))
	{
		m_sMapURL.Empty();
	}
}
////////////////////////////////////////////////////////////////////////////
CSecID CVisionApp::GetNextHtmlWindowID()
{
	CSecID id(SECID_HTML_WINDOWS, ++m_wHtmlWindowID);
	return id;
}
////////////////////////////////////////////////////////////////////////////
BOOL CVisionApp::DeleteMap()
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
