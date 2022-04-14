/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SDITest.cpp $
// ARCHIVE:		$Archive: /Project/TestPrograms/SDITest/SDITest.cpp $
// CHECKIN:		$Date: 29.12.05 23:03 $
// VERSION:		$Revision: 50 $
// LAST CHANGE:	$Modtime: 29.12.05 22:19 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "!todo.h"
#include "SDITest.h"
#include "ascii.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "SDIMultiDocTemplate.h"
#include "SDITestDoc.h"
#include "SDITestView.h"

#include "DocLandauDtp.h"
#include "ViewLandauDtp.h"
#include "DocKebaPc2000.h"
#include "ViewKebaPc2000.h"
#include "DocNcr_1_1.h"
#include "ViewNcr_1_1.h"
#include "DocEngelMultipass.h"
#include "ViewEngelMultipass.h"
#include "DocSniDos.h"
#include "ViewSniDos.h"
#include "DocIbm.h"
#include "ViewIbm.h"
#include "DocGermanParcel.h"
#include "ViewGermanParcel.h"
#include "DocNcr_1_2.h"
#include "ViewNcr_1_2.h"
#include "DocSniIbmEmulation.h"
#include "ViewSniIbmEmulation.h"
//#include "DocAvmCustomerSafe.h"
//#include "ViewAvmCustomerSafe.h"
#include "DocSniCom.h"
#include "ViewSniCom.h"
#include "DocKebaPasador.h"
#include "ViewKebaPasador.h"
#include "DocAscom.h"
#include "ViewAscom.h"
//#include "DocDialogbank.h"
//#include "ViewDialogbank.h"
//#include "DocRondo.h"
//#include "ViewRondo.h"
#include "DocFile.h"
#include "ViewFile.h"
#include "DocPM100IPAlarm.h"
#include "ViewPM100IPAlarm.h"
#include "DocPM100XML.h"
#include "ViewPM100XML.h"
#include "DocSchneiderIntercom.h"
#include "ViewSchneiderIntercom.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSDITestApp
/////////////////////////////////////////////////////////////////////////////
// The one and only CSDITestApp object
CSDITestApp theApp;
/////////////////////////////////////////////////////////////////////////////
// CSDITestApp construction
CSDITestApp::CSDITestApp()
{
	m_sDirectory = "C:\\LOG";
}
CString CSDITestApp::GetLocalIP()
{
	char szHostName[128];
	CString str;
	if( gethostname(szHostName, 128) == 0 )
	{
		// Get host adresses
		struct hostent * pHost;
		int i;

		pHost = gethostbyname(szHostName);

		for( i = 0; pHost!= NULL && pHost->h_addr_list[i]!= NULL; i++ )
		{

			int j;

			for( j = 0; j < pHost->h_length; j++ )
			{
				CString addr;

				if( j > 0 )
					str += _T(".");

				addr.Format(_T("%u"),(unsigned int)((unsigned char*)pHost->h_addr_list[i])[j]);
				str += addr;
			}
		}
	}
	return str;

}
/////////////////////////////////////////////////////////////////////////////
// CSDITestApp initialization
BOOL CSDITestApp::InitInstance()
{
/*
	if (WK_ALONE(WK_APP_NAME_SDITEST)==FALSE) {
		HWND hWndFirstInstance = FindWindow(WK_APP_NAME_SDITEST,NULL);
		if (hWndFirstInstance) {
			SetWindowPos(hWndFirstInstance,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
			SetWindowPos(hWndFirstInstance,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
		}
		return FALSE;
	}
*/

//	CString sReg = WK_RegistryKey;
//	sReg += '\\';
//	sReg += WK_SecurityKey;
//	SetRegistryKey(sReg);

	if (!AfxSocketInit())
	{
		return FALSE;
	}

	// OOPS Debugger bekommt WAI_INVALID, da SDITest eh nur als stand-alone Loesung laeuft
	InitDebugger(_T("SDITest.log"), WAI_INVALID);

	WK_TRACE(_T("Version %s\n"),
				PLAIN_TEXT_VERSION(12, 5, 15, 57, _T("12/05/99 , 15:57\0"), // @AUTO_UPDATE
				0)
				);

	// Dongle vorhanden?
//	CWK_Dongle dongle(WAI_SDICONFIG); 
//	if (dongle.IsExeOkay()==FALSE) {
//		WK_TRACE_WARNING(_T("No Dongle or not allowed\n"));
//		Sleep(100);
//		AfxMessageBox(IDP_INVALID_DONGLE);
//		return FALSE;
//	}

	
	RegisterWindowClass();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#if _MFC_VER <0x0700
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
	CSDIMultiDocTemplate* pDocTemplate;

//	pDocTemplate = new CSDIMultiDocTemplate(
//		IDR_SDITESTYPE,
//		RUNTIME_CLASS(CSDITestDoc),
//		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
//		RUNTIME_CLASS(CSDITestView));
//	AddDocTemplate(pDocTemplate);

	// Geldautomaten
	pDocTemplate = new CSDIMultiDocTemplate(
		IDR_ASCOM,
		RUNTIME_CLASS(CDocAscom),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CViewAscom));
	AddDocTemplate(pDocTemplate);

/*	pDocTemplate = new CSDIMultiDocTemplate(
		IDR_DIALOGBANK,
		RUNTIME_CLASS(CDocDialogbank),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CViewDialogbank));
	AddDocTemplate(pDocTemplate);
*/
	pDocTemplate = new CSDIMultiDocTemplate(
		IDR_IBM,
		RUNTIME_CLASS(CDocIbm),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CViewIbm));
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CSDIMultiDocTemplate(
		IDR_DTP,
		RUNTIME_CLASS(CDocLandauDtp),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CViewLandauDtp));
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CSDIMultiDocTemplate(
		IDR_NCR,
		RUNTIME_CLASS(CDocNcr_1_1),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CViewNcr_1_1));
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CSDIMultiDocTemplate(
		IDR_NCR_NEW,
		RUNTIME_CLASS(CDocNcr_1_2),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CViewNcr_1_2));
	AddDocTemplate(pDocTemplate);

/*	pDocTemplate = new CSDIMultiDocTemplate(
		IDR_RONDO,
		RUNTIME_CLASS(CDocRondo),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CViewRondo));
	AddDocTemplate(pDocTemplate);
*/
	pDocTemplate = new CSDIMultiDocTemplate(
		IDR_SNI,
		RUNTIME_CLASS(CDocSniDos),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CViewSniDos));
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CSDIMultiDocTemplate(
		IDR_SNI_COM,
		RUNTIME_CLASS(CDocSniCom),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CViewSniCom));
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CSDIMultiDocTemplate(
		IDR_SNI_IBM_EMU,
		RUNTIME_CLASS(CDocSniIbmEmulation),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CViewIbmEmulation));
	AddDocTemplate(pDocTemplate);

	//Zutrittskontrollen
/*	pDocTemplate = new CSDIMultiDocTemplate(
		IDR_AV_CUSTOMER_SAFE,
		RUNTIME_CLASS(CDocAvmCustomerSafe),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CViewAvmCustomerSafe));
	AddDocTemplate(pDocTemplate);
*/
	pDocTemplate = new CSDIMultiDocTemplate(
		IDR_ENGEL,
		RUNTIME_CLASS(CDocEngelMultipass),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CViewEngelMultipass));
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CSDIMultiDocTemplate(
		IDR_KEBA_PASADOR,
		RUNTIME_CLASS(CDocKebaPasador),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CViewKebaPasador));
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CSDIMultiDocTemplate(
		IDR_KEBA,
		RUNTIME_CLASS(CDocKebaPc2000),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CViewKebaPc2000));
	AddDocTemplate(pDocTemplate);

	// Datei
	pDocTemplate = new CSDIMultiDocTemplate(
		IDR_FILE,
		RUNTIME_CLASS(CFileDoc),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CViewFile));
	AddDocTemplate(pDocTemplate);

	// Industrielle Anlagen
	pDocTemplate = new CSDIMultiDocTemplate(
		IDR_GERMAN_PARCEL,
		RUNTIME_CLASS(CDocGermanParcel),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CViewGermanParcel));
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CSDIMultiDocTemplate(
		IDR_PM_100_IP_ALARM,
		RUNTIME_CLASS(CDocPM100IPAlarm),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CViewPM100IPAlarm));
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CSDIMultiDocTemplate(
		IDR_PM_100_XML,
		RUNTIME_CLASS(CDocPM100XML),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CViewPM100XML));
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CSDIMultiDocTemplate(
		IDR_SCHNEIDER_INTERCOM,
		RUNTIME_CLASS(CDocSchneiderIntercom),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CViewSchneiderIntercom));
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME)) {
		return FALSE;
	}
	m_pMainWnd = pMainFrame;

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// Enable DDE Execute open
/*
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);
*/

	// Parse command line for standard shell commands, DDE, file open
//	CCommandLineInfo cmdInfo;
	CSDITestCommandLineInfo cmdInfo;
	// Standardmaessig keine neue Datei oeffnen
//	cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo)) {
		return FALSE;
	}

	ProcessSDITestShellCommand(cmdInfo);

	// The main window has been initialized, so show and update it.
	pMainFrame->CenterWindow();
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CSDITestApp::RegisterWindowClass()
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
	wndclass.lpszClassName	=	WK_APP_NAME_SDITEST;

	AfxRegisterClass(&wndclass);
}
/////////////////////////////////////////////////////////////////////////////
int CSDITestApp::ExitInstance() 
{
	CloseDebugger();
	return 0; // returns the value from PostQuitMessage
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSDITestApp, CWinApp)
	//{{AFX_MSG_MAP(CSDITestApp)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CSDITestApp commands
/////////////////////////////////////////////////////////////////////////////
void CSDITestApp::OnFileOpen() 
{
	TCHAR szOldDir[_MAX_PATH];
	memset(szOldDir, 0, _MAX_PATH);
	if ( 0 != GetCurrentDirectory(_MAX_PATH, szOldDir) ) {
		SetCurrentDirectory(m_sDirectory);
		CWinApp::OnFileOpen();
		SetCurrentDirectory(szOldDir);
	}
}
/////////////////////////////////////////////////////////////////////////////
CDocument* CSDITestApp::OpenDocumentFile(LPCTSTR lpszFileName) 
{
	// TODO: Add your specialized code here and/or call the base class
	CString sPathName = lpszFileName;
	m_sDirectory = sPathName.Left( sPathName.ReverseFind(_T('\\')) );
	
	CDocument* pDoc = CWinApp::OpenDocumentFile(lpszFileName);
	if ( pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CSDITestDoc)) ) {
		((CSDITestDoc*)pDoc)->ConfigCom();
	}
	return pDoc;
}
/////////////////////////////////////////////////////////////////////////////
void CSDITestApp::ProcessSDITestShellCommand(CSDITestCommandLineInfo& rCmdInfo)
{
	if (rCmdInfo.m_bAutoStart) {
		POSITION posTemplate = m_pDocManager->GetFirstDocTemplatePosition();
		CDocTemplate* pDocTemplate;
		CDocument* pDoc;
		while (posTemplate) {
			pDocTemplate = m_pDocManager->GetNextDocTemplate(posTemplate);
			if (pDocTemplate) {
				POSITION posDoc = pDocTemplate->GetFirstDocPosition();
				while (posDoc) {
					pDoc = pDocTemplate->GetNextDoc(posDoc);
					if (pDoc) {
					}
				}
			}
		}
	}
}