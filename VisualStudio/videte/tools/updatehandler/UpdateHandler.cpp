// UpdateHandler.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "UpdateHandler.h"

#include "MainFrm.h"
#include "UpdateDoc.h"
#include "DirectoryView.h"

#include "UpdateHandlerInput.h"
#include "SendDialog.h"

#include "DirDialog.h"
#include "SettingsDlg.h"

#include "SystemInfoDialog.h"
#include "CreateServiceDialog.h"

#include "SetupDialog.h"

#include "CmdLinParams.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static TCHAR szServiceInf [] = _T("a:\\service.inf");
static TCHAR szServiceDat [] = _T("a:\\service.dat");

#define GET_TEMPPATH _T("c:\\gettemp.txt")
/////////////////////////////////////////////////////////////////////////////
static UINT updates[] =
{
	ID_UPDATE_LAUNCHER,
    ID_UPDATE_SERVER,
    ID_UPDATE_DBS,
    ID_UPDATE_EXPLORER,
    ID_UPDATE_PH,
    ID_UPDATE_SV,
    ID_UPDATE_ISDN,
    ID_UPDATE_SIM,
    ID_UPDATE_SDIUNIT,
    ID_UPDATE_COMM,
    ID_UPDATE_AKU,
    ID_UPDATE_MICO,
    ID_UPDATE_COCO,
    ID_UPDATE_GAUNIT,
    ID_UPDATE_HELP
};

/////////////////////////////////////////////////////////////////////////////
// CCmdLine
CCmdLine::CCmdLine(CString sCmdLineFile, DWORD nWaitTime)
{
	m_sCmdLineFile = sCmdLineFile;
	m_nWaitTime = nWaitTime;
	m_CmdState = EXECUTE_CMD;
}

/////////////////////////////////////////////////////////////////////////////
// CCmdLineArray
CCmdLinePtr CCmdLineArray::GetCmdLine(const CString&sLine, CCmdLine::eCmdState eNextState)
{
	CCmdLinePtr pCmdLine;
	int i, nSize = GetSize();
	for (i=0; i<nSize; i++)
	{
		pCmdLine = GetAtFast(i);
		if (sLine.Find(pCmdLine->m_sCmdLineFile) != -1)
		{
			pCmdLine->m_CmdState = eNextState;
			if (eNextState == CCmdLine::FILE_DELETED)
			{
				RemoveAt(i);
				delete pCmdLine;
				pCmdLine = NULL;
			}
			return pCmdLine;
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CCmdLinePtr CCmdLineArray::GetCmdLine(DWORD nTickCount, CCmdLine::eCmdState eNextState)
{
	CCmdLinePtr pCmdLine;
	int i, nSize = GetSize();
	for (i=0; i<nSize; i++)
	{
		pCmdLine = GetAtFast(i);
		if (   pCmdLine->m_CmdState == CCmdLine::WAITING 
			&& nTickCount > pCmdLine->m_nWaitTime)
		{
			pCmdLine->m_CmdState = eNextState;
			return pCmdLine;
		}
		else if (   nTickCount == INFINITE
			     && nTickCount == pCmdLine->m_nWaitTime)
		{
			pCmdLine->m_CmdState = eNextState;
			return pCmdLine;
		}
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CUpdateHandler
BEGIN_MESSAGE_MAP(CUpdateHandler, CWinApp)
	//{{AFX_MSG_MAP(CUpdateHandler)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_VERBINDEN, OnVerbinden)
	ON_UPDATE_COMMAND_UI(ID_VERBINDEN, OnUpdateVerbinden)
	ON_COMMAND(ID_TRENNEN, OnTrennen)
	ON_UPDATE_COMMAND_UI(ID_TRENNEN, OnUpdateTrennen)
	ON_UPDATE_COMMAND_UI(ID_CONNECTION, OnUpdateConnection)
	ON_COMMAND(ID_UPDATE_LAUNCHER, OnUpdateLauncher)
	ON_COMMAND(ID_UPDATE_SOURCE, OnUpdateSource)
	ON_COMMAND(ID_UPDATE_AKU, OnUpdateAku)
	ON_COMMAND(ID_UPDATE_COMM, OnUpdateComm)
	ON_COMMAND(ID_UPDATE_DBS, OnUpdateDbs)
	ON_COMMAND(ID_UPDATE_EXPLORER, OnUpdateExplorer)
	ON_COMMAND(ID_UPDATE_GAUNIT, OnUpdateGaunit)
	ON_COMMAND(ID_UPDATE_ISDN, OnUpdateIsdn)
	ON_COMMAND(ID_UPDATE_PH, OnUpdatePh)
	ON_COMMAND(ID_UPDATE_SERVER, OnUpdateServer)
	ON_COMMAND(ID_UPDATE_SIM, OnUpdateSim)
	ON_COMMAND(ID_UPDATE_SV, OnUpdateSv)
	ON_COMMAND(ID_UPDATE_SDIUNIT, OnUpdateSdiUnit)
	ON_COMMAND(ID_UPDATE_TCPIP, OnUpdateTcpip)
	ON_UPDATE_COMMAND_UI(ID_UPDATE_AKU, OnUpdateUpdate)
	ON_COMMAND(ID_UPDATE_COCO, OnUpdateCoco)
	ON_COMMAND(ID_UPDATE_COMPLETE, OnUpdateComplete)
	ON_COMMAND(ID_UPDATE_FILE, OnUpdateFile)
	ON_COMMAND(ID_UPDATE_HELP, OnUpdateHelp)
	ON_COMMAND(ID_UPDATE_MICO, OnUpdateMico)
	ON_COMMAND(ID_OPTIONS_SETTINGS, OnOptionsSettings)
	ON_COMMAND(ID_FILE_SYSTEM_INFO, OnFileSystemInfo)
	ON_COMMAND(ID_FILE_REGEXPORT, OnFileRegexport)
	ON_COMMAND(ID_INFO_TIME, OnInfoTime)
	ON_COMMAND(ID_UPDATE_SECANA, OnUpdateSecana)
	ON_COMMAND(ID_UPDATE_REBOOT, OnUpdateReboot)
	ON_COMMAND(ID_UPDATE_ZIEL, OnUpdateZiel)
	ON_COMMAND(ID_UPDATE_DISK, OnUpdateDisk)
	ON_COMMAND(ID_CONNECT_LOCAL, OnConnectLocal)
	ON_COMMAND(ID_SETUP, OnSetup)
	ON_UPDATE_COMMAND_UI(ID_UPDATE_COCO, OnUpdateUpdate)
	ON_UPDATE_COMMAND_UI(ID_UPDATE_COMM, OnUpdateUpdate)
	ON_UPDATE_COMMAND_UI(ID_UPDATE_COMPLETE, OnUpdateUpdate)
	ON_UPDATE_COMMAND_UI(ID_UPDATE_DBS, OnUpdateUpdate)
	ON_UPDATE_COMMAND_UI(ID_UPDATE_EXPLORER, OnUpdateUpdate)
	ON_UPDATE_COMMAND_UI(ID_UPDATE_FILE, OnUpdateUpdate)
	ON_UPDATE_COMMAND_UI(ID_UPDATE_GAUNIT, OnUpdateUpdate)
	ON_UPDATE_COMMAND_UI(ID_UPDATE_HELP, OnUpdateUpdate)
	ON_UPDATE_COMMAND_UI(ID_UPDATE_ISDN, OnUpdateUpdate)
	ON_UPDATE_COMMAND_UI(ID_UPDATE_LAUNCHER, OnUpdateUpdate)
	ON_UPDATE_COMMAND_UI(ID_UPDATE_MICO, OnUpdateUpdate)
	ON_UPDATE_COMMAND_UI(ID_UPDATE_PH, OnUpdateUpdate)
	ON_UPDATE_COMMAND_UI(ID_UPDATE_SERVER, OnUpdateUpdate)
	ON_UPDATE_COMMAND_UI(ID_UPDATE_SIM, OnUpdateUpdate)
	ON_UPDATE_COMMAND_UI(ID_UPDATE_SV, OnUpdateUpdate)
	ON_UPDATE_COMMAND_UI(ID_UPDATE_SDIUNIT, OnUpdateUpdate)
	ON_UPDATE_COMMAND_UI(ID_UPDATE_TCPIP, OnUpdateUpdate)
	ON_UPDATE_COMMAND_UI(ID_FILE_SYSTEM_INFO, OnUpdateUpdate)
	ON_UPDATE_COMMAND_UI(ID_FILE_REGEXPORT, OnUpdateUpdate)
	ON_UPDATE_COMMAND_UI(ID_INFO_TIME, OnUpdateUpdate)
	ON_UPDATE_COMMAND_UI(ID_UPDATE_SECANA, OnUpdateUpdate)
	ON_UPDATE_COMMAND_UI(ID_UPDATE_REBOOT, OnUpdateUpdate)
	ON_UPDATE_COMMAND_UI(ID_SETUP, OnUpdateUpdate)
	ON_COMMAND(ID_CONNECTION_RESET, OnConnectionReset)
	ON_UPDATE_COMMAND_UI(ID_CONNECTION_RESET, OnUpdateConnectionReset)
	ON_COMMAND(ID_FILE_EXECUTE_CMD_LINE, OnFileExecuteCmdLine)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXECUTE_CMD_LINE, OnUpdateFileExecuteCmdLine)
	ON_COMMAND(ID_OPTION_PARTIAL_TRANSMISSION, OnOptionPartialTransmission)
	ON_UPDATE_COMMAND_UI(ID_OPTION_PARTIAL_TRANSMISSION, OnUpdateOptionPartialTransmission)
	ON_COMMAND(ID_FILE_FETCH_CMD_FILE, OnFileFetchCmdFile)
	ON_UPDATE_COMMAND_UI(ID_FILE_FETCH_CMD_FILE, OnUpdateFileFetchCmdFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CUpdateHandler::OnAppAbout()
{
	COemGuiApi::AboutDialog(AfxGetMainWnd());
}

/////////////////////////////////////////////////////////////////////////////
// CUpdateHandler construction

CUpdateHandler::CUpdateHandler()
{
	m_pUpdateHandlerInput = NULL;
	m_pUser = NULL;
	m_pPermission = NULL;
	m_pMainFrame = NULL;
	m_iCompleteUpdateIndex = 0;
	m_bBusy = FALSE;
	m_bComplete = FALSE;
	m_dwJobStartTime = 0;

	// settings
	m_bDisconnectAtJobEnd = FALSE;
	m_bOnlyExisting = FALSE;
	m_bPartialTransmission = TRUE;
	m_pSetupDialog = NULL;
}
CUpdateHandler::~CUpdateHandler()
{
	m_SentCmdLines.DeleteAll();
}
void CUpdateHandler::SetBusy()
{
	m_bBusy = TRUE;
}
void CUpdateHandler::ClearBusy()
{
	m_bBusy = FALSE;
}
void CUpdateHandler::Start()
{
	m_dwJobStartTime = GetTickCount();
}
int CUpdateHandler::Stop()
{
	int sek = (GetTickCount() - m_dwJobStartTime) / 1000;
	m_dwJobStartTime = 0;
	return sek;
}
/////////////////////////////////////////////////////////////////////////////
// The one and only CUpdateHandler object

CUpdateHandler theApp;

/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::RegisterWindowClass()
{
	WNDCLASS  wndclass;

	// register window class
	wndclass.style =         CS_DBLCLKS|CS_HREDRAW;
	wndclass.lpfnWndProc =   DefWindowProc;
	wndclass.cbClsExtra =    0;
	wndclass.cbWndExtra =    0;
	wndclass.hInstance =     m_hInstance;
	wndclass.hCursor =       LoadCursor(IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1) ;
	wndclass.lpszMenuName =  0L;
	wndclass.lpszClassName = WK_APP_NAME_UPDATE_HANDLER;

	wndclass.hIcon = LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME));

	// main app window
	AfxRegisterClass(&wndclass);
}
/////////////////////////////////////////////////////////////////////////////
// CUpdateHandler initialization

BOOL CUpdateHandler::InitInstance()
{
	if ( WK_ALONE(WK_APP_NAME_UPDATE_HANDLER)==FALSE ) return FALSE;

    if (!AfxOleInit ()) {
        AfxMessageBox (_T("AfxOleInit failed"));
        return FALSE;
    }
#if _MFC_VER < 0x0700
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif // _MFC_VER < 0x0700

	RegisterWindowClass();

	InitDebugger(_T("UpdateHandler.log"),WAI_UPDATE_HANDLER);

	m_Symbols.Create(IDB_SYMBOL,16,0,RGB(0,255,0));

	CSingleDocTemplate* pDocTemplate;

	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CUpdateDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CDirectoryView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	TRACE(_T("%08lx\n"),m_pMainWnd);

	if (!Login())
	{
		return FALSE;
	}

	// The main window has been initialized, so show and update it.
//	m_pMainFrame->ShowWindow(m_nCmdShow);
//	m_pMainFrame->UpdateWindow();

	CWK_Profile wkp;
	m_sHost.LoadString(ID_CONNECTION);

	m_bOnlyExisting = wkp.GetInt(_T("UpdateHandler"),_T("OnlyExisting"),FALSE);
	m_bDisconnectAtJobEnd = wkp.GetInt(_T("UpdateHandler"),_T("AutoDisconnect"),FALSE);
	m_sSourceDirectory = wkp.GetString(_T("UpdateHandler"),_T("Source"),_T(""));
	m_sDestination = wkp.GetString(_T("UpdateHandler"),_T("Dest"),_T(""));
	TCHAR szBuf[_MAX_PATH];
	GetCurrentDirectory(sizeof(szBuf), szBuf);

	if (m_sSourceDirectory.IsEmpty())
	{
		m_sSourceDirectory = szBuf;
	}
	if (m_sDestination.IsEmpty())
	{
		m_sDestination = szBuf;
	}

	m_pMainFrame = (CMainFrame*)m_pMainWnd;

	m_pMainWnd->PostMessage(WM_COMMAND,ID_FIRST);

	CString sLine;
	sLine.Format(_T("Quellverzeichnis %s"),m_sSourceDirectory);
	AddReportLine(sLine);
	sLine.Format(_T("Zielverzeichnis  %s"),m_sDestination);
	AddReportLine(sLine);

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
int CUpdateHandler::ExitInstance() 
{
	CWK_Profile wkp;

	wkp.WriteString(_T("UpdateHandler"),_T("Source"),m_sSourceDirectory);
	wkp.WriteString(_T("UpdateHandler"),_T("Dest"),m_sDestination);
	wkp.WriteInt(_T("UpdateHandler"),_T("OnlyExisting"),m_bOnlyExisting);
	wkp.WriteInt(_T("UpdateHandler"),_T("AutoDisconnect"),m_bDisconnectAtJobEnd);

	WK_DELETE(m_pPermission);
	WK_DELETE(m_pUser);
	WK_DELETE(m_pUpdateHandlerInput);

	CloseDebugger();

	return CWinApp::ExitInstance();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CUpdateHandler::Login()
{
	COEMLoginDialog dlg(AfxGetMainWnd());

	if (IDOK==dlg.DoModal())
	{
		if (m_pUser)
		{
			WK_TRACE_USER(_T("%s hat sich ausgeloggt.\n"),m_pUser->GetName());
		}
		WK_DELETE(m_pUser);
		m_pUser = new CUser(dlg.GetUser());
		WK_DELETE(m_pPermission);
		m_pPermission = new CPermission(dlg.GetPermission());
		if (m_pUser)
		{
			WK_TRACE_USER(_T("%s hat sich eingeloggt.\n"),m_pUser->GetName());
		}

		return (m_pUser && m_pUser->GetID()!=SECID_NO_ID) &&
			   (m_pPermission && m_pPermission->GetID()!=SECID_NO_ID);
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CUpdateHandler::Connect()
{
	CString sHost;
	CIPCFetchResult frInput;
	CString sLine;
	sLine.Format(_T("Verbindung zu %s herstellen"),m_sHost);
	AddReportLine(sLine);

	CString shmInputName;
	DWORD dwConnectTimeout=10000;	// default timeout

	sHost = m_sNumber;

	if (sHost==_T("0"))
	{
		sHost = LOCAL_LOOP_BACK;
	}

	ASSERT(m_pUser && m_pPermission);
	CConnectionRecord c(sHost, 
						*m_pUser, 
						m_pPermission->GetName(),
						m_pPermission->GetPassword(),
						dwConnectTimeout
						);
	
	CHostArray ha;
	CWK_Profile wkp;
	ha.Load(wkp);
	CHost*  pHost = ha.GetHost(m_idHost);
	if (   pHost
		&& pHost->IsPINRequired())
	{
		CString sPin;
		sPin = pHost->GetPIN();
		if (!sPin.IsEmpty())
		{
			c.SetFieldValue(CSD_PIN, sPin);
		}

	}
	CIPCFetch fetch;
	WK_TRACE(_T("connecting to %s\n"),sHost);

	frInput = fetch.FetchInput(c);
	shmInputName = frInput.GetShmName();
	if ( (frInput.GetError()==CIPC_ERROR_OKAY) && (shmInputName.GetLength()!=0))
	{
		m_pUpdateHandlerInput = new CUpdateHandlerInput(shmInputName,frInput.GetAssignedID()
#ifdef _UNICODE
			, frInput.GetCodePage()
#endif
			);
		m_pUpdateHandlerInput->SetHost(m_sHost);

		return TRUE;
	}
	else
	{
		sLine.Format(_T("keine Verbindung zu %s wegen %s"),sHost,frInput.GetErrorMessage());
		AddReportLine(sLine);
		return FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnConnectLocal() 
{
	// TODO: Add your command handler code here
	CWK_Profile wkp;
	CHostArray hosts;
	hosts.Load(wkp);
	m_sHost = hosts.GetLocalHostName();
	m_sNumber = LOCAL_LOOP_BACK;
	m_idHost = hosts.GetLocalHostID();

	Connect();
	
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnVerbinden() 
{
	CWK_Profile wkp;
	CHostArray hosts;
	hosts.Load(wkp);
	COEMConnectionDialog dlg(hosts,AfxGetMainWnd());

	dlg.m_sFilterTypes = _T("MINI B3,MINI B6,SMS");

	if (IDOK==dlg.DoModal())
	{
		m_sHost = dlg.m_sHost;
		m_sNumber = dlg.m_sTelefon;
		m_idHost = dlg.m_idHost;
		Connect();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnUpdateVerbinden(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pUpdateHandlerInput==NULL);
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnTrennen() 
{
	CString sLine;
	sLine.Format(_T("Verbindung zu %s trennen"),m_sHost);
	AddReportLine(sLine);
	WK_DELETE(m_pUpdateHandlerInput);
	ClearBusy();
	WK_TRACE(_T("disconnecting from %s\n"),m_sHost);
	sLine.Format(_T("Verbindung zu %s getrennt"),m_sHost);
	AddReportLine(sLine);
	m_sHost.LoadString(ID_CONNECTION);
	((CMainFrame*)m_pMainWnd)->ClearDirFileView();

	CString s;
	s.LoadString(ID_HOST);
	s += _T(":");
	((CMainFrame*)m_pMainWnd)->GetDialogBar()->GetDlgItem(IDC_HOST)->SetWindowText(s);
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnUpdateTrennen(CCmdUI* pCmdUI) 
{
	if (m_pUpdateHandlerInput)
		pCmdUI->Enable(m_pUpdateHandlerInput->GetIPCState()==CIPC_STATE_CONNECTED);
	else
		pCmdUI->Enable(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnInputConnect()
{
	CString sLine;
	sLine.Format(_T("Verbindung zu %s hergestellt"),m_sHost);

	CString s;
	s.LoadString(ID_HOST);
	s += _T(": ") +m_sHost;
	((CMainFrame*)m_pMainWnd)->GetDialogBar()->GetDlgItem(IDC_HOST)->SetWindowText(s);

	ClearBusy();
	AddReportLine(sLine);
	if (m_bComplete)
	{
		NextUpdate();
	}

	SendCommand(_T("GetLogicalDrives"));
	SendCommand(_T("GlobalMemoryStatus"));
	SendCommand(_T("GetSoftwareVersion"));
	SendCommand(_T("GetOSVersion"));
	SendCommand(_T("GetSystemInfo"));

	CString sFileCmd, sCmdLineFile;
	sCmdLineFile = GET_TEMPPATH;
	sFileCmd.Format(_T("cmd.exe /C \"set temp >> %s\""), sCmdLineFile);
	m_SentCmdLines.Add(new CCmdLine(sCmdLineFile, 1000));
	m_pUpdateHandlerInput->DoRequestGetFile(RFU_FULLPATH|RFU_EXECUTE, sFileCmd);

	sFileCmd.Format(_T("%s HKEY_LOCAL_MACHINE\\SOFTWARE\\DVRT"), SI_ENUMREGKEYS);
	m_pUpdateHandlerInput->DoRequestGetFile(RFU_STRING_INTERFACE, sFileCmd);
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnInputDisconnect()
{
	m_SentCmdLines.DeleteAll();
	WK_DELETE(m_pUpdateHandlerInput);
	CString sLine;
	sLine.Format(_T("Verbindung zu %s getrennt"),m_sHost);
	AddReportLine(sLine);
	((CMainFrame*)m_pMainWnd)->ClearDirFileView();
	CString s;
	s.LoadString(ID_HOST);
	s += _T(":");
	((CMainFrame*)m_pMainWnd)->GetDialogBar()->GetDlgItem(IDC_HOST)->SetWindowText(s);

	if (m_bComplete)
	{
		AddReportLine(_T("warte 10 Sekunden für Wiederverbindung"));
		((CMainFrame*)m_pMainWnd)->CountDown(10,ID_CONNECT);
	}
	else
	{
		m_sHost.LoadString(ID_CONNECTION);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnUpdateConnection(CCmdUI* pCmdUI) 
{
	pCmdUI->SetText(m_sHost);
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::AddReportLine(UINT nID)
{
	CString s;
    s.LoadString(nID);
	AddReportLine(s);
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::AddReportLine(const CString& sLine)
{
	if (m_pMainWnd)
	{
		((CMainFrame*)m_pMainWnd)->AddReportLine(sLine);
		m_pMainWnd->UpdateWindow();
		WK_TRACE(_T("%s\n"),sLine);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnUpdateLauncher() 
{
	SetBusy();
	m_sCurrentJob = _T("Launcher updaten");
	UpdateExecutable(_T("SecurityLauncher.exe"));
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::UpdateExecutable(const CString& sExe) 
{
	if (m_pUpdateHandlerInput==NULL)
	{
		return;
	}

	Start();

	CString sPath;
	CFile file;
	CFileException cfe;
	CFileStatus cfs;
	LPVOID pBuffer;
	CString mText,rString;

	sPath = m_sSourceDirectory + _T('\\') + sExe;
	CString sLine;
	sLine.Format(_T("Executable %s updaten"),sExe);
	AddReportLine(sLine);

	sLine.Format(_T("lese %s"),sPath);
	AddReportLine(sLine);
	if (file.Open(sPath,CFile::modeRead|CFile::shareDenyNone,&cfe))
	{
		if (file.GetStatus(cfs))
		{
			if (cfs.m_size > UINT_MAX)
			{
				WK_TRACE(_T("UpdateExecutable() filesize too big\n"));
				return;
			}
			LONG filesize = (LONG)cfs.m_size;
			pBuffer = malloc(filesize);
			DWORD dwBitrate = m_pUpdateHandlerInput->GetBitrate();
			if (dwBitrate)
			{
				int sek = (filesize)/(dwBitrate/8);
				sLine.Format(_T("vermutete Zeitdauer %d Sekunden"),sek);
				AddReportLine(sLine);
				((CMainFrame*)m_pMainWnd)->CountDown(sek);
			}

			if (filesize==(LONG)file.Read(pBuffer,filesize))
			{
				m_pUpdateHandlerInput->DoRequestFileUpdate(RFU_APPDIR,sExe,pBuffer,filesize,FALSE);
			}
			else
			{
				rString.LoadString(IDS_NO_READFILE);
				mText.Format(rString,sPath);
				AfxMessageBox(mText);
			}
			free(pBuffer);
		}
	}
	else
	{
		rString.LoadString(IDS_NO_OPENFILE);
		mText.Format(rString,sPath);
		AfxMessageBox(mText);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::MultipleUpdate(const CStringArray& sFiles, BOOL bToDisk /*= FALSE*/)
{
	if ((m_pUpdateHandlerInput==NULL) && !bToDisk)
	{
		return;
	}

	int i,c;
	CString sPath,sName;
	CFile fCompress,fSource,fBubble;
	CFileException cfe;
	CFileStatus cfs;
	LPVOID pBuffer;
	CString sCommand,sLine,sSize,sRel;
	DWORD dwSize,dwLargest;
	TCHAR szBuf[_MAX_PATH];
	CString sSys,sDest;
	int p;
	CStringArray sSources;

	SetBusy();
	m_pMainWnd->UpdateWindow();

	GetSystemDirectory(szBuf, sizeof(szBuf));
	sSys = szBuf;
	sSys.MakeLower();

	TCHAR szTempPathName[_MAX_PATH];
	TCHAR szTempFileName[_MAX_PATH];
	CString sZip;

	GetTempPath(_MAX_PATH,szTempPathName);
	GetTempFileName(szTempPathName, _T("uhz"), 0, szTempFileName);
	sZip = szTempFileName;
	p = sZip.ReverseFind(_T('.'));
	sZip = sZip.Left(p) + _T(".uhz");

	sCommand = _T("MultipleUpdate ");
	// first calc the size for buffer
	dwSize = 0;
	dwLargest = 0;
	c = sFiles.GetSize();

	for (i=0;i<c;i++)
	{
		p = sFiles[i].Find(_T('@'));
		if (p!=-1)
		{
			sRel = sFiles[i].Left(p);
			sDest = sFiles[i].Mid(p+1);
		}
		else
		{
			sRel = sFiles[i];
			sDest = _T("p");
		}
		if (sRel[1]==_T(':'))
		{
			sPath = sRel;
		}
		else
		{
			sPath = m_sSourceDirectory + _T('\\') + sRel;
		}
		sName = sPath.Mid(sPath.ReverseFind(_T('\\'))+1);

		if (CFile::GetStatus(sPath,cfs))
		{
			if (cfs.m_size > UINT_MAX)
			{
				WK_TRACE(_T("MultipleUpdate() filesize too big\n"));
				return;
			}
			LONG filesize = (LONG)cfs.m_size;
			sLine.Format(_T("%s = %d Bytes"),sPath,filesize);
			AddReportLine(sLine);
			dwSize += filesize;
			if (filesize>(LONG)dwLargest)
			{
				dwLargest = filesize;
			}

			sSize.Format(_T("%d"),filesize);
			sCommand += sName + _T(',') + sSize + _T(',') + sDest + _T(';');
			sSources.Add(sPath);
		}
		else
		{
			sPath = sSys + _T('\\') + sName;
			if (CFile::GetStatus(sPath,cfs))
			{
				if (cfs.m_size > UINT_MAX)
				{
					WK_TRACE(_T("MultipleUpdate() filesize too big\n"));
					return;
				}
				LONG filesize = (LONG)cfs.m_size;
				sLine.Format(_T("%s = %d Bytes"),sPath,filesize);
				AddReportLine(sLine);
				dwSize += filesize;
				if (filesize>(LONG)dwLargest)
				{
					dwLargest = filesize;
				}

				sSize.Format(_T("%d"),filesize);
				sCommand += sName + _T(',') + sSize + _T(',') + sDest + _T(';');
				sSources.Add(sPath);
			}
			else
			{
				// one file not found error
				sLine.Format(_T("Datei nicht gefunden %s"),sPath);
				AddReportLine(sLine);
				ClearBusy();
				return;
			}
		}
	}
	sLine.Format(_T("gesamt %d Bytes lese und komprimiere Dateien"),dwSize);
	AddReportLine(sLine);
	WK_TRACE(sCommand);
	pBuffer = malloc(dwLargest);

	// copy all source files to szTempFileName
	c = sSources.GetSize();
	for (i=0;i<c;i++)
	{
		sPath = sSources[i];
		if (fSource.Open(sPath,CFile::modeRead|CFile::shareDenyNone,&cfe))
		{
			if (fSource.GetStatus(cfs))
			{
				if (cfs.m_size > UINT_MAX)
				{
					WK_TRACE(_T("MultipleUpdate() filesize too big\n"));
					return;
				}
				LONG filesize = (LONG)cfs.m_size;
				if (filesize==(LONG)fSource.Read(pBuffer,filesize))
				{
					if (fBubble.Open(szTempFileName,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite))
					{
						fBubble.SeekToEnd();
						fBubble.Write(pBuffer,filesize);
						fBubble.Flush();
						fBubble.Close();
					}
				}
				else
				{
					free(pBuffer);
					return;
				}
			}
			else
			{
				free(pBuffer);
				return;
			}
			fSource.Close();
		}
	}
	free(pBuffer);
	pBuffer = NULL;

	WK_CompressFile(szTempFileName, sZip);

	TRY
	{ CFile::Remove(szTempFileName);}
	CATCH(CFileException, e)
	{}
	END_CATCH

	if (!bToDisk)
	{
		if (fCompress.Open(sZip,CFile::modeRead))
		{
			if (fCompress.GetStatus(cfs))
			{
				if (cfs.m_size > UINT_MAX)
				{
					WK_TRACE(_T("MultipleUpdate() filesize too big\n"));
					return;
				}
				DWORD filesize = (DWORD)cfs.m_size;
				sLine.Format(_T("Dateien gelesen und komprimiert %d Bytes"),filesize);
				AddReportLine(sLine);
				pBuffer = malloc(filesize);
				if (filesize==fCompress.Read(pBuffer,filesize))
				{
					DWORD dwBitRate = m_pUpdateHandlerInput->GetBitrate();
					if (dwBitRate)
					{
						int sek = (dwSize)/(m_pUpdateHandlerInput->GetBitrate()/8) + 5;
						sLine.Format(_T("vermutete Zeitdauer %d Sekunden"),sek);
						AddReportLine(sLine);
						((CMainFrame*)m_pMainWnd)->CountDown(sek,ID_TIMEOUT);
					}
					Start();
					m_pUpdateHandlerInput->DoRequestFileUpdate(RFU_STRING_INTERFACE,sCommand,pBuffer,dwSize,FALSE);
				}
				free(pBuffer);
				fCompress.Close();

				TRY
				{
					CFile::Remove(sZip);
				}
				CATCH(CFileException, e)
				{
				}
				END_CATCH

			}
		}
	}
	else
	{
		// to disk
		WritePrivateProfileString(_T("service"),_T("command"), sCommand, szServiceInf);
		AddReportLine(_T("komprimierte Datei kopieren"));
		if (CopyFile(sZip, szServiceDat, FALSE))
		{
			AddReportLine(_T("komprimierte Datei kopiert"));
		}
		else
		{
			AddReportLine(GetLastErrorString());
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnConfirmFileUpdate(const CString& sName)
{
	CString sLine;
	sLine.Format(_T("%s ausgeführt in %d Sekunden"),m_sCurrentJob,Stop());
	AddReportLine(sLine);
	m_sCurrentJob.Empty();
	AddReportLine(m_sCurrentJob);
	ClearBusy();
	((CMainFrame*)m_pMainWnd)->CountDown(0);

	if (m_bComplete)
	{
		if ((updates[m_iCompleteUpdateIndex] != ID_UPDATE_SERVER) &&
		   (updates[m_iCompleteUpdateIndex] != ID_UPDATE_ISDN))
		{
			NextUpdate();
		}
	}

	if (WK_IS_WINDOW(m_pSetupDialog))
	{
		m_pSetupDialog->OnConfirmFile(sName);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnTimeOut()
{
	if (m_bBusy)
	{
		AddReportLine(_T("time out erreicht"));
		ClearBusy();
		if (m_bComplete && m_bDisconnectAtJobEnd)
		{
			OnTrennen();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnDrives()
{
	if (m_pUpdateHandlerInput)
	{
		((CMainFrame*)m_pMainWnd)->AddDrives(m_pUpdateHandlerInput->m_DriveInfo);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnTimeArrived()
{
	if (m_pUpdateHandlerInput)
	{
		CString s = _T("Auf ") + m_sHost + _T(" ist es ") + m_pUpdateHandlerInput->m_sCurrentTime;
		AddReportLine(s);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::NextUpdate()
{
	m_iCompleteUpdateIndex++;
	if (m_iCompleteUpdateIndex < (sizeof(updates) / sizeof(UINT)))
	{
		m_pMainWnd->PostMessage(WM_COMMAND,updates[m_iCompleteUpdateIndex]);
	}
	else
	{
		AddReportLine(_T("Komplett Update ausgeführt"));
		m_bComplete = FALSE;
		ClearBusy();
		if (m_bDisconnectAtJobEnd)
		{
			OnTrennen();
		}
	}
}

static TCHAR BASED_CODE szFilter[] = _T("Invisible (*.qqq)|*.qqq||");
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnUpdateSource() 
{
	CString sLine;
	AddReportLine(_T("Quellverzeichnis wählen"));

	CDirDialog dlg(TRUE,NULL,_T("trick.dat"),OFN_HIDEREADONLY|OFN_EXPLORER,szFilter);
	dlg.m_ofn.lpstrInitialDir = m_sSourceDirectory.GetBuffer(0);
	dlg.m_sDir = m_sSourceDirectory;
	int res = dlg.DoModal();
	if (res==IDOK)
	{
		m_sSourceDirectory.ReleaseBuffer(0);
		CString sTemp = dlg.GetPathName();
		int p = sTemp.ReverseFind(_T('\\'));
		m_sSourceDirectory = sTemp.Left(p);
	}
	else
	{
	}
	sLine.Format(_T("Quellverzeichnis ist %s"),m_sSourceDirectory);
	AddReportLine(sLine);
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnUpdateAku() 
{
	if (m_pUpdateHandlerInput)
	{
		if (-1==m_pUpdateHandlerInput->m_SoftwareVersion.Find(_T("GARNY")))
		{
			CStringArray saFiles;
			saFiles.Add(_T("akuunit.exe@p"));
			m_sCurrentJob = _T("AKUUnit updaten");
			AddReportLine(m_sCurrentJob);
			MultipleUpdate(saFiles);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnUpdateComm() 
{
	if (m_pUpdateHandlerInput)
	{
		if (-1==m_pUpdateHandlerInput->m_SoftwareVersion.Find(_T("GARNY")))
		{
			CStringArray saFiles;
			saFiles.Add(_T("communit.exe@p"));
			m_sCurrentJob = _T("CommUnit updaten");
			AddReportLine(m_sCurrentJob);
			MultipleUpdate(saFiles);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnUpdateDbs() 
{
	CStringArray saFiles;
	saFiles.Add(_T("databaseserver.exe@p"));
	MultipleUpdate(saFiles);
	m_sCurrentJob = _T("DataBaseServer updaten");
	AddReportLine(m_sCurrentJob);
//	UpdateExecutable(_T("DataBaseServer.exe"));
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnUpdateExplorer() 
{
	CStringArray saFiles;
	saFiles.Add(_T("vision.exe@p"));
	m_sCurrentJob = _T("SecurityExplorer updaten");
	AddReportLine(m_sCurrentJob);
	MultipleUpdate(saFiles);
//	UpdateExecutable(_T("SecurityExplorer.exe"));
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnUpdateGaunit() 
{
	if (m_pUpdateHandlerInput)
	{
		if (-1!=m_pUpdateHandlerInput->m_SoftwareVersion.Find(_T("GARNY")))
		{
			CStringArray saFiles;
			saFiles.Add(_T("gaunit.exe@p"));
			m_sCurrentJob = _T("GAUnit updaten");
			AddReportLine(m_sCurrentJob);
			MultipleUpdate(saFiles);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnUpdateIsdn() 
{
	CStringArray saFiles;
	saFiles.Add(_T("isdnunit.exe@p"));
	m_sCurrentJob = _T("ISDNUnit updaten");
	AddReportLine(m_sCurrentJob);
	MultipleUpdate(saFiles);
//	UpdateExecutable(_T("ISDNUnit.exe"));
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnUpdatePh() 
{
	CStringArray saFiles;
	saFiles.Add(_T("recherche.exe@p"));
	m_sCurrentJob = _T("PictureHandler updaten");
	AddReportLine(m_sCurrentJob);
	MultipleUpdate(saFiles);
//	UpdateExecutable(_T("DataBaseClient.exe"));
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnUpdateServer() 
{
	CStringArray saFiles;
	saFiles.Add(_T("sec3.exe@p"));
	m_sCurrentJob = _T("Server updaten");
	AddReportLine(m_sCurrentJob);
	MultipleUpdate(saFiles);
//	UpdateExecutable(_T("Sec3.exe"));
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnUpdateSim() 
{
	CStringArray saSimFiles;
	saSimFiles.Add(_T("simunit.exe@p"));
	m_sCurrentJob = _T("SIMUnit updaten");
	AddReportLine(m_sCurrentJob);
	MultipleUpdate(saSimFiles);
//	UpdateExecutable(_T("SimUnit.exe"));
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnUpdateSv() 
{
	CStringArray saFiles;
	saFiles.Add(_T("supervisor_sec3.exe@p"));
	m_sCurrentJob = _T("SystemVerwaltung updaten");
	AddReportLine(m_sCurrentJob);
	MultipleUpdate(saFiles);
//	UpdateExecutable(_T("Supervisor_Sec3.exe"));
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnUpdateSdiUnit() 
{
	CStringArray saFiles;
	saFiles.Add(_T("sdiunit.exe@p"));
	m_sCurrentJob = _T("SDIUnit updaten");
	AddReportLine(m_sCurrentJob);
	MultipleUpdate(saFiles);
//	UpdateExecutable(_T("sdiunit.exe"));
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnUpdateTcpip() 
{
	CStringArray saFiles;
	saFiles.Add(_T("socketunit.exe@p"));
	m_sCurrentJob = _T("SocketUnit updaten");
	AddReportLine(m_sCurrentJob);
	MultipleUpdate(saFiles);
//	UpdateExecutable(_T("SocketUnit.exe"));
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnUpdateUpdate(CCmdUI* pCmdUI) 
{
	if (m_pUpdateHandlerInput)
	{
		if (m_bBusy)
		{
			pCmdUI->Enable(FALSE);
		}
		else
		{
			pCmdUI->Enable(m_pUpdateHandlerInput->GetIPCState()==CIPC_STATE_CONNECTED);
		}
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnUpdateCoco() 
{
	CStringArray saCocoFiles;

	saCocoFiles.Add(_T("coco\\cocounit.exe@c"));
	saCocoFiles.Add(_T("coco\\cocosrv.exe@c"));
	saCocoFiles.Add(_T("coco\\avp3api.dll@c"));
	saCocoFiles.Add(_T("coco\\avpalmpi.dll@c"));
	saCocoFiles.Add(_T("coco\\avpaudpi.dll@c"));
	saCocoFiles.Add(_T("coco\\avpcpi.dll@c"));
	saCocoFiles.Add(_T("coco\\avpfrs.dll@c"));
	saCocoFiles.Add(_T("coco\\avphdar.dll@c"));
	saCocoFiles.Add(_T("coco\\avpscpi.dll@c"));
	saCocoFiles.Add(_T("coco\\avpvidpi.dll@c"));
	saCocoFiles.Add(_T("coco\\mapper31.dll@c"));
	saCocoFiles.Add(_T("coco\\pcidebug.dll@c"));
	saCocoFiles.Add(_T("coco\\coco263.mcs@c"));
	saCocoFiles.Add(_T("coco\\vconf263.drs@c"));

	saCocoFiles.Add(_T("coco\\ccodec32.dll@s"));
	saCocoFiles.Add(_T("coco\\convrt32.dll@s"));
	saCocoFiles.Add(_T("coco\\megrav2.dll@s"));
	saCocoFiles.Add(_T("coco\\h221vxd.386@s"));
	saCocoFiles.Add(_T("coco\\hdarvxd.386@s"));

	m_sCurrentJob = _T("CocoUnit updaten");
	AddReportLine(m_sCurrentJob);
	MultipleUpdate(saCocoFiles);
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnUpdateComplete() 
{
	m_iCompleteUpdateIndex = 0;

	// first make sure connection is established
	m_bComplete = TRUE;
	m_sCurrentJob = _T("Komplett updaten");
	AddReportLine(m_sCurrentJob);
	OnUpdateLauncher();
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnUpdateFile() 
{
	CSendDialog dlg(m_pMainWnd);

	if (IDOK==dlg.DoModal())
	{
		CFile file;
		CFileException cfe;
		CFileStatus cfs;
		CString sSource = dlg.m_Source;
		LPVOID pBuffer;
		CString mText,rString;

		if (file.Open(sSource,CFile::modeRead|CFile::shareDenyNone,&cfe))
		{
			if (file.GetStatus(cfs))
			{
				if (cfs.m_size > UINT_MAX)
				{
					WK_TRACE(_T("OnUpdateFile() filesize too big\n"));
					return;
				}
				LONG filesize = (LONG)cfs.m_size;
				pBuffer = malloc(filesize);
				if (filesize==(LONG)file.Read(pBuffer,filesize))
				{
					WK_TRACE(_T("sending %d,%s copying to %s\n"),dlg.m_iType,sSource,dlg.m_DestinationFileName);
					m_pUpdateHandlerInput->DoRequestFileUpdate(dlg.m_iType,dlg.m_DestinationFileName,pBuffer,filesize,dlg.m_bReboot);
				}
				else
				{
					rString.LoadString(IDS_NO_READFILE);
					mText.Format(rString,sSource);
					AfxMessageBox(mText);
				}
				free(pBuffer);
			}
		}
		else
		{
			rString.LoadString(IDS_NO_OPENFILE);
			mText.Format(rString,sSource);
			AfxMessageBox(mText);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnUpdateHelp() 
{
	CStringArray saFiles;
	saFiles.Add(_T("cocounit.hlp@p"));
	saFiles.Add(_T("databaseclient.hlp@p"));
	saFiles.Add(_T("databaseserver.hlp@p"));
	saFiles.Add(_T("glossar.hlp@p"));
	saFiles.Add(_T("secanalyzer.hlp@p"));
	saFiles.Add(_T("securityexplorer.hlp@p"));
	saFiles.Add(_T("servpack_d.hlp@p"));
	saFiles.Add(_T("spez.hlp@p"));
	saFiles.Add(_T("supervisor_sec3.hlp@p"));
	m_sCurrentJob = _T("Hilfe updaten");
	AddReportLine(m_sCurrentJob);
	MultipleUpdate(saFiles);
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnUpdateMico() 
{
	CStringArray saMicoFiles;

	saMicoFiles.Add(_T("mico\\micounit.exe@m"));
	saMicoFiles.Add(_T("mico\\mico.vxd@s"));
	saMicoFiles.Add(_T("mico\\megrav2.dll@s"));
	saMicoFiles.Add(_T("mico\\convrt32.dll@s"));

	m_sCurrentJob = _T("MicoUnit updaten");
	AddReportLine(m_sCurrentJob);
	MultipleUpdate(saMicoFiles);
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnOptionsSettings() 
{
	CSettingsDlg dlg;

	dlg.m_Disconnect = m_bDisconnectAtJobEnd;
	dlg.m_OnlyExisting = m_bOnlyExisting;

	if (IDOK==dlg.DoModal())
	{
		m_bDisconnectAtJobEnd = dlg.m_Disconnect;
		m_bOnlyExisting = dlg.m_OnlyExisting;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnFileSystemInfo() 
{
	if (m_pUpdateHandlerInput)
	{
		CSystemInfoDialog dlg;

		dlg.m_SystemInfo = m_pUpdateHandlerInput->m_SystemInfo;
		dlg.m_OSVersionInfo = m_pUpdateHandlerInput->m_OSVersionInfo;
		dlg.m_MemoryStatus = m_pUpdateHandlerInput->m_MemoryStatus;
		dlg.m_sSoftwareVersion = m_pUpdateHandlerInput->m_SoftwareVersion;
		dlg.DoModal();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnFileRegexport() 
{
	SendCommand(_T("ExportRegistry"));
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnInfoTime() 
{
	SendCommand(_T("GetCurrentTime"));
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::GetDirectory(const CString& sDir)
{
	if (m_pUpdateHandlerInput)
	{
		m_sCurrentJob = _T("Lese Verzeichnis ") + sDir;
		AddReportLine(m_sCurrentJob);
		int iDest = RFU_FULLPATH;
		if (m_bPartialTransmission)
		{
			iDest |= RFU_DIVIDE_IN_BITS;
		}
		if (sDir.Find(REG_PATH_ID) != -1)
		{
			CString sRequest = sDir.Mid(4, sDir.GetLength()-5);
			CString s = SI_ENUMREGKEYS;
			m_pUpdateHandlerInput->DoRequestGetFile(RFU_STRING_INTERFACE, s + _T(" ") + sRequest);
			s = SI_ENUMREGVALUES;
			m_pUpdateHandlerInput->DoRequestGetFile(RFU_STRING_INTERFACE, s + _T(" ") + sRequest);
		}
		else
		{
			m_pUpdateHandlerInput->DoRequestGetFile(iDest, sDir);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::DeleteFile(const CString& sFile)
{
	if (m_pUpdateHandlerInput)
	{
		m_sCurrentJob = _T("Lösche Datei: ") + sFile;
		AddReportLine(m_sCurrentJob);
		Start();
		m_pUpdateHandlerInput->DoRequestGetFile(RFU_FULLPATH|RFU_DELETE,sFile);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::GetFile(const CString& sFile, DWORD dwSize /*= 0*/)
{
	if (m_pUpdateHandlerInput)
	{
		m_sCurrentJob = _T("Empfange Datei: ") + sFile;
		AddReportLine(m_sCurrentJob);

		if (dwSize)
		{
			CString sLine;
			DWORD dwBitRate = m_pUpdateHandlerInput->GetBitrate();
			if (dwBitRate)
			{
				int sek = (dwSize)/(dwBitRate/8);
				sLine.Format(_T("vermutete Zeitdauer %d Sekunden"),sek);
				AddReportLine(sLine);
				((CMainFrame*)m_pMainWnd)->CountDown(sek,ID_TIMEOUT);
			}
		}
		Start();
		m_pUpdateHandlerInput->DoRequestGetFile(RFU_FULLPATH,sFile);
	}
}
/////////////////////////////////////////////////////////////////////////////
int CUpdateHandler::SendCommand(const CString& sCommand)
{
	CString s;
	if (m_pUpdateHandlerInput)
	{
		s = sCommand;
		if (-1 == s.Find(_T(' ')))
		{
			s += _T(" nop");
		}
		m_pUpdateHandlerInput->DoRequestGetFile(RFU_STRING_INTERFACE,s);
		AddReportLine(sCommand);
	}
	return s.GetLength();
}
/////////////////////////////////////////////////////////////////////////////
int CUpdateHandler::SendUpdateCommand(const CString& sCommand, BOOL bReboot)
{
	CString s;
	if (m_pUpdateHandlerInput)
	{
		s = sCommand;
		if (-1 == s.Find(_T(' ')))
		{
			s += _T(" nop");
		}
		m_pUpdateHandlerInput->DoRequestFileUpdate(RFU_STRING_INTERFACE,s,NULL,0,bReboot);
		AddReportLine(sCommand);
	}
	return s.GetLength();
}
/////////////////////////////////////////////////////////////////////////////
int CUpdateHandler::SendFile(const CString& sSource, const CString& sDest)
{
	int iRet = 0;
	if (m_pUpdateHandlerInput)
	{
		
		CFile file;
		CFileException cfe;
		CFileStatus cfs;
		LPVOID pBuffer;
		CString mText,rString;
		CString sDestFile;
		int p;

		p = sSource.ReverseFind(_T('\\'));
		sDestFile = sDest + sSource.Mid(p+1);

		m_sCurrentJob = _T("Sende Datei: ") + sSource + _T(" nach ") + sDestFile;
		AddReportLine(m_sCurrentJob);
		Start();
		if (file.Open(sSource,CFile::modeRead|CFile::shareDenyNone,&cfe))
		{
			if (file.GetStatus(cfs))
			{
				if (cfs.m_size > UINT_MAX)
				{
					WK_TRACE(_T("SendFile() filesize too big\n"));
					return iRet;
				}
				LONG filesize = (LONG)cfs.m_size;
				pBuffer = malloc(filesize);

				CString sLine;
				int byterate = m_pUpdateHandlerInput->GetBitrate()/8;
				if (byterate==0)
				{
					byterate = 1024*1024;
				}
				int sek = (filesize)/byterate;
				sLine.Format(_T("vermutete Zeitdauer %d Sekunden"),sek);
				AddReportLine(sLine);

				if (filesize==(LONG)file.Read(pBuffer,filesize))
				{
					m_pUpdateHandlerInput->DoRequestFileUpdate(RFU_FULLPATH,
						sDestFile,pBuffer,filesize,FALSE);
					((CMainFrame*)m_pMainWnd)->CountDown(sek,ID_TIMEOUT);
					iRet = sek;
				}
				else
				{
					rString.LoadString(IDS_NO_READFILE);
					mText.Format(rString,sSource);
					AddReportLine(mText);
				}
				free(pBuffer);
			}
		}
		else
		{
			rString.LoadString(IDS_NO_OPENFILE);
			mText.Format(rString,sSource);
			AddReportLine(mText);
		}
	}

	return iRet;
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::ExecuteFile(const CString& sFile)
{
	if (m_pUpdateHandlerInput)
	{
		CCmdLineParamDlg dlg;
		if (dlg.DoModal() == IDOK)
		{
			CString sFileCmd = sFile;
			if (!dlg.m_sCmdLine.IsEmpty())
			{
				sFileCmd += dlg.m_sCmdLine;
			}
			m_sCurrentJob = _T("Führe Datei aus: ") + sFileCmd;
			AddReportLine(m_sCurrentJob);
			Start();
			m_pUpdateHandlerInput->DoRequestGetFile(RFU_FULLPATH|RFU_EXECUTE,sFileCmd);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnError()
{
	if (m_pUpdateHandlerInput)
	{
		CString sLine = _T("Fehler: ") + m_pUpdateHandlerInput->m_sLastError;
		AddReportLine(sLine);
		CCmdLinePtr pCmd = m_SentCmdLines.GetCmdLine(m_pUpdateHandlerInput->m_sLastError, CCmdLine::REQUEST_FILE);
		if (pCmd)
		{
			if (pCmd->m_nWaitTime == INFINITE)
			{
				pCmd->m_CmdState = CCmdLine::DELETE_FILE;
				m_pUpdateHandlerInput->DoRequestGetFile(RFU_FULLPATH|RFU_DELETE, pCmd->m_sCmdLineFile);
			}
			else
			{
				pCmd->m_nWaitTime = INFINITE;
			}
		}
	}
	if (WK_IS_WINDOW(m_pSetupDialog))
	{
		m_pSetupDialog->OnError(m_pUpdateHandlerInput->m_sLastError);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnFileDeleted()
{
	if (m_pUpdateHandlerInput)
	{
		int nOldSize = m_SentCmdLines.GetSize();
		m_SentCmdLines.GetCmdLine(m_pUpdateHandlerInput->m_sLastDeleted, CCmdLine::FILE_DELETED);
		int nSize = m_SentCmdLines.GetSize();
		if (nSize == 0)
		{
			m_pMainFrame->SetIdleTimer(FALSE);
		}
		if (nOldSize > nSize)
		{
			return;
		}
		CString sLine = _T("Datei gelöscht: ") + m_pUpdateHandlerInput->m_sLastDeleted;
		AddReportLine(sLine);
		m_pUpdateHandlerInput->RemoveFileFromDirContent(m_pUpdateHandlerInput->m_sLastDeleted);
		((CMainFrame*)m_pMainWnd)->RemoveFileFromFileView(m_pUpdateHandlerInput->m_sLastDeleted);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnFileExecuted()
{
	if (m_pUpdateHandlerInput)
	{
		CCmdLinePtr pCmd = m_SentCmdLines.GetCmdLine(m_pUpdateHandlerInput->m_sLastExecuted, CCmdLine::WAITING);
		if (pCmd)
		{
			if (pCmd->m_nWaitTime != -1)
			{
				pCmd->m_nWaitTime += GetTickCount();
			}
			return;
		}
		CString sLine = _T("Datei ausgeführt: ") + m_pUpdateHandlerInput->m_sLastExecuted;
		AddReportLine(sLine);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnFileGot()
{
	if (m_pUpdateHandlerInput)
	{
		CCmdLinePtr pCmd = m_SentCmdLines.GetCmdLine(m_pUpdateHandlerInput->m_sLastGot, CCmdLine::GOT_FILE);
		if (pCmd)
		{
			BOOL bTempPath = (pCmd->m_sCmdLineFile == GET_TEMPPATH) ? TRUE : FALSE;
			CStdioFileU file;
			if (file.Open(m_pUpdateHandlerInput->m_sLastWrittenTo, CFile::modeRead))
			{
				CString sLine;

				while (file.ReadString(sLine))
				{
					if (!sLine.IsEmpty())
					{
						if (bTempPath)
						{
							int nFind = sLine.Find(_T("TEMP="));
							if (nFind != -1)
							{
								m_pUpdateHandlerInput->m_sRemoteTemp = sLine.Mid(nFind+5);
								int nFind = m_pUpdateHandlerInput->m_sRemoteTemp.GetLength();
								if (nFind)
								{
									if (m_pUpdateHandlerInput->m_sRemoteTemp.GetAt(nFind-1) != _T('\\'))
									{
										m_pUpdateHandlerInput->m_sRemoteTemp += _T("\\");
									}
								}
								break;
							}
						}
						else
						{
							AddReportLine(sLine);
						}
					}
				}
				pCmd->m_CmdState = CCmdLine::DELETE_FILE;
				m_pUpdateHandlerInput->DoRequestGetFile(RFU_FULLPATH|RFU_DELETE, pCmd->m_sCmdLineFile);
				return;
			}
			file.Close();
			file.Remove(m_pUpdateHandlerInput->m_sLastWrittenTo);
		}
		CString sLine = _T("Datei empfangen: ") + 
					    m_pUpdateHandlerInput->m_sLastGot + 
						_T(" --> ") + 
					    m_pUpdateHandlerInput->m_sLastWrittenTo; 
		AddReportLine(sLine);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnUpdateSecana() 
{
	CStringArray saFiles;
	saFiles.Add(_T("secanalyzer.exe@p"));
	m_sCurrentJob = _T("SecAnalyzer updaten");
	AddReportLine(m_sCurrentJob);
	MultipleUpdate(saFiles);
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnUpdateReboot() 
{
	// TODO: Add your command handler code here
	if (m_pUpdateHandlerInput)
	{
		m_pUpdateHandlerInput->DoRequestFileUpdate(RFU_APPDIR,_T("reboot"),NULL,0,TRUE);
	}
	
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnUpdateZiel() 
{
	CString sLine;
	AddReportLine(_T("Zielverzeichnis wählen"));

	CDirDialog dlg(TRUE,NULL,_T("trick.dat"),OFN_HIDEREADONLY,szFilter);
	dlg.m_ofn.lpstrInitialDir = m_sDestination.GetBuffer(0);
	dlg.m_sDir = m_sDestination;
	int res = dlg.DoModal();
	if (res==IDOK)
	{
		m_sDestination.ReleaseBuffer(0);
		CString sTemp = dlg.GetPathName();
		int p = sTemp.ReverseFind(_T('\\'));
		m_sDestination = sTemp.Left(p);
	}
	else
	{
	}
	sLine.Format(_T("Zielverzeichnis ist %s"),m_sDestination);
	AddReportLine(sLine);
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnUpdateDisk() 
{
	CString sInfo;
//	UINT	nNr = 0;
	CFileStatus cfs;
	LONG    lSize;


	if (CFile::GetStatus(szServiceInf,cfs))
	{
		if (cfs.m_size > UINT_MAX)
		{
			WK_TRACE(_T("OnUpdateDisk() filesize too big\n"));
			return;
		}
		lSize = (LONG)cfs.m_size;

		::GetPrivateProfileString(_T("service"),_T("info"),_T(""),sInfo.GetBuffer(lSize),lSize,szServiceInf);
		sInfo.ReleaseBuffer();
	}

	// Service Diskette erstellen
	CCreateServiceDialog dlg(m_pMainWnd);

	dlg.m_InitialDir = m_sSourceDirectory;
	dlg.m_sTitel = sInfo;
	if (IDOK==dlg.DoModal())
	{
		AddReportLine(_T("Service Diskette erstellen"));

		WritePrivateProfileString(_T("service"),_T("info"),dlg.m_sTitel,szServiceInf);
		WritePrivateProfileString(_T("service"),_T("version"),dlg.m_sVersion,szServiceInf);
		CString sNr;
		sNr.Format(_T("%d"),1997*34);
		WritePrivateProfileString(_T("service"),_T("nr"),sNr,szServiceInf);
		// update
		CStringArray m_FormatFiles;
		CString sOne,sName;
		int i,c;


		c = dlg.m_Files.GetSize();
		if (c==0)
		{
			AddReportLine(_T("keine Service Diskette ohne Datei"));
			return;
		}
		for (i=0;i<c;i++)
		{
			sOne = dlg.m_Files[i];
			sOne.MakeLower();
			sName = sOne;//.Mid(sOne.ReverseFind(_T('\\'))+1);
			if ((-1!=sOne.Find(_T("\\coco"))) && (dlg.m_iKind==0))
			{
				sName += _T("@c");
			}
			else if ((-1!=sOne.Find(_T("\\mico"))) && (dlg.m_iKind==0))
			{
				sName += _T("@m");
			}
			else if ((-1!=sOne.Find(_T(".vxd"))) ||
					 (-1!=sOne.Find(_T(".dll"))) ||
					 (-1!=sOne.Find(_T(".386"))) )
			{
				sName += _T("@s");
			}
			else
			{
				if (dlg.m_iKind==0)
				{
					sName += _T("@p");
				}
				else
				{
					sName += _T("@t");
				}
			}
			m_FormatFiles.Add(sName);
		}

		MultipleUpdate(m_FormatFiles,TRUE);
		
		if (dlg.m_iKind==1)
		{
			// execute
			CString sExec;
			sExec = m_FormatFiles[0].Mid(m_FormatFiles[0].ReverseFind(_T('\\'))+1);
			WritePrivateProfileString(_T("service"),_T("execute"),sExec,szServiceInf);
		}

		AddReportLine(_T("Service Diskette erstellt"));
	}
	
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnSetup() 
{
	// TODO: Add your command handler code here
	CString sSetup;

	sSetup = m_sSourceDirectory + _T("\\setup.exe");

	if (DoesFileExist(sSetup))
	{
		AddReportLine(_T("benutze ") + sSetup);

		CSetupDialog dlg;
		m_pSetupDialog = &dlg;
		dlg.DoModal();
		m_pSetupDialog = NULL;
	}
	else
	{
		AddReportLine(_T("Datei ") + sSetup + _T(" nicht gefunden"));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnConnectionReset() 
{
	if (   m_pUpdateHandlerInput 
		&& m_pUpdateHandlerInput->GetIPCState() == CIPC_STATE_CONNECTED)
	{
		CFileDialog dlg(TRUE);
		
		if (IDOK == dlg.DoModal())
		{
			CFile file;

			if (file.Open(dlg.GetPathName(),CFile::modeRead))
			{
				ULONGLONG ullLen = file.GetLength();
				if (ullLen > UINT_MAX)
				{
					WK_TRACE(_T("OnConnectionReset() filesize too big\n"));
					return;
				}
				DWORD dwLen = (DWORD)ullLen;
				BYTE* pBuffer = new BYTE[dwLen];

				if (dwLen==file.Read(pBuffer,dwLen))
				{
					m_pUpdateHandlerInput->DoRequestFileUpdate(RFU_STRING_INTERFACE,
															   _T("ImportRegistry"),
															   pBuffer,
															   dwLen,FALSE);
				}

				file.Close();

				WK_DELETE_ARRAY(pBuffer);
			}
		}
	}
	
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnUpdateConnectionReset(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnFileExecuteCmdLine()
{
	CCmdLineParamDlg dlg;
	dlg.m_sHeadline.LoadString(IDS_EXECUTE_CMD_LINE);
	if (    dlg.DoModal() == IDOK 
		&& !dlg.m_sCmdLine.IsEmpty())
	{

		m_sCurrentJob = _T("Führe Kommando aus: ") + dlg.m_sCmdLine;
		CString sFileCmd, sCmdLineFile;
		GetTempFileName(_T("."), _T("cmd"), 0, sCmdLineFile.GetBufferSetLength(MAX_PATH));
		sCmdLineFile.ReleaseBuffer();
		sCmdLineFile = m_pUpdateHandlerInput->m_sRemoteTemp + sCmdLineFile.Mid(1);
		sFileCmd.Format(_T("cmd.exe /C \"%s >> %s\""), dlg.m_sCmdLine, sCmdLineFile);
		m_SentCmdLines.Add(new CCmdLine(sCmdLineFile, dlg.m_nWaitTime));
		m_pMainFrame->SetIdleTimer(TRUE);
		AddReportLine(m_sCurrentJob);
		Start();
		m_pUpdateHandlerInput->DoRequestGetFile(RFU_FULLPATH|RFU_EXECUTE, sFileCmd);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnUpdateFileExecuteCmdLine(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pUpdateHandlerInput != NULL);
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnOptionPartialTransmission()
{
	m_bPartialTransmission = !m_bPartialTransmission;
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnUpdateOptionPartialTransmission(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
	pCmdUI->SetCheck(m_bPartialTransmission);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CUpdateHandler::OnIdle(LONG lCount)
{
	CCmdLinePtr pCmd = m_SentCmdLines.GetCmdLine(GetTickCount(), CCmdLine::REQUEST_FILE);
	if (pCmd)
	{
		m_pUpdateHandlerInput->DoRequestGetFile(RFU_FULLPATH, pCmd->m_sCmdLineFile);
	}
	return CWinApp::OnIdle(lCount);
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnFileFetchCmdFile()
{
	CCmdLinePtr pCmd = m_SentCmdLines.GetCmdLine(INFINITE, CCmdLine::REQUEST_FILE);
	if (pCmd)
	{
		m_pUpdateHandlerInput->DoRequestGetFile(RFU_FULLPATH, pCmd->m_sCmdLineFile);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CUpdateHandler::OnUpdateFileFetchCmdFile(CCmdUI *pCmdUI)
{
	CCmdLinePtr pCmd = m_SentCmdLines.GetCmdLine(INFINITE, CCmdLine::REQUEST_FILE);
	pCmdUI->Enable(pCmd != NULL);
}
