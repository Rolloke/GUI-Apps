///////////////////////////////////////////////////////////////
// FILE   : $Workfile: UnitTest.cpp $ 
// ARCHIVE: $Archive: /Project/Tools/UnitTest/UnitTest.cpp $
// DATE   :	$Date: 25.04.97 11:57 $ 
// VERSION: $Revision: 2 $
// LAST   : $Modtime: 25.04.97 10:31 $ 
// AUTHOR : $Author: Hajo $
//        : $Nokeywords:$

/**modulref ***************************************************
 * name    : UnitTest.cpp
 * class   : CUnitTestApp
 * derived : CWinApp
 * members :
 * public  : CUnitTestApp();
 *         : 
 * include : "wk.h"
 *         : "CIPCInputTester.h"
 *         : "CIPCServerControlClientSide.h"
 *         : "User.h"
 *         : "host.h"
 *         : "WK_Names.h"
 *         :
 * private : int	DoTest(const CString&);
 *         :
 * purpose : test of the ISDN unit
 *         : Defines the class behaviors for the application.
 *         :
 * output  : c:\log\UnitTest.log  
 *         : may be followed by SecAnalyser
 *         :
 * author  : Hajo Fierke 1997 w+k
 * history : 09.04.97 hajo
 *         : 10.04.97 hajo added COutputTest 
 *         : 25.04.97 hajo 
 *         : 
 **************************************************************/
#include "stdafx.h"
#include "TestTool.h"
#include "UnitTest.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "UnitTestDoc.h"
#include "UnitTestView.h"

#include "wk.h"
#include "CIPCServerControlClientSide.h"
#include "User.h"
#include "host.h"

#include "OutputTester.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////
// CUnitTestApp

BEGIN_MESSAGE_MAP(CUnitTestApp, CWinApp)
	//{{AFX_MSG_MAP(CUnitTestApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////
// CUnitTestApp construction

CUnitTestApp::CUnitTestApp()
{
	m_pCTestTool = NULL;
	m_pOutput    = NULL;
}

///////////////////////////////////////////////////////////////
// The one and only CUnitTestApp object

CUnitTestApp theApp;

///////////////////////////////////////////////////////////////
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
		// DDX/DDV support
		virtual void DoDataExchange(CDataExchange* pDX);
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
void CUnitTestApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

///////////////////////////////////////////////////////////////
// CUnitTestApp initialization

BOOL CUnitTestApp::InitInstance()
{
	// Standard initialization

#ifdef _AFXDLL
	// Call this when using MFC in a shared DLL
	Enable3dControls();			
#else
	// Call this when linking to MFC statically
	Enable3dControlsStatic();	
#endif
	// Load standard INI file options (including MRU)
	LoadStdProfileSettings();  

	// Register document templates

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_PICTESTYPE,
		RUNTIME_CLASS(CUnitTestDoc),
		RUNTIME_CLASS(CChildFrame), 
		RUNTIME_CLASS(CUnitTestView));
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	CCommandLineInfo cmdInfo;
	// open up your mind and let your life time sun shine now
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	InitDebugger("UnitTest.log");
	m_pCTestTool    = new CTestTool();
	CString l_sHost = m_pCTestTool->GetHost();

	if (m_lpCmdLine[0] == '\0')
	{
		WK_TRACE("Host= %s\n",(const char*)l_sHost);
	}
	else
	{
 		m_pCTestTool->SplitArgs(m_lpCmdLine);
		l_sHost = m_pCTestTool->ParseOptions();
//obsolete	l_sHost = m_pCTestTool->FindHostNumberByName(l_sHost);
	}
	pMainFrame->ShowWindow(m_nCmdShow);
//	pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);
	pMainFrame->UpdateWindow();
	DoTest(m_pCTestTool->GetShmName());
	
	return TRUE;
}

/*
@MD
*/
int CUnitTestApp::ExitInstance() 
{

 WK_DELETE(m_pCTestTool);
 WK_DELETE(m_pOutput);
	
 CloseDebugger();
	
 return CWinApp::ExitInstance();
}

/*
@MD
*/
int CUnitTestApp::DoTest(const CString& jauseString)
{
	WK_TRACE("sShmName=%s\n", (const char*)jauseString);	
	m_pOutput = new COutputTester(m_pCTestTool, jauseString );
	m_pOutput->StartThread();

 return 42;
}
