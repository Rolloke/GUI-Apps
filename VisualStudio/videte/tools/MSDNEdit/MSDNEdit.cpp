// MSDNEdit.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "MSDNEdit.h"

#include "MainFrm.h"
#include "MSDNEditDoc.h"
#include "LeftView.h"
#include "BatchCompile.h"
#include "MSDNIntegration.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMSDNEditApp

BEGIN_MESSAGE_MAP(CMSDNEditApp, CWinApp)
	//{{AFX_MSG_MAP(CMSDNEditApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileOpen)
	ON_COMMAND(ID_EDIT_COMILE_BATCH, OnEditComileBatch)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMSDNEditApp construction


CMSDNEditApp::CMSDNEditApp()
{
   m_pDT2         = NULL;
   m_pIntegration = NULL;
   ZeroMemory(&m_piMSNDIntegratior, sizeof(PROCESS_INFORMATION));
}

CMSDNEditApp::~CMSDNEditApp()
{
   if (m_piMSNDIntegratior.hProcess)
   {
      DWORD dwExitCode;
      if (::GetExitCodeProcess(m_piMSNDIntegratior.hProcess, &dwExitCode) && (dwExitCode == STILL_ACTIVE))
      {
         dwExitCode = -1;
         if (m_piMSNDIntegratior.dwThreadId && ::IsWindow((HWND)m_piMSNDIntegratior.dwThreadId))
         {
            ::PostMessage((HWND)m_piMSNDIntegratior.dwThreadId, WM_CLOSE, 0, 0);
         }
         else
         {
            ::TerminateProcess(m_piMSNDIntegratior.hProcess, dwExitCode);
         }
         ::CloseHandle(m_piMSNDIntegratior.hProcess);
         ::CloseHandle(m_piMSNDIntegratior.hThread);
      }
   }
   if (m_pDT2) delete m_pDT2;
   if (m_pIntegration) delete m_pIntegration;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMSDNEditApp object

CMSDNEditApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMSDNEditApp initialization

BOOL CMSDNEditApp::InitInstance()
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
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
   DWORD dwDummy = 0;
   char  szFileName[_MAX_PATH];
   GetModuleFileName(NULL, szFileName, _MAX_PATH);
   int   bytes = GetFileVersionInfoSize(szFileName, &dwDummy);
   int nVersion = 1;
   int nSubVers = 0;
   if(bytes)
   {
      void * buffer = new char[bytes];
      GetFileVersionInfo(szFileName, 0, bytes, buffer);
      VS_FIXEDFILEINFO * Version;
      unsigned int     length;
      if(VerQueryValue(buffer,"\\",(void **) &Version, &length))
      {
         nVersion = HIWORD(Version->dwFileVersionMS);
         nSubVers = LOWORD(Version->dwFileVersionMS);
         if ((nSubVers > 0) && (nSubVers < 10)) nSubVers *= 10;
      }
      delete[] buffer;
   }
   SetRegistryKey(_T("Videte"));                                    // Firmenname
   if (m_pszProfileName)
   {
      char profilename[_MAX_PATH];                                   // und Versionsnummer
      wsprintf(profilename, "%s\\V%d.%02d", m_pszProfileName, nVersion, nSubVers);
      free((void*)m_pszProfileName);
      m_pszProfileName = _tcsdup(profilename);
   }

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
   m_pIntegration = new CMSDNIntegration;
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CMSDNEditDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CLeftView));
	AddDocTemplate(pDocTemplate);
   m_pDT2 = new CSingleDocTemplate(IDR_MAINFRAME2, NULL, NULL, NULL);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

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
void CMSDNEditApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CMSDNEditApp message handlers

class CDocManagerExt : public CDocManager
{
public:
   void RemoveDocTemplate(CDocTemplate*pdt)
   {
   	POSITION pos = NULL;
      pos = m_templateList.Find(pdt);
	   if (pos != NULL)
	   {
         m_templateList.RemoveAt(pos);
	   }
   }
};


void CMSDNEditApp::OnFileOpen() 
{
   ASSERT(m_pDocManager != NULL);

   SetDocTemplates();
	CString newName;
   BOOL bReturn = m_pDocManager->DoPromptFileName(newName, AFX_IDS_OPENFILE, OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, TRUE, NULL);
   RemoveDocTemplates();
   if (bReturn)
      OpenDocumentFile(newName);
}

void CMSDNEditApp::OnUpdateFileOpen(CCmdUI* pCmdUI)
{
   pCmdUI->Enable();
}

void CMSDNEditApp::SetDocTemplates()
{
   m_pDocManager->AddDocTemplate(m_pDT2);
}

void CMSDNEditApp::RemoveDocTemplates()
{
   ((CDocManagerExt*)m_pDocManager)->RemoveDocTemplate(m_pDT2);
}

int  CMSDNEditApp::TestMSDNIntProcess()
{
   if (m_piMSNDIntegratior.hProcess)
   {
      DWORD dwExitCode;
      if (::GetExitCodeProcess(m_piMSNDIntegratior.hProcess, &dwExitCode) && (dwExitCode == STILL_ACTIVE))
      {
         return 1;
      }
      else
      {
         CloseHandle(m_piMSNDIntegratior.hProcess);
         CloseHandle(m_piMSNDIntegratior.hThread);
         ZeroMemory(&m_piMSNDIntegratior, sizeof(PROCESS_INFORMATION));
      }
   }
   return 0;
}

void CMSDNEditApp::OnEditComileBatch() 
{
   CBatchCompile dialog(AfxGetMainWnd());
   dialog.DoModal();
}
