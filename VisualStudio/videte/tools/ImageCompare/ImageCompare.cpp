/////////////////////////////////////////////////////////////////////////////
// PROJECT:		ImageCompare
// FILE:		$Workfile: ImageCompare.cpp $
// ARCHIVE:		$Archive: /Project/Tools/ImageCompare/ImageCompare.cpp $
// CHECKIN:		$Date: 7.07.99 12:55 $
// VERSION:		$Revision: 10 $
// LAST CHANGE:	$Modtime: 7.07.99 12:43 $
// BY AUTHOR:	$Author: Uwe $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImageCompare.h"

#include "MainFrm.h"

#include "ImageCompareDoc.h"
#include "ImageCompareView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only CImageCompareApp object
CImageCompareApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CImageCompareApp

BEGIN_MESSAGE_MAP(CImageCompareApp, CWinApp)
	//{{AFX_MSG_MAP(CImageCompareApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CImageCompareApp::CImageCompareApp()
{
	// Imagendll öffnen
	IM_Init(0);
	IM_UL(136494055,1211337344,1341787902,19292);
}

/////////////////////////////////////////////////////////////////////////////
CImageCompareApp::~CImageCompareApp()
{
	IM_DeInit();
}
/////////////////////////////////////////////////////////////////////////////
void CImageCompareApp::RegisterWindowClass()
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
	wndclass.lpszClassName = WK_APP_NAME_IMAGE_COMPARE;

	// main app window
	AfxRegisterClass(&wndclass);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CImageCompareApp::InitInstance()
{
	// nur eine Instanz starten
	if ( WK_ALONE(WK_APP_NAME_IMAGE_COMPARE)==FALSE )
	{
		HWND hWndFirstInstance = FindWindow(WK_APP_NAME_IMAGE_COMPARE,NULL);
		if (hWndFirstInstance)
		{
			SetWindowPos(hWndFirstInstance,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
			SetWindowPos(hWndFirstInstance,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
		}
		return FALSE;
	}

	CString sTitle;
	sTitle = COemGuiApi::GetApplicationName(WAI_IMAGE_COMPARE);
	m_pszAppName = _tcsdup(sTitle);    // human readable title

	// Debugger öffnen
	InitDebugger("ImageCompare.log", WAI_IMAGE_COMPARE, NULL);

	WK_TRACE("Starting ImageCompare...\n");

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	
	RegisterWindowClass();

	// Register the application's document templates.  Document templates
	// serve as the connection between documents, frame windows and views.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CImageCompareDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CImageCompareView));
	AddDocTemplate(pDocTemplate);

	if (!Login())
		return -1;

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
int CImageCompareApp::ExitInstance() 
{
	// Debugger schließen
	CloseDebugger();
	
	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
void CImageCompareApp::OnAppAbout()
{
	COemGuiApi::AboutDialog(m_pMainWnd, TRUE);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CImageCompareApp::Login()
{
	BOOL bRet = FALSE;			 

	COEMLoginDialog dlg(AfxGetMainWnd(), WK_ALLOW_IMAGECOMPARE);
							 			   
	if (IDOK==dlg.DoModal())	  		 
	{
		m_User = dlg.GetUser();
		bRet = TRUE;		 
	}						    
	return bRet;
}



