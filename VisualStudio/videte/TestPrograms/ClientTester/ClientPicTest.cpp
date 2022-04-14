///////////////////////////////////////////////////////////////
// $Workfile  : ClientPicTest.cpp $ 
// $Archive   : /Project/Tools/ClientTester/ClientPicTest.cpp $
// $Date      : 24.04.97 9:38 $ 
// $Revision  : 1 $
// $Modtime   : 23.04.97 17:45 $ 
// $Author    : Hajo $
// $Nokeywords: $

/**modulref ***************************************************
 * name    : ClientPicTest.cpp
 * class   : CClientPicTestApp
 * derived : CWinApp
 * members :
 * public  : CClientPicTestApp();
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
 * author  : Hajo Fierke 1997 w+k
 * history : 09.04.97 hajo
 *         : 10.04.97 hajo added COutputTest 
 *         : 
 **************************************************************/
#include "stdafx.h"
#include "TestTool.h"
#include "ClientPicTest.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "ClientPicTestDoc.h"
#include "ClientPicTestView.h"

#include "wk.h"
#include "WK_Trace.h"
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
// CClientPicTestApp

BEGIN_MESSAGE_MAP(CClientPicTestApp, CWinApp)
	//{{AFX_MSG_MAP(CClientPicTestApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////
// CClientPicTestApp construction

CClientPicTestApp::CClientPicTestApp()
{
	m_pCTestTool = NULL;
	m_pOutput    = NULL;
}

///////////////////////////////////////////////////////////////
// The one and only CClientPicTestApp object

CClientPicTestApp theApp;

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
void CClientPicTestApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/**funcref ***************************************************
 * class   : CClientPicTestApp 
 * name    : InitInstance(
 * input   : );
 * output  : BOOL
 *         :
 * purpose : CClientPicTestApp initialization
 * uses    : nil
 *         :
 * author  : Hajo Fierke  1997 w+k
 * history : 23.04.1997
 *         : 24.04.1997	self
 *************************************************************/
/*
@MD
*/
BOOL CClientPicTestApp::InitInstance()
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

	// for mili seconds in traces
	WK_STAT_TICK_COUNT = GetTickCount(); 
	InitDebugger("ClientPicTest.log");

	// Register document templates
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_PICTESTYPE,
		RUNTIME_CLASS(CClientPicTestDoc),
		RUNTIME_CLASS(CChildFrame), 
		RUNTIME_CLASS(CClientPicTestView));
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	CCommandLineInfo cmdInfo;
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

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
		
//		if ClientPicTest ist set to server simulation
		if(m_pCTestTool->GetIsServer())
		{	
			l_sHost = m_pCTestTool->GetShmName();
		}
		else
		{	
			l_sHost = m_pCTestTool->FindHostNumberByName(l_sHost);
		}
	}

	pMainFrame->ShowWindow(SW_SHOWMINIMIZED);//SW_SHOWMAXIMIZED);

	pMainFrame->UpdateWindow();
	
	DoTest(l_sHost);

	return TRUE;
}

/**funcref ***************************************************
 * class   : CClientPicTestApp 
 * name    : ExitInstance(
 * input   : );
 * output  : int
 *         :
 * purpose : Exit
 * uses    : WK_DELETE(...);
 *         : CloseDebugger();
 *         :
 * author  : Hajo Fierke  1997 w+k
 * history : 23.04.1997
 *         : 24.04.1997	self
 *************************************************************/
/*
@MD
*/
int CClientPicTestApp::ExitInstance() 
{
 WK_DELETE(m_pCTestTool);
 WK_DELETE(m_pOutput);
	
 CloseDebugger();
	
 return CWinApp::ExitInstance();
}

/**funcref ***************************************************
 * class   : CClientPicTestApp 
 * name    : DoTest(
 * input   : CString& sHost); host number or shared memory
 *         :				  name
 * output  : int 42
 *         :
 * purpose : got an outputconnection and starts threads
 * uses    : CConnectionRecord cRec(...);
           : CIPCFetchResult NewFetchOutputConnection(...);
 *         : CIPCOutput StartThread();
 *         :
 * author  : Hajo Fierke  1997 w+k
 * history : 23.04.1997
 *         : 24.04.1997	self
 *************************************************************/
/*
@MD
*/
int CClientPicTestApp::DoTest(CString& sHost)
{
 BOOL bAsMaster=TRUE; //=WK_MASTER ;

 if(!m_pCTestTool->GetIsServer())
 {
	bAsMaster=FALSE; //=WK_SLAVE ;

	CUser userDummy;
	userDummy.SetName( "Tester" );
	CConnectionRecord cRec(
		sHost,			// const CString &remoteHost,
		userDummy,		// CUser user
		"SuperVisor",	// sPermission,
		""				// sPassword,
						// DWORD dwTimeout=30000
		);
	CTickCounter l_TickCounter;
	l_TickCounter.StartCount();

	CIPCFetchResult result = NewFetchOutputConnection(cRec);
	sHost=result.GetShmName();

	// double check on Okay AND a shared memory name
	if( result.IsOkay()==FALSE || sHost.GetLength()==0)	 
	{
		WK_TRACE_ERROR("gotta no FetchResult\n");
		WK_TRACE("\terror %s[%d]\n",
					NameOfEnum(result.GetError()),
					result.GetErrorCode()
					);	
		WK_TRACE("\tmsg %s\n",
					(const char *)result.GetErrorMessage()
					);	
		AfxGetMainWnd()->PostMessage(WM_CLOSE);
		return 0;	// <<< EXIT >>>
	}

	l_TickCounter.StopCount();

	WK_TRACE("NewFetchOutputConnection nach %d ms\n",
		l_TickCounter.GetDeltaTicks()
		);

 }
 else
 {

 }	
 
 WK_TRACE("sShmName=%s\n", (const char*)sHost );	
 m_pOutput = new COutputTester(m_pCTestTool, sHost, bAsMaster);
 m_pOutput->StartThread();

 return 42;
}
