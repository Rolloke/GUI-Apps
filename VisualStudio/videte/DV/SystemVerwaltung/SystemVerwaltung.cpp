// SystemVerwaltung.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "SystemVerwaltung.h"

#include "MainFrm.h"
#include "SVDoc.h"
#include "SVView.h"

#include "wk_msg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
int CheckDigits(CString& s)
{
	int r = -1;
	int i,c;
	c = s.GetLength();

	for (i=0;i<c;i++)
	{
		if (!isdigit(s[i]))
		{
			s = s.Left(i);
			r = i;
			break;
		}
	}

	return r;
}
/////////////////////////////////////////////////////////////////////////////
// CSVApp

BEGIN_MESSAGE_MAP(CSVApp, CWinApp)
	//{{AFX_MSG_MAP(CSVApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	// Standard print setup command
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSVApp construction

CSVApp::CSVApp()
{
	m_pUser = NULL;
	m_pPermission = NULL;
	m_bRunAnyLinkUnit = FALSE;
	m_pDongle = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSVApp object

CSVApp theApp;

/////////////////////////////////////////////////////////////////////////////
void CSVApp::RegisterWindowClass()
{
	WNDCLASS  wndclass ;

	// register window class
	wndclass.style =         0;
	wndclass.lpfnWndProc =   DefWindowProc;
	wndclass.cbClsExtra =    0;
	wndclass.cbWndExtra =    0;
	wndclass.hInstance =     m_hInstance;
	wndclass.hIcon =         LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME));
	wndclass.hCursor =       LoadStandardCursor(IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1) ;
	wndclass.lpszMenuName =  0L;
	wndclass.lpszClassName = WK_APP_NAME_SUPERVISOR;

	AfxRegisterClass(&wndclass);
}
/////////////////////////////////////////////////////////////////////////////
// CSVApp initialization
BOOL CSVApp::InitInstance()
{
	if ( WK_ALONE(WK_APP_NAME_SUPERVISOR)==FALSE )
	{
		HWND hWndFirstInstance = FindWindow(WK_APP_NAME_SUPERVISOR,NULL);
		if (hWndFirstInstance)
		{
			SetWindowPos(hWndFirstInstance,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
			SetWindowPos(hWndFirstInstance,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
		}
		return FALSE;
	}

	AfxInitRichEdit();

	InitDebugger(_T("SystemVerwaltung.log"),WAI_SUPERVISOR);
	WK_TRACE_USER(_T("SystemVerwaltung wird gestartet\n"));
	m_nCmdShow = SW_SHOWMAXIMIZED;

	m_pDongle = new CWK_Dongle();
	CString sTitle;
	sTitle = COemGuiApi::GetApplicationName(WAI_SUPERVISOR);
	if (m_pDongle->IsDemo())
	{
		sTitle += _T(" DEMO");
	}
	m_pszAppName = _tcsdup(sTitle);    // human readable title
	if (!m_pDongle->IsValid())
	{
		AfxMessageBox(IDP_INVALID_DONGLE);
		return FALSE;
	}
	m_bRunAnyLinkUnit = m_pDongle->RunAnyLinkUnit();

	WK_CreateDirectory(_T("c:\\out"));

	if (!Login())
	{
		return FALSE;
	}

	RegisterWindowClass();

	if (m_pPermission->GetFlags()==0)
	{
		// nothing allowed
		AfxMessageBox(IDP_ACCESS_DENIED);
		return IDCANCEL;
	}

	// bitmap fuer small icons laden
	m_SmallImage.Create(IDB_IMAGES,27,1,RGB(0,255,255));
	// bitmap fuer state icons laden

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

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CSVDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CSVView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// CSVApp commands
void CSVApp::OnAppAbout()
{
	COemGuiApi::AboutDialog(m_pMainWnd);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVApp::LoginAsSuperVisor()
{
	CWK_Profile wkp; // ML 1.9.99 Sollte nicht lieber GetProfile() benutzt werden?
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
BOOL CSVApp::Login()
{
//#ifdef _DEBUG
//	return LoginAsSuperVisor();
//#endif

	CWK_Profile wkp;
	CString sUser;
	CString sPassword;
	CString sText;
	BOOL bRet = FALSE;
	CPermission* pPermission;
	CUserArray ua;
	CPermissionArray pa;

	ua.Load(wkp);
	pa.Load(wkp);

	pPermission = pa.GetSpecialReceiver();
	if (pPermission)
	{
		WK_DELETE(m_pUser);
		WK_DELETE(m_pPermission);
		m_pPermission = new CPermission(*pPermission);
		m_pUser = new CUser();
		m_pUser->SetName(_T("SpecialReceiver"));
		sText.Format(IDS_LOGIN_SUCCESFUL, (LPCTSTR)m_pUser->GetName());
		WK_TRACE_USER(sText + _T("\n"));
		return TRUE;
	}					  
	if (ua.GetSize()==0)
	{
		WK_DELETE(m_pUser);

		pPermission = pa.GetSuperVisor();
		
		if (sPassword==pPermission->GetPassword())
		{
			WK_DELETE(m_pPermission);
			m_pPermission = new CPermission(*pPermission);
			m_pUser = new CUser();
			m_pUser->SetName(_T("SuperVisor"));
			sText.Format(IDS_LOGIN_SUCCESFUL, (LPCTSTR)m_pUser->GetName());
			WK_TRACE_USER(sText + _T("\n"));
			return TRUE;
		}
	}
 
	COEMLoginDialog dlg;
							    
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

	if (m_pUser) {
		sUser = m_pUser->GetName();
	}
	if (bRet)
	{
		sText.Format(IDS_LOGIN_SUCCESFUL, sUser);
		WK_TRACE_USER(sText + _T("\n"));
	}
	else
	{
		sText.Format(IDS_LOGIN_DENIED, sUser);
		WK_TRACE_USER(sText + _T("\n"));
	}

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
int CSVApp::ExitInstance() 
{
	WK_DELETE(m_pUser);
	WK_DELETE(m_pPermission);
	WK_DELETE(m_pDongle);
	
	WK_TRACE_USER(_T("SystemVerwaltung wird beendet\n"));
	CloseDebugger();
	return 0L;
}
/////////////////////////////////////////////////////////////////////////////

