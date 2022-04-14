/////////////////////////////////////////////////////////////////////////////
// PROJECT:		LogView
// FILE:		$Workfile: LogView.cpp $
// ARCHIVE:		$Archive: /Project/Tools/LogView/LogView.cpp $
// CHECKIN:		$Date: 3/15/06 10:33a $
// VERSION:		$Revision: 44 $
// LAST CHANGE:	$Modtime: 3/15/06 10:33a $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LogView.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "LogViewDoc.h"
#include "LogViewView.h"
#include "EditTimeFormatDlg.h"

#include "HtmlViewDoc.h"
#include "HtmlViewView.h"
#include "oemgui\oemgui.h"

#include "oemgui\OemFileDialog.h"
#include "wkclasses\WK_RunTimeErrors.h"

// include for wk_string options, if nessesary
// add also wkclassesxxx.lib 
// #include "wkclasses\wk_string.h"

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
#include ".\logview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLogViewApp

BEGIN_MESSAGE_MAP(CLogViewApp, CWinApp)
	//{{AFX_MSG_MAP(CLogViewApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	ON_UPDATE_COMMAND_UI(ID_FILE_READ_LOGFILE_UNICODE, OnUpdateFileReadLogfileUnicode)
	ON_COMMAND(ID_FILE_READ_LOGFILE_UNICODE, OnFileReadLogfileUnicode)
	ON_COMMAND(ID_FILE_RUNTIME_ERRORS, OnFileRuntimeErrors)
	ON_UPDATE_COMMAND_UI(ID_FILE_RUNTIME_ERRORS, OnUpdateFileRuntimeErrors)
	//}}AFX_MSG_MAP
    ON_UPDATE_COMMAND_UI(ID_FILE_EDIT_TIME_FORMATS, &CLogViewApp::OnUpdateFileEditTimeFormats)
    ON_COMMAND(ID_FILE_EDIT_TIME_FORMATS, &CLogViewApp::OnFileEditTimeFormats)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogViewApp construction

/////////////////////////////////////////////////////////////////////////////
CLogViewApp::CLogViewApp()
{
	m_pDispOM = NULL;
	m_bDoReadUnicode = FALSE;
    mFormats.Add(_T("%d.%m.%Y, %H:%M:%S,%s"));
    mFormats.Add(_T("%d.%m.%Y, %H:%M:%S"));
    mFormats.Add(_T("[%d.%m.%Y %H:%M:%S.%s]"));
    mFormats.Add(_T("[%H:%M:%S.%s]"));
    mFormats.Add(_T("[%H:%M:%S]"));
    mFormats.Add(_T("[%m/%d/%Y %H:%M:%S.%s"));
    mFixedFormats = mFormats.GetCount();
    
}

/////////////////////////////////////////////////////////////////////////////
CLogViewApp::~CLogViewApp()
{
	WK_DELETE(m_pDispOM);
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CLogViewApp object

CLogViewApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CLogViewApp initialization

/////////////////////////////////////////////////////////////////////////////
void CLogViewApp::RegisterWindowClass()
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
	wndclass.lpszClassName = WK_APP_NAME_LOG_VIEW;

	// main app window
	AfxRegisterClass(&wndclass);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLogViewApp::InitInstance()
{
	// nur eine Instanz starten
	if ( WK_ALONE(WK_APP_NAME_LOG_VIEW)==FALSE )
	{
		HWND hWndFirstInstance = FindWindow(WK_APP_NAME_LOG_VIEW,NULL);
		if (hWndFirstInstance)
		{
			SetWindowPos(hWndFirstInstance,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
			SetWindowPos(hWndFirstInstance,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
			if (m_lpCmdLine[0] != 0)
			{
				SendMessage(hWndFirstInstance, WM_SETTEXT, 0, (LPARAM)m_lpCmdLine);
				PostMessage(hWndFirstInstance, WM_OPEN_FILE, 0, 0);
			}
		}
		return FALSE;
	}


	// Debugger öffnen
	InitDebugger(_T("LogView.log"),WAI_LOG_VIEW, NULL);

	WK_TRACE(_T("Starting LogView...\n"));

	m_pszAppName = _tcsdup(COemGuiApi::GetApplicationName(WAI_LOG_VIEW));

	RegisterWindowClass();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	// Create a custom control manager class so we can overide the site
	CCustomOccManager* pMgr = new CCustomOccManager;

	// Create an IDispatch class for extending the Dynamic HTML Object Model 
	m_pDispOM = new CImpIDispatch;

	// Set our control containment up but using our control container 
	// management class instead of MFC's default
	AfxEnableControlContainer(pMgr);


#if _MFC_VER >= 0x0710
 #ifndef _DTS
    {
	    CLoadResourceDll ResourceDll;
	    m_hLangResourceDLL = ResourceDll.DetachResourceDllHandle();
	    if (m_hLangResourceDLL == NULL)
	    {
		    WK_TRACE(_T("Did not find any ResourceDLL\n"));
		    ASSERT(FALSE);
		    return FALSE;
	    }
	    AfxSetResourceHandle(m_hLangResourceDLL);
    //  RKE: initialize codepage, if necessary (needs wkclasses)
	    CWK_String::SetCodePage(_ttoi(COemGuiApi::GetCodePage()));
        CWK_Profile wkp;
	    wkp.SetCodePage(CWK_String::GetCodePage());

	    DWORD dwCPBits = COemGuiApi::GetCodePageBits();
	    if (dwCPBits)
	    {
		    SetFontFaceNamesFromCodePageBits(dwCPBits);
    //		CWK_Profile prof;
    //		if (prof.GetInt(SECTION_LOG, _T("CanReadUnicode"), 1))
    //		{
    //			m_bDoReadUnicode = TRUE;
    //		}
	    }
    }
 #endif //_DTS
#else
 #ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
 #else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
 #endif // _AFXDLL
#endif //_MFC_VER == 0x0710

	SetRegistryKey(_T("LogView"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

    CWK_Profile wkp(CWK_Profile::WKP_ABSOLUT, GetAppRegistryKey(), _T(""), _T(""));

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_LOGVIEWTYPE,
		RUNTIME_CLASS(CLogViewDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CLogViewView));
//	pDocTemplate->SetContainerInfo(IDR_LOGVIEWTYPE);
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(
		IDR_HTMLVIEWTYPE,
		RUNTIME_CLASS(CHtmlViewDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CHtmlViewView));
//	pDocTemplate->SetContainerInfo(IDR_HTMLVIEWTYPE);
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(
		IDR_HTMLVIEWTYPE2,
		RUNTIME_CLASS(CHtmlViewDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CHtmlViewView));
//	pDocTemplate->SetContainerInfo(IDR_HTMLVIEWTYPE2);
	AddDocTemplate(pDocTemplate);

	CMainFrame* pMainFrame = new CMainFrame();
	SplitCommandLine(m_lpCmdLine, pMainFrame->m_saCmdParam, CMD_PARAM_USER);
	SplitCommandLine(m_lpCmdLine, pMainFrame->m_saCmdParam, CMD_PARAM_PASSWORD);
	// create main MDI Frame window
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		WK_TRACE_ERROR(_T("LoadFrame failed\n"));
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
	
	m_pMainWnd->DragAcceptFiles(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
    CStringArray fFormats;
    wkp.GetMultiString(_T("LogView"), _T("TimeFormats"), fFormats);
    mFormats.Append(fFormats);

	// Dispatch commands specified on the command line
	if (!cmdInfo.m_strFileName.IsEmpty())
	{
		((CMainFrame*)m_pMainWnd)->SetLogPath(cmdInfo.m_strFileName);
		if (!ProcessShellCommand(cmdInfo))
			return FALSE;
	}
	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	COemGuiApi::ToggleKeyboardInput(SELECT_USER_DEFINED);

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
int CLogViewApp::ExitInstance() 
{
    CWK_Profile wkp(CWK_Profile::WKP_ABSOLUT, GetAppRegistryKey(), _T(""), _T(""));
    
    CStringArray fFormats;
    for (int i=mFixedFormats; i<mFormats.GetCount(); i++)
    {
        fFormats.Add(mFormats[i]);
    }
    wkp.WriteMultiString(_T("LogView"), _T("TimeFormats"), fFormats);
	WK_TRACE(_T("ExitInstance\n"));
	// Debugger schließen
	CloseDebugger();
	
	return CWinApp::ExitInstance();
}

// App command to run the dialog
void CLogViewApp::OnAppAbout()
{
	COemGuiApi::AboutDialog(m_pMainWnd);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CLogViewApp::SplitCommandLine(const CString sCommandLine, CStringArray& saParameters, LPCTSTR szParam)
{
	CString sCL = _T(" ") + sCommandLine;
	int nFind1, nFind2;
	nFind1 = sCL.Find(szParam);
	if (nFind1 != -1)
	{
		nFind1 += _tcslen(szParam);
		nFind2 = sCL.Find(_T(" -"), nFind1);
		if (nFind2 != -1 && nFind2 >= nFind1)
		{
			saParameters.Add(sCL.Mid(nFind1, nFind2-nFind1));
		}
		else
		{
			saParameters.Add(sCL.Mid(nFind1));
		}
		return TRUE;
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLogViewApp::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYUP && pMsg->wParam == VK_F11 && COemGuiApi::GetKeyboardInput(0).IsValid())
	{
		COemGuiApi::ToggleKeyboardInput();
		return TRUE;
	}

	return CWinApp::PreTranslateMessage(pMsg);
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewApp::OnUpdateFileReadLogfileUnicode(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	pCmdUI->SetCheck(m_bDoReadUnicode);
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewApp::OnFileReadLogfileUnicode()
{
	m_bDoReadUnicode = !m_bDoReadUnicode;
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewApp::OnFileRuntimeErrors()
{
	COemFileDialog dlg(GetMainWnd());
	CString sOpen, sName, sExt, sOpenBtn;
	sName.LoadString(IDS_RTE);
	sExt = _T("dat");

	sOpen.LoadString(IDS_OPEN_RTE_FILE);
	sOpenBtn.LoadString(AFX_IDS_OPENFILE);
	dlg.SetProperties(TRUE,sOpen,sOpenBtn);
	dlg.SetInitialDirectory(((CMainFrame*)m_pMainWnd)->GetCurrentLogPath());
	dlg.AddFilter(sName, sExt); 
	if (dlg.DoModal() == IDOK)
	{
		CString sFileName = dlg.GetPathname();
		CWK_RunTimeErrors rte;
		if (   rte.LoadFromFile(sFileName) 
			&& rte.GetCount() > 0)
		{
			CString sTempLogFileName;
			TCHAR* pszTempName = _ttempnam(sFileName, _T("RTE"));
			int nLen = _MAX_PATH*2;
			sTempLogFileName		= pszTempName;
			int nBackSlash = sTempLogFileName.ReverseFind(_T('\\'));
			sTempLogFileName = sTempLogFileName.Left(nBackSlash);
			LPTSTR sBuffer = sTempLogFileName.GetBufferSetLength(nLen);
			WK_GetLongPathName(sBuffer, sBuffer, nLen);
			sTempLogFileName.ReleaseBuffer();
			sTempLogFileName += &pszTempName[nBackSlash];
			free(pszTempName);
			sTempLogFileName += TEMPORARY_LOGFILE;
			CStdioFileU fileRTE;
			if (rte.GetCodePage())
			{
				fileRTE.SetFileMCBS((WORD)rte.GetCodePage());
			}
			else
			{
				fileRTE.SetFileMCBS((WORD)CWK_String::GetCodePage());
			}
			if (fileRTE.Open(sTempLogFileName, CFile::modeCreate|CFile::modeWrite))
			{
				CString str;
				int i, n = rte.GetCount();
				CWK_RunTimeError *pRTE;
				SYSTEMTIME st;
				for (i=0; i<n; i++)
				{
					pRTE = rte.GetAtFast(i);
					st = pRTE->GetTimeStamp();

					str.Format(_T("%s:%s, %s, %s, (%x, %x)\r\n%s\r\n"), pRTE->GetFormattedTimeStamp(), 
					NameOfEnum(pRTE->GetApplicationId()),
					NameOfEnum(pRTE->GetError()),
					NameOfEnum(pRTE->GetSubcode()),
					pRTE->GetParam1(),
					pRTE->GetParam2(),
					pRTE->GetErrorText()
					);
					fileRTE.WriteString(str);
				}

				fileRTE.Close();
				theApp.OpenDocumentFile(sTempLogFileName);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CLogViewApp::OnUpdateFileRuntimeErrors(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}

void CLogViewApp::OnUpdateFileEditTimeFormats(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();
}

void CLogViewApp::OnFileEditTimeFormats()
{
    EditTimeFormatDlg dlg;
    dlg.DoModal();
}
