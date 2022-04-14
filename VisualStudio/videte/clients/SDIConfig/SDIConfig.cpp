/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SDIConfig.cpp $
// ARCHIVE:		$Archive: /PROJECT/Clients/SDIConfig/SDIConfig.cpp $
// CHECKIN:		$Date: 30.03.99 17:56 $
// VERSION:		$Revision: 44 $
// LAST CHANGE:	$Modtime: 30.03.99 17:56 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#include "stdafx.h"

#include "SDIConfig.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "SDIMultiDocTemplate.h"
#include "SDIConfigDoc.h"
#include "SDIConfigView.h"

#include "SDIConfigKebaDoc.h"
#include "SDIConfigKebaView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only CSDIConfigApp object
CSDIConfigApp	theApp;
BOOL			g_bExtraDebugTrace = FALSE;

/////////////////////////////////////////////////////////////////////////////
// CSDIConfigApp
BEGIN_MESSAGE_MAP(CSDIConfigApp, CWinApp)
	//{{AFX_MSG_MAP(CSDIConfigApp)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	//}}AFX_MSG_MAP
	// Standard file based document commands
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
/// CSDIConfigApp construction
CSDIConfigApp::CSDIConfigApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_pUser = NULL;
	m_pPermission = NULL;
}
/////////////////////////////////////////////////////////////////////////////
// CSDIConfigApp initialization
BOOL CSDIConfigApp::InitInstance()
{
	if (WK_ALONE(WK_APP_NAME_SDICONFIG)==FALSE) {
		HWND hWndFirstInstance = FindWindow(WK_APP_NAME_SDICONFIG,NULL);
		if (hWndFirstInstance) {
			SetWindowPos(hWndFirstInstance,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
			SetWindowPos(hWndFirstInstance,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
		}
		return FALSE;
	}

	CString sTitle;
	sTitle = COemGuiApi::GetApplicationName(WAI_SDICONFIG);
	m_pszAppName = _tcsdup(sTitle);    // human readable title

	InitDebugger("SDIConfig.log",WAI_SDICONFIG);

	WK_TRACE("Version %s\n",
				PLAIN_TEXT_VERSION(30, 3, 17, 56, "30/03/99,17:56\0", // @AUTO_UPDATE
				0)
				);

	// Dongle vorhanden?
	CWK_Dongle dongle(WAI_SDICONFIG); 
	if (dongle.IsExeOkay()==FALSE) {
		WK_TRACE_WARNING("No Dongle or not allowed\n");
		Sleep(100);
		AfxMessageBox(IDP_INVALID_DONGLE);
		return FALSE;
	}

	RegisterWindowClass();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// You should modify this string to be something appropriate
	// such as the name of your company or organization.
//	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

//	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
	CMultiDocTemplate* pDocTemplate;

//	pDocTemplate = new CSDIMultiDocTemplate(
//		IDR_SDICONTYPE,
//		RUNTIME_CLASS(CSDIConfigDoc),
//		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
//		RUNTIME_CLASS(CSDIConfigView));
//	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CSDIMultiDocTemplate(
		IDR_KEBA,
		RUNTIME_CLASS(CSDIConfigKebaDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CSDIConfigKebaView));
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME)) {
		return FALSE;
	}
	m_pMainWnd = pMainFrame;

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	// Standardmaessig keine neue Datei oeffnen
//	cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo)) {
		m_pMainWnd->DestroyWindow();
		m_pMainWnd = NULL;
		return FALSE;
	}

	// The main window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(m_nCmdShow);
	m_pMainWnd->UpdateWindow();

	// Nicht jeder darf das Programm benutzen!
	if (!Login()) {
		m_pMainWnd->DestroyWindow();
		m_pMainWnd = NULL;
		return FALSE;
	}

	LoadDebugConfiguration();
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigApp::RegisterWindowClass()
{
	WNDCLASS  wndclass;

	// register window class
	wndclass.style			=	0;
	wndclass.lpfnWndProc	=	DefWindowProc;
	wndclass.cbClsExtra		=	0;
	wndclass.cbWndExtra		=	0;
	wndclass.hInstance		=	m_hInstance;
	wndclass.hIcon			=	LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME));
	wndclass.hCursor		=	LoadStandardCursor(IDC_ARROW);
	wndclass.hbrBackground	=	(HBRUSH) (COLOR_WINDOW + 1) ;
	wndclass.lpszMenuName	=	0L;
	wndclass.lpszClassName	=	WK_APP_NAME_SDICONFIG;

	AfxRegisterClass(&wndclass);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigApp::LoadDebugConfiguration() 
{
	// Einlesen der cfg-Datei
	CWKDebugOptions debugOptions;
	// check for C:\SDIConfig.cfg first, then in current directory
	BOOL bFoundFile = debugOptions.ReadFromFile("c:\\SDIConfig.cfg");
	if (bFoundFile==FALSE) {
		bFoundFile = debugOptions.ReadFromFile("SDIConfig.cfg");
	}

	// Setzen der Variablen
	g_bExtraDebugTrace = debugOptions.GetValue("ExtraDebugTrace",FALSE);

	// Im Debug-Modus einige Werte standartmaessig setzen
#ifdef _DEBUG
	WK_TRACE("===> Debug-Version <===\n");
	g_bExtraDebugTrace = TRUE;
#endif

	// Ausgeben der Variablen
	WK_TRACE("g_bExtraDebugTrace = %d\n",g_bExtraDebugTrace);

	Sleep(100);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIConfigApp::LoginAsSuperVisor()
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
	m_pUser->SetName("SuperVisor");
	m_pUser->SetPermissionID(m_pPermission->GetID());
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIConfigApp::Login()
{
#ifdef _DEBUG
	return LoginAsSuperVisor();
#endif

	COEMLoginDialog dlg;

	if (IDOK==dlg.DoModal())
	{
		WK_DELETE(m_pUser);
		m_pUser = new CUser(dlg.GetUser());
		WK_DELETE(m_pPermission);
		m_pPermission = new CPermission(dlg.GetPermission());

		return (m_pUser && m_pUser->GetID()!=SECID_NO_ID) &&
			   (m_pPermission && m_pPermission->GetID()!=SECID_NO_ID);
	}
	else
	{
		return FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIConfigApp::OnIdle(LONG lCount) 
{
	return CWinApp::OnIdle(lCount);
}
/////////////////////////////////////////////////////////////////////////////
int CSDIConfigApp::ExitInstance() 
{
	WK_DELETE(m_pPermission);
	WK_DELETE(m_pUser);
	CloseDebugger();
	return m_msgCur.wParam; // returns the value from PostQuitMessage
}
/////////////////////////////////////////////////////////////////////////////
// CSDIConfigApp commands
void CSDIConfigApp::OnFileNew()
{
	// TODO: Add your command handler code here
	CMainFrame* pMainWnd = GetMainWnd();
	BOOL bMaximized = FALSE;
	CMDIChildWnd* pMDIChild = pMainWnd->MDIGetActive(&bMaximized);
	if (pMDIChild) {
		if ( IDNO == AfxMessageBox(IDS_NEW_DOCUMENT, MB_YESNO) ) {
			return;
		}
		// Wenn ein View vorhanden ist
		CView *pView = pMDIChild->GetActiveView();
		if (pView) {
			// Sicherstellen, dass es auch ein SDIConfigView ist
			ASSERT(pView->IsKindOf( RUNTIME_CLASS(CSDIConfigView) ) );
			CSDIConfigView* pSDIView = (CSDIConfigView*)pView;
			// Ist wohlmoeglich noch das externe Programm aktiv
			if ( pSDIView->GetDocument()->IsExternBusy() ) {
				// Will der User das wirklich
				if (IDNO == AfxMessageBox(IDP_EXTERN_PROGRAM_BUSY, MB_YESNO|MB_DEFBUTTON2)) {
					return;
				}
			}
		}
		pMDIChild->DestroyWindow();
	}
	CWinApp::OnFileNew();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigApp::OnFileOpen() 
{
	// TODO: Add your command handler code here
	CWinApp::OnFileOpen();
}
