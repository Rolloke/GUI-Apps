// WkTranslator.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "WkTranslator.h"

#include "MainFrm.h"
#include "WkTranslatorDoc.h"
#include "WkTranslatorView.h"
#include "CommandLineInfoWkTranslator.h"

#include "SelectCodePageDlg.h"

#include <Imm.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWkTranslatorApp
BEGIN_MESSAGE_MAP(CWkTranslatorApp, CWinApp)
	//{{AFX_MSG_MAP(CWkTranslatorApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, OnUpdateFileNew)
	ON_COMMAND(ID_VIEW_EDITFONT, OnViewEditfont)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EDITFONT, OnUpdateViewEditfont)
	ON_COMMAND(ID_GLOSSARY_SELECT_CODEPAGE, OnGlossarySelectCodepage)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWkTranslatorApp construction

CWkTranslatorApp::CWkTranslatorApp()
{
	m_bCmdLineFile = FALSE;
	m_wCodePage = 0;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CWkTranslatorApp object

CWkTranslatorApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CWkTranslatorApp initialization

BOOL CWkTranslatorApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	TRACE(_T("AfxGetApp %08x\n"), AfxGetApp());
//	InitDebugger(_T("wktranslator.log"),WAI_INVALID);

#if _MFC_VER < 0x0700
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

   DWORD dwDummy = 0;
   _TCHAR  szFileName[_MAX_PATH];
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
      if(VerQueryValue(buffer, _T("\\"),(void **) &Version, &length))
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
      _TCHAR profilename[_MAX_PATH];                                   // und Versionsnummer
      wsprintf(profilename, _T("%s\\V%d.%02d"), m_pszProfileName, nVersion, nSubVers);
      free((void*)m_pszProfileName);
      m_pszProfileName = _tcsdup(profilename);
   }

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)


	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CWkTranslatorDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CWkTranslatorView));
	AddDocTemplate(pDocTemplate);

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfoWkTranslator cmdInfo;
	ParseCommandLine(cmdInfo);
	m_bCmdLineFile = !cmdInfo.m_strFileName.IsEmpty();

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	BYTE* pData  = NULL;
	UINT  nBytes = 0;
	GetProfileBinary(_T("Settings"), _T("EditFont"), &pData, &nBytes);
	if (pData)
	{
		if (nBytes == sizeof(LOGFONT))
		{
			m_lfEdit.CreateFontIndirect((LOGFONT*)pData);
		}
		delete pData;
	}
	SetFontForChildren(m_pMainWnd->m_hWnd);
	m_wCodePage = (WORD)GetProfileInt(_T("Settings"), _T("CodePage"), CP_UNICODE);
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
	virtual BOOL OnInitDialog();
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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CWkTranslatorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CWkTranslatorApp commands

void CWkTranslatorApp::OnUpdateFileSaveAs(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_bCmdLineFile);
}

void CWkTranslatorApp::OnUpdateFileNew(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

int CWkTranslatorApp::ExitInstance() 
{
//	CloseDebugger();
	
	return CWinApp::ExitInstance();
}

void CWkTranslatorApp::OnViewEditfont() 
{
	LOGFONT lf;
	if ((HFONT)m_lfEdit)
	{
		m_lfEdit.GetLogFont(&lf);
	}
	else
	{
		ZeroMemory(&lf, sizeof(LOGFONT));
		lf.lfHeight = -12;
	}
	CFontDialog dlg(&lf);
	if (dlg.DoModal() == IDOK)
	{
		HGDIOBJ hObj = m_lfEdit.Detach();
		m_lfEdit.CreateFontIndirect(&lf);
		WriteProfileBinary(_T("Settings"), _T("EditFont"), (BYTE*)&lf, sizeof(lf));

		CWnd *pWnd = GetMainWnd();
		SetFontForChildren(pWnd->m_hWnd);
		if (hObj)
		{
			::DeleteObject(hObj);
		}
	}
}

void CWkTranslatorApp::OnUpdateViewEditfont(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
}

BOOL CALLBACK CWkTranslatorApp::EnumChildToSetFont(HWND hwnd, LPARAM lParam)
{
	::SendMessage(hwnd, WM_SETFONT, lParam, 1);
	return 1;
}

void CWkTranslatorApp::SetFontForChildren(HWND hWnd)
{
	if (HFONT(theApp.m_lfEdit))
	{
		EnumChildWindows(hWnd, EnumChildToSetFont, (LPARAM)HFONT(m_lfEdit));
	}
}

void CWkTranslatorApp::OnGlossarySelectCodepage() 
{
	CSelectCodePageDlg dlg;
	dlg.m_wCodePage = m_wCodePage;
	if (dlg.DoModal() == IDOK)
	{
		m_wCodePage = dlg.m_wCodePage;
		WriteProfileInt(_T("Settings"), _T("CodePage"), m_wCodePage);
	}
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	theApp.SetFontForChildren(m_hWnd);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
