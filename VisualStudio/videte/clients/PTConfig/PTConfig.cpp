// PTConfig.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PTConfig.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "PTConfigDoc.h"
#include "PTConfigView.h"

#include "wk.h"
#include "WK_Trace.h"
#include "TimedMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPTConfigApp

BEGIN_MESSAGE_MAP(CPTConfigApp, CWinApp)
	//{{AFX_MSG_MAP(CPTConfigApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPTConfigApp construction

CPTConfigApp::CPTConfigApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPTConfigApp object

CPTConfigApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPTConfigApp initialization

BOOL CPTConfigApp::InitInstance()
{
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
	CString sReg = WK_RegistryKey;
	sReg += "\\Security\\3.00";
	SetRegistryKey(sReg);


	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	InitDebugger("PTConfig.log");
	
	WK_STAT_LOG("PTConfigApp",1,"reset");
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_PTCONFTYPE,
		RUNTIME_CLASS(CPTConfigDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CPTConfigView));
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CPTConfigApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CPTConfigApp commands

int CPTConfigApp::ExitInstance() 
{
	WK_STAT_LOG("PTConfigApp",0,"reset");
	CloseDebugger();
	
	return CWinApp::ExitInstance();
}

CDocument* CPTConfigApp::OpenDocumentFile(LPCTSTR lpszFileName) 
{
	return CWinApp::OpenDocumentFile(lpszFileName);
}

void CPTConfigApp::OnFileOpen() 
{
	WK_TRACE("OnFileOpen, NOT YET any action\n");
}

static CTimedMessage s_halfSecondFilter(500);

BOOL CPTConfigApp::OnIdle(LONG lCount) 
{
	// only do idle processing if there are no messages
	if (CWinApp::OnIdle(lCount)==0) {
		// only do something 2 times per second
		if (s_halfSecondFilter.OmitMessage()==FALSE) {
			// update all views with "online time"
			for (int i=0;i<g_theDocs.GetSize();i++) {
				CPTConfigDoc *pDoc = g_theDocs[i];
				CString sTitle;
				if (pDoc->m_sState.Find("verbunden")!=-1) {
					CTimeSpan delta = CTime::GetCurrentTime()-pDoc->m_onlineStartTime;
					sTitle.Format("%s - Online seit %02d:%02d:%02d",
						(const char *)pDoc->m_sRemoteHost,
						delta.GetTotalHours(),
						delta.GetTotalMinutes()%60,
						delta.GetTotalSeconds()%60
						);
				} else {
					sTitle.Format("%s - Nicht verbunden",(const char *)pDoc->m_sRemoteHost);
				}
				
				pDoc->SetTitle(sTitle);
			}
			// not needed, TimedMessage does the job anyway Sleep(100);
		}
		Sleep(100);
	}
		
	return 1;
}



int CPTConfigApp::Run() 
{
	return CWinApp::Run();
}
